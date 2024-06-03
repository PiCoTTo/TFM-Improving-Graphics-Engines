#include "DebugPass.h"
#include "renderer/Renderer.h"
#include "imgui.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "misc/cpp/imgui_stdlib.h"
#include "input/Input.h"
#include "core/ExportedVariablesManager.h"
#include <typeindex>
#include <sstream>
#include <algorithm>
#include "imgui_internal.h"
#include <string>


namespace nimo
{
    DebugPass::DebugPass(std::shared_ptr<SceneRenderer> renderer) :
        RenderPass(renderer),
        m_shadersEditorViewEnabled(true)
    {
        // Analyze dump file
        std::vector<std::string> lines(10, ",");
        int headerNumber{ 0 };
        std::ifstream csvFile("PerformanceDump.csv");
        if (!csvFile.fail())
        {
            lines.clear();

            std::string line;
            std::getline(csvFile, line, '\n');
            while (!csvFile.eof())
            {
                lines.push_back(line);
                std::getline(csvFile, line);
            }
            
            if (lines.size() != 0)
            {
                std::stringstream ss(lines[0]);
                std::string header, lastHeader;
                int numHeders{ 0 };
                while (std::getline(ss, header, ','))
                {
                    if (!header.empty())
                        lastHeader = header;
                }

                if (lastHeader.find("#") != std::string::npos)
                {
                    auto headerNumberStr = lastHeader.substr(lastHeader.find("#") + 1, lastHeader.size() - lastHeader.find("#"));
                    headerNumber = std::stoi(headerNumberStr);
                    m_headerName = lastHeader.substr(0, lastHeader.find("#") + 1) + std::to_string(headerNumber + 1);
                }
            }

            csvFile.close();
        }
    }

    void DebugPass::update(float deltaTime)
    {
        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::D))
            m_statsViewEnabled = !m_statsViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::V))
            m_exportedVariablesViewEnabled = !m_exportedVariablesViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::S))
            m_shadersEditorViewEnabled = !m_shadersEditorViewEnabled;

        m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, ScriptComponent, CameraComponent>().each([&](ActiveComponent& active, ScriptComponent& script, CameraComponent& camera)
        {
            active.active = !m_shadersEditorViewEnabled;
        });

        if(m_shadersEditorViewEnabled && nimo::Input::GetKey(nimo::KeyCode::LeftControl) && nimo::Input::GetKeyPressed(nimo::KeyCode::D8))
            applyRequested = true;

        if (m_shadersEditorViewEnabled && nimo::Input::GetKey(nimo::KeyCode::LeftControl) && nimo::Input::GetKeyPressed(nimo::KeyCode::R))
            revertRequested = true;

        if (m_resetDisplayedStats)
        {
            m_displayedStats = m_emptyDisplayedStats;
            m_samples.clear();
            m_frameTimeSamplesSum = 0;
            m_resetDisplayedStats = false;
        }

        m_currentTime += deltaTime;
        m_timeDebugRefresh += deltaTime;
        if (m_timeDebugRefresh > 1.f / m_refreshRate)
        {
            //m_displayedStats.frameTime = m_gameViewPanel->getFrameTime();
            m_displayedStats.frameTime = m_renderer->currentFrameTime();
            // Wait 2 seconds of time to reach stable performance
            if (m_currentTime > 2)
            {
                m_displayedStats.maximumFrameTime = std::max(m_displayedStats.maximumFrameTime, m_displayedStats.frameTime);
                m_displayedStats.minimumFrameTime = std::min(m_displayedStats.minimumFrameTime, m_displayedStats.frameTime);
            }
            m_displayedStats.renderFrameTime = m_renderer->m_renderFrameTimer.ElapsedMillis();
            m_displayedStats.geometryFrameTime = m_renderer->m_geometryFrameTimer.ElapsedMillis();
            m_displayedStats.lightingFrameTime = m_renderer->m_lightingFrameTimer.ElapsedMillis();
            m_displayedStats.bloomFrameTime = m_renderer->m_bloomFrameTimer.ElapsedMillis();
            m_displayedStats.geometry2DFrameTime = m_renderer->m_geometry2DFrameTimer.ElapsedMillis();

            // Record the metrics
            m_samples.push_back({
                m_currentTime,
                m_displayedStats.frameTime,
                m_displayedStats.frameTime > 0 ? 1000.f / m_displayedStats.frameTime : 0,
                m_displayedStats.renderFrameTime
                });

            m_frameTimeSamplesSum += m_displayedStats.frameTime;

            m_displayedStats.averageFrameTime = m_frameTimeSamplesSum / m_samples.size();

            // Dump the range of records to CSV
            if (m_recordingSamples)
            {
                if (m_currentTime - m_samples[m_firstSample2DumpIdx].m_time >= m_recordingTime)
                {
                    m_lastSample2DumpIdx = m_samples.size() - 1;

                    // Analyze dump file
                    std::vector<std::string> lines(10);
                    std::string lastHeader;
                    int headerNumber{ 0 };
                    std::fstream csvFile("PerformanceDump.csv", std::ios::in);
                    if (!csvFile.fail())
                    {
                        lines.clear();
                        
                        std::string line;
                        std::getline(csvFile, line);
                        while (!csvFile.eof() && line != "")
                        {
                            lines.push_back(line + ',');
                            std::getline(csvFile, line);
                        }

                        if (lines.size() > 1)
                        {
                            std::stringstream ss(lines[0]);
                            std::string header;
                            int numHeders{ 0 };
                            while (std::getline(ss, header, ','))
                            {
                                if (!header.empty())
                                    lastHeader = header;
                            }
                        }
                        else
                            lines.insert(lines.begin(), 2 + (m_lastSample2DumpIdx - m_firstSample2DumpIdx), "");

                        // Get the header numbering if any
                        int delimIdx = m_headerName.find("#");
                        if (delimIdx != std::string::npos)
                        {
                            lastHeader = m_headerName.substr(0, delimIdx);
                            headerNumber = std::atoi(m_headerName.substr(delimIdx + 1, m_headerName.size() - 1 - delimIdx).c_str()) + 1;
                        }
                        else // No header number delimiter found
                            lastHeader = m_headerName;

                        csvFile.close();
                    }

                    // Dump the samples
                    csvFile.open("PerformanceDump.csv", std::ios::out);
                    csvFile << lines[0] + m_headerName + ",\n";
                    csvFile << lines[1] + "FRAME TIME,FPS\n";

                    for (unsigned int i = 0; i < std::min(lines.size() - 2, static_cast<size_t>(m_lastSample2DumpIdx - m_firstSample2DumpIdx)); ++i)
                        csvFile << lines[i + 2] << m_samples[m_firstSample2DumpIdx + i].m_lastFrameTime << ',' << 1000.f / m_samples[m_firstSample2DumpIdx + i].m_lastFrameTime << std::endl;

                    m_headerName = lastHeader + '#' + std::to_string(headerNumber);

                    csvFile.close();

                    m_recordingSamples = false;
                }
            }

            m_timeDebugRefresh = 0;
        }
    }


    void DebugPass::render(std::shared_ptr<FrameBuffer> target, const CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
    {
        if (!m_renderer->mustRender())
            return;

        // Stats GUI
        //if (!m_statsViewEnabled) return;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (m_statsViewEnabled)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

            ImGui::Begin("Stats", &m_statsViewEnabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Text("Render stats");
            ImGui::TextDisabled("Draw calls: %d", nimo::Renderer::stats.totalDrawCalls);
            ImGui::TextDisabled("Triangle count: %d", nimo::Renderer::stats.totalTriangles);
            ImGui::Text("FPS: %.2f", m_displayedStats.frameTime > 0 ? 1000.f / m_displayedStats.frameTime : 0);
            ImGui::Text("Max FPS: %.2f", m_displayedStats.minimumFrameTime > 0 ? 1000.f / m_displayedStats.minimumFrameTime : 0);
            ImGui::Text("Avg FPS: %.2f", m_displayedStats.averageFrameTime > 0 ? 1000.f / m_displayedStats.averageFrameTime : 0);
            ImGui::Text("Min FPS: %.2f", m_displayedStats.maximumFrameTime > 0 ? 1000.f / m_displayedStats.maximumFrameTime : 0);
            ImGui::Text("Last frame time: %.3f ms", m_displayedStats.frameTime);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Sampling Recording");
            //ImGui::SameLine();
            //std::string dataTitle = "data";
            ImGui::InputText("Data Title", &m_headerName);
            ImGui::InputInt("Rec Time", &m_recordingTime);

            ImGui::BeginDisabled(m_recordingSamples);
            if (ImGui::Button("Reset"))
                m_resetDisplayedStats = true;
            ImGui::EndDisabled();

            if (m_recordingSamples)
                ImGui::Text("Recording");
            std::string recButtonText = m_recordingSamples ? "Cancel" : "Record";
            if (ImGui::Button(recButtonText.c_str()))
            {
                m_recordingSamples = !m_recordingSamples;
                m_firstSample2DumpIdx = m_samples.size() - 1;
            }

            ImGui::End();
        }

        if (m_exportedVariablesViewEnabled)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

            ImGui::Begin("Variables", &m_exportedVariablesViewEnabled, ImGuiWindowFlags_NoCollapse);

            ImGui::Button("\tRestore\t");

            bool anythingChanged{ false };
            for (const auto& exportedVariablePair : ExportedVariablesManager::instance()->variables())
            {
                auto exportedVariable = exportedVariablePair.second;
                if (exportedVariable->m_type == std::type_index(typeid(bool)))
                {
                    anythingChanged |= ImGui::Checkbox(exportedVariable->m_name.c_str(), static_cast<bool*>(exportedVariable->m_value));
                }

                if (exportedVariable->m_type == std::type_index(typeid(unsigned int)))
                {
                    ImGui::PushItemWidth(40.f);
                    anythingChanged |= ImGui::DragInt(exportedVariable->m_name.c_str(), static_cast<int*>(exportedVariable->m_value));
                }

                if (exportedVariable->m_type == std::type_index(typeid(glm::vec3)))
                {
                    auto vector = static_cast<glm::vec3*>(exportedVariable->m_value);
                    ImGui::PushItemWidth(200.f);
                    anythingChanged |= ImGui::DragFloat3(exportedVariable->m_name.c_str(), &vector->x, 0.05f);
                }
            }
            if (anythingChanged)
                m_renderer->updateFromChangedVariables();

            //TODO Add more types
            //if (typeid(i) == typeid(int))
            //{
            //    //ImGui::Text("Vertex");
            //    //ImGui::SameLine();
            //    ImGui::InputInt("##i", &i);
            //}
            //if (typeid(f) == typeid(float))
            //{
            //    //ImGui::Text("Vertex Shader");
            //    //ImGui::SameLine();
            //    ImGui::InputFloat("##f", &f);
            //}
            //if (typeid(d) == typeid(double))
            //{
            //    //ImGui::Text("Vertex Shader Code");
            //    //ImGui::SameLine();
            //    ImGui::InputDouble("##d", &d);
            //}
            //ImGui::NextColumn();
            //if (typeid(i) == typeid(int))
            //{
            //    //ImGui::Text("Vertex");
            //    //ImGui::SameLine();
            //    ImGui::InputInt("##i", &i);
            //}
            //if (typeid(f) == typeid(float))
            //{
            //    //ImGui::Text("Vertex Shader");
            //    //ImGui::SameLine();
            //    ImGui::InputFloat("##f", &f);
            //}
            //if (typeid(d) == typeid(double))
            //{
            //    //ImGui::Text("Vertex Shader Code");
            //    //ImGui::SameLine();
            //    ImGui::InputDouble("##d", &d);
            //}


            ImGui::End();
        }

        if (m_shadersEditorViewEnabled)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

            ImGui::Begin("Shaders", &m_shadersEditorViewEnabled, ImGuiWindowFlags_NoCollapse);

            ImGui::Text("Shaders in use");

            ImGui::PushItemWidth(-1);
            bool resListBox = ImGui::BeginListBox("##draw_list");

            auto shaders = AssetManager::GetAllLoaded<Shader>();

            std::string file, selectedFile = "";
            for (const auto& shader : shaders)
            {
                auto metadata = AssetManager::GetMetadata(shader);

                file = metadata.filepath.filename().string();
                ImGui::Selectable(file.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    selectedFile = file;
                    if (m_openShaders.find(selectedFile) == m_openShaders.end())
                    {
                        auto shaderEntry = std::make_unique<ShaderEntry>();
                        shaderEntry->id = metadata.id;
                        shaderEntry->m_shader = shader;
                        shaderEntry->vertexSourceBak = shader->GetVertexCode();
                        shaderEntry->fragmentSourceBak = shader->GetFragmentCode();
                        m_openShaders[selectedFile] = std::move(shaderEntry);
                    }
                }
            }

            if (resListBox)
                ImGui::EndListBox();

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            applyRequested |= ImGui::Button("\tApply Changes (Ctrl+8)\t");
            ImGui::SameLine();
            if (ImGui::Button("\tRevert Changes (Ctrl+R)\t"))
                revertRequested = true;

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::BeginTabBar("OpenedShaders");

            int i = 0;
            auto closedShader = m_openShaders.end();
            for (auto& [shaderFile, shader] : m_openShaders)
            {
                ImGuiTabItemFlags tabFlags = shader->dirty ? ImGuiTabItemFlags_UnsavedDocument : ImGuiTabItemFlags_None;
                bool open;
                bool res = ImGui::BeginTabItem((shaderFile).c_str(), &open, (shaderFile == selectedFile ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None) | tabFlags);
                if (!open)
                    closedShader = m_openShaders.find(shaderFile);

                if (res)
                {
                    ImGui::BeginChild("Code");

                    //ImGui::PushItemWidth(-1);
                    float sizeY{ 400.f };
                    bool changed{ false };

                    changed = renderShaderEditPanel(std::string("Vertex"), shader->m_shader->GetVertexCodePtr(), shader->vertexScrollY, 0, sizeY);
                    changed |= renderShaderEditPanel(std::string("Fragment"), shader->m_shader->GetFragmentCodePtr(), shader->fragmentScrollY, 0, sizeY);

                    if (revertRequested)
                    {
                        shader->dirty = shader->vertexSourceBak.compare(shader->m_shader->GetVertexCode()) | shader->fragmentSourceBak.compare(shader->m_shader->GetFragmentCode());
                        shader->m_shader->GetVertexCode() = shader->vertexSourceBak;
                        shader->m_shader->GetFragmentCode() = shader->fragmentSourceBak;
                        //shader->dirty = false;
                        revertRequested = false;
                    }

                    if (changed)
                        shader->dirty = !shader->vertexSourceBak.compare(shader->m_shader->GetVertexCode()) | !shader->fragmentSourceBak.compare(shader->m_shader->GetFragmentCode());

                    if (applyRequested && shader->dirty)
                    {
                        shader->m_shader->Recompile();
                        shader->dirty = false;
                        applyRequested = false;
                    }

                    ImGui::EndChild();
                }

                if (res)
                    ImGui::EndTabItem();
            }
            if (closedShader != m_openShaders.end())
                m_openShaders.erase(closedShader);

            ImGui::EndTabBar();

            ImGui::End();
        }

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    bool DebugPass::renderShaderEditPanel(std::string& label, std::string* code, float& scrollY, float width, float height)
    {
        ImGui::Text(label.c_str());
        std::ostringstream ss;

        std::string::difference_type count = std::count(code->begin(), code->end(), '\n');
        for (int i = 1; i <= count + 1; ++i)
            ss << i << "\n";

        ImGui::InputTextMultiline((std::string("##") + label).c_str(), &ss.str(), ImVec2(40.f, height), ImGuiInputTextFlags_ReadOnly);
        ImGuiContext* context = ImGui::GetCurrentContext();
        char childWindowName[300];
        ImFormatString(childWindowName, 300, "%s/%s_%08X", context->CurrentWindow->Name, (std::string("##") + label).c_str(), ImGui::GetID((std::string("##") + label).c_str()));
        ImGuiWindow* childWindow = ImGui::FindWindowByName(childWindowName);
        ImGui::SetScrollY(childWindow, scrollY);

        ImGui::SameLine();
        bool changed = ImGui::InputTextMultiline(label.c_str(), code, ImVec2(ImGui::GetContentRegionAvailWidth(), height), ImGuiInputTextFlags_AllowTabInput);
        ImFormatString(childWindowName, 300, "%s/%s_%08X", context->CurrentWindow->Name, label.c_str(), ImGui::GetID(label.c_str()));
        childWindow = ImGui::FindWindowByName(childWindowName);
        scrollY = childWindow->Scroll.y;

        return changed;
    }


    // https://github.com/libigl/libigl/issues/1300#issuecomment-1310174619
    std::string DebugPass::labelPrefix(const char* const label)
    {
        float width = ImGui::CalcItemWidth();

        float x = ImGui::GetCursorPosX();
        ImGui::Text(label);
        ImGui::SameLine();
        ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SetNextItemWidth(-1);

        std::string labelID = "##";
        labelID += label;

        return labelID;
    }


    const ImGuiTreeNodeFlags DebugPass::TREENODE_BASE_FLAGS = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding;
    const ImGuiTabBarFlags TAB_BAR_BASE_FLAGS = ImGuiTabBarFlags_Reorderable;
}
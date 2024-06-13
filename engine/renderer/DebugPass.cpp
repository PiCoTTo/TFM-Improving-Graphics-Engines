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
#include "core/Application.h"
#include "glad/glad.h"
#include "scene/Components.h"


namespace nimo
{
    DebugPass::DebugPass(std::shared_ptr<SceneRenderer> renderer) :
        RenderPass(renderer)
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
            int numLines = 1;
            while (!csvFile.eof())
            {
                numLines++;
                lines.push_back(line);
                std::getline(csvFile, line);
            }
            m_recordingTime = numLines - 2;
            
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

        nimo::ExportedVariablesManager::instance()->addVariable("DEBUG_DRAW_OBB", m_obbDrawEnabled);
    }

    void DebugPass::update(float deltaTime)
    {
        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::D))
            m_statsViewEnabled = !m_statsViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::V))
            m_exportedVariablesViewEnabled = !m_exportedVariablesViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::S))
            m_shadersEditorViewEnabled = !m_shadersEditorViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::F))
            m_fbosViewEnabled = !m_fbosViewEnabled;

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
        // Introduces a configurable sampling period both for data recording and for the user to clearly read values
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
            m_displayedStats.renderFrameTime        = m_renderer->m_renderFrameTimer.ElapsedMillis();
            m_displayedStats.geometryFrameTime      = m_renderer->m_geometryFrameTimer.ElapsedMillis();
            m_displayedStats.lightingFrameTime      = m_renderer->m_lightingFrameTimer.ElapsedMillis();
            m_displayedStats.bloomFrameTime         = m_renderer->m_bloomFrameTimer.ElapsedMillis();
            m_displayedStats.geometry2DFrameTime    = m_renderer->m_geometry2DFrameTimer.ElapsedMillis();

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

                        csvFile.close();
                    }

                    // Get the header numbering if any
                    int delimIdx = m_headerName.find("#");
                    if (delimIdx != std::string::npos)
                    {
                        lastHeader = m_headerName.substr(0, delimIdx);
                        headerNumber = std::atoi(m_headerName.substr(delimIdx + 1, m_headerName.size() - 1 - delimIdx).c_str()) + 1;
                    }
                    else // No header number delimiter found
                        lastHeader = m_headerName;

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


    void DebugPass::render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
    {
        if (!m_renderer->mustRender())
            return;

        if (m_obbDrawEnabled)
        {
            float width = target ? target->GetDetails().width : Application::Instance().GetWindow().GetWidth();
            float height = target ? target->GetDetails().height : Application::Instance().GetWindow().GetHeight();
            glViewport(0, 0, target ? target->GetDetails().width : Application::Instance().GetWindow().GetWidth(), target ? target->GetDetails().height : Application::Instance().GetWindow().GetHeight());

            auto camTransform = cameraTransform;
            auto cam = cameraSettings;
            glm::mat4 projection = glm::perspectiveFov(glm::radians(cam.FOV),
                target ? (float)target->GetDetails().width : (float)Application::Instance().GetWindow().GetWidth(),
                target ? (float)target->GetDetails().height : (float)Application::Instance().GetWindow().GetHeight(),
                cam.ClippingPlanes.Near, cam.ClippingPlanes.Far);
            glm::mat4 projectionOrtho = glm::ortho(
                -(target ? (float)target->GetDetails().width : (float)Application::Instance().GetWindow().GetWidth()) * 0.5f,
                (target ? (float)target->GetDetails().width : (float)Application::Instance().GetWindow().GetWidth()) * 0.5f,
                -(target ? (float)target->GetDetails().height : (float)Application::Instance().GetWindow().GetHeight()) * 0.5f,
                (target ? (float)target->GetDetails().height : (float)Application::Instance().GetWindow().GetHeight()) * 0.5f,
                -0.1f, cam.ClippingPlanes.Far);
            glm::mat4 viewMatrix = camTransform.GetView();
            auto viewPosition = glm::vec3(camTransform.Translation.x, camTransform.Translation.y, camTransform.Translation.z);

            FrameBuffer::Unbind();
            m_renderer->m_shaderUnlitColor->use();
            m_renderer->m_shaderUnlitColor->Set("view", viewMatrix);
            m_renderer->m_shaderUnlitColor->Set("projection", projection);
            m_renderer->m_shaderUnlitColor->Set("color", glm::vec3(0.f, 1.f, 0.f));
            unsigned int entitiesDrawn = 0;
            m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent, TransformComponent>().each(
                [&](ActiveComponent& active, IDComponent& id, MeshComponent& m, TransformComponent& t)
            {
                if (entitiesDrawn >= m_renderer->m_renderEntitiesLimit) return;
                if (!active.active) return;
                if (!m.source || !m.inFrustum) return;

                m_renderer->m_shaderUnlitColor->Set("transform", m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)));

                auto obb = m.source->getOOB();
                renderOBB(obb);

                entitiesDrawn++;
            });
        }

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
            renderExportedVariablesView(cameraSettings, cameraTransform);
        }

        if (m_shadersEditorViewEnabled)
        {
            renderShaderEditView();
        }

        if (m_fbosViewEnabled)
        {
            renderFbosView();
        }

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    void DebugPass::renderExportedVariablesView(CameraComponent& cameraSettings, const TransformComponent& cameraTransform)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("Variables", &m_exportedVariablesViewEnabled, ImGuiWindowFlags_NoCollapse);

        ImGui::Button("\tRestore\t");

        auto cameraParam = static_cast<glm::vec3*>(&cameraTransform.GetFront());
        ImGui::DragFloat3("Camera Forward", &cameraParam->x, 0.05f);

        //if (m_renderer->enabledFrustumCulling)
        //{
            //glm::vec3 frustumParam(cameraSettings.frustum->visibleVertices[static_cast<int>(Frustum::FrustumVertice::FarBottomLeft)]);
            //ImGui::DragFloat3("Frustum Min (far bottom left)", &frustumParam.x, 0.05f);
            //frustumParam = cameraSettings.frustum->visibleVertices[static_cast<int>(Frustum::FrustumVertice::NearTopRight)];
            //ImGui::DragFloat3("Frustum Max (near top right)", &frustumParam.x, 0.05f);
        //}

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
        }

        // Things to do and check when changing the configuration
        if (anythingChanged)
            m_renderer->updateFromChangedVariables();

        ImGui::End();
    }


    void DebugPass::renderShaderEditView()
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
                    auto shaderEntry                    = std::make_unique<ShaderEntry>();
                    shaderEntry->id                     = metadata.id;
                    shaderEntry->m_shader               = shader;
                    shaderEntry->vertexSourceBak        = shader->GetVertexCode();
                    shaderEntry->fragmentSourceBak      = shader->GetFragmentCode();
                    shaderEntry->vertexLastApplied      = shader->GetVertexCode();
                    shaderEntry->fragmentLastApplied    = shader->GetFragmentCode();

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
                    shader->dirty = !shader->vertexSourceBak.compare(shader->vertexLastApplied) || !shader->fragmentSourceBak.compare(shader->fragmentLastApplied);

                if (applyRequested)
                {
                    if (shader->dirty)
                    {
                        shader->vertexLastApplied = shader->m_shader->GetVertexCode();
                        shader->fragmentLastApplied = shader->m_shader->GetFragmentCode();
                        shader->m_shader->Recompile();
                        shader->dirty = false;
                    }
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


    void DebugPass::renderFbosView()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("FBOs", &m_fbosViewEnabled, ImGuiWindowFlags_NoCollapse);

        if (ImGui::TreeNode("GBuffer"))
        {
            if (ImGui::TreeNode("Positions"))
            {
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(0),
                    ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Normals"))
            {
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(1),
                    ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Albedo"))
            {
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(2),
                    ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("ARM"))
            {
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(3),
                    ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if (m_renderer->enabledFSR2)
            {
                if (ImGui::TreeNode("FSR2 Motion Vectors"))
                {
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(4),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Shadow Map"))
        {
            ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_directionalLightDepthBuffer->GetColorAttachmentId(0),
                ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_directionalLightDepthBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("FSR2"))
        {
            ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrFsrColorBuffer->GetColorAttachmentId(0),
                ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_directionalLightDepthBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Lighting"))
        {
            ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrColorBuffer->GetColorAttachmentId(0),
                ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrColorBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Blooming"))
        {
            if (ImGui::TreeNode("Result"))
            {
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrFinalBloomBuffer->GetColorAttachmentId(0),
                    ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrFinalBloomBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Bright threshold"))
            {
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBrightnessBuffer->GetColorAttachmentId(0),
                    ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBrightnessBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Downsampling"))
            {
                static int bloomDownsample = 0;
                ImGui::SliderInt("##BloomDownsample", &bloomDownsample, 0, 5, "%d", ImGuiSliderFlags_AlwaysClamp);
                switch (bloomDownsample)
                {
                case 0:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample1Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample1Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 1:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample2Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample2Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 2:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample3Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample3Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 3:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample4Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample4Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 4:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample5Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample5Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 5:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample6Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample6Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;

                default:
                    break;
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Upsampling"))
            {
                static int bloomUpsample = 0;
                ImGui::SliderInt("##BloomUpsample", &bloomUpsample, 0, 5, "%d", ImGuiSliderFlags_AlwaysClamp);
                switch (bloomUpsample)
                {
                case 0:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample1Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample1Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 1:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample2Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample2Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 2:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample3Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample3Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 3:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample4Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample4Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 4:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample5Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample5Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 5:
                    ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample6Buffer->GetColorAttachmentId(0),
                        ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample6Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;

                default:
                    break;
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::End();
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


    unsigned int cubeVAO2 = 0;
    unsigned int cubeVBO2 = 0;

    void DebugPass::renderOBB(const std::shared_ptr<OBB>& oob)
    {
        // Top face
        glm::vec3 top1{ oob->center.x - oob->extents.x, oob->center.y + oob->extents.y, oob->center.z + oob->extents.z };
        glm::vec3 top2{ oob->center.x - oob->extents.x, oob->center.y + oob->extents.y, oob->center.z - oob->extents.z };
        glm::vec3 top3{ oob->center.x + oob->extents.x, oob->center.y + oob->extents.y, oob->center.z - oob->extents.z };
        glm::vec3 top4{ oob->center.x + oob->extents.x, oob->center.y + oob->extents.y, oob->center.z + oob->extents.z };

        auto sizeX = top3 - top2;

        // Bottom face
        glm::vec3 bot1{ oob->center.x - oob->extents.x, oob->center.y - oob->extents.y, oob->center.z + oob->extents.z };
        glm::vec3 bot2{ oob->center.x - oob->extents.x, oob->center.y - oob->extents.y, oob->center.z - oob->extents.z };
        glm::vec3 bot3{ oob->center.x + oob->extents.x, oob->center.y - oob->extents.y, oob->center.z - oob->extents.z };
        glm::vec3 bot4{ oob->center.x + oob->extents.x, oob->center.y - oob->extents.y, oob->center.z + oob->extents.z };

        // initialize (if necessary)
        //if (cubeVAO2 == 0)
        //{
            float vertices[] = {
                // top lines
                top1.x, top1.y, top1.z, top2.x, top2.y, top2.z,
                top2.x, top2.y, top2.z, top3.x, top3.y, top3.z,
                top3.x, top3.y, top3.z, top4.x, top4.y, top4.z,
                top4.x, top4.y, top4.z, top1.x, top1.y, top1.z,

                // bottom lines
                bot1.x, bot1.y, bot1.z, bot2.x, bot2.y, bot2.z,
                bot2.x, bot2.y, bot2.z, bot3.x, bot3.y, bot3.z,
                bot3.x, bot3.y, bot3.z, bot4.x, bot4.y, bot4.z,
                bot4.x, bot4.y, bot4.z, bot1.x, bot1.y, bot1.z,

                // side lines
                top1.x, top1.y, top1.z, bot1.x, bot1.y, bot1.z,
                top2.x, top2.y, top2.z, bot2.x, bot2.y, bot2.z,
                top3.x, top3.y, top3.z, bot3.x, bot3.y, bot3.z,
                top4.x, top4.y, top4.z, bot4.x, bot4.y, bot4.z
            };
            glGenVertexArrays(1, &cubeVAO2);
            glGenBuffers(1, &cubeVBO2);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO2);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO2);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        //}
        // render Cube
        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(cubeVAO2);
        glDrawArrays(GL_LINES, 0, 32);
        glBindVertexArray(0);
    }


    unsigned int frustumVAO2 = 0;
    unsigned int frustumVBO2 = 0;

    void DebugPass::renderFrustum(const std::shared_ptr<Frustum>& frustum)
    {
        glm::vec3 farBL     = frustum->visibleVertices[0];
        glm::vec3 farTL     = frustum->visibleVertices[1];
        glm::vec3 farTR     = frustum->visibleVertices[2];
        glm::vec3 farBR     = frustum->visibleVertices[3];
        glm::vec3 nearBL    = frustum->visibleVertices[4];
        glm::vec3 nearTL    = frustum->visibleVertices[5];
        glm::vec3 nearTR    = frustum->visibleVertices[6];
        glm::vec3 nearBR    = frustum->visibleVertices[7];

        float vertices[] = {
            // far plane
            farBL.x, farBL.y, farBL.z, farTL.x, farTL.y, farTL.z,
            farTL.x, farTL.y, farTL.z, farTR.x, farTR.y, farTR.z,
            farTR.x, farTR.y, farTR.z, farBR.x, farBR.y, farBR.z,
            farBR.x, farBR.y, farBR.z, farBL.x, farBL.y, farBL.z,

            ////// near plane
            //nearBL.x, nearBL.y, nearBL.z, nearTL.x, nearTL.y, nearTL.z,
            //nearTL.x, nearTL.y, nearTL.z, nearTR.x, nearTR.y, nearTR.z,
            //nearTR.x, nearTR.y, nearTR.z, nearBR.x, nearBR.y, nearBR.z,
            //nearBR.x, nearBR.y, nearBR.z, nearBL.x, nearBL.y, nearBL.z,

            ////// side lines
            //farBL.x, farBL.y, farBL.z, nearBL.x, nearBL.y, nearBL.z,
            //farTL.x, farTL.y, farTL.z, nearTL.x, nearTL.y, nearTL.z,
            //farTR.x, farTR.y, farTR.z, nearTR.x, nearTR.y, nearTR.z,
            //farBR.x, farBR.y, farBR.z, nearBR.x, nearBR.y, nearBR.z
        };

        if (frustumVAO2 == 0)
        {
            glGenVertexArrays(1, &frustumVAO2);
            glGenBuffers(1, &frustumVBO2);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, frustumVBO2);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(frustumVAO2);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        // render Cube
        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(frustumVAO2);
        glDrawArrays(GL_LINES, 0, sizeof(vertices) / 3);
        glBindVertexArray(0);
    }


    const ImGuiTreeNodeFlags DebugPass::TREENODE_BASE_FLAGS = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding;
    const ImGuiTabBarFlags TAB_BAR_BASE_FLAGS = ImGuiTabBarFlags_Reorderable;
}
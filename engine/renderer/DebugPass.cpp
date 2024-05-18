#include "DebugPass.h"
#include "renderer/Renderer.h"
#include "imgui.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "misc/cpp/imgui_stdlib.h"
#include "input/Input.h"
#include "core/ExportedVariablesManager.h"
#include <typeindex>


namespace nimo
{
    DebugPass::DebugPass(std::shared_ptr<SceneRenderer> renderer) :
        m_renderer(renderer),
        m_shadersEditorViewEnabled(true)
    {
    }

    void DebugPass::update(float deltaTime)
    {
        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::D))
            m_statsViewEnabled = !m_statsViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::V))
            m_exportedVariablesViewEnabled = !m_exportedVariablesViewEnabled;

        if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::S))
            m_shadersEditorViewEnabled = !m_shadersEditorViewEnabled;

        if(m_shadersEditorViewEnabled && nimo::Input::GetKey(nimo::KeyCode::LeftControl) && nimo::Input::GetKeyPressed(nimo::KeyCode::D8))
            applyRequested = true;

        if (m_shadersEditorViewEnabled && nimo::Input::GetKey(nimo::KeyCode::LeftControl) && nimo::Input::GetKeyPressed(nimo::KeyCode::Z))
            revertRequested = true;

        m_currentTime += deltaTime;
        m_timeDebugRefresh += deltaTime;
        if (m_timeDebugRefresh > 1.f / m_refreshRate)
        {
            //m_displayedStats.frameTime = m_gameViewPanel->getFrameTime();
            m_displayedStats.frameTime = m_renderer->currentFrameTime();
            if (m_currentTime > 2)
                m_displayedStats.maximumFrameTime = std::max(m_displayedStats.maximumFrameTime, m_displayedStats.frameTime);
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

            // Periodically dump the records to CSV

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

            ImGui::Begin("Stats", &m_statsViewEnabled, ImGuiWindowFlags_NoCollapse);

            ImGui::Text("Render stats");
            ImGui::TextDisabled("Draw calls: %d", nimo::Renderer::stats.totalDrawCalls);
            ImGui::TextDisabled("Triangle count: %d", nimo::Renderer::stats.totalTriangles);
            ImGui::Text("FPS: %.2f", m_displayedStats.frameTime > 0 ? 1000.f / m_displayedStats.frameTime : 0);
            ImGui::Text("Avg FPS: %.2f", m_displayedStats.averageFrameTime > 0 ? 1000.f / m_displayedStats.averageFrameTime : 0);
            ImGui::Text("Min FPS: %.2f", m_displayedStats.maximumFrameTime > 0 ? 1000.f / m_displayedStats.maximumFrameTime : 0);
            ImGui::Text("Last frame time: %.3f ms", m_displayedStats.frameTime);

            ImGui::End();
        }

        if (m_exportedVariablesViewEnabled)
        {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

            ImGui::Begin("Variables", &m_exportedVariablesViewEnabled, ImGuiWindowFlags_NoCollapse);

            ImGui::Button("\tRestore\t");

            for (const auto& exportedVariablePair : ExportedVariablesManager::instance()->variables())
            {
                auto exportedVariable = exportedVariablePair.second;
                if (exportedVariable->m_type == std::type_index(typeid(bool)))
                {
                    ImGui::Checkbox(exportedVariable->m_name.c_str(), static_cast<bool*>(exportedVariable->m_value));
                }

                if (exportedVariable->m_type == std::type_index(typeid(unsigned int)))
                {
                    //ImGui::Text("Vertex");
                    //ImGui::SameLine();
                    ImGui::PushItemWidth(40.f);
                    ImGui::DragInt(exportedVariable->m_name.c_str(), static_cast<int*>(exportedVariable->m_value));
                }
            }

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

            applyRequested |= ImGui::Button("\tApply Changes (Ctrl+A)\t");
            ImGui::SameLine();
            if (ImGui::Button("\tRevert Changes (Ctrl+Z)\t"))
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
                    //ImGui::PushItemWidth(-1);
                    ImVec2 size(1000.f, 1000.f);
                    bool changed{ false };
                    changed = ImGui::InputTextMultiline((std::string("Vertex") + std::to_string(i)).c_str(), shader->m_shader->GetVertexCodePtr(), ImVec2(ImGui::GetContentRegionAvailWidth(), 400.f), ImGuiInputTextFlags_AllowTabInput);
                    changed |= ImGui::InputTextMultiline((std::string("Fragment") + std::to_string(i)).c_str(), shader->m_shader->GetFragmentCodePtr(), ImVec2(ImGui::GetContentRegionAvailWidth(), 400.f), ImGuiInputTextFlags_AllowTabInput);

                    if (revertRequested)
                    {
                        shader->dirty = shader->vertexSourceBak.compare(shader->m_shader->GetVertexCode()) | shader->fragmentSourceBak.compare(shader->m_shader->GetFragmentCode());
                        shader->m_shader->GetVertexCode() = shader->vertexSourceBak;
                        shader->m_shader->GetFragmentCode() = shader->fragmentSourceBak;
                        revertRequested = false;
                    }

                    if (changed)
                    {
                        shader->dirty = shader->vertexSourceBak.compare(shader->m_shader->GetVertexCode()) | shader->fragmentSourceBak.compare(shader->m_shader->GetFragmentCode());
                    }

                    if (applyRequested && shader->dirty)
                    {
                        shader->m_shader->Recompile();
                        shader->dirty = false;
                        applyRequested = false;
                    }
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

    const ImGuiTreeNodeFlags DebugPass::TREENODE_BASE_FLAGS = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding;
    const ImGuiTabBarFlags TAB_BAR_BASE_FLAGS = ImGuiTabBarFlags_Reorderable;
}
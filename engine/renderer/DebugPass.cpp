#include "DebugPass.h"
#include "imgui.h"
#include "renderer/Renderer.h"
#include "imgui.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "input/Input.h"
#include "core/ExportedVariablesManager.h"
#include <typeindex>


void nimo::DebugPass::update(float deltaTime)
{
    if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::D))
        m_statsViewEnabled = !m_statsViewEnabled;

    if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::V))
        m_exportedVariablesViewEnabled = !m_exportedVariablesViewEnabled;

    if (nimo::Input::GetKey(nimo::KeyCode::LeftAlt) && nimo::Input::GetKeyPressed(nimo::KeyCode::S))
        m_shadersEditorViewEnabled = !m_shadersEditorViewEnabled;

    m_currentTime += deltaTime;
    m_timeDebugRefresh += deltaTime;
    if (m_timeDebugRefresh > 1.f / m_refreshRate)
    {
        //m_displayedStats.frameTime = m_gameViewPanel->getFrameTime();
        m_displayedStats.frameTime = m_renderer->currentFrameTime();
        if(m_currentTime > 2)
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


void nimo::DebugPass::render(std::shared_ptr<FrameBuffer> target, const CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
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
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

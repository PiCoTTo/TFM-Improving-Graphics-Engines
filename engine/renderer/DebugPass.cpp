#include "DebugPass.h"
#include "imgui.h"
#include "renderer/Renderer.h"
#include "imgui.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>


void nimo::DebugPass::update(float deltaTime)
{
    m_currentTime += deltaTime;
    m_timeDebugRefresh += deltaTime;
    if (m_timeDebugRefresh > 1.f / m_refreshRate)
    {
        //m_displayedStats.frameTime = m_gameViewPanel->getFrameTime();
        m_displayedStats.frameTime = m_renderer->currentFrameTime();
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

        // Periodically dump the records to CSV

        m_timeDebugRefresh = 0;
    }
}


void nimo::DebugPass::render()
{
    if (!m_renderer->mustRender())
        return;

	// Stats GUI
	if (!m_statsViewEnabled) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("Stats", &m_statsViewEnabled, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Render stats");
    ImGui::TextDisabled("Draw calls: %d", nimo::Renderer::stats.totalDrawCalls);
    ImGui::TextDisabled("Triangle count: %d", nimo::Renderer::stats.totalTriangles);
    ImGui::Text("FPS: %.2f", m_displayedStats.frameTime > 0 ? 1000.f / m_displayedStats.frameTime : 0);
    ImGui::Text("Last frame time: %.3f ms", m_displayedStats.frameTime);

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

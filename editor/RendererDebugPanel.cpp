#include "RendererDebugPanel.h"
#include "renderer/Renderer.h"


void RendererDebugPanel::OnRender(float deltaTime)
{
    if (!m_renderer) return;
    if (frameTimes.size() > 1499)
    {
        frameTimes.erase(frameTimes.begin());
    }
    frameTimes.push_back(m_renderer->currentFrameTime());

    m_timeDebugRefresh += deltaTime;
    if (m_timeDebugRefresh > 1.f / m_refreshRate)
    {
        if (m_gameViewPanel->open)
        {
            //m_displayedStats.frameTime = m_gameViewPanel->getFrameTime();
            m_displayedStats.frameTime = m_renderer->currentFrameTime();
            m_displayedStats.renderFrameTime = m_renderer->m_renderFrameTimer.ElapsedMillis();
            m_displayedStats.geometryFrameTime = m_renderer->m_geometryFrameTimer.ElapsedMillis();
            m_displayedStats.lightingFrameTime = m_renderer->m_lightingFrameTimer.ElapsedMillis();
            m_displayedStats.bloomFrameTime = m_renderer->m_bloomFrameTimer.ElapsedMillis();
            m_displayedStats.geometry2DFrameTime = m_renderer->m_geometry2DFrameTimer.ElapsedMillis();
        }
        else
            m_displayedStats = {};

        m_timeDebugRefresh = 0;
    }

    ImGui::Spacing();
    ImGui::PlotLines("Frame Times", frameTimes.data(), frameTimes.size(), 0, "Frame times", 0.0f, 35.0f, ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / 3.0f));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Text("Render stats");
    ImGui::TextDisabled("Draw calls: %d", nimo::Renderer::stats.totalDrawCalls);
    ImGui::TextDisabled("Triangle count: %d", nimo::Renderer::stats.totalTriangles);
    //ImGui::Text("FPS: %d", static_cast<unsigned int>(1000.f / m_displayedStats.frameTime));
    ImGui::Text("FPS: %.2f", m_displayedStats.frameTime > 0 ? 1000.f / m_displayedStats.frameTime : 0);
    ImGui::Text("Last frame time: %.3f ms", m_displayedStats.frameTime);
    ImGui::TextDisabled("Render: %.3f ms", m_displayedStats.renderFrameTime);
    ImGui::TextDisabled("Geometry 3D: %.3f ms", m_displayedStats.geometryFrameTime);
    ImGui::TextDisabled("Lighting: %.3f ms", m_displayedStats.lightingFrameTime);
    ImGui::TextDisabled("Bloom: %.3f ms", m_displayedStats.bloomFrameTime);
    ImGui::TextDisabled("Geometry 2D: %.3f ms", m_displayedStats.geometry2DFrameTime);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::TreeNode("GBuffer"))
    {
        ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(1), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(2), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_gBuffer->GetColorAttachmentId(3), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Directional Lighting"))
    {
        ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_directionalLightDepthBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_directionalLightDepthBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Lighting pass"))
    {
        ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrColorBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrColorBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Bloom pass"))
    {
        if (ImGui::TreeNode("Result"))
        {
            ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrFinalBloomBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrFinalBloomBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Bright threshold"))
        {
            ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBrightnessBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBrightnessBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Downsampling"))
        {
            static int bloomDownsample = 0;
            ImGui::SliderInt("##BloomDownsample", &bloomDownsample, 0, 5, "%d", ImGuiSliderFlags_AlwaysClamp);
            switch (bloomDownsample)
            {
            case 0:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample1Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample1Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 1:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample2Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample2Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 2:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample3Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample3Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 3:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample4Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample4Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 4:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample5Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample5Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 5:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomDownsample6Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomDownsample6Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
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
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample1Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample1Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 1:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample2Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample2Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 2:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample3Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample3Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 3:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample4Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample4Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 4:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample5Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample5Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;
            case 5:
                ImGui::Image((ImTextureID)(uint64_t)m_renderer->m_hdrBloomUpsample6Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth() / m_renderer->m_hdrBloomUpsample6Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                break;

            default:
                break;
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}
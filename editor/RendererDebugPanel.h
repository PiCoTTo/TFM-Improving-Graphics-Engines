#pragma once
#include "EditorPanel.h"
#include "imgui.h"
#include "renderer/SceneRenderer.h"

struct RendererDebugPanel : public EditorPanel
{
    RendererDebugPanel() : EditorPanel("Renderer Debug"){
        frameTimes.reserve(1500);
    }
    void SetRenderer(std::shared_ptr<nimo::SceneRenderer> r) {renderer=r;}

private:
    std::vector<float> frameTimes;
    std::shared_ptr<nimo::SceneRenderer> renderer;
    void OnRender(float deltaTime){
        if(!renderer) return;
        if(frameTimes.size() > 1499)
        {
            frameTimes.erase(frameTimes.begin());
        }
        frameTimes.push_back(renderer->m_totalFrameTimer.ElapsedMillis());
        ImGui::Spacing();
        ImGui::PlotLines("Frame Times", frameTimes.data(), frameTimes.size(),0,"Frame times",0.0f, 35.0f, ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/3.0f));
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Render stats");
        ImGui::TextDisabled("Draw calls: %d", nimo::Renderer::stats.totalDrawCalls);
        ImGui::TextDisabled("Triangle count: %d", nimo::Renderer::stats.totalTriangles);
        ImGui::Text("Last frame time: %.3f ms", renderer->m_totalFrameTimer.ElapsedMillis());
        ImGui::TextDisabled("Geometry 3D: %.3f ms", renderer->m_geometryFrameTimer.ElapsedMillis());
        ImGui::TextDisabled("Lighting: %.3f ms", renderer->m_lightingFrameTimer.ElapsedMillis());
        ImGui::TextDisabled("Bloom: %.3f ms", renderer->m_bloomFrameTimer.ElapsedMillis());
        ImGui::TextDisabled("Geometry 2D: %.3f ms", renderer->m_geometry2DFrameTimer.ElapsedMillis());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if(ImGui::TreeNode("GBuffer"))
        {
            ImGui::Image((ImTextureID)(uint64_t)renderer->m_gBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Image((ImTextureID)(uint64_t)renderer->m_gBuffer->GetColorAttachmentId(1), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Image((ImTextureID)(uint64_t)renderer->m_gBuffer->GetColorAttachmentId(2), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Image((ImTextureID)(uint64_t)renderer->m_gBuffer->GetColorAttachmentId(3), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_gBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Directional Lighting"))
        {
            ImGui::Image((ImTextureID)(uint64_t)renderer->m_directionalLightDepthBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_directionalLightDepthBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Lighting pass"))
        {
            ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrColorBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrColorBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Bloom pass"))
        {
            if(ImGui::TreeNode("Result"))
            {
                ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrFinalBloomBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrFinalBloomBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Bright threshold"))
            {
                ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBrightnessBuffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBrightnessBuffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Downsampling"))
            {
                static int bloomDownsample = 0;
                ImGui::SliderInt("##BloomDownsample", &bloomDownsample, 0, 5, "%d", ImGuiSliderFlags_AlwaysClamp);
                switch (bloomDownsample)
                {
                case 0:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomDownsample1Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomDownsample1Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 1:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomDownsample2Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomDownsample2Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 2:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomDownsample3Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomDownsample3Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 3:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomDownsample4Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomDownsample4Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 4:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomDownsample5Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomDownsample5Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 5:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomDownsample6Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomDownsample6Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                
                default:
                    break;
                }
                ImGui::TreePop();
            }
            if(ImGui::TreeNode("Upsampling"))
            {
                static int bloomUpsample = 0;
                ImGui::SliderInt("##BloomUpsample", &bloomUpsample, 0, 5, "%d", ImGuiSliderFlags_AlwaysClamp);
                switch (bloomUpsample)
                {
                case 0:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomUpsample1Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomUpsample1Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 1:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomUpsample2Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomUpsample2Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 2:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomUpsample3Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomUpsample3Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 3:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomUpsample4Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomUpsample4Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 4:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomUpsample5Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomUpsample5Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case 5:
                    ImGui::Image((ImTextureID)(uint64_t)renderer->m_hdrBloomUpsample6Buffer->GetColorAttachmentId(0), ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvailWidth()/renderer->m_hdrBloomUpsample6Buffer->GetAspectRatio()), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                
                default:
                    break;
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
};
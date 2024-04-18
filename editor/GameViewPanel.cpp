#include "GameViewPanel.h"
#include "imgui.h"
#include "EditorLayer.h"

void GameViewPanel::OnRender(float deltaTime)
{
    if (!m_editor->renderer) return;

    if (EditorLayer::mustRender())
    {
        m_frameTimer.Stop();
        m_frameTime = m_frameTimer.ElapsedMillis();
        //NIMO_DEBUG("{}", 1000 / 60.f);
        NIMO_DEBUG("{}", m_frameTime);
        m_frameTimer.Reset();

        nimo::Renderer::BeginFrame(m_editor->fb);
        for (auto scene : nimo::AssetManager::GetAllLoaded<nimo::Scene>())
        {
            m_editor->renderer->SetScene(scene);
            m_editor->renderer->Render(m_editor->fb, scene->GetMainCamera(), scene->GetMainCameraTransform(), deltaTime);
        }
        nimo::Renderer::EndFrame();
    }

    // Using a Child allow to fill all the space of the window.
    // It also alows customization
    ImGui::BeginChild("GameRender");
    // Get the size of the child (i.e. the whole draw size of the windows).
    ImVec2 wsize = ImGui::GetWindowSize();
    // Because I use the texture from OpenGL, I need to invert the V from the UV.
    ImGui::Image((ImTextureID)(uint64_t)m_editor->fb->GetColorAttachmentId(0), wsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
}

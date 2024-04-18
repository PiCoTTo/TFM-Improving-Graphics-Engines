#include "SceneViewPanel.h"
#include "imgui.h"
#include "EditorLayer.h"
#include "input/Input.h"
#include "core/Log.h"

void SceneViewPanel::OnRender(float deltaTime)
{
    if (!m_editor->renderer) return;

    if (EditorLayer::mustRender())
    {
        nimo::Renderer::BeginFrame(fb);
        for (auto scene : nimo::AssetManager::GetAllLoaded<nimo::Scene>())
        {
            m_editor->renderer->SetScene(scene);
            m_editor->renderer->Render(fb, {}, t, deltaTime);
        }
        nimo::Renderer::EndFrame();
    }

    // Using a Child allow to fill all the space of the window.
    // It also alows customization
    ImGui::BeginChild("SceneRender");
    // Get the size of the child (i.e. the whole draw size of the windows).
    ImVec2 wsize = ImGui::GetWindowSize();
    // Because I use the texture from OpenGL, I need to invert the V from the UV.
    ImGui::Image((ImTextureID)(uint64_t)fb->GetColorAttachmentId(0), wsize, ImVec2(0, 1), ImVec2(1, 0));
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::W))
    {
        t.Translation += deltaTime * 3.0f * t.GetFront();
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::A))
    {
        t.Translation -= deltaTime * 3.0f * t.GetRight();
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::S))
    {
        t.Translation -= deltaTime * 3.0f * t.GetFront();
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::D))
    {
        t.Translation += deltaTime * 3.0f * t.GetRight();
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::Space))
    {
        t.Translation.y += deltaTime * 3.0f;
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::LeftControl))
    {
        t.Translation.y -= deltaTime * 3.0f;
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::Q))
    {
        t.Rotation.y -= deltaTime * 50.0f;
    }
    if (ImGui::IsWindowFocused() && nimo::Input::GetKey(nimo::KeyCode::E))
    {
        t.Rotation.y += deltaTime * 50.0f;
    }
    ImGui::EndChild();
}

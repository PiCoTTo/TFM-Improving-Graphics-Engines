#include "EditorPanel.h"

#include "imgui.h"

void EditorPanel::Render(float deltaTime){
    if(open){
        ImGui::Begin(m_name.c_str(), &open, ImGuiWindowFlags_NoCollapse);
        OnRender(deltaTime);
        ImGui::End();
    }
}
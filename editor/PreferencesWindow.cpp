#include "PreferencesWindow.h"
//#include "imgui.h"
#include "imgui_internal.h"


PreferencesWindow::PreferencesWindow(std::shared_ptr<PreferencesController>& controller) :
    m_controller(controller)
{
}

void PreferencesWindow::show(bool& show)
{
    ImGui::Begin("Preferences", &show);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

    ImGui::Text("Frame limits");
    
    bool currentValue = m_limitFPS_Global;
    ImGui::Checkbox("Limit Global FPS", &m_limitFPS_Global);
    m_anyChange |= m_limitFPS_Global != currentValue;
    
    currentValue = m_limitFPS_3D;
    ImGui::Checkbox("Limit 3D FPS", &m_limitFPS_3D);
    m_anyChange |= m_limitFPS_3D != currentValue;

    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        if (m_anyChange)
        {
            m_controller->setLimitFPS(m_limitFPS_Global, m_limitFPS_3D);
            m_anyChange = false;
        }
        show = false;
    }

    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0)))
        show = false;

    ImGui::End();
}

void PreferencesWindow::update(Preferences* subject)
{
    m_limitFPS_3D = subject->limitFPS_AppInDev;
    m_limitFPS_Global = subject->limitFPS_Global;
}

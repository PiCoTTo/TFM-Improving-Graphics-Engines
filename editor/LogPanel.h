#pragma once
#include "EditorPanel.h"
#include "imgui.h"
#include "events/EventManager.h"
#include "core/Log.h"

struct LogPanel : public EditorPanel
{
    LogPanel() : EditorPanel("Log"){
        nimo::EventManager::Subscribe(this, &LogPanel::AddLog);
        logBuffer.reserve(5 * 1024 * 1024);
    }
    ~LogPanel(){
        nimo::EventManager::Unsubscribe(this, &LogPanel::AddLog);
    }

    void AddLog(const nimo::LogEvent& e)
    {
        if(e.level == nimo::Log::Level::Info && !debug) return;
        if(e.level == nimo::Log::Level::Debug && !info) return;
        if(e.level == nimo::Log::Level::Warn && !warning) return;
        logBuffer.append((e.msg + '\n').c_str());
    }

private:
    ImGuiTextBuffer logBuffer;
    bool autoscroll = true;
    bool debug = true;
    bool info = true;
    bool warning = true;
    void OnRender(float deltaTime){
        if(ImGui::Button("Clear"))
        {
            logBuffer.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Autoscroll", &autoscroll);
        ImGui::SameLine();
        ImGui::Checkbox("Warning", &warning);
        ImGui::SameLine();
        ImGui::Checkbox("Info", &info);
        ImGui::SameLine();
        ImGui::Checkbox("Debug", &debug);
        ImGui::BeginChildFrame(ImGui::GetID("LogBufferWindow"), ImVec2(0,0), ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::TextUnformatted(logBuffer.begin(), logBuffer.end());
        if (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChildFrame();
    }
};
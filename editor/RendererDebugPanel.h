#pragma once
#include "EditorPanel.h"
#include "imgui.h"
#include "renderer/SceneRenderer.h"
#include "renderer/Renderer.h"
#include "GameViewPanel.h"


class RendererDebugPanel : public EditorPanel
{
public:
    RendererDebugPanel() : EditorPanel("Renderer Debug") {
        frameTimes.reserve(1500);
    }
    void SetRenderer(std::shared_ptr<nimo::SceneRenderer> r) { renderer = r; }
    void SetGameViewPanel(std::shared_ptr<GameViewPanel> gvp) { m_gameViewPanel = gvp; }

private:
    nimo::RendererStats m_displayedStats;
    int m_refreshRate = 2;  // Stats display update frequency (times/sec)
    float m_timeDebugRefresh = 0;   // Time elapsed since last debug stats update

    std::vector<float> frameTimes;
    std::shared_ptr<nimo::SceneRenderer> renderer;
    std::shared_ptr<GameViewPanel> m_gameViewPanel;
    void OnRender(float deltaTime);
};
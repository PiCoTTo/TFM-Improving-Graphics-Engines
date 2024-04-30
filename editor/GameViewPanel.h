#pragma once
#include "EditorPanel.h"

#include <memory>
#include "core/Timer.h"
#include "renderer/SceneRenderer.h"

class EditorLayer;
struct GameViewPanel : public EditorPanel
{
    GameViewPanel(EditorLayer* editor) : m_editor(editor), EditorPanel("Game") {}

    void setRenderer(std::shared_ptr<nimo::SceneRenderer> renderer) { m_renderer = renderer; }

    //float getFrameTime() { return m_frameTime; }

private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;
    std::shared_ptr<nimo::SceneRenderer> m_renderer;

    float m_cumulativeFrameTime = 1 / 60.0f;
    nimo::Timer m_frameTimer;
    float m_frameTime;
};
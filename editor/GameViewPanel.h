#pragma once
#include "EditorPanel.h"

#include <memory>
#include "core/Timer.h"

class EditorLayer;
struct GameViewPanel : public EditorPanel
{
    GameViewPanel(EditorLayer* editor) : m_editor(editor), EditorPanel("Game") {}

    float getFrameTime() { return m_frameTime; }

private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;

    float m_cumulativeFrameTime = 1 / 60.0f;
    nimo::Timer m_frameTimer;
    float m_frameTime;
};
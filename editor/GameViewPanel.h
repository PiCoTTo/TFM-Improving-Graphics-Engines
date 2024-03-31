#pragma once
#include "EditorPanel.h"

#include <memory>

class EditorLayer;
struct GameViewPanel : public EditorPanel
{
    GameViewPanel(EditorLayer* editor) : m_editor(editor), EditorPanel("Game"){}

private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;
};
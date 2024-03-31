#pragma once
#include "EditorPanel.h"
#include "scene/Scene.h"

#include <memory>

class EditorLayer;
struct SceneContentsPanel : public EditorPanel
{
    SceneContentsPanel(EditorLayer* editor) : m_editor(editor), EditorPanel("Scene"){}
    void PaintEntity(const std::shared_ptr<nimo::Scene>& scene, nimo::Entity& ent);

private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;
    nimo::GUID selectedEntity;
};
#pragma once
#include "EditorPanel.h"
#include "scene/Scene.h"
#include "assets/AssetSettings.h"

#include <memory>

class EditorLayer;
struct InspectorPanel : public EditorPanel
{
    InspectorPanel(EditorLayer* editor) : m_editor(editor), EditorPanel("Inspector"){}
    void SetViewItem(nimo::GUID id);
    void ResetViewItem() {selectedItem = nimo::GUID();}
private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;
    nimo::GUID selectedItem;
};
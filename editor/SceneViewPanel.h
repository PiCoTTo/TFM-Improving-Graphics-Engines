#pragma once
#include "EditorPanel.h"
#include "scene/Components.h"
#include "renderer/FrameBuffer.h"
#include <memory>
#include "glad/glad.h"

class EditorLayer;
struct SceneViewPanel : public EditorPanel
{
    SceneViewPanel(EditorLayer* editor) : m_editor(editor), EditorPanel("Scene View"){
        nimo::FrameBuffer::Details d;
        d.width = 1920;
        d.height = 1080;
        d.colorAttachments.push_back({GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE});
        fb = std::make_shared<nimo::FrameBuffer>(d);
    }

private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;
    nimo::TransformComponent t;
    std::shared_ptr<nimo::FrameBuffer> fb;
};
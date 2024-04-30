#pragma once
#include "EditorPanel.h"
#include "scene/Components.h"
#include "renderer/FrameBuffer.h"
#include "renderer/SceneRenderer.h"
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

    void setRenderer(std::shared_ptr<nimo::SceneRenderer> renderer) { m_renderer = renderer; }

private:
    void OnRender(float deltaTime);
    EditorLayer* m_editor;
    std::shared_ptr<nimo::SceneRenderer> m_renderer;

    nimo::TransformComponent t;
    std::shared_ptr<nimo::FrameBuffer> fb;
};
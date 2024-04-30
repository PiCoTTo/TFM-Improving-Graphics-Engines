#pragma once
#include "core/Log.h"
#include "core/Layer.h"
#include "core/FileHandling.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "renderer/EnvironmentMap.h"
#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/FrameBuffer.h"
#include "renderer/Mesh.h"
#include "renderer/SceneRenderer.h"
#include "assets/AssetManager.h"
#include "scene/Scene.h"
#include "scene/SceneSerializer.h"
#include "project/ProjectSerializer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "nfd.h"
#include "UIHelpers.h"
#include "LogPanel.h"
#include "Subject.h"
#include "PreferencesWindow.h"
#include "core/Timer.h"

class GameViewPanel;
class SceneContentsPanel;
class InspectorPanel;
class AssetExplorerPanel;
class SceneViewPanel;
class RendererDebugPanel;

class EditorLayer : public nimo::Layer, public Subject<EditorLayer>
{
public:
    EditorLayer();
    ~EditorLayer()
    {
        delete inspectorPanel;
        delete sceneContentsPanel;
        delete assetExplorerPanel;
        delete sceneViewPanel;
        delete rendererDebugPanel;
        delete logPanel;
    }

    static const float FPS_LIMIT;
    static const float GLOBAL_FPS_LIMIT;
    static bool mustRender() { return m_mustRender; }

    void openProject(nfdchar_t* path);

private:
    friend class GameViewPanel;
    friend class SceneContentsPanel;
    friend class InspectorPanel;
    friend class AssetExplorerPanel;
    friend class SceneViewPanel;

    nimo::FrameBuffer::Details d;
    std::shared_ptr<nimo::FrameBuffer> fb;
    std::shared_ptr<nimo::SceneRenderer> m_sceneViewRenderer;
    std::shared_ptr<nimo::SceneRenderer> m_gameViewRenderer;

    LogPanel* logPanel;
    std::shared_ptr<GameViewPanel> gameViewPanel;
    SceneViewPanel* sceneViewPanel;
    InspectorPanel* inspectorPanel;
    SceneContentsPanel* sceneContentsPanel;
    AssetExplorerPanel* assetExplorerPanel;
    RendererDebugPanel* rendererDebugPanel;

    nimo::GUID lastModifiedScene;
    ChangeNameModalWindow newNameModal;

    inline static Preferences preferences;
    std::shared_ptr<PreferencesController> preferencesController;
    PreferencesWindow* preferencesWindow;

    std::map<nimo::AssetType, std::shared_ptr<nimo::Texture>> assetIcons;

    float m_cumulativeFrameTime = 1 / FPS_LIMIT;
    float m_cumulativeGlobalFrameTime = 1 / GLOBAL_FPS_LIMIT;
    static bool m_mustRender;

    bool m_showPreferencesWindow = 0;

    void OnAttach() override;
    void OnUpdate(float deltaTime) override;

    void CreateNewProject(const std::filesystem::path& folder, const std::string& name);
    std::shared_ptr<nimo::Texture> entityIcon;
};
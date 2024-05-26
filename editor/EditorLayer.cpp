#include "EditorLayer.h"

#include "shellapi.h"
#include "GameViewPanel.h"
#include "SceneViewPanel.h"
#include "SceneContentsPanel.h"
#include "InspectorPanel.h"
#include "AssetExplorerPanel.h"
#include "RendererDebugPanel.h"
#include "misc/cpp/imgui_stdlib.h"
#include "UIHelpers.h"

#include "core/Application.h"
#include "scene/SceneManager.h"
#include "scene/Prefab.h"
#include "scripting/ScriptManager.h"
#include "input/Input.h"


const float EditorLayer::FPS_LIMIT = 60.f;
const float EditorLayer::GLOBAL_FPS_LIMIT = 120.f;
bool EditorLayer::m_mustRender = false;

EditorLayer::EditorLayer() :
    newNameModal("New Name")
{
    assetIcons[nimo::AssetType::None] = std::make_shared<nimo::Texture>("icons/icon_file.png");
    assetIcons[nimo::AssetType::Texture] = std::make_shared<nimo::Texture>("icons/icon_camera.png");
    assetIcons[nimo::AssetType::Mesh] = std::make_shared<nimo::Texture>("icons/icon_cube.png");
    assetIcons[nimo::AssetType::Shader] = std::make_shared<nimo::Texture>("icons/icon_polaroid.png");
    assetIcons[nimo::AssetType::Material] = std::make_shared<nimo::Texture>("icons/icon_paintroll.png");
    assetIcons[nimo::AssetType::Scene] = std::make_shared<nimo::Texture>("icons/icon_castle.png");
    assetIcons[nimo::AssetType::EnvironmentMap] = std::make_shared<nimo::Texture>("icons/icon_picture.png");
    assetIcons[nimo::AssetType::Script] = std::make_shared<nimo::Texture>("icons/icon_gears.png");
    assetIcons[nimo::AssetType::Prefab] = std::make_shared<nimo::Texture>("icons/icon_box.png");
    assetIcons[nimo::AssetType::Audio] = std::make_shared<nimo::Texture>("icons/icon_audio_wave.png");
    assetIcons[nimo::AssetType::Font] = std::make_shared<nimo::Texture>("icons/icon_signature.png");

    preferencesController = std::make_shared<PreferencesController>(preferences);
    preferencesWindow = new PreferencesWindow(preferencesController);
    preferences.addObserver(preferencesWindow);
}

void EditorLayer::OnAttach()
{
    d.width = 1920;
    d.height = 1080;
    entityIcon = std::make_shared<nimo::Texture>("icons/icon_dimension.png");
    d.colorAttachments.push_back({ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE });
    fb = std::make_shared<nimo::FrameBuffer>(d);
    logPanel = new LogPanel();
    //gameViewPanel = new GameViewPanel(this);
    gameViewPanel = std::make_shared<GameViewPanel>(this);
    sceneViewPanel = new SceneViewPanel(this);
    sceneContentsPanel = new SceneContentsPanel(this);
    inspectorPanel = new InspectorPanel(this);
    assetExplorerPanel = new AssetExplorerPanel(this);
    rendererDebugPanel = new RendererDebugPanel(gameViewPanel);

    auto& style = ImGui::GetStyle();
    auto& colors = ImGui::GetStyle().Colors;

    //========================================================
    /// Fonts
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontFromFileTTF("fonts/Quicksand/static/Quicksand-Medium.ttf", 20.0f);

    //========================================================
    /// Colours

    // Headers
    colors[ImGuiCol_MenuBarBg] = ImColor(16, 16, 16, 255);
    colors[ImGuiCol_Header] = ImColor(188, 108, 37, 255);
    colors[ImGuiCol_HeaderHovered] = ImColor(188, 108, 37, 125);
    colors[ImGuiCol_HeaderActive] = ImColor(188, 108, 37, 255);

    // Buttons
    colors[ImGuiCol_Button] = ImColor(188, 108, 37, 200);
    colors[ImGuiCol_ButtonHovered] = ImColor(251, 209, 162, 150);
    colors[ImGuiCol_ButtonActive] = ImColor(188, 108, 37, 150);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImColor(16, 16, 16, 200);
    colors[ImGuiCol_FrameBgHovered] = ImColor(66, 66, 66, 255);
    colors[ImGuiCol_FrameBgActive] = ImColor(66, 66, 66, 255);

    // Tabs
    colors[ImGuiCol_Tab] = ImColor(44, 44, 44, 255);
    colors[ImGuiCol_TabHovered] = ImColor(188, 108, 37, 255);
    colors[ImGuiCol_TabActive] = ImColor(188, 108, 37, 225);
    colors[ImGuiCol_TabUnfocused] = ImColor(44, 44, 44, 255);
    colors[ImGuiCol_TabUnfocusedActive] = ImColor(188, 108, 37, 150);

    // Title
    colors[ImGuiCol_TitleBg] = ImColor(16, 16, 16, 255);
    colors[ImGuiCol_TitleBgActive] = ImColor(16, 16, 16, 255);
    colors[ImGuiCol_TitleBgCollapsed] = ImColor(16, 16, 16, 255);

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

    // Check Mark
    colors[ImGuiCol_CheckMark] = ImColor(200, 200, 200, 255);

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);

    // Text
    colors[ImGuiCol_Text] = ImColor(240, 240, 240, 255);

    // Checkbox
    colors[ImGuiCol_CheckMark] = ImColor(240, 240, 240, 255);

    // Separator
    colors[ImGuiCol_Separator] = ImColor(16, 16, 16, 200);
    colors[ImGuiCol_SeparatorActive] = ImColor(251, 209, 162, 255);
    colors[ImGuiCol_SeparatorHovered] = ImColor(251, 209, 162, 150);

    // Window Background
    colors[ImGuiCol_WindowBg] = ImColor(44, 44, 44, 255);
    colors[ImGuiCol_ChildBg] = ImColor(66, 66, 66, 255);
    colors[ImGuiCol_PopupBg] = ImColor(66, 66, 66, 255);
    colors[ImGuiCol_Border] = ImColor(66, 66, 66, 255);

    colors[ImGuiCol_DragDropTarget] = ImColor(200, 200, 200, 255);

    // Tables
    // colors[ImGuiCol_TableHeaderBg]		= ImGui::ColorConvertU32ToFloat4(Colours::Theme::groupHeader);
    // colors[ImGuiCol_TableBorderLight]	= ImGui::ColorConvertU32ToFloat4(Colours::Theme::backgroundDark);

    // Menubar
    colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f };

    //========================================================
    /// Style
    style.FrameRounding = 10.0f;
    style.FrameBorderSize = 0.0f;
    style.IndentSpacing = 20.0f;
    style.WindowPadding = ImVec2(5.f, 5.f);
    style.FramePadding = ImVec2(5.f, 3.f);
    style.ItemSpacing = ImVec2(5.f, 4.f);
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.GrabMinSize = 10.f;
    style.GrabRounding = 3.f;
}

void EditorLayer::OnUpdate(float deltaTime)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoWindowMenuButton;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);


    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    ImGui::End();

    static bool showCreateNewProjectModal = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene"))
            {
                if (nimo::Project::GetActiveProject())
                {
                    // Unload current scene
                    // Create new Scene
                    // Show new scene
                }
                else
                {
                    NIMO_ERROR("No project loaded to create new scene");
                }
            }
            if (ImGui::MenuItem("Open Scene"))
            {
                if (nimo::Project::GetActiveProject())
                {
                    nfdchar_t* outPath = NULL;
                    nfdresult_t result = NFD_OpenDialog("nscene", NULL, &outPath);

                    if (result == NFD_OKAY) {
                        std::filesystem::path scenePath(outPath);
                        // size_t lastdot = scenePath.filename().string().find_last_of(".");
                        // std::string sceneName;
                        // if (lastdot == std::string::npos)
                        //     sceneName = scenePath.filename().string();
                        // else
                        //     sceneName = scenePath.filename().string().substr(0, lastdot);

                        // std::shared_ptr<nimo::Scene> scene = std::make_shared<nimo::Scene>();
                        // scene->SetName(sceneName);
                        // if(!scene->id.Valid())
                        // {
                        //     scene->id = nimo::GUID::Create();
                        // }
                        nimo::SceneManager::LoadScene(nimo::AssetManager::GetMetadata(scenePath.string()).id);
                        lastModifiedScene = nimo::AssetManager::GetMetadata(scenePath.string()).id;
                        free(outPath);
                    }
                }
                else
                {
                    NIMO_ERROR("No project loaded to create new scene");
                }
            }
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                if (lastModifiedScene.Valid())
                {
                    nimo::AssetManager::Serialize<nimo::Scene>(lastModifiedScene);
                }
            }
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            if (ImGui::MenuItem("New Project..."))
            {
                showCreateNewProjectModal = true;
            }
            if (ImGui::MenuItem("Open Project..."))
            {
                nfdchar_t* outPath = NULL;
                nfdresult_t result = NFD_OpenDialog("nproj", NULL, &outPath);

                if (result == NFD_OKAY) {
                    openProject(outPath);

                    free(outPath);
                }
            }
            if (ImGui::MenuItem("Save Project"))
            {
                if (nimo::Project::GetActiveProject())
                {
                    nimo::ProjectSerializer projectSer(nimo::Project::GetActiveProject());
                    projectSer.Serialize((nimo::Project::GetActiveProject()->GetSettings().projectDirectory / nimo::Project::GetActiveProject()->GetSettings().name).replace_extension(".nproj"));
                }
            }
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            if (ImGui::MenuItem("Preferences..."))
            {
                m_showPreferencesWindow = true;
            }
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            if (ImGui::MenuItem("Exit"))
            {
                nimo::Application::Instance().Close();
            }
            ImGui::EndMenu();
        }
        if (nimo::Project::GetActiveProject())
        {
            if (ImGui::BeginMenu("Assets"))
            {
                ShowAssetsMenu(nimo::Project::GetActiveProject()->GetAssetsFolderPath(), newNameModal);
                ImGui::EndMenu();
            }
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Scene", NULL, &sceneContentsPanel->open);
            ImGui::MenuItem("Game", NULL, &gameViewPanel->open);
            ImGui::MenuItem("Scene View", NULL, &sceneViewPanel->open);
            ImGui::MenuItem("Inspector", NULL, &inspectorPanel->open);
            ImGui::MenuItem("Asset Explorer", NULL, &assetExplorerPanel->open);
            ImGui::MenuItem("Log", NULL, &logPanel->open);
            ImGui::MenuItem("Renderer Debug", NULL, &rendererDebugPanel->open);
            ImGui::EndMenu();
        }
        if (nimo::Project::GetActiveProject())
        {
            if (ImGui::MenuItem("Launch Game"))
            {
                auto p = nimo::Project::GetActiveProject()->GetAssetIndexPath();
                p.replace_extension(".exe");
                NIMO_INFO("Launching game at: {}", p.string());
                ShellExecuteA(NULL, "open", p.string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
                // additional information
                STARTUPINFOA si;
                PROCESS_INFORMATION pi;

                // set the size of the structures
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                // start the program up
                CreateProcessA((LPCSTR)p.string().c_str(),   // the path
                    (LPSTR)((p.string()).c_str()),        // Command line
                    NULL,           // Process handle not inheritable
                    NULL,           // Thread handle not inheritable
                    FALSE,          // Set handle inheritance to FALSE
                    CREATE_NEW_PROCESS_GROUP | CREATE_NEW_CONSOLE,              // Creation flags
                    NULL,           // Use parent's environment block
                    p.parent_path().string().c_str(),           // Starting directory 
                    &si,            // Pointer to STARTUPINFO structure
                    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
                );
                // Close process and thread handles. 
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }
    ImGui::EndMainMenuBar();

    newNameModal.Show();

    if (m_showPreferencesWindow)
        preferencesWindow->show(m_showPreferencesWindow);

    if (showCreateNewProjectModal)
        ImGui::OpenPopup("Create New Project");

    static std::string projectName = "";
    static std::string projectLocation = "";
    if (ImGui::BeginPopupModal("Create New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        showCreateNewProjectModal = false;
        ImGui::Text("PROJECT SETTINGS\n");
        ImGui::Separator();

        ImGui::InputTextWithHint("##CreateProject##Name", "Project Name", &projectName);
        ImGui::InputTextWithHint("##CreateProject##Location", "Project Location", &projectLocation);
        ImGui::SameLine();
        if (ImGui::Button("...", ImVec2(50, 0))) {
            nfdchar_t* folderPath = NULL;
            nfdresult_t result = NFD_PickFolder(NULL, &folderPath);
            if (result == NFD_OKAY)
            {
                projectLocation = folderPath;
                free(folderPath);
            }
        }

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            CreateNewProject(projectLocation, projectName);
            projectName = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            projectName = "";
            projectLocation = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (nimo::Input::GetKey(nimo::KeyCode::LeftControl) && nimo::Input::GetKeyPressed(nimo::KeyCode::S))
    {
        if (lastModifiedScene.Valid())
        {
            nimo::AssetManager::Serialize<nimo::Scene>(lastModifiedScene);
        }
    }

    m_mustRender = true;
    // If FPS is limited for the current project
    if (EditorLayer::preferences.limitFPS_AppInDev)
    {
        m_cumulativeFrameTime += deltaTime;

        if (m_cumulativeFrameTime < 1 / EditorLayer::FPS_LIMIT)
        {
            m_mustRender = false;
        }
        else
            m_cumulativeFrameTime = 0;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    sceneViewPanel->Render(deltaTime);

    ImGui::SetNextWindowSize(ImVec2(600, 100), ImGuiCond_FirstUseEver);
    logPanel->Render(deltaTime);

    ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_FirstUseEver);
    sceneContentsPanel->Render(deltaTime);

    ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_FirstUseEver);
    inspectorPanel->Render(deltaTime);

    ImGui::SetNextWindowSize(ImVec2(1000, 200), ImGuiCond_FirstUseEver);
    assetExplorerPanel->Render(deltaTime);

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    gameViewPanel->Render(deltaTime);

    ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_FirstUseEver);
    rendererDebugPanel->Render(deltaTime);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void EditorLayer::CreateNewProject(const std::filesystem::path& folder, const std::string& name)
{
    if (!(nimo::FileHandling::Exists(folder) && nimo::FileHandling::IsDirectory(folder)))
    {
        NIMO_ERROR("Path specified for project creation does not exits or it's not a folder: {}", folder.string());
        return;
    }
    auto projectFolderPath = folder / name;
    if (nimo::FileHandling::CreateDiretory(projectFolderPath))
    {
        NIMO_DEBUG("Created directory {}", projectFolderPath.string());
        nimo::ProjectSettings settings;
        settings.name = projectFolderPath.filename().string();
        settings.projectDirectory = projectFolderPath;
        settings.assetDirectory = "Assets";
        settings.assetIndexPath = name + ".nidx";
        settings.logsDirectory = "Logs";
        settings.modulesDirectory = "Modules";
        settings.startScene = "NewScene";
        std::shared_ptr<nimo::Project> project = std::make_shared<nimo::Project>(settings);
        nimo::Project::SetActiveProject(project);
        nimo::Application::Instance().GetWindow().SetTitle(nimo::Project::GetActiveProject()->GetSettings().name + " - Nimo Editor");


        nimo::FileHandling::Copy("NewProjectAssets", projectFolderPath / "Assets");
        nimo::FileHandling::Copy("assimp-vc143-mt.dll", projectFolderPath / "assimp-vc143-mt.dll");
        nimo::FileHandling::Copy("nimo_runtime.exe", projectFolderPath / (name + ".exe"));
        nimo::FileHandling::CreateDiretory(projectFolderPath / "Assets" / "Scenes");
        nimo::FileHandling::CreateDiretory(projectFolderPath / "Assets" / "Materials");
        nimo::FileHandling::CreateDiretory(projectFolderPath / "Assets" / "Prefabs");
        nimo::FileHandling::CreateDiretory(projectFolderPath / "Logs");
        nimo::FileHandling::Copy("modules", projectFolderPath / "Modules");
        nimo::AssetManager::ImportDirectory(projectFolderPath / "Assets");

        // Create bust material
        {
            auto unlitShader = nimo::AssetManager::Get<nimo::Shader>("Shaders/gBuffer.nshader");
            std::vector<nimo::IMaterialProperty*> props;
            {
                auto prop = new nimo::MaterialPropertyTexture();
                prop->name = "albedoMap";
                prop->type = nimo::ShaderUniformDataType::Sampler2D;
                prop->SetTexture(nimo::AssetManager::Get<nimo::Texture>("MarbleBust/textures/marble_bust_01_diff_4k.jpg"));
                prop->SetValue(0);
                props.push_back(prop);
            }
            {
                auto prop = new nimo::MaterialPropertyTexture();
                prop->name = "normalMap";
                prop->type = nimo::ShaderUniformDataType::Sampler2D;
                prop->SetTexture(nimo::AssetManager::Get<nimo::Texture>("MarbleBust/textures/marble_bust_01_nor_dx_4k.jpg"));
                prop->SetValue(1);
                props.push_back(prop);
            }
            {
                auto prop = new nimo::MaterialPropertyTexture();
                prop->name = "armMap";
                prop->type = nimo::ShaderUniformDataType::Sampler2D;
                prop->SetTexture(nimo::AssetManager::Get<nimo::Texture>("MarbleBust/textures/marble_bust_01_arm_4k.jpg"));
                prop->SetValue(2);
                props.push_back(prop);
            }
            std::shared_ptr<nimo::Material> material = std::make_shared<nimo::Material>(unlitShader, props);
            nimo::AssetManager::CreateAssetFromMemory<nimo::Material>("Materials/bust.nmat", material);
        }
        // Create crate material
        {
            auto unlitShader = nimo::AssetManager::Get<nimo::Shader>("Shaders/gBuffer.nshader");
            std::vector<nimo::IMaterialProperty*> props;
            {
                auto prop = new nimo::MaterialPropertyTexture();
                prop->name = "albedoMap";
                prop->type = nimo::ShaderUniformDataType::Sampler2D;
                prop->SetTexture(nimo::AssetManager::Get<nimo::Texture>("Crate/textures/wooden_crate_01_diff_4k.jpg"));
                prop->SetValue(0);
                props.push_back(prop);
            }
            {
                auto prop = new nimo::MaterialPropertyTexture();
                prop->name = "normalMap";
                prop->type = nimo::ShaderUniformDataType::Sampler2D;
                prop->SetTexture(nimo::AssetManager::Get<nimo::Texture>("Crate/textures/wooden_crate_01_nor_dx_4k.jpg"));
                prop->SetValue(1);
                props.push_back(prop);
            }
            {
                auto prop = new nimo::MaterialPropertyTexture();
                prop->name = "armMap";
                prop->type = nimo::ShaderUniformDataType::Sampler2D;
                prop->SetTexture(nimo::AssetManager::Get<nimo::Texture>("Crate/textures/wooden_crate_01_arm_4k.jpg"));
                prop->SetValue(2);
                props.push_back(prop);
            }
            std::shared_ptr<nimo::Material> material = std::make_shared<nimo::Material>(unlitShader, props);
            nimo::AssetManager::CreateAssetFromMemory<nimo::Material>("Materials/crate.nmat", material);
        }

        {
            std::shared_ptr<nimo::Scene> createdScene = nimo::SceneManager::CreateScene("Dummy");
            {
                nimo::Entity e = createdScene->CreateEntity("Bust");
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.0f, -0.2f, 0.0f };
                e.GetComponent<nimo::TransformComponent>().Rotation = { -90.0f, 0.0f, 0.0f };
                e.AddComponent<nimo::MeshComponent>().source = nimo::AssetManager::Get<nimo::Mesh>("MarbleBust/marble_bust_01_4k.fbx");
                e.AddComponent<nimo::MeshRendererComponent>().material = nimo::AssetManager::Get<nimo::Material>("Materials/bust.nmat");
                std::shared_ptr<nimo::Prefab> prefab = std::make_shared<nimo::Prefab>();
                prefab->SetData(createdScene, e);
                nimo::AssetManager::CreateAssetFromMemory<nimo::Prefab>("Prefabs/bust.nprefab", prefab);
                nimo::AssetManager::WriteIndex();
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Crate");
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.0f, -0.55f, -1.0f };
                e.GetComponent<nimo::TransformComponent>().Rotation = { -90.0f, 0.0f, 0.0f };
                e.GetComponent<nimo::TransformComponent>().Scale = { 2.0f, 1.0f, 1.0f };
                e.AddComponent<nimo::MeshComponent>().source = nimo::AssetManager::Get<nimo::Mesh>("Crate/wooden_crate_01_4k.fbx");
                e.AddComponent<nimo::MeshRendererComponent>().material = nimo::AssetManager::Get<nimo::Material>("Materials/crate.nmat");
                std::shared_ptr<nimo::Prefab> prefab = std::make_shared<nimo::Prefab>();
                prefab->SetData(createdScene, e);
                nimo::AssetManager::CreateAssetFromMemory<nimo::Prefab>("Prefabs/crate.nprefab", prefab);
                nimo::AssetManager::WriteIndex();
            }
        }

        {
            std::shared_ptr<nimo::Scene> createdScene = nimo::SceneManager::CreateScene("NewScene");
            {
                auto e = createdScene->CreateEntity("Main Camera");
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.0f, 0.3f, 0.0f };
                e.GetComponent<nimo::TransformComponent>().Rotation = { 20.0f, 0.0f, 0.0f };
                e.AddComponent<nimo::CameraComponent>();
                e.AddComponent<nimo::ScriptComponent>();
                e.GetComponent<nimo::ScriptComponent>().instances.push_back(nimo::ScriptManager::CreateInstance(nimo::AssetManager::Get<nimo::Script>("Scripts/CameraController.lua"), e.ID(), createdScene));
            }
            {
                auto e = createdScene->GetEntity(nimo::AssetManager::Get<nimo::Prefab>("Prefabs/bust.nprefab")->Create(createdScene));
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.0f, -0.2f, -1.0f };
            }
            {
                auto e = createdScene->GetEntity(nimo::AssetManager::Get<nimo::Prefab>("Prefabs/bust.nprefab")->Create(createdScene));
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.5f, -0.2f, -1.0f };
            }
            {
                auto e = createdScene->GetEntity(nimo::AssetManager::Get<nimo::Prefab>("Prefabs/bust.nprefab")->Create(createdScene));
                e.GetComponent<nimo::TransformComponent>().Translation = { -0.5f, -0.2f, -1.0f };
            }
            {
                auto e = createdScene->GetEntity(nimo::AssetManager::Get<nimo::Prefab>("Prefabs/crate.nprefab")->Create(createdScene));
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Point Light");
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.0f, -0.3f, 0.0f };
                auto& light = e.AddComponent<nimo::PointLightComponent>();
                light.Color = { 1.000f, 0.928f, 0.569f };
                light.Intensity = 40.0f;
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Directional Light");
                e.GetComponent<nimo::TransformComponent>().Translation = { -10.0f, 10.0f, 0.0f };
                e.GetComponent<nimo::TransformComponent>().Rotation = { 45.0f, 90.0f, 0.0f };
                auto& light = e.AddComponent<nimo::DirectionalLightComponent>();
                light.Color = { 1.000f, 0.928f, 0.569f };
                light.Intensity = 10.0f;
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Sky Light");
                auto& light = e.AddComponent<nimo::SkyLightComponent>();
                light.environment = nimo::AssetManager::Get<nimo::EnvironmentMap>("Environment/old_room_4k.hdr");
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Music");
                auto& audio = e.AddComponent<nimo::AudioSourceComponent>();
                audio.volume = 0.5f;
                audio.playOnCreate = true;
                audio.loop = true;
                audio.source = nimo::AssetManager::Get<nimo::AudioSource>("Audio/Strength of the Titans.mp3");
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Title");
                e.GetComponent<nimo::TransformComponent>().Translation = { -450.0f, 400.0f, 0.0f };
                auto& text = e.AddComponent<nimo::TextRendererComponent>();
                text.font = nimo::AssetManager::Get<nimo::Font>("Fonts/canterbury/Canterbury.ttf");
                text.text = "Three busts - Demo";
                text.Color = { 1.000f, 0.800f, 0.000f, 0.700f };
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Instructions 1");
                e.GetComponent<nimo::TransformComponent>().Translation = { -900.0f, -430.0f, 0.0f };
                e.GetComponent<nimo::TransformComponent>().Scale = { 0.7f, 0.7f, 0.7f };
                auto& text = e.AddComponent<nimo::TextRendererComponent>();
                text.font = nimo::AssetManager::Get<nimo::Font>("Fonts/canterbury/Canterbury.ttf");
                text.text = "Right click to look around";
            }
            {
                nimo::Entity e = createdScene->CreateEntity("Instructions 2");
                e.GetComponent<nimo::TransformComponent>().Translation = { -900.0f, -500.0f, 0.0f };
                e.GetComponent<nimo::TransformComponent>().Scale = { 0.7f, 0.7f, 0.7f };
                auto& text = e.AddComponent<nimo::TextRendererComponent>();
                text.font = nimo::AssetManager::Get<nimo::Font>("Fonts/canterbury/Canterbury.ttf");
                text.text = "WASD to move";
            }
            nimo::AssetManager::CreateAssetFromMemory<nimo::Scene>("Scenes/NewScene.nscene", createdScene);
            nimo::AssetManager::WriteIndex();
            nimo::SceneManager::SetActiveScene(createdScene);
        }
        {
            std::shared_ptr<nimo::Scene> createdScene = nimo::SceneManager::CreateScene("Second");
            {
                auto e = createdScene->CreateEntity("MainCamera");
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.0f, 0.0f, -0.7f };
                e.AddComponent<nimo::CameraComponent>();
            }
            {
                nimo::Entity e = createdScene->CreateEntity("PointLight");
                e.GetComponent<nimo::TransformComponent>().Translation = { 0.5f, 0.5f, 0.0f };
                auto& light = e.AddComponent<nimo::PointLightComponent>();
                light.Color = { 1.0f,1.0f,1.0f };
                light.Intensity = 5.0f;
            }
            nimo::AssetManager::CreateAssetFromMemory<nimo::Scene>("Scenes/Second.nscene", createdScene);
            nimo::AssetManager::WriteIndex();
        }

        nimo::AssetManager::UnloadUnused();
        nimo::Project::GetActiveProject()->GetSettings().startScene = nimo::AssetManager::Get<nimo::Scene>("Scenes/NewScene.nscene")->id.Str();
        nimo::ProjectSerializer projectSer(project);
        projectSer.Serialize((projectFolderPath / settings.name).replace_extension(".nproj"));
        lastModifiedScene = nimo::AssetManager::Get<nimo::Scene>("Scenes/NewScene.nscene")->id;
        NIMO_DEBUG("Serialized Project {}", settings.name);
        rendererDebugPanel->SetRenderer({});
        m_sceneViewRenderer = std::make_shared<nimo::SceneRenderer>();
        m_sceneViewRenderer->initialize();
        sceneViewPanel->setRenderer(m_sceneViewRenderer);
        m_gameViewRenderer = std::make_shared<nimo::SceneRenderer>();
        m_gameViewRenderer->initialize();
        gameViewPanel->setRenderer(m_gameViewRenderer);
        rendererDebugPanel->SetRenderer(m_gameViewRenderer);
    }
    else {
        NIMO_ERROR("Error Creating directory {}", projectFolderPath.string());
    }
}

void EditorLayer::openProject(nfdchar_t* path)
{
    nimo::ProjectSettings settings;
    std::shared_ptr<nimo::Project> project = std::make_shared<nimo::Project>(settings);
    nimo::ProjectSerializer projectSer(project);
    if (!projectSer.Deserialize(path))
        return;
    nimo::Project::SetActiveProject(project);
    nimo::Application::Instance().GetWindow().SetTitle(nimo::Project::GetActiveProject()->GetSettings().name + " - Nimo Editor");
    nimo::AssetId startingSceneId = nimo::AssetId(nimo::Project::GetActiveProject()->GetSettings().startScene);
    NIMO_DEBUG(nimo::AssetManager::GetMetadata(startingSceneId).filepath.string());
    nimo::SceneManager::LoadScene(startingSceneId);
    lastModifiedScene = startingSceneId;
    nimo::AssetManager::UnloadUnused();
    rendererDebugPanel->SetRenderer({});

    m_sceneViewRenderer = std::make_shared<nimo::SceneRenderer>();
    m_sceneViewRenderer->initialize();
    sceneViewPanel->setRenderer(m_sceneViewRenderer);

    m_gameViewRenderer = std::make_shared<nimo::SceneRenderer>();
    m_gameViewRenderer->initialize();
    gameViewPanel->setRenderer(m_gameViewRenderer);
    rendererDebugPanel->SetRenderer(m_gameViewRenderer);
}
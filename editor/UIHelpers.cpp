#include "UIHelpers.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "assets/AssetManager.h"
#include "shellapi.h"
#include "renderer/Material.h"
#include "core/FileHandling.h"
#include "scene/Scene.h"
#include "project/Project.h"
#include "scene/SceneSerializer.h"
#include "nfd.h"
#include "scripting/Script.h"

void ShowAssetsMenu(const std::filesystem::path& root, ChangeNameModalWindow& modal)
{
    if(nimo::FileHandling::IsDirectory(root))
    {
        if (ImGui::BeginMenu("Create"))
        {
            if(ImGui::MenuItem("Folder"))
            {
                nimo::FileHandling::CreateDiretory(root/"NewFolder");
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Script"))
            {
                NIMO_INFO("Creating new script asset in {}", root.string());
                if(nimo::FileHandling::Copy("NewScript.lua", root/"NewScript.lua"))
                {
                    nimo::AssetManager::Import(root/"NewScript.lua");
                    nimo::AssetManager::WriteIndex();
                }
            }
            if(ImGui::MenuItem("Material"))
            {
                NIMO_INFO("Creating new material asset in {}", root.string());
                std::shared_ptr<nimo::Material> newMaterial = std::make_shared<nimo::Material>(nimo::AssetManager::Get<nimo::Shader>("Shaders/unlit_color.nshader"), std::vector<nimo::IMaterialProperty*>());
                for(auto uniform : nimo::AssetManager::Get<nimo::Shader>("Shaders/unlit_color.nshader")->GetUniforms())
                {
                    if(uniform.name == "transform" || uniform.name == "view" || uniform.name == "projection") continue;
                    newMaterial->AddProperty(uniform.name, uniform.type);
                }
                nimo::AssetManager::CreateAssetFromMemory<nimo::Material>((nimo::AssetManager::GetRelativePath(root)/"NewMaterial.nmat").lexically_normal().string(), newMaterial);
                nimo::AssetManager::WriteIndex();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Scene")){   
                std::shared_ptr<nimo::Scene> createdScene = std::make_shared<nimo::Scene>();
                {
                    auto e = createdScene->CreateEntity("MainCamera");
                    e.GetComponent<nimo::TransformComponent>().Translation = {0.0f, 0.0f, -5.0f};
                    e.AddComponent<nimo::CameraComponent>();
                }
                if(!nimo::FileHandling::Exists(nimo::Project::GetActiveProject()->GetAssetsFolderPath()/"NewScene.nscene"));
                {
                    nimo::AssetManager::CreateAssetFromMemory<nimo::Scene>("NewScene.nscene", createdScene);
                    nimo::AssetManager::WriteIndex();    
                }
            }
            ImGui::EndMenu(); 
        }
        ImGui::Separator();
    }
    if(ImGui::MenuItem("Show in Explorer")){
        ShellExecuteA(NULL, "explore", root.string().c_str(), NULL, NULL, SW_RESTORE);
    }
    if(ImGui::MenuItem("Open")){
		auto absolutePath = std::filesystem::canonical(root);
		ShellExecuteA(NULL, "open", absolutePath.string().c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    if(ImGui::MenuItem("Delete")){
        if(nimo::FileHandling::IsDirectory(root))
        {
            for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(root))
            {
                nimo::AssetManager::Unregister(nimo::AssetManager::GetMetadata(dir_entry.path()).id);
            }
        }
        else{
            nimo::AssetManager::Unregister(nimo::AssetManager::GetMetadata(root).id);
        }
        nimo::FileHandling::Remove(root);
    }
    if(ImGui::MenuItem("Rename")){
        modal.Open(root);
    }
    if(ImGui::MenuItem("Duplicate")){
        std::filesystem::path dest = (root.parent_path()/root.stem()).string() + "_copy" + (root.has_extension() ? root.extension().string() : "");
        nimo::FileHandling::Copy(root, dest);
        nimo::FileHandling::IsDirectory(dest) ? nimo::AssetManager::ImportDirectory(dest) : nimo::AssetManager::Import(dest);
    }
    ImGui::Separator();
    if(ImGui::MenuItem("Import New Asset..."))
    {
        nfdpathset_t pathSet;
        nfdresult_t result = NFD_OpenDialogMultiple( NULL, nimo::FileHandling::IsDirectory(root) ? root.string().c_str() : root.parent_path().string().c_str(), &pathSet );
            
        if ( result == NFD_OKAY ) {
            size_t i;
            for ( i = 0; i < NFD_PathSet_GetCount(&pathSet); ++i )
            {
                nfdchar_t *path = NFD_PathSet_GetPath(&pathSet, i);
                if(nimo::AssetManager::Import(path).Valid())
                    nimo::AssetManager::WriteIndex();
            }
            NFD_PathSet_Free(&pathSet);
        }
    }
}

void ChangeNameModalWindow::Open(const std::filesystem::path& oldPath)
{
    oldNamePath = oldPath;
    mustOpen = true;
}
void ChangeNameModalWindow::Show()
{
    if(mustOpen)
        ImGui::OpenPopup(id.c_str());

    if (ImGui::BeginPopupModal(id.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        mustOpen = false;
        ImGui::Text("New name\n");
        ImGui::Spacing();
        ImGui::Separator();


        ImGui::InputTextWithHint("##CreateProject##Name", "New Name", &newName);
        if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
            ImGui::SetKeyboardFocusHere(-1);

        if (ImGui::Button("OK", ImVec2(120, 0))) 
        {
            finished = true;
            if(newName != "")
            {
                if(nimo::FileHandling::IsDirectory(oldNamePath))
                {
                    std::filesystem::path newPath = oldNamePath.parent_path()/newName;
                    std::vector<std::pair<std::filesystem::path ,nimo::AssetId>> toUpdate;
                    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(oldNamePath))
                    {
                        auto info = nimo::AssetManager::GetMetadata(dir_entry.path());
                        if(info.id.Valid()) // Found in asset manager
                        {
                            toUpdate.push_back(std::make_pair(dir_entry.path().lexically_relative(oldNamePath), info.id));
                        }
                    }
                    if(nimo::FileHandling::Move(oldNamePath, newPath))
                    {
                        for (auto const& toUpdatePair : toUpdate)
                        {
                            nimo::AssetManager::UpdatePath(toUpdatePair.second, newPath/toUpdatePair.first);
                            auto info = nimo::AssetManager::GetMetadata(toUpdatePair.second);
                            NIMO_INFO("Updated filepath in registry {}", info.filepath.string());
                        }
                    }
                }
                else
                {
                    std::filesystem::path newPath = (oldNamePath.parent_path()/newName).replace_extension(oldNamePath.extension());
                    if(nimo::FileHandling::Move(oldNamePath, newPath))
                    {
                        auto info = nimo::AssetManager::GetMetadata(oldNamePath);
                        if(info.id.Valid()) // Found in asset manager
                        {
                            nimo::AssetManager::UpdatePath(info.id, newPath);
                            info = nimo::AssetManager::GetMetadata(info.id);
                            NIMO_INFO("Updated filepath in registry {}", info.filepath.string());
                        }
                    }
                }
            }
            newName = "";
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) 
        { 
            newName = "";
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
    }
}
bool ChangeNameModalWindow::IsFinished()
{
    if(finished)
    {
        finished = false;
        return true;
    }
    return finished;
}
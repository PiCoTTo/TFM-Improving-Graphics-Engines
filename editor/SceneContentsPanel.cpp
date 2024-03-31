#include "SceneContentsPanel.h"
#include "imgui.h"
#include "EditorLayer.h"
#include "core/Log.h"
#include "InspectorPanel.h"
#include "scene/Components.h"
#include "scene/Prefab.h"

void SceneContentsPanel::PaintEntity(const std::shared_ptr<nimo::Scene>& scene, nimo::Entity& ent)
{
    const static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;
    ImGuiTreeNodeFlags node_flags = base_flags;
    if(selectedEntity == ent.GetComponent<nimo::IDComponent>().Id)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    std::string entityName =  ent.GetComponent<nimo::LabelComponent>().Label;
    std::string entityIdString =  ent.GetComponent<nimo::IDComponent>().Id.Str();
    bool open = false;

    // Paint as tree or leaf
    if(ent.GetComponent<nimo::FamilyComponent>().Children.size() != 0) // Has children
    {
        open = ImGui::TreeNodeEx((entityName + "##" + entityIdString + "##" + scene->id.Str()).c_str(), node_flags);
    }
    else // No children, paint as leaf node
    {
        node_flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf; // ImGuiTreeNodeFlags_Bullet
        ImGui::TreeNodeEx((entityName + "##" + entityIdString + "##" + scene->id.Str()).c_str(), node_flags);
    }

    // Popup right click
    bool mustDestroy = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Create Empty")){
            scene->CreateEntityWithParent(ent);
        }
        if (ImGui::MenuItem("Mesh")){
            auto e = scene->CreateEntityWithParent(ent);
            e.GetComponent<nimo::LabelComponent>().Label = "Mesh";
            e.AddComponent<nimo::MeshComponent>();
            e.AddComponent<nimo::MeshRendererComponent>();
        }
        if(ImGui::BeginMenu("Lighting"))
        {
            if (ImGui::MenuItem("Point Light")){
                auto e = scene->CreateEntityWithParent(ent);
                e.GetComponent<nimo::LabelComponent>().Label = "Point Light";
                e.AddComponent<nimo::PointLightComponent>();
            }
            if (ImGui::MenuItem("Directional Light")){
                auto e = scene->CreateEntityWithParent(ent);
                e.GetComponent<nimo::LabelComponent>().Label = "Directional Light";
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Audio"))
        {
            if (ImGui::MenuItem("Audio Source")){
                auto e = scene->CreateEntityWithParent(ent);
                e.GetComponent<nimo::LabelComponent>().Label = "Audio Source";
                e.AddComponent<nimo::AudioSourceComponent>();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("UI"))
        {
            if (ImGui::MenuItem("Text")){
                auto e = scene->CreateEntityWithParent(ent);
                e.GetComponent<nimo::LabelComponent>().Label = "Text";
                e.AddComponent<nimo::TextRendererComponent>();
            }
            if (ImGui::MenuItem("Sprite")){
                auto e = scene->CreateEntityWithParent(ent);
                e.GetComponent<nimo::LabelComponent>().Label = "Sprite";
                e.AddComponent<nimo::SpriteRendererComponent>();
                e.GetComponent<nimo::TransformComponent>().Scale = {100.0f, 100.0f, 1.0f};
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete")){
            mustDestroy = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Create Prefab")){
            auto prefab = std::make_shared<nimo::Prefab>();
            prefab->SetData(scene, ent);
            nimo::AssetManager::CreateAssetFromMemory<nimo::Prefab>(entityName + ".nprefab", prefab);
            nimo::AssetManager::WriteIndex();
        }
        ImGui::EndPopup();
    }
    if(mustDestroy)
    {
        if(open) ImGui::TreePop();
        scene->DestroyEntity(ent);
        m_editor->inspectorPanel->ResetViewItem();
        return;
    }

    // Check if selected
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
    {
        selectedEntity = ent.GetComponent<nimo::IDComponent>().Id;
        m_editor->inspectorPanel->SetViewItem(selectedEntity);
        NIMO_DEBUG("Selected entity: {}", entityName);
    }

    // Drag n drop
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        ImGui::SetDragDropPayload("NIMO_ENTITY_ID", &ent.GetComponent<nimo::IDComponent>().Id, sizeof(nimo::GUID));
        ImGui::Text(entityName.c_str());
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ENTITY_ID"))
        {
            IM_ASSERT(payload->DataSize == sizeof(nimo::GUID));
            nimo::GUID payload_n = *(const nimo::GUID*)payload->Data;
            NIMO_DEBUG("Received drag drop entity: {}", payload_n.Str());
            auto payloadEntity = scene->GetEntity(payload_n);
            if(payloadEntity.GetComponent<nimo::FamilyComponent>().Parent.Valid())
            {
                auto payloadParent = scene->GetEntity(payloadEntity.GetComponent<nimo::FamilyComponent>().Parent);
                NIMO_DEBUG("Removing {} as child from {}", payloadEntity.GetComponent<nimo::LabelComponent>().Label, payloadParent.GetComponent<nimo::LabelComponent>().Label);
                for(int i = 0; i< payloadParent.GetComponent<nimo::FamilyComponent>().Children.size(); ++i)
                {
                    if(payloadParent.GetComponent<nimo::FamilyComponent>().Children[i] == payload_n)
                    {
                        payloadParent.GetComponent<nimo::FamilyComponent>().Children.erase(payloadParent.GetComponent<nimo::FamilyComponent>().Children.begin() + i);
                        break;
                    }
                }
                NIMO_DEBUG("Entiy {} has now {} children", payloadParent.GetComponent<nimo::LabelComponent>().Label, payloadParent.GetComponent<nimo::FamilyComponent>().Children.size());
            }
            payloadEntity.GetComponent<nimo::FamilyComponent>().Parent = ent.GetComponent<nimo::IDComponent>().Id;
            ent.GetComponent<nimo::FamilyComponent>().Children.push_back(payload_n);
        }
        ImGui::EndDragDropTarget();
    }

    // Paint children recursively
    if(open)
    {
        // Paint children
        for(auto child : ent.GetComponent<nimo::FamilyComponent>().Children)
        {
            PaintEntity(scene, scene->GetEntity(child));
        }
        if(ent.GetComponent<nimo::FamilyComponent>().Children.size())
            ImGui::TreePop();
    }
}

void SceneContentsPanel::OnRender(float deltaTime)
{
    ImRect windowRect = { ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax() };
    for(auto scene : nimo::AssetManager::GetAllLoaded<nimo::Scene>())
    {
        float totalHeight = 0;
        ImVec2 rectMin;
        ImVec2 rectMax;
        bool res;
        bool openHeader = ImGui::CollapsingHeader((scene->GetName() + "##" + scene->id.Str()).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            if (ImGui::MenuItem("Create Empty")){
                scene->CreateEntity();
            }
            if (ImGui::MenuItem("Mesh")){
                auto e = scene->CreateEntity();
                e.GetComponent<nimo::LabelComponent>().Label = "Mesh";
                e.AddComponent<nimo::MeshComponent>();
                e.AddComponent<nimo::MeshRendererComponent>();
            }
            if(ImGui::BeginMenu("Lighting"))
            {
                if (ImGui::MenuItem("Point Light")){
                    auto e = scene->CreateEntity();
                    e.GetComponent<nimo::LabelComponent>().Label = "Point Light";
                    e.AddComponent<nimo::PointLightComponent>();
                }
                if (ImGui::MenuItem("Directional Light")){
                    auto e = scene->CreateEntity();
                    e.GetComponent<nimo::LabelComponent>().Label = "Directional Light";
                    e.AddComponent<nimo::DirectionalLightComponent>();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Audio"))
            {
                if (ImGui::MenuItem("Audio Source")){
                    auto e = scene->CreateEntity();
                    e.GetComponent<nimo::LabelComponent>().Label = "Audio Source";
                    e.AddComponent<nimo::AudioSourceComponent>();
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("UI"))
            {
                if (ImGui::MenuItem("Text")){
                    auto e = scene->CreateEntity();
                    e.GetComponent<nimo::LabelComponent>().Label = "Text";
                    e.AddComponent<nimo::TextRendererComponent>();
                }
                if (ImGui::MenuItem("Sprite")){
                    auto e = scene->CreateEntity();
                    e.GetComponent<nimo::LabelComponent>().Label = "Sprite";
                    e.AddComponent<nimo::SpriteRendererComponent>();
                    e.GetComponent<nimo::TransformComponent>().Scale = {100.0f, 100.0f, 1.0f};
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ENTITY_ID"))
            {
                IM_ASSERT(payload->DataSize == sizeof(nimo::GUID));
                nimo::GUID payload_n = *(const nimo::GUID*)payload->Data;
                NIMO_DEBUG("Received drag drop entity: {}", payload_n.Str());
                auto payloadEntity = scene->GetEntity(payload_n);
                if(payloadEntity.GetComponent<nimo::FamilyComponent>().Parent.Valid())
                {
                    auto payloadParent = scene->GetEntity(payloadEntity.GetComponent<nimo::FamilyComponent>().Parent);
                    NIMO_DEBUG("Removing {} as child from {}", payloadEntity.GetComponent<nimo::LabelComponent>().Label, payloadParent.GetComponent<nimo::LabelComponent>().Label);
                    for(int i = 0; i< payloadParent.GetComponent<nimo::FamilyComponent>().Children.size(); ++i)
                    {
                        if(payloadParent.GetComponent<nimo::FamilyComponent>().Children[i] == payload_n)
                        {
                            payloadParent.GetComponent<nimo::FamilyComponent>().Children.erase(payloadParent.GetComponent<nimo::FamilyComponent>().Children.begin() + i);
                            break;
                        }
                    }
                    NIMO_DEBUG("Entiy {} has now {} children", payloadParent.GetComponent<nimo::LabelComponent>().Label, payloadParent.GetComponent<nimo::FamilyComponent>().Children.size());
                }
                payloadEntity.GetComponent<nimo::FamilyComponent>().Parent = nimo::GUID();
            }
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
            {
                std::filesystem::path payloadPath = std::string((char*)payload->Data);
                auto info = nimo::AssetManager::GetMetadata(payloadPath);
                if(info.id.Valid() && info.type == nimo::AssetType::Prefab) // Found in asset manager
                {
                    nimo::AssetManager::Get<nimo::Prefab>(info.id)->Create(scene);
                }
            }
            ImGui::EndDragDropTarget();
        }
        if(openHeader)
        {
            rectMin = ImGui::GetItemRectMin();
            rectMax = ImGui::GetItemRectMax();
            totalHeight += ImGui::GetItemRectSize().y;
            scene->ForEachEntity([&](nimo::Entity& ent)
            {
                if(!ent.GetComponent<nimo::FamilyComponent>().Parent.Valid())
                    this->PaintEntity(scene, ent);
            });
        }
        else{
            totalHeight += ImGui::GetItemRectSize().y;
        }
        res = ImGui::IsMouseHoveringRect(rectMin, ImVec2(rectMax.x, rectMax.y + totalHeight)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        ImGui::Separator();
        if(res)
        {
            m_editor->lastModifiedScene = scene->id;
        }
    }

}

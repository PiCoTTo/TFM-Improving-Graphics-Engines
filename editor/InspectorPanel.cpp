#include "InspectorPanel.h"
#include "imgui.h"
#include "EditorLayer.h"
#include "core/Log.h"
#include "misc/cpp/imgui_stdlib.h"
#include "scripting/ScriptManager.h"
#include "scene/Prefab.h"

void InspectorPanel::SetViewItem(nimo::GUID id)
{
    selectedItem=id;
}
void InspectorPanel::OnRender(float deltaTime)
{
    if(!selectedItem.Valid()) return;
    auto& metadata = nimo::AssetManager::GetMetadata(selectedItem);
    if(metadata.id.Valid())
    {
        ImGui::Image((ImTextureID)(uint64_t)m_editor->assetIcons[metadata.type]->GetInternalId(), ImVec2(48, 48), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();
        ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y +12 });
        ImGui::Text((metadata.filepath.string()).c_str());
        ImGui::TextDisabled(metadata.id.Str().c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        //Show asset widgets based on type
        switch (metadata.type)
        {
        case nimo::AssetType::Material:
            // Show associated shader and properties
            {
                std::shared_ptr<nimo::Material> materialAsset = nimo::AssetManager::Get<nimo::Material>(metadata.id);
                static nimo::AssetMetadata selectedShader;
                ImGui::Spacing();
                ImGui::Text("Shader");
                ImGui::SameLine();
                if(materialAsset->shader)
                    selectedShader = nimo::AssetManager::GetMetadata(materialAsset->shader->id);
                static bool shaderChanged = false;
                if (ImGui::BeginCombo("##Inspector##Asset##Material", selectedShader.filepath.string().c_str(), ImGuiComboFlags_None))
                {
                    if (ImGui::Selectable("PBR"))
                    {
                        selectedShader = nimo::AssetManager::GetMetadata(nimo::AssetManager::Get<nimo::Shader>("Shaders/gBuffer.nshader"));
                        shaderChanged = true;
                    }
                    if (ImGui::Selectable("Unlit color"))
                    {
                        selectedShader = nimo::AssetManager::GetMetadata(nimo::AssetManager::Get<nimo::Shader>("Shaders/unlit_color.nshader"));
                        shaderChanged = true;
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                float alignment = 1.0f;
                const char* label = "\tApply\t";
                ImGuiStyle& style = ImGui::GetStyle();

                float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
                float avail = ImGui::GetContentRegionAvail().x;

                float off = (avail - size) * alignment;
                if (off > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
                if(ImGui::Button(label))
                {
                    nimo::AssetManager::Serialize<nimo::Material>(materialAsset->id);
                }
                if(shaderChanged)
                {
                    shaderChanged = false;
                    nimo::AssetMetadata selectedShaderInfo = nimo::AssetManager::GetMetadata(selectedShader.id);
                    if(selectedShaderInfo.id.Valid())
                    {
                        materialAsset->ClearProperties();
                        for(auto uniform : nimo::AssetManager::Get<nimo::Shader>(selectedShaderInfo.id)->GetUniforms())
                        {
                            if(uniform.name == "transform" || uniform.name == "view" || uniform.name == "projection") continue;
                            NIMO_DEBUG("Adding {} material {} property", metadata.filepath.string(), uniform.name);
                            materialAsset->AddProperty(uniform.name, uniform.type);
                        }
                        NIMO_DEBUG("Serializing {}", metadata.filepath.string());
                        materialAsset->shader = nimo::AssetManager::Get<nimo::Shader>(selectedShaderInfo.id);
                        //nimo::AssetManager::Serialize<nimo::Material>(materialAsset->id);
                    }
                }
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("Properties");
                ImGui::Spacing();
                for(auto p: materialAsset->properties)
                {
                    switch (p->type)
                    {
                    case nimo::ShaderUniformDataType::Float2:
                        {
                            ImGui::DragFloat2((p->name + "##Inspector##Asset##Material1##" +materialAsset->id.Str()).c_str(), (float*)p->GetDataPtr(), 0.01f);
                        }
                        break;
                    case nimo::ShaderUniformDataType::Float3:
                        {
                            ImGui::DragFloat3((p->name + "##Inspector##Asset##Material1##" +materialAsset->id.Str()).c_str(), (float*)p->GetDataPtr(), 0.01f);
                        }
                        break;
                    case nimo::ShaderUniformDataType::Float4:
                        {
                            ImGui::DragFloat4((p->name + "##Inspector##Asset##Material2##" +materialAsset->id.Str()).c_str(), (float*)p->GetDataPtr(), 0.01f);
                        }
                        break;
                    case nimo::ShaderUniformDataType::Sampler2D:
                        {
                            ImGui::InputInt((p->name + "##Inspector##Asset##Material3##TextureSlot##" +materialAsset->id.Str()).c_str(), (int*)p->GetDataPtr());

                            static std::string filepath;
                            if (((nimo::MaterialPropertyTexture*)p)->GetTexture())
                                ImGui::InputTextWithHint((p->name + "##Inspector##Asset##Material4##TextureAsset##" + materialAsset->id.Str()).c_str(), "Drag Texture asset", &nimo::AssetManager::GetMetadata(((nimo::MaterialPropertyTexture*)p)->GetTexture()->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                            else
                                ImGui::InputTextWithHint((p->name + "##Inspector##Asset##Material4##TextureAsset##" + materialAsset->id.Str()).c_str(), "Drag Texture asset", &filepath, ImGuiInputTextFlags_ReadOnly);

                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                {
                                    std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                    auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                    if(info.id.Valid() && info.type == nimo::AssetType::Texture) // Found in asset manager
                                    {
                                        ((nimo::MaterialPropertyTexture*)p)->SetTexture(nimo::AssetManager::Get<nimo::Texture>(info.id));
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }
                        }
                        break;
                    
                    default:
                        break;
                    }
                }
            }
            break;
        case nimo::AssetType::Mesh:
            // Show vertices, submeshes, indices,...
            {
                std::shared_ptr<nimo::Mesh> meshAsset = nimo::AssetManager::Get<nimo::Mesh>(metadata.id);
                auto settings = std::static_pointer_cast<nimo::AssetSettings<nimo::Mesh>>(metadata.serializerSettings);
                ImGui::TextDisabled("Information");
                ImGui::Text("Submeshes: %d", meshAsset->GetSubmeshes().size());
                ImGui::Spacing();
                ImGui::Separator();
                for(auto submesh : meshAsset->GetSubmeshes())
                {
                    ImGui::Text(submesh->m_name.c_str());
                    ImGui::TextDisabled("\tVertices: %d", submesh->m_vertices.size());
                    ImGui::TextDisabled("\tIndices: %d", submesh->m_indices.size());
                }
                ImGui::Spacing();
                ImGui::Separator();
                
                ImGui::TextDisabled("Settings");
                ImGui::Checkbox("Merge meshes with shared material", &settings->mergeMeshesByMaterial);
                if(ImGui::Button("Apply"))
                {
                    nimo::AssetManager::WriteIndex();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("Changes will take place after reloading project");
                ImGui::Spacing();
                ImGui::Separator();
            }
            break;
        case nimo::AssetType::Shader:
            // Show source code for fragment and vertex. Allow for modification
            {
                std::shared_ptr<nimo::Shader> shaderAsset = nimo::AssetManager::Get<nimo::Shader>(metadata.id);
                ImGui::Text("Vertex Shader Code");
                ImGui::SameLine();
                float alignment = 1.0f;
                const char* label = "\tApply\t";
                ImGuiStyle& style = ImGui::GetStyle();

                float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
                float avail = ImGui::GetContentRegionAvail().x;

                float off = (avail - size) * alignment;
                if (off > 0.0f)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
                if(ImGui::Button(label))
                {
                    nimo::AssetManager::Serialize<nimo::Shader>(shaderAsset->id);
                    shaderAsset->Recompile();
                }
                ImGui::Spacing();
                ImGui::InputTextMultiline("##Shader##VertexCode", shaderAsset->GetVertexCodePtr(), ImVec2(ImGui::GetContentRegionAvailWidth(), 500.f), ImGuiInputTextFlags_AllowTabInput);
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::Text("Fragment Shader Code");
                ImGui::Spacing();
                ImGui::InputTextMultiline("##Shader##FragmentCode", shaderAsset->GetFragmentCodePtr(), ImVec2(ImGui::GetContentRegionAvailWidth(), 500.f), ImGuiInputTextFlags_AllowTabInput);
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }
            break;
        case nimo::AssetType::Scene:
            // Show nothing
            break;
        case nimo::AssetType::Texture:
            // Show image, width, height,...
            {
                std::shared_ptr<nimo::Texture> textureAsset = nimo::AssetManager::Get<nimo::Texture>(metadata.id);
                auto settings = std::static_pointer_cast<nimo::AssetSettings<nimo::Texture>>(metadata.serializerSettings);
                ImGui::TextDisabled("Information");
                ImGui::Text("Size: %dx%d", textureAsset->Width(), textureAsset->Height());
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::TextDisabled("Settings");
                ImGui::Checkbox("Generate Mipmaps", &settings->generateMipMaps);
                ImGui::Checkbox("Flip vertically", &settings->flip);
                const char* filteringComboPreview = NULL;
                switch(settings->filtering)
                {
                    case nimo::TextureFiltering::Average:
                        filteringComboPreview = "Average";
                        break;
                    case nimo::TextureFiltering::Nearest:
                        filteringComboPreview = "Nearest";
                        break;
                }

                if(ImGui::BeginCombo("Filtering", filteringComboPreview))
                {
                    if(ImGui::Selectable("Average"))
                    {
                        settings->filtering = nimo::TextureFiltering::Average;
                    }
                    if(ImGui::Selectable("Nearest"))
                    {
                        settings->filtering = nimo::TextureFiltering::Nearest;
                    }
                    ImGui::EndCombo();
                }
                const char* wrappingComboPreview = NULL;
                switch(settings->wrapping)
                {
                    case nimo::TextureWrapping::Repeat:
                        wrappingComboPreview = "Repeat";
                        break;
                    case nimo::TextureWrapping::RepeatMirrored:
                        wrappingComboPreview = "Repeat Mirrored";
                        break;
                    case nimo::TextureWrapping::Clamp:
                        wrappingComboPreview = "Clamp";
                        break;
                }
                if(ImGui::BeginCombo("Wrapping", wrappingComboPreview))
                {
                    if(ImGui::Selectable("Repeat"))
                    {
                        settings->wrapping = nimo::TextureWrapping::Repeat;
                    }
                    if(ImGui::Selectable("Repeat Mirrored"))
                    {
                        settings->wrapping = nimo::TextureWrapping::RepeatMirrored;
                    }
                    if(ImGui::Selectable("Clamp"))
                    {
                        settings->wrapping = nimo::TextureWrapping::Clamp;
                    }
                    ImGui::EndCombo();
                }
                if(ImGui::Button("Apply"))
                {
                    nimo::AssetManager::WriteIndex();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("Changes will take place after reloading project");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextDisabled("Preview");
                float offset = 0.0f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset/2.0f);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset/2.0f);
                ImGui::Image((ImTextureID)(uint64_t)textureAsset->GetInternalId(), ImVec2(ImGui::GetContentRegionAvailWidth() - offset, (ImGui::GetContentRegionAvailWidth()- offset)*(float)textureAsset->Height()/(float)textureAsset->Width()), ImVec2(0, 1), ImVec2(1, 0));
            }
            break;
        case nimo::AssetType::EnvironmentMap:
            // Show nothing
            {
                std::shared_ptr<nimo::EnvironmentMap> textureAsset = nimo::AssetManager::Get<nimo::EnvironmentMap>(metadata.id);
                auto settings = std::static_pointer_cast<nimo::AssetSettings<nimo::EnvironmentMap>>(metadata.serializerSettings);
                ImGui::TextDisabled("Information");
                ImGui::Text("Size: %dx%d", textureAsset->Width(), textureAsset->Height());
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextDisabled("Settings");
                if(ImGui::BeginCombo("Render resolution", std::to_string(settings->renderingResolution).c_str()))
                {
                    if(ImGui::Selectable("128"))
                    {
                        settings->renderingResolution = 128;
                    }
                    if(ImGui::Selectable("256"))
                    {
                        settings->renderingResolution = 256;
                    }
                    if(ImGui::Selectable("512"))
                    {
                        settings->renderingResolution = 512;
                    }
                    if(ImGui::Selectable("1024"))
                    {
                        settings->renderingResolution = 1024;
                    }
                    ImGui::EndCombo();
                }
                if(ImGui::Button("Apply"))
                {
                    nimo::AssetManager::WriteIndex();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("Changes will take place after reloading project");
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextDisabled("Preview");
                float offset = 20.0f;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset/2.0f);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset/2.0f);
                ImGui::Image((ImTextureID)(uint64_t)textureAsset->GetInternalId(), ImVec2(ImGui::GetContentRegionAvailWidth() - offset, (ImGui::GetContentRegionAvailWidth()- offset)*(float)textureAsset->Height()/(float)textureAsset->Width()), ImVec2(0, 1), ImVec2(1, 0));
            }
            break;
        case nimo::AssetType::Audio:
            // Show nothing
            {
                std::shared_ptr<nimo::AudioSource> audio = nimo::AssetManager::Get<nimo::AudioSource>(metadata.id);
                ImGui::Text("Duration: %.3f seconds", audio->GetDuration());
                ImGui::Spacing();
            }
            break;
        case nimo::AssetType::Font:
            // Show nothing
            {
                std::shared_ptr<nimo::Font> font = nimo::AssetManager::Get<nimo::Font>(metadata.id);
                auto settings = std::static_pointer_cast<nimo::AssetSettings<nimo::Font>>(metadata.serializerSettings);
                ImGui::TextDisabled("Settings");
                ImGui::DragInt("Pixel size", (int*)&settings->pixelSize, 1.0f, 0, 128, "%d", ImGuiSliderFlags_AlwaysClamp);
                if(ImGui::Button("Apply"))
                {
                    nimo::AssetManager::WriteIndex();
                }
                ImGui::SameLine();
                ImGui::TextDisabled("Changes will take place after reloading project");
                ImGui::Spacing();
                ImGui::Separator();
            }
            break;
        default:
            break;
        }
        return;
    }
    for(auto scene : nimo::AssetManager::GetAllLoaded<nimo::Scene>())
    {
        auto ent = scene->GetEntity(selectedItem);
        auto entityIdString = ent.GetComponent<nimo::IDComponent>().Id.Str();
        ImGui::Image((ImTextureID)(uint64_t)m_editor->entityIcon->GetInternalId(), ImVec2(48, 48), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();
        ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y +12 });
        static bool activeValue;
        activeValue = ent.GetComponent<nimo::ActiveComponent>().active;
        if(ImGui::Checkbox(("##Active##" + entityIdString).c_str(), &activeValue))
        {
            scene->SetEntityActive(ent, activeValue);
        }
        ImGui::SameLine();
        ImGui::InputText(("##Label##" + entityIdString).c_str(), &ent.GetComponent<nimo::LabelComponent>().Label);
        ImGui::TextDisabled(entityIdString.c_str());
        ImGui::Spacing();
        if(ent.HasComponent<nimo::TransformComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Transform##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                std::string p = "Position##";
                p += entityIdString;
                std::string r = "Rotation##";
                r += entityIdString;
                std::string s = "Scale##";
                s += entityIdString;
                ImGui::DragFloat3(p.c_str(), (float*)&ent.GetComponent<nimo::TransformComponent>().Translation, 0.05f);
                ImGui::DragFloat3(r.c_str(), (float*)&ent.GetComponent<nimo::TransformComponent>().Rotation, 0.25f);
                ImGui::DragFloat3(s.c_str(), (float*)&ent.GetComponent<nimo::TransformComponent>().Scale, 0.1f);
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::CameraComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Camera##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::CameraComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::CameraComponent>())
                {
                    static int item_current_2 = 0;
                    ImGui::Combo((std::string("Projection##") + (std::string("Camera##")+entityIdString)).c_str(), &item_current_2, "Perspective\0Orthographic\0\0");
                    if(item_current_2 == 0)
                    {
                        ent.GetComponent<nimo::CameraComponent>().Projection = nimo::CameraComponent::Projection::Perspective;
                        ImGui::SliderFloat((std::string("FOV##") + (std::string("Camera##")+entityIdString)).c_str(), &ent.GetComponent<nimo::CameraComponent>().FOV, 0.1f, 179.0f);
                    }
                    if(item_current_2 == 1)
                    {
                        ent.GetComponent<nimo::CameraComponent>().Projection = nimo::CameraComponent::Projection::Orthographic;
                    }
                    ImGui::DragFloat((std::string("Clipping plane Far##") + (std::string("Camera##")+entityIdString)).c_str(), &ent.GetComponent<nimo::CameraComponent>().ClippingPlanes.Far, 0.1f, 0.0f);
                    ImGui::DragFloat((std::string("Clipping plane Near##") + (std::string("Camera##")+entityIdString)).c_str(), &ent.GetComponent<nimo::CameraComponent>().ClippingPlanes.Near, 0.1f, 0.0f);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::MeshRendererComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Mesh Renderer##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::MeshRendererComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::MeshRendererComponent>())
                {
                    ImGui::Text("Material");
                    ImGui::SameLine();
                    auto material = ent.GetComponent<nimo::MeshRendererComponent>().material;
                    std::string filepath;
                    if (material)
                        ImGui::InputTextWithHint(("##Asset##Material##" + entityIdString).c_str(), "Drag Material asset", &nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::MeshRendererComponent>().material->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                    else
                        ImGui::InputTextWithHint(("##Asset##Material##" + entityIdString).c_str(), "Drag Material asset", &filepath, ImGuiInputTextFlags_ReadOnly);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                        {
                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                            if(info.id.Valid() && info.type == nimo::AssetType::Material) // Found in asset manager
                            {
                                ent.GetComponent<nimo::MeshRendererComponent>().material = nimo::AssetManager::Get<nimo::Material>(info.id);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::MeshComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Mesh##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::MeshComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::MeshComponent>())
                {
                    // ImGui::Text((std::string("Mesh asset:").c_str()));
                    // ImGui::SameLine();
                    // ImGui::TextDisabled((std::string(nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::MeshComponent>().source->id).filepath.string())).c_str());
                    ImGui::Text("Mesh");
                    ImGui::SameLine();
                    auto mesh = ent.GetComponent<nimo::MeshComponent>().source;
                    std::string filepath;
                    if (mesh)
                        ImGui::InputTextWithHint(("##Asset##Mesh##"+entityIdString).c_str(), "Drag mesh asset", &nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::MeshComponent>().source->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                    else
                        ImGui::InputTextWithHint(("##Asset##Mesh##" + entityIdString).c_str(), "Drag mesh asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                        {
                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                            if(info.id.Valid() && info.type == nimo::AssetType::Mesh) // Found in asset manager
                            {
                                ent.GetComponent<nimo::MeshComponent>().source = nimo::AssetManager::Get<nimo::Mesh>(info.id);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    if (mesh)
                        ImGui::SliderInt(("Submesh##Asset##Mesh##"+entityIdString).c_str(), (int*)&ent.GetComponent<nimo::MeshComponent>().submeshIndex, 0, mesh->GetSubmeshes().size() -1, "%d", ImGuiSliderFlags_AlwaysClamp);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::PointLightComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Point Light##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::PointLightComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::PointLightComponent>())
                {
                    ImGui::ColorEdit3(("Color##Point Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::PointLightComponent>().Color, ImGuiColorEditFlags_Float);
                    ImGui::DragFloat(("Intesity##Point Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::PointLightComponent>().Intensity, 0.1f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::DirectionalLightComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Directional Light##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::DirectionalLightComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::DirectionalLightComponent>())
                {
                    ImGui::ColorEdit3(("Color##Directional Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::DirectionalLightComponent>().Color, ImGuiColorEditFlags_Float);
                    ImGui::DragFloat(("Intesity##Directional Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::DirectionalLightComponent>().Intensity, 0.1f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::SkyLightComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Sky Light##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::SkyLightComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::SkyLightComponent>())
                {
                    auto env = ent.GetComponent<nimo::SkyLightComponent>().environment;
                    std::string filepath;
                    if (env)
                        ImGui::InputTextWithHint(("Environment map##SkyLightComponent##"+entityIdString).c_str(), "Drag environment map asset", &nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::SkyLightComponent>().environment->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                    else
                        ImGui::InputTextWithHint(("Environment map##SkyLightComponent##" + entityIdString).c_str(), "Drag environment map asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                        {
                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                            if(info.id.Valid() && info.type == nimo::AssetType::EnvironmentMap) // Found in asset manager
                            {
                                ent.GetComponent<nimo::SkyLightComponent>().environment = nimo::AssetManager::Get<nimo::EnvironmentMap>(info.id);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::ColorEdit3(("Color##Sky Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::SkyLightComponent>().Color, ImGuiColorEditFlags_Float);
                    ImGui::DragFloat(("Intesity##Sky Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::SkyLightComponent>().Intensity, 0.1f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::SpriteRendererComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Sprite Renderer##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::SpriteRendererComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::SpriteRendererComponent>())
                {
                    auto tex = ent.GetComponent<nimo::SpriteRendererComponent>().texture;
                    std::string filepath;
                    if (tex)
                        ImGui::InputTextWithHint(("Texture##SpriteRendererComponent##"+entityIdString).c_str(), "Drag texture asset", &nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::SpriteRendererComponent>().texture->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                    else
                        ImGui::InputTextWithHint(("Texture##SpriteRendererComponent##" + entityIdString).c_str(), "Drag texture asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                        {
                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                            if(info.id.Valid() && info.type == nimo::AssetType::Texture) // Found in asset manager
                            {
                                ent.GetComponent<nimo::SpriteRendererComponent>().texture = nimo::AssetManager::Get<nimo::Texture>(info.id);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::ColorEdit4(("Color##Point Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::SpriteRendererComponent>().Color, ImGuiColorEditFlags_Float);
                    ImGui::DragFloat2(("Tiling##Point Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::SpriteRendererComponent>().tiling, 0.02f);
                    ImGui::DragFloat2(("Offset##Point Light##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::SpriteRendererComponent>().offset, 0.02f);
                    ImGui::SliderInt(("Layer##Point Light##" + entityIdString).c_str(), &ent.GetComponent<nimo::SpriteRendererComponent>().layer, 0, 9, "%d", ImGuiSliderFlags_AlwaysClamp);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::TextRendererComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Text Renderer##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::TextRendererComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::TextRendererComponent>())
                {
                    ImGui::InputTextMultiline(("Text##TextRenderer##" + entityIdString).c_str(), &ent.GetComponent<nimo::TextRendererComponent>().text);
                    auto tex = ent.GetComponent<nimo::TextRendererComponent>().font;
                    std::string filepath;
                    if (tex)
                        ImGui::InputTextWithHint(("Texture##TextRendererComponent##"+entityIdString).c_str(), "Drag font asset", &nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::TextRendererComponent>().font->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                    else
                        ImGui::InputTextWithHint(("Texture##TextRendererComponent##" + entityIdString).c_str(), "Drag font asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                        {
                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                            if(info.id.Valid() && info.type == nimo::AssetType::Font) // Found in asset manager
                            {
                                ent.GetComponent<nimo::TextRendererComponent>().font = nimo::AssetManager::Get<nimo::Font>(info.id);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::ColorEdit4(("Color##TextRenderer##" + entityIdString).c_str(), (float*)&ent.GetComponent<nimo::TextRendererComponent>().Color, ImGuiColorEditFlags_Float);
                    ImGui::DragFloat(("Character spacing##TextRenderer##" + entityIdString).c_str(), &ent.GetComponent<nimo::TextRendererComponent>().characterSpacing, 0.1f, 0.0f);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::AudioSourceComponent>())
        {
            if (ImGui::CollapsingHeader((std::string("Audio Source##")+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Selectable("Remove component"))
                    {
                        ent.RemoveComponent<nimo::AudioSourceComponent>();
                    }
                    ImGui::EndPopup();
                }
                if(ent.HasComponent<nimo::AudioSourceComponent>())
                {
                    ImGui::Text("Audio source");
                    ImGui::SameLine();
                    auto audio = ent.GetComponent<nimo::AudioSourceComponent>().source;
                    std::string filepath;
                    if (audio)
                        ImGui::InputTextWithHint(("##Asset##AudioSource##"+entityIdString).c_str(), "Drag audio asset", &nimo::AssetManager::GetMetadata(ent.GetComponent<nimo::AudioSourceComponent>().source->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                    else
                        ImGui::InputTextWithHint(("##Asset##AudioSource##" + entityIdString).c_str(), "Drag audio asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                        {
                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                            if(info.id.Valid() && info.type == nimo::AssetType::Audio) // Found in asset manager
                            {
                                ent.GetComponent<nimo::AudioSourceComponent>().source = nimo::AssetManager::Get<nimo::AudioSource>(info.id);
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::SliderFloat(("Volume##AudioSource##"+ entityIdString).c_str(), &ent.GetComponent<nimo::AudioSourceComponent>().volume, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat(("Pitch##AudioSource##"+ entityIdString).c_str(), &ent.GetComponent<nimo::AudioSourceComponent>().pitch, 0.01f, 3.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat(("Pan##AudioSource##"+ entityIdString).c_str(), &ent.GetComponent<nimo::AudioSourceComponent>().pan, -1.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::Checkbox(("Loop##AudioSource##"+ entityIdString).c_str(), &ent.GetComponent<nimo::AudioSourceComponent>().loop);
                    ImGui::Checkbox(("Play on create##AudioSource##"+ entityIdString).c_str(), &ent.GetComponent<nimo::AudioSourceComponent>().playOnCreate);
                }
            }
            ImGui::Spacing();
            ImGui::Separator();
        }
        if(ent.HasComponent<nimo::ScriptComponent>())
        {
            for(auto& instance = ent.GetComponent<nimo::ScriptComponent>().instances.begin(); instance != ent.GetComponent<nimo::ScriptComponent>().instances.end();)
            {
                if (ImGui::CollapsingHeader((std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (ImGui::BeginPopupContextItem())
                    {
                        if(ImGui::Selectable("Remove component"))
                        {
                            nimo::ScriptManager::DestroyInstance(*instance);
                            instance = ent.GetComponent<nimo::ScriptComponent>().instances.erase(instance);
                            ImGui::EndPopup();
                            continue;
                        }
                        ImGui::EndPopup();
                    }
                    for(auto& field : instance->fields)
                    {
                        ImGui::Text(field.first.c_str());
                        ImGui::SameLine();
                        ImGui::Spacing();
                        ImGui::SameLine();
                        switch (field.second->GetType())
                        {
                        case nimo::ScriptFieldType::Number:
                            ImGui::DragFloat(
                                ("##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(),
                                (float*)&std::static_pointer_cast<nimo::ScriptFieldNumber>(field.second)->value);
                            break;
                        case nimo::ScriptFieldType::Boolean:
                            ImGui::Checkbox(
                                ("##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(),
                                &std::static_pointer_cast<nimo::ScriptFieldBool>(field.second)->value);
                            break;
                        case nimo::ScriptFieldType::String:
                            ImGui::InputText(
                                ("##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(),
                                &std::static_pointer_cast<nimo::ScriptFieldString>(field.second)->value);
                            break;
                        case nimo::ScriptFieldType::Entity:
                        {
                            auto fieldEntityId = std::static_pointer_cast<nimo::ScriptFieldEntity>(field.second)->entity;
                            std::string fieldEntityIdString;
                            if (fieldEntityId.Valid())
                                ImGui::InputTextWithHint(("##Entity##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag entity", &scene->GetEntity(fieldEntityId).GetComponent<nimo::LabelComponent>().Label, ImGuiInputTextFlags_ReadOnly);
                            else
                                ImGui::InputTextWithHint(("##Entity##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag entity", &fieldEntityIdString, ImGuiInputTextFlags_ReadOnly);
                          
                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ENTITY_ID"))
                                {
                                    IM_ASSERT(payload->DataSize == sizeof(nimo::GUID));
                                    nimo::GUID payload_n = *(const nimo::GUID*)payload->Data;
                                    NIMO_DEBUG("Received drag drop entity: {}", payload_n.Str());
                                    std::static_pointer_cast<nimo::ScriptFieldEntity>(field.second)->entity = payload_n;
                                }
                                ImGui::EndDragDropTarget();
                            }
                        }
                        break;
                        case nimo::ScriptFieldType::Asset:
                        {
                            auto asset = std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value;
                            switch (std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->type)
                            {
                            case nimo::AssetType::Prefab:
                                {
                                    std::string filepath;
                                    if (asset)
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag prefab asset", &nimo::AssetManager::GetMetadata(asset->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                                    else
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag prefab asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                        {
                                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                            if(info.id.Valid() && info.type == nimo::AssetType::Prefab) // Found in asset manager
                                            {
                                                std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value = std::static_pointer_cast<nimo::Asset>(nimo::AssetManager::Get<nimo::Prefab>(info.id));
                                            }
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                }
                                break;
                            case nimo::AssetType::Material:
                                {
                                    std::string filepath;
                                    if (asset)
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag material asset", &nimo::AssetManager::GetMetadata(asset->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                                    else
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag material asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                        {
                                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                            if(info.id.Valid() && info.type == nimo::AssetType::Material) // Found in asset manager
                                            {
                                                std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value = std::static_pointer_cast<nimo::Asset>(nimo::AssetManager::Get<nimo::Material>(info.id));
                                            }
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                }
                                break;
                            case nimo::AssetType::Mesh:
                                {
                                    std::string filepath;
                                    if (asset)
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag mesh asset", &nimo::AssetManager::GetMetadata(asset->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                                    else
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag mesh asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                        {
                                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                            if(info.id.Valid() && info.type == nimo::AssetType::Mesh) // Found in asset manager
                                            {
                                                std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value = std::static_pointer_cast<nimo::Asset>(nimo::AssetManager::Get<nimo::Mesh>(info.id));
                                            }
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                }
                                break;
                            case nimo::AssetType::Audio:
                                {
                                    std::string filepath;
                                    if (asset)
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag audio asset", &nimo::AssetManager::GetMetadata(asset->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                                    else
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag audio asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                        {
                                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                            if(info.id.Valid() && info.type == nimo::AssetType::Audio) // Found in asset manager
                                            {
                                                std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value = std::static_pointer_cast<nimo::Asset>(nimo::AssetManager::Get<nimo::AudioSource>(info.id));
                                            }
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                }
                                break;
                            case nimo::AssetType::Font:
                                {
                                    std::string filepath;
                                    if (asset)
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag font asset", &nimo::AssetManager::GetMetadata(asset->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                                    else
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag font asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                        {
                                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                            if(info.id.Valid() && info.type == nimo::AssetType::Font) // Found in asset manager
                                            {
                                                std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value = std::static_pointer_cast<nimo::Asset>(nimo::AssetManager::Get<nimo::Font>(info.id));
                                            }
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                }
                                break;
                            case nimo::AssetType::EnvironmentMap:
                                {
                                    std::string filepath;
                                    if (asset)
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag environment map asset", &nimo::AssetManager::GetMetadata(asset->id).filepath.string(), ImGuiInputTextFlags_ReadOnly);
                                    else
                                        ImGui::InputTextWithHint(("##Asset##" + field.first + "##" + (std::filesystem::path(instance->script->m_filepath).stem().string()+"##"+std::to_string(instance->stackReference)+entityIdString)).c_str(), "Drag environment map asset", &filepath, ImGuiInputTextFlags_ReadOnly);
                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NIMO_ASSET_FILE"))
                                        {
                                            std::filesystem::path payloadPath = std::string((char*)payload->Data);
                                            auto info = nimo::AssetManager::GetMetadata(payloadPath);
                                            if(info.id.Valid() && info.type == nimo::AssetType::EnvironmentMap) // Found in asset manager
                                            {
                                                std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value = std::static_pointer_cast<nimo::Asset>(nimo::AssetManager::Get<nimo::EnvironmentMap>(info.id));
                                            }
                                        }
                                        ImGui::EndDragDropTarget();
                                    }
                                }
                                break;
                            default:
                                break;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
                ++instance;
            }
            if(!ent.GetComponent<nimo::ScriptComponent>().instances.size())
                ent.RemoveComponent<nimo::ScriptComponent>();
            ImGui::Spacing();
            ImGui::Separator();
        }
        ImGui::Spacing();

        float alignment = 0.5f;
        const char* label = "\tAdd New Component...\t";
        ImGuiStyle& style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

        if(ImGui::Button(label))
        {
            ImGui::OpenPopup("add_component_popup");
        }
        if (ImGui::BeginPopup("add_component_popup"))
        {
            ImGui::CollapsingHeader("\tComponents...", ImGuiTreeNodeFlags_Leaf);
            ImGui::Separator();
            if (ImGui::MenuItem("Mesh")){
                ent.AddComponent<nimo::MeshComponent>();
            }
            if (ImGui::MenuItem("Mesh Renderer")){
                ent.AddComponent<nimo::MeshRendererComponent>();
            }
            if (ImGui::MenuItem("Sprite Renderer")){
                ent.AddComponent<nimo::SpriteRendererComponent>();
            }
            if (ImGui::MenuItem("Text Renderer")){
                ent.AddComponent<nimo::TextRendererComponent>();
            }
            if (ImGui::MenuItem("Point Light")){
                ent.AddComponent<nimo::PointLightComponent>();
            }
            if (ImGui::MenuItem("Directional Light")){
                ent.AddComponent<nimo::DirectionalLightComponent>();
            }
            if (ImGui::MenuItem("Sky Light")){
                ent.AddComponent<nimo::SkyLightComponent>();
            }
            if (ImGui::MenuItem("Audio Source")){
                ent.AddComponent<nimo::AudioSourceComponent>();
            }
            if (ImGui::MenuItem("Camera")){}
            if(ImGui::BeginMenu("Scripts"))
            {
                auto scripts = nimo::AssetManager::GetAllExisting<nimo::Script>();
                for(int i = 0; i < scripts.size(); ++i)
                {
                    if (ImGui::MenuItem(scripts[i].filepath.stem().string().c_str())){
                        if(!ent.HasComponent<nimo::ScriptComponent>())
                            ent.AddComponent<nimo::ScriptComponent>();
                        ent.GetComponent<nimo::ScriptComponent>().instances.push_back(nimo::ScriptManager::CreateInstance(nimo::AssetManager::Get<nimo::Script>(scripts[i].id), ent.ID(), scene));
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
    }
}

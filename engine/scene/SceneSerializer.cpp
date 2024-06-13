#include "SceneSerializer.h"
#include "json.hpp"
#include "Components.h"
#include <fstream>
#include "project/Project.h"
#include "scripting/ScriptManager.h"
#include "SceneManager.h"
#include "Prefab.h"
#include "renderer/Texture.h"
#include "renderer/Mesh.h"
#include "renderer/Material.h"
#include "renderer/EnvironmentMap.h"
#include "core/ExportedVariablesManager.h"


void nimo::AssetSerializer<nimo::Scene>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Scene>& asset)
{
    nlohmann::ordered_json j;
    nlohmann::ordered_json jentities = nlohmann::ordered_json::array();
    j["Name"] = asset->m_name;
    j["GUID"] = asset->id.Str();

    asset->m_registry.each([&](entt::entity id)
    {
        Entity e(id, asset->m_registry);
        if(!e.GetComponent<FamilyComponent>().Parent.Valid())
        {
            nlohmann::ordered_json jentity = SerializeEntity(asset, e);
            jentities.push_back(jentity);
        }
    });
    j["Entities"] = jentities;

    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    std::ofstream ofs(p/metadata.filepath);
    ofs << j.dump(1, '\t');
}
std::shared_ptr<nimo::Scene> nimo::AssetSerializer<nimo::Scene>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    std::ifstream ifs(p/metadata.filepath);
    nlohmann::ordered_json j;
    j << ifs;

    std::shared_ptr<Scene> scene = SceneManager::CreateScene(metadata.filepath.stem().string());

    scene->id = GUID(std::string(j["GUID"]));
    for(auto entity : j["Entities"])
    {
        DeserializeEntity(scene, entity, GUID(std::string(entity["GUID"])));
    }
    return scene;
}

nlohmann::ordered_json nimo::AssetSerializer<nimo::Scene>::SerializeEntity(const std::shared_ptr<nimo::Scene>& scene, const Entity& e)
{
    nlohmann::ordered_json jentity;
    if(e.HasComponent<IDComponent>())
    {
        jentity["GUID"] = e.GetComponent<IDComponent>().Id.Str();
    }
    if(e.HasComponent<LabelComponent>())
    {
        jentity["Label"] = e.GetComponent<LabelComponent>().Label;
    }
    if(e.HasComponent<ActiveComponent>())
    {
        jentity["Active"] = e.GetComponent<ActiveComponent>().active;
    }
    if(e.HasComponent<FamilyComponent>())
    {
        const FamilyComponent& f = e.GetComponent<FamilyComponent>();
        nlohmann::ordered_json jfamily{
            {"Parent", f.Parent.Str()}
        };
        nlohmann::ordered_json jchildren = nlohmann::ordered_json::array();
        for(auto child : f.Children)
        {
            jchildren.push_back(SerializeEntity(scene, scene->GetEntity(child)));
        }
        jfamily["Children"] = jchildren;
        jentity["Family"] = jfamily;
    }
    if(e.HasComponent<TransformComponent>())
    {
        TransformComponent t = e.GetComponent<TransformComponent>();
        nlohmann::ordered_json jtransform{
            {"Translation", {t.Translation.x, t.Translation.y, t.Translation.z}},
            {"Rotation", {t.Rotation.x, t.Rotation.y, t.Rotation.z}},
            {"Scale", {t.Scale.x, t.Scale.y, t.Scale.z}}
        };
        jentity["Transform"] = jtransform;
    }
    if(e.HasComponent<CameraComponent>())
    {
        CameraComponent c = e.GetComponent<CameraComponent>();
        nlohmann::ordered_json jcamera{
            {"FOV", c.FOV},
            {"ClippingPlanes", {{"Near", c.ClippingPlanes.Near},{"Far", c.ClippingPlanes.Far}}}
        };
        c.Projection == CameraComponent::Projection::Perspective ? jcamera["Projection"] = "Perspective" : jcamera["Projection"] = "Orthographic";
        jentity["Camera"] = jcamera;
    }
    if(e.HasComponent<MeshComponent>())
    {
        MeshComponent m = e.GetComponent<MeshComponent>();
        nlohmann::ordered_json jmesh{
            {"Source", m.source ? m.source->id.Str() : GUID().Str()},
            {"SubmeshIndex", m.submeshIndex}
        };
        jentity["Mesh"] = jmesh;
    }
    if(e.HasComponent<MeshRendererComponent>())
    {
        MeshRendererComponent m = e.GetComponent<MeshRendererComponent>();
        nlohmann::ordered_json jmesh{
            {"Material", m.material ? m.material->id.Str() : GUID().Str()}
        };
        jentity["MeshRenderer"] = jmesh;
    }
    if(e.HasComponent<PointLightComponent>())
    {
        PointLightComponent l = e.GetComponent<PointLightComponent>();
        nlohmann::ordered_json jpointlight{
            {"Color",{l.Color.r,l.Color.g,l.Color.b}},
            {"Intensity",l.Intensity}
        };
        jentity["PointLight"] = jpointlight;
    }
    if(e.HasComponent<DirectionalLightComponent>())
    {
        DirectionalLightComponent l = e.GetComponent<DirectionalLightComponent>();
        nlohmann::ordered_json jdl{
            {"Color",{l.Color.r,l.Color.g,l.Color.b}},
            {"Intensity",l.Intensity}
        };
        jentity["DirectionalLight"] = jdl;
    }
    if(e.HasComponent<SkyLightComponent>())
    {
        SkyLightComponent l = e.GetComponent<SkyLightComponent>();
        nlohmann::ordered_json jdl{
            {"Color",{l.Color.r,l.Color.g,l.Color.b}},
            {"Intensity",l.Intensity},
            {"Environment", l.environment ?  l.environment->id.Str() : GUID().Str()}
        };
        jentity["SkyLight"] = jdl;
    }
    if(e.HasComponent<SpriteRendererComponent>())
    {
        SpriteRendererComponent c = e.GetComponent<SpriteRendererComponent>();
        nlohmann::ordered_json jsr{
            {"Color",{c.Color.r,c.Color.g,c.Color.b,c.Color.a}},
            {"Tiling",{c.tiling.r,c.tiling.g}},
            {"Offset",{c.offset.r,c.offset.g}},
            {"Layer",c.layer},
            {"Texture",c.texture ? c.texture->id.Str() : GUID().Str()}
        };
        jentity["SpriteRenderer"] = jsr;
    }
    if(e.HasComponent<TextRendererComponent>())
    {
        TextRendererComponent c = e.GetComponent<TextRendererComponent>();
        nlohmann::ordered_json jtr{
            {"Color",{c.Color.r,c.Color.g,c.Color.b,c.Color.a}},
            {"Text",c.text},
            {"CharacterSpacing",c.characterSpacing},
            {"Font",c.font ? c.font->id.Str() : GUID().Str()}
        };
        jentity["TextRenderer"] = jtr;
    }
    if(e.HasComponent<AudioSourceComponent>())
    {
        const AudioSourceComponent& a = e.GetComponent<AudioSourceComponent>();
        nlohmann::ordered_json jaudio{
            {"Source", a.source ? a.source->id.Str() : GUID().Str()},
            {"Loop", a.loop},
            {"Volume", a.volume},
            {"PlayOnCreate", a.playOnCreate},
            {"Pitch", a.pitch},
            {"Pan", a.pan}
        };
        jentity["AudioSource"] = jaudio;
    }
    if(e.HasComponent<ScriptComponent>())
    {
        const ScriptComponent& l = e.GetComponent<ScriptComponent>();
        auto instances = nlohmann::json::array();
        for(auto& instance : l.instances)
        {
            auto instanceJson = nlohmann::json({{"Source", instance.script->id.Str()}});
            auto fields = nlohmann::json::array();
            for(auto& field : instance.fields)
            {
                switch (field.second->GetType())
                {
                case nimo::ScriptFieldType::Number:
                    fields.push_back({{"Name",field.first}, {"Value",std::static_pointer_cast<nimo::ScriptFieldNumber>(field.second)->value}, {"Type","Number"}});
                    break;
                case nimo::ScriptFieldType::Boolean:
                    fields.push_back({{"Name",field.first}, {"Value",std::static_pointer_cast<nimo::ScriptFieldBool>(field.second)->value}, {"Type","Boolean"}});
                    break;
                case nimo::ScriptFieldType::String:
                    fields.push_back({{"Name",field.first}, {"Value",std::static_pointer_cast<nimo::ScriptFieldString>(field.second)->value}, {"Type","String"}});
                    break;
                case nimo::ScriptFieldType::Asset:
                    fields.push_back({
                        {"Name",field.first},
                        {"Value",std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value ? std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->value->id.Str() : GUID().Str()},
                        {"Type","Asset"},
                        {"AssetType",AssetTypeToString(std::static_pointer_cast<nimo::ScriptFieldAsset>(field.second)->type)}
                    });
                case nimo::ScriptFieldType::Entity:
                    fields.push_back({
                        {"Name",field.first},
                        {"Value",std::static_pointer_cast<nimo::ScriptFieldEntity>(field.second)->entity.Valid() ? std::static_pointer_cast<nimo::ScriptFieldEntity>(field.second)->entity.Str() : GUID().Str()},
                        {"Type","Entity"},
                    });
                    break;
                default:
                    break;
                }
            }
            instanceJson.push_back({"Fields", fields});
            instances.push_back(instanceJson);
        }
        jentity["Scripts"] = instances;
    }
    return jentity;
}
nimo::GUID nimo::AssetSerializer<nimo::Scene>::DeserializeEntity(const std::shared_ptr<nimo::Scene>& scene, const nlohmann::ordered_json& source, GUID desiredId, GUID parentId)
{
    GUID id;
    if(desiredId.Valid())
        id = desiredId;
    else
        id = GUID::Create();
    nimo::Entity createdEntity = scene->CreateEntityWithID(id);
    bool exportVariables{ false };
    if (source.contains("PointLight") || source.contains("AudioSource") || source.contains("Camera") || source.contains("Mesh") || source.contains("DirectionalLight"))
    {
        exportVariables = true;
    }
    for (auto field : source.items())
    {
        if(field.key() == "Label")
        {
            createdEntity.GetComponent<LabelComponent>().Label = field.value();
        }
        if(field.key() == "Active")
        {
            createdEntity.GetComponent<ActiveComponent>().active = field.value();
            if (exportVariables)
                nimo::ExportedVariablesManager::instance()->addVariable(createdEntity.GetComponent<LabelComponent>().Label + "_Active", createdEntity.GetComponent<ActiveComponent>().active);
        }
        if(field.key() == "Transform")
        {
            auto Translation = glm::vec3((float)field.value()["Translation"][0], (float)field.value()["Translation"][1], (float)field.value()["Translation"][2]);
            auto Rotation = glm::vec3((float)field.value()["Rotation"][0], (float)field.value()["Rotation"][1], (float)field.value()["Rotation"][2]);
            auto Scale = glm::vec3((float)field.value()["Scale"][0], (float)field.value()["Scale"][1], (float)field.value()["Scale"][2]);
            TransformComponent& t = createdEntity.GetComponent<TransformComponent>();
            t.Translation = Translation;
            t.Rotation = Rotation;
            t.Scale = Scale;
            if (exportVariables)
            {
                nimo::ExportedVariablesManager::instance()->addVariable(createdEntity.GetComponent<LabelComponent>().Label + "_Translation", t.Translation);
                nimo::ExportedVariablesManager::instance()->addVariable(createdEntity.GetComponent<LabelComponent>().Label + "_Rotation", t.Rotation);
                nimo::ExportedVariablesManager::instance()->addVariable(createdEntity.GetComponent<LabelComponent>().Label + "_Scale", t.Scale);
            }
        }
        if(field.key() == "Family")
        {
            FamilyComponent& f = createdEntity.GetComponent<FamilyComponent>();
            f.Parent = desiredId.Valid() ? GUID(std::string(field.value()["Parent"])) : parentId;
            for(auto child : field.value()["Children"])
            {
                f.Children.push_back(DeserializeEntity(scene, child, desiredId.Valid() ? GUID(std::string(child["GUID"])) : GUID(), id));
            }
        }
        if(field.key() == "Camera")
        {
            CameraComponent& c = createdEntity.AddComponent<CameraComponent>();
            c.FOV = (float)field.value()["FOV"];
            c.ClippingPlanes.Far = (float)field.value()["ClippingPlanes"]["Far"];
            c.ClippingPlanes.Near = (float)field.value()["ClippingPlanes"]["Near"];
            if(field.value()["Projection"] == "Perspective")
            {
                c.Projection = CameraComponent::Projection::Perspective;
            }
            else {
                c.Projection = CameraComponent::Projection::Orthographic;
            }
        }
        if(field.key() == "Mesh")
        {
            MeshComponent& c = createdEntity.AddComponent<MeshComponent>();
            c.source = AssetId((std::string)field.value()["Source"]).Valid() ? AssetManager::Get<Mesh>(AssetId((std::string)field.value()["Source"])) : std::shared_ptr<Mesh>();
            c.submeshIndex = field.value()["SubmeshIndex"];
        }
        if(field.key() == "MeshRenderer")
        {
            MeshRendererComponent& c = createdEntity.AddComponent<MeshRendererComponent>();
            c.material = AssetId((std::string)field.value()["Material"]).Valid() ? AssetManager::Get<Material>(AssetId((std::string)field.value()["Material"])) : std::shared_ptr<Material>();
        }
        if(field.key() == "PointLight")
        {
            PointLightComponent& l = createdEntity.AddComponent<PointLightComponent>();
            l.Color = glm::vec3((float)field.value()["Color"][0], (float)field.value()["Color"][1], (float)field.value()["Color"][2]);
            l.Intensity = (float)field.value()["Intensity"];
        }
        if(field.key() == "DirectionalLight")
        {
            DirectionalLightComponent& l = createdEntity.AddComponent<DirectionalLightComponent>();
            l.Color = glm::vec3((float)field.value()["Color"][0], (float)field.value()["Color"][1], (float)field.value()["Color"][2]);
            l.Intensity = (float)field.value()["Intensity"];
        }
        if(field.key() == "SkyLight")
        {
            SkyLightComponent& l = createdEntity.AddComponent<SkyLightComponent>();
            l.Color = glm::vec3((float)field.value()["Color"][0], (float)field.value()["Color"][1], (float)field.value()["Color"][2]);
            l.Intensity = (float)field.value()["Intensity"];
            l.environment = AssetId((std::string)field.value()["Environment"]).Valid() ? AssetManager::Get<EnvironmentMap>(AssetId((std::string)field.value()["Environment"])) : std::shared_ptr<EnvironmentMap>();
        }
        if(field.key() == "SpriteRenderer")
        {
            SpriteRendererComponent& c = createdEntity.AddComponent<SpriteRendererComponent>();
            c.Color = glm::vec4((float)field.value()["Color"][0], (float)field.value()["Color"][1], (float)field.value()["Color"][2], (float)field.value()["Color"][3]);
            c.tiling = glm::vec2((float)field.value()["Tiling"][0], (float)field.value()["Tiling"][1]);
            c.offset = glm::vec2((float)field.value()["Offset"][0], (float)field.value()["Offset"][1]);
            c.layer = (int)field.value()["Layer"];
            c.texture = AssetId((std::string)field.value()["Texture"]).Valid() ? AssetManager::Get<Texture>(AssetId((std::string)field.value()["Texture"])) : std::shared_ptr<Texture>();
        }
        if(field.key() == "TextRenderer")
        {
            TextRendererComponent& c = createdEntity.AddComponent<TextRendererComponent>();
            c.Color = glm::vec4((float)field.value()["Color"][0], (float)field.value()["Color"][1], (float)field.value()["Color"][2], (float)field.value()["Color"][3]);
            c.text = field.value()["Text"];
            c.characterSpacing = (float)field.value()["CharacterSpacing"];
            c.font = AssetId((std::string)field.value()["Font"]).Valid() ? AssetManager::Get<Font>(AssetId((std::string)field.value()["Font"])) : std::shared_ptr<Font>();
        }
        if(field.key() == "AudioSource")
        {
            AudioSourceComponent& a = createdEntity.AddComponent<AudioSourceComponent>();
            a.source = AssetId((std::string)field.value()["Source"]).Valid() ? AssetManager::Get<AudioSource>(AssetId((std::string)field.value()["Source"])) : std::shared_ptr<AudioSource>();
            a.loop = (bool)field.value()["Loop"];
            a.volume = (float)field.value()["Volume"];
            a.playOnCreate = (bool)field.value()["PlayOnCreate"];
            a.pitch = (float)field.value()["Pitch"];
            a.pan = (float)field.value()["Pan"];
        }
        if(field.key() == "Scripts")
        {
            ScriptComponent& l = createdEntity.AddComponent<ScriptComponent>();
            for(auto i : field.value())
            {
                auto instance = ScriptManager::CreateInstance(AssetManager::Get<Script>(GUID(std::string(i["Source"]))), createdEntity.ID(), scene);
                for(auto field : i ["Fields"])
                {
                    auto it = instance.fields.find(field["Name"]);
                    auto value = field["Value"];
                    auto type = field["Type"];
                    if(it != instance.fields.end())
                    {
                        if(type == "Number" && it->second->GetType() == ScriptFieldType::Number)
                        {
                            std::static_pointer_cast<nimo::ScriptFieldNumber>(instance.fields[field["Name"]])->value = value;
                        }
                        if(type == "Boolean" && it->second->GetType() == ScriptFieldType::Boolean)
                        {
                            std::static_pointer_cast<nimo::ScriptFieldBool>(instance.fields[field["Name"]])->value = value;
                        }
                        if(type == "String" && it->second->GetType() == ScriptFieldType::String)
                        {
                            std::static_pointer_cast<nimo::ScriptFieldString>(instance.fields[field["Name"]])->value = value;
                        }
                        if(type == "Entity" && it->second->GetType() == ScriptFieldType::Entity)
                        {
                            std::static_pointer_cast<nimo::ScriptFieldEntity>(instance.fields[field["Name"]])->entity = GUID(std::string(value));
                        }
                        if(type == "Asset" && it->second->GetType() == ScriptFieldType::Asset)
                        {
                            if(value != "")
                            {
                                switch(AssetManager::GetMetadata(GUID(std::string(value))).type)
                                {
                                case AssetType::Prefab:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<Prefab>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                case AssetType::Texture:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<Texture>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                case AssetType::EnvironmentMap:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<EnvironmentMap>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                case AssetType::Mesh:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<Mesh>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                case AssetType::Material:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<Material>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                case AssetType::Script:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<Script>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                case AssetType::Font:
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->value = std::static_pointer_cast<nimo::Asset>(AssetManager::Get<Font>(GUID(std::string(value))));
                                    std::static_pointer_cast<nimo::ScriptFieldAsset>(instance.fields[field["Name"]])->type = AssetTypeFromString(field["AssetType"]);
                                    break;
                                default:
                                    break;
                                }                                
                            }
                        }
                    }
                }
                ScriptManager::ApplyFields(instance);
                if(createdEntity.GetComponent<ActiveComponent>().active)
                    ScriptManager::OnEnable(instance);
                l.instances.push_back(instance);
            }
        }
    }
    return createdEntity.GetComponent<IDComponent>().Id;
}
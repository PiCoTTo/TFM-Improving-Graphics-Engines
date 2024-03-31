#include "AssetSerializer.h"

#include "core/Log.h"
#include "renderer/EnvironmentMap.h"
#include "renderer/Texture.h"
#include "renderer/Shader.h"
#include "renderer/Material.h"
#include "renderer/Mesh.h"
#include "project/Project.h"
#include "scripting/Script.h"
#include "scene/Prefab.h"
#include "audio/AudioSource.h"
#include "AssetSettings.h"

#include <fstream>

//Texture
void nimo::AssetSerializer<nimo::Texture>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Texture>& asset){}
std::shared_ptr<nimo::Texture> nimo::AssetSerializer<nimo::Texture>::Deserialize(const nimo::AssetMetadata& metadata)
{ 
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    auto settings = std::static_pointer_cast<AssetSettings<Texture>>(metadata.serializerSettings);
    TextureSpecification spec;
    spec.generateMipmaps = settings->generateMipMaps;
    spec.filtering = settings->filtering;
    spec.wrapping = settings->wrapping;
    return std::make_shared<Texture>((p/metadata.filepath).string(), settings->flip, spec);
}

//EnvironmentMap
void nimo::AssetSerializer<nimo::EnvironmentMap>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::EnvironmentMap>& asset){}
std::shared_ptr<nimo::EnvironmentMap> nimo::AssetSerializer<nimo::EnvironmentMap>::Deserialize(const nimo::AssetMetadata& metadata)
{ 
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    auto settings = std::static_pointer_cast<AssetSettings<EnvironmentMap>>(metadata.serializerSettings);
    return std::make_shared<EnvironmentMap>((p/metadata.filepath).string(), settings->renderingResolution);
}

//Shader
void nimo::AssetSerializer<nimo::Shader>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Shader>& asset)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    std::ofstream ofs(p/metadata.filepath);

    ofs << "@VERTEX";
    ofs << asset->GetVertexCode();
    ofs << "@VERTEXEND";
    ofs << "@FRAGMENT";
    ofs << asset->GetFragmentCode();
    ofs << "@FRAGMENTEND";
}
std::shared_ptr<nimo::Shader> nimo::AssetSerializer<nimo::Shader>::Deserialize(const nimo::AssetMetadata& metadata)
{     
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    return std::make_shared<Shader>((p/metadata.filepath).string());
}

//Material
void nimo::AssetSerializer<nimo::Material>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Material>& asset)
{
    nlohmann::json j;
    j["shader"] = asset->shader->id.Str();
    nlohmann::json jprops = nlohmann::json::array();
    for(auto p : asset->properties)
    {
        switch(p->type)
        {
            case ShaderUniformDataType::Float2:
            {
                auto pv2 = static_cast<MaterialProperty<glm::vec2>*>(p);
                jprops.push_back({
                    {"name", pv2->name},
                    {"type", "vector2"},
                    {"value", {((glm::vec2*)pv2->GetDataPtr())->x, ((glm::vec2*)pv2->GetDataPtr())->y}}
                });
            }
            break;
            case ShaderUniformDataType::Float3:
            {
                auto pv3 = static_cast<MaterialProperty<glm::vec3>*>(p);
                jprops.push_back({
                    {"name", pv3->name},
                    {"type", "vector3"},
                    {"value", {((glm::vec3*)pv3->GetDataPtr())->x, ((glm::vec3*)pv3->GetDataPtr())->y, ((glm::vec3*)pv3->GetDataPtr())->z}}
                });
            }
            break;
            case ShaderUniformDataType::Float4:
            {
                auto pv2 = static_cast<MaterialProperty<glm::vec4>*>(p);
                jprops.push_back({
                    {"name", pv2->name},
                    {"type", "vector4"},
                    {"value", {((glm::vec4*)pv2->GetDataPtr())->x, ((glm::vec4*)pv2->GetDataPtr())->y, ((glm::vec4*)pv2->GetDataPtr())->z, ((glm::vec4*)pv2->GetDataPtr())->w}}
                });
            }
            break;
            case ShaderUniformDataType::Sampler2D:
            {
                auto pt = static_cast<MaterialPropertyTexture*>(p);
                jprops.push_back({
                    {"name", pt->name},
                    {"type", "texture"},
                    {"value", pt->GetValue()},
                    {"asset", pt->GetTexture() ? pt->GetTexture()->id.Str() : GUID().Str()}
                });
            }
            break;
        }
    }
    j["properties"] = jprops;
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    std::ofstream ofs(p/metadata.filepath);
    ofs << j.dump(4);
}
std::shared_ptr<nimo::Material> nimo::AssetSerializer<nimo::Material>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    return std::make_shared<Material>((p/metadata.filepath).string());
}

//Mesh
void nimo::AssetSerializer<nimo::Mesh>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Mesh>& asset){}
std::shared_ptr<nimo::Mesh> nimo::AssetSerializer<nimo::Mesh>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    auto settings = std::static_pointer_cast<AssetSettings<Mesh>>(metadata.serializerSettings);
    return std::make_shared<Mesh>((p/metadata.filepath).string(), settings->mergeMeshesByMaterial);
}

// Script
void nimo::AssetSerializer<nimo::Script>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Script>& asset){}
std::shared_ptr<nimo::Script> nimo::AssetSerializer<nimo::Script>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    return std::make_shared<Script>((p/metadata.filepath).string());
}

// Script
void nimo::AssetSerializer<nimo::Prefab>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Prefab>& asset)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    std::ofstream ofs(p/metadata.filepath);
    ofs << asset->m_source.dump(4);
}
std::shared_ptr<nimo::Prefab> nimo::AssetSerializer<nimo::Prefab>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    return std::make_shared<Prefab>((p/metadata.filepath).string());
}

void nimo::AssetSerializer<nimo::AudioSource>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::AudioSource>& asset){}
std::shared_ptr<nimo::AudioSource> nimo::AssetSerializer<nimo::AudioSource>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    return std::make_shared<AudioSource>((p/metadata.filepath).string());
}
void nimo::AssetSerializer<nimo::Font>::Serialize(const AssetMetadata& metadata, const std::shared_ptr<nimo::Font>& asset){}
std::shared_ptr<nimo::Font> nimo::AssetSerializer<nimo::Font>::Deserialize(const nimo::AssetMetadata& metadata)
{
    std::filesystem::path p = ".";
    if(nimo::Project::GetActiveProject())
        p = nimo::Project::GetActiveProject()->GetAssetsFolderPath();
    auto settings = std::static_pointer_cast<AssetSettings<Font>>(metadata.serializerSettings);
    return std::make_shared<Font>((p/metadata.filepath).string(), settings->pixelSize);
}

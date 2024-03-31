#include "AssetManager.h"
#include "AssetFileExtensions.h"
#include "AssetSettingsSerializer.h"
#include <fstream>
#include <iostream>
#include "core/Log.h"
#include "core/FileHandling.h"

#include "project/Project.h"

nimo::AssetIndex nimo::AssetManager::m_index = nimo::AssetIndex();
std::unordered_map<std::type_index, std::unordered_map<nimo::AssetId, std::shared_ptr<nimo::Asset>>> nimo::AssetManager::m_loadedAssets = {};

void nimo::AssetManager::Initialize()
{
    LoadAssetIndex(Project::GetActiveProject()->GetAssetIndexPath().string());
}
void nimo::AssetManager::Cleanup()
{
    m_index.clear();
    m_loadedAssets.clear();
}

void nimo::AssetManager::LoadAssetIndex(const std::string& filename)
{
    NIMO_DEBUG("Loading AssetIndex from {}", filename);
    if(FileHandling::Exists(filename)){
        std::ifstream fin(filename);
        nlohmann::json j;
        j << fin;
        for(auto o : j["assets"])
        {
            AssetMetadata metadata;
            metadata.id = GUID(std::string(o["guid"]));
            metadata.filepath = std::string(o["filepath"]);
            metadata.type = AssetTypeFromString(std::string(o["type"]));
            switch (metadata.type)
            {
            case AssetType::Texture:
                {
                    AssetSettingsSerializer<Texture> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Prefab:
                {
                    AssetSettingsSerializer<Prefab> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::EnvironmentMap:
                {
                    AssetSettingsSerializer<EnvironmentMap> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Shader:
                {
                    AssetSettingsSerializer<Shader> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Mesh:
                {
                    AssetSettingsSerializer<Mesh> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Material:
                {
                    AssetSettingsSerializer<Material> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Script:
                {
                    AssetSettingsSerializer<Script> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Scene:
                {
                    AssetSettingsSerializer<Scene> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Audio:
                {
                    AssetSettingsSerializer<AudioSource> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            case AssetType::Font:
                {
                    AssetSettingsSerializer<Font> settingsSerializer;
                    metadata.serializerSettings = std::static_pointer_cast<IAssetSettings>(settingsSerializer.Deserialize(o["settings"]));
                }
                break;
            default:
                break;
            }
            NIMO_DEBUG("\t- {}", o.dump());
            if(metadata.id.Valid() && FileHandling::Exists(Project::GetActiveProject()->GetAssetsFolderPath()/metadata.filepath))
                m_index[metadata.id] = metadata;
        }
    }
    else{
        WriteIndex();
    }
}
void nimo::AssetManager::WriteIndex()
{
    nlohmann::json j;
    nlohmann::json assetsjson = nlohmann::json::array();

    int i = 0;
    for(auto a : m_index)
    {
        if(!a.second.id.Valid() || !FileHandling::Exists(Project::GetActiveProject()->GetAssetsFolderPath()/a.second.filepath))
            continue;
        nlohmann::json settings;
        switch (a.second.type)
        {
        case AssetType::Texture:
            {
                AssetSettingsSerializer<Texture> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Texture>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Prefab:
            {
                AssetSettingsSerializer<Prefab> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Prefab>>(a.second.serializerSettings));
            }
            break;
        case AssetType::EnvironmentMap:
            {
                AssetSettingsSerializer<EnvironmentMap> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<EnvironmentMap>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Shader:
            {
                AssetSettingsSerializer<Shader> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Shader>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Mesh:
            {
                AssetSettingsSerializer<Mesh> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Mesh>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Material:
            {
                AssetSettingsSerializer<Material> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Material>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Script:
            {
                AssetSettingsSerializer<Script> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Script>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Scene:
            {
                AssetSettingsSerializer<Scene> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Scene>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Audio:
            {
                AssetSettingsSerializer<AudioSource> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<AudioSource>>(a.second.serializerSettings));
            }
            break;
        case AssetType::Font:
            {
                AssetSettingsSerializer<Font> settingsSerializer;
                settings = settingsSerializer.Serialize(std::static_pointer_cast<AssetSettings<Font>>(a.second.serializerSettings));
            }
            break;
        default:
            break;
        }
        assetsjson[i] = {{"guid",a.second.id.Str()}, {"filepath",a.second.filepath.lexically_normal().string()}, {"type", AssetTypeToString(a.second.type)}, {"settings", settings}};
        i++;
    }
    j["assets"] = assetsjson;
    std::ofstream fout(Project::GetActiveProject()->GetAssetIndexPath());
    fout << j.dump(4);
}

const nimo::AssetMetadata& nimo::AssetManager::GetMetadata(nimo::AssetId id)
{
    return m_index[id];
}

const nimo::AssetMetadata& nimo::AssetManager::GetMetadata(const std::filesystem::path& path)
{
    const auto relativePath = GetRelativePath(path);

    for (auto& [id, metadata] : m_index)
    {
        if (metadata.filepath == relativePath)
            return metadata;
    }

    return {};
}

std::filesystem::path nimo::AssetManager::GetRelativePath(const std::filesystem::path& filepath)
{
    std::filesystem::path relativePath = filepath.lexically_normal();
    std::string temp = filepath.string();
    if (temp.find(Project::GetAssetsFolderPath().string()) != std::string::npos)
    {
        relativePath = std::filesystem::relative(filepath, Project::GetAssetsFolderPath());
        if (relativePath.empty())
        {
            relativePath = filepath.lexically_normal();
        }
    }
    return relativePath;
}

void nimo::AssetManager::UpdatePath(AssetId id, const std::filesystem::path& newPath)
{
    AssetMetadata& metadata = GetMetadataRef(id);
    if (!metadata.id.Valid())
        return;
    metadata.filepath = GetRelativePath(newPath);
    WriteIndex();
}

nimo::AssetMetadata& nimo::AssetManager::GetMetadataRef(AssetId id)
{
    return m_index[id];
}

void nimo::AssetManager::Unregister(AssetId id)
{
    AssetMetadata info = GetMetadata(id);
    if (!info.id.Valid())
        return;

    m_index.remove(id);
    for(auto [type,map] : m_loadedAssets)
    {
        map.erase(id);
    }
    NIMO_INFO("Unregistering {} with id {} from AssetIndex", info.filepath.string(), info.id.Str());
    WriteIndex();
}

nimo::AssetId nimo::AssetManager::Import(const std::filesystem::path& filepath)
{
    std::filesystem::path path = GetRelativePath(filepath);
    auto info = GetMetadata(path);

    if(info.id.Valid())
    {
        NIMO_INFO("Asset {}({}) already imported with id: {}", info.filepath.string(), AssetTypeToString(info.type), info.id.Str());
        return info.id;
    }

    if(!filepath.has_extension())
        return info.id;

    AssetType type = AssetFileExtensions::GetTypeFromExtension(path.extension().string());
    NIMO_INFO("Importing asset {} as {}", path.string(), AssetTypeToString(type));
    if (type == AssetType::None)
        return AssetId();

    AssetMetadata metadata;
    metadata.id = AssetId::Create();
    metadata.filepath = path;
    metadata.type = type;
    metadata.serializerSettings = CreateAssetSettings(type);
    m_index[metadata.id] = metadata;

    return metadata.id;
}

void nimo::AssetManager::ImportDirectory(const std::filesystem::path& folderpath, bool includeSubdirectories)
{
    if(!FileHandling::Exists(folderpath) || !FileHandling::IsDirectory(folderpath))
    {
        NIMO_WARN("Trying to import {}, as it may not exist or be a directory.", folderpath.string());
        return;
    }
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(folderpath))
    {
        if(FileHandling::IsDirectory(dir_entry.path()) && includeSubdirectories)
        {
            ImportDirectory(dir_entry.path());
        }
        else
        {
            Import(dir_entry.path());
        }
    }
}

void nimo::AssetManager::UnloadUnused()
{
    bool needRecurse = false;
    for(auto& [type,map] : m_loadedAssets)
    {
        std::vector<GUID> toUnload;
        for(auto& [id, asset] : map)
        {
            NIMO_DEBUG("Asset loaded {} - ref count: {}", id.Str(), asset.use_count());
            if(asset.use_count() == 1)
            {
                toUnload.push_back(id);
            }
        }
        for(auto id : toUnload)
        {
            map.erase(id);
            needRecurse = true;
        }
    }
    if(needRecurse)
    {
        UnloadUnused();
    }
}

std::shared_ptr<nimo::IAssetSettings> nimo::AssetManager::CreateAssetSettings(AssetType type)
{
    switch (type)
    {
    case AssetType::Texture:
        {
            AssetSettingsSerializer<Texture> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Texture>>());
        }
        break;
    case AssetType::Prefab:
        {
            AssetSettingsSerializer<Prefab> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Prefab>>());
        }
        break;
    case AssetType::EnvironmentMap:
        {
            AssetSettingsSerializer<EnvironmentMap> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<EnvironmentMap>>());
        }
        break;
    case AssetType::Shader:
        {
            AssetSettingsSerializer<Shader> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Shader>>());
        }
        break;
    case AssetType::Mesh:
        {
            AssetSettingsSerializer<Mesh> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Mesh>>());
        }
        break;
    case AssetType::Material:
        {
            AssetSettingsSerializer<Material> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Material>>());
        }
        break;
    case AssetType::Script:
        {
            AssetSettingsSerializer<Script> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Script>>());
        }
        break;
    case AssetType::Scene:
        {
            AssetSettingsSerializer<Scene> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Scene>>());
        }
        break;
    case AssetType::Audio:
        {
            AssetSettingsSerializer<AudioSource> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<AudioSource>>());
        }
        break;
    case AssetType::Font:
        {
            AssetSettingsSerializer<Font> settingsSerializer;
            return std::static_pointer_cast<IAssetSettings>(std::make_shared<AssetSettings<Font>>());
        }
        break;
    default:
        break;
    }
    
}




#pragma once 

#include <string>

namespace nimo
{
    enum class AssetType : unsigned int
	{
		None = 0,
        Texture,
        Mesh,
        Shader,
        Material,
        Scene,
        EnvironmentMap,
        Script,
        Prefab,
        Audio,
        Font
	};

    inline AssetType AssetTypeFromString(const std::string& str){
        if (str == "None")			return AssetType::None;
        if (str == "Texture")			return AssetType::Texture;
        if (str == "Mesh")			return AssetType::Mesh;
        if (str == "Shader")			return AssetType::Shader;
        if (str == "Material")			return AssetType::Material;
        if (str == "Scene")			return AssetType::Scene;
        if (str == "EnvironmentMap")			return AssetType::EnvironmentMap;
        if (str == "Script")			return AssetType::Script;
        if (str == "Prefab")			return AssetType::Prefab;
        if (str == "Audio")			return AssetType::Audio;
        if (str == "Font")			return AssetType::Font;
        return AssetType::None;
    }

    inline const char* AssetTypeToString(AssetType type)
    {
        switch (type)
        {
            case AssetType::None:			return "None"; break;
            case AssetType::Texture:		return "Texture"; break;
            case AssetType::Mesh:		return "Mesh"; break;
            case AssetType::Shader:		return "Shader"; break;
            case AssetType::Material:		return "Material"; break;
            case AssetType::Scene:		return "Scene"; break;
            case AssetType::EnvironmentMap:		return "EnvironmentMap"; break;
            case AssetType::Script:		return "Script"; break;
            case AssetType::Prefab:		return "Prefab"; break;
            case AssetType::Audio:		return "Audio"; break;
            case AssetType::Font:		return "Font"; break;
            default:  break;
        }
        return "None";
    }
    
} // namespace nimo

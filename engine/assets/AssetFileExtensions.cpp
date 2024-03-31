#include "AssetFileExtensions.h"

std::unordered_map<std::string, nimo::AssetType> nimo::AssetFileExtensions::m_extensions = 
{
    //Scene
    {".nscene", AssetType::Scene},
    //Material
    {".nmat", AssetType::Material},
    //Shader
    {".nshader", AssetType::Shader},
    //Texture
    { ".png", AssetType::Texture },
    { ".jpg", AssetType::Texture },
    { ".jpeg", AssetType::Texture },
    // Environment 
    { ".hdr", AssetType::EnvironmentMap},
    //Mesh
    {".obj", AssetType::Mesh},
    {".fbx", AssetType::Mesh},
    {".gltf", AssetType::Mesh},
    // Script
    {".lua", AssetType::Script},
    // Prefab
    {".nprefab", AssetType::Prefab},
    // Audio
    {".wav", AssetType::Audio},
    {".mp3", AssetType::Audio},
    {".ogg", AssetType::Audio},
    // Font
    {".ttf", AssetType::Font},

};


nimo::AssetType nimo::AssetFileExtensions::GetTypeFromExtension(std::string_view extension)
{
    return m_extensions[extension.data()];
}
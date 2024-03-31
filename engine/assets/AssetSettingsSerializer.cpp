#include "AssetSettingsSerializer.h"
#include "core/Log.h"

nlohmann::json nimo::AssetSettingsSerializer<nimo::Texture>::Serialize(std::shared_ptr<nimo::AssetSettings<Texture>> settings)
{
    return {{"GenerateMipMaps", settings->generateMipMaps}, {"Flip", settings->flip}, {"Wrapping", (int)settings->wrapping}, {"Filtering", (int)settings->filtering}};
}
std::shared_ptr<nimo::AssetSettings<nimo::Texture>> nimo::AssetSettingsSerializer<nimo::Texture>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Texture>>();
    settings->generateMipMaps = data["GenerateMipMaps"];
    settings->flip = data["Flip"];
    settings->filtering = (TextureFiltering)(int)data["Filtering"];
    settings->wrapping = (TextureWrapping)(int)data["Wrapping"];
    return settings;
}

nlohmann::json nimo::AssetSettingsSerializer<nimo::Shader>::Serialize(std::shared_ptr<nimo::AssetSettings<Shader>> settings)
{
    return {};
}
std::shared_ptr<nimo::AssetSettings<nimo::Shader>> nimo::AssetSettingsSerializer<nimo::Shader>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Shader>>();
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::Material>::Serialize(std::shared_ptr<nimo::AssetSettings<Material>> settings)
{
    return {};
}
std::shared_ptr<nimo::AssetSettings<nimo::Material>> nimo::AssetSettingsSerializer<nimo::Material>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Material>>();
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::Mesh>::Serialize(std::shared_ptr<nimo::AssetSettings<Mesh>> settings)
{
    return {{"MergeMeshesByMaterial", settings->mergeMeshesByMaterial}};
}
std::shared_ptr<nimo::AssetSettings<nimo::Mesh>> nimo::AssetSettingsSerializer<nimo::Mesh>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Mesh>>();
    settings->mergeMeshesByMaterial = data["MergeMeshesByMaterial"];
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::EnvironmentMap>::Serialize(std::shared_ptr<nimo::AssetSettings<EnvironmentMap>> settings)
{
    return {{"RenderingResolution", settings->renderingResolution}};
}
std::shared_ptr<nimo::AssetSettings<nimo::EnvironmentMap>> nimo::AssetSettingsSerializer<nimo::EnvironmentMap>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::EnvironmentMap>>();
    settings->renderingResolution = data["RenderingResolution"];
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::Script>::Serialize(std::shared_ptr<nimo::AssetSettings<Script>> settings)
{
    return {};
}
std::shared_ptr<nimo::AssetSettings<nimo::Script>> nimo::AssetSettingsSerializer<nimo::Script>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Script>>();
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::Prefab>::Serialize(std::shared_ptr<nimo::AssetSettings<Prefab>> settings)
{
    return {};
}
std::shared_ptr<nimo::AssetSettings<nimo::Prefab>> nimo::AssetSettingsSerializer<nimo::Prefab>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Prefab>>();
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::AudioSource>::Serialize(std::shared_ptr<nimo::AssetSettings<AudioSource>> settings)
{
    return {};
}
std::shared_ptr<nimo::AssetSettings<nimo::AudioSource>> nimo::AssetSettingsSerializer<nimo::AudioSource>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::AudioSource>>();
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::Font>::Serialize(std::shared_ptr<nimo::AssetSettings<Font>> settings)
{
    return {{"PixelSize", settings->pixelSize}};
}
std::shared_ptr<nimo::AssetSettings<nimo::Font>> nimo::AssetSettingsSerializer<nimo::Font>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Font>>();
    settings->pixelSize = data["PixelSize"];
    return settings;
}
nlohmann::json nimo::AssetSettingsSerializer<nimo::Scene>::Serialize(std::shared_ptr<nimo::AssetSettings<Scene>> settings)
{
    return {};
}
std::shared_ptr<nimo::AssetSettings<nimo::Scene>> nimo::AssetSettingsSerializer<nimo::Scene>::Deserialize(const nlohmann::json& data)
{
    auto settings = std::make_shared<nimo::AssetSettings<nimo::Scene>>();
    return settings;
}
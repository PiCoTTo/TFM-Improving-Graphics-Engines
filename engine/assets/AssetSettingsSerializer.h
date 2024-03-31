#pragma once

#include "AssetSettings.h"
#include <memory>
#include "json.hpp"

namespace nimo
{
    template<typename T>
    struct AssetSettingsSerializer{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<T>> settings) = delete;
        std::shared_ptr<nimo::AssetSettings<T>> Deserialize(const nlohmann::json& data) = delete;
    };

    class Texture;
    template<> struct AssetSettingsSerializer<Texture>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Texture>> settings);
        std::shared_ptr<nimo::AssetSettings<Texture>> Deserialize(const nlohmann::json& data);
    };
    class Shader;
    template<> struct AssetSettingsSerializer<Shader>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Shader>> settings);
        std::shared_ptr<nimo::AssetSettings<Shader>> Deserialize(const nlohmann::json& data);
    };
    class Material;
    template<> struct AssetSettingsSerializer<Material>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Material>> settings);
        std::shared_ptr<nimo::AssetSettings<Material>> Deserialize(const nlohmann::json& data);
    };
    class Mesh;
    template<> struct AssetSettingsSerializer<Mesh>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Mesh>> settings);
        std::shared_ptr<nimo::AssetSettings<Mesh>> Deserialize(const nlohmann::json& data);
    };
    class EnvironmentMap;
    template<> struct AssetSettingsSerializer<EnvironmentMap>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<EnvironmentMap>> settings);
        std::shared_ptr<nimo::AssetSettings<EnvironmentMap>> Deserialize(const nlohmann::json& data);
    };
    class Script;
    template<> struct AssetSettingsSerializer<Script>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Script>> settings);
        std::shared_ptr<nimo::AssetSettings<Script>> Deserialize(const nlohmann::json& data);
    };
    class Prefab;
    template<> struct AssetSettingsSerializer<Prefab>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Prefab>> settings);
        std::shared_ptr<nimo::AssetSettings<Prefab>> Deserialize(const nlohmann::json& data);
    };
    class AudioSource;
    template<> struct AssetSettingsSerializer<AudioSource>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<AudioSource>> settings);
        std::shared_ptr<nimo::AssetSettings<AudioSource>> Deserialize(const nlohmann::json& data);
    };
    class Font;
    template<> struct AssetSettingsSerializer<Font>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Font>> settings);
        std::shared_ptr<nimo::AssetSettings<Font>> Deserialize(const nlohmann::json& data);
    };
    class Scene;
    template<> struct AssetSettingsSerializer<Scene>{
        nlohmann::json Serialize(std::shared_ptr<nimo::AssetSettings<Scene>> settings);
        std::shared_ptr<nimo::AssetSettings<Scene>> Deserialize(const nlohmann::json& data);
    };
} // namespace nimo

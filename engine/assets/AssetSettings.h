#pragma once

#include <memory>
#include "json.hpp"
#include "renderer/Texture.h"

namespace nimo
{
    struct IAssetSettings{};
    template<typename T>
    struct AssetSettings : public IAssetSettings{};

    class Texture;
    template<> struct AssetSettings<Texture> : public IAssetSettings{
        bool generateMipMaps = true;
        bool flip = true;
        TextureFiltering filtering = TextureFiltering::Average;
        TextureWrapping wrapping = TextureWrapping::Repeat;
    };
    class Shader;
    template<> struct AssetSettings<Shader> : public IAssetSettings{
    };
    class Material;
    template<> struct AssetSettings<Material> : public IAssetSettings{
    };
    class Mesh;
    template<> struct AssetSettings<Mesh> : public IAssetSettings{
        bool mergeMeshesByMaterial = true;
    };
    class EnvironmentMap;
    template<> struct AssetSettings<EnvironmentMap> : public IAssetSettings{
        unsigned int renderingResolution = 512;
    };
    class Script;
    template<> struct AssetSettings<Script> : public IAssetSettings{
    };
    class Prefab;
    template<> struct AssetSettings<Prefab> : public IAssetSettings{
    };
    class AudioSource;
    template<> struct AssetSettings<AudioSource> : public IAssetSettings{
    };
    class Font;
    template<> struct AssetSettings<Font> : public IAssetSettings{
        unsigned int pixelSize = 64;
    };
    class Scene;
    template<> struct AssetSettings<Scene> : public IAssetSettings{
    };
} // namespace nimo

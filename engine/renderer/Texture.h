#pragma once

#include <string>
#include "assets/Asset.h"

namespace nimo{
enum class TextureFiltering : unsigned char
{
    Nearest = 0,
    Average
};
enum class TextureWrapping : unsigned char
{
    Repeat = 0,
    RepeatMirrored,
    Clamp,
};
struct TextureSpecification
{
    bool generateMipmaps = false;
    TextureFiltering filtering = TextureFiltering::Average;
    TextureWrapping wrapping = TextureWrapping::Repeat;
};
class Texture : public Asset
{
public:
    Texture(unsigned int width, unsigned int height, void* data, unsigned int channels = 4, const TextureSpecification& spec = {});
    Texture(const std::string& path, bool flip = true, const TextureSpecification& spec = {});
    ~Texture();
    void bind(unsigned int slot);
    unsigned int GetInternalId() {return m_id;}

    AssetType Type() const { return AssetType::Texture; }
    static AssetType StaticType(){return AssetType::Texture;}

    inline unsigned int Width(){return m_width;}
    inline unsigned int Height(){return m_height;}

private:
    std::string m_path;
    unsigned int m_id;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_internalFormat;
    unsigned int m_dataFormat;
};
};
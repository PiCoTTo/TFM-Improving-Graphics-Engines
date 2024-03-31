#include "Texture.h"

#include <glad/glad.h>
#include "core/Log.h"
#include "stb_image.h"
    
nimo::Texture::Texture(unsigned int width, unsigned int height, void* data, unsigned int channels, const TextureSpecification& spec)
    : m_width(width)
    , m_height(height)
{
    GLenum internalFormat = 0, dataFormat = 0;
    switch (channels)
    {
    case 1:
        internalFormat = GL_R8;
        dataFormat = GL_RED;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        break;
    case 2:
        internalFormat = GL_RG8;
        dataFormat = GL_RG;
        break;
    case 3:
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
        break;
    case 4:
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    
    default:
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    }

    m_internalFormat = internalFormat;
    m_dataFormat = dataFormat;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glTextureStorage2D(m_id, 1, m_internalFormat, m_width, m_height);
    if(spec.generateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    switch (spec.filtering)
    {
    case TextureFiltering::Average :
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFiltering::Nearest :
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    
    default:
        break;
    }
    switch (spec.wrapping)
    {
    case TextureWrapping::Repeat:
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    case TextureWrapping::RepeatMirrored:
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;
    case TextureWrapping::Clamp:
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    
    default:
        break;
    }
    
    glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

nimo::Texture::Texture(const std::string& path, bool flip, const TextureSpecification& spec)
    : m_path(path)
{
    NIMO_DEBUG("nimo::Texture::Texture({})", path);
    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip);
    stbi_uc* data = nullptr;
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
    if (data)
    {
        NIMO_DEBUG("Loaded Texture Data - Size: {}x{} Channels: {}", width, height, channels);

        m_width = width;
        m_height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        switch (channels)
        {
        case 1:
            internalFormat = GL_RED;
            dataFormat = GL_RED;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            break;
        case 2:
            internalFormat = GL_RG8;
            dataFormat = GL_RG;
            break;
        case 3:
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
            break;
        case 4:
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
            break;
        
        default:
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
            break;
        }

        m_internalFormat = internalFormat;
        m_dataFormat = dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureStorage2D(m_id, 1, internalFormat, m_width, m_height);
        if(spec.generateMipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);

        switch (spec.filtering)
        {
        case TextureFiltering::Average :
            glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TextureFiltering::Nearest :
            glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        
        default:
            break;
        }
        switch (spec.wrapping)
        {
        case TextureWrapping::Repeat:
            glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case TextureWrapping::RepeatMirrored:
            glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
        case TextureWrapping::Clamp:
            glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        
        default:
            break;
        }


        glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        stbi_image_free(data);
        NIMO_DEBUG("nimo::Texture::Texture({}) - Id: {}", path, m_id);
    }
}

nimo::Texture::~Texture()
{
    NIMO_DEBUG("nimo::Texture::~Texture({}) - Id: {}", m_path, m_id);
    glDeleteTextures(1, &m_id);
}

void nimo::Texture::bind(unsigned int slot)
{
    // glBindTextureUnit(slot, m_id);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
    // glBindTextureUnit(slot, m_id);
}
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace nimo{
class FrameBuffer{
public:
    enum class BlendMode{
		None = 0,
		OneZero,
		SrcAlphaOneMinusSrcAlpha,
		Additive,
		Zero_SrcColor
    };
    struct ColorAttachment{
        int internalFormat;
        int format;
        int type;
    };
    struct Details{
        uint32_t width = 0;
        uint32_t height = 0;
        glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        float depthClearValue = 1.0f;
        bool clearColorOnBind = true;
        bool clearDepthOnBind = true;
        BlendMode blendMode = BlendMode::SrcAlphaOneMinusSrcAlpha;
        std::vector<ColorAttachment> colorAttachments;
    };
    FrameBuffer(const Details& details);
    ~FrameBuffer();
    void Bind();
    static void Unbind();
    void BindColorTexture(int id, unsigned int slot);
    void BindDepthTexture(unsigned int slot);
    unsigned int GetColorAttachmentId(int id) {return m_textureAttachments[id];}
    inline const Details& GetDetails(){return m_details; }
    inline float GetAspectRatio(){return (float)m_details.width/(float)m_details.height; }
    void CopyDepthTo(std::shared_ptr<FrameBuffer> destination);
    void CopyColorTo(std::shared_ptr<FrameBuffer> destination);
    void CopyStencilTo(std::shared_ptr<FrameBuffer> destination);

    unsigned int m_id;
private:
    std::vector<unsigned int> m_textureAttachments;
    unsigned int m_renderBuffer;
    Details m_details;
};
};
#pragma once

#include <vector>
#include <string>

namespace nimo{
enum class ShaderDataType
{
    None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};
static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
    }
    return 0;
}
static uint32_t ShaderDataTypeCount(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:    return 1;
        case ShaderDataType::Float2:   return 2;
        case ShaderDataType::Float3:   return 3;
        case ShaderDataType::Float4:   return 4;
        case ShaderDataType::Mat3:     return 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4;
        case ShaderDataType::Int:      return 1;
        case ShaderDataType::Int2:     return 2;
        case ShaderDataType::Int3:     return 3;
        case ShaderDataType::Int4:     return 4;
        case ShaderDataType::Bool:     return 1;
    }
    return 0;
}
static uint32_t ShaderDataTypeUint(ShaderDataType type);

class VertexBuffer
{
public:
    struct Layout{
        struct Attribute{
            Attribute() = default;
            Attribute(const std::string& name, ShaderDataType type)
                : name(name)
                , type(type)
                , size(ShaderDataTypeSize(type))
            {}
            std::string name;
            ShaderDataType type;
            uint32_t size;
        };
		Layout() = default;

		Layout(const std::initializer_list<Attribute>& attributes)
			: m_attributes(attributes)
		{
		}
        std::vector<Attribute> m_attributes;
    };
    enum class Usage{ None = 0, Static = 1, Dynamic = 2};
    VertexBuffer(const Layout& layout, const void* data, unsigned int size, bool dynamic = false);
    ~VertexBuffer();
    void ApplyLayout();
    void SetData(const void* data, unsigned int size);
    void Bind();
    void Unbind();

private:
    unsigned int m_id;
    Layout m_layout;
};
};
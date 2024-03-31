#include "VertexBuffer.h"

#include "glad/glad.h"

static uint32_t nimo::ShaderDataTypeUint(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:   
        case ShaderDataType::Float2:  
        case ShaderDataType::Float3:  
        case ShaderDataType::Float4:  
        case ShaderDataType::Mat3:    
        case ShaderDataType::Mat4:     return GL_FLOAT;
        case ShaderDataType::Int:      
        case ShaderDataType::Int2:     
        case ShaderDataType::Int3:     
        case ShaderDataType::Int4:     return GL_INT;
        case ShaderDataType::Bool:     return GL_BYTE;
    }
    return 0;
}

nimo::VertexBuffer::VertexBuffer(const Layout& layout, const void* data, unsigned int size, bool dynamic) 
    : m_layout(layout)
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

nimo::VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void nimo::VertexBuffer::ApplyLayout()
{
    uint32_t stride = 0;

    for(size_t i = 0; i < m_layout.m_attributes.size(); ++i)
    {
        stride += m_layout.m_attributes[i].size;
    }
    
    uint32_t offset = 0;
    for(size_t i = 0; i < m_layout.m_attributes.size(); ++i)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, ShaderDataTypeCount(m_layout.m_attributes[i].type), ShaderDataTypeUint(m_layout.m_attributes[i].type), GL_FALSE, stride, (void*)offset);
        offset += m_layout.m_attributes[i].size;
    }
}

void nimo::VertexBuffer::SetData(const void* data, unsigned int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void nimo::VertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void nimo::VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
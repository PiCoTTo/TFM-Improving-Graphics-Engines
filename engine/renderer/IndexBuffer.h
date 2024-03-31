#pragma once

namespace nimo{
class IndexBuffer
{
public:
    IndexBuffer(unsigned int* indices, unsigned int indexCount);
    ~IndexBuffer();
    void Bind();
    void Unbind();
    unsigned int Count();

private:
    unsigned int m_id;
    unsigned int m_count;
};
};
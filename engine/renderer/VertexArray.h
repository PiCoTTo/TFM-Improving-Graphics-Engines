#pragma once

namespace nimo{
class VertexArray
{
public:
    VertexArray();
    ~VertexArray();
    void Bind();
    void Unbind();

private:
    unsigned int m_id;
};
};
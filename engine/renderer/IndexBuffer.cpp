#include "IndexBuffer.h"

#include "glad/glad.h"

nimo::IndexBuffer::IndexBuffer(unsigned int* indices, unsigned int indexCount) 
    : m_count(indexCount)
{
    glCreateBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

nimo::IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void nimo::IndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void nimo::IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int nimo::IndexBuffer::Count()
{
    return m_count;
}
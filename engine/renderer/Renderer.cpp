#include "Renderer.h"
#include "glad/glad.h"

nimo::RendererStats nimo::Renderer::stats = {};
std::shared_ptr<nimo::Mesh> nimo::Renderer::m_fullScreenQuad = {};
std::shared_ptr<nimo::Mesh> nimo::Renderer::m_quad = {};

void nimo::Renderer::Initialize()
{
    // Full screen Quad mesh
    {
        std::vector<Vertex> vertices ={
            {{1.0f,  1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{1.0f,  -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-1.0f,  -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}
        };
        std::vector<unsigned int> indices ={
            0,1,3,
            1,2,3
        };
        m_fullScreenQuad = std::make_shared<Mesh>(vertices, indices);
    }
    // Quad mesh
    {
        std::vector<Vertex> vertices ={
            {{0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}
        };
        std::vector<unsigned int> indices ={
            0,1,3,
            1,2,3
        };
        m_quad = std::make_shared<Mesh>(vertices, indices);
    }
}
void nimo::Renderer::BeginFrame(std::shared_ptr<FrameBuffer> target){
    if(target)
        target->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
    stats = {};
}
void nimo::Renderer::EndFrame(){
    FrameBuffer::Unbind();
}

void nimo::Renderer::DrawMesh(const Submesh& mesh)
{
    mesh.m_vao->Bind();
    glDrawElements(GL_TRIANGLES, mesh.m_ibo->Count(), GL_UNSIGNED_INT, 0);
    stats.totalDrawCalls++;
    stats.totalTriangles += mesh.m_ibo->Count()/3;
}

void nimo::Renderer::DrawFullScreenQuad()
{
    DrawMesh(*m_fullScreenQuad->GetSubmesh(0));
}
void nimo::Renderer::DrawQuad()
{
    DrawMesh(*m_quad->GetSubmesh(0));
}


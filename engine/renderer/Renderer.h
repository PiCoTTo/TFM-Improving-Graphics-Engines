#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "FrameBuffer.h"
#include "Mesh.h"

namespace nimo{
    struct RendererStats
    {
        int totalTriangles = 0;
        int totalDrawCalls = 0;
    };
    class Renderer{
        public:
            static void Initialize();
            static void BeginFrame(std::shared_ptr<FrameBuffer> target = {});
            static void EndFrame();
            static void DrawMesh(const Submesh& mesh);
            static void DrawFullScreenQuad();
            static void DrawQuad();
            static RendererStats stats;
        private:
            static std::shared_ptr<Mesh> m_fullScreenQuad;
            static std::shared_ptr<Mesh> m_quad;
    };
}
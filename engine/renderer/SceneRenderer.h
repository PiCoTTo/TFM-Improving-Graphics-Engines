#pragma once
#include "scene/Scene.h"
#include "EnvironmentMap.h"
#include "core/Timer.h"
#include "fonts/Font.h"

namespace nimo
{
    class SceneRenderer
    {
    public:
        SceneRenderer();
        ~SceneRenderer();

        bool limitFPS = false;

        inline float currentFrameTime() const
        {
            return m_frameTime;
        }

        inline float currentFPS() const
        {
            return 1000 / m_frameTime;
        }

        void SetScene(std::shared_ptr<Scene> scene);
        void Render(std::shared_ptr<FrameBuffer> target = {}, const CameraComponent& cameraSettings = {}, const TransformComponent& cameraTransform = {}, float deltaTime = 0);
    public:
        Timer m_frameTimer;
        float m_frameTime;
        Timer m_renderFrameTimer;
        Timer m_bloomFrameTimer;
        Timer m_geometryFrameTimer;
        Timer m_geometry2DFrameTimer;
        Timer m_lightingFrameTimer;
        std::shared_ptr<Scene> m_scene;
        std::shared_ptr<FrameBuffer> m_directionalLightDepthBuffer;
        std::shared_ptr<FrameBuffer> m_gBuffer;
        std::shared_ptr<FrameBuffer> m_hdrColorBuffer;
        std::shared_ptr<FrameBuffer> m_hdrBrightnessBuffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomDownsample1Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomDownsample2Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomDownsample3Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomDownsample4Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomDownsample5Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomDownsample6Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomUpsample1Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomUpsample2Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomUpsample3Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomUpsample4Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomUpsample5Buffer;
        std::shared_ptr<FrameBuffer> m_hdrBloomUpsample6Buffer;
        std::shared_ptr<FrameBuffer> m_hdrFinalBloomBuffer;
        std::shared_ptr<Shader> m_shaderLightingPass;
        std::shared_ptr<Shader> m_backgroundPass;
        std::shared_ptr<Shader> m_hdrToneMappingPass;
        std::shared_ptr<Shader> m_hdrBrightFilterPass;
        std::shared_ptr<Shader> m_hdrBloomDownsamplePass;
        std::shared_ptr<Shader> m_hdrBloomUpsamplePass;
        std::shared_ptr<Shader> m_shader2d;
        std::shared_ptr<Shader> m_shaderText;
        std::shared_ptr<Shader> m_shaderDepth;
        std::shared_ptr<Texture> m_white;
        std::shared_ptr<Texture> m_black;
        VertexArray* m_vaoText = nullptr;
        VertexBuffer* m_vboText = nullptr;
        IndexBuffer* m_iboText = nullptr;

        const float FPS_LIMIT = 60.f;
        float m_cumulativeFrameTime = 1 / FPS_LIMIT;
    };
} // namespace nimo

#pragma once
#include "scene/Scene.h"
#include "EnvironmentMap.h"
#include "core/Timer.h"
#include "fonts/Font.h"
#include "renderer/RenderPass.h"
#include "ffx_fsr2.h"
#include "ffx_fsr2_gl.h"


namespace nimo
{
    static glm::uint pcg_hash(glm::uint seed)
    {
        glm::uint state = seed * 747796405u + 2891336453u;
        glm::uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    class RenderPass;

    enum class RenderPassId
    {
        Deferred,
        Forward,
        Shadow,
        Postprocess,
        Debug
    };

    struct SceneInfo
    {
        glm::mat4 projectionMatrix;
        glm::mat4 orthoMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 modelMatrix;
    };

    class SceneRenderer : public std::enable_shared_from_this<SceneRenderer>
    {
    public:
        struct Plane
        {
            Plane() {}
            Plane(const glm::vec3& p1, const glm::vec3& norm)
                : position(p1),
                normal(glm::normalize(norm)),
                distance(glm::dot(normal, p1))
            {}

            glm::vec3 position{ 0.f, 0.f, 0.f };
            glm::vec3 normal{ 0.f, 0.f, 0.f };
            float distance{ 0.f };

            float getSignedDistanceToPlane(const glm::vec3& point) const
            {
                return glm::dot(normal, point) - distance;
            }
        };

        struct Frustum
        {
            Plane topFace;
            Plane bottomFace;
            Plane rightFace;
            Plane leftFace;

            Plane farFace;
            Plane nearFace;
        };

        struct BoundingVolume
        {
            virtual bool isOnFrustum(const Frustum& camFrustum,
                const nimo::TransformComponent& modelTransform) const = 0;
        };

        struct AABB : public BoundingVolume
        {
            glm::vec3 center{ 0.f, 0.f, 0.f };
            glm::vec3 extents{ 0.f, 0.f, 0.f };

            AABB(const glm::vec3& min, const glm::vec3& max)
                : BoundingVolume{},
                center{ (max + min) * 0.5f },
                extents{ max.x - center.x, max.y - center.y, max.z - center.z }
            {}

            AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
                : BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
            {}

            bool isOnFrustum(const Frustum& camFrustum, const TransformComponent& modelTransform) const;
            bool isOnOrForwardPlane(const Plane& plane) const;
        };

        SceneRenderer(bool enableDebug = false);
        ~SceneRenderer();

        bool limitFPS{ true };
        bool m_useDeferredShading{ false };
        bool enabledFrustumCulling{ false };
        bool enabledFSR2{ false };
        bool m_enabledDebug{ false };

        inline float currentFrameTime() const
        {
            return m_frameTime;
        }

        inline float currentFPS() const
        {
            return 1000 / m_frameTime;
        }

        inline bool mustRender() const
        {
            return m_mustRender;
        }

        void initialize();

        void SetScene(std::shared_ptr<Scene> scene);

        void update(float deltaTime = 0);

        void Render(std::shared_ptr<FrameBuffer> target = {}, const CameraComponent& cameraSettings = {}, const TransformComponent& cameraTransform = {}, float deltaTime = 0);

        void updateFromChangedVariables();

        void updateFrustumCulling(const nimo::TransformComponent& camTransform, const CameraComponent& camera, float viewportWidth, float viewportHeight);

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
        std::shared_ptr<FrameBuffer> m_hdrFsrColorBuffer;
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
        std::shared_ptr<Shader> m_shaderForwardLightingPass;
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
        bool m_mustRender{ true };
        unsigned int m_renderEntitiesLimit{ 724 };
        unsigned int m_pointLightEntitiesLimit{ 32 };

        // FSR2 variables
        uint32_t renderWidth;
        uint32_t renderHeight;
        uint32_t frameIndex = 0;
        uint32_t seed = pcg_hash(17);

        bool fsr2FirstInit = true;
        float fsr2Sharpness = 0;
        //float fsr2Ratio = 1.7f; // FFX_FSR2_QUALITY_MODE_BALANCED
        float fsr2Ratio = 3.0f; // FFX_FSR2_QUALITY_MODE_ULTRA_PERFORMANCE
        FfxFsr2Context fsr2Context;
        std::unique_ptr<char[]> fsr2ScratchMemory;

    private:
        // Render passes variables
        std::vector < std::pair < RenderPassId, std::shared_ptr<nimo::RenderPass> > > m_renderPasses;
        std::shared_ptr<nimo::RenderPass> m_debugPass;
        std::shared_ptr<SceneRenderer> m_renderer;

        bool m_mustReconfigurePipeline{ false };
        bool m_currentlyUsingFSR2{ false };

        Frustum getFrustumFromCamera(const nimo::TransformComponent& transform, float fov, float width, float height, float nearDist, float farDist);
        void initFBOs(bool fsrActive = false);
    };
} // namespace nimo

void renderCube2();

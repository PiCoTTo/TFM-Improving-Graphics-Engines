#include "SceneRenderer.h"
#include "glad/glad.h"
#include "core/Application.h"
#include "glm/gtc/matrix_inverse.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "Renderer.h"
#include "project/Project.h"
#include "core/ExportedVariablesManager.h"
#include "renderer/DebugPass.h"
#include "renderer/DeferredPass.h"
#include "renderer/ForwardPass.h"
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"


struct TextVertex
{
    glm::vec4 vertex;
};

unsigned int cubeVAO2 = 0;
unsigned int cubeVBO2 = 0;
void renderCube2()
{
    // initialize (if necessary)
    if (cubeVAO2 == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO2);
        glGenBuffers(1, &cubeVBO2);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO2);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO2);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
nimo::SceneRenderer::SceneRenderer(bool enableDebug) :
    m_enabledDebug(enableDebug)
{
    nimo::ExportedVariablesManager::instance()->addVariable("RENDERER_LIMIT_FPS", limitFPS);
    nimo::ExportedVariablesManager::instance()->addVariable("RENDERER_FRUSTUM_CULLING", enabledFrustumCulling);
    nimo::ExportedVariablesManager::instance()->addVariable("RENDERER_ENTITIES_LIMIT", m_renderEntitiesLimit);
    nimo::ExportedVariablesManager::instance()->addVariable("RENDERER_LIGHT_POINTS_LIMIT", m_pointLightEntitiesLimit);
    nimo::ExportedVariablesManager::instance()->addVariable("RENDERER_FSR2", enabledFSR2);

    if (enabledFSR2)
        initFSR2();

    // Directional Light buffer
    FrameBuffer::Details directionalLightBufferDetails;
    directionalLightBufferDetails.width = 4096;
    directionalLightBufferDetails.height = 4096;
    directionalLightBufferDetails.clearColorOnBind = true;
    directionalLightBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT });
    m_directionalLightDepthBuffer = std::make_shared<FrameBuffer>(directionalLightBufferDetails);
    // GBuffer
    initFBOs(enabledFSR2);
    // HDR brightness buffer
    FrameBuffer::Details hdrBrightnessBufferDetails;
    hdrBrightnessBufferDetails.width = 960;
    hdrBrightnessBufferDetails.height = 540;
    hdrBrightnessBufferDetails.clearColorOnBind = true;
    hdrBrightnessBufferDetails.clearDepthOnBind = true;
    hdrBrightnessBufferDetails.colorAttachments.push_back({ GL_RGBA16F, GL_RGB, GL_FLOAT });
    m_hdrBrightnessBuffer = std::make_shared<FrameBuffer>(hdrBrightnessBufferDetails);
    // HDR bloom buffers
    FrameBuffer::Details hdrBloomBufferDetails;
    hdrBloomBufferDetails.clearColorOnBind = true;
    hdrBloomBufferDetails.clearDepthOnBind = true;
    hdrBloomBufferDetails.colorAttachments.push_back({ GL_RGBA16F, GL_RGB, GL_FLOAT });
    hdrBloomBufferDetails.width = 1920;
    hdrBloomBufferDetails.height = 1080;
    m_hdrFinalBloomBuffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 960;
    hdrBloomBufferDetails.height = 540;
    m_hdrBloomUpsample1Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 480;
    hdrBloomBufferDetails.height = 270;
    m_hdrBloomDownsample1Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    m_hdrBloomUpsample2Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 240;
    hdrBloomBufferDetails.height = 135;
    m_hdrBloomDownsample2Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    m_hdrBloomUpsample3Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 120;
    hdrBloomBufferDetails.height = 67;
    m_hdrBloomDownsample3Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    m_hdrBloomUpsample4Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 60;
    hdrBloomBufferDetails.height = 33;
    m_hdrBloomDownsample4Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    m_hdrBloomUpsample5Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 30;
    hdrBloomBufferDetails.height = 16;
    m_hdrBloomDownsample5Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    m_hdrBloomUpsample6Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);
    hdrBloomBufferDetails.width = 15;
    hdrBloomBufferDetails.height = 8;
    m_hdrBloomDownsample6Buffer = std::make_shared<FrameBuffer>(hdrBloomBufferDetails);

    //Lighting shader
    m_shaderLightingPass = nimo::AssetManager::Get<Shader>("Shaders/deferred_shading_pbr.nshader");
    m_shaderForwardLightingPass = nimo::AssetManager::Get<Shader>("Shaders/forward_shading_pbr.nshader");
    //Cubemap background shader
    m_backgroundPass = nimo::AssetManager::Get<Shader>("Shaders/background.nshader");
    //Tone mapping shaderm_backgroundPass
    m_hdrToneMappingPass = nimo::AssetManager::Get<Shader>("Shaders/hdr_tone_mapping.nshader");
    //Bloom
    m_hdrBrightFilterPass = nimo::AssetManager::Get<Shader>("Shaders/hdr_bright_filter.nshader");
    m_hdrBloomDownsamplePass = nimo::AssetManager::Get<Shader>("Shaders/hdr_bloom_downsample.nshader");
    m_hdrBloomUpsamplePass = nimo::AssetManager::Get<Shader>("Shaders/hdr_bloom_upsample.nshader");
    //2D
    m_shader2d = nimo::AssetManager::Get<Shader>("Shaders/unlit_texture.nshader");
    m_shaderText = AssetManager::Get<Shader>("Shaders/text.nshader");

    m_shaderDepth = nimo::AssetManager::Get<Shader>("Shaders/depth.nshader");

    //White texture in memory
    unsigned int whitePixel = 0xFFFFFFFF;
    m_white = std::make_shared<Texture>(1, 1, &whitePixel);
    //Black texture in memory
    unsigned int blackPixel = 0x00000000;
    m_black = std::make_shared<Texture>(1, 1, &blackPixel);

    std::vector<QuadVertex> m_vertices = {
        {{1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}},
        {{1.0f,  -1.0f, 0.0f}, {1.0f, 0.0f}},
        {{-1.0f,  -1.0f, 0.0f}, {0.0f, 0.0f}},
        {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    std::vector<unsigned int> indices = {
        0,1,3,
        1,2,3
    };

    std::vector<TextVertex> m_textvertices = {
        {{1.0f,  1.0f, 1.0f, 1.0f}},
        {{1.0f,  -1.0f, 1.0f, 0.0f}},
        {{-1.0f,  -1.0f, 0.0f, 0.0f}},
        {{-1.0f,  1.0f, 0.0f, 1.0f}},
    };
    m_vaoText = new VertexArray();
    m_vboText = new VertexBuffer(
        {
            {"vertex", ShaderDataType::Float4}
        },
        m_textvertices.data(), sizeof(TextVertex) * m_textvertices.size()
    );
    m_iboText = new IndexBuffer(indices.data(), indices.size());
    m_vaoText->Bind();
    m_iboText->Bind();
    m_vboText->Bind();
    m_vboText->ApplyLayout();

    m_currentlyUsingFSR2 = enabledFSR2;

    ExportedVariablesManager::instance()->addVariable("RENDERER_USE_DEFERRED", m_useDeferredShading);
}

nimo::SceneRenderer::~SceneRenderer()
{
    delete m_vaoText;
    delete m_vboText;
    delete m_iboText;
}

void nimo::SceneRenderer::SetScene(std::shared_ptr<Scene> scene)
{
    m_scene = scene;
}

void nimo::SceneRenderer::initialize()
{
    //m_renderPasses.push_back(std::make_shared<nimo::DeferredPass>(shared_from_this()));
    m_renderPasses.clear();
    if(m_useDeferredShading)
        m_renderPasses.push_back({ RenderPassId::Deferred, std::make_shared<nimo::DeferredPass>(shared_from_this()) });
    else
        m_renderPasses.push_back({ RenderPassId::Forward, std::make_shared<nimo::ForwardPass>(shared_from_this()) });

    if (m_enabledDebug)
    {
        if (!m_debugPass)
            m_debugPass = std::make_shared<nimo::DebugPass>(shared_from_this());
        m_renderPasses.push_back({ RenderPassId::Debug, m_debugPass });
    }
}

void nimo::SceneRenderer::update(float deltaTime)
{
    // When FSR2 is toggled
    if (m_currentlyUsingFSR2 != enabledFSR2)
    {
        if (m_currentlyUsingFSR2)
        {
            ffxFsr2ContextDestroy(&fsr2Context);
        }
        if (enabledFSR2)
        {
            initFSR2();
        }

        initFBOs(enabledFSR2);

        m_currentlyUsingFSR2 = enabledFSR2;
    }

    if (m_mustReconfigurePipeline)
    {
        initialize();
        m_mustReconfigurePipeline = false;
    }

    for (const auto& renderPass : m_renderPasses)
        renderPass.second->update(deltaTime);
}

void nimo::SceneRenderer::Render(std::shared_ptr<FrameBuffer> target, const CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
{
    m_mustRender = true;
    // If FPS is limited for the current project
    if (limitFPS)
    {
        m_cumulativeFrameTime += deltaTime;

        if (m_cumulativeFrameTime < 1 / FPS_LIMIT)
        {
            m_mustRender = false;
        }
        else
            m_cumulativeFrameTime = 0;
    }

    if (!m_mustRender)
        return;

    nimo::Renderer::BeginFrame();

    for (const auto& renderPass : m_renderPasses)
        renderPass.second->render(target, cameraSettings, cameraTransform, deltaTime);

    nimo::Renderer::EndFrame();

    glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);  
    glDisable(GL_BLEND);
    m_geometry2DFrameTimer.Stop();
    m_renderFrameTimer.Stop();
    m_scene = {};
}

void nimo::SceneRenderer::updateFromChangedVariables()
{
    auto renderPass = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), [](auto i) {
        return i.first == RenderPassId::Deferred; });

    if (m_useDeferredShading != (renderPass != m_renderPasses.end()))
        m_mustReconfigurePipeline = true;
}

nimo::SceneRenderer::Frustum nimo::SceneRenderer::getFrustumFromCamera(const nimo::TransformComponent& transform, float fov, float width, float height, float nearDist, float farDist)
{
    Frustum     frustum;

    float aspect = width / height;
    glm::vec3 front = transform.GetFront();
    glm::vec3 up = transform.GetUp();
    glm::vec3 right = transform.GetRight();

    const float halfVSide = farDist * tanf(fov * .5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = farDist * front;

    frustum.nearFace = { transform.Translation + nearDist * front, front };
    frustum.farFace = { transform.Translation + frontMultFar, -front };

    frustum.leftFace = { transform.Translation,
                            glm::cross(frontMultFar - right * halfHSide, up) };
    frustum.rightFace = { transform.Translation,
                            glm::cross(up,frontMultFar + right * halfHSide) };
    frustum.bottomFace = { transform.Translation,
                            glm::cross(right, frontMultFar - up * halfVSide) };
    frustum.topFace = { transform.Translation,
                            glm::cross(frontMultFar + up * halfVSide, right) };

    //frustum.rightFace = { transform.Translation,
    //                        glm::cross(frontMultFar - right * halfHSide, up) };
    //frustum.leftFace = { transform.Translation,
    //                        glm::cross(up,frontMultFar + right * halfHSide) };
    //frustum.topFace = { transform.Translation,
    //                        glm::cross(right, frontMultFar - up * halfVSide) };
    //frustum.bottomFace = { transform.Translation,
    //                        glm::cross(frontMultFar + up * halfVSide, right) };

    return frustum;
}

void nimo::SceneRenderer::initFSR2()
{
    frameIndex = 0;
    renderWidth = 1920 / fsr2Ratio;
    renderHeight = 1080 / fsr2Ratio;

    FfxFsr2ContextDescription contextDesc{
        FFX_FSR2_ENABLE_DEBUG_CHECKING | FFX_FSR2_ENABLE_AUTO_EXPOSURE | FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE |
                FFX_FSR2_ALLOW_NULL_DEVICE_AND_COMMAND_LIST, // flags
        {renderWidth, renderHeight}, // maxRenderSize
        {1920, 1080}, // displaySize
        {}, // callBacks
        {}, // device
        [](FfxFsr2MsgType type, const wchar_t* message) // fpMessage
        {
        char cstr[256] = {};
        wcstombs_s(nullptr, cstr, sizeof(cstr), message, sizeof(cstr));
        cstr[255] = '\0';
        printf("FSR 2 message (type=%d): %s\n", type, cstr);
        },
    };
    fsr2ScratchMemory = std::make_unique<char[]>(ffxFsr2GetScratchMemorySizeGL());
    ffxFsr2GetInterfaceGL(&contextDesc.callbacks, fsr2ScratchMemory.get(), ffxFsr2GetScratchMemorySizeGL(), glfwGetProcAddress);
    if (ffxFsr2ContextCreate(&fsr2Context, &contextDesc) != FFX_OK)
    {
        NIMO_ERROR("Error initializing FSR2 !");
    }
}


void nimo::SceneRenderer::initFBOs(bool fsrActive)
{
    // GBuffer
    FrameBuffer::Details gBufferDetails;
    gBufferDetails.width = fsrActive ? renderWidth : 1920;
    gBufferDetails.height = fsrActive ? renderHeight : 1080;
    gBufferDetails.clearColorOnBind = true;
    gBufferDetails.clearDepthOnBind = true;
    gBufferDetails.colorAttachments.push_back({ fsrActive ? GL_R11F_G11F_B10F : GL_RGB16F, GL_RGB, GL_FLOAT, "gPosition" }); // Positions
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT, "gNormal" }); // Normals
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT, "gNormal" }); // Normals
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT, "gAlbedo" }); // Albedo
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT, "gARM" }); // ARM
    gBufferDetails.colorAttachments.push_back({ GL_RG16F, GL_RG, GL_FLOAT, "gMotion" }); // Motion vectors
    m_gBuffer = std::make_shared<FrameBuffer>(gBufferDetails);

    // HDR FSR2 color buffer
    FrameBuffer::Details hdrFsrColorBufferDetails;
    hdrFsrColorBufferDetails.width = renderWidth;
    hdrFsrColorBufferDetails.height = renderHeight;
//    hdrFsrColorBufferDetails.clearColorOnBind = true;
//    hdrFsrColorBufferDetails.clearDepthOnBind = true;
    hdrFsrColorBufferDetails.colorAttachments.push_back({ GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT, "colorHdrRenderRes" });
    m_hdrFsrColorBuffer = std::make_shared<FrameBuffer>(hdrFsrColorBufferDetails);

    // HDR color buffer
    FrameBuffer::Details hdrColorBufferDetails;
    hdrColorBufferDetails.width = 1920;
    hdrColorBufferDetails.height = 1080;
    //hdrColorBufferDetails.clearColorOnBind = true;
    //hdrColorBufferDetails.clearDepthOnBind = true;
    hdrColorBufferDetails.colorAttachments.push_back({ fsrActive ? GL_R11F_G11F_B10F : GL_RGBA16F, GL_RGB, GL_FLOAT, "colorHdrWindowRes" });
    m_hdrColorBuffer = std::make_shared<FrameBuffer>(hdrColorBufferDetails);
}


void nimo::SceneRenderer::updateFrustumCulling(const nimo::TransformComponent& camTransform, const CameraComponent& camera, float viewportWidth, float viewportHeight)
{
    // Frustum Culling
    unsigned int entitiesDrawn = 0;
    if (enabledFrustumCulling)
    {
        auto frustum = getFrustumFromCamera(camTransform, glm::radians(camera.FOV), viewportWidth, viewportHeight, camera.ClippingPlanes.Near, camera.ClippingPlanes.Far);

        m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent, TransformComponent>().each(
            [&](ActiveComponent& active, IDComponent& id, MeshComponent& m, TransformComponent& t)
        {
            if (entitiesDrawn >= m_renderEntitiesLimit) return;
            if (!active.active) return;
            if (!m.source) return;
            if (enabledFrustumCulling)
            {
                SceneRenderer::AABB aabb(t.Translation, t.Scale.x, t.Scale.y, t.Scale.z);
                m.inFrustum = aabb.isOnFrustum(frustum, t.Translation);
            }
            else
                m.inFrustum = true;

            entitiesDrawn++;
        });
    }
}


bool nimo::SceneRenderer::AABB::isOnFrustum(const Frustum& camFrustum, const TransformComponent& modelTransform) const
{
    auto modelMatrix = modelTransform.GetTransform();

    //Get global scale thanks to our transform
    const glm::vec3 globalCenter{ modelMatrix * glm::vec4(center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = modelTransform.GetRight() * extents.x;
    const glm::vec3 up = modelTransform.GetUp() * extents.y;
    const glm::vec3 forward = modelTransform.GetFront() * extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    //We not need to divise scale because it's based on the half extention of the AABB
    const AABB globalAABB(globalCenter, newIi, newIj, newIk);

    return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.farFace));
}


bool nimo::SceneRenderer::AABB::isOnOrForwardPlane(const Plane& plane) const
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(plane.normal.x) +
        extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

    return -r <= plane.getSignedDistanceToPlane(center);
}

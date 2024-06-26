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
#include "renderer/PostprocessPass.h"
#include "renderer/GuiPass.h"
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
    directionalLightBufferDetails.clearDepthOnBind = true;
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
    m_shaderUnlitColor = nimo::AssetManager::Get<Shader>("Shaders/unlit_color.nshader");

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
    // Render pipeline configuration
    m_renderPasses.clear();

    if(m_useDeferredShading)
        m_renderPasses.push_back({ RenderPassId::Deferred, std::make_shared<nimo::DeferredPass>(shared_from_this()) });
    else
        m_renderPasses.push_back({ RenderPassId::Forward, std::make_shared<nimo::ForwardPass>(shared_from_this()) });

    m_renderPasses.push_back({ RenderPassId::Postprocess, std::make_shared<nimo::PostprocessPass>(shared_from_this()) });
    m_renderPasses.push_back({ RenderPassId::GUI, std::make_shared<nimo::GuiPass>(shared_from_this()) });

    if (m_enabledDebug)
    {
        // If debugging pass already exists, keep it
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

void nimo::SceneRenderer::Render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
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
    else
        m_cumulativeFrameTime = 0;

    if (!m_mustRender)
        return;

    nimo::Renderer::BeginFrame();

    for (const auto& renderPass : m_renderPasses)
        renderPass.second->render(target, cameraSettings, cameraTransform, limitFPS ? m_cumulativeFrameTime : deltaTime);

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
    // Atomation due to current incompatibilities
    if (!m_useDeferredShading)
        enabledFSR2 = false;

    auto renderPass = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), [](auto i) {
        return i.first == RenderPassId::Deferred; });

    if (m_useDeferredShading != (renderPass != m_renderPasses.end()))
        m_mustReconfigurePipeline = true;
}

std::shared_ptr<nimo::Frustum> nimo::SceneRenderer::getFrustumFromCamera(const nimo::TransformComponent& camTransform, float fov, float width, float height, float nearDist, float farDist)
{
    std::shared_ptr < nimo::Frustum> frustum = std::make_shared<nimo::Frustum>();

    float aspect = width / height;
    glm::vec3 front = camTransform.GetFront();
    glm::vec3 up = camTransform.GetUp();
    glm::vec3 right = camTransform.GetRight();

    ///*glm::vec3*/ right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    ///*glm::vec3*/ up = glm::normalize(glm::cross(right, front));

    // Expand the FOV of the frustum culling to avoid premature culling that removes meshes while still visible on the screen
    float halfVSide = farDist * tanf(fov*1.15f * .5f);
    float halfHSide = halfVSide * aspect;
    glm::vec3 frontMultFar = farDist * front;

    frustum->nearFace = { camTransform.Translation + nearDist * front, front };
    frustum->farFace = { camTransform.Translation + frontMultFar, -front };

    frustum->leftFace = { camTransform.Translation,
                            glm::cross(frontMultFar - right * halfHSide, up) };
    frustum->rightFace = { camTransform.Translation,
                            glm::cross(up,frontMultFar + right * halfHSide) };
    frustum->bottomFace = { camTransform.Translation,
                            glm::cross(right, frontMultFar - up * halfVSide) };
    frustum->topFace = { camTransform.Translation,
                            glm::cross(frontMultFar + up * halfVSide, right) };
    
    //frustum->rightFace = { camTransform.Translation,
    //                        glm::cross(frontMultFar - right * halfHSide, up) };
    //frustum->leftFace = { camTransform.Translation,
    //                        glm::cross(up,frontMultFar + right * halfHSide) };
    //frustum->topFace = { camTransform.Translation,
    //                        glm::cross(right, frontMultFar - up * halfVSide) };
    //frustum->bottomFace = { camTransform.Translation,
    //                        glm::cross(frontMultFar + up * halfVSide, right) };

    float narrowFactor = 0.50f;
    halfHSide *= narrowFactor;
    halfVSide *= narrowFactor;
    frontMultFar *= narrowFactor;

    // Generate vertices with a slightly narrowed frustum to be drawn. Follow same order as enum Frustum::FrustumVertice
    frustum->visibleVertices.push_back(camTransform.Translation + (frontMultFar - right * halfHSide - up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (frontMultFar - right * halfHSide + up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (frontMultFar + right * halfHSide + up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (frontMultFar + right * halfHSide - up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (nearDist * front - right * halfHSide - up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (nearDist * front - right * halfHSide + up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (nearDist * front + right * halfHSide + up * halfVSide));
    frustum->visibleVertices.push_back(camTransform.Translation + (nearDist * front + right * halfHSide - up * halfVSide));

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
    hdrFsrColorBufferDetails.clearColorOnBind = true;
    hdrFsrColorBufferDetails.clearDepthOnBind = true;
    hdrFsrColorBufferDetails.colorAttachments.push_back({ GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT, "colorHdrRenderRes" });
    m_hdrFsrColorBuffer = std::make_shared<FrameBuffer>(hdrFsrColorBufferDetails);

    // HDR color buffer
    FrameBuffer::Details hdrColorBufferDetails;
    hdrColorBufferDetails.width = 1920;
    hdrColorBufferDetails.height = 1080;
    hdrColorBufferDetails.clearColorOnBind = true;
    hdrColorBufferDetails.clearDepthOnBind = true;
    hdrColorBufferDetails.colorAttachments.push_back({ fsrActive ? GL_R11F_G11F_B10F : GL_RGBA16F, GL_RGB, GL_FLOAT, "colorHdrWindowRes" });
    m_hdrColorBuffer = std::make_shared<FrameBuffer>(hdrColorBufferDetails);
}


void nimo::SceneRenderer::updateFrustumCulling(const nimo::TransformComponent& camTransform, CameraComponent& camera, float viewportWidth, float viewportHeight)
{
    // Frustum Culling
    unsigned int entitiesDrawn = 0;
    if (enabledFrustumCulling)
    {
        m_mustCleanCulledEntities = true;

        camera.frustum = getFrustumFromCamera(camTransform, glm::radians(camera.FOV), viewportWidth, viewportHeight, camera.ClippingPlanes.Near, camera.ClippingPlanes.Far);

        m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent, TransformComponent>().each(
            [&](ActiveComponent& active, IDComponent& id, MeshComponent& m, TransformComponent& t)
        {
            if (entitiesDrawn >= m_renderEntitiesLimit) return;
            if (!active.active) return;
            if (!m.source) return;
            if (enabledFrustumCulling)
            {
                auto oob = m.source->getOOB();
                //SceneRenderer::AABB aabb(t.Translation, t.Scale.x, t.Scale.y, t.Scale.z);
                m.inFrustum = oob->isOnFrustum(camera.frustum, t.Translation);
            }

            entitiesDrawn++;
        });
    }
    else if (m_mustCleanCulledEntities)
    {
        m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent, TransformComponent>().each(
            [&](ActiveComponent& active, IDComponent& id, MeshComponent& m, TransformComponent& t)
        {
            m.inFrustum = true;
        });

        m_mustCleanCulledEntities = false;
    }
}
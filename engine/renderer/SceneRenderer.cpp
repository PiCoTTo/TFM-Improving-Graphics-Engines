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
nimo::SceneRenderer::SceneRenderer()
{
    ExportedVariablesManager::instance()->addVariable("RENDERER_LIMIT_FPS", limitFPS);
    ExportedVariablesManager::instance()->addVariable("RENDERER_ENTITIES_LIMIT", m_renderEntitiesLimit);

    // Directional Light buffer
    FrameBuffer::Details directionalLightBufferDetails;
    directionalLightBufferDetails.width = 4096;
    directionalLightBufferDetails.height = 4096;
    directionalLightBufferDetails.clearColorOnBind = true;
    directionalLightBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT });
    m_directionalLightDepthBuffer = std::make_shared<FrameBuffer>(directionalLightBufferDetails);
    // GBuffer
    FrameBuffer::Details gBufferDetails;
    gBufferDetails.width = 1920;
    gBufferDetails.height = 1080;
    gBufferDetails.clearColorOnBind = true;
    gBufferDetails.clearDepthOnBind = true;
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT });
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT });
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT });
    gBufferDetails.colorAttachments.push_back({ GL_RGB16F, GL_RGB, GL_FLOAT });
    m_gBuffer = std::make_shared<FrameBuffer>(gBufferDetails);
    // HDR color buffer
    FrameBuffer::Details hdrColorBufferDetails;
    hdrColorBufferDetails.width = 1920;
    hdrColorBufferDetails.height = 1080;
    hdrColorBufferDetails.clearColorOnBind = true;
    hdrColorBufferDetails.clearDepthOnBind = true;
    hdrColorBufferDetails.colorAttachments.push_back({ GL_RGBA16F, GL_RGB, GL_FLOAT });
    m_hdrColorBuffer = std::make_shared<FrameBuffer>(hdrColorBufferDetails);
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

    m_renderer.reset(this);
    m_renderPasses.push_back(std::make_shared<nimo::DeferredPass>(m_renderer));
    m_renderPasses.push_back(std::make_shared<nimo::DebugPass>(m_renderer));

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

void nimo::SceneRenderer::update(float deltaTime)
{
    for (const auto& renderPass : m_renderPasses)
        renderPass->update(deltaTime);
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
        renderPass->render(target, cameraSettings, cameraTransform, deltaTime);

    glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);  
    glDisable(GL_BLEND);
    m_geometry2DFrameTimer.Stop();
    m_renderFrameTimer.Stop();
    m_scene = {};
}
#include "DeferredPass.h"
#include "core/Application.h"
#include "scene/Entity.h"
#include "glad/glad.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "SceneRenderer.h"


namespace nimo
{
    static glm::vec2 GetJitterOffset(uint32_t frameIndex, uint32_t renderWidth, uint32_t renderHeight, uint32_t windowWidth)
    {
        float jitterX{};
        float jitterY{};
        ffxFsr2GetJitterOffset(&jitterX, &jitterY, frameIndex, ffxFsr2GetJitterPhaseCount(renderWidth, windowWidth));
        return { 2.0f * jitterX / static_cast<float>(renderWidth), 2.0f * jitterY / static_cast<float>(renderHeight) };
    }

    void DeferredPass::update(float deltaTime)
    {
        if (m_renderer->enabledFSR2)
        {
            shadingUniforms.random = { rng(m_renderer->seed), rng(m_renderer->seed) };
        }
    }


    glm::mat4 oldViewProjUnjittered;

    void DeferredPass::render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
	{
        m_renderer->frameIndex++;

        // Performance metrics
        m_renderer->m_frameTimer.Stop();
        m_renderer->m_frameTime = m_renderer->m_frameTimer.ElapsedMillis();
        m_renderer->m_frameTimer.Reset();

        m_renderer->m_renderFrameTimer.Reset();

        float displayedWidth = target ? target->GetDetails().width : Application::Instance().GetWindow().GetWidth();
        float displayedHeight = target ? target->GetDetails().height : Application::Instance().GetWindow().GetHeight();

        auto camTransform = cameraTransform;
        auto cam = cameraSettings;

        const float fsr2LodBias = m_renderer->enabledFSR2 ? log2(float(m_renderer->renderWidth) / float(displayedWidth)) - 1.0 : 0;
        const auto jitterOffset = m_renderer->enabledFSR2 ? GetJitterOffset(m_renderer->frameIndex, m_renderer->renderWidth, m_renderer->renderHeight, displayedWidth) : glm::vec2{};
        const auto jitterMatrix = glm::translate(glm::mat4(1), glm::vec3(jitterOffset, 0));
        const auto projUnjittered = glm::perspectiveNO(glm::radians(cam.FOV), m_renderer->renderWidth / (float)m_renderer->renderHeight, cam.ClippingPlanes.Near, cam.ClippingPlanes.Far);
        const auto projJittered = jitterMatrix * projUnjittered;

        const auto viewProj = projJittered * cameraTransform.GetView();
        const auto viewProjUnjittered = projUnjittered * cameraTransform.GetView();

        glViewport(0, 0, target ? target->GetDetails().width : Application::Instance().GetWindow().GetWidth(), target ? target->GetDetails().height : Application::Instance().GetWindow().GetHeight());

        glm::mat4 projection = glm::perspectiveFov(glm::radians(cam.FOV),
            target ? (float)target->GetDetails().width : (float)Application::Instance().GetWindow().GetWidth(),
            target ? (float)target->GetDetails().height : (float)Application::Instance().GetWindow().GetHeight(),
            cam.ClippingPlanes.Near, cam.ClippingPlanes.Far);
        glm::mat4 projectionOrtho = glm::ortho(
            -(target ? (float)target->GetDetails().width : (float)Application::Instance().GetWindow().GetWidth()) * 0.5f,
            (target ? (float)target->GetDetails().width : (float)Application::Instance().GetWindow().GetWidth()) * 0.5f,
            -(target ? (float)target->GetDetails().height : (float)Application::Instance().GetWindow().GetHeight()) * 0.5f,
            (target ? (float)target->GetDetails().height : (float)Application::Instance().GetWindow().GetHeight()) * 0.5f,
            -0.1f, cam.ClippingPlanes.Far);
        glm::mat4 viewMatrix = camTransform.GetView();
        auto viewPosition = glm::vec3(camTransform.Translation.x, camTransform.Translation.y, camTransform.Translation.z);

        // Frustum Culling
        m_renderer->updateFrustumCulling(camTransform, cam, displayedWidth, displayedHeight);

        m_renderer->m_geometryFrameTimer.Reset();
        // Render scene into gbuffer
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        m_renderer->m_gBuffer->Bind();
        unsigned int entitiesDrawn = 0;
        m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent, MeshRendererComponent>().each([&](ActiveComponent& active, IDComponent& id, MeshComponent& m, MeshRendererComponent& r) {
            if (entitiesDrawn >= m_renderer->m_renderEntitiesLimit) return;
            if (!active.active) return;
            r.material->restoreShader();
            if (!r.material || !r.material->shader || !m.source || !m.inFrustum) return;
            r.material->shader->use();
            r.material->Setup();
            r.material->shader->Set("viewPos", viewPosition);
            r.material->shader->Set("transform", m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)));
            r.material->shader->Set("view", viewMatrix);
            r.material->shader->Set("projection", projection);
            r.material->shader->Set("oldViewProjUnjittered", m_renderer->frameIndex == 1 ? viewProjUnjittered : oldViewProjUnjittered);
            r.material->shader->Set("viewProjUnjittered", viewProjUnjittered);
            r.material->shader->Set("fsr2Enabled", m_renderer->enabledFSR2);
            Renderer::DrawMesh(*m.source->GetSubmesh(m.submeshIndex));
            entitiesDrawn++;
        });
        oldViewProjUnjittered = viewProjUnjittered;
        m_renderer->m_geometryFrameTimer.Stop();

        m_renderer->m_lightingFrameTimer.Reset();
        // Render scene into directional light depth buffer
        auto directionalLightEntities = m_renderer->m_scene->entitiesRegistry().view<DirectionalLightComponent>();
        auto directionalLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
        if (directionalLightEntities.size())
        {
            Entity directionalLight(*directionalLightEntities.begin(), m_renderer->m_scene->entitiesRegistry());
            glCullFace(GL_FRONT_AND_BACK);
            m_renderer->m_directionalLightDepthBuffer->Bind();
            m_renderer->m_shaderDepth->use();
            auto directionalLightView = directionalLight.GetComponent<TransformComponent>().GetView();
            m_renderer->m_shaderDepth->Set("projection", directionalLightProjection);
            m_renderer->m_shaderDepth->Set("view", directionalLightView);
            entitiesDrawn = 0;
            m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent>().each([&](ActiveComponent& active, IDComponent& id, MeshComponent& m) {
                if (entitiesDrawn >= m_renderer->m_renderEntitiesLimit) return;
                if (!active.active) return;
                if (!m.source || !m.inFrustum) return;
                m_renderer->m_shaderDepth->Set("transform", m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)));
                Renderer::DrawMesh(*m.source->GetSubmesh(m.submeshIndex));
                entitiesDrawn++;
            });
            glCullFace(GL_BACK);
        }
        // Lighting pass
        // Clean LightDepthBuffer
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glClearDepthf(1.0f);

        std::shared_ptr<FrameBuffer> m_hdrRenderColorBuffer = m_renderer->enabledFSR2 ? m_renderer->m_hdrFsrColorBuffer : m_renderer->m_hdrColorBuffer;

        m_hdrRenderColorBuffer->Bind();
        m_renderer->m_shaderLightingPass->use();
        m_renderer->m_shaderLightingPass->Set("gPosition", 0);
        m_renderer->m_shaderLightingPass->Set("gNormal", 1);
        m_renderer->m_shaderLightingPass->Set("gAlbedo", 2);
        m_renderer->m_shaderLightingPass->Set("gARM", 3);
        m_renderer->m_shaderLightingPass->Set("gDepth", 4);
        m_renderer->m_shaderLightingPass->Set("InvProjection", glm::inverse(projection));
        m_renderer->m_gBuffer->BindColorTexture(0, 0);
        m_renderer->m_gBuffer->BindColorTexture(1, 1);
        m_renderer->m_gBuffer->BindColorTexture(2, 2);
        m_renderer->m_gBuffer->BindColorTexture(3, 3);
        m_renderer->m_gBuffer->BindDepthTexture(4);
        if (directionalLightEntities.size())
        {
            Entity directionalLight(*directionalLightEntities.begin(), m_renderer->m_scene->entitiesRegistry());
            auto directionalLightPosition = directionalLight.GetComponent<TransformComponent>().Translation;
            auto directionalLightView = directionalLight.GetComponent<TransformComponent>().GetView();
            m_renderer->m_shaderLightingPass->Set("directionalLightShadowMap", 5);
            m_renderer->m_directionalLightDepthBuffer->BindDepthTexture(5);
            m_renderer->m_shaderLightingPass->Set("directionalLightSpaceMatrix", directionalLightProjection * directionalLightView);
            m_renderer->m_shaderLightingPass->Set("directionalLightPos", directionalLightPosition);
            m_renderer->m_shaderLightingPass->Set("directionalLightColor", directionalLight.GetComponent<DirectionalLightComponent>().Color);
            m_renderer->m_shaderLightingPass->Set("directionalLightIntensity", directionalLight.GetComponent<DirectionalLightComponent>().Intensity);
        }
        int currentLights = 0;
        m_renderer->m_scene->entitiesRegistry().view<IDComponent, ActiveComponent, PointLightComponent, TransformComponent>().each([&](IDComponent id, ActiveComponent active, PointLightComponent& light, TransformComponent& lightTransform)
        {
            if (currentLights >= m_renderer->m_pointLightEntitiesLimit) return;
            if (!active.active) return;
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)), scale, rotation, translation, skew, perspective);
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Position", translation);
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Color", light.Color);
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Intensity", light.Intensity);
            static const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            static const float linear = 0.7f;
            static const float quadratic = 1.8f;
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Linear", linear);
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(light.Color.r, light.Color.g), light.Color.b) * light.Intensity;
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Radius", radius);
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(currentLights) + "].Active", true);
            currentLights++;
        });
        for (int i = currentLights; i < 32; ++i)
        {
            m_renderer->m_shaderLightingPass->Set("lights[" + std::to_string(i) + "].Active", false);
        }
        m_renderer->m_shaderLightingPass->Set("viewPos", viewPosition);
        auto skyLightEntities = m_renderer->m_scene->entitiesRegistry().view<SkyLightComponent>();
        if (skyLightEntities.size())
        {
            Entity skyLight(*skyLightEntities.begin(), m_renderer->m_scene->entitiesRegistry());
            if (skyLight.GetComponent<SkyLightComponent>().environment)
            {
                m_renderer->m_shaderLightingPass->Set("irradianceMap", 8);
                skyLight.GetComponent<SkyLightComponent>().environment->BindIrradiance(8);
            }
        }
        Renderer::DrawFullScreenQuad();
        m_renderer->m_lightingFrameTimer.Stop();

        // Background pass
        glDepthFunc(GL_LEQUAL);
        m_renderer->m_gBuffer->CopyDepthTo(m_hdrRenderColorBuffer);
        m_renderer->m_backgroundPass->use();
        m_renderer->m_backgroundPass->Set("view", viewMatrix);
        m_renderer->m_backgroundPass->Set("projection", projection);
        if (skyLightEntities.size())
        {
            Entity skyLight(*skyLightEntities.begin(), m_renderer->m_scene->entitiesRegistry());
            if (skyLight.GetComponent<SkyLightComponent>().environment)
            {
                m_renderer->m_backgroundPass->Set("environmentMap", 0);
                skyLight.GetComponent<SkyLightComponent>().environment->Bind(0);
            }
        }
        renderCube2();

        // FSR2
        if (m_renderer->enabledFSR2)
        {
            std::string_view name("FSR 2");
            //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(name.size()), name.data());

            if (m_renderer->frameIndex == 1)
            {
                deltaTime = 17.0 / 1000.0;
            }

            float jitterX{};
            float jitterY{};
            ffxFsr2GetJitterOffset(&jitterX, &jitterY, m_renderer->frameIndex, ffxFsr2GetJitterPhaseCount(m_renderer->renderWidth, displayedWidth));

            FfxFsr2DispatchDescription dispatchDesc{
                {}, // commandList
                ffxGetTextureResourceGL(m_hdrRenderColorBuffer->GetColorAttachmentId(0), m_renderer->renderWidth, m_renderer->renderHeight, GL_R11F_G11F_B10F), // color
                ffxGetTextureResourceGL(m_renderer->m_gBuffer->GetDepthTextureId(), m_renderer->renderWidth, m_renderer->renderHeight, GL_DEPTH_COMPONENT32F), // depth
                ffxGetTextureResourceGL(m_renderer->m_gBuffer->GetColorAttachmentId(4), m_renderer->renderWidth, m_renderer->renderHeight, GL_RG16F), // motionVectors
                {}, // exposure
                {}, // reactive
                {}, // transparencyAndComposition
                ffxGetTextureResourceGL(m_renderer->m_hdrColorBuffer->GetColorAttachmentId(0), displayedWidth, displayedHeight, GL_R11F_G11F_B10F), // output
                {jitterX, jitterY}, // jitterOffset
                {float(m_renderer->renderWidth), float(m_renderer->renderHeight)}, // motionVectorScale
                {m_renderer->renderWidth, m_renderer->renderHeight}, // renderSize
                m_renderer->fsr2Sharpness != 0, // enableSharpening
                m_renderer->fsr2Sharpness, // sharpness
                static_cast<float>(deltaTime * 1000.0), // frameTimeDelta
                1, // preExposure
                false, // reset
                cam.ClippingPlanes.Near, // cameraNear
                cam.ClippingPlanes.Far, // cameraFar
                glm::radians(cam.FOV), // cameraFovAngleVertical
                1, // viewSpaceToMetersFacter
                false, // deviceDepthNegativeOneToOne
            };

            if (auto err = ffxFsr2ContextDispatch(&m_renderer->fsr2Context, &dispatchDesc); err != FFX_OK)
            {
                printf("FSR 2 error: %d\n", err);
            }

            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
            //glPopDebugGroup();
        }

        glDepthFunc(GL_LESS);
        m_renderer->m_bloomFrameTimer.Reset();
        // Bloom
        // Get bright pixels in buffer
        m_renderer->m_hdrBrightnessBuffer->Bind(); //960x520
        m_renderer->m_hdrBrightFilterPass->use();
        m_renderer->m_hdrBrightFilterPass->Set("bloomThreshold", 1.2f);
        m_renderer->m_hdrBrightFilterPass->Set("hdrBuffer", 0);
        m_renderer->m_hdrColorBuffer->BindColorTexture(0, 0);
        Renderer::DrawFullScreenQuad();
        // Downsample
        m_renderer->m_hdrBloomDownsample1Buffer->Bind(); //480x270
        m_renderer->m_hdrBloomDownsamplePass->use();
        m_renderer->m_hdrBloomDownsamplePass->Set("hdrBuffer", 0);
        m_renderer->m_hdrBrightnessBuffer->BindColorTexture(0, 0); //960x540
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(960.0f, 540.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomDownsample2Buffer->Bind(); //240x135
        m_renderer->m_hdrBloomDownsample1Buffer->BindColorTexture(0, 0); //480x270
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(480.0f, 270.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomDownsample3Buffer->Bind(); //120x67
        m_renderer->m_hdrBloomDownsample2Buffer->BindColorTexture(0, 0); //240x135
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(240.0f, 135.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomDownsample4Buffer->Bind(); //60x33
        m_renderer->m_hdrBloomDownsample3Buffer->BindColorTexture(0, 0); //120x67
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(120.0f, 67.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomDownsample5Buffer->Bind(); //30x16
        m_renderer->m_hdrBloomDownsample4Buffer->BindColorTexture(0, 0); //60x33
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(60.0f, 33.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomDownsample6Buffer->Bind(); //15x8
        m_renderer->m_hdrBloomDownsample5Buffer->BindColorTexture(0, 0); //30x16
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(30.0f, 16.0f));
        Renderer::DrawFullScreenQuad();
        //Upsample
        m_renderer->m_hdrBloomUpsample6Buffer->Bind(); //30x16
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrBloomDownsample5Buffer->BindColorTexture(0, 0); //30x16
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomDownsample6Buffer->BindColorTexture(0, 1); //15x8
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(15.0f, 8.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomUpsample5Buffer->Bind(); //60x33
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrBloomDownsample4Buffer->BindColorTexture(0, 0); //60x33
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomUpsample6Buffer->BindColorTexture(0, 1); //30x16
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(30.0f, 16.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomUpsample4Buffer->Bind(); //120x67
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrBloomDownsample3Buffer->BindColorTexture(0, 0); //120x67
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomUpsample5Buffer->BindColorTexture(0, 1); //60x33
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(60.0f, 33.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomUpsample3Buffer->Bind(); //240x135
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrBloomDownsample2Buffer->BindColorTexture(0, 0); //240x135
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomUpsample4Buffer->BindColorTexture(0, 1); //120x67
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(120.0f, 67.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomUpsample2Buffer->Bind(); //480x270
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrBloomDownsample1Buffer->BindColorTexture(0, 0); //480x270
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomUpsample3Buffer->BindColorTexture(0, 1); //240x135
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(240.0f, 135.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_hdrBloomUpsample1Buffer->Bind(); //960x540
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrBrightnessBuffer->BindColorTexture(0, 0); //960x540
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomUpsample2Buffer->BindColorTexture(0, 1); //480x270
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(480.0f, 270.0f));
        Renderer::DrawFullScreenQuad();

        m_renderer->m_hdrFinalBloomBuffer->Bind(); //1920x1080
        m_renderer->m_hdrBloomUpsamplePass->use();
        m_renderer->m_hdrBloomUpsamplePass->Set("textureBig", 0);
        m_renderer->m_hdrColorBuffer->BindColorTexture(0, 0); //1920x1080
        m_renderer->m_hdrBloomUpsamplePass->Set("textureSmall", 1);
        m_renderer->m_hdrBloomUpsample1Buffer->BindColorTexture(0, 1); //960x540
        m_renderer->m_hdrBloomDownsamplePass->Set("textureResolution", glm::vec2(960.0f, 540.0f));
        Renderer::DrawFullScreenQuad();
        m_renderer->m_bloomFrameTimer.Stop();

        // HDR tone mapping pass
        if (target)
            target->Bind();
        else
        {
            FrameBuffer::Unbind();
            glViewport(0, 0, Application::Instance().GetWindow().GetWidth(), Application::Instance().GetWindow().GetHeight());
        }
        m_renderer->m_hdrToneMappingPass->use();
        m_renderer->m_hdrToneMappingPass->Set("hdrBuffer", 0);
        m_renderer->m_hdrFinalBloomBuffer->BindColorTexture(0, 0);
        Renderer::DrawFullScreenQuad();

        m_renderer->m_geometry2DFrameTimer.Reset();
        // glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
        glDisable(GL_DEPTH_TEST);  // disable depth-testing
        glEnable(GL_BLEND); // enable blend
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_renderer->m_shader2d->use();
        m_renderer->m_shader2d->Set("view", viewMatrix);
        m_renderer->m_shader2d->Set("projection", projectionOrtho);
        m_renderer->m_shader2d->Set("mainTexture", 0);
        auto draw2DLayer = [&](int layer)
        {
            m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, SpriteRendererComponent>().each([&](ActiveComponent& active, IDComponent& id, SpriteRendererComponent& r) {
                if (!active.active) return;
                if (r.layer != layer) return;
                m_renderer->m_shader2d->Set("transform", m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)));
                if (!r.texture)
                {
                    m_renderer->m_white->bind(0);
                }
                else
                {
                    r.texture->bind(0);
                }
                m_renderer->m_shader2d->Set("color", r.Color);
                m_renderer->m_shader2d->Set("tiling", r.tiling);
                m_renderer->m_shader2d->Set("offset", r.offset);
                Renderer::DrawQuad();
            });
        };
        draw2DLayer(0);
        draw2DLayer(1);
        draw2DLayer(2);
        draw2DLayer(3);
        draw2DLayer(4);
        draw2DLayer(5);
        draw2DLayer(6);
        draw2DLayer(7);
        draw2DLayer(8);
        draw2DLayer(9);
        m_renderer->m_shaderText->use();
        m_renderer->m_shaderText->Set("projection", projectionOrtho);
        m_renderer->m_shaderText->Set("text", 0);
        m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, TransformComponent, TextRendererComponent>().each([&](ActiveComponent active, IDComponent& id, TransformComponent& t, TextRendererComponent& r) {
            if (!active.active) return;
            if (!r.font) return;
            float x = .0f;
            float y = .0f;
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)), scale, rotation, translation, skew, perspective);
            m_renderer->m_shaderText->Set("color", r.Color);
            for (std::string::const_iterator c = r.text.begin(); c != r.text.end(); ++c)
            {
                auto& glyphIt = r.font->m_glyphs.find(*c);
                if (glyphIt == r.font->m_glyphs.end()) continue;
                auto& glyph = glyphIt->second;
                if (*c == '\n')
                {
                    y -= (r.font->lineSpacing >> 6) * t.Scale.y;
                    x = 0.0f;
                    continue;
                }
                float xpos = x + ((float)glyph.bearing.x + (float)glyph.size.x * 0.5f) * t.Scale.x;
                float ypos = y + ((float)glyph.bearing.y) * 0.5f * t.Scale.y - ((float)glyph.size.y - (float)glyph.bearing.y) * 0.5f * t.Scale.y;
                float w = (float)glyph.size.x * t.Scale.x;
                float h = (float)glyph.size.y * t.Scale.y;
                // model = glm::translate(model, glm::vec3(xpos , ypos, .0f));

                TransformComponent t2;
                t2.Translation = translation + glm::vec3(xpos, ypos, 0.0f);
                t2.Scale = scale * glm::vec3((float)glyph.size.x, (float)glyph.size.y, 1.0f);
                m_renderer->m_shaderText->Set("transform", t2.GetTransform());
                glyph.texture->bind(0);
                Renderer::DrawQuad();
                // std::vector<TextVertex> textvertices ={
                //     {{xpos + w,  ypos, 1.0f, 1.0f}},
                //     {{xpos + w,  ypos + h, 1.0f, 0.0f}},
                //     {{xpos,  ypos + h, 0.0f, 0.0f}},
                //     {{xpos,  ypos, 0.0f, 1.0f}},
                // };
                // glyph.texture->bind(0);
                // m_vaoText->Bind();
                // m_vboText->Bind();
                // m_vboText->SetData(textvertices.data(), sizeof(TextVertex) * textvertices.size());
                // m_vboText->Unbind();
                // glDrawElements(GL_TRIANGLES, m_iboText->Count(), GL_UNSIGNED_INT, 0);
                x += ((glyph.advance.x >> 6) + r.characterSpacing) * t.Scale.x;
                if (*c == '\n')
                {
                    y -= (r.font->lineSpacing >> 6) * t.Scale.y;
                    x = 0.0f;
                }
            }
        });

        glEnable(GL_DEPTH_TEST);
        // glDepthMask(GL_TRUE);  
        glDisable(GL_BLEND);
        m_renderer->m_geometry2DFrameTimer.Stop();
        m_renderer->m_renderFrameTimer.Stop();
	}
}

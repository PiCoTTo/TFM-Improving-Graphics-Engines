#include "ForwardPass.h"
#include "core/Application.h"
#include "scene/Entity.h"
#include "glad/glad.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "SceneRenderer.h"


namespace nimo
{
	void ForwardPass::render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
	{
        // Performance metrics
        m_renderer->m_frameTimer.Stop();
        m_renderer->m_frameTime = m_renderer->m_frameTimer.ElapsedMillis();
        m_renderer->m_frameTimer.Reset();

        m_renderer->m_renderFrameTimer.Reset();

        float width = target ? target->GetDetails().width : Application::Instance().GetWindow().GetWidth();
        float height = target ? target->GetDetails().height : Application::Instance().GetWindow().GetHeight();
        if (!width || !height)
            width = height = 1;

        glViewport(0, 0, target ? target->GetDetails().width : width, target ? target->GetDetails().height : height);

        auto camTransform = cameraTransform;
        auto cam = cameraSettings;
        glm::mat4 projection = glm::perspectiveFov(glm::radians(cam.FOV),
            target ? (float)target->GetDetails().width : width,
            target ? (float)target->GetDetails().height : height,
            cam.ClippingPlanes.Near, cam.ClippingPlanes.Far);
        glm::mat4 projectionOrtho = glm::ortho(
            -(target ? (float)target->GetDetails().width : width) * 0.5f,
            (target ? (float)target->GetDetails().width : width) * 0.5f,
            -(target ? (float)target->GetDetails().height : height) * 0.5f,
            (target ? (float)target->GetDetails().height : height) * 0.5f,
            -0.1f, cam.ClippingPlanes.Far);
        glm::mat4 viewMatrix = camTransform.GetView();
        auto viewPosition = glm::vec3(camTransform.Translation.x, camTransform.Translation.y, camTransform.Translation.z);

        // Frustum Culling
        m_renderer->updateFrustumCulling(camTransform, cameraSettings, width, height);

        // Lighting
        m_renderer->m_lightingFrameTimer.Reset();
        unsigned int entitiesDrawn = 0;

        // Shadow map
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
                if (!m.source || (m_renderer->enabledFrustumCulling && !m.inFrustum)) return;
                m_renderer->m_shaderDepth->Set("transform", m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)));
                Renderer::DrawMesh(*m.source->GetSubmesh(m.submeshIndex));
                entitiesDrawn++;
            });
            glCullFace(GL_BACK);
        }

        // Clean LightDepthBuffer
        //glClear(GL_DEPTH_BUFFER_BIT);
        //glClearDepthf(1.0f);

        // Lighting pass
        m_renderer->m_hdrColorBuffer->Bind();
        m_renderer->m_shaderForwardLightingPass->use();
        
        if (directionalLightEntities.size())
        {
            Entity directionalLight(*directionalLightEntities.begin(), m_renderer->m_scene->entitiesRegistry());
            auto directionalLightPosition = directionalLight.GetComponent<TransformComponent>().Translation;
            auto directionalLightView = directionalLight.GetComponent<TransformComponent>().GetView();
            m_renderer->m_shaderForwardLightingPass->Set("directionalLightShadowMap", 5);
            m_renderer->m_directionalLightDepthBuffer->BindDepthTexture(5);
            m_renderer->m_shaderForwardLightingPass->Set("directionalLightSpaceMatrix", directionalLightProjection * directionalLightView);
            m_renderer->m_shaderForwardLightingPass->Set("directionalLightPos", directionalLightPosition);
            m_renderer->m_shaderForwardLightingPass->Set("directionalLightColor", directionalLight.GetComponent<DirectionalLightComponent>().Color);
            m_renderer->m_shaderForwardLightingPass->Set("directionalLightIntensity", directionalLight.GetComponent<DirectionalLightComponent>().Intensity);
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
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Position", translation);
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Color", light.Color);
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Intensity", light.Intensity);
            static const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            static const float linear = 0.7f;
            static const float quadratic = 1.8f;
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Linear", linear);
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Quadratic", quadratic);
            // then calculate radius of light volume/sphere
            const float maxBrightness = std::fmaxf(std::fmaxf(light.Color.r, light.Color.g), light.Color.b) * light.Intensity;
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Radius", radius);
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(currentLights) + "].Active", true);
            currentLights++;
        });
        for (int i = currentLights; i < m_renderer->m_maxNumberPointLights; ++i)
        {
            m_renderer->m_shaderForwardLightingPass->Set("lights[" + std::to_string(i) + "].Active", false);
        }
        m_renderer->m_shaderForwardLightingPass->Set("viewPos", viewPosition);
        auto skyLightEntities = m_renderer->m_scene->entitiesRegistry().view<SkyLightComponent>();
        if (skyLightEntities.size())
        {
            Entity skyLight(*skyLightEntities.begin(), m_renderer->m_scene->entitiesRegistry());
            if (skyLight.GetComponent<SkyLightComponent>().environment)
            {
                m_renderer->m_shaderForwardLightingPass->Set("irradianceMap", 8);
                skyLight.GetComponent<SkyLightComponent>().environment->BindIrradiance(8);
            }
        }

        // Render scene
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        entitiesDrawn = 0;
        m_renderer->m_scene->entitiesRegistry().view<ActiveComponent, IDComponent, MeshComponent, MeshRendererComponent>().each([&](ActiveComponent& active, IDComponent& id, MeshComponent& m, MeshRendererComponent& r) {
            if (entitiesDrawn >= m_renderer->m_renderEntitiesLimit) return;
            if (!active.active) return;
            if (!r.material || !r.material->shader || !m.source || (m_renderer->enabledFrustumCulling && !m.inFrustum)) return;
            r.material->setShader(m_renderer->m_shaderForwardLightingPass);
            r.material->shader->use();
            r.material->Setup();
            r.material->shader->Set("viewPos", viewPosition);
            r.material->shader->Set("transform", m_renderer->m_scene->GetWorldSpaceTransformMatrix(m_renderer->m_scene->GetEntity(id.Id)));
            r.material->shader->Set("view", viewMatrix);
            r.material->shader->Set("projection", projection);

            Renderer::DrawMesh(*m.source->GetSubmesh(m.submeshIndex));
            entitiesDrawn++;
        });

        m_renderer->m_lightingFrameTimer.Stop();

        // Background pass
        glDepthFunc(GL_LEQUAL);
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
	}
}
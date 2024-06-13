#include "PostprocessPass.h"
#include "core/Application.h"
#include "scene/Entity.h"
#include "glad/glad.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "SceneRenderer.h"


namespace nimo
{
    void PostprocessPass::render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
	{
        float displayedWidth = target ? target->GetDetails().width : Application::Instance().GetWindow().GetWidth();
        float displayedHeight = target ? target->GetDetails().height : Application::Instance().GetWindow().GetHeight();
        if (!displayedWidth || !displayedHeight)
            displayedWidth = displayedHeight = 1;

        auto camTransform = cameraTransform;
        auto cam = cameraSettings;

        glViewport(0, 0, target ? target->GetDetails().width : displayedWidth, target ? target->GetDetails().height : displayedHeight);

        glm::mat4 projectionOrtho = glm::ortho(
            -(target ? (float)target->GetDetails().width : displayedWidth) * 0.5f,
            (target ? (float)target->GetDetails().width : displayedWidth) * 0.5f,
            -(target ? (float)target->GetDetails().height : displayedHeight) * 0.5f,
            (target ? (float)target->GetDetails().height : displayedHeight) * 0.5f,
            -0.1f, cam.ClippingPlanes.Far);
        glm::mat4 viewMatrix = camTransform.GetView();

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
	}
}

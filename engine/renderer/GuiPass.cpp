#include "GuiPass.h"
#include "core/Application.h"
#include "scene/Entity.h"
#include "glad/glad.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "SceneRenderer.h"


namespace nimo
{
    void GuiPass::render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime)
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

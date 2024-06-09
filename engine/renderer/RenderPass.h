#pragma once
#include <memory>
#include "renderer/FrameBuffer.h"
#include "scene/Components.h"
#include "renderer/SceneRenderer.h"


namespace nimo
{
	class RenderPass
	{
	public:
		RenderPass(std::shared_ptr<SceneRenderer> renderer) : m_renderer(renderer) {}
		virtual ~RenderPass() {}

		virtual void update(float deltaTime = 0) {}
		virtual void render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform = {}, float deltaTime = 0) {}

	protected:
		std::shared_ptr<SceneRenderer> m_renderer;
	};
}
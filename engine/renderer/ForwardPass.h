#pragma once
#include "RenderPass.h"
#include "Renderer.h"
#include "renderer/SceneRenderer.h"


namespace nimo
{
	class ForwardPass : public RenderPass
	{
	public:
		ForwardPass(std::shared_ptr<SceneRenderer> renderer) : RenderPass(renderer) {}

		// From RenderPass
		void render(std::shared_ptr<FrameBuffer> target = {}, const CameraComponent& cameraSettings = {}, const TransformComponent& cameraTransform = {}, float deltaTime = 0) override;
	};
}
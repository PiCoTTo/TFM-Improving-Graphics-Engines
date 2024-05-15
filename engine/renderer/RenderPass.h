#pragma once
#include <memory>
#include "renderer/FrameBuffer.h"
#include "scene/Components.h"


namespace nimo
{
	class RenderPass
	{
	public:
		virtual ~RenderPass() {}

		virtual void update(float deltaTime = 0) {}
		virtual void render(std::shared_ptr<FrameBuffer> target = {}, const CameraComponent& cameraSettings = {}, const TransformComponent& cameraTransform = {}, float deltaTime = 0) {}
	};
}
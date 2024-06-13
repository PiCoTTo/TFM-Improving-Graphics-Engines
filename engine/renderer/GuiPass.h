#pragma once
#include "RenderPass.h"
#include "Renderer.h"
#include "renderer/SceneRenderer.h"
#include "src/ffx-fsr2-api/ffx_fsr2.h"
#include "src/ffx-fsr2-api/gl/ffx_fsr2_gl.h"

namespace nimo
{
	class GuiPass : public RenderPass
	{
	public:
		GuiPass(std::shared_ptr<SceneRenderer> renderer) : RenderPass(renderer) {}

		void render(std::shared_ptr<FrameBuffer> target, CameraComponent& cameraSettings, const TransformComponent& cameraTransform = {}, float deltaTime = 0) override;
	};
}

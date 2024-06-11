#pragma once
#include "RenderPass.h"
#include "Renderer.h"
#include "renderer/SceneRenderer.h"
#include "src/ffx-fsr2-api/ffx_fsr2.h"
#include "src/ffx-fsr2-api/gl/ffx_fsr2_gl.h"

namespace nimo
{

    // Used to advance the PCG state.
    static glm::uint rand_pcg(glm::uint& rng_state)
    {
        glm::uint state = rng_state;
        rng_state = rng_state * 747796405u + 2891336453u;
        glm::uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    // Advances the prng state and returns the corresponding random float.
    static float rng(glm::uint& state)
    {
        glm::uint x = rand_pcg(state);
        state = x;
        return float(x) * glm::uintBitsToFloat(0x2f800004u);
    }

	class DeferredPass : public RenderPass
	{
	public:
		DeferredPass(std::shared_ptr<SceneRenderer> renderer) : RenderPass(renderer) {}

		// From RenderPass
        void update(float deltaTime) override;

		void render(std::shared_ptr<FrameBuffer> target = {}, const CameraComponent& cameraSettings = {}, const TransformComponent& cameraTransform = {}, float deltaTime = 0) override;

	private:

        // FSR2 resources
        struct ObjectUniforms
        {
            glm::mat4 model;
        };

        struct GlobalUniforms
        {
            glm::mat4 viewProj;
            glm::mat4 oldViewProjUnjittered;
            glm::mat4 viewProjUnjittered;
            glm::mat4 invViewProj;
            glm::mat4 proj;
            glm::vec4 cameraPos;
        };

        struct ShadingUniforms
        {
            glm::mat4 sunViewProj;
            glm::vec4 sunDir;
            glm::vec4 sunStrength;
            glm::mat4 sunView;
            glm::mat4 sunProj;
            glm::vec2 random;
        };

        ShadingUniforms shadingUniforms{};
	};
}
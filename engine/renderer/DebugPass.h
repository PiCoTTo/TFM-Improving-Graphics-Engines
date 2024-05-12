#pragma once
#include "renderer/RenderPass.h"
#include "Renderer.h"
#include "renderer/SceneRenderer.h"

namespace nimo
{
	struct PerformanceSample
	{
		float m_time;
		float m_lastFrameTime;
		float m_FPS;
		float m_lastRenderTime;
	};

	class DebugPass : public RenderPass
	{
	public:
		DebugPass(std::shared_ptr<SceneRenderer> renderer) : m_renderer(renderer) {}

		void update(float deltaTime) override;
		void render() override;

	private:
		bool m_statsViewEnabled{ true };
		bool m_exportedVariablesViewEnabled{ true };
		bool m_shadersEditorViewEnabled{ true };

		std::shared_ptr<nimo::SceneRenderer> m_renderer;
		nimo::RendererStats m_displayedStats;
		int m_refreshRate = 2;  // Stats display update frequency (times/sec)
		float m_timeDebugRefresh = 0;   // Time elapsed since last debug stats update
		float m_currentTime = 0;
		std::vector<PerformanceSample> m_samples;
	};
}
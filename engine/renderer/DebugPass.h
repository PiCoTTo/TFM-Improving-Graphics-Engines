#pragma once
#include "renderer/RenderPass.h"
#include "Renderer.h"
#include "renderer/SceneRenderer.h"
#include "imgui.h"

namespace nimo
{
	struct PerformanceSample
	{
		float m_time;
		float m_lastFrameTime;
		float m_FPS;
		float m_lastRenderTime;
	};

	struct ShaderEntry
	{
		bool dirty{ false };
		AssetId id;
		std::shared_ptr<Shader> m_shader;
		std::string vertexSourceBak;
		std::string fragmentSourceBak;
	};

	class DebugPass : public RenderPass
	{
	public:
		DebugPass(std::shared_ptr<SceneRenderer> renderer);

		// From RenderPass
		void update(float deltaTime) override;
		void render(std::shared_ptr<FrameBuffer> target, const CameraComponent& cameraSettings, const TransformComponent& cameraTransform, float deltaTime) override;

	private:
		bool m_statsViewEnabled{ true };
		bool m_exportedVariablesViewEnabled{ true };
		bool m_shadersEditorViewEnabled{ true };
		bool applyRequested{ false };
		bool revertRequested{ false };

		std::shared_ptr<nimo::SceneRenderer> m_renderer;
		nimo::RendererStats m_displayedStats;
		double m_frameTimeSamplesSum = 0;
		int m_refreshRate = 2;  // Stats display update frequency (times/sec)
		float m_timeDebugRefresh = 0;   // Time elapsed since last debug stats update
		float m_currentTime = 0;
		std::vector<PerformanceSample> m_samples;

		std::map<std::string, std::shared_ptr<ShaderEntry>> m_openShaders;
		//std::vector<std::string> m_openShaders;

		const static ImGuiTreeNodeFlags TREENODE_BASE_FLAGS;
		static const ImGuiSelectableFlags SELECTABLE_BASE_FLAGS;
		static const ImGuiTabBarFlags TAB_BAR_BASE_FLAGS;
	};
}
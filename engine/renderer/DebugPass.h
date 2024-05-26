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
		float vertexScrollY{ 0 };
		float fragmentScrollY{ 0 };
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
		bool m_shadersEditorViewEnabled{ false };
		bool applyRequested{ false };
		bool revertRequested{ false };

		std::shared_ptr<nimo::SceneRenderer> m_renderer;
		nimo::RendererStats m_displayedStats;
		double m_frameTimeSamplesSum = 0;
		int m_refreshRate = 2;  // Stats display update frequency (times/sec)
		float m_timeDebugRefresh = 0;   // Time elapsed since last debug stats update
		float m_currentTime = 0;
		std::vector<PerformanceSample> m_samples;
		bool m_recordingSamples{ false };
		std::string m_recButtonText{ "Record" };
		std::string m_headerName{ "Samples#0" };
		int m_recordingTime{ 2 };
		unsigned int m_firstSample2DumpIdx { 0 };
		unsigned int m_lastSample2DumpIdx{ 0 };

		std::map<std::string, std::shared_ptr<ShaderEntry>> m_openShaders;

		const static ImGuiTreeNodeFlags TREENODE_BASE_FLAGS;
		static const ImGuiSelectableFlags SELECTABLE_BASE_FLAGS;
		static const ImGuiTabBarFlags TAB_BAR_BASE_FLAGS;

		bool renderShaderEditPanel(std::string& label, std::string* code, float& scrollY, float width, float height);
	};
}
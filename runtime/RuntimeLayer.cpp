#include "RuntimeLayer.h"
#include "project/ProjectSerializer.h"
#include "renderer/Renderer.h"
#include "renderer/DebugPass.h"
#include "scene/SceneManager.h"

void RuntimeLayer::OnAttach()
{
    nimo::ProjectSettings settings;
    auto project = std::make_shared<nimo::Project>(settings);
    nimo::ProjectSerializer ps(project);
    ps.Deserialize(projectFilepath);
    nimo::Project::SetActiveProject(project);
    nimo::AssetId startingSceneId = nimo::AssetId(nimo::Project::GetActiveProject()->GetSettings().startScene);
    NIMO_DEBUG("Loading scene {}", nimo::Project::GetActiveProject()->GetSettings().startScene);
    nimo::SceneManager::LoadScene(startingSceneId);
    renderer = std::make_shared<nimo::SceneRenderer>();
    m_renderPasses.push_back(std::make_shared<nimo::DebugPass>(renderer));
}
  
void RuntimeLayer::OnUpdate(float deltaTime)
{
    nimo::SceneManager::ProcessLoadRequests();
    /*nimo::Renderer::BeginFrame();*/
    nimo::SceneManager::UpdateScenes(deltaTime);
    for(auto scene : nimo::AssetManager::GetAllLoaded<nimo::Scene>())
    {
        renderer->SetScene(scene);
        renderer->Render({}, scene->GetMainCamera(), scene->GetMainCameraTransform(), deltaTime);

        for (const auto& renderPass : m_renderPasses)
        {
            renderPass->update(deltaTime);
            renderPass->render();
        }
    }
    nimo::Renderer::EndFrame();
}
#pragma once
#include "core/Layer.h"
#include "core/FileHandling.h"
#include "project/Project.h"
#include "renderer/SceneRenderer.h"

class RuntimeLayer : public nimo::Layer
{
public:
    RuntimeLayer(const std::filesystem::path& projectFilepath) : projectFilepath(projectFilepath){}
    void OnAttach();
    void OnUpdate(float deltaTime);
private:
    std::shared_ptr<nimo::SceneRenderer> renderer;
    std::filesystem::path projectFilepath;

    std::vector<std::shared_ptr<RenderPass>> m_renderPasses;
};
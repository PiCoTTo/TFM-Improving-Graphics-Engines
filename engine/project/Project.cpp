#include "Project.h"
#include "assets/AssetManager.h"
#include "scripting/ScriptManager.h"
#include "scene/SceneManager.h"
#include "events/EventManager.h"
#include "events/ProjectEvents.h"

nimo::Project::Project(const ProjectSettings& settings)
    : m_settings(settings)
{

}
nimo::Project::~Project()
{
}
void nimo::Project::SetActiveProject(std::shared_ptr<Project> p) 
{ 
    if(m_activeProject){
        SceneManager::SetActiveScene({});
        AssetManager::Cleanup();
        ScriptManager::Cleanup();
    }
    m_activeProject = p;
    if(m_activeProject){
        ScriptManager::Initialize();
        AssetManager::Initialize();
    }
    EventManager::Publish(ActiveProjectChanged());
}
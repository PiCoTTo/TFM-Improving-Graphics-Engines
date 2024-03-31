#pragma once
#include "Scene.h"

namespace nimo
{
class SceneManager
{
public:
    static std::shared_ptr<Scene> CreateScene(const std::string& name);
    static void LoadScene(const std::string& name);
    static void LoadScene(GUID id);
    static void UpdateScenes(float deltaTime);
    inline static std::shared_ptr<Scene> GetActiveScene() {return m_activeScene;}
    inline static void SetActiveScene(std::shared_ptr<Scene> scene) {m_activeScene = scene;}
    inline static void AddLoadRequest(const std::string& name){m_loadSceneRequests.push_back(name);}
    inline static void ProcessLoadRequests() { 
        if(m_loadSceneRequests.size() == 0) return;
        for(auto name : m_loadSceneRequests) 
            LoadScene(name);
        m_loadSceneRequests.clear();
    }
private:
    static std::shared_ptr<Scene> m_activeScene;
    static std::vector<std::string> m_loadSceneRequests;
};
};
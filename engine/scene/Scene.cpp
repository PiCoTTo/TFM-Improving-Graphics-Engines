#include "Scene.h"
#include "Components.h"
#include "renderer/Shader.h"
#include "glad/glad.h"
#include "project/Project.h"
#include "scripting/ScriptManager.h"

// const unsigned int NR_LIGHTS = 32;
// std::vector<glm::vec3> lightPositions;
// std::vector<glm::vec3> lightColors;
nimo::Scene::Scene(const std::string& name)
    : m_name(name)
{
    NIMO_DEBUG("nimo::Scene::Scene({})", name);
}
nimo::Scene::~Scene()
{
    NIMO_DEBUG("nimo::Scene::~Scene({})", m_name);
    ForEachEntity([](Entity entity)
    {
        if(entity.HasComponent<ScriptComponent>())
        {
            const auto& s = entity.GetComponent<ScriptComponent>();
            for(const auto& i : s.instances)
            {
                ScriptManager::DestroyInstance(i);
            }
        }
    });
}

void nimo::Scene::Update()
{
    m_registry.view<ActiveComponent, AudioSourceComponent>().each([&](ActiveComponent& active, AudioSourceComponent& audio)
    {
        if (!active.active)
        {
            audio.sound->Stop();
            return;
        }
        if(!audio.initialized)
        {
            audio.Apply();
            if(audio.playOnCreate)
            {
                if(audio.sound)
                    audio.sound->Play();
            }
            audio.initialized = true;
        }
        else if (audio.sound && !audio.sound->IsPlaying())
            audio.sound->Play();
    });
    m_registry.view<ActiveComponent, ScriptComponent>().each([&](ActiveComponent& active, ScriptComponent& script)
    {
        if(!active.active) return;
        for(auto& instance : script.instances)
        {
            if(!instance.initialized)
            {
                ScriptManager::OnCreate(instance);
                instance.initialized = true;
            }
            ScriptManager::OnUpdate(instance);
        }
    });
    m_registry.view<ActiveComponent, ScriptComponent>().each([&](ActiveComponent& active, ScriptComponent& script)
    {
        if(!active.active) return;
        for(auto& instance : script.instances)
        {
            ScriptManager::ExecuteCoroutines(instance);
        }
    });
}
void nimo::Scene::LateUpdate()
{
    m_registry.view<ActiveComponent, ScriptComponent>().each([&](ActiveComponent& active, ScriptComponent& script)
    {
        if(!active.active) return;
        for(auto& instance : script.instances)
        {
            ScriptManager::OnLateUpdate(instance);
        }
    });
    for(auto e : m_requestedEntitiesToDestroy)
    {
        DestroyEntity(e);
    }
    m_requestedEntitiesToDestroy.clear();
}
nimo::Entity nimo::Scene::CreateEntity(const std::string& name)
{
    auto id = m_registry.create();
    auto e = Entity(id, m_registry);
    e.AddComponent<IDComponent>().Id = GUID::Create();
    if(name == "")
        e.AddComponent<LabelComponent>().Label = e.GetComponent<IDComponent>().Id.Str();
    else
        e.AddComponent<LabelComponent>().Label = name;
    e.AddComponent<FamilyComponent>();
    e.AddComponent<TransformComponent>();
    e.AddComponent<ActiveComponent>();
    m_entities[e.GetComponent<IDComponent>().Id] = id;
    return e;
}
nimo::Entity nimo::Scene::CreateEntityWithID(GUID desiredId)
{
    auto id = m_registry.create();
    auto e = Entity(id, m_registry);
    e.AddComponent<IDComponent>().Id = desiredId;
    if(m_name == "")
        e.AddComponent<LabelComponent>().Label = e.GetComponent<IDComponent>().Id.Str();
    else
        e.AddComponent<LabelComponent>().Label = m_name;
    e.AddComponent<FamilyComponent>();
    e.AddComponent<TransformComponent>();
    e.AddComponent<ActiveComponent>();
    m_entities[e.GetComponent<IDComponent>().Id] = id;
    return e;
}

nimo::Entity nimo::Scene::CreateEntityWithParent(Entity parent, const std::string& name)
{
    auto e = CreateEntity(name);
    e.GetComponent<FamilyComponent>().Parent = parent.GetComponent<IDComponent>().Id;
    parent.GetComponent<FamilyComponent>().Children.push_back(e.GetComponent<IDComponent>().Id);
    return e;
}
void nimo::Scene::SetEntityActive(Entity e, bool active)
{
    bool prevActive = e.GetComponent<nimo::ActiveComponent>().active;
    e.GetComponent<ActiveComponent>().active = active;
    if(prevActive != active)
    {
        if(e.HasComponent<ScriptComponent>())
        {
            for(const auto& instance : e.GetComponent<ScriptComponent>().instances)
            {
                prevActive ? ScriptManager::OnDisable(instance) : ScriptManager::OnEnable(instance);
            }
        }
    }
    for(auto child : e.Children())
    {
        SetEntityActive(GetEntity(child), active);
    }
}

void nimo::Scene::DestroyEntity(Entity entity)
{
    if(entity.Parent().Valid())
    {
        auto parent = GetEntity(entity.Parent());
        parent.GetComponent<FamilyComponent>().Children.erase(
            std::remove(parent.GetComponent<FamilyComponent>().Children.begin(), parent.GetComponent<FamilyComponent>().Children.end(), entity.ID()),
             parent.GetComponent<FamilyComponent>().Children.end());
    }
    m_entities.erase(entity.ID());
    for(auto child : entity.Children())
    {
        DestroyEntity(GetEntity(child));
    }
    if(entity.HasComponent<ScriptComponent>())
    {
        const auto& s = entity.GetComponent<ScriptComponent>();
        for(const auto& i : s.instances)
        {
            ScriptManager::DestroyInstance(i);
        }
    }
    m_registry.destroy(entity.m_handle);
}

void nimo::Scene::ForEachEntity(std::function<void(Entity&)> action)
{
    m_registry.each([&](entt::entity e)
    {
        Entity ent(e, m_registry);
        action(ent);
    });
}

glm::mat4 nimo::Scene::GetWorldSpaceTransformMatrix(Entity entity)
{
    glm::mat4 transform(1.0f);

    if (entity.GetComponent<FamilyComponent>().Parent.Valid())
    {
        Entity parent = GetEntity(entity.GetComponent<FamilyComponent>().Parent);
        if (parent.GetComponent<IDComponent>().Id.Valid())
            transform = GetWorldSpaceTransformMatrix(parent);
    }

    return transform * entity.GetComponent<TransformComponent>().GetTransform();
}
void nimo::Scene::RequestEntityDestruction(Entity entity)
{
    m_requestedEntitiesToDestroy.push_back(entity);
}

nimo::CameraComponent nimo::Scene::GetMainCamera()
{
    Entity camera(*m_registry.view<CameraComponent>().begin(), m_registry);
    return camera.GetComponent<CameraComponent>();
}
nimo::TransformComponent nimo::Scene::GetMainCameraTransform()
{
    Entity camera(*m_registry.view<CameraComponent>().begin(), m_registry);
    return camera.GetComponent<TransformComponent>();
}


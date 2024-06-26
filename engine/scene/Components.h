#pragma once

#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "renderer/Mesh.h"
#include "renderer/Material.h"
#include "renderer/EnvironmentMap.h"
#include "core/GUID.h"
#include "scripting/ScriptInstance.h"
#include "audio/AudioSound.h"
#include "fonts/Font.h"

namespace nimo{
struct IDComponent
{
    GUID Id;
};
struct LabelComponent
{
    std::string Label;
};
struct ActiveComponent
{
    bool active = true;
};
struct FamilyComponent
{
    GUID Parent;
    std::vector<GUID> Children;

    FamilyComponent(GUID parent) : Parent(parent) {}
    FamilyComponent() = default;
    FamilyComponent(const FamilyComponent& other) = default;
};


static glm::vec3 defaultUpVector() { return glm::vec3(0.0f, 1.0f, 0.0f); }
static glm::vec3 defaultForwardVector() { return glm::vec3(0.0f, 0.0f, -1.0f); }
struct TransformComponent
{
    glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent& other) = default;
    TransformComponent(const glm::vec3& translation)
        : Translation(translation) {}

    glm::mat4 GetTransform() const;

    glm::mat4 GetView() const;

    glm::vec3 GetFront() const;

    glm::vec3 GetUp() const;

    glm::vec3 GetRight() const;
};

struct CameraComponent{
    enum class Projection : unsigned int{
        Perspective,
        Orthographic
    } Projection{Projection::Perspective};
    float FOV = 56.0f;
    struct ClippingPlanes{
        float Near{0.1f};
        float Far{100.0f};
    }ClippingPlanes;
    std::shared_ptr<Frustum> frustum;
};

struct PointLightComponent{
    glm::vec3 Color{1.0f, 1.0f, 1.0f};
    float Intensity = 3.0f;
};
struct DirectionalLightComponent{
    glm::vec3 Color{1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;
};
struct SkyLightComponent{
    glm::vec3 Color{1.0f, 1.0f, 1.0f};
    float Intensity = 1.0f;
    std::shared_ptr<EnvironmentMap> environment;
};

struct MeshComponent{
    std::shared_ptr<Mesh> source;
    unsigned int submeshIndex = 0;
    bool inFrustum = 1;
    glm::vec3 center;
};

struct MeshRendererComponent{
    std::shared_ptr<Material> material;
};
struct ScriptComponent{
    std::vector<ScriptInstance> instances;
};
struct AudioSourceComponent{
    std::shared_ptr<AudioSource> source;
    float volume = 0.5f;
    float pitch = 1.0f;
    float pan = 0.0f;
    bool loop = false;
    bool playOnCreate = false;
    std::unique_ptr<AudioSound> sound;
    bool initialized = false;
    void Apply(){
        if(source)
        {
            sound = std::make_unique<AudioSound>(source);
            sound->SetVolume(volume);
            sound->SetPan(pan);
            sound->SetPitch(pitch);
            sound->SetLoop(loop);
        }
    }
};
struct SpriteRendererComponent{
    glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
    std::shared_ptr<Texture> texture;
    glm::vec2 tiling = {1.0f, 1.0f};
    glm::vec2 offset = {0.0f, 0.0f};
    int layer = 0;
};
struct TextRendererComponent{
    glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
    std::shared_ptr<Font> font;
    std::string text = "New text";
    float characterSpacing = 0.0f;
};
};
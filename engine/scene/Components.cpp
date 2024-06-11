#include "Components.h"

glm::mat4 nimo::TransformComponent::GetTransform() const
{
    return glm::translate(glm::mat4(1.0f), Translation)
        * glm::toMat4(glm::quat(glm::radians(Rotation)))
        * glm::scale(glm::mat4(1.0f), Scale);
}

glm::mat4 nimo::TransformComponent::GetView() const
{
    auto orientation =
        glm::angleAxis(glm::radians(Rotation.x), glm::vec3(1, 0, 0)) *
        glm::angleAxis(glm::radians(Rotation.y), glm::vec3(0, 1, 0)) *
        glm::angleAxis(glm::radians(Rotation.z), glm::vec3(0, 0, 1));
    auto pUpVector = defaultUpVector() * orientation;
    auto pLookAt = Translation + (defaultForwardVector() * orientation);
    auto pView = glm::lookAt(Translation, pLookAt, pUpVector);
    return pView;
    // glm::vec3 dir;
    // dir.x = glm::cos(glm::radians(Rotation.x)) * glm::cos(glm::radians(Rotation.y + 90.0f));
    // dir.y = glm::sin(glm::radians(Rotation.x));
    // dir.z = glm::cos(glm::radians(Rotation.x)) * glm::sin(glm::radians(Rotation.y + 90.0f));
    // dir = glm::normalize(dir);
    // glm::quatLookAtLH()
    // return glm::lookAt(Translation, Translation + dir, glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, -1.0f))
    return glm::toMat4(glm::conjugate(glm::quat(glm::radians(Rotation))))
        * glm::translate(glm::mat4(1.0f), { Translation.x, Translation.y, -Translation.z });
}

glm::vec3 nimo::TransformComponent::GetFront() const
{
    auto orientation =
        glm::angleAxis(glm::radians(Rotation.x), glm::vec3(1, 0, 0)) *
        glm::angleAxis(glm::radians(Rotation.y), glm::vec3(0, 1, 0)) *
        glm::angleAxis(glm::radians(Rotation.z), glm::vec3(0, 0, 1));
    return defaultForwardVector() * orientation;
    //return normalize(glm::vec3(glm::inverse(GetTransform())[2]));
}

glm::vec3 nimo::TransformComponent::GetUp() const
{
    return glm::normalize(glm::cross(GetRight(), GetFront()));
    //return normalize(glm::vec3(glm::inverse(GetTransform())[1]));
}

glm::vec3 nimo::TransformComponent::GetRight() const
{
    return glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
    //return normalize(glm::vec3(glm::inverse(GetTransform())[0]));
}

#include "Entity.h"
#include <iostream>
#include "Components.h"

nimo::Entity::Entity(entt::entity handle, entt::registry& registry)
    : m_handle(handle)
    , m_registry(registry)
{

}
nimo::Entity::~Entity()
{
}

const nimo::GUID& nimo::Entity::ID() const
{
    return GetComponent<IDComponent>().Id;
}
const nimo::GUID& nimo::Entity::Parent() const
{
    return GetComponent<FamilyComponent>().Parent;
}
const std::vector<nimo::GUID>& nimo::Entity::Children() const
{
    return GetComponent<FamilyComponent>().Children;
}
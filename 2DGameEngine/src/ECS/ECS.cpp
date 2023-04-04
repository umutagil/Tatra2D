#include "ECS.h"

#include "../Logger/Logger.h"
#include <cassert>

// TODO: implementation

unsigned IComponent::nextId = 0;

Entity::Entity(const unsigned the_id)
    : id(the_id)
    , registry(nullptr)
{
}

void Entity::Kill()
{
    registry->KillEntity(*this);
}

void System::AddEntityToSystem(const Entity entity)
{
    entities.push_back(entity);
}

void System::RemoveEntityFromSystem(const Entity entity)
{
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
}

std::vector<Entity>& System::GetSystemEntities()
{
    return entities;
}

Signature System::GetComponentSignature() const
{
    return componentSignature;
}

Registry::~Registry()
{
    Logger::Log("Registry destructor called.");
}

void Registry::Update()
{
    for (Entity entity : entitiesToBeKilled) {
        RemoveEntityFromSystems(entity);

        const unsigned entityId = entity.GetId();
        entityComponentSignatures[entityId].reset();
        freeIds.push_back(entityId);
    }

    entitiesToBeKilled.clear();

    for (Entity entity : entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }

    entitiesToBeAdded.clear();
}

Entity Registry::CreateEntity()
{
    unsigned entityId;
    if (freeIds.empty()) {
        entityId = static_cast<unsigned>(numEntities++);
        if (entityId >= entityComponentSignatures.size()) {
            entityComponentSignatures.resize(entityId + 1);
        }
    }
    else {
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    Entity entity(entityId);
    entity.registry = this;
    entitiesToBeAdded.insert(entity);

    Logger::Log("Entity created with ID: " + std::to_string(entityId));

    return entity;
}

void Registry::KillEntity(const Entity entity)
{
    entitiesToBeKilled.insert(entity);
}

void Registry::AddEntityToSystems(Entity entity)
{   
    for (auto& system : systems) {
        if (isSystemInterestedInEntity(*system.second, entity)) {
            system.second->AddEntityToSystem(entity);
        }
    }
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
    for (auto& system : systems) {
        if (isSystemInterestedInEntity(*system.second, entity)) {
            system.second->RemoveEntityFromSystem(entity);
        }
    }
}

/**
 * @brief Does the entity has necessary components required by the system?
 * @param system 
 * @param entity 
 * @return @c true if it is interested in, else @c false.
 */
bool Registry::isSystemInterestedInEntity(const System& system, const Entity entity) const
{
    const unsigned entityId = entity.GetId();
    const Signature entityComponentSignature = entityComponentSignatures[entityId];
    const Signature systemComponentSignature = system.GetComponentSignature();
    return (systemComponentSignature & entityComponentSignature) == systemComponentSignature;
}

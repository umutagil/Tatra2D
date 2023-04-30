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
    //Logger::Log("Entity (" + std::to_string(id) + ") got destroyed.");
}

void Entity::Tag(const std::string& tag)
{
    registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const
{
    return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group)
{
    registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const
{
    return registry->EntityBelongsToGroup(*this, group);
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

        for (auto pool : componentPools) {
            //assert(pool); TODO: fix empty pool pointer
            if (pool) {
                pool->RemoveEntityFromPool(entityId);
            }
        }

        // Remove entity from the tags and groups
        RemoveEntityTag(entity);
        RemoveEntityGroup(entity);
    }

    entitiesToBeKilled.clear();

    for (Entity entity : entitiesToBeAdded) {
        AddEntityToSystems(entity);
    }

    entitiesToBeAdded.clear();
}

/**
 * @brief Creates an entity and adds it to list of pending entities.
 * 
 * In the next update these pending entities are added to systems which
 * are interested in those entities.
 */
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

    //Logger::Log("Entity created with ID: " + std::to_string(entityId));

    return entity;
}

/**
 * @brief Adds given entity in to-be-killed list.
 * 
 * @param entity.
*/
void Registry::KillEntity(const Entity entity)
{
    entitiesToBeKilled.insert(entity);
}

/**
 * @brief Adds given entity to the systems which are interested in it.
 * 
 * @param entity.
 */
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

void Registry::TagEntity(Entity entity, const std::string& tag)
{
    entityPerTag.emplace(tag, entity.GetId());
    tagPerEntity.emplace(entity.GetId(), tag);
}

void Registry::RemoveEntityTag(Entity entity)
{
    auto tagIt = tagPerEntity.find(entity.GetId());
    if (tagIt != tagPerEntity.end()) {
        entityPerTag.erase(tagIt->second);
        tagPerEntity.erase(tagIt);
    }
}

bool Registry::EntityHasTag(const Entity entity, const std::string& tag) const
{
    auto tagIt = tagPerEntity.find(entity.GetId());
    return (tagIt != tagPerEntity.end()) && (tagIt->second == tag);
}

Entity Registry::GetEntityByTag(const std::string& tag) const
{
    auto entityIt = entityPerTag.find(tag);
    return entityIt != entityPerTag.end() ? entityIt->second : Entity(INVALID_ENTITY_ID);
}

void Registry::GroupEntity(Entity entity, const std::string& group)
{
    groupPerEntity.emplace(entity.GetId(), group);
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
}

void Registry::RemoveEntityGroup(Entity entity)
{
    auto groupIt = groupPerEntity.find(entity.GetId());
    if (groupIt == groupPerEntity.end()) {
        return;
    }

    auto entitiesIt = entitiesPerGroup.find(groupIt->second);
    if (entitiesIt != entitiesPerGroup.end()) {
        std::set<Entity>& entities = entitiesIt->second;
        auto entityIt = entities.find(entity);
        if (entityIt != entities.end()) {
            entities.erase(entityIt);
        }
    }

    groupPerEntity.erase(groupIt);
}

bool Registry::EntityBelongsToGroup(const Entity entity, const std::string& group) const
{
    auto groupIt = groupPerEntity.find(entity.GetId());
    return (groupIt != groupPerEntity.end()) && (groupIt->second == group);
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const
{
    auto entitiesIt = entitiesPerGroup.find(group);
    if (entitiesIt == entitiesPerGroup.end()) {
        return std::vector<Entity>();
    }

    const std::set<Entity>& entitiesSet = entitiesIt->second;
    return std::vector<Entity>(entitiesSet.begin(), entitiesSet.end());
}

#pragma once

#include <vector>
#include <bitset>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>
#include <type_traits>

#include "../Logger/Logger.h"
#include <cassert>

class Registry;
class System;

const unsigned MAX_COMPONENTS = 32;
const unsigned INVALID_ENTITY_ID = -1;

// Bitset to keep track of which components an entity has.
// Also used for the system to keep track of entities that are affected.
using Signature = std::bitset<MAX_COMPONENTS>;

struct IComponent
{
protected:
	static unsigned nextId;
};

/**
 * @brief Component
 * @tparam T 
 */
template<typename T>
class Component : public IComponent
{
public:
	inline static unsigned GetId();
};

/**
 * @brief Entity
 */
class Entity
{
public:
	Entity(const unsigned the_id);

	void Kill();

	inline unsigned GetId() const { return id; };

	inline Entity& operator =(const Entity& other) = default;
	inline bool operator ==(const Entity& other) const { return id == other.GetId(); };
	inline bool operator !=(const Entity& other) const { return id != other.GetId(); };
	inline bool operator <(const Entity& other) const { return id < other.GetId(); }
	inline bool operator >(const Entity& other) const { return id > other.GetId(); }

	template<typename TComponent, typename ...TArgs>
	void AddComponent(TArgs&& ...args);

	template<typename TComponent, typename ...TArgs>
	void RemoveComponent();

	template<typename TComponent, typename ...TArgs>
	bool HasComponent() const;

	template<typename TComponent, typename ...TArgs>
	TComponent& GetComponent() const;

	// Manage tags and groups
	void Tag(const std::string& tag);
	bool HasTag(const std::string& tag) const;
	void Group(const std::string& group);
	bool BelongsToGroup(const std::string& group) const;

private:
	unsigned id;

public:
	Registry* registry;
};

/**
 * @brief System
 */
class System
{

public:
	System() = default;
	~System() = default;

	void AddEntityToSystem(const Entity entity);
	void RemoveEntityFromSystem(const Entity entity);
	std::vector<Entity>& GetSystemEntities();
	Signature GetComponentSignature() const;

	template <typename T>
	void RequireComponent();

protected:
	Signature componentSignature;

	std::vector<Entity> entities;

};


class IPool
{
public:
	virtual ~IPool() {}
	virtual void RemoveEntityFromPool(const unsigned entityId) = 0;
};

/**
 * @brief Pool
 * @tparam T 
 */
template<typename T>
class Pool : public IPool
{
public:
	Pool(const size_t capacity = 100)
	{ 
		size = 0;
		data.reserve(size);
	}
	virtual ~Pool() = default;

	bool IsEmpty() const					{ return size == 0; }
	size_t GetSize() const					{ return size; }
	void Resize(size_t new_size)			{ data.resize(new_size); }
	
	void Clear()							{ data.clear(); size = 0; }
	void Add(T object)						{ data.push_back(object); }

	void Set(const unsigned entityId, T object);
	void Remove(const unsigned entityId);

	virtual void RemoveEntityFromPool(const unsigned entityId) override;

	T& Get(const unsigned entityId);
	T& operator [](size_t index)			{ return data[index]; }


private:
	// Vector of component objects
	std::vector<T> data;

	// Current size of component objects
	size_t size;

	// Maps to keep track of entity Ids and their indices.
	std::unordered_map<unsigned, size_t> entityIdToIndex;
	std::unordered_map<size_t, unsigned> indexToEntityId;
};

/**
 * @brief Manages the creation and destruction of entities, systems and components.
 */
class Registry
{
public:
	Registry() = default;
	~Registry();

	void Update();

	// Entity management

	Entity CreateEntity();
	void KillEntity(const Entity entity);

	// Component management

	template<typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&& ...args);

	template<typename TComponent>
	void RemoveComponent(Entity entity);

	template<typename TComponent>
	bool HasComponent(Entity entity) const;

	template<typename TComponent>
	TComponent& GetComponent(Entity entity) const;

	template<typename TSystem, typename ...TArgs>
	void AddSystem(TArgs&& ...args);

	template<typename TSystem>
	void RemoveSystem();

	template<typename TSystem>
	bool HasSystem() const;

	template<typename TSystem>
	TSystem& GetSystem() const;

	// Tag management
	void TagEntity(Entity entity, const std::string& tag);
	void RemoveEntityTag(Entity entity);
	bool EntityHasTag(const Entity entity, const std::string& tag) const;
	Entity GetEntityByTag(const std::string& tag) const;

	// Group management
	void GroupEntity(Entity entity, const std::string& group);
	void RemoveEntityGroup(Entity entity);
	bool EntityBelongsToGroup(const Entity entity, const std::string& group) const;
	std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;

private:
	// Add and remove entities to/from systems.
	void AddEntityToSystems(Entity entity);
	void RemoveEntityFromSystems(Entity entity);

	bool isSystemInterestedInEntity(const System& system, const Entity entity) const;

private:
	size_t numEntities = 0;

	// List of entity IDs, that can be reused.
	std::deque<unsigned> freeIds;

	// Set of entities to added or removed in the next registry update.
	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeKilled;

	/**
	 * @brief Vector of component pools, where each element
	 * contains all the data for a certain component type.
	 * Vector index is entity ID.
	 */
	std::vector<std::shared_ptr<IPool>> componentPools;
	
	/**
	 * @brief Vector of component signatures, where each element
	 * shows which components are active for an entity.
	 * Vector index is entity ID.
	 */
	std::vector<Signature> entityComponentSignatures;

	// @brief Map of all systems.
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Entity tags
	std::unordered_map<std::string, unsigned> entityPerTag;
	std::unordered_map<unsigned, std::string> tagPerEntity;

	// Entity groups
	std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
	std::unordered_map<unsigned, std::string> groupPerEntity;

};


/**
 * @brief Defines the component type that entities must have.
 * @tparam T 
 */
template<typename T>
inline void System::RequireComponent()
{
	static_assert(!std::is_base_of_v<System, T>);
	const auto componentId = Component<T>::GetId();
	componentSignature.set(componentId);
}

template<typename T>
inline unsigned Component<T>::GetId()
{
	static unsigned id = nextId++;
	return id;
}

template<typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args)
{
	const unsigned componentId = Component<TComponent>::GetId();
	const unsigned entityId = entity.GetId();

	if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	if (!componentPools[componentId]) {
		std::shared_ptr<Pool<TComponent>> componentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentId] = componentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

	TComponent newComponent(std::forward<TArgs>(args)...);
	componentPool->Set(entityId, newComponent);

	entityComponentSignatures[entityId].set(componentId);

	//Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template<typename TComponent>
void Registry::RemoveComponent(Entity entity)
{
	const unsigned entityId = entity.GetId();
	const unsigned componentId = Component<TComponent>::GetId();

	// Remove the component from the pool.
	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	componentPool->Remove(entityId);

	// Set component signature to false for this entity.
	entityComponentSignatures[entityId].set(componentId);

	//Logger::Log("Component (ID: " + std::to_string(componentId) + ")" + " removed from the entity (ID: " + std::to_string(entityId) + ")");
}

template<typename TComponent>
bool Registry::HasComponent(Entity entity) const
{
	const unsigned entityId = entity.GetId();
	const unsigned componentId = Component<TComponent>::GetId();

	return entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const
{
	const unsigned entityId = entity.GetId();
	const unsigned componentId = Component<TComponent>::GetId();

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
	return componentPool->Get(entityId);
}

template<typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args)
{
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
void Registry::RemoveSystem()
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template<typename TSystem>
bool Registry::HasSystem() const
{
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template<typename TSystem>
TSystem& Registry::GetSystem() const
{
	auto system = systems.find(std::type_index(typeid(TSystem)));
	std::shared_ptr<TSystem> system_ptr = std::static_pointer_cast<TSystem>(system->second);
	return *system_ptr;
}

template<typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	static_assert(!std::is_base_of_v<System, TComponent>);
	registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template<typename TComponent, typename ...TArgs>
void Entity::RemoveComponent()
{
	registry->RemoveComponent<TComponent>(*this);
}

template<typename TComponent, typename ...TArgs>
bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>(*this);
}

template<typename TComponent, typename ...TArgs>
TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>(*this);
}

// Pool related functions

template<typename T>
inline void Pool<T>::Set(const unsigned entityId, T object)
{
	auto indexIt = entityIdToIndex.find(entityId);
	if (indexIt != entityIdToIndex.end()) {
		const size_t index = indexIt->second;
		data[index] = object;
		return;
	}

	if (size >= data.capacity()) {
		data.reserve(data.capacity() * 2);
	}

	const size_t index = size;
	entityIdToIndex.emplace(entityId, index);
	indexToEntityId.emplace(index, entityId);
	
	if (index == data.size()) {
		data.emplace_back(object);
	}
	else {
		data[index] = object;
	}

	++size;
}

template<typename T>
inline void Pool<T>::Remove(const unsigned entityId)
{
	const size_t indexOfRemoved = entityIdToIndex[entityId];
	const size_t indexOfLast = size - 1;
	data[indexOfRemoved] = data[indexOfLast];

	const unsigned entityIdOfLast = indexToEntityId[indexOfLast];
	indexToEntityId[indexOfRemoved] = entityIdOfLast;
	indexToEntityId.erase(indexOfLast);

	entityIdToIndex[entityIdOfLast] = indexOfRemoved;
	entityIdToIndex.erase(entityId);

	--size;
}

template<typename T>
inline void Pool<T>::RemoveEntityFromPool(const unsigned entityId)
{
	if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
		Remove(entityId);
	}
}

template<typename T>
inline T& Pool<T>::Get(const unsigned entityId)
{
	const size_t index = entityIdToIndex[entityId];
	return data[index];
}

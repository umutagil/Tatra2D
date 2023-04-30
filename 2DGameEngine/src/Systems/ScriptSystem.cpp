#include "ScriptSystem.h"

#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Logger/Logger.h"

#include <sol/sol.hpp>
#include <tuple>

std::tuple<double, double> GetEntityPosition(Entity entity)
{
	if (!entity.HasComponent<TransformComponent>()) {
		Logger::Err("Trying to get position but the entity has no transform component.");
		return std::make_tuple(0.0, 0.0);
	}

	const TransformComponent& transform = entity.GetComponent<TransformComponent>();
	return std::make_tuple(transform.position.x, transform.position.y);
}

void SetEntityPosition(Entity entity, const double xPos, const double yPos)
{
	if (! entity.HasComponent<TransformComponent>()) {
		Logger::Err("Trying to set position but the entity has no transform component.");
		return;
	}

	TransformComponent& transform = entity.GetComponent<TransformComponent>();
	transform.position.x = static_cast<float>(xPos);
	transform.position.y = static_cast<float>(yPos);
}

std::tuple<double, double> GetEntityVelocity(Entity entity)
{
	if (!entity.HasComponent<RigidBodyComponent>()) {
		Logger::Err("Trying to get velocity but the entity has no rigidbody component.");
		return std::make_tuple(0.0, 0.0);
	}

	const RigidBodyComponent& rigidbody = entity.GetComponent<RigidBodyComponent>();
	return std::make_tuple(rigidbody.velocity.x, rigidbody.velocity.y);
}

void SetEntityVelocity(Entity entity, const double velocityX, const double velocityY)
{
	if (!entity.HasComponent<RigidBodyComponent>()) {
		Logger::Err("Trying to set velocity but the entity has no rigidbody component.");
		return;
	}

	RigidBodyComponent& rigidbody = entity.GetComponent<RigidBodyComponent>();
	rigidbody.velocity = glm::vec2(velocityX, velocityY);
}

double GetEntityRotation(Entity entity)
{
	if (!entity.HasComponent<TransformComponent>()) {
		Logger::Err("Trying to get rotation but the entity has no transform component.");
		return 0.0f;
	}

	const TransformComponent& transform = entity.GetComponent<TransformComponent>();
	return transform.rotation;
}

void SetEntityRotation(Entity entity, const double angle)
{
	if (!entity.HasComponent<TransformComponent>()) {
		Logger::Err("Trying to set rotation but the entity has no transform component.");
		return;
	}

	TransformComponent& transform = entity.GetComponent<TransformComponent>();
	transform.rotation = angle;
}

std::tuple<double, double> GetProjectileVelocity(Entity entity)
{
	if (!entity.HasComponent<ProjectileEmitterComponent>()) {
		Logger::Err("Trying to get projectile velocity but the entity has no projectile emitter component.");
		return std::make_tuple(0.0, 0.0);
	}

	const ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
	return std::make_tuple(projectileEmitter.projectileVelocity.x, projectileEmitter.projectileVelocity.y);
}

void SetProjectileVelocity(Entity entity, const double velocityX, const double velocityY)
{
	if (!entity.HasComponent<ProjectileEmitterComponent>()) {
		Logger::Err("Trying to set velocity but the entity has no projectile emitter component.");
		return;
	}

	ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
	projectileEmitter.projectileVelocity = glm::vec2(velocityX, velocityY);
}

void SetEntityAnimationFrame(Entity entity, const int frameIdx)
{
	if (!entity.HasComponent<AnimationComponent>()) {
		Logger::Err("Trying to set animation frame but the entity has no animation component.");
		return;
	}

	AnimationComponent& animation = entity.GetComponent<AnimationComponent>();
	animation.currentFrame = frameIdx;
}

ScriptSystem::ScriptSystem()
{
	RequireComponent<ScriptComponent>();
}

void ScriptSystem::CreateLuaBindings(sol::state& lua)
{
	// Create the "entity" user type for Lua
	lua.new_usertype<Entity>(
		"entity",
		"get_id", &Entity::GetId,
		"destroy", &Entity::Kill,
		"has_tag", &Entity::HasTag,
		"belongs_to_group", &Entity::BelongsToGroup
	);

	// Create the bindings between C++ and Lua functions
	lua.set_function("get_position", GetEntityPosition);
	lua.set_function("set_position", SetEntityPosition);
	lua.set_function("get_velocity", GetEntityVelocity);
	lua.set_function("set_velocity", SetEntityVelocity);
	lua.set_function("get_rotation", GetEntityRotation);
	lua.set_function("set_rotation", SetEntityRotation);
	lua.set_function("get_projectile_velocity", GetProjectileVelocity);
	lua.set_function("set_projectile_velocity", SetProjectileVelocity);
	lua.set_function("set_animation_frame", SetEntityAnimationFrame);
}

void ScriptSystem::Update(const float deltaTime, const double elapsedtime)
{
	for (Entity entity : GetSystemEntities()) {
		const ScriptComponent& script = entity.GetComponent<ScriptComponent>();
		script.func(entity, deltaTime, static_cast<double>(elapsedtime));
	}
}

#include "DebugRenderSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Systems/CollisionSystem.h"
#include "../Logger/Logger.h"

#include <SDL.h>
#include <algorithm>

const Uint8 colliderColorCollision[4] = { 255, 0, 0 , 255 };
const Uint8 colliderColorNoCollision[4] = { 0, 255, 0 , 255 };

DebugRenderSystem::DebugRenderSystem()
	: colliderDrawingEnabled(true)
{
	RequireComponent<TransformComponent>();
	RequireComponent<BoxColliderComponent>();
}

void DebugRenderSystem::Update(SDL_Renderer* renderer)
{
	if (colliderDrawingEnabled) {
		DrawColliders(renderer);
	}
}

void DebugRenderSystem::HandleInput(const SDL_Event& event)
{
}


void DebugRenderSystem::DrawColliders(SDL_Renderer* renderer)
{
	for (Entity entity : GetSystemEntities()) {

		bool isColliding = false;
		const Aabb aabb(GetEntityAabb(entity));

		for (Entity entityOther : GetSystemEntities()) {

			if (entity == entityOther) {
				continue;
			}

			const Aabb aabbOther(GetEntityAabb(entityOther));
			if (aabb.Overlaps(aabbOther)) {
				isColliding = true;
				break;
			}
		}

		const Uint8* collisionColor = isColliding ? colliderColorCollision : colliderColorNoCollision;
		SDL_SetRenderDrawColor(renderer, collisionColor[0], collisionColor[1], collisionColor[2], collisionColor[3]);

		SDL_Rect boxColliderRect = {
			aabb.min.x,
			aabb.min.y,
			aabb.GetWidth(),
			aabb.GetHeight(),
		};

		SDL_RenderDrawRect(renderer, &boxColliderRect);
	}
}

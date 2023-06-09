#include "CameraMovementSystem.h"

#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../Game/Game.h"

#include <algorithm>

CameraMovementSystem::CameraMovementSystem()
{
	RequireComponent<CameraFollowComponent>();
	RequireComponent<TransformComponent>();
}

void CameraMovementSystem::Update(SDL_Rect& camera)
{
	for (Entity entity : GetSystemEntities()) {
		TransformComponent& transform = entity.GetComponent<TransformComponent>();

		if (camera.w < Game::mapWidth) {
			const int cameraX = static_cast<int>(transform.position.x) - (camera.w / 2);
			camera.x = std::clamp(cameraX, 0, Game::mapWidth - camera.w);
		}
		
		if (camera.h < Game::mapHeight) {
			const int cameraY = static_cast<int>(transform.position.y) - (camera.h / 2);
			camera.y = std::clamp(cameraY, 0, Game::mapHeight - camera.h);
		}
	}
}

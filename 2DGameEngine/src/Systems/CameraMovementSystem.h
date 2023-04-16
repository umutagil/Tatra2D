#pragma once

#include "../ECS/ECS.h"
#include <SDL_rect.h>

class CameraMovementSystem : public System
{
public:
	CameraMovementSystem();

	void Update(SDL_Rect& camera);
};



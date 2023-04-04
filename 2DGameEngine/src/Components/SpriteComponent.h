#pragma once

#include<string>
#include<SDL.h>

struct SpriteComponent
{
	SpriteComponent(std::string assetId = std::string(), const int width = 0, const int height = 0, const int zIndex = 0, const int srcRectX = 0, const int srcRectY = 0);

	std::string assetId;
	int width;
	int height;
	int zIndex;
	SDL_Rect srcRect;
	
};


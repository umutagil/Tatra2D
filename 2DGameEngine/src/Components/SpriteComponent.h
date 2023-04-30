#pragma once

#include<string>
#include<SDL.h>

struct SpriteComponent
{
	SpriteComponent(std::string assetId = std::string(), const int width = 0, const int height = 0, 
					const int zIndex = 0, const bool isFixed = false, const int srcRectX = 0, const int srcRectY = 0)
		: assetId(assetId)
		, width(width)
		, height(height)
		, zIndex(zIndex)
		, flip(SDL_FLIP_NONE)
		, isFixed(isFixed)
		, srcRect{ srcRectX, srcRectY, width, height }
	{
	}

	std::string assetId;
	int width;
	int height;
	int zIndex;
	SDL_RendererFlip flip;
	bool isFixed;
	SDL_Rect srcRect;
	
};


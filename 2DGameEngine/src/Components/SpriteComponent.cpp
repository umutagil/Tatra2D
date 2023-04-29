#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(std::string assetId, const int width, const int height, 
								const int zIndex, const bool isFixed, const int srcRectX, const int srcRectY)
	: assetId(assetId)
	, width(width)
	, height(height)
	, zIndex(zIndex)
	, flip(SDL_FLIP_NONE)
	, isFixed(isFixed)
	, srcRect{srcRectX, srcRectY, width, height}
{
}

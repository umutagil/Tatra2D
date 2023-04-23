#include "RenderTextSystem.h"

#include "../Components/TextLabelComponent.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_rect.h>

RenderTextSystem::RenderTextSystem()
{
	RequireComponent<TextLabelComponent>();

}

void RenderTextSystem::Update(SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera)
{
	for (Entity entity : GetSystemEntities()) {
		const TextLabelComponent& textLabel = entity.GetComponent<TextLabelComponent>();

		SDL_Surface* surface = TTF_RenderText_Blended(assetStore.GetFont(textLabel.assetId), textLabel.text.c_str(), textLabel.color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(&renderer, surface);
		SDL_FreeSurface(surface);

		const int textPosX = static_cast<int>(textLabel.position.x) - (textLabel.isFixed ? 0 : camera.x);
		const int textPosY = static_cast<int>(textLabel.position.y) - (textLabel.isFixed ? 0 : camera.y);

		int labelWidth = 0;
		int labelHeight = 0;
		SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);

		const SDL_Rect destRect = {
			textPosX,
			textPosY,
			labelWidth,
			labelHeight
		};

		SDL_RenderCopy(&renderer, texture, nullptr, &destRect);
	}
}

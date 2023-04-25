#pragma once

#include <glm/glm.hpp>
#include <SDL.h>
#include <string>

struct TextLabelComponent
{
	TextLabelComponent(const glm::vec2 position = glm::vec2(0), const std::string& text = "",
						const std::string& fontAssetId = "", const SDL_Color& color = { 0, 0, 0 }, const bool isFixed = true)
		: position(position)
		, text(text)
		, fontAssetId(fontAssetId)
		, color(color)
		, isFixed(isFixed)
	{
	}

	glm::vec2 position;
	std::string text;
	std::string fontAssetId;
	SDL_Color color;
	bool isFixed;
};




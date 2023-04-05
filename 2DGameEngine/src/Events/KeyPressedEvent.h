#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

#include <SDL.h>

class KeyPressedEvent : public Event
{
public:
	KeyPressedEvent(const SDL_Keycode pressedKey) : key(pressedKey) {}

public:
	SDL_Keycode key;
};
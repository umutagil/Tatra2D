#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

#include <SDL.h>

class MouseButtonEvent : public Event
{
public:
	MouseButtonEvent(const SDL_MouseButtonEvent mouseButtonEvent) : mouseButton(mouseButtonEvent) {}

public:
	SDL_MouseButtonEvent mouseButton;
};

class MouseMotionEvent : public Event
{
public:
	MouseMotionEvent(const SDL_MouseMotionEvent mouseMoveEvent) : mousemotion(mouseMoveEvent) {}

public:
	SDL_MouseMotionEvent mousemotion;
};
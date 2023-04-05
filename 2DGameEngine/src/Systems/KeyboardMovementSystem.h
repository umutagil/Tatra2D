#pragma once

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardMovementSystem : public System
{
public:
	KeyboardMovementSystem();

	void SubscribeToEvents(EventBus& eventBus);

private:
	void OnKeyPressed(KeyPressedEvent& event);
};


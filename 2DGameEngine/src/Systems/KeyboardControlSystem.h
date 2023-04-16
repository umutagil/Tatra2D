#pragma once

#include "../ECS/ECS.h"

class EventBus;
class KeyPressedEvent;

class KeyboardControlSystem : public System
{
public:
	KeyboardControlSystem();

	void SubscribeToEvents(EventBus& eventBus);

private:
	void OnKeyPressed(KeyPressedEvent& event);
};


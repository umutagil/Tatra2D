#include "KeyboardMovementSystem.h"
#include "../Logger/Logger.h"

KeyboardMovementSystem::KeyboardMovementSystem()
{

}

void KeyboardMovementSystem::OnKeyPressed(KeyPressedEvent& event)
{
	Logger::Log("Key pressed!");
}

void KeyboardMovementSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvents(this, &KeyboardMovementSystem::OnKeyPressed);
}

#include "KeyboardControlSystem.h"

#include "../Logger/Logger.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

KeyboardControlSystem::KeyboardControlSystem()
{
	RequireComponent<KeyboardControlledComponent>();
	RequireComponent<RigidBodyComponent>();
	RequireComponent<SpriteComponent>();
}

void KeyboardControlSystem::OnKeyPressed(KeyPressedEvent& event)
{
	// Change the sprite and the velocity of the entity.

	for (Entity entity : GetSystemEntities()) {
		const KeyboardControlledComponent& keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
		SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
		RigidBodyComponent& rigidbody = entity.GetComponent<RigidBodyComponent>();

		const int yOffset = sprite.height;

		switch (event.key) {
			
			case SDLK_UP:
				sprite.srcRect.y = 0;
				rigidbody.velocity = keyboardControl.velocityUp;
				break;

			case SDLK_RIGHT:
				sprite.srcRect.y = yOffset;
				rigidbody.velocity = keyboardControl.velocityRight;
				break;

			case SDLK_DOWN:
				sprite.srcRect.y = yOffset * 2;
				rigidbody.velocity = keyboardControl.velocityDown;
				break;

			case SDLK_LEFT:
				sprite.srcRect.y = yOffset * 3;
				rigidbody.velocity = keyboardControl.velocityLeft;
				break;

			default:
				break;
		}
	}
}

void KeyboardControlSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvents(this, &KeyboardControlSystem::OnKeyPressed);
}

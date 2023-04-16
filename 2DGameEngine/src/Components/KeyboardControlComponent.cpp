#include "KeyboardControlComponent.h"

KeyboardControlledComponent::KeyboardControlledComponent(glm::vec2 upVelocity, glm::vec2 rightVelocity, glm::vec2 downVelocity, glm::vec2 leftVelocity)
	: velocityUp(upVelocity)
	, velocityRight(rightVelocity)
	, velocityDown(downVelocity)
	, velocityLeft(leftVelocity)
{
}

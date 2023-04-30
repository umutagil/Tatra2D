#pragma once

#include <glm/glm.hpp>

struct KeyboardControlledComponent
{
	KeyboardControlledComponent(glm::vec2 upVelocity = glm::vec2(0.0f), glm::vec2 rightVelocity = glm::vec2(0.0f),
								glm::vec2 downVelocity = glm::vec2(0.0f), glm::vec2 leftVelocity = glm::vec2(0.0f))
		: velocityUp(upVelocity)
		, velocityRight(rightVelocity)
		, velocityDown(downVelocity)
		, velocityLeft(leftVelocity)
	{
	}

	glm::vec2 velocityUp;
	glm::vec2 velocityRight;
	glm::vec2 velocityDown;
	glm::vec2 velocityLeft;

};



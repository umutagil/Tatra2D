#pragma once

#include <glm/glm.hpp>

struct RigidBodyComponent
{
	RigidBodyComponent(const glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	glm::vec2 velocity;
};

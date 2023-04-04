#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent
{
	BoxColliderComponent(const int width = 0, const int height = 0, const glm::vec2 offset = glm::vec2(0));

	int width;
	int height;
	glm::vec2 offset;
};
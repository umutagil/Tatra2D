#pragma once

#include <glm/glm.hpp>

/**
 * @brief Component that stores collision box parameters.
 * 
 * Currently entity scale is not reflected on the collider automatically,
 * it needs to be done manually.
 * TODO: Find a way to set it easily.
 */
struct BoxColliderComponent
{
	BoxColliderComponent(const int width = 0, const int height = 0, const glm::vec2 offset = glm::vec2(0));

	int width;
	int height;
	glm::vec2 offset;
};
#pragma once

#include <glm/glm.hpp>

struct TransformComponent
{
	TransformComponent(glm::vec2 position = glm::vec2(0.0f, 0.0f), glm::vec2 scale= glm::vec2(1.0f, 1.0f), const float rotation = 0.0);

	glm::vec2 position;
	glm::vec2 scale;
	double rotation;
};


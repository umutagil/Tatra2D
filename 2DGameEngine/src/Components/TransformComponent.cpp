#include "TransformComponent.h"

TransformComponent::TransformComponent(glm::vec2 position /* vec2(0.0f, 0.0f,) */, glm::vec2 scale /* = vec2(1.0f, 1.0f,) */, const float rotation /* = 0.0 */)
	: position(position)
	, scale(scale)
	, rotation(rotation)
{
}

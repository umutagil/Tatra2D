#pragma once

#include <glm/glm.hpp>
#include <assert.h>

struct Aabb
{
	Aabb(const glm::vec2& min, const glm::vec2& max);
	bool Overlaps(const Aabb& aabb) const;
	bool Contains(const glm::vec2 pos) const;
	float GetWidth() const;
	float GetHeight() const;

	glm::vec2 min;
	glm::vec2 max;
};
#include "Geometry.h"

Aabb::Aabb(const glm::vec2& min, const glm::vec2& max)
	: min(min)
	, max(max)
{
	assert(glm::all(glm::lessThanEqual(min, max)));
}

bool Aabb::Overlaps(const Aabb& aabb) const
{
	return glm::all(glm::lessThanEqual(min, aabb.max)) && glm::all(glm::greaterThanEqual(max, aabb.min));
}

bool Aabb::Contains(const glm::vec2 pos) const
{
	return glm::all(glm::lessThanEqual(min, pos)) && glm::all(glm::greaterThanEqual(max, pos));
}

float Aabb::GetWidth() const
{
	return max.x - min.x;
}

float Aabb::GetHeight() const
{
	return max.y - min.y;
}
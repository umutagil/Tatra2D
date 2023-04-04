#pragma once

#include <glm/glm.hpp>

bool findIntersection(glm::vec2& out, const glm::vec2 p0, const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 p3)
{
	const float a1 = p1.y - p0.y;
	const float b1 = p0.x - p1.x;
	const float c1 = a1 * p0.x + b1 * p0.y;

	const float a2 = p3.y - p2.y;
	const float b2 = p2.x - p3.x;
	const float c2 = a2 * p2.x + b2 * p2.y;

	const float denominator = a1 * b2 - a2 * b1;

	if (denominator == 0) {
		// Lines are either parallel or co-linear.
		return false;
	}

	const float resX = (c1 * b2 - c2 * b1) / denominator;
	const float resY = (a1 * c2 - a2 * c1) / denominator;
	out = glm::vec2(resX, resY);

	return true;
}



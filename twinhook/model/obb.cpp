#include "stdafx.h"
#include "obb.h"

std::vector<vec2> obb::toVertices(const vec2& position, float length, float radius, float angle)
{
	return {
		vec2(0, radius).rotate(angle) + position,
		vec2(0, -radius).rotate(angle) + position,
		vec2(length, -radius).rotate(angle) + position,
		vec2(length, radius).rotate(angle) + position
	};
}

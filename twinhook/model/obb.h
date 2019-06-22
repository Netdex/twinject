#pragma once
#include <memory>
#include <string>
#include <utility>

#include "util/vec2.h"
#include "polygon.h"

class obb : public polygon
{
	static std::vector<vec2> toVertices(const vec2& position,
		float length, float radius, float angle);
public:
	obb(const vec2& position, float length, float radius, float angle, const vec2& velocity = vec2())
		: polygon(toVertices(position, length, radius, angle), velocity) {}
};

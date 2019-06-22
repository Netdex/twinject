#pragma once

#include <memory>
#include <string>
#include <utility>

#include "entity.h"

class aabb : public entity
{
public:
	vec2 position;
	vec2 size;

	aabb(const vec2& position = vec2(), const vec2& velocity = vec2(), const vec2& size = vec2(1, 1))
		: entity(AABB, velocity),
		position(position), size(size) {}


	aabb(const aabb& other)
		: entity{other}, position{other.position}, size{other.size} {}

	vec2 com() const override;
	std::shared_ptr<entity> translate(vec2 delta) const override;
	std::shared_ptr<entity> withVelocity(vec2 newVelocity) const override;
	std::shared_ptr<aabb> boundingBox() const override;

	float willExit(const entity& o) const override;
	float willCollideWith(const entity& o) const override;

	polygon toPolygon() const;

	std::ostream& serialize(std::ostream& os) const override;
};

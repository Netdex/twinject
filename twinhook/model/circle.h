#pragma once

#include <memory>
#include <string>
#include <utility>

#include "util/vec2.h"
#include "entity.h"

class aabb;

class circle : public entity
{
public:
	vec2 center;
	float radius;

	circle(const vec2& center = vec2(), const vec2& velocity = vec2(), float radius = 1)
		:entity(Circle, velocity), center(center), radius(radius) {}

	vec2 com() const override;
	std::shared_ptr<entity> translate(vec2 delta) const override;
	std::shared_ptr<entity> withVelocity(vec2 newVelocity) const override;
	std::shared_ptr<aabb> boundingBox() const override;

	float willExit(const entity& o) const override;
	float willCollideWith(const entity& o) const override;

	std::ostream& serialize(std::ostream& os) const override;

};

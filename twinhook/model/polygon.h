#pragma once

#include <memory>
#include <string>
#include <utility>

#include "util/vec2.h"
#include "entity.h"

class polygon : public entity
{
public:
	std::vector<vec2> points;

	polygon(std::vector<vec2> points, const vec2& velocity = vec2())
		: entity(Polygon, velocity), points(std::move(points)) {}

	vec2 com() const override;
	std::shared_ptr<entity> translate(vec2 delta) const override;
	std::shared_ptr<entity> withVelocity(vec2 newVelocity) const override;
	std::shared_ptr<aabb> boundingBox() const override;

	float willExit(const entity& o) const override;
	float willCollideWith(const entity& o) const override;

	std::ostream& serialize(std::ostream& os) const override;
};

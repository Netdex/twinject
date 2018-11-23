#pragma once

#include "util/vec2.h"
#include <memory>


class entity
{
protected:
	enum entity_type
	{
		AABB,
		Circle,
		Polygon
	};

	entity(entity_type type, vec2 velocity = vec2())
		: type(type), velocity(velocity) {}

public:
	entity_type type;
	vec2 velocity;

	virtual ~entity() = default;

	virtual std::shared_ptr<entity> translate(vec2 delta) = 0;

	/**
	 * \brief Predict time until collision between this entity and another
	 * \param o The other entity
	 * \return Result of the vector collision predictor (frames until collision)
	 */
	virtual float willCollideWith(const entity &o) = 0;

	std::shared_ptr<entity> entityAtCollision(const entity &o);
};

class aabb : public entity
{
public:
	vec2 position;
	vec2 size;

	aabb(vec2 position = vec2(), vec2 velocity = vec2(), vec2 size = vec2(1,1))
		: entity(AABB, velocity), position(position), size(size) {}

	std::shared_ptr<entity> translate(vec2 delta) override;

	float willCollideWith(const entity& o) override;
};

class circle : public entity
{
public:
	vec2 center;
	float radius;

	circle(vec2 center = vec2(), vec2 velocity = vec2(), float radius = 1)
		:entity(Circle, velocity), center(center), radius(radius) {}

	std::shared_ptr<entity> translate(vec2 delta) override;

	float willCollideWith(const entity& o) override;
};

class polygon : public entity
{
public:
	std::vector<vec2> points;

	polygon(std::vector<vec2> points, vec2 velocity = vec2()) 
		: entity(Polygon, velocity), points(std::move(points)) {}

	std::shared_ptr<entity> translate(vec2 delta) override;

	float willCollideWith(const entity& o) override;

};

class obb : public polygon
{
	static std::vector<vec2> toVertices(vec2 position, 
		float length, float radius, float angle);
public:
	obb(vec2 position, float length, float radius, float angle, vec2 velocity = vec2())
		: polygon(toVertices(position, length, radius, angle), velocity) {}
};
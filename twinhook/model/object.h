#pragma once

#include "util/vec2.h"

enum entity_type
{
	AABB,
	Circle,
	Polygon
};

class entity
{
protected:
	entity(entity_type type) : type(type) {}
	entity(entity_type type, vec2 velocity)
		: type(type), velocity(velocity) {}

public:
	const entity_type type;
	vec2 velocity;

	virtual ~entity() = default;

	/**
	 * \brief Predict time until collision between this entity and another
	 * \param o The other entity
	 * \return Result of the vector collision predictor (frames until collision)
	 */
	virtual float willCollideWith(const entity &o) = 0;
};

class aabb : public entity
{
public:
	vec2 position;
	vec2 size;

	aabb(vec2 position, vec2 velocity, vec2 size)
		: entity(AABB, velocity), position(position), size(size) {}

	float willCollideWith(const entity& o) override;
};

class circle : public entity
{
public:
	vec2 center;
	float radius;

	circle(vec2 center, vec2 velocity, float radius)
		:entity(Circle, velocity), center(center), radius(radius) {}

	float willCollideWith(const entity& o) override;
};

class polygon : public entity
{
public:
	std::vector<vec2> points;

	polygon(std::vector<vec2> points, vec2 velocity) 
		: entity(Polygon, velocity), points(std::move(points)) {}

	float willCollideWith(const entity& o) override;

};
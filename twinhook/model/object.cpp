#include "stdafx.h"
#include "object.h"

std::shared_ptr<entity> entity::entityAtCollision(const entity &o)
{
	return this->translate(this->willCollideWith(o) * this->velocity);
}

std::shared_ptr<entity> aabb::translate(vec2 delta)
{
	auto c = std::make_shared<aabb>(*this);
	c->position += delta;
	return c;
}

float aabb::willCollideWith(const entity& o)
{
	switch (o.type)
	{
	case AABB: {
		const auto& a = dynamic_cast<const aabb&>(o);
		return vec2::willCollideAABB(position, a.position,
			size, a.size, velocity, a.velocity);
	}
	default: return -1.f;
	}
}

std::shared_ptr<entity> circle::translate(vec2 delta)
{
	auto c = std::make_shared<circle>(*this);
	c->center += delta;
	return c;
}

float circle::willCollideWith(const entity& o)
{
	switch (o.type)
	{
	case Circle: {
		const auto& a = dynamic_cast<const circle&>(o);
		return vec2::willCollideCircle(center, a.center,
			radius, a.radius, velocity, a.velocity);
	}
	default: return -1.f;
	}
}

std::shared_ptr<entity> polygon::translate(vec2 delta)
{
	auto c = std::make_shared<polygon>(*this);
	for (vec2& v : c->points)
		v += delta;
	return c;
}

float polygon::willCollideWith(const entity& o)
{
	switch (o.type)
	{
	case Polygon: {
		const auto& a = dynamic_cast<const polygon&>(o);
		return vec2::willCollideSAT(points, velocity, a.points, a.velocity);
	}
	default: return -1.f;
	}
}

std::vector<vec2> obb::toVertices(vec2 position, float length, float radius, float angle)
{
	std::vector<vec2> vertices;
	// note: these vertices must be in VERTEX ORDER
	vec2 points[] = {
		vec2(0, radius), vec2(0, -radius),
		vec2(length, -radius), vec2(length, radius)
	};
	vertices.reserve(4);
	for (const auto& point : points)
		vertices.push_back(point.rotate(angle) + position);
	return vertices;
}

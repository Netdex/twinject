#pragma once

#include "util/vec2.h"
#include <memory>

class polygon;
class aabb;

class __declspec(dllexport) entity
{
public:
	enum entity_type
	{
		AABB,
		Circle,
		Polygon
	};
protected:
	entity(entity_type type, vec2 velocity = vec2())
		: type(type), velocity(velocity) {}
public:
	entity_type type;
	vec2 velocity;

	virtual ~entity() = default;

	/**
	 * \brief Center of mass function
	 * \return Center of mass of entity
	 */
	virtual vec2 com() const = 0;

	virtual std::shared_ptr<aabb> boundingBox() const = 0;

	/**
	 * \brief Get translated entity
	 * \param delta Translation delta
	 * \return Entity translated by delta
	 */
	virtual std::shared_ptr<entity> translate(vec2 delta) const = 0;
	/**
	 * \brief Get entity with new velocity
	 * \param newVelocity The new velocity
	 * \return Entity with new velocity
	 */
	virtual std::shared_ptr<entity> withVelocity(vec2 newVelocity) const = 0;

	/**
	 * \brief Predict time until collision between this entity and another
	 * \param o The other entity
	 * \return Result of the vector collision predictor (frames until collision)
	 */
	virtual float willCollideWith(const entity &o) const = 0;
	/**
	 * \brief Predict time until entity leaves another's bounds
	 * \param o The other entity
	 * \return Result of the vector exit predictor (frames until exit)
	 */
	virtual float willExit(const entity& o) const = 0;

	std::shared_ptr<entity> entityAtCollision(const entity &o) const;


};

class __declspec(dllexport) aabb : public entity
{
public:
	vec2 position;
	vec2 size;

	aabb(vec2 position = vec2(), vec2 velocity = vec2(), vec2 size = vec2(1, 1))
		: entity(AABB, velocity), position(position), size(size) {}

	vec2 com() const override;
	std::shared_ptr<entity> translate(vec2 delta) const override;
	std::shared_ptr<entity> withVelocity(vec2 newVelocity) const override;
	std::shared_ptr<aabb> boundingBox() const override;

	float willExit(const entity& o) const override;
	float willCollideWith(const entity& o) const override;

	polygon toPolygon() const;
};

class circle : public entity
{
public:
	vec2 center;
	float radius;

	circle(vec2 center = vec2(), vec2 velocity = vec2(), float radius = 1)
		:entity(Circle, velocity), center(center), radius(radius) {}

	vec2 com() const override;
	std::shared_ptr<entity> translate(vec2 delta) const override;
	std::shared_ptr<entity> withVelocity(vec2 newVelocity) const override;
	std::shared_ptr<aabb> boundingBox() const override;

	float willExit(const entity& o) const override;
	float willCollideWith(const entity& o) const override;
};

class __declspec(dllexport) polygon : public entity
{
public:
	std::vector<vec2> points;

	polygon(std::vector<vec2> points, vec2 velocity = vec2())
		: entity(Polygon, velocity), points(std::move(points)) {}

	vec2 com() const override;
	std::shared_ptr<entity> translate(vec2 delta) const override;
	std::shared_ptr<entity> withVelocity(vec2 newVelocity) const override;
	std::shared_ptr<aabb> boundingBox() const override;

	float willExit(const entity& o) const override;
	float willCollideWith(const entity& o) const override;

};

class __declspec(dllexport) obb : public polygon
{
	static std::vector<vec2> toVertices(vec2 position,
		float length, float radius, float angle);
public:
	obb(vec2 position, float length, float radius, float angle, vec2 velocity = vec2())
		: polygon(toVertices(position, length, radius, angle), velocity) {}
};
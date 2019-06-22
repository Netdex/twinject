#pragma once

#include <memory>
#include <string>
#include <utility>

#include "util/vec2.h"

class polygon;
class aabb;

class entity
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
		: type(type), velocity(std::move(velocity)) {}

	entity& operator=(const entity& o) = default;

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
	virtual float willCollideWith(const entity& o) const = 0;
	/**
	 * \brief Predict time until entity leaves another's bounds
	 * \param o The other entity
	 * \return Result of the vector exit predictor (frames until exit)
	 */
	virtual float willExit(const entity& o) const = 0;

	std::shared_ptr<entity> entityAtCollision(const entity& o) const;

	virtual std::string serialize() const = 0;

	friend void swap(entity& lhs, entity& rhs) noexcept;
};

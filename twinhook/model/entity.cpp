#include "entity.h"

std::shared_ptr<entity> entity::entityAtCollision(const entity& o) const
{
	float t = this->willCollideWith(o);
	if (t < 0)
		return nullptr;
	return this->translate(t * this->velocity);
}

std::ostream& operator<<(std::ostream& os, const entity& o)
{
	return o.serialize(os);
}

void swap(entity& lhs, entity& rhs) noexcept
{
	using std::swap;
	swap(lhs.type, rhs.type);
	swap(lhs.velocity, rhs.velocity);
}

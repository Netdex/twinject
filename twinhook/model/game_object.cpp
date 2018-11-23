#include "game_object.h"
#include "util/cdraw.h"
#include "config/th_config.h"

static void cdraw_polygon(const std::vector<vec2> &points)
{
	for (size_t i = 0; i < points.size(); i++)
	{
		vec2 p1 = points[i];
		vec2 p2 = points[(i + points.size() + 1) % points.size()];
		cdraw::line(
			th_param.GAME_X_OFFSET + p1.x,
			th_param.GAME_Y_OFFSET + p1.y,
			th_param.GAME_X_OFFSET + p2.x,
			th_param.GAME_Y_OFFSET + p2.y,
			D3DCOLOR_ARGB(255, 255, 0, 0)
		);
	}
}

static void cdraw_aabb(const std::shared_ptr<aabb> &c)
{
	cdraw::rect(th_param.GAME_X_OFFSET + c->position.x,
		th_param.GAME_Y_OFFSET + c->position.y, c->size.w, c->size.h,
		D3DCOLOR_ARGB(255, 0, 0, 255));
	vec2 com = c->com();
	vec2 proj = com + c->velocity * 10;
	cdraw::line(com.x + th_param.GAME_X_OFFSET, com.y + th_param.GAME_Y_OFFSET,
		proj.x + th_param.GAME_X_OFFSET, proj.y + th_param.GAME_Y_OFFSET, D3DCOLOR_ARGB(255, 0, 255, 0));
}

void game_object::render() const
{
	// I'm not a fan of using a switch for this, but I want to leave 
	// the object impl as clean as possible

	switch (obj->type)
	{
	case entity::AABB: {
		auto c = std::dynamic_pointer_cast<aabb>(obj);
		cdraw_aabb(c);
		break;
	}
	case entity::Circle: {
		auto a = std::dynamic_pointer_cast<circle>(obj);
		cdraw_aabb(a->boundingBox());
		break;
	}
	case entity::Polygon: {
		auto c = std::dynamic_pointer_cast<polygon>(obj);
		cdraw_polygon(c->points);
		break;
	}
	}
}

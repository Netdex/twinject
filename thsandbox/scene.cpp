#include "scene.h"



scene::scene()
{
}


scene::~scene()
{

}

// https://stackoverflow.com/questions/38334081/howto-draw-circles-arcs-and-vector-graphics-in-sdl
static void draw_circle(SDL_Renderer *renderer, int _x, int _y, int radius)
{
	int x = radius - 1;
	int y = 0;
	int tx = 1;
	int ty = 1;
	int err = tx - (radius << 1); // shifting bits left by 1 effectively
								  // doubles the value. == tx - diameter
	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, _x + x, _y - y);
		SDL_RenderDrawPoint(renderer, _x + x, _y + y);
		SDL_RenderDrawPoint(renderer, _x - x, _y - y);
		SDL_RenderDrawPoint(renderer, _x - x, _y + y);
		SDL_RenderDrawPoint(renderer, _x + y, _y - x);
		SDL_RenderDrawPoint(renderer, _x + y, _y + x);
		SDL_RenderDrawPoint(renderer, _x - y, _y - x);
		SDL_RenderDrawPoint(renderer, _x - y, _y + x);

		if (err <= 0)
		{
			y++;
			err += ty;
			ty += 2;
		}
		if (err > 0)
		{
			x--;
			tx += 2;
			err += tx - (radius << 1);
		}
	}
}

static void draw_entity(SDL_Renderer *renderer,
	const std::shared_ptr<entity> &e)
{
	switch (e->type)
	{
	case entity::Circle: {
		auto a = std::dynamic_pointer_cast<circle>(e);
		draw_circle(renderer, a->center.x, a->center.y, a->radius);
		break;
	}
	case entity::AABB: {
		auto a = std::dynamic_pointer_cast<aabb>(e);
		SDL_Rect outlineRect = { (int)a->position.x, (int)a->position.y,
			(int)a->size.w,(int)a->size.h };
		SDL_RenderDrawRect(renderer, &outlineRect);
		break;
	}
	case entity::Polygon: {
		auto a = std::dynamic_pointer_cast<polygon>(e);
		for (size_t i = 0; i < a->points.size(); i++)
		{
			vec2 p1 = a->points[i];
			vec2 p2 = a->points[(i + a->points.size() + 1) % a->points.size()];
			SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		}
		break;
	}
	}
}
void scene::render(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for (auto e : entities)
		draw_entity(renderer, e);

	for (size_t i = 0; i < entities.size(); ++i)
	{
		for (size_t j = i + 1; j < entities.size(); ++j)
		{
			auto e1 = entities[i];
			auto e2 = entities[j];
			float t = e1->willCollideWith(*e2);
			if (t >= 0)
			{
				auto et1 = e1->translate(e1->velocity * t);
				auto et2 = e2->translate(e2->velocity * t);
				if (t == 0)
					SDL_SetRenderDrawColor(renderer, 0xFF, 0x40, 0x40, 0xFF);
				else
					SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xFF);
				draw_entity(renderer, et1);
				draw_entity(renderer, et2);
				SDL_SetRenderDrawColor(renderer, 0x80, 0xFF, 0x80, 0xFF);
				auto c1 = e1->com();
				auto c2 = e2->com();
				SDL_RenderDrawLine(renderer, (int)c1.x, (int)c1.y,
					(int)c2.x, (int)c2.y);
				SDL_SetRenderDrawColor(renderer, 0x30, 0x30, 0x30, 0xFF);
				auto ct1 = et1->com();
				auto ct2 = et2->com();
				SDL_RenderDrawLine(renderer, (int)c1.x, (int)c1.y,
					(int)ct1.x, (int)ct1.y);
				SDL_RenderDrawLine(renderer, (int)c2.x, (int)c2.y,
					(int)ct2.x, (int)ct2.y);
			}
		}
	}
}

void scene::tick()
{
	std::vector<std::shared_ptr<entity>> newEntities;
	for (const auto& e : entities)
		newEntities.push_back(e->translate(e->velocity));
	entities = newEntities;
}

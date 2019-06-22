#pragma once

#include <utility>

#include "model/object.h"

class game_object
{
protected:
	enum game_object_type
	{
		Bullet,
		Laser,
		Player,
		Powerup,
		Enemy
	};

	game_object(game_object_type type, std::shared_ptr<entity> obj) : type(type), obj(std::move(obj)) {}
public:
	game_object_type type;
	std::shared_ptr<entity> obj;

	virtual ~game_object() = default;

	// HACK render via cdraw
	void render() const;
};

class bullet : public game_object
{
public:
	long long meta;

	bullet(const aabb &a, long long meta = 0) 
		: game_object(Bullet, std::make_shared<aabb>(a)), meta(meta) {}

	bullet(const circle &a, long long meta = 0)
		: game_object(Bullet, std::make_shared<circle>(a)), meta(meta) {}
};

class laser : public game_object
{
public:
	laser(const obb &a)
		: game_object(Laser, std::make_shared<obb>(a)) {}
};

class player : public game_object
{
public:

	player(const aabb &a, long long meta = 0)
		: game_object(Player, std::make_shared<aabb>(a)) {}

	player(const circle &a, long long meta = 0)
		: game_object(Player, std::make_shared<circle>(a)) {}
};

class enemy : public game_object
{
public:

	enemy(const aabb &a, long long meta = 0)
		: game_object(Enemy, std::make_shared<aabb>(a)) {}

	enemy(const circle &a, long long meta = 0)
		: game_object(Enemy, std::make_shared<circle>(a)) {}
};

class powerup : public game_object
{
public:
	long long meta;

	powerup(const aabb &a, long long meta = 0)
		: game_object(Powerup, std::make_shared<aabb>(a)), meta(meta) {}

	powerup(const circle &a, long long meta = 0)
		: game_object(Powerup, std::make_shared<circle>(a)), meta(meta) {}
};
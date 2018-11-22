#pragma once

enum game_object_type
{
	Bullet,
	Laser
};

class game_object
{
protected:
	game_object() {}
public:
	virtual ~game_object() = default;
};

class bullet
{
	
};
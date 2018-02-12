#pragma once
#include <unordered_map>
#include "data.h"


/**
 * \brief Object representing a human player.
 */
class th_player
{
	std::unordered_map<std::string, data> state;

public:
	th_player() {}
	virtual ~th_player() = default;

	/**
	 * \brief Called when player has been initialized.
	 */
	virtual void on_init() {}

	/**
	 * \brief Activate all hooks
	 */
	virtual void hook() {}


	virtual void on_before_tick() {}
	virtual void on_tick() {}
	virtual void handle_input(BYTE keys[256]) {}
	virtual void draw(IDirect3DDevice9 *d3dDev) {}
};

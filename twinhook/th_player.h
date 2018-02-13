#pragma once
#include <unordered_map>


/**
 * \brief Object representing a human player.
 */
class th_player
{

public:
	th_player() {}
	virtual ~th_player() = default;

	/**
	 * \brief Called when player has been initialized.
	 */
	virtual void on_init() {}
	virtual void on_before_tick() {}
	virtual void on_tick() {}
	virtual void draw(IDirect3DDevice9 *d3dDev) {}
};

#pragma once
#include <unordered_map>


/**
 * \brief Object representing a human player.
 */
class th_player
{
	
public:
	bool enabled = false;

	th_player() {}
	virtual ~th_player() = default;

	/**
	 * \brief Called when player has been initialized.
	 */
	virtual void on_init() {}
	virtual void on_begin_tick() {}
	virtual void on_tick() {}
	virtual void on_after_tick() {}
	virtual void draw(IDirect3DDevice9 *d3dDev) {}

	virtual void handle_input(BYTE diKeys[256]) {}
	virtual void on_enable_changed(bool enable) {}

	void set_enable(bool enable)
	{
		enabled = enable;
		on_enable_changed(enable);
	}
};

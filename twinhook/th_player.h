#pragma once

#include "vec2.h"
#include "th_algorithm.h"

struct entity
{
	vec2 p;
	vec2 v;
	vec2 sz;			// bullet size
	DWORD me;			// metadata
};


/**
 * \brief Object representing a human player.
 */
class th_player
{
protected:
	th_algorithm *algorithm = nullptr;
public:
	std::vector<entity> bullets;
	std::vector<entity> powerups;

	bool enabled = false;
	bool render = false;

	th_player() {}
	virtual ~th_player() = default;

	/**
	 * \brief Called when player has been initialized.
	 */
	virtual void on_init() {}

	/**
	 * \brief Called right before a tick has begun.
	 */
	virtual void on_begin_tick() {}

	/**
	 * \brief Called during a tick.
	 */
	virtual void on_tick() {}

	/**
	 * \brief Called right after a tick is completed.
	 */
	virtual void on_after_tick() {}

	/**
	 * \brief Draw debugging information and visualizations to the game device.
	 * \param d3dDev D3D9 wrapper device
	 */
	virtual void draw(IDirect3DDevice9 *d3dDev) {}

	/**
	 * \brief Process raw input from game
	 * \param diKeys Key state
	 */
	virtual void handle_input(BYTE diKeys[256]) {}

	/**
	 * \brief Called when the bot's enable state is changed
	 * \param enable Bot enable state
	 */
	virtual void on_enable_changed(bool enable) {}

	/**
	 * \brief Change the bot's enable state
	 * \param enable The new enable state
	 */
	void set_enable(bool enable)
	{
		// debounce
		if(enable != enabled)
			on_enable_changed(enable);
		enabled = enable;
	}

	void bind_algorithm(th_algorithm *algo);

	virtual vec2 get_plyr_loc() = 0;
};

inline void th_player::bind_algorithm(th_algorithm* algo)
{
	algorithm = algo;
}

#pragma once

#include "vec2.h"
#include "th_algorithm.h"
#include "th_config.h"
#include "keypress_detect.h"

/*
 * Representing an entity. This struct has two interpretations depending on
 * whether the game using hitcircles or hitboxes because I was lazy
 * (yes, this is confusing, I'll fix this)
 *
 * If hitcircles:
 * p: center of circle
 * v: velocity of circle
 * sz: radius of circle (both components are the same)
 * me: game specific
 *
 * If hitboxes:
 * p: top left corner of box
 * v: velocity of box
 * sz: dimensions (width, height) of box
 * me: game specific
 */
struct entity
{
	vec2 p;				// position
	vec2 v;				// velocity
	vec2 sz;			// bullet size
	DWORD me;			// metadata
};

struct laser
{
	vec2 p;				// position
	vec2 v;				// velocity
	vec2 ex;			// laser extent
	float length;
	float rad;			// perpendicular radius
	float ang;			// angle
};

// game-specific addresses for common behaviour
struct gs_addr
{
	uint8_t *plyr_pos;
	uint8_t *kbd_state;
};

union th_kbd_state
{
	struct {
		bool shot;		// 0
		bool bomb;		// 1
		bool slow;		// 2
		bool skip;		// 3
		bool up;		// 4
		bool left;		// 5
		bool down;		// 6
		bool right;		// 7
	};
	bool keys[8];
};

/**
 * \brief Object representing a human player.
 */
class th_player
{
protected:
	th_algorithm * algorithm = nullptr;
	keypress_detect kpd;

	// game specific pointers
	gs_addr gs_ptr;
public:
	std::vector<entity> bullets;
	std::vector<entity> powerups;
	std::vector<laser> lasers;

	bool enabled = false;
	bool render = false;

	th_player(gs_addr gsa) : gs_ptr(gsa) {}
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
	virtual void handle_input(const BYTE diKeys[256], const BYTE press[256]) {}

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
		if (enable != enabled)
			on_enable_changed(enable);
		enabled = enable;
	}

	void bind_algorithm(th_algorithm *algo);

	/**
	 * \brief Get player characteristics
	 * \return An entity struct populated with player characteristics
	 */
	virtual entity get_plyr_ent()
	{
		PBYTE PlayerPtrAddr = (PBYTE)this->gs_ptr.plyr_pos;
		entity plyr = {
			vec2(*(float*)PlayerPtrAddr - th_param.GAME_X_OFFSET,
				*(float*)(PlayerPtrAddr + 4) - th_param.GAME_Y_OFFSET),
			vec2(),
			vec2(6,6),		// hard-coded player size
			0
		};
		return plyr;
	}

	/*
	 * Memory addresses and values borrowed from
	 * https://www.shrinemaiden.org/forum/index.php?topic=16024.0
	 * Thank you!
	 */
	th_kbd_state get_kbd_state() const
	{
		uint8_t *ptr_gkbd_st = gs_ptr.kbd_state;
		th_kbd_state s = {
			(bool)(ptr_gkbd_st[0] & 1),
			(bool)(ptr_gkbd_st[0] & 2),
			(bool)(ptr_gkbd_st[0] & 8),
			(bool)(ptr_gkbd_st[1] & 2),
			(bool)(ptr_gkbd_st[0] & 16),
			(bool)(ptr_gkbd_st[0] & 64),
			(bool)(ptr_gkbd_st[0] & 32),
			(bool)(ptr_gkbd_st[0] & 128),
		};
		return s;
	}
};

inline void th_player::bind_algorithm(th_algorithm* algo)
{
	algorithm = algo;
}

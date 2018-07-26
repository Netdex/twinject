#pragma once

#include "../util/vec2.h"
#include "../algo/th_algorithm.h"
#include "../info/keypress_detect.h"
#include "config/th_command_proc.h"
#include "gfx/imgui_controller.h"

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

	std::vector<vec2> laser::getVertices() const
	{
		std::vector<vec2> vertices;
		// note: these vertices must be in VERTEX ORDER
		vec2 points[] = {
			vec2(0, rad), vec2(0, -rad),
			vec2(length, -rad), vec2(length, rad)
		};
		for (int i = 0; i < 4; ++i)
		{
			vertices.push_back(points[i].rotate(ang) + p);
		}
		return vertices;
	}
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
	th_algorithm *algorithm = nullptr;
	keypress_detect kpd;
	th_command_proc cmdp;
	imgui_controller *imguictl = nullptr;

	// game specific pointers
	gs_addr gs_ptr;
public:
	std::vector<entity> bullets;
	std::vector<entity> powerups;
	std::vector<laser> lasers;

	bool enabled = false;
	bool render = false;

	th_player(gs_addr gsa) : gs_ptr(gsa), cmdp(this) {}
	virtual ~th_player()
	{
		if (imguictl)	delete imguictl;
	};

	/**
	 * \brief Called when player has been initialized.
	 */
	virtual void onInit();

	/**
	 * \brief Called right before a tick has begun.
	 */
	virtual void onBeginTick();

	/**
	 * \brief Called during a tick.
	 */
	virtual void onTick();

	/**
	 * \brief Called right after a tick is completed.
	 */
	virtual void onAfterTick();

	/**
	 * \brief Draw debugging information and visualizations to the game device.
	 * \param d3dDev D3D9 wrapper device
	 */
	virtual void draw(IDirect3DDevice9* d3dDev);

	/**
	 * \brief Process raw input from game
	 * \param diKeys Key state
	 */
	virtual void handleInput(const BYTE diKeys[256], const BYTE press[256]);

	/**
	 * \brief Called when the bot's enable state is changed
	 * \param enable Bot enable state
	 */
	virtual void onEnableChanged(bool enable);

	/**
	 * \brief Change the bot's enable state
	 * \param enable The new enable state
	 */
	void setEnable(bool enable);

	void bindAlgorithm(th_algorithm *algo);

	/**
	 * \brief Get player characteristics
	 * \return An entity struct populated with player characteristics
	 */
	virtual entity getPlayerEntity();

	/*
	 * Memory addresses and values borrowed from
	 * https://www.shrinemaiden.org/forum/index.php?topic=16024.0
	 * Thank you!
	 */
	th_kbd_state getKeyboardState() const;
};

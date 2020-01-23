#pragma once

#include "util/vec2.h"

namespace control
{
	enum Movement : int
	{
		Hold = 0,
		Up,
		Down,
		Left,
		Right,
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight,
		FocusUp,
		FocusDown,
		FocusLeft,
		FocusRight,
		FocusTopLeft,
		FocusTopRight,
		FocusBottomLeft,
		FocusBottomRight,

		MaxValue
	};

	/*
	* Unit velocity as a result of moving in specified direction
	* [0]: Hold,			[1]: Up,				[2]: Down,				[3]: Left,
	* [4]: Right,			[5]: Top-left,			[6]: Top-right,			[7]: Bottom-left,
	* [8]: Bottom-right,	[9]: F Up,				[10]: F Down,			[11]: F Left,
	* [12]: F Right,		[13]: F Top-left,		[14]: F top-right,		[15]: F bottom-left,
	* [16]: F bottom-right
	*/
	constexpr vec2 kMovementVelocity[] = {
		vec2(0,0), vec2(0,-1), vec2(0,1), vec2(-1,0), vec2(1,0),
		vec2(-M_SQRT2, -M_SQRT2), vec2(M_SQRT2, -M_SQRT2),
		vec2(-M_SQRT2, M_SQRT2), vec2(M_SQRT2, M_SQRT2),
		vec2(0,-1), vec2(0,1), vec2(-1,0), vec2(1,0),
		vec2(-M_SQRT2, -M_SQRT2), vec2(M_SQRT2, -M_SQRT2),
		vec2(-M_SQRT2, M_SQRT2), vec2(M_SQRT2, M_SQRT2)
	};

	// Whether the direction index is at focused velocity
	constexpr bool kMovementFocused[] = {
		false,false,false,false,false,false,false,false,false,
		true,true,true,true,true,true,true,true
	};

	// Keys to press in order to move in a certain direction
	constexpr uint8_t kMovementToInput[][3] = {
			{ DIK_LSHIFT,	NULL,			NULL },	// focus by default
			{ DIK_UP,		NULL,			NULL },
			{ DIK_DOWN,		NULL,			NULL },
			{ DIK_LEFT,		NULL,			NULL },
			{ DIK_RIGHT,	NULL,			NULL },
			{ DIK_UP,		DIK_LEFT,		NULL },
			{ DIK_UP,		DIK_RIGHT,		NULL },
			{ DIK_DOWN,		DIK_LEFT,		NULL },
			{ DIK_DOWN,		DIK_RIGHT,		NULL },
			{ DIK_UP,		NULL,			DIK_LSHIFT },
			{ DIK_DOWN,		NULL,			DIK_LSHIFT },
			{ DIK_LEFT,		NULL,			DIK_LSHIFT },
			{ DIK_RIGHT,	NULL,			DIK_LSHIFT },
			{ DIK_UP,		DIK_LEFT,		DIK_LSHIFT },
			{ DIK_UP,		DIK_RIGHT,		DIK_LSHIFT },
			{ DIK_DOWN,		DIK_LEFT,		DIK_LSHIFT },
			{ DIK_DOWN,		DIK_RIGHT,		DIK_LSHIFT }
	};

	// Key presses which should be reset each frame
	constexpr uint8_t kControlKeys[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_LSHIFT, DIK_X };

}


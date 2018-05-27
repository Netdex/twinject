#include "stdafx.h"
#include "di8_input_overlay.h"

#include "util/cdraw.h"
#include "hook/th_di8_hook.h"
#include "config/th_config.h"

void DI8_Overlay_RenderInput(IDirect3DDevice9 *d3dDev, th_kbd_state state)
{
	for (int i = 0; i < sizeof kg_MonitoredKeys / sizeof key_graphic; i++)
	{
		struct key_graphic kg = kg_MonitoredKeys[i];
		float x = th_param.WINDOW_WIDTH - (640 - 520) + kg.x * InputOverlayUnitSize;
		float y = th_param.WINDOW_HEIGHT - (480 - 430) + kg.y * InputOverlayUnitSize;
		float w = kg.w * InputOverlayUnitSize;
		float h = kg.h * InputOverlayUnitSize;
		D3DCOLOR c = state.keys[kg.vkCode] ? kg.active_color : kg.default_color;
		cdraw::fill_rect(x, y, w, h, c);
	}
}

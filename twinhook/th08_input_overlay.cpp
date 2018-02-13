#include "stdafx.h"
#include "th08_input_overlay.h"

#include "cdraw.h"
#include "th_di8_hook.h"

void Overlay_RenderInput(IDirect3DDevice9 *d3dDev)
{
	for (int i = 0; i < sizeof kg_MonitoredKeys / sizeof key_graphic; i++)
	{
		struct key_graphic kg = kg_MonitoredKeys[i];
		float x = InputOverlayPositionX + kg.x * InputOverlayUnitSize;
		float y = InputOverlayPositionY + kg.y * InputOverlayUnitSize;
		float w = kg.w * InputOverlayUnitSize;
		float h = kg.h * InputOverlayUnitSize;
		D3DCOLOR c = th_di8_hook::inst()->get_vk_state(kg.vkCode) ? kg.active_color : kg.default_color;
		CDraw_FillRect(x, y, w, h, c);
	}
}

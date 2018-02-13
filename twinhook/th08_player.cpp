#include "stdafx.h"
#include "th08_player.h"
#include "IDI8ADevice_Wrapper.h"

th08_player::th08_player()
{

}

void th08_player::on_init()
{
	LOG("th08 player initialized");
}

void th08_player::on_tick()
{
	BYTE diKeys[256];
	// TODO move the DI8 code into the tick code, it's not needed here.
	// TODO remove the handle_input code, it's not needed here.
	/*if (di8_hook.get_di8w()) {
		if (di8_hook.get_di8w()->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			this->handle_input(diKeys);
		}

	}*/
}

void th08_player::on_before_tick()
{
}

void th08_player::draw(IDirect3DDevice9* d3dDev)
{
}

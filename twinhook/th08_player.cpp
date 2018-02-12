#include "stdafx.h"
#include "th08_player.h"
#include "IDI8ADevice_Wrapper.h"

th08_player::th08_player() :
	d3d9_hook(th_d3d9_hook(this)),
	di8_hook(th_di8_hook(this)),
	bp_hook(th08_bullet_process_hook(this))
{

}

void th08_player::on_init()
{
	LOG("intialization based hook");
}

void th08_player::hook()
{
	d3d9_hook.hook();
	di8_hook.hook();
	bp_hook.hook();
}

void th08_player::on_tick()
{
	BYTE diKeys[256];
	// TODO move the DI8 code into the tick code, it's not needed here.
	// TODO remove the handle_input code, it's not needed here.
	if (di8_hook.get_di8w()) {
		if (di8_hook.get_di8w()->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			this->handle_input(diKeys);
		}

	}
}

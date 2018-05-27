#include "../stdafx.h"
#include "th10_bullet_proc_hook.h"

th10_bullet_proc_hook* th10_bullet_proc_hook::instance = nullptr;

void th10_bullet_proc_hook::bind(th10_player* player)
{
	ASSERT(("cannot multi-bind", !instance));
	instance = new th10_bullet_proc_hook(player);
}

th10_bullet_proc_hook* th10_bullet_proc_hook::inst()
{
	// TODO move statements into base class
	ASSERT(("cannot obtain unbounded instance", instance));
	return instance;
}

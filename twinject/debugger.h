#pragma once

#include <windows.h>

DWORD OnLoadDllDebugEvent(const LPDEBUG_EVENT DebugEv);
DWORD OnOutputDebugStringEvent(const LPDEBUG_EVENT debug_event);
void EnterDebugLoop(const LPDEBUG_EVENT DebugEv);
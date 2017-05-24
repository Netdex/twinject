#include "stdafx.h"
#include "detour.h"

bool DetourFunction(PVOID *ppPointer, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (DetourAttach(ppPointer, pDetour))
		return false;
	if (DetourTransactionCommit() != NO_ERROR)
		return false;
	return true;
}
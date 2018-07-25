#include "stdafx.h"
#include "th_wndproc_imgui_hook.h"

static LONG_PTR originalWndProc = NULL;

static LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return TRUE;
	return CallWindowProc((WNDPROC)originalWndProc, hWnd, msg, wParam, lParam);
}

void th_wndproc_imgui_hook::bind(HWND hWnd)
{
	originalWndProc = SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)WndProcHook);
}

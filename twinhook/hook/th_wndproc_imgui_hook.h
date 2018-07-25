#pragma once

/*
 * IMGUI requires a handler in WndProc in order to process input. 
 * We give it that by replacing the game's WndProc with a MITM-style 
 * hook.
 */

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace th_wndproc_imgui_hook
{
	void bind(HWND hWnd);
}
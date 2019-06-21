#include "stdafx.h"
#include "imgui_controller.h"
#include <imgui.h>
#include <examples/imgui_impl_dx9.h>
#include <examples/imgui_impl_win32.h>
#include "hook/th_wndproc_imgui_hook.h"
#include <winuser.h>

using namespace ImGui;

void imgui_controller::init()
{
	ASSERT(pD3DDev->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) >= 0);

	th_wndproc_imgui_hook::bind(hWnd);

	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO(); (void)io;
	io.FontAllowUserScaling = true;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pD3DDev);
	StyleColorsClassic();
}

void imgui_controller::preframe()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();
}

void imgui_controller::render()
{
	Render();

	// We must apply state of working case, then return state back to normal
	ASSERT(pD3DDev->CreateStateBlock(D3DSBT_ALL, &d3d9_sb_preserve) >= 0);
	d3d9_state_block->Apply();
	ImGui_ImplDX9_RenderDrawData(GetDrawData());
	d3d9_sb_preserve->Apply();
	d3d9_sb_preserve->Release();
}

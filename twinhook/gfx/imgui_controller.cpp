#include "stdafx.h"
#include "imgui_controller.h"
#include <imgui/imgui.h>
#include <imgui/examples/imgui_impl_dx9.h>
#include <imgui/examples/imgui_impl_win32.h>
#include "hook/th_wndproc_imgui_hook.h"

void imgui_controller::init()
{
	ASSERT(pD3DDev->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) >= 0);

	th_wndproc_imgui_hook::bind(hWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pD3DDev);
	ImGui::StyleColorsDark();
}

void imgui_controller::preframe()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void imgui_controller::render()
{
	ImGui::EndFrame();
	ImGui::Render();

	// We must apply state of working case, then return state back to normal
	ASSERT(pD3DDev->CreateStateBlock(D3DSBT_ALL, &d3d9_sb_preserve) >= 0);
	d3d9_state_block->Apply();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	d3d9_sb_preserve->Apply();
	d3d9_sb_preserve->Release();
}

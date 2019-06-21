#include "stdafx.h"
#include "imgui_mixins.h"
#include <imgui.h>

using namespace ImGui;

void ShowHelpMarker(const char* desc)
{
	TextDisabled("(?)");
	if (IsItemHovered())
	{
		BeginTooltip();
		PushTextWrapPos(GetFontSize() * 35.0f);
		TextUnformatted(desc);
		PopTextWrapPos();
		EndTooltip();
	}
}
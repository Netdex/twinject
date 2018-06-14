#include "stdafx.h"
#include "th_command_proc.h"
#include "util/cdraw.h"
#include "th_config.h"

th_command_proc::th_command_proc()
{
}

th_command_proc::~th_command_proc()
{
}

void th_command_proc::handleInput(BYTE state[256], BYTE change[256])
{
	for (int k = 0; k < 256; ++k)
	{
		if (change[k])
		{
			switch (k)
			{
			case DIK_SLASH:
				if (inpState == WAITING) {
					inpState = RECEIVING;
				}
				break;
			case DIK_BACKSPACE:
				if (inpState == RECEIVING) {
					if (promptBufferPos > 0)
						promptBufferPos--;
				}
				break;
			case DIK_ESCAPE:
				if (inpState == RECEIVING) {
					inpState = WAITING;
					promptBufferPos = 0;
				}
				break;
			case DIK_RETURN:
				if (inpState == RECEIVING) {
					inpState = WAITING;
					char command[PROMPT_BUFFER_SZ + 1] = { 0 };
					memcpy(command, promptBuffer, promptBufferPos);
					print(command);
					promptBufferPos = 0;
					// TODO executor
				}
				break;
			default:
				if (inpState == RECEIVING)
				{
					if (promptBufferPos < PROMPT_BUFFER_SZ)
					{
						int vk = MapVirtualKey(k, MAPVK_VSC_TO_VK);
						// this part is a bit inefficient
						BYTE kbds[256];
						GetKeyboardState(kbds);
						WORD c;
						if (ToAscii(vk, k, kbds, &c, 0))
						{
							promptBuffer[promptBufferPos++] = (char) c;
						}
					}
				}
				break;
			}
		}
	}
}

void th_command_proc::render(IDirect3DDevice9* pD3dDev)
{
	if (inpState == RECEIVING) {
		cdraw::fillRect(0, th_param.WINDOW_HEIGHT - 20,
			th_param.WINDOW_WIDTH, 20, D3DCOLOR_ARGB(200, 0, 0, 0));
		cdraw::text(promptRenderBuffer, promptBufferPos + PROMPT_PROLOGUE_SZ,
			D3DCOLOR_ARGB(255, 255, 255, 255),
			10, (int)(th_param.WINDOW_HEIGHT - 17));
	}
}

void th_command_proc::printf(const char* fmt, ...) const
{
	char szUserFmt[256];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(szUserFmt, 256, fmt, args);
	print(szUserFmt);
	va_end(args);
}

void th_command_proc::print(const char* str) const
{
	OutputDebugStringA(str);
}

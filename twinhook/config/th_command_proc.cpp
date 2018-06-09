#include "stdafx.h"
#include "th_command_proc.h"
#include "util/cdraw.h"
#include "th_config.h"

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
					if (bufferPos > 0)
						bufferPos--;
				}
				break;
			case DIK_ESCAPE:
				if (inpState == RECEIVING) {
					inpState = WAITING;
					bufferPos = 0;
				}
				break;
			case DIK_RETURN:
				if (inpState == RECEIVING) {
					inpState = WAITING;
					bufferPos = 0;
					// TODO executor
				}
				break;
			default:
				if (inpState == RECEIVING)
				{
					if (bufferPos < BUFFER_SZ)
					{
						int vk = MapVirtualKey(k, MAPVK_VSC_TO_VK);
						// this part is a bit inefficient
						BYTE kbds[256];
						GetKeyboardState(kbds);
						WORD c;
						if (ToAscii(vk, k, kbds, &c, 0))
						{
							buffer[bufferPos++] = (char) c;
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
		cdraw::text(renderBuffer, bufferPos + PROLOGUE_SZ, D3DCOLOR_ARGB(255, 255, 255, 255),
			10, (int)(th_param.WINDOW_HEIGHT - 17));
	}
}

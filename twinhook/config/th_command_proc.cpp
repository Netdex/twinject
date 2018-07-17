#include "stdafx.h"
#include "th_command_proc.h"
#include "util/cdraw.h"
#include "th_config.h"
#include "patch/th_patch_registry.h"

void th_command_proc::handleInput(BYTE state[256], BYTE change[256])
{
	for (int k = 0; k < 256; ++k)
	{
		if (change[k])
		{
			switch (k)
			{
			case DIK_SLASH:
				if (inputState == WAITING) {
					inputState = RECEIVING;
				}
				break;
			case DIK_BACKSPACE:
				if (inputState == RECEIVING) {
					if (promptBufferPos > 0)
						promptBufferPos--;
				}
				break;
			case DIK_ESCAPE:
				if (inputState == RECEIVING) {
					inputState = WAITING;
					promptBufferPos = 0;
				}
				break;
			case DIK_RETURN:
				if (inputState == RECEIVING) {
					inputState = WAITING;
					char command[PROMPT_BUFFER_SZ + 1] = { 0 };
					memcpy(command, promptBuffer, promptBufferPos);

					// string tokenization
					std::string str(command);
					std::stringstream ss(str);
					std::vector<std::string> tokens;
					std::string tok;
					while (ss >> tok)
						tokens.push_back(tok);
					process(tokens);

					promptBufferPos = 0;
				}
				break;
			default:
				if (inputState == RECEIVING)
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
							promptBuffer[promptBufferPos++] = (char)c;
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
	if (inputState == RECEIVING) {
		cdraw::fillRect(0, th_param.WINDOW_HEIGHT - 20,
			th_param.WINDOW_WIDTH, 20, D3DCOLOR_ARGB(200, 0, 0, 0));
		cdraw::text(promptRenderBuffer, promptBufferPos + PROMPT_PROLOGUE_SZ,
			D3DCOLOR_ARGB(255, 255, 255, 255),
			10, (int)(th_param.WINDOW_HEIGHT - 17));
	}
}

void th_command_proc::process(const std::vector<std::string> &args)
{
	if (args.size() < 1) return;
	if (commands.find(args[0]) == commands.end())
	{
		print("unknown command");
		return;
	}
	(this->*commands[args[0]])(args);
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

void th_command_proc::_cmd_patch(const std::vector<std::string>& args)
{
}

void th_command_proc::_cmd_algo(const std::vector<std::string>& args)
{
}

void th_command_proc::_cmd_reg(const std::vector<std::string>& args)
{
}

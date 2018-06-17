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
	if (inpState == RECEIVING) {
		cdraw::fillRect(0, th_param.WINDOW_HEIGHT - 20,
			th_param.WINDOW_WIDTH, 20, D3DCOLOR_ARGB(200, 0, 0, 0));
		cdraw::text(promptRenderBuffer, promptBufferPos + PROMPT_PROLOGUE_SZ,
			D3DCOLOR_ARGB(255, 255, 255, 255),
			10, (int)(th_param.WINDOW_HEIGHT - 17));
	}
}

void th_command_proc::process(const std::vector<std::string>& args) const
{
	// TODO I don't like using if statements for stuff like this, 
	// I'd rather use a map of classes and command executors, 
	// since they can do stuff like autogenerate help docs
	// But for now this is fine

	if (args.size() < 1) return;
	if (args[0] == "patch")
	{
		if (args.size() < 2) return;
		if(args[1] == "set")
		{
			
		}
		else if(args[1] == "reset")
		{
			
		}
		else
		{
			printf("usage: /patch (set|reset) <patch>");
		}
	}
	else if (args[0] == "plyr")
	{
		if (args.size() < 2) return;
		if(args[1] == "algo")
		{
			
		}
		else
		{
			printf("usage: /plyr (algo) ...");
		}
	}
	else if(args[0] == "reg")
	{
		if (args.size() < 2) return;
	}
	else
	{
		print("unknown command");
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

#pragma once

class th_command_proc
{

public:

	enum inp_state
	{
		WAITING,
		RECEIVING,
		BLOCKED
	};

	inp_state inpState = WAITING;

	th_command_proc();
	virtual ~th_command_proc();

	void handleInput(BYTE state[256], BYTE change[256]);
	void render(IDirect3DDevice9 *pD3dDev);
	void process(const char* args[]);
	void printf(const char* fmt, ...) const;
	void print(const char* str) const;

private:
	const static int PROMPT_BUFFER_SZ = 60;
	const static int PROMPT_PROLOGUE_SZ = 2;

	char promptRenderBuffer[PROMPT_BUFFER_SZ + PROMPT_PROLOGUE_SZ] = { '>', ' ' };
	char *promptBuffer	= promptRenderBuffer + PROMPT_PROLOGUE_SZ;
	int promptBufferPos	= 0;
};
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

	th_command_proc() {}

	void handleInput(BYTE state[256], BYTE change[256]);
	void render(IDirect3DDevice9 *pD3dDev);

private:
	const static int BUFFER_SZ = 60;
	const static int PROLOGUE_SZ = 2;

	char renderBuffer[BUFFER_SZ + PROLOGUE_SZ] = { '>', ' ' };
	char *buffer	= renderBuffer + PROLOGUE_SZ;
	int bufferPos	= 0;
	
};
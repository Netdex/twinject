#pragma once
#include <unordered_map>

class th_player;

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

	explicit th_command_proc(th_player *player) : player(player) {}
	virtual ~th_command_proc() = default;

	void handleInput(BYTE state[256], BYTE change[256]);
	void render(IDirect3DDevice9 *pD3dDev);
	void process(const std::vector<std::string> &args) const;
	void printf(const char* fmt, ...) const;
	void print(const char* str) const;

private:
	th_player *player;

	const static int PROMPT_BUFFER_SZ = 60;
	const static int PROMPT_PROLOGUE_SZ = 2;

	char promptRenderBuffer[PROMPT_BUFFER_SZ + PROMPT_PROLOGUE_SZ] = { '>', ' ' };
	char *promptBuffer	= promptRenderBuffer + PROMPT_PROLOGUE_SZ;
	int promptBufferPos	= 0;
};
#pragma once
#include <unordered_map>
#include <string>

class th_player;

class th_command_proc
{

public:
	enum input_state
	{
		WAITING,
		RECEIVING,
		BLOCKED
	};

	input_state inputState = WAITING;

	explicit th_command_proc(th_player *player) : player(player) {}
	virtual ~th_command_proc() = default;

	void handleInput(BYTE state[256], BYTE change[256]);
	void render(IDirect3DDevice9 *pD3dDev);
	void process(const std::vector<std::string> &args);
	void printf(const char* fmt, ...) const;
	void print(const char* str) const;

private:
	th_player *player;

	const static int PROMPT_BUFFER_SZ = 60;
	const static int PROMPT_PROLOGUE_SZ = 2;

	char promptRenderBuffer[PROMPT_BUFFER_SZ + PROMPT_PROLOGUE_SZ] = { '>', ' ' };
	char *promptBuffer	= promptRenderBuffer + PROMPT_PROLOGUE_SZ;
	int promptBufferPos	= 0;

	// command handlers
	void _cmd_patch(const std::vector<std::string> &args);
	void _cmd_algo(const std::vector<std::string> &args);
	void _cmd_reg(const std::vector<std::string> &args);

	// this part was a real pain to get right
	typedef void (th_command_proc::*command_proc_t)(const std::vector<std::string> &args);
	std::unordered_map<std::string, command_proc_t> commands {
		{"patch",	&th_command_proc::_cmd_patch },
		{"algo",	&th_command_proc::_cmd_algo },
		{"reg",		&th_command_proc::_cmd_reg }
	};

};
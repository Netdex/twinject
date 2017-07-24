#pragma once
#define DIRECTINPUT_VERSION 0x0900
#define _WIN32_WINNT 0x0500

#define GAME_WIDTH 384
#define GAME_HEIGHT 448
#define GAME_X_OFFSET 32
#define GAME_Y_OFFSET 16

#include <windows.h>
#include <tchar.h>
#include <detours.h>
#include <fstream>
#include <cstdio>
#include <vector>

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#include "logfs.h"

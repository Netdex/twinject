#pragma once
#define DIRECTINPUT_VERSION 0x0900
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <tchar.h>
#include <detours.h>
#include <fstream>
#include <cstdio>
#include <cmath>
//#include <cassert> // cassert doesn't work with dll injection

#include <vector>
#include <tuple>

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#include "logfs.h"

const float M_PI = 3.14159265358979323846f;



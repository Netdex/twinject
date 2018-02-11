# Twinhook Technical Description
## Functionality Summary
twinhook is a DLL which is injected into *Touhou Eiyashou ~ Imperishable Night* (henceforth referred to as IN), that allows direct interfacing with IN's nitty gritty bits. It's functionality can be split into several parts:

### Graphical Control
IN uses DirectX for its graphics. Originally it uses DirectX 8.0, but with the DX8 -> DX9 converter patch it can use DirectX 9.0. I chose to use DirectX 9.0 because I already had the SDK on my computer.

#### DX8 -> DX9
The DX8 -> DX9 patch consists of a file named d3d8.dll, that IN gladly uses instead of the one provided by the system, a process called DLL hijacking. The fake d3d8.dll then chainloads d3d9, loading it with the WinAPI's `LoadLibrary`.

#### Two-stage Hook + Wrapper
twinhook uses MS Detours to hook `LoadLibrary` globally, waiting for it to load d3d9.dll. Once it is loaded and we have a handle to it, we then hook `Direct3DCreate9` in d3d9. In our hooked function, we return a fake `IDirect3D9` which is actually a user-defined wrapper. Through this wrapper, we have complete control over IN's graphics, and can draw debugging information and visuals over the game.

This is mainly done through hooking `IDirect3DDevice9->EndScene()`, and drawing everything we want before the scene ends.


### Input Control
IN uses DirectInput8 for its keyboard input. The same technique is used as for graphics, but we hook `DirectInput8Create` directly now since dinput8.dll is loaded statically so the exported function is already available when twinhook is attached.

#### Single Stage Hook + Wrapper
Again, we return a wrapper of `IDirectInput8`, and gain control of `IDirectInput8Device->GetDeviceState()` to manipulate the input that IN sees. `GetDeviceState` modifies a 256-byte buffer to represent the keyboard's state - we can modify that returned buffer through the hook to control the keyboard.

### Information Gathering
We need a lot of information in order for our bot to be able to make decisions about the game. This is done by hooking various methods and extracting them off function parameters or memory locations. I used a combination of Ollydbg, x64dbg, Cheat Engine, and IDA Pro to reverse-engineer IN.

#### Bullet Locations
Using Cheat Engine, memory scanning was used (along with precariously pausing and resuming the game while bullets were moving) to discover subroutines that manipulated bullet locations. Although I couldn't find a routine that wrote bullet structures right into the memory, I found one that added a bullet's velocity to its position, as a part of the physics step. Every frame, this function is called once for every bullet, so we can just attach a hook to it and add the bullets to a list that is cleared every frame. We also know when each frame ends thanks to hooking to the graphics earlier.

The problem here is that this routine isn't only used for bullets, but is used to update positions of every single entity in the game. Therefore we cannot just trampoline the function, because we only want bullets.

The following snippet of code illustrates a possible solution:
```cpp
// BulletProcessHook.cpp

/* 
 * hook function that matches original function signature,
 * declared naked so we can manipulate the stack
 *
 * a1 is a pointer to a position D3DVECTOR
 * a3 is a pointer to a velocity D3DVECTOR
 * a2 is unused, but usually refers to the 'this' context
 */
__declspec(naked) int __fastcall sub_410A70_Hook(int a1, int a2, int a3)
{
	__asm {
		// get return address, put it somewhere safe
		mov eax, [esp]
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		// move __fastcall arguments into function parameters
		mov a1, ecx
		mov a2, edx
		mov ecx, [ebp + 8]
		mov a3, ecx
	}

	unsigned int addr;
	__asm mov addr, eax

    // do something with the entity data
	TH08_VectorUpdate_Hook(addr, a1, a2, a3);

	// return original value
	int retval;
	retval = sub_410A70_Original(a1, a2, a3);
	__asm {
		mov eax, retval

		mov esp, ebp
		pop ebp
		ret
	}
}
```

We can hook the function, but also extract the return address of the function so we know where it was called from. Therefore, we can compare the return address to the address right after the call from the bullet location update routine to see what type of entity is being updated. This is demonstrated in the following method:

```cpp
// TH08Control.cpp

void TH08_VectorUpdate_Hook(int retaddr, int a1, int a2, int a3)
{
	// routine from bullet update
	if (retaddr == 0x004314B3)
	{
		entity b;
		...
		TH08_Bullets.push_back(b);
	}
	else if(retaddr == 0x0044095B)
	{
		entity b;
		...
		TH08_Powerups.push_back(b);
	}
    ...
}
```

One extra note, at `*((DWORD*)a2 + 875))` is an identifier for the state of the current bullet. There are several different states, which are used for manipulating the bullet patterns (ex. curving in a certain direction, stopping temporarily, accelerating). I won't summarize them all here, but this information may be useful to a bot.

Bullets and the player are not actually circles, but are boxes. The collision detection does an axis-aligned bounding box check, which is fast but not as accurate to the graphics. Later games use hitcircles instead, since computational efficiency is not as important now. Bullets are defined by their position (the center of the box), and their size (width, height). These are available as two sequential floats at `*(float*)(a2 + 3380)`.

#### Powerup Locations
This is actually demonstrated above, it's the exact same process as the bullets but with a return address of `0x0044095B` instead.

One extra note, at `*(BYTE*)(a1 - 676 + 727)` is an identifier for the ephemerality of the powerup. Those tiny time orbs and other small particles count as powerups as well, but we don't want to target them because they'll be sucked into the player anyways. We will only target important powerups, such as point, power, bomb, and 1-up.

#### Player Location
This was luckily not that difficult, finding the memory location (it's static by the way) for the player location with Cheat Engine should be simple enough. It's two sequential floats (X, then Y) at `0x017D6110`.

```cpp
// TH08Control.cpp

PBYTE PlayerPosAddr = (PBYTE)0x017D6110;

D3DVECTOR TH08_GetPlayerLocation()
{
	D3DVECTOR pos{};
	pos.x = *(float*)PlayerPosAddr - GAME_X_OFFSET;
	pos.y = *(float*)(PlayerPosAddr + 4) - GAME_Y_OFFSET;
	return pos;
}
``` 

#### Enemy Locations
`TODO`
  
### Decision Making
Now that we have all this information and a way to manipulate the game, we can program a bot.

#### General Idea

Several other factors are taken into account to create the final algorithm:
- we don't want the player to run into bullets or enemies
- we don't want the player to run into the walls, so make the walls repulsive
- we want the player closer to the bottom middle of the screen, but don't confine them there because some spellcards need you to move a lot
- we don't want the player to prioritize anything (getting powerups, avoiding walls) over dodging bullets
- we don't want the player to chase powerups up to the top of the screen
- we want the player to prioritize dodging closer bullets
- we don't want the player to get stuck in a rut, chased into a corner by a wall of bullets
- we want the player to anticipate fast bullets
- ... and several other considerations.

##### Micrododging
So far the technique I am using is a local optimization based on Coulomb's law, treating the bullets, player, enemies, and powerups as charged particles. We cannot change the bullet, enemy, or powerup positions (actually we can, but that's cheating), so we only simulate the electrostatic interactions involving the player. The player has the same charge as bullets and enemies, but has an opposite charge from powerups. Therefore, the player is repelled from bullets and enemies, but attracted to powerups. Bullet size and player hitbox size is taken into account to make the player graze as close as possible.

This technique requires finetuning of several parameters for maximum effectiveness, I plan to either use a genetic algorithm or simulated annealing approach to optimize these parameters.

The problem here is that extremely fast bullets may not be reacted to quickly enough to dodge.

##### Macrododging
An additional technique I added to solve the fast bullet problem involves preemtively predicting bullet motion and sidestepping bullets in advance.


The net force vector is then used to determine which arrow keys to send to the game. Of course, Z is held down all the time, and SHIFT is useless for a precise bot (except some of the shot types are better in focus mode).


#### Deathbombing
Deathbombing can be implemented with the local optimization technique as well. If the effective electrostatic force acting on the player has a large magnitude, then the player is probably stuck, since if it can, it will try to move to a state that has the least force acting on it.

Perfect deathbombing can be implemented by patching IN and simply wiring out the deathbomb test, so that every death automatically deathbombs. However, I think this kidn of goes against the spirit of this project, so I'll try to use the alternative solution.

#### Focus Control
`TODO`

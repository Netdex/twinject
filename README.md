# thautomation

`thautomation` is a continuation of
[Netdex/twinject](https://github.com/Netdex/twinject), an injected automated
player for the Windows Touhou games. This fork focuses on filling in game
versions, object bindings, hooks, and player features that the original project
left unsupported.

The project is still experimental. Do not submit bot-generated replays to normal
replay leaderboards.

## One-click launcher

The old console injector and hand-edited TOML workflow have been replaced by a
native Windows launcher.

1. Extract the complete release package.
2. Run `THAutomation.exe`.
3. Select a game and browse to its executable, or drag the game EXE onto the
   launcher.
4. Click **Inject and launch**.

The launcher validates the game and hook DLL, remembers a separate path for each
game, starts the game in its own directory, reports actionable errors, and
monitors the launched process. The next launch only requires one click.

For TH06, TH07, and TH08, keep the packaged `legacy` directory beside the
launcher. When needed, the launcher installs the included DirectX 8 to DirectX 9
compatibility layer into the game directory. It never overwrites an existing
`d3d8.dll`.

## Support

This table is audited against the current source. A ✅ means that the binding
feeds usable objects to the algorithm; it does not mean that every regional or
patched executable has been tested. “Source” includes either a hook or guarded
memory polling.

| Binding / source | th06 | th07 | th08 | th09 | th10 | th11 | th15 |
|---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Bullets | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Enemies | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ |
| Powerups | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ |
| Lasers | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ✅ |
| Player / stage info | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Direct3D | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| DirectInput | ✅ | ✅ | ✅ | ✅ | ✅ | ✅* | ✅ |

\* th11 applies the included DirectInput compatibility patch.

| Feature | th06 | th07 | th08 | th09 | th10 | th11 | th15 |
|---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Dodge bullets | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Dodge enemies | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ |
| Dodge lasers | ✅ | ✅ | ❌ | ❌ | ✅ | ❌ | ✅ |
| Collect powerups | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ |
| Target enemies | ✅ | ✅ | ✅ | ❌ | ✅ | ✅ | ✅ |

Compared with the upstream table, this fork currently adds full th06 polling,
full th07 object support, th08 enemy polling, th11/th15 enemy and powerup
polling, th09 bullet polling, and enemy targeting for games that expose enemy
objects. Powerup collection and enemy targeting can be toggled independently in
the `th_vo_algo` debug window.

## Launcher features

- One-click injection and game launch
- Chinese dark-mode UI with live readiness status and launch log
- Per-game path memory; no configuration file editing
- EXE drag and drop plus nearby-game auto-detection
- 32-bit PE validation for both the game and hook DLL
- Clear DirectX runtime, file, permissions, and injection error messages
- Duplicate game-process protection
- Optional launch arguments
- Automatic legacy renderer installation for TH06–TH08
- Process monitoring and instant relaunch after the game exits

## Building

The dependencies are included in the repository and referenced by relative
path.

1. Clone with submodules.
2. Open `twinject.sln`.
3. Build the `Release | x86` configuration.

Or run:

```powershell
.\scripts\build.ps1
```

The build script finds an installed MSVC toolset, retargets when v142 is not
available, and creates both `THAutomation-release.zip` and an unpacked portable
package under `dist/`:

```text
dist/
├── THAutomation.exe
├── twinhook.dll
└── legacy/
    ├── d3d8.dll
    └── enbconvertor.ini
```

`twinhook.dll` uses `d3dx9_43.dll`. Install Microsoft's
[DirectX End-User Runtimes (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=8109)
if it is not already present.

The original project used Visual Studio 2017, the v141 C++ toolset, a Windows 10
SDK, and Visual C++ ATL. Newer Visual Studio versions can retarget the solution.

## Controls

- Enable/disable the bot: `G`
- Toggle debug rendering: `H`
- The in-game debug window exposes the remaining controls.

Enable the velocity-obstacle algorithm only after the player can move, because
it calibrates movement speed at startup.

## Credits

The original project and its credits are available at
[Netdex/twinject](https://github.com/Netdex/twinject). Game-internal information
used by this fork remains credited in the relevant source files.

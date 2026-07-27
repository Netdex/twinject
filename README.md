# thautomation

`thautomation` is a continuation of
[Netdex/twinject](https://github.com/Netdex/twinject), an injected automated
player for the Windows Touhou games. This fork focuses on filling in game
versions, object bindings, hooks, and player features that the original project
left unsupported.

The project is still experimental. Do not submit bot-generated replays to normal
replay leaderboards.

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

## Building

The dependencies are included in the repository and referenced by relative
path.

1. Clone with submodules.
2. Open `twinject.sln`.
3. Build the `Release | x86` configuration.

`twinhook.dll` uses `d3dx9_43.dll`. Install Microsoft's
[DirectX End-User Runtimes (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=8109)
if it is not already present.

The original project used Visual Studio 2017, the v141 C++ toolset, a Windows 10
SDK, and Visual C++ ATL. Newer Visual Studio versions can retarget the solution.

Copy these files into the target game directory:

- `twinject.exe`
- `twinhook.dll`
- `twinject.toml` (a sample is included under `deploy/`)

Older games may also require the included DirectX 8 to DirectX 9 converter.
See `deploy/INSTRUCTIONS.TXT` for usage details.

## Controls

- Enable/disable the bot: `G`
- Toggle debug rendering: `H`
- The debug window also exposes clickable controls.

Enable the velocity-obstacle algorithm only after the player can move, because
it calibrates movement speed at startup.

## Credits

The original project and its credits are available at
[Netdex/twinject](https://github.com/Netdex/twinject). Game-internal information
used by this fork remains credited in the relevant source files.

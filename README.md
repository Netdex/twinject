# twinject
`This project is a work-in-progress.`

`This has nothing to do with the auto-injector.`

![alt text?](https://i.imgur.com/uimgJ4L.png)
## What is this?
_twinject_ (**t**ouhou **w**indows **inject**or) is a bot for *Touhou Eiyashou ~ Imperishable Night*. Instead of using computer vision techniques to extract data from the game, a debugger is attached and the relevant game data is directly extracted.

This project's focus is not data extraction, but rather developing an effective dodging algorithm when all information is available.

So far, I plan to only implement for *Imperishable Night* with the d3d9 patch, because I like IN, and I cannot find SDKs for dxd8.

[Video Demonstration](https://www.youtube.com/watch?v=xiQNC4w72L4)

## Building
```
Download and build in Visual Studio 2017, compile with Release target.

Put twinhook.dll and twinject.exe into Touhou 08 ~ Imperishable Night directory.
Obtain dx8->dx9 converter patch (included in releases in this repo as dxd8.dll and enbconvertor.ini),
and place into game directory.
```

## Usage
```
-- INSTALLATION (from release) --
Extract all files to your Touhou 08 ~ Imperishable Night directory.

-- RUNNING --
Run twinject.exe, which will chainload th08.exe.
Make sure your game executable and the one defined in twinject.ini match.

-- CONTROLS --
G - Enable bot
B - Disable bot
H - Display debug graphics
N - Hide debug graphics 

To close the bot, close the console window and the game will close after.
Closing the game first may not close the console window properly.

Make sure DirectInput is not disabled.
```

## Troubleshooting
### Hotkeys do not work
Make sure DirectInput is not disabled.
![alt text?](https://i.imgur.com/r2unX8N.png)

### Graphical overlay does not appear
Make sure the dx8->dx9 patch is applied with `dxd8.dll` and `enbconvertor.ini`.

### Game does not open
Make sure you have DirectX 9 installed, preferrably the one from 2010.

### Extreme frame drop
Disable the debug visualization if it is enabled.

## Functionality
Consists of two parts, **twinhook** and **twinject**.

### twinhook
twinhook is a DLL, injected during runtime into a Touhou game. It uses MS Detours to trampoline functions.

[Technical Description](https://github.com/Netdex/twinject/blob/master/docs/twinhook_technical.md)

### twinject
twinject loads twinhook into th08.exe with Detours.

[Technical Description](https://github.com/Netdex/twinject/blob/master/docs/twinject_technical.md)


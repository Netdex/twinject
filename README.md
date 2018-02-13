# twinject
`This project is a work-in-progress.`

![alt text?](https://i.imgur.com/uimgJ4L.png)
## What is this?
_twinject_ (**t**ouhou **w**indows **inject**or) is an automated player for the bullet hell games from the *Touhou Project*. Instead of using computer vision techniques to extract data from the game, a DLL is injected and the relevant game data is directly extracted through trampolined functions.

For the unaware, bullet hells are a category of *Shoot 'em Up* video games where the player controls a ship, which must dodge large numbers of obstacles and destroy large numbers of enemies. In the demonstration video, the fast moving projectiles are the obstacles which must be dodged - if the player hits any of these projectiles they die immediately. The player itself also fires projectiles, which damage enemies. The player is the little red sprite near the bottom of the screen. Props to ZUN for making games that people still play 20 years later.

Why did I do this? I thought it would be a neat challenge to apply my knowledge of reverse engineering, and low-level development. This project's focus is not data extraction, but rather developing an effective dodging algorithm when all information is available. The dodging algorithm uses a mix of linear algebra and analytical optimization to quickly approximate good movements for the bot to make. This is explained in detail in the Technical Descriptions linked below.

So far, I plan to implement bindings for this bot for every mainline game. As of now, I have completed bindings for th08 with the d3d9 patch. th07 should follow soon, as their engines are very similar.

### [Simple Video Demonstration (Easy for the bot)](https://youtu.be/aW7tWWkghPY)

## Functionality
Consists of two parts, **twinhook** and **twinject**.

### twinhook
twinhook is a DLL, injected during runtime into a Touhou game. It uses MS Detours to trampoline functions.

[Technical Description](https://github.com/Netdex/twinject/blob/master/docs/twinhook_technical.md)

### twinject
twinject loads twinhook into th08.exe with Detours.

[Technical Description](https://github.com/Netdex/twinject/blob/master/docs/twinject_technical.md)

## Building
```
Download and build in Visual Studio 2017, compile with Release (x86) target.

Put twinhook.dll and twinject.exe into the game directory.
Obtain dx8->dx9 converter patch (included in releases in this repo as dxd8.dll and enbconvertor.ini),
and place into game directory if the game requires it.
```

## Usage
```
-- INSTALLATION (from release) --
Extract all files to your game directory.

-- RUNNING --
Run twinject.exe, which will chainload the game executable.
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
Also, make sure `twinhook.dll` is **not** compiled with the Debug target, as it adds debugging code which mangles the hooks for the DirectInput8 hook.

### Graphical overlay does not appear
If the game requires the dx8->dx9 patch, make sure it is applied (`dxd8.dll` and `enbconvertor.ini`).

### Game does not open
Make sure the game actually runs without twinject first (make sure you have DirectX 9 installed, preferrably the package from 2010). 
Also make sure that the correct game executable is set in twinject.ini (create this file if it does not exist, there's an example config in the Releases).

### Extreme frame drop
Disable the debug visualization if it is enabled.

#### [Alternate Simple Video Demonstration (Easy for the bot)](https://youtu.be/lxQqjiYvZiE)
#### [Complex Video Demonstration (Difficult for the bot)](https://www.youtube.com/watch?v=xiQNC4w72L4)

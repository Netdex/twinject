# twinject
`This project is a work-in-progress.`

![alt text?](https://i.imgur.com/uimgJ4L.png)
## What is this?
_twinject_ (**t**ouhou **w**indows **inject**or) is an automated player for the bullet hell games from the *Touhou Project*. Instead of using computer vision techniques to extract data from the game, a DLL is injected and the relevant game data is directly extracted through trampolined functions.

For the unaware, bullet hells are a category of *Shoot 'em Up* video games where the player controls a ship, which must dodge large numbers of obstacles and destroy large numbers of enemies. In the demonstration videos, the fast moving projectiles are the obstacles which must be dodged - if the player hits any of these projectiles they die immediately. The player itself also fires projectiles, which damage enemies. The player is the little red sprite near the bottom of the screen. Props to ZUN for making games that people still play 20 years later.

Why did I do this? I thought it would be a neat challenge to apply my knowledge of reverse engineering, and low-level development. This project's focus is not data extraction, but rather developing an effective dodging algorithm when all information is available. The dodging algorithm uses a mix of linear algebra and numerical optimization to quickly approximate good movements for the bot to make. This is explained in detail in the Technical Descriptions linked below.

So far, I plan to implement bindings for this bot for every mainline game. As of now, I have completed basic bindings for th07, th08 and th15. Since all the necessary abstractions already exist, I also plan to utilize machine learning strategies as a possible solution.

## Demonstrations

### Constrained Velocity Obstacle Algorithm
Watch as it struggles to deathbomb through every laser it sees.  
#### [1CC LoLK Normal](https://www.youtube.com/watch?v=lwiM6dhBiYQ)  
#### [LoLK Stage 6](https://youtu.be/9WElfhVE-Lk)

### Vector Dodging + Method of Virtual Potential Field Algorithm
#### [Alternate Simple Video Demonstration (Easy for the bot)](https://youtu.be/lxQqjiYvZiE)  
#### [Complex Video Demonstration (Difficult for the bot)](https://www.youtube.com/watch?v=xiQNC4w72L4)  
#### [Simple Video Demonstration (Easy for the bot)](https://youtu.be/aW7tWWkghPY)

## Support
|Feature               |th07 |th08 |th10 |th15 |
|----------------------|-----|-----|-----|-----|
|Dodge Bullet          |✔️   |✔️  |✔️  |✔️  |
|Dodge Enemy           |❌   |❌  |✔️  |✔️  |
|Dodge Laser           |❌   |❌  |✔️  |✔️  |
|Collect Powerups      |❌   |✔️  |✔️  |❌  |
|Target Enemies        |❌   |❌  |❌  |❌  |

... and some other undocumented features.

## Documentation of Functionality
Consists of two parts, **twinhook** and **twinject**.

### twinhook
twinhook is a DLL, injected before runtime into a Touhou game. It uses MS Detours to trampoline functions.  
This technical description is now a bit out of date, as radical changes were made to its functionality. I will update the documentation in the near future.

[Technical Description](https://github.com/Netdex/twinject/blob/master/desc/twinhook_technical.md)

### twinject
twinject loads twinhook into game with MS Detours.

[Technical Description](https://github.com/Netdex/twinject/blob/master/desc/twinject_technical.md)

### Code Structure
#### [Doxygen](https://rawgit.com/Netdex/twinject/master/docs/html/index.html) (mostly undocumented)

Hooks for different parts of games are created. Some hooks can be reused across games (Direct3D, DirectInput), some are game specific (bullet processing hooks).

An algorithm is a methodology to process game data, and determine player movements. Multiple implementations of algorithms are/will be supported, including:
- Method of Velocity Vector and Virtual Potential Field
- Method of Constrained Vector Obstacle
- Method of Artificial Neural Network

The automated player is a game-specific wrapper for each game that twinject supports. It gathers information from hooks and delivers them to the algorithm. This is required since every game requires different hooks, and may have some game-specific quirk that needs to be handled.

Each game has a startup routine. This routine is responsible for intializing hooks at the correct time, setting up the chosen algorithm, and initializing the correct automated player for each game.

## Building
The dependencies are included in the repository, and are referenced by relative path.

### Build Instructions
```
1. Clone repository to disk
2. Build in Visual Studio 2017 with Release (x86) target.

Place the following files into the game directory:
twinject.exe
twinhook.dll
twinject.ini (you may need to create this yourself, specified below)

Obtain dx8->dx9 converter patch (included in releases in this repo as dxd8.dll and enbconvertor.ini),
and place into game directory if the game requires it.
```

### Configuration
twinject.ini must be in the same folder as twinject.exe, and contains configuration options. Here's the default configuration:
```
[twinject]
exename=th08.exe
dllname=twinhook.dll
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

To close the bot, close the debug console window and the game will close after.
Closing the game first may cause cleanup to not occur.

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

## Ethics
### Replays
Note that this bot generates valid replays. **Do not submit replays** generated by this bot to replay aggregators such as royalflare, as they are prohibited. You may only submit to a tool-assisted category if they exist.

## 3rd Party
This code is [licensed under the MIT](https://github.com/Netdex/twinject/blob/master/LICENSE).

Information regarding Touhou game internals to assist in reverse engineering have been used from the following sources:  
[TH10_Collision_Points by binvec](https://github.com/binvec/TH10_Collision_Points)  
[thinput by Drake](https://www.shrinemaiden.org/forum/index.php?topic=16024.0)  
[Nice cheat.](http://cheater.seesaa.net/)  
[Ethan Silver's Junkpile](https://web.archive.org/web/20130630114418/http://diceroller.site40.net/?page=touhou_12e)  

Thank you.

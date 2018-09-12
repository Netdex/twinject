# twinject 
[![Build status](https://ci.appveyor.com/api/projects/status/wy06i4vscngsel5n?svg=true)](https://ci.appveyor.com/project/Netdex/twinject) 
### [Click here to visit Wiki (documentation)](https://github.com/Netdex/twinject/wiki)
```
This project is a work-in-progress, new features are being added every week. 
At the moment all of my resources are focused on development, and very few are 
allocated to documentation. As a result, some of the documentation here might be 
out-of-date or just plain wrong.
```
```
If you have any questions, inquiries, want to help, or are just having trouble getting 
something working, feel free to open a ticket or email me (my email address is on my GitHub profile). 
It's much easier to answer emails when they come rather than keep the documentation here up-to-date 
all the time, so I apologize if any of the documentation here is confusing or wrong.
```
## [Demonstration Video](https://youtu.be/-3xhwImamQw)
[![alt text?](https://i.imgur.com/Nos4Z1D.png)](https://youtu.be/-3xhwImamQw)

## What is this?
_twinject_ (**t**ouhou **w**indows **inject**or) is an automated player for the bullet hell games from the *Touhou Project*. Instead of using computer vision techniques to extract data from the game, a DLL is injected and the relevant game data is directly extracted through trampolined functions. _twinject_'s first goal is to expose game information and APIs to interact with the game.

For the unaware, bullet hells are a category of *Shoot 'em Up* video games where the player controls a ship, which must dodge large numbers of obstacles and destroy large numbers of enemies. In the demonstration videos, the fast moving projectiles are the obstacles which must be dodged - if the player hits any of these projectiles they die immediately. The player itself also fires projectiles, which damage enemies. Props to ZUN for making games that people still play 20 years later.

Why did I do this? I thought it would be a neat challenge to apply my knowledge of reverse engineering, and low-level development. This project's focus is not data extraction, but rather developing an effective dodging algorithm when all information is available. The dodging algorithm uses a mix of linear algebra and numerical optimization to quickly approximate good movements for the bot to make. This is explained in detail in the Technical Descriptions linked below.

Documentation is currently incomplete/outdated, and is currently on my list of things to do. At the moment, I am prioritizing features and demonstrations.

So far, I plan to implement bindings for this bot for every mainline game. As of now, I have completed basic bindings for games listed in the table below. Some binding implementations or reverse engineering knowledge have been borrowed from other sources and are cited at the bottom. Thank you! Since all the necessary abstractions already exist, I also plan to utilize machine learning strategies as a possible solution.

## Demonstrations
As of now, I have implemented two player algorithms with varying degrees of success.

### Constrained Velocity Obstacle Algorithm
The constrained velocity obstacle algorithm uses a linear velocity obstacle predictor based approach with an infinite horizon but constrained bounds, and picks the best possible move at the current instant in a greedy fashion.  
Note that when these demonstrations were recorded, the bot was incapable of dodging lasers. Watch as it struggles to deathbomb through every laser it sees.  
#### [MoF 1CC Normal](https://youtu.be/-3xhwImamQw)
#### [MoF Stage 3 Lunatic](https://youtu.be/UKoz499YXr8)
#### [1CC LoLK Normal](https://www.youtube.com/watch?v=lwiM6dhBiYQ)  
#### [LoLK Stage 6](https://youtu.be/9WElfhVE-Lk)

More demonstrations are available in the [wiki](https://github.com/Netdex/twinject/wiki).

## Support
|Bindings              |th07 |th08 |th10 |th11 |th15 |
|----------------------|-----|-----|-----|-----|-----|
|Bullet Hook           |✔️   |✔️  |✔️  |✔️  |✔️  |
|Enemy Hook            |❌   |❌  |✔️  |❌  |❓   |
|Powerup Hook          |❌   |✔️  |✔️  |❌  |❌  |
|Laser Hook            |❌   |❌  |✔️  |❌  |✔️  |
|Stage Info Hook       |✔️   |✔️  |✔️  |✔️  |✔️  |
|Direct3D Hook         |✔️   |✔️  |✔️  |✔️  |✔️  |
|DirectInput Hook      |✔️   |✔️  |✔️  |✔️* |✔️  |

|Feature               |th07 |th08 |th10 |th11 |th15 |
|----------------------|-----|-----|-----|-----|-----|
|Dodge Bullet          |✔️   |✔️  |✔️  |✔️  |✔️  |
|Dodge Enemy           |❌   |❌  |✔️  |❌  |✔️  |
|Dodge Laser           |❌   |❌  |✔️  |❌  |✔️  |
|Collect Powerups      |❌   |✔️  |✔️  |❌  |❌  |
|Target Enemies        |❌   |❌  |❌  |❌  |❌  |

... and some other undocumented features. Keep in mind that this table probably isn't kept up to date.

## Building
The dependencies are included in the repository, and are referenced by relative path.

### Build Instructions
```
1. Clone repository to disk
2. Build in Visual Studio 2017 with Release (x86) target.
The following components are required:
- VC++ 2017 version 15.8 v14.5 latest v141 tools (other verisons should be okay, you will need to retarget)
- Windows 10 SDK (10.0.17134.0) (other versions should be okay, you will need to retarget)
- Visual C++ ATL for x86 and x64 (technically this isn't required, but I used CString for convenience)

Place the following files into the game directory:
twinject.exe
twinhook.dll
twinject.ini (you may need to create this yourself, specified below)

Obtain dx8->dx9 converter patch (included in releases in this repo as dxd8.dll and enbconvertor.ini),
and place into game directory if the game requires it.
```
If you don't want to build it, there are stable Releases in this repository. The tagged commits represent stable points, since sometimes I break the build and it doesn't work. If you want to test the latest features (since the Releases take effort to create, so I don't do them often), you can download the build artifacts from [Appveyor](https://ci.appveyor.com/project/netdex/twinject/build/artifacts). If you are having trouble getting it to build, feel free to open a ticket or contact me.

### Configuration
twinject.ini must be in the same folder as twinject.exe, and contains configuration options. Here's the default configuration:
```
[twinject]
; Filename of game executable
exename=th08.exe
; Filename of twinhook dll
dllname=twinhook.dll
; Name of game loader (e.g. th=th08)
env=th=th08
```

## Usage
```
-- INSTALLATION (from release) --
Extract all files to your game directory.

-- RUNNING --
Run twinject.exe, which will chainload the game executable.
Make sure your game executable and the one defined in twinject.ini match.

-- CONTROLS --
G - Enable/disable bot
H - Show/hide debug graphics
/ - Input debug command

You can click the IMGUI windows to control twinject as well.  
(note: key controls have been temporarily replaced with IMGUI while I sort stuff out.)

To close the bot, close the debug console window and the game will close after.
Closing the game first may cause cleanup to not occur.

If you are using the th_vo_algo (the default), make sure you only enable the bot after the stage loads and the player is able to move, 
since the th_vo_algo needs to calibrate the player by moving it around. You should see a log message after successful calibration.

Make sure DirectInput is not disabled, it is required for movement.

If you have any other problems, please read the troubleshooting section below. 
If you still have any other problems, feel free to contact me. 
You can either make an issue or email me (my email is on my GitHub profile).
```

## [Architecture](https://github.com/Netdex/twinject/wiki/Architecture)

## [Troubleshooting](https://github.com/Netdex/twinject/wiki/Troubleshooting)

## Ethics
### Replays
Note that this bot generates valid replays. **Do not submit replays** generated by this bot to replay aggregators such as royalflare, as they are prohibited. You may only submit to a tool-assisted category if they exist.

## 3rd Party
This code is [licensed under the specified terms here](https://github.com/Netdex/twinject/blob/master/LICENSE).

Libraries and their licenses are documented under `dependencies/`. 

Information regarding Touhou game internals to assist in reverse engineering have been used from the following sources:  
[TH10_Collision_Points by binvec](https://github.com/binvec/TH10_Collision_Points)  
[thinput by Drake](https://www.shrinemaiden.org/forum/index.php?topic=16024.0)  
[Nice cheat.](http://cheater.seesaa.net/)  
[Ethan Silver's Junkpile](https://web.archive.org/web/20130630114418/http://diceroller.site40.net/?page=touhou_12e)  

Thank you.

# twinject
`This project is a work-in-progress.`

`This has nothing to do with the auto-injector.`

## What is this?
_twinject_ (**t**ouhou **w**indows **inject**or) is a bot for *Touhou Eiyashou ~ Imperishable Night*. Instead of using computer vision techniques to extract data from the game, a debugger is attached and the relevant game data is directly extracted.

This project's focus is not data extraction, but rather developing an effective dodging algorithm when all information is available.

So far, I plan to only implement for *Imperishable Night* with the d3d9 patch, because I like IN, and I cannot find SDKs for dxd8.



## Functionality
Consists of two parts, **twinhook** and **twinject**.

### twinhook
twinhook is a DLL, injected during runtime into a Touhou game. It uses MS Detours to trampoline functions.

[Technical Description](http://github.com/netdex/twinject/docs/twinhook_technical.md)

### twinject
twinject loads twinhook into th08.exe with Detours.

[Technical Description](http://github.com/netdex/twinject/docs/twinject_technical.md)

# Twinject Technical Description
## Functionality Summary
Injects twinhook into the game using MS Detours.

## Technical
twinhook is injected using `DetourCreateProcessWithDll`, which starts the process in a suspended state, injects the DLL, then starts it back up again. This ensures the DLL is loaded before the program starts running.

twinject also acts as a debugger, displaying debug output from twinhook, catching exceptions, and possibly more future functionality.

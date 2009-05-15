2009-04-27 Sago007

You can now compile the OpenArena mod SDK in Windows!

use the batch files for baseoa:
windows_compile_game.bat
windows_compile_cgame.bat
windows_compile_q3_ui.bat

Follwing for missionpack:
windows_compile_game_missionpack.bat
windows_compile_cgame_missionpack.bat
windows_compile_ui_missionpack.bat

The qvm files will be placed in:
windows\baseoa\vm

or

windows\missionpack\vm

The batch files has no error checking so you need to read the output. Feel free to improve the scripts.

The binary blobs:
lcc.exe
q3cpp.exe
q3rpp.exe
q3asm.exe

id software provided both the source and precompiled binaries so I guess they knew how hard these files are to compile. If you succed in compiling them please tell how. I would really apreciate if anyone can get ioquake3's improved qvm tools compiled as those provided here has some bugs.


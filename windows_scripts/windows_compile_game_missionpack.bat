cd ..
mkdir windows
mkdir windows\build
mkdir windows\missionpack
mkdir windows\missionpack\vm
mkdir windows\build\game
copy windows_scripts\lcc.exe  windows\build\game\
copy windows_scripts\q3cpp.exe  windows\build\game\
copy windows_scripts\q3rcc.exe  windows\build\game\
copy windows_scripts\q3asm.exe  windows\build\game\
copy windows_scripts\game_mp.q3asm windows\build\game.q3asm

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -DMISSIONPACK -S -Wf-target=bytecode -Wf-g -I..\..\..\code\game -I..\..\..\code\qcommon

cd windows\build\game

copy  ..\..\..\code\game\g_syscalls.asm ..

for /f "tokens=*" %%i in (../../../oalist-gamemp.txt) DO %cc% ../../../%%i

q3asm -f ../game

cd ..\..\..
pause

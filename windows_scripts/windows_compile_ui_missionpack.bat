cd ..
mkdir windows
mkdir windows\build
mkdir windows\missionpack
mkdir windows\missionpack\vm
mkdir windows\build\ui
copy windows_scripts\lcc.exe  windows\build\ui\
copy windows_scripts\q3cpp.exe  windows\build\ui\
copy windows_scripts\q3rcc.exe  windows\build\ui\
copy windows_scripts\q3asm.exe  windows\build\ui\
copy windows_scripts\ui.q3asm windows\build\

set LIBRARY=
set INCLUDE=

set cc=lcc -DMISSIONPACK -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\ui -I..\..\..\code\qcommon %1

cd windows\build\ui

%cc%  ../../../code/ui/ui_atoms.c
%cc%  ../../../code/ui/ui_gameinfo.c
%cc%  ../../../code/ui/ui_main.c
%cc%  ../../../code/ui/ui_players.c
%cc%  ../../../code/ui/ui_shared.c

copy  ..\..\..\code\ui\ui_syscalls.asm ..

%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../ui

cd ..\..\..
pause

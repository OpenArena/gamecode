cd ..
mkdir windows
mkdir windows\build
mkdir windows\baseoa
mkdir windows\baseoa\vm
mkdir windows\build\cgame
copy windows_scripts\lcc.exe  windows\build\cgame\
copy windows_scripts\q3cpp.exe  windows\build\cgame\
copy windows_scripts\q3rcc.exe  windows\build\cgame\
copy windows_scripts\q3asm.exe  windows\build\cgame\
copy windows_scripts\cgame.q3asm windows\build\

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\cgame -I..\..\..\code\qcommon %1

cd windows\build\cgame

%cc%  ../../../code/cgame/cg_challenges.c
%cc%  ../../../code/cgame/cg_consolecmds.c
%cc%  ../../../code/cgame/cg_draw.c
%cc%  ../../../code/cgame/cg_drawtools.c
%cc%  ../../../code/cgame/cg_effects.c
%cc%  ../../../code/cgame/cg_ents.c
%cc%  ../../../code/cgame/cg_event.c
%cc%  ../../../code/cgame/cg_info.c
%cc%  ../../../code/cgame/cg_localents.c
%cc%  ../../../code/cgame/cg_main.c
%cc%  ../../../code/cgame/cg_marks.c
%cc%  ../../../code/cgame/cg_newdraw.c
%cc%  ../../../code/cgame/cg_particles.c
%cc%  ../../../code/cgame/cg_players.c
%cc%  ../../../code/cgame/cg_playerstate.c
%cc%  ../../../code/cgame/cg_predict.c
%cc%  ../../../code/cgame/cg_scoreboard.c
%cc%  ../../../code/cgame/cg_servercmds.c
%cc%  ../../../code/cgame/cg_snapshot.c
%cc%  ../../../code/cgame/cg_unlagged.c
%cc%  ../../../code/cgame/cg_view.c
%cc%  ../../../code/cgame/cg_weapons.c

%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c
%cc%  ../../../code/game/bg_pmove.c
%cc%  ../../../code/game/bg_slidemove.c

copy  ..\..\..\code\cgame\cg_syscalls.asm ..

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../cgame

cd ..\..\..
pause

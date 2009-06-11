cd ..
mkdir windows
mkdir windows\build
mkdir windows\baseoa
mkdir windows\baseoa\vm
mkdir windows\build\game
copy windows_scripts\lcc.exe  windows\build\game\
copy windows_scripts\q3cpp.exe  windows\build\game\
copy windows_scripts\q3rcc.exe  windows\build\game\
copy windows_scripts\q3asm.exe  windows\build\game\
copy windows_scripts\game.q3asm windows\build\

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\game -I..\..\..\code\qcommon %1

cd windows\build\game

%cc%  ../../../code/game/ai_chat.c
%cc%  ../../../code/game/ai_cmd.c
%cc%  ../../../code/game/ai_dmnet.c
%cc%  ../../../code/game/ai_dmq3.c
%cc%  ../../../code/game/ai_main.c
%cc%  ../../../code/game/ai_team.c
%cc%  ../../../code/game/ai_vcmd.c
%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c
%cc%  ../../../code/game/bg_pmove.c
%cc%  ../../../code/game/bg_slidemove.c
%cc%  ../../../code/game/g_active.c
%cc%  ../../../code/game/g_admin.c
%cc%  ../../../code/game/g_arenas.c
%cc%  ../../../code/game/g_bot.c
%cc%  ../../../code/game/g_client.c
%cc%  ../../../code/game/g_cmds.c
%cc%  ../../../code/game/g_cmds_ext.c
%cc%  ../../../code/game/g_combat.c
%cc%  ../../../code/game/g_items.c
%cc%  ../../../code/game/g_main.c
%cc%  ../../../code/game/bg_alloc.c
%cc%  ../../../code/game/g_fileops.c
%cc%  ../../../code/game/g_killspree.c
%cc%  ../../../code/game/g_misc.c
%cc%  ../../../code/game/g_missile.c
%cc%  ../../../code/game/g_mover.c
%cc%  ../../../code/game/g_playerstore.c
rem %cc%  ../../../code/game/g_rankings.c
%cc%  ../../../code/game/g_session.c
%cc%  ../../../code/game/g_spawn.c
%cc%  ../../../code/game/g_svcmds.c
%cc%  ../../../code/game/g_svcmds_ext.c
rem %cc%  ../../../code/game/g_syscalls.c
copy  ..\..\..\code\game\g_syscalls.asm ..
%cc%  ../../../code/game/g_target.c
%cc%  ../../../code/game/g_team.c
%cc%  ../../../code/game/g_trigger.c
%cc%  ../../../code/game/g_unlagged.c
%cc%  ../../../code/game/g_utils.c
%cc%  ../../../code/game/g_vote.c
%cc%  ../../../code/game/g_weapon.c

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../game

cd ..\..\..
pause

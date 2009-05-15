cd ..
mkdir windows
mkdir windows\build
mkdir windows\baseoa
mkdir windows\baseoa\vm
mkdir windows\build\q3_ui
copy windows_scripts\lcc.exe  windows\build\q3_ui\
copy windows_scripts\q3cpp.exe  windows\build\q3_ui\
copy windows_scripts\q3rcc.exe  windows\build\q3_ui\
copy windows_scripts\q3asm.exe  windows\build\q3_ui\
copy windows_scripts\q3_ui.q3asm windows\build\

set LIBRARY=
set INCLUDE=

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\q3_ui -I..\..\..\code\qcommon %1

cd windows\build\q3_ui

%cc%  ../../../code/q3_ui/ui_addbots.c
%cc%  ../../../code/q3_ui/ui_atoms.c
%cc%  ../../../code/q3_ui/ui_cdkey.c
%cc%  ../../../code/q3_ui/ui_challenges.c
%cc%  ../../../code/q3_ui/ui_cinematics.c
%cc%  ../../../code/q3_ui/ui_confirm.c
%cc%  ../../../code/q3_ui/ui_connect.c
%cc%  ../../../code/q3_ui/ui_controls2.c
%cc%  ../../../code/q3_ui/ui_credits.c
%cc%  ../../../code/q3_ui/ui_demo2.c
%cc%  ../../../code/q3_ui/ui_display.c
%cc%  ../../../code/q3_ui/ui_firstconnect.c
%cc%  ../../../code/q3_ui/ui_gameinfo.c
%cc%  ../../../code/q3_ui/ui_ingame.c
%cc%  ../../../code/q3_ui/ui_loadconfig.c
%cc%  ../../../code/q3_ui/ui_login.c
%cc%  ../../../code/q3_ui/ui_main.c
%cc%  ../../../code/q3_ui/ui_menu.c
%cc%  ../../../code/q3_ui/ui_mfield.c
%cc%  ../../../code/q3_ui/ui_mods.c
%cc%  ../../../code/q3_ui/ui_network.c
%cc%  ../../../code/q3_ui/ui_options.c
%cc%  ../../../code/q3_ui/ui_password.c
%cc%  ../../../code/q3_ui/ui_playermodel.c
%cc%  ../../../code/q3_ui/ui_players.c
%cc%  ../../../code/q3_ui/ui_playersettings.c
%cc%  ../../../code/q3_ui/ui_preferences.c
%cc%  ../../../code/q3_ui/ui_qmenu.c
%cc%  ../../../code/q3_ui/ui_rankings.c
%cc%  ../../../code/q3_ui/ui_rankstatus.c
%cc%  ../../../code/q3_ui/ui_removebots.c
%cc%  ../../../code/q3_ui/ui_saveconfig.c
%cc%  ../../../code/q3_ui/ui_serverinfo.c
%cc%  ../../../code/q3_ui/ui_servers2.c
%cc%  ../../../code/q3_ui/ui_setup.c
rem %cc%  ../../../code/q3_ui/ui_signup.c
%cc%  ../../../code/q3_ui/ui_sound.c
%cc%  ../../../code/q3_ui/ui_sparena.c
%cc%  ../../../code/q3_ui/ui_specifyleague.c
%cc%  ../../../code/q3_ui/ui_specifyserver.c
%cc%  ../../../code/q3_ui/ui_splevel.c
%cc%  ../../../code/q3_ui/ui_sppostgame.c
%cc%  ../../../code/q3_ui/ui_spreset.c
%cc%  ../../../code/q3_ui/ui_spskill.c
%cc%  ../../../code/q3_ui/ui_startserver.c
%cc%  ../../../code/q3_ui/ui_team.c
%cc%  ../../../code/q3_ui/ui_teamorders.c
%cc%  ../../../code/q3_ui/ui_video.c
%cc%  ../../../code/q3_ui/ui_votemenu.c
%cc%  ../../../code/q3_ui/ui_votemenu_fraglimit.c
%cc%  ../../../code/q3_ui/ui_votemenu_custom.c
%cc%  ../../../code/q3_ui/ui_votemenu_gametype.c
%cc%  ../../../code/q3_ui/ui_votemenu_kick.c
%cc%  ../../../code/q3_ui/ui_votemenu_map.c
%cc%  ../../../code/q3_ui/ui_votemenu_timelimit.c

copy  ..\..\..\code\ui\ui_syscalls.asm ..

%cc%  ../../../code/game/bg_lib.c
%cc%  ../../../code/game/bg_misc.c

%cc%  ../../../code/qcommon/q_math.c
%cc%  ../../../code/qcommon/q_shared.c

q3asm -f ../q3_ui

cd ..\..\..
pause

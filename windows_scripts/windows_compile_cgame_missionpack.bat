cd ..
mkdir windows
mkdir windows\build
mkdir windows\missionpack
mkdir windows\missionpack\vm
mkdir windows\build\cgame
copy windows_scripts\lcc.exe  windows\build\cgame\
copy windows_scripts\q3cpp.exe  windows\build\cgame\
copy windows_scripts\q3rcc.exe  windows\build\cgame\
copy windows_scripts\q3asm.exe  windows\build\cgame\
copy windows_scripts\cgame_mp.q3asm windows\build\cgame.q3asm

set LIBRARY=
set INCLUDE=

set cc=lcc -DMISSIONPACK -DSCRIPTHUD -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\cgame -I..\..\..\code\ui -I..\..\..\code\qcommon

cd windows\build\cgame

copy  ..\..\..\code\cgame\cg_syscalls.asm ..

for /f "tokens=*" %%i in (../../../oalist-cgamemp.txt) DO %cc% ../../../%%i

q3asm -f ../cgame

cd ..\..\..
pause

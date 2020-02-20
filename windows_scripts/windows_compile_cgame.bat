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

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\cgame -I..\..\..\code\qcommon

cd windows\build\cgame

copy  ..\..\..\code\cgame\cg_syscalls.asm ..

for /f "tokens=*" %%i in (../../../oalist-cgame.txt) DO %cc% ../../../%%i

q3asm -f ../cgame

cd ..\..\..
pause

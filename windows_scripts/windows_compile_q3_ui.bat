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

set cc=lcc -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\q3_ui -I..\..\..\code\qcommon

cd windows\build\q3_ui

copy  ..\..\..\code\ui\ui_syscalls.asm ..

for /f "tokens=*" %%i in (../../../oalist-ui.txt) DO %cc% ../../../%%i

q3asm -f ../q3_ui

cd ..\..\..
pause

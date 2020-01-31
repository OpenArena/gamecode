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

set cc=lcc -DMISSIONPACK -DSCRIPTHUD  -DQ3_VM -S -Wf-target=bytecode -Wf-g -I..\..\..\code\ui -I..\..\..\code\qcommon

cd windows\build\ui

copy  ..\..\..\code\ui\ui_syscalls.asm ..

for /f "tokens=*" %%i in (../../../oalist-uimp.txt) DO %cc% ../../../%%i

q3asm -f ../ui

cd ..\..\..

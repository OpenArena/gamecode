cmd.exe "/c windows_compile_cgame.bat"
cmd.exe "/c windows_compile_cgame_missionpack.bat"
cmd.exe "/c windows_compile_game.bat"
cmd.exe "/c windows_compile_game_missionpack.bat"
cmd.exe "/c windows_compile_q3_ui.bat"
cmd.exe "/c windows_compile_ui_missionpack.bat"
pushd ../windows/baseoa
cp -r ../../botfiles botfiles
../../windows_scripts/zip oax.pk3 vm/*.qvm
../../windows_scripts/zip oax.pk3 botfiles/*.*
popd
pushd ../windows/missionpack
cp -r ../../botfiles botfiles
../../windows_scripts/zip oax-mp.pk3 vm/*.qvm
../../windows_scripts/zip oax-mp.pk3 botfiles/*.*
popd

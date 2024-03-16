# OpenArena eXpanded Changelog resume

## Important changes since OA 0.8.8

### Laconic version

**Note:** An ampersand ("&") preceding a value means that the cvar is a bitflag, and you must add said value to the total before using its effect (i.e. if dmflags is 1096 and you want to add the new underwater speed, then dmflags should be 5192 (1096+4096).

* New gametype: Possession:
  * Can be played on all maps (for now, for testing purposes).
  * Free-For-All based gametype.
  * Hold a flag for the longest amount of time in order to win, or reach the holding limit. (Default 120, 2 minutes).
  * The flag is the white flag from One Flag CTF. It spawns at either the designated spawnpoint (`team_CTF_neutralflag`), a Domination control point (`domination_point`) or a regular spawnpoint (`info_player_deathmatch` or `info_player_start`).
  * `g_voteGametypes`: Possession is a votable gametype (`13/`).
* New cvars and values:
  * `g_awardpushing 2`: [Rewards the last attacker who pushed the fragged player into a hazard within 5 seconds, even if the fragged player wasn't airborne.](https://openarena.ws/board/index.php?topic=5289.msg54334#msg54334)
  * `cg_bob`: If set to 0, it will disable cg_bobUp/cg_bobRoll/cg_bobPitch. **Default: 1.**
  * `cg_kickScale`: Controls how much the screen shakes when receiving damage. **Default: 1.0.**
  * `cg_muzzleflashStyle`: Alternative muzzleflash styles for player preference, or "aesthetic," or maybe just less flashing (flashes can also be disabled). **Default: 1.**
  * `g_runes 2`: Enables the `tossrune` command (see below) in the server.
  * `dmflags &4096`: Allows players to move significantly faster underwater. Mostly for demonstration. Could be fun in class based gametypes.
  * `g_grapple`: Gives Grappling Hook to all players. Replaces `elimination_grapple`. **Default: 0.**
  * `g_harvesterFromBodies`: In Harvester matches, skulls now spawn from dead bodies (a la [UT3!Greed](https://antifandom.com/unreal/wiki/Greed)) rather than a skull receptacle in the middle of the arena. Allows Harvester matches to take place in maps that don't feature a skull receptacle. **Default: 0.**
  * `g_ddCaptureTime` and `g_ddRespawnDelay`: New cvars for Double Domination that control the amount of holding time to score and the waiting time before a new round starts. **Default for both: 10.**
  * `g_weaponArena` and `g_weaponArenaWeapon`: two cvars that replace and extend `g_rockets` in order to be able to use `g_rockets` with every weapon other than rockets.
  * `elimination_selfdamage` is now `elimination_damage` and accepts four options: (Only Enemies), (Enemies and Self), (Enemies and Teammates) and (Enemies, Self and Teammates).
* Development mode with new cheat-protected cvars and debugging tools.
* Shuffle has been reworked by implementing the solution from Aftershock.
* New commands:
  * `weapbest`: Selects the best weapon.
  * `tossrune`: Tosses the rune that's been carried on. (Akin to TWCTF/TWCTF II) Needs `g_runes 2` in order to work.
  * `ui_writemappools`: If the arena files are loaded, this command dumps the gamelists so they can be used by g_autonextmap (should be used to generate new gamelists for new versions).
* Classic UI:
  * Updated to reflect the latest scoring changes.
  * `elimination_damage` can now be set in the UI for Elimination and eCTF matches.
  * `g_grapple` can now be set for all match types.
  * `g_harvesterFromBodies` can now be set for Harvester matches.
* Now it's possible to compile OAX on Mac (thanks EddieBrrrock!)
* AI enhancements (LOTS!) for holdable handling, grappling hook handling, Domination and Double Domination.
* Elimination/eCTF/LMS: If all humans have been killed in a round, bots will be eliminated one by one.
* Lots of fixes for Classic UI and backend of UI3.
* Consistent default score limits across all modules.
* Added frag message display with icons.
* Keyboard/Joystick input in MPUI/UI3.
* "Next round" Elimination countdown is also shown in Double Domination.
* The game no longer shuts down with _"Couldn't find a spawn point"_ when a spawnpoint is missing.
* New tool for mappers: `state_targetname` key for `func_door`, allows the creation of doors for Elimination-based modes that open during the active time of the game.
* New tool for mappers: Support for `.info` files. Some configuration examples can be found [here](https://github.com/NeonKnightOA/nk-oa-assets/blob/master/maps/DM/galmevish/am_galmevish3.info) and [here](https://github.com/NeonKnightOA/nk-oa-assets/blob/master/maps/DM/blitzkrieg/blitzkrieg3.info).
* New tool for mappers: `info_player_dom_red` and `info_player_dom_blue`, separate spawn points for Domination. These new entities are supported in [the gamepack](https://github.com/NeonKnightOA/oagamepack).
* Added ability to execute a script when the server is empty.
* Server now executes the file `gametype_GAMETYPENUMBER.cfg` (i.e. `gametype_0.cfg` for FFA or `gametype_3.cfg` for TDM) on gametype change.
* Server now executes `mapscripts/g_MAPNAME.cfg` and `mapscripts/g_MAPNAME_GAMETYPENUMBER.cfg` when changing map. It falls back to `mapscripts/g_default.cfg` and/or `mapscripts/g_default_GAMETYPENUMBER.cfg` (i.e. `g_default_0.cfg` for FFA and `g_default_3.cfg` for TDM) if any or both or the scripts are missing.
* The background is now stretched on widescreen displays.
* Tons of other bug fixes.

## oaxB53

**Release date:** TBA

* `g_grapple` can now be selected in the Classic Menu.
* `elimination_selfdamage` is now `elimination_damage`, can be set in the Classic UI, and is now a bitflag with the following values:
  * `elimination_damage 0` = Only Enemies.
  * `elimination_damage 1` = Enemies and self.
  * `elimination_damage 2` = Enemies and team.
  * `elimination_damage 3` = Enemies, self and team.
* Updated Classic UI in order to reflect the scoring changes. The Skirmish and Create Server menus will now display the following in the score field:
  * FFA, 1on1 and TDM: _"Frag Limit"_.
  * CTF, 1FCTF, Overload and DD: _"Capture Limit"_.
  * Harvester, Domination and Possession: _"Score Limit"_.
  * Elimination, eCTF and LMS: _"Round Limit"_.
* New helper functions `GametypeUsesFragLimit()` and `GametypeUsesCaptureLimit()`, return true if a gametype uses that scoring system.
* New helper function `BotTeamOwnsControlPoint()`, used for Double Domination and Domination, returns true if the bot's team owns a control point.
* Development mode: All debugging cvars now require `developer 1` before they can be used.
  * New cheat-protected development cvars: `cg_debugOrbit`, `cg_debugAccuracy`, `bot_debugChat`, `bot_debugLTG` and `bot_debugPaths`.
  * Restored `cg_debugDelag`.
  * Other cvars that require `developer 1` include `cg_debugAnim`, `cg_debugPosition`, `cg_debugEvents`, `cg_leiDebug`, `g_debugMove`, `g_debugDamage`, `g_debugAlloc`, `ui_debug` (MPUI/UI3) and `ui_leiDebug` (MPUI/UI3).
* New helper function: `G_IsANoPickupsMode()`, returns true if the mode doesn't allow pickups.
* New cvars that replace `g_rockets`: `g_weaponArena` and `g_weaponArenaWeapon`.
  * `g_weaponArena` enables the "Weapon Arena" mode, which is the old g_rockets, but with any other weapon. `g_weaponArena 2` adds the Grappling Hook to the inventory.
  * `g_weaponArenaWeapon` controls, via specific strings, which weapon it's spawned with its ammo boxes. The weapon cannot be picked up via menu, only (for now) via console. The full string list can be locate [here](https://github.com/OpenArena/gamecode/pull/171).
* Missionpack/UI3 backend refactors.
* "You Have Been Mined" message outright displays the counter instead of delaying it.
* Team status bar fix and cg_drawTeamOverlay fix by EddieBrrrock.
* New command: tossrune. Drops the currently held rune.
* New cvar value: g_runes &2: Enables the "tossrune" command in the server. (By default, g_runes 1, the command is disabled).
* Reimplementation of shuffle, ported from Aftershock.
* Unified score limits across all modules. The default limits are now as follows:
  * Free For All: 20 frags, 0 minutes.
  * Tournament: 0 frags, 15 minutes.
  * Single-Player: 10 frags, 0 minutes.
  * Team Deathmatch: 0 frags, 20 minutes.
  * Capture the Flag: 8 flags, 30 minutes.
  * One Flag CTF: 8 flags, 30 minutes.
  * Overload: 8 obelisk destructions, 30 minutes.
  * Harvester: 20 skulls, 30 minutes.
  * Elimination: 8 rounds won, 20 minutes.
  * CTF Elimination: 8 rounds won, 30 minutes.
  * Last Man Standing: 20 rounds won, 0 minutes.
  * Double Domination: 8 rounds won, 30 minutes.
  * Domination: 500 points, 30 minutes.
  * Possession: 120 flag-holding seconds (2 minutes), 20 minutes.
* Fix: Sound bug in cg_playerstate.c where flag taken messages played in non-flag-based gametypes. Also modified a condition in the fraglimit warnings.
* AI enhancement: Holdable item (Teleporter/Medkit/Kamikaze/Invulnerability) handling improvements.
* AI enhancement: Double Domination bot logic improved (team formations, voice orders, voice speech support)
* Regression fix: Obelisk bases are now drawn outside Harvester/Overload.
* Fix: Now it's possible to compile OAX on Mac (thanks EddieBrrrock!)
* Fix: Team Orders in Classic UI now shows proper team orders depending on the gametype.
* AI enhancement: Domination support vastly improved with team formations, basic logic and team orders.
* AI enhancement: Bots now properly decide if they should use the Grappling Hook or not.
* Fix: Classic menu now accounts for "Hide Private".
* Fix: Yourteamcapture.wav is now only played once when it's a personal capture instead of two before (thanks EddieBrrrock!)
* Fix: cg_weaponBarStyle with value outside of range now displays default weapon bar. Previously it would hide the weapon bar.
* Removal of #ifdefs which blocked legit AI logic that the main game could benefit from.
* Fix: Fragment of CG_DrawInformation in cg_info.c was not accounting for TDM.
* Fix: SendVictoryChallenge() in g_main.c wasn't counting TDM wins.
* Protect against overflow for the !gametype key
* Added state_targetname instead of targetname. Of course state_targetname does still overrule targetname in Elimination games but only in Elimination games.
* It is now possible to bind "Show accuracy" in the menu
* Move the version number in its own file and update the year
* Corrected "auther" to "author"
* Replaced "gnu_printf" with "printf" for clang compatiblity. Also fixed a problem with a boolean having 3 different values
* Added a WP_NUM_INVALID instead of a -1 for weapon numbers.
* Fixed a lot of formatting bugs.
* Added Possession as a gametype to vote for in the old UI
* Deleted some the remains of the old GO-button on the gametype vote menu
* Fixed some formatting in the vote menu.
* Do now allow missing spawnpoints if cheats is on.
* Lowered the default time needed to hold the flag in Possession from 5 minutes to 2 minutes.
* Removed SelectRandomFurthestSpawnPoint. It is no longer used. The spawn points became to predictive.
* Added logging events and Win challangex
* Added the option to move significantly faster under water using a dmflag. Mostly for demonstration. Could be fun in class based gametypes.
* Added a more aggressive form of g_awardpushing. Set it to 2 and the fragged player doesn't need to be airborne. Also blowing yourself up with a rocket gives a point to the last attacker too.
* Added cg_bob variable. If set to 0 it will disable cg_bob[up|roll|pitch].
* Added cg_kickScale variable.
* Added the neccecary paramter to missionpack to use the UI3 gui
* Added frag message display with icons.
* Added a Train_Blocked callback function.
  * Removed support for TRAIN_BLOCK_STOPS. It appeared to be broken beyond repair and completly undocumented. Trains now always crush. This might seem aggressive but there are no way logical way to turn them around and the only logical spawnflag was already in use.
* Changed func_train to work more like func_bob rather than a crusher.
* The short prox fuse functionalite no longer Applies to Team Deathmatch, Harvester, Elimination, Domination and Double Domination.
* Added UI3 font support for frag messages.
* Realigned the images for frag message display.
* Updated default g_voteGametypes value, adding "13/" (Possession gametype).
* Short prox mines fuse feat applied to obelisks in Harvester.
* Start killing bots in Elimination and CTF Elimination if all humans have been eliminated.
* If all humans are killed start killing the bots in Last Man Standing.
* Use early return instead of deep nesting in the CheckLMS and CheckElimination functions.
* Fix failure to build from source on GNU/kFreeBSD
* Add support for the GNU/Hurd architecture
* cg_muzzleflashStyle - alternative flash styles for player preference, or "aesthetic," or maybe just less flashing (flashes can also be disabled)
* Don't make impact explosions for weapons if particles are enabled.
* All custom Muzzles uses early return
* Extracted the Q1 muzzle flash from CG_AddPlayerWeapon to make it shorter
* Print what we are voting about in the console
* Select best weapon when out of ammo
* Introduced a CG_GetBestWeapon which returns the best weapon that the player has ammo for. Used after out-of-ammo
* Keybind labels now have the same color as other labels. Elements where you can rebind the key still have a lighter color.
* Added weapbest command. Selecting the best weapon. Also made BFG the ultimate weapon
* Simplified the spawning procedure by registrering all items. This also makes "map_restart" more reliable
* Ensure that base is always set to something non-zero
* Rename "g_persistantpowerups" to "g_runes". The variable and the actual command now has the same name.
* Move check for persistant powerup into Disabled item
* Checks that now account for LMS and Possession. Required for the SP to work well.
* More aggressive AI for Possession. Now will actively try to track and combat the flag carrier.
* Stopped setting com_hunkmegs in the UI. It was a workaround from before it was changed in the engine.
* Announce changes to pmove_* values. Not that they should be changed during gameplay but still.
* Added `g_grapple`. Gives Grappling Hook to all players. Also replaces `elimination_grapple`, which has been removed. **Default: 0.**
* Added `g_harvesterFromBodies`. Allows Harvester mode skulls to spawn where the frag happened instead of at the skull generator in the middle of the map. In case those skulls end up in a "nodrop" zone, they will respawn at the skull generator instead. **Default: 0.**
* Remove more NO_RESTART from CVARs
* Leilei changes for the MP UI.
* Add ui_randomIntToCvar to the missionpack UI. Add randomFloatToCvar command. Add ui_randomStringToCvar.
* Backport from ioq3: b21a59a "Fix negative glyph index in Team Arena text functions"
* Backport from ioq3: 5020361 "Fix off-by-one range checks in Team Arena UI". Also readded a commented function that may be useful in a future.
* Backport from ioq3: 07eb0f6 "Fix possible string buffer overflows in Team Arena UI"
* Backport from ioq3: f74479a "Fix handling too many characters or aliases in Team Arena UI"
* Backport from ioq3: 84e792f "Fix (unused) "orders" menu script logic"
* Backport from ioq3: "Make Window_Paint check if w is NULL before dereference"
* Backport from ioq3: 9c4b75c "Remove logically dead code from UI_GetServerStatusInfo"
* Backport from ioq3: c91fe0b "Fix handling too many teams in Team Arena UI"
* Backport from ioq3: b85935d "Fix glyph width calculation in Text_Paint_Limit"
* Backport from ioq3: 01bfb15 "UI_BuildFindPlayerList: avoid array underflow"
* Backport from ioq3: f1a133a "UI_BuildFindPlayerList: make a sizeof() more obviously correct"
* Big backport from ioq3: d875c1f "Improve keyboard/joystick input in Team Arena UI"
* Fix incorrect check for possession in cg_newdraw.c.
* Small fix for function G_InitBots where the minimum for FFA and Team gametypes ended up being the same.
* Backport from ioq3: f6f2710 "Make server browser default to Internet"
* Backport from ioq3: 2091a2e "Fix favorite servers player count message in Team Arena UI"
* Backport from ioq3: 0a19ae0 "Fix levelshot displayed in Team Arena server browser"
* Backport from ioq3: bd06754 "Fix hitch when opening Team Arena find friend menu"
* Backport from ioq3: db1198f "Add mouse wheel support to UI list boxes"
* Extended backport from ioq3: d58234a "Fix 'missing token' in parsers for animations.cfg"
* Backport from ioq3: 1048073 "Unify checks for missing COM_Parse() token"
* Backport from ioq3: 21eeaee "Make Team Arena UI aspect correct in widescreen"
* Backport from ioq3: a33a904 "Fix warning about using abs() with floats"
* Backport from ioq3: 8192f66 "Don't draw crosshair 0 in Team Arena setup menu"
* Backport from ioq3: dcf5707 "Don't reload arenas.txt/*.arena files in Team Arena UI"
* Backport from ioq3: a48dcdf "Fix crash when out of memory in Team Arena's String_Alloc"
* Backport from ioq3: 11b3bca "Update UI player animation handling to match CGame"
* Backport from ioq3: 471ea9e "Fix Team Arena server refresh time format"
* Backport from ioq3: dfb49e7 "Make setting r_stencilbits more consistent in Team Arena UI"
* Backport from ioq3: "Fix map list in Team Arena start server menu after entering SP menu"
* Backport from ioq3: 45f8512 "Fix clearing keys for control in Team Arena UI"
* Backport from ioq3: 10abac8 "Fix duplicate bots displayed in Team Arena ingame add bot menu"
* Make bot_minplayers work for Possession and LMS too.
* Backport of ioquake/ioq3#f7c3276
* Add new Double Domination Cvars Add g_ddCaptureTime which states how many seconds to hold the points to score Add g_ddRespawnDelay which states how many seconds after a capture until we can capture again.
* Complete application of ioq3: a3c2f77: "Fix Gauntlet barrel axis in UI"
* Backport from ioq3: af79d2c: "Fix an invalid null deref check in the slider code."
* Backport from ioq3: f9c202f: "Use correct type for thinktime"
* Backport from ioq3: 3273df1: "Add missing EV_USE_ITEM15 cases"
* Backport from ioq3: 424122c "Fix bot's teamleader name field being too short"
* Backport from ioq3: 5cf45c5: "Don't use dead view angles after stop following a dead player"
* Backport from ioq3: 4463af8: "When player stops following a player, keep view angles"
* Backport from ioq3: 70e3d61: "Fix cg.intermissionStarted only being enabled at first intermission"
* Backport from ioq3: d7f00e2: "Fix Team Arena tauntGauntlet command"
* Backport from ioq3: daa604a: "Fix parsing bots in arena info with trailing spaces"
* Backport from ioq3: 5fb49ac: "Clean up CG_DrawProxWarning design"
* Backport from ioq3: 1897afb: "Fix crosshair drawing not clearing color"
* Backport from ioq3: 7a39f4a: "Fix attacker icon being default image if attacker left"
* Backport from ioq3: 2292bf5: "Save bot accompany distance across map change or restart"
* Backport from ioq3: 08ac364: "Fix CG_WaterLevel() checks for waterlevel 2 and 3"
* Backport from ioq3: 386a00f: "Fix CGame CG_WaterLevel() comparisons"
* Backport from ioq3: 604b63f: "Fix cgs.teamVoteString buffer overflow in CG_ConfigStringModified"
* Backport from ioq3: f4aa39a: "Remove unused define CG_FONT_THRESHOLD"
* Backport from ioq3: 274fa89: "Fix typo of empty in ai_main.c"
* Backport from ioq3: 4474297: "Fix bot team order to kill last player it killed"
* Backport from ioq3: 3c8da8c: "[game/ai_main.c] Use floating-point fabs() for floating-point values"
* Backport from ioq3: 7297661: "Don't start a vote after vote passed for map change"
* Backport from ioq3: 4506ebd: "Fix joining team when starting local team play server"
* Backport from ioq3: 4227d97: "Make Team Arena win logic handle more game types/blue team"
* Backport from ioq3: 4006358: "Fix spawn/freed entity logic (specifically harvester skulls)"
* Backport from ioq3: 1066214: "Fix "brought in 1 skulls" Harvester message"
* Backport from ioq3: 082376e: "Enable tourney scoreboard in Team Arena"
* Backport from ioq3: c14cb70: "Draw disconnect icon over lagometer in Team Arena too"
* Backport from ioq3: c96acec: "Fix (unused) check for map restart in CG_TransitionSnapshot"
* Backport from ioq3: b511b8f: "Fix Coverity warning that endVelocity is uninitialized"
* Backport from ioq3: 91acf8a: "Don't build score info for bots, they don't parse it"
* Backport from ioq3: eeb28dc: "Fix score info being dropped by server"
* Backport from ioq3: 71512bb: "Show client's name in callvote clientkick vote display message"
* Backport from ioq3: 5b9302a: "Don't start game entity loops at index 1"
* Backport from ioq3: 74aa426: "Stop caching sv_maxclients in bot code."
* Backport from ioq3: 6e340f9: "Don't use uninitialized ps from BotAI_GetClientState."
* Backport from ioq3: a738cb9: "Fix overdraw in CG_DrawRect"
* Backport from ioq3: 730b917: "Fix comment in BotAIPredictObstacles"
* Backport from ioq3: 8a6c9d1: "Remove unneeded 'angles' variables/clearing in ai_dmq3.c"
* Backport from ioq3: c99281a: "Make bots stop attacking player after disconnect"
* Backport from ioq3: 8956ab4: "Fix notarget cheat"
* Backport from ioq3: f19efb7: "Fix Team Arena team base models not dropping to floor"
* Backport from ioq3: 520b100: "Make cg_teamChatsOnly only affect team gametypes"
* Backport from ioq3: c2ca5e7: "Check for unlimited time power up using INT_MAX"
* Backport from ioq3: 7b9ccd1: "Have spectator always be in first person"
* Backport from ioq3: 03336dd: "Allow spectators to use noclip cheat"
* Backport from ioq3: 007e250: "Split G_AddRandomBot into multiple functions"
* Backport from ioq3: 23a331c: "Make 'addbot random' command select a random bot info"
* Backport from ioq3: 5164969: "Fix random bot not looking for bots by funname"
* Backport from ioq3: d0d1fe1: "Fix bot_minplayers passing delay as team to addbot in non-team gametypes"
* Backport from ioq3: 7c601da: "Fix not adding random bot when all bot info are in use on team"
* Backport from ioq3: d8f2ff7: "Check delayed bot's team when counting bots for bot_minplayers"
* Backport from ioq3: 0999aff: "Fix duplicate (delayed) random bots being choosen"
* Backport from ioq3: cabc323: "Don't pick duplicate random bots until all bot types are added"
* Backport from ioq3: b984dd4: "Add range check for bot skill in addbot command"
* Backport from ioq3: c8db6c5: "Fix score bonus for defending the flag carrier in CTF"
* Backport from ioq3: 13831f9: "Restore not giving defense score bonus to flag carrier"
* Backport from ioq3: 5f2e4a0: "Add score bonus for defending the flag carrier in 1 Flag CTF"
* Backport from ioq3: 3971674: "Silence g_util.c warning about set but not read variable"
* Backport from ioq3: 0bce546: "Add spawnflags to QUAKED for trigger_multiple"
* Backport from ioq3: f0b74a2: "Check for all command separators in callTeamVote"
* Backport from ioq3: 9736e7f: "Fix compiling Cmd_CallTeamVote_f"
* Backport from ioq3: e793c0c: "Fix crash when pmove_msec is 0"
* Backport from ioq3: ad1d0e6: "Make bots use crusher on other q3tourney6 maps"
* Backport from ioq3: "Make bots only use q3tourney6 crusher to kill their enemy"
* Backport from ioq3: e152761: "Fix team chat box for spectators"
* Backport from ioq3: 51e9aa2: "Fix hit accuracy stats for lightning gun and shotgun kills"
* Backport from ioq3: 690c5a4: "Don't send team overlay info to bots"
* Backport from ioq3: 424e1ac: "Fix invalid model frame developer warnings in Team Arena"
* Backport from ioq3: c904f6d: "fix a few potential buffer overwrite in Game VM"
* Backport from ioq3: 0822772: "Fix timelimit causing an infinite map ending loop"
* Backport from ioq3: 0b6d97f: "Fix negative frag/capturelimit causing an infinite map end loop"
* Backport from ioq3: 51743bb: "Improvements for dedicated camera followers (team follow1/2)"
* Backport from ioq3: 60a3112: "Fix console offset while Team Arena voiceMenu is open"
* Backport from ioq3: 809a776: "Make testgun command without argument disable test gun model"
* Backport from ioq3: e4208cf: "Improve finding obelisk entitynum for bot AI"
* Backport from ioq3: 33a899d: "Fix predicting entity origin on rotating mover"
* Backport from ioq3: 09166ba: "Make Team Arena prevTeamMember command loop around player list"
* Bonus backport: "Bad counting in parsers for animation.cfg" From this issue: https://github.com/ioquake/ioq3/issues/396
* Unification of cvars for "developer" mode.
* Replacement of a "developer" call for the cleaner alternative.
* Unification of the Missionpack Checks into a single cvar.
* Fix for picking up flags in gametypes that don't make use of them.
* Many fixes to the Windows compiling scripts.
* Solution for only picking up cubes in harvester
* Stop running prediction in round based games before the round start
* Fix for item spawn in LMS (#95)
* Fix: format ‘%f’ expects argument of type ‘double’, but argument 2 has type ‘int’ (#109)
* Fix: Flag check only for flag-based gametypes. (#110)
* Fix: Capturelimit is no longer accounted for TDM wherever it's referenced.
* Fix for slicks surfaces. Now always snaps while touching slick surfaces.
* "Next round" Elimination countdown is also shown in Double Domination (#105)
* New fields for .info files

## oaxB52

**Release date:** [August 15, 2016](https://openarena.ws/board/index.php?topic=1908.msg54109#msg54109)

* Added gametype "Possession". Players must hold a flag (the white flag from 1FCTF) for X seconds in order to win. Or be the player with the highest amount of carrying time.
* Added support for info files
* Added the option of having doors that are only open during the active part of Elimination and CTF Elimination
* The game no longer shuts down with "Couldn't find a spawn point" when a spawnpoint is missing.
* Added ability to execute a script when the server is empty.
* Code now compiles with gcc-4.8

## oaxB51

**Release date:** [May 14, 2012](https://openarena.ws/board/index.php?topic=1908.msg44498#msg44498)

* All changes from 0.8.8
* Added command "ui_writemappools". If the arena files are loaded this command will dump the gamelists so they can be used by g_autonextmap (should be used to generate new gamelists for new versions)
* Server will now execute "gametype_GAMETYPENUMBER.cfg" on gametype change
* Server executes "mapscripts/g_MAPNAME.cfg" and "mapscripts/g_MAPNAME_GAMETYPENUMBER.cfg" then changing map. It will fallback to "mapscripts/g_default.cfg" and/or "mapscripts/g_default_GAMETYPENUMBER.cfg" if any or both or the scripts are missing.
* Moved STAT_PERSISTANT_POWERUP to last so that STAT_* are numbered as in 0.8.1
* Weapon accuracy are now written to the logfile. The stats are written at the end of the game or then a player leaves
* Teams can now have seperate spawn points in Domination
* The background are now streched on widescreen displays

## oaxB50

**Release date:** [September 14, 2011](https://openarena.ws/board/index.php?topic=1908.msg39895#msg39895)

* Removed some problematic code that caused some problems in B49
* Added new dmflags:
** 64 = instant weapon change (weapon change is instant, weapons still need to reload)
** 128 = no bunny (disables Q2-style jumping and uses realism jumping instead, breaks most maps)
** 256 = no invisible skin (invisible players are completly invisible)
** 512 = allows non majority vote, was default in 0.8.5

## oaxB49

**Release date:** [August 23, 2011](https://openarena.ws/board/index.php?topic=1908.msg39625#msg39625)

* Challenges are now logged
* Last man standing event are now logged
* Fixed ammobar for 3 different weaponbar styles (grapple)
* Fromhell's changes ( http://openarena.ws/board/index.php?topic=1933.msg39483#msg39483 )
* You can now see how much health the obelisk have left in Overload
* Fixed a crash when the obelisk was damaged while using native cgame. (Thanks to jackmcbarn for the patch)
* Autoswitch weapon can now be set toonly change to better or new weapons. (The default is set in the binaries)
* Votekick is now canceled if the player in question leaves.

## oaxB48

**Release date:** [February 24, 2011](https://openarena.ws/board/index.php?topic=1908.msg37620#msg37620)

* Remove the "Click to respawn" message if you are spectator or if the gametype is Elimination, CTF Elimination or LMS 
* sessionTeam is now set for bots. this should stop them from doing stupid things
* Added a "!gametype" flag parallel to "gametype", so map makers can disable items in given gametypes without affecting all furture gametypes 
* Changes to logging to make it possible to get more information. This includes log messages for most gametype events except LMS.

## oaxB47

**Release date:** [September 17, 2010](https://openarena.ws/board/index.php?topic=1908.msg35098#msg35098)

* Prevent team changing and disconnect from changing team score in Team Deathmatch 
* Don't lake vampire resurrect dead players. 
* Sets sv_dorestart in case players wants to change variables in game. 
* Now only loads the important things if g_elimination or g_instangib is set (no pickups)
* Changed default customvotefile to customvote.cfg
* Added the possibility for spawning in waves. The cvar g_respawntime says how often players should spawn.
* Added a CVAR g_voteBan X - bans a player for X minutes using the KK-admin system
* Handicap is reset to 100 once returned to the main menu
* Added g_gravityModifier (default 1.0) that modifies g_gravity relatively to the value set by the map 

## oaxB46

**Release date:** [July 7, 2010](https://openarena.ws/board/index.php?topic=1908.msg33556#msg33556)

* Added note to handicap option in menu
* Added g_maxvotes to allow the server admin to pick a value.
* Added g_spawnprotect - milliseconds of invulnerability after spawning. 
* Added replace_* cvars like disable_*. Can be used like "set replace_weapon_shotgun weapon_bfg" for replacing shutgun by BFG.
* Added Evil.HD.unOA's accuracy display although it cannot be bound in the menu.
* PlayerStore now records time and accuracy
* You can now use black charecters in your name provided that there is at least one non-black alphanumeric charecter
* Now uses FFA spawnpoints in domination 
* Sort by human players in the menu is now remembered and recovered upon return 
* Added option for changing some files: g_votemapsfile and g_votecustomfile
* Some code cleanup

## oaxB45

**Release date:** [May 11, 2010](https://openarena.ws/board/index.php?topic=1908.msg31793#msg31793)

* Fixed TEAMMATE kill message bug.
* Added g_autonextmap that automatically changes maps on game end
* Added g_autonextmap to the start server menu. Added to the map select place as the gui is rather full at the moment.
* motd.cfg can now be configured to look for a different file with g_motdfile
* elimination_roundtime 0 does no longer stop matches imidiently.
* stopped some broadcasts during intermission. Hopefully reduced the risk of a netchan error.
* Workaround for Kamikaze bug
* No longer asks the user before quitting from the menu

## oaxB44

**Release date:** [October 24, 2009](https://openarena.ws/board/index.php?topic=1908.msg27750#msg27750)

* cg_crosshairHealth toggleable in the menu
* Fixed crash during intermission in Elimination and CTF Elimination if a human was dead while the capturelimit was reached and possible other situations.

## oaxB43

**Release date:** [October 23, 2009](https://openarena.ws/board/index.php?topic=1908.msg27739#msg27739)

* Colored crosshair support by chaing RGB values
* BG_CanAlloc introduced, can predict out-of-memory errors and handle them - adding too many bots no longer crashes the game
* Some missing blueish menu items
* Added g_catchup, that makes cheap frags a little more expensive if activated
* A few notices added to UI in different places
* Server source is now: Internet, Internet(2), Internet(3) ... instead of Internet1, Internet2, Internet3 ...
* Dead players are sorted last in Elimination/CTF Elimination
* Some imports from ioquake3: Ratio selection, new ClientCleanName and new bglib

## oaxB42

**Release date:** [September 28, 2009](https://openarena.ws/board/index.php?topic=1908.msg27589#msg27589)

* g_persistantpowerups is now renamed to g_runes
* vertex light is no longer forced on in single player
* Now runs the correct logic then a single player map ends
* Blueish menu
* Added clientkick_game, same as clientkick but located in the game code, so it can be improved without touching the engine
* Double Domination counter will no longer count forever during warmup (however points will not respawn)

## oaxB41

**Release date:** [June 21, 2009](https://openarena.ws/board/index.php?topic=1908.msg25566#msg25566)

* Persistent powerups are now toggleable (default off in baseoa, default on in missionpack) - g_persistantpowerups
* Penalty points in 1 on 1 are awarded to the opponent rather than subtracted.
* Negative scores in FFA are no longer possible. 
* Changed default ammo count in Elimination
* Exclude bots from duplicate names check
* Missionpack now supports ui_humansonly
* Fixed wrong gamenames in Missionpack server browser
* Removed Mplayer as an option in Missionpack server browser (still no support for different master servers through)
* Removed remaining references to punkbuster
* Kemikazie and Invincibility are now availeble (beware that Invincibility is missing models). Availble in bases3 only (as far as I know)
* Fixed infinity loop bug in AAS code caused by the gametype flag.
* Generally improved the bot's Area Awareness System. 
* Added basic 'fariness' system. The CVAR is 'fairflags'. Default value 7 (1+2+4). 1 verifies some basic values. 2 verifies some extended graphics settings. 4 disables vertex light.  
* Added CVAR cg_autovertex. Automatically enables vertex light if allowed. Off by default. Gets enabled if you enable vertex light through the menu.
Note: The fairness system might result in a vid_restart if it detects a bad r_ cvar after the render has been initialized

## oaxB40

**Release date:** [June 15, 2009](https://openarena.ws/board/index.php?topic=1908.msg25361#msg25361)

* Fixed CleanStr
* Added color 8 (menu color)
* BG_Alloc/BG_Free added. replaces G_Alloc
* Mines will self destruct very quickly if placed near the flag
* Kill sprees/death sprees and multikills added
* Some fixes backported from ioquake3
* Servers with no humans on are considered empty if "Only humans" is enabled
* Added elimination_lockspectator: 0=no lock, 1=cannot follow enemy, 2=must follow teammate (For Elimination and CTF Elimination only)
* Different crosshairs per weapon
* Crosshair pulse can be disabled
* Skulls are now visible on Skull carrier in Harvester
* Bots are now a little better at understanding prox mines
* Added deathShader to scoreboard in missionpack (for Elimination and CTF Elimination only, missionpack only)
* Breath and Dust effect backported from missionpack
* Added elimflags and voteflags than are now used to tranfer some booleans to save network traffic
* Vote system is more secure
* Vote can now pass even if majority is not reached then time runs out. It requires twice as many yes votes as no votes through or high percentage of yes votes.
* log2stdout cvar removed
* Teams can now be shuffled 
* pmove_fixed on be default
* pmove_msec defaults to 11 (controversial - I know)
* Non-rcon based admin system (ban, kick, lock teams, cancel votes etc.)
* Shuffle teams function added. Can be called by an admin "shuffle" or by "callvote shuffle"
* Parts of code has been cleaned a little
* Duplicate GUID check
* Added G_GlobalSound
* Server command handling now modularized ( ClientCommand & ConsoleCommand )

## oaxB39

**Release date:** [May 8, 2009](https://openarena.ws/board/index.php?topic=1908.msg24600#msg24600)

* Fixed double throw bug when a client disconnected
* No killed message then a client leaves (still counts as a suicide)
* Added 91 Hz fixed framerate to startserver (pmove_msec 11)
* More ingame information about pmove

## oaxB38

**Release date:** [May 2, 2009](https://openarena.ws/board/index.php?topic=1908.msg24490#msg24490)

* g_humanplayers always 0 during intermission
* bots now reads team status from game rather than ai config string. Hopefully stopping bots from shoting at there own team during certain changes.
* challenges.cfg renamed to challenges.dat, to indicate that it is not an text file
* Removed compiler warning in TeamCvarSet
* pmoved_fixed now gets disabled if pmove_float is selected in the UI

## oaxB37

**Release date:** [April 26, 2009](https://openarena.ws/board/index.php?topic=1908.msg24407#msg24407)

* g_humanplayers might make fewer mistakes now
* Gametype filter now also works for One Flag, Obelisk, Harvester and Domination
* First Connect screen reminds new players of some important settings
* bot_nochat >= 2 will now stop all bot chat
* Added "Optimize for LAN" to "start server" 
* Added "Physics" to "start server"
* Added "Oneway capture" to "start server" for the CTF Elimination gametype
* Added comments to pure and All Rockets in "start server"
* Fixed LMS mode bug in "start server"

## oaxB36

**Release date:** [April 25, 2009](https://openarena.ws/board/index.php?topic=1908.msg24384#msg24384)

* Callvote map menu now responds the first time it is activated
* pmove_float added. Makes the physics framerate independent but cost up to 8 times as much network traffic (worst case, normally only a few percent). Mostly for LAN gaming. Note that you must reduce g_gravity to ~756 to get 125 fps gravity.
* Fixed vote exploit (some bugs still remain, needs rewrite)
* Fixed a bug that allowed a client to call more than 3 votes in a single game.
* cg_oldRail now defaults to 0
* Added restore system based on guid. You no longer lose scores when changing team, spectating or leave+rejoin
* Chainging team or leaving the game now counts as a suicide to prevent misuse of the restore system. In Team Deathmatch the team will be compensated so they don't loose points by loosing a player to the other team.
* You cannot ban localhost anymore (the system needs rewrite)
* g_elimination cvar added (like g_instantgib and g_rockets, but full elimination arsenal, health and damage rules)

## oaxB35

**Release date:** [March 13, 2009](https://openarena.ws/board/index.php?topic=1908.msg23231#msg23231)

* You can now command bots in One Flag, Harvester, Obelisk and Domination.
* Last Man Standing will no longer count connecting players as survivors.
* Quad will no longer spawn if q_quadfactor <= 1.0
* Added CVAR 'g_logfile2stdout'. If set to 1 on a dedicated server a copy of the log will also be sent to standard out. Removes debug information from stdout. Must be considered experimental. Garbage data might occour. 
* Changed the way the game picks the spawn point to prevent maps with few spawnpoints that are marked with nobot or nohuman from hanging the game.
* Midair suicide while g_awardpushing is enabled will result in a point to the attacker.
* cl_guid is now written to the log file for all human players joining

## oaxB34

**Release date:** [February 8, 2009](https://openarena.ws/board/index.php?topic=1908.msg22589#msg22589)

* Can now handle 20*1024 charecters in the *.arena file list. Significantly increases the number of maps supported.
* No maps are ignored if number of single player maps are not divideble by 4.
* CVAR_SYSTEMINFO is now set in cgame to allow game to override them
* Powerups recorded in stats
* Support for custom votes with a votecustom.cfg (I shall post an example)

## oaxB33

**Release date:** [February 5, 2009](https://openarena.ws/board/index.php?topic=1908.msg22465#msg22465)

* Call Vote Map implemented
* Fixed infinity loop bug caused by bot_roams with gametype flags
* Gametype flag "ctfelimination" removed, replaced by "ctf"
* Password promt when joining servers
* g_humanplayers is no longer a SERVERINFO CVAR
* g_lms_mode is now a SERVERINFO CVAR
* Changed tab order in some vote menues
* If gametype are changed by a vote then the map will now automatically restart and reassign nextmap.
* You no longer get gauntlet award against bots in stats
* Stats no longer recorded during warmup
* Hook are no longer selected by default on spawn
* Flags are used for domination points instead of using Point A from DD

## oaxB32

**Release date:** [January 26, 2009](https://openarena.ws/board/index.php?topic=1908.msg22085#msg22085)

* Grapple can be bound in option menu
* Grapple is now placed before gauntlet
* Grapple will never be selected then autoswitching from a weapon because you ran out of ammo
* You can get "No ammo" even if you have the grapple
* Client side cvar "cg_cyclegrapple" to tell if Grapple should be part of the weapon cycle. Default is 1 since there are no default key.
* Allowed maps for voting is now read from votemaps.cfg if it exists.
* Call Vote is grayed if disabled on the server
* Call Vote Kick implemented (uses clientkick to kick players)
* Server side cvar "elimination_grapple" added. Allows grapple in elimination gametypes.

## oaxB31

**Release date:** [January 24, 2009](https://openarena.ws/board/index.php?topic=1908.msg22021#msg22021)

* Vote system now a lot more robust. Especially the kick functions are now more likely to kick the correct player
* Vote menu for calling votes - does not yet support Kick and Map functions.
* Clamp on cg_errorDecay as suggested by jessicaRA
* New ui_demo2.c by jessicaRA
* From ioquake3 svn-1492: fix overflow in CG_ParseTeamInfo
* From ioquake3 svn-1494: fix potential segfault (found by DerSaidin in xreal)
* From ioquake3 svn-1493: security fix: prevent command injection via callvote
* Mouse wheel works in mods menu

## oaxB30

* Bots now attack the Obelisk in Overload
* Bots should now acknowledge the gametype property of items (not tested)
* If g_awardPushing is enabled players will get frags for killing with a mover (be aware that this makes it possible to be noted for a teamkill even if friendly fire is off)
Changed message for teamkill from "you fragged <TARGET>" to "you fragged ^1TEAMMATE^7 <TARGET>"
* Telefrags and crushing are now in Stats
* Team only voice chat (cg_voipTeamOnly) enabled by default. While this function is enabled any value the player assign to "cl_voipSendTarget" will be overwritten when a player joins or leaves the team.
* Crosshairs will now change to white if a colored crosshair is choosen and for some reason the server does not support it.

## oaxB29

* Chaingun kill message is no longer the same as the machinegun kill message
* bot_minplayers works better but still not perfect.
* Starting challenge system programming. Currently there are no challenges, it only counts kills and awards against humans. Therefore it is currently called 'statistics'

## oaxB28

* Chaingun prediction effect while unlagged
* Some model/name changes in missionpack

## oaxB27

* Harvester fixed
* You can mine yourself now
* Number of skulls is printed in Harvester
* Flag status in One Flag Capture is on the screen
* oneflag, overload, harvester and dom are now selecteble through the UI

## oaxB26

* Load flagbase in baseoa in CTF games to stop the null from appearing at the flags.
* VMs believes they are baseoa
* Fixed LMS not accepting notteam anymore.
* follow1 and follow2 no longer causes an infinite loop when cycling. 
* Includes spectators then counting humans
* Big fat text in the middle of the screen then a team is dominating in DD

## oaxB25

* Print protocol number of different from 70 (0.8.x default)

## oaxB24

* Larger buffer to hold all colors in server names (to prevent a new sprintf overflow like 0.7.6)
* No more color overwrite in server browser
* cvar vote strings are no longer SYSTEMINFO
* Removed some CVARs only relevant for Missionpack in baseoa
* Removed g_lightningdamage cvar

## oaxB23

* Fixed missing Point B (White) in Double Domination
* Added function to filter bots in server browser (requires new engine)
* Added function to filter private servers in the browser (requires new engine)
* Sends names of allowed votes if a wrong command is entered
* Removes mines in Elimination, CTF Elimination and LMS between rounds
* Now shows if it is a IPv4 or IPv6 server in games browser
* Stereo vision
* Removed credits (there was none anyway)
* Removed cinematics (the list was almost empty)

## oax B22

**Release date:** [June 22, 2008](https://openarena.ws/board/index.php?topic=1908.0)

* Renamed the mod from Elimination to OpenArena eXpanded (OAX).
* Team spawn points in Double Domination (optional).
* Log information about Awards, captures, rounds etc. (Not yet in harvester, Obelisk and Last Man Standing).
* Removed "Dedicated" option from "Start Server". It caused a nasty freeze when used.
* Removed unimplemented "g_voteStrictGametypes".

## Elimination B21

* Based on 0.7.7
* Most changes from B20 are not included!
* Uses optimized collision detection again (like 0.7.7 but unlike B19 and B20)
* Missionpack weapons usable in baseoa
* One Flag CTF, Harvester and Overload available in baseoa (although some GUI elements are missing, not to mention all models)
* Bots should be slightly more intelligent in standard Domination
* The new gametypes now accept elimination, ctfelimination, lms, dd and dom to be used in "gametype" parameter in map entities. However this breaks many 0.7.7 maps.
* New way of showing domination point status in Double Domination

## Elimination B20

* Some of beast's changes to server list
* More player spots in GUI, better bot management (but not beast's preferred bots yet)
* Some cleanup to collision detection
* Placeholder icons for Double Domination points

## Elimination B19

* Instantgib and All rockets disabled during single player startup
* Clears nextmap after single player
* Beginning vote restriction
* Smooth clients reverted to pre-unlagged
* Collision detection reverted to 0.7.0
* Logging fixed
* Fixed com_sprintf overflows in the server list

## Elimination B18

* baseoa 0.7.6
* Unlagged disabled by default
* Different standard bots (safe)

## Elimination B17

* "You have taken/are tied for/lost the lead" sounds playing in Last Man Standing
* Fixed hanging bug in Last Man Standing
* Last Man Standing scoring mode selecteble from the GUI
* Removed GUI references to PunkBuster and CD key
* Added "Always show weapon bar" to the Game Options
* Added "Instant gib" and "All rockets" to the Start Server screen
* Removed "Friendly fire" option in Elimination Games as it is ignored anyway (elimination_selfdamage = 2 should be used instead)
* Double Domination default UI value can now be controlled using CVARS.

## Elimination B16

* Merged Niel Toronto's Unlagged code into elimination (except for lag simulation)
* UI enhancements to the game server screen (such as default capturelimit in Elimination to 8)
* openAL can be enabled and disabled from the menu
* cg_alwaysWeaponBar allows people to turn the weapon bar permanently on

## Elimination B15

* Added "Auto join" to the classic join game menu. This automatically joins the weakest team (following the auto balance rule). (This was suggested by kick52)
* Sending people to there death with knockback damage now counts as a kill.
* Bots accepts orders in Double Domination again.

## Elimination B14

* Matchwarmup now useable in Elimination/CTF Elimination/LMS/DD/Standard Domination
* Weapons enabled in warmup and while waiting for other players.

## Elimination B13

* Fixed player bar color when spectating in Elimination/CTF Elimination
* Lives are now showed in LMS instead of "*DEAD*"
* No shaking camera then hit in LMS, Elimination, CTF Elimination. It is mostly noticed in overtime in LMS.
* Prevent elimination warmup CVARs from taking bad values.

## Elimination B12

* The other rocket arena type... by dmn_clown (taken from OA svn)
* Some other things from SVN (including a WTF bug)
* Standard Domination
* One test map

## Elimination B11

* Used some of next ghosts patch file to fix some bugs
* Fixed the bugs caused by fixing the above bugs
* No defend bonus for the attacking team in Oneway CTF
* Some chat commands + chat analysing + bot leadership in Double Domination. WARNING: I have not worked on this on my own computer, so I haven't tested on missionpack.
* Oneway CTF bot strategy might not be the best.

## Elimination B10

* Build on SVN 625 + instantgib missionpackbug
* Added <tt>elimination_ctf_oneway</tt>, if set to '1' only one team can attack. Bots don't really understand this yet but I think I can get it. In the scene jargon, this mode would be called "CaptureStrike".
* Fixed flags respawn bug introduced in B9.
* Bots now have a basic understanding of Double Domination and will try to capture the points, however they will not cooperate
* Domination points from SVN is included for people who test on 0.7.x.

## Elimination B09

* Reenabled spectator following when dead, now without score bug
* Spectators can now use teleporters
* Spectators can teleport though doors
* Added a message to the client informing of the possibility to leave spectator mode by pressing USE_ITEM
* Added cvars for all weapons but gauntlet
* Added Chaingun, Nailgun and Prox Launcher in missionpack
* Maybe solved the bot_min_players+no humans+forcejoin bug
* Mines now work in LMS
* Some code fixes incl possible pointer problems
* You cannot lose points by killing yourself/team mates in Elimination/CTF Elimination warmup any more
* Fixed an unknown problem (I could just see the code was wrong)

## Elimination B08

* Removed white flags in missionpack/ctfelimination
* Added 3 new modes to LMS, use cvar g_lms_mode [0-3].
* Client prints LMS mode in right corner if scoreboard showing
* No longer does anything in intermission in any gametype. Hopefully this will solve a problem with no-humans play.
* New UI from SVN (LMS now a FFA gametype)

## Elimination B07

* Possible fixed a bug in Missionpack if it is compiled as a shared module (might not work but its worth a try and makes the code more effective)
* Added personal scores to Double Domination
* Removed Whiteflags and persistant powerups from Double Domination (The removal of the powerups might be discussed)
* Can no longer score in Double Domination after time has run out
* Some UI things from SVN (I don't really know what)
* The Domination Point B is now a BFG ammo if not taken (not really a change)
* Other undocumented changes

## Elimination B06

* Fixed sounds in CTF Elimination.
* Bots no longer select a team leader in LMS.
* Fixed a serious Missionpack bug (Signal 11).
* Changed respawning times in the new gametypes.
* Added a version of Double Domination as gametype 11. No bot support yet!

## Elimination B05

* Fixed a Railgun pickup bug in Instantgib/CTF Elimination.
* Added gametype: Last Man Standing (LMS).
* Added CTF Elimination and LMS to the create server screen.
* New arenas.txt file, adds the existing 0.70 maps to the gametypes.

## Elimination B04

* New gametype: CTF Elimination
* New gametype template: Last Man Standing
* Elimination teams change sides after each round, allowing unbalanced maps.
* Taunts after elimination.
* Added variables: 
** int g_ffa_gt //if this is 1, the game is FFA even if it has a high gametype number
** int cgs.ffa_gt //same, but in cgame. Remember to change CG_ParseServerinfo in cg_servercmds.c to set this from gametype
* The UI was left unchanged since Beta 2, so you can't see the new gametypes there with this beta.

## Elimination B03

* g_instantgib is now a LATCH CVAR, map_restart required
* Everything is now loaded in instantgib, so nothing bad happens if it is suddenly disabled
* Added CVAR g_regen
* Added instantgib with gauntlet (from OA svn)

## Elimination B02

* Instagib can be enabled in all gamemodes
* Vampire can be enabled in all gamemodes
* Colorfull timer
* No free point to first team
* Free spectator when dead
* Elimination is now gametype 8
* Default CVAR values changed
* Correct update of the scores in the corner

## Elimination B01

**Initial release date:** [June 30, 2007](https://openarena.ws/board/index.php?topic=831.0)

_"I have played around with the almighty source code.<br />.<br />This mod adds a gametype "Elimination". It is round based. You have all weapons (bfg is disabled by default) and full health and must eliminate the other team. It is like elimination from SoF2, combined with the UT version of Rocket Arena (no self damage, only two teams, warmup)..<br />.<br />I believe most bugs are gone by now."_

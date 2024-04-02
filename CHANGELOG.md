# OpenArena eXpanded Changelog resume

## Important changes since OA 0.8.8

### Laconic version

**Note:** An ampersand ("&") preceding a value means that the cvar is a bitflag, and you must add said value to the total before using its effect (i.e. if dmflags is 1096 and you want to add the new underwater speed -`dmflags &4096`-, then dmflags should be 5192 (1096+4096).

* New gamemode: Possession:
  * Players must hold a flag (the white flag from 1FCTF) for X seconds in order to win. Or be the player with the highest amount of carrying time.
  * Uses `fraglimit` (holding time in seconds, i.e. `fraglimit 120` for 2 minutes, the default time) and `timelimit`.
  * It can be voted after matches as `/13`. (Credit: @The-Gig)
* New cvars:
  * `cg_bob`: Controls how bobbing behaves in-game. It accepts the following values: (Credits: @ldrone (options 0/1) and @leilei- (options 2-6))
    * `cg_bob 0`: Disables it.
    * `cg_bob 1`: Pitch/Roll/Up Bob.
    * `cg_bob 2`: Pitch/Up-only Bob.
    * `cg_bob 3`: Roll/Up-only Bob.
    * `cg_bob 4`: Up-only Bob.
    * `cg_bob 6`: "Fake '99" a.k.a. "Sweeny Bob". (We cannot write the actual name because [writing around trademarks](https://tvtropes.org/Main/WritingAroundTrademarks))
  * `cg_kickScale`: Controls how much the screen shakes when hit. (Credit: @ldrone)
  * `cg_muzzleFlashStyle`: Controls how the muzzle flash is rendered. (Credit: @leilei-)
  * `cg_obituaryOutput`: Controls how and where the death messages are rendered. (Credit: @ldrone)
  * `g_classicMode`. It replaces `g_runes` by extending it. In addition to removing the Runes, it also replaces the Nailgun with the Shotgun, the Chaingun with the Machinegun ammo, the Prox Launcher with the Grenade Launcher, the Kamikaze holdable with the Personal Teleporter and the Invulnerability holdable with the Medkit. In Round-based gamemodes, it prevents the Nailgun, Chaingun and Prox Launcher from being added to the player's starting inventory.
  * `g_ddCaptureTime` and `g_ddRespawnDelay` for Double Domination. Both control the amount of holding time required to score, and the time between the scoring and start of a new round.
  * `g_grapple`. It replaces `elimination_grapple` by extending it so it works with all gamemodes.
  * `g_harvesterFromBodies` for Harvester. If enabled, skulls spawn from bodies rather than a central skull generator. Allows Harvester matches to take place in maps without a skull generator. However, the placement of a skull generator is still recommended for those cases where the players commit suicide or are killed via level hazard.
  * `g_weaponArena` and `g_weaponArenaWeapon`. The former replaces `g_rockets` by extending it so it can be used with any weapon, not just the Rocket Launcher. The latter controls which weapon is spawned by `g_weaponArena`.
  * `missionpackChecks`. Enables/disables missionpack checkers in both Classic UI and UI3. It's enabled by default for the latter, and disabled for the former.
* New commands:
  * `+acc`. Reveals your weapon accuracy after matches.
  * `dropRune`. Frees the rune for another teammate to use it.
  * `ui_writemappools`. Dumps the gamelists so they can be used by `g_autonextmap`, provided the .arena files (and arenas.txt) are loaded first.
  * `weapbest`. Selects the "best" weapon out of all those that the player carry which have ammo. The cvar controlling this priority is `cg_weaponOrder`; unlike the command, the cvar still needs to be set via console.
* New _dmflag_: `dmflag &4096`, allows faster underwater movement a la Q2.
* `bot_minplayers` now work for LMS and Possession.
* `capturelimit` no longer affects Team Deathmatch.
* `cg_teamChatsOnly` now only affects team-based gamemodes.
* `cg_weaponBarStyle` now returns the default weapon bar if its value falls outside its limits.
* `developer` mode has been extended with several cvars now requiring the mode to be enabled. If a cvar has "debug" in its name, it requires this mode.
* `elimination_selfDamage` now takes four values: 0 (no damage to self or teammates), 1 (only damage to self), 2 (only damage to teammates a.k.a. Friendly Fire) and 3 (self and teammate damage).
* New option for `g_awardPushing`: `g_awardPushing 2`. [If a player commits suicide, the last player who attacked them scores a point.](https://openarena.ws/board/index.php?topic=5289.msg54334#msg54334)
* Spectators can now use the `noclip` cheat code.
* `shuffle` command has been removed and reimplemented, with the solution present in Aftershock being used here.
* Classic UI:
  * Skirmish/Create Server/Game Options/H.U.D.: All options now have small helper texts. (Status bar) Some options will display what each selected option does rather than a general help text.
  * Skirmish/Create Server:
    * Server options are now displayed in the left column, while Gametype options are displayed at the right.
    * New options:
      * `g_grapple` (_"Grappling Hook"_).
      * "Weapon Ruleset", allows picking up between the _"All Weapons (Standard)"_, _"Instantgib"_ (`g_instantgib 1`), _"Single Weapon Arena"_ (`g_weaponArena 1`), _"Classic Arena"_ (`g_classicMode 1`) and _"All Weapons (Elimination)"_ (`g_elimination 1`) rulesets for non-Round-based gamemodes, and between _"All Weapons (Elimination)"_, _"Instantgib"_, _"Single Weapon Arena"_ and _"Classic Arena"_ rulesets for Round-based gamemodes.
      * `g_weaponArenaWeapon` (_"SWA Mode Weapon"_).
      * `g_awardPushing` (_"Award Pushing Rule"_).
      * `g_harvesterFromBodies` (_"Skulls From Bodies"_) for Harvester.
      * `g_obeliskRespawnRate` (_"Time Between Rounds"_) for Overload.
      * `elimination_roundTime` (_"Round Time Limit"_) for Elimination, eCTF and LMS.
      * `elimination_selfDamage` (_"Damage To"_) for Elimination and eCTF.
      * `g_lms_lives` (_"Lives Per Player"_) for LMS.
    * Transplanted from Network: `sv_allowDownload` (split from "Autodownload" as _"Clients Can Download"_).
    * "Physics" is now named _"Physics Ruleset"_.
    * "One-Way Mode" for CTF Elimination (`elimination_ctf_oneway`) is now named _"Attack vs. Defense"_.
  * New H.U.D. menu, accessible from Setup, controls the elements that are displayed in, well, the HUD, including the crosshair and weapon bar. Among the new options added and not mentioned later in this changelog:
    * Transplanted from Video: `cg_drawFPS` (_"Draw FPS"_).
    * Transplanted from Game Options: `cg_drawCrosshair` (_"Crosshair Style"_), `cg_crosshairHealth` (_"Crosshair Shows Health"_), `cg_crosshairColorRed` (_"Crosshair Color (Red)"_), `cg_crosshairColorGreen` (_"Crosshair Color (Green)"_), `cg_crosshairColorBlue` (_"Crosshair Color (Blue)"_), `cg_alwaysWeaponBar` (_"Always Show Weapon Bar"_), `cg_drawCrosshairNames` (_"Show Crosshair Target Name"_ f.k.a. "Identify Target") and `cg_drawTeamOverlay` (_"Show Team Overlay"_).
    * New options: `cg_crosshairPulse` (_"Crosshair Pickup Pulse"_), `cg_draw3DIcons` (_"Weapon Bar Icon Rendering"_ in _"2D"_ or _"3D"_), `cg_weaponBarStyle` (_"Weapon Bar Style"_), `cg_obituaryOutput` (_"Death Messages"_), `cg_drawTimer` (_"Show Timer"_), `cg_drawStatus` (_"Show Player Status"_), `cg_drawAmmoWarning` (_"Show Low Ammo Warning"_), `cg_drawAttacker` (_"Show Last Attacker"_), `cg_drawSpeed` (_"Show Movement Speed"_) and `cg_drawRewards` (_"Show Medal(s) Earned"_).
  * Game Options:
    * Options transplanted to many menus as mentioned above and below.
    * New options: `cg_drawGun` (_"Weapon Hand"_: _"Hidden"_, _"Left"_, _"Right"_, _"Center"_), `cg_muzzleFlashStyle` (_"Muzzle Flash Style"_), `com_blood` (_"Show Blood"_), `cg_gibs` (_"Show Gibs"_), `cg_bob` (_"View Bobbing"_), `cg_bobModel` (_"Weapon Bobbing"_), `cg_railTrailTime` (_"Railgun Slug Trail Time"_) and `cg_kickScale` (_"Screen Shaking Rate"_).
    * Renamed "Simple Items" to _"In-Game Pickup Rendering"_ (with 2D and 3D options).
  * Network:
    * Transplanted from Game Options: `cl_allowDownload` (_"Download From Servers"_, f.k.a. "Autodownload") and `cg_delag` (_"Delag Hitscan Latency"_ f.k.a. "Unlag Hitscan").
  * Sound:
    * Transplanted from Game Options: `cl_chatBeep` (_"Beep on Chat"_) and `cg_teamChatBeep` (_"Beep on Team Chat"_).
    * New option: `cg_hitSound` (_"Enable Hitsound"_).
  * Video:
    * Transplanted from Game Options: `r_dynamicLight` (_"Dynamic Lights"_).
  * Find Servers:
    * The tab now remembers the value for _"Hide Private"_.
  * Controls:
    * `+acc` (_"Show Accuracy"_ under Misc)
    * `droprune` (_"Free Rune"_ under Misc)
    * `weapbest` (_"Select Best Weapon"_ under Shoot)
* AI improvements:
  * Grappling Hook handling, they won't use the weapon nor switch to it if they don't have it.
  * Domination support.
  * Double Domination support.
  * Support for CTF Elimination in "One-Way" (a.k.a. "Attack vs. Defense") mode.
  * Holdable item handling. No support for the cut Portal item, though.
* New tools for modders in general: gametype checkers for better control on how gametype and options behave.
  * `CG_IsATeamGametype(int gametype)` (cgame), `G_IsATeamGametype(int gametype)` (game), and `UI_IsATeamGametype(int gametype)` (q3_ui and ui) return true if the gametype is team-based. (For OA, it would be TDM, CTF, 1FCTF, Harvester -HAR-, Overload, Elimination, CTF Elimination -eCTF-, Double Domination -DD- and Domination -DOM-)
  * `CG_UsesTeamFlags(int gametype)` (cgame), `G_UsesTeamFlags(int gametype)` (game), and `UI_UsesTeamFlags(int gametype)` (q3_ui and ui) return true if the gametype uses the team-colored flags. (For OA, it would be CTF, 1FCTF and eCTF)
  * `CG_UsesTheWhiteFlag(int gametype)` (cgame), `G_UsesTheWhiteFlag(int gametype)` (game), and `UI_UsesTheWhiteFlag(int gametype)` (q3_ui and ui) return true if the gametype uses the neutral flag. (For OA, it would be 1FCTF and Possession -POS-)
  * `CG_IsARoundBasedGametype(int gametype)` (cgame), `G_IsARoundBasedGametype(int gametype)` (game), and `UI_IsARoundBasedGametype(int gametype)` (q3_ui and ui) return true if the gametype has a rounds rule. (For OA, it would be Elimination, eCTF and Last Man Standing -LMS-)
  * `CG_GametypeUsesFragLimit(int gametype)` (cgame), `G_GametypeUsesFragLimit(int gametype)` (game), and `UI_GametypeUsesFragLimit(int gametype)` (q3_ui and ui) return true if the gametype uses frags as a score limit. (For OA, it would be FFA, 1on1, TDM, HAR, DOM and POS)
  * `CG_GametypeUsesCaptureLimit(int gametype)` (cgame), `CG_GametypeUsesCaptureLimit(int gametype)` (game), and `CG_GametypeUsesCaptureLimit(int gametype)` (q3_ui and ui) return true if the gametype uses captures as a score limit. Done this way in case other score limits are later introduced. (For OA, it would be CTF, 1FCTF, Overload, Elimination, eCTF, LMS and DD)
  * `G_IsANoPickupsMode()` (game) returns true if the match uses a no-pickup rule. So far, this is used for the "Instantgib", "Single Weapon Arena" and "All Weapons (Elimination-like)" Weapon Rulesets.
  * `G_GametypeUsesRunes(int gametype)` (game) returns true if the gametype belongs to a short list where Runes can be used.
  * `G_GetAttackingTeam()` (game) returns the team that has the current attacker role for eCTF Attack vs. Defense matches.
* New AI-related tools for modders:
  * `BotTeamOwnsControlPoint(bot_state_t *bs,int point)` is used for both DD and DOM and returns true if the bot's team controls a Control Point.
  * `BotAreThereDOMPointsInTheMap()`, used for Domination, returns true if the match contains at least one Control Point (`domination_point`).
  * `BotIsThereDDPointAInTheMap()` and `BotIsThereDDPointBInTheMap()` return true if the match contain, respectively, the DD control point A (`team_ctf_redflag`) or the DD control point B (`team_ctf_blueflag`).
  * `BotIsThereABlueFlagInTheMap()`, `BotIsThereARedFlagInTheMap()` and `BotIsThereANeutralFlagInTheMap()` return true if the match contains a red flag (`team_ctf_redflag`), a blue flag (`team_ctf_blueflag`) or a neutral flag (`team_ctf_neutralflag`).
  * `BotIsThereABlueObeliskInTheMap()`, `BotIsThereARedObeliskInTheMap()` and `BotIsThereANeutralObeliskInTheMap()` return true if the match contains a red obelisk (`team_redobelisk`), a blue obelisk (`team_blueobelisk`) or a neutral obelisk (`team_neutralobelisk`).
  * `BotIsThereOurFlagInTheMap(bot_state_t *bs)` and `BotIsThereAnEnemyFlagInTheMap(bot_state_t *bs)` return true if, from the bot's own POV, there's their own team's flag or the enemy team's flag in the map.
  * `BotIsThereOurObeliskInTheMap(bot_state_t *bs)` and `BotIsThereAnEnemyObeliskInTheMap(bot_state_t *bs)` return true if, from the bot's own POV, there's their own team's obelisk or the enemy team's obelisk in the map.
  * `BotGetDominationPoint(bot_state_t *bs)` returns the actual control point the bot is taking care of. Only used for Domination.
  * `BotSetDominationPoint(bot_state_t *bs, int controlPoint)` and `BotSetOwnDominationPoint(bot_state_t *bs)` controls the control points the bot is taking care of. In the former case, it looks for a control point the bot's team doesn't own, or operate in a random point, provided the bot's team controls all the points. The latter searches among the control point the bot's team is already controlling, or assigns it a random one in case the bot's team doesn't control any point. Only used for Domination.
  * `BotMatch_AcknowledgeOrder(bot_state_t *bs, bot_match_t *match, int ltgType, float teamGoalTime)` is the general purpose order handler that replaces many functions with a similar working, some of which even called other functions, directly affecting the function stack.
  * `BotCanChat(bot_state_t *bs,float seed)` returns true if the bot is able to chat.
  * `BotCanAndWantsToUseTheGrapple(bot_state_t *bs)` returns true if the bot, well, is in a position to use the Grappling Hook.
  * `BotCanAndWantsToUseTheTeleporter(bot_state_t *bs)`, `BotCanAndWantsToUseTheMedkit(bot_state_t *bs)`, `BotCanAndWantsToUseTheKamikaze(bot_state_t *bs)` and `BotCanAndWantsToUseTheInvulnerability(bot_state_t *bs)` replace the old "BotBattleUseItems" function. They control in which situations the bots can use the Personal Teleporter, Medkit, Kamikaze and Invulnerability holdables.
  * `BotIsOnAttackingTeam(bot_state_t *bs)` is used for CTF Elimination in One-Way/Attack vs. Defense mode, and returns true if the bot is on the attacking team.
  * `BotIsFlagOnOurBase(bot_state_t *bs)` returns true if the bot's team's flag is in their team's base. Used for CTF and eCTF.
  * `BotIsEnemyFlagOnEnemyBase(bot_state_t *bs)` returns true if the bot's enemy team's flag is in the enemy team's base. Used for CTF and eCTF.
* New tools for modders for UI3:
  * `ui_randomIntToCvar()` saves a random integer to a cvar.
  * `ui_randomStringToCvar()` saves a random string from a list to a cvar.
  * `ui_randomFloatToCvar()` saves a random floating point number to a cvar.
* New tools for mappers:
  * `state_targetname` key for `func_door`, it overrides `targetname` in round-based gametypes. [It allows the creation of doors for Elimination-based modes that open during the active time of the game](https://openarena.ws/board/index.php?topic=5437.0). In-game demonstration video [here](https://www.youtube.com/watch?v=lHq56Gx058w). It's already supported by [the gamepack](https://github.com/NeonKnightOA/oagamepack).
  * New entities: `info_player_dom_red` and `info_player_dom_blue`. Both act as dedicated spawnpoints for Domination. They are supported by the gamepack too.
  * Support for `.info` map files (i.e. `maps/oasago2.info`). They are text files containing important data for the maps. Some configuration examples can be found [here](https://github.com/NeonKnightOA/nk-oa-assets/blob/master/maps/DM/galmevish/am_galmevish3.info) and [here](https://github.com/NeonKnightOA/nk-oa-assets/blob/master/maps/DM/blitzkrieg/blitzkrieg3.info). Supported fields:
    * `author` (Author of the map)
    * `title` (Map title, i.e. for oasago2 it would display "Sago's Castle")
    * `description` (Short description of the context/backstory of the map)
    * `mpBots` (Bots per name to be added to non-team-based games for this map; impacts directly in SP)
    * `mpBotCount` (Amount of randomly picked bots that should be summoned instead of calling them by name for this map)
    * `redBots` (Bots per name to be added to the Red team in team-based games for this map)
    * `redBotCount` (Amount of randomly picked bots that should be added to the Red team in team-based games for this map)
    * `blueBots` (Bots per name to be added to the Blue team in team-based games for this map)
    * `blueBotCount` (Amount of randomly picked bots that should be added to the Blue team in team-based games for this map)
    * `teamBotCount` (Amount of randomly picked up bots that should be added in each team)
    * `captureLimit` (Limit of captures in general for capture-based gamemodes for this map)
    * `fragLimit` (Limit of points in general for score-based gamemodes for this map; impacts directly in SP)
    * `timeLimit` (Time limit in general for this map; impacts directly in SP)
    * `maxPlayers` (Amount of ideal max players in games in this map)
    * `minPlayers` (Amount of ideal min players in games in this map)
    * `maxTeamSize` (Amount of ideal max team members in team games in this map)
    * `minTeamSize` (Amount of ideal max team members in team games in this map)
    * `recommendedPlayers` (Amount of ideal players in games in this map)
    * `recommendedTeamSize` (Amount of ideal team members in team games)
    * `special` (Field meant to be used for SP, specifies the kind of match and the rules that should be applied to it (Training, Instagib, Final... see below))
    * `timeToBeatPlatinum`, `timeToBeatGold`, `timeToBeatSilver` and `timeToBeatBronze` (Also meant for SP, specifies the par times for each trophy, with Platinum being the first not to be "earned" and Bronze the last).
* New special matches for Classic SP in addition to `"training"` and `"final"`. These can be specified in either the map's specific .info file (the game will look at this first) the map's .arena file or arenas.txt:
  * `"special" "instantGib"`: Instagib match.
  * `"special" "singleWeaponGA"`: Single Weapon: Gauntlet match.
  * `"special" "singleWeaponMG"`: Single Weapon: Machinegun match.
  * `"special" "singleWeaponSG"`: Single Weapon: Shotgun match.
  * `"special" "singleWeaponGL"`: Single Weapon: Grenade Launcher match.
  * `"special" "singleWeaponRL"`: Single Weapon: Rocket Launcher match.
  * `"special" "singleWeaponLG"`: Single Weapon: Lightning Gun match.
  * `"special" "singleWeaponRG"`: Single Weapon: Railgun match.
  * `"special" "singleWeaponPG"`: Single Weapon: Plasma Gun match.
  * `"special" "singleWeaponBFG"`: Single Weapon: BFG match.
  * `"special" "singleWeaponNG"`: Single Weapon: Nailgun match.
  * `"special" "singleWeaponCG"`: Single Weapon: Chaingun match.
  * `"special" "singleWeaponPL"`: Single Weapon: Prox Launcher match.
  * `"special" "grappleMatch"`: Grappling Hook match. (Adds the GH to the (re)spawning inventory).
  * `"special" "eliminationMode"`: Elimination Mode match. (No pickups, all weapons, full health and armor).
  * `"special" "vampireMode"`: Vampire Mode match. 50% of all damage dealt becomes health for the player.
  * `"special" "lowGravity"`: Low Gravity match. (Gravity of the level reduced to 100).
* In-game feedback:
  * Double Domination now displays a "New Round" message between rounds.
  * CTF Elimination "Attack vs. Defense" mode (formerly "One-Way" mode): Now players on both teams are properly informed of their roles. When a team attacks, the message (_"You Are Attacking!"_) displayed is fully saturated in the team's color (i.e. fully blue for Blue team, fully red for Red team), while when defending the message (_"You Are Defending!"_) is displayed in a lighter color (magenta for the Red team, cyan for the Blue team). The original messages (_"Blue/Red/Unknown Team is on Offence"_) are available for spectators.
  * The "You've Been Mined" message, shown whenever you're directly hit by a proxy mine, now begins its combustion countdown instantly.
* Default score limits are now consistent across all gamemodes.
* Compatibility fixes to make gamecode buidable under M1 Mac. (Thanks @Bishop-333!)
* Team Orders now display the proper team orders rather than those of Team Deathmatch for all team-based gamemodes.
* Challenges now account for TDM wins.
* Removed some #ifdef that blocked legit AI code out of Classic UI.
* Bots now use tourney6-like crushers against their enemies, and ONLY their enemies.
* Score bonus fixes for CTF and 1FCTF.
* UI3: Now displays a correct aspect in widescreen.
* UI3: List boxes now support the mouse wheel.
* Improved keyboard/joystick input in UI3.
* Vote Ballots are now logged in the console.
* Compatibility fixes to make gamecode buidable under GNU-Hurd and kFreeBSD (Credit: Cyril Brulebois, Svante Signell and @smcv).
* Round-based gametypes: Bots are now gradually killed if all human players were fragged.
* The Proxy Mine short fuse now works only on key objective-based gametypes.
* With cheats enabled, "Missing Spawnpoints" won't trigger. If cheats are disabled, the message that outputs is more explicit about what's exactly missing.
* Servers will now execute the file `gametype_GAMETYPENUMBER.cfg` (i.e. `gametype_0.cfg` for FFA or `gametype_3.cfg` for TDM) on gametype change.
* Servers will now execute `mapscripts/g_MAPNAME.cfg` (i.e. `mapscripts/g_oasago2.cfg` for FFA or `mapscripts/g_oa_minia.cfg` for TDM) and `mapscripts/g_MAPNAME_GAMETYPENUMBER.cfg` (i.e. `mapscripts/g_oasago2_4.cfg` for CTF or `mapscripts/g_oa_minia_11.cfg` for DD) prior to a map change. It falls back to `mapscripts/g_default.cfg` and/or `mapscripts/g_default_GAMETYPENUMBER.cfg` (i.e. `mapscripts/g_default_0.cfg` for FFA or `mapscripts/g_default_3.cfg` for TDM) if any or both or the scripts are missing.
* Weapon accuracy is now written to the logfile. The stats are written at the end of the game or when a player leaves.
* The background is now streched on widescreen displays.
* Many other under-the-hood changes.

## oaxB53.2

**Estimated release date:** April 5, 2024

* New special matches for Classic SP in addition to `"training"` and `"final"`. These can be specified in either the map's specific .info file (the game will look at this first) the map's .arena file or arenas.txt:
  * `"special" "instantGib"`: Instagib match.
  * `"special" "singleWeaponGA"`: Single Weapon: Gauntlet match.
  * `"special" "singleWeaponMG"`: Single Weapon: Machinegun match.
  * `"special" "singleWeaponSG"`: Single Weapon: Shotgun match.
  * `"special" "singleWeaponGL"`: Single Weapon: Grenade Launcher match.
  * `"special" "singleWeaponRL"`: Single Weapon: Rocket Launcher match.
  * `"special" "singleWeaponLG"`: Single Weapon: Lightning Gun match.
  * `"special" "singleWeaponRG"`: Single Weapon: Railgun match.
  * `"special" "singleWeaponPG"`: Single Weapon: Plasma Gun match.
  * `"special" "singleWeaponBFG"`: Single Weapon: BFG match.
  * `"special" "singleWeaponNG"`: Single Weapon: Nailgun match.
  * `"special" "singleWeaponCG"`: Single Weapon: Chaingun match.
  * `"special" "singleWeaponPL"`: Single Weapon: Prox Launcher match.
  * `"special" "grappleMatch"`: Grappling Hook match. (Adds the GH to the (re)spawning inventory).
  * `"special" "eliminationMode"`: Elimination Mode match. (No pickups, all weapons, full health and armor).
  * `"special" "vampireMode"`: Vampire Mode match. 50% of all damage dealt becomes health for the player.
  * `"special" "lowGravity"`: Low Gravity match. (Gravity of the level reduced to 100).
* The fields `fragLimit`, `timeLimit`, `special` and `mpBots` from the .info files now impact directly in Classic SP.
* New fields for .info files: `teamBotCount`, `special`, `timeToBeatPlatinum`, `timeToBeatGold`, `timeToBeatSilver` and `timeToBeatBronze`.
* Non-capturelimit modes won't display capturelimit if fraglimit is <= 0.
* New default round time limits for Elimination/eCTF/LMS.
* Reverted Harvester as a capturelimit gamemode.
* All default Time Limits set to 15.
* In classic SP, the console won't complain anymore for missing character announcement lines.
* Railgun glass now matches the selected beam color. (Credit: @Bishop-333)
* Suppressed the lots of ".info file not found" messages to Developer mode.
* The key objective checker now checks for (at least one) Domination points in DOM and for the white flag in POS.
* Flags aren't drawn anymore in Harvester/Overload.
* New helper function for game: `G_GetAttackingTeam()`, returns the actual team that's in the attacker role in eCTF AvD mode.
* CTF Elimination, AvD mode (formerly "One-Way mode"): Now both teams will get informed on their role appropriately:
  * If the team is attacking, the players will receive a _"You Are Attacking!"_ message with the fully saturated color of their team.
  * If they're defending instead, they will receive _"You Are Defending!"_ in either Magenta (Red Team) or Cyan (Blue Team) coloring.
  * The older messages (_"Blue/Red/Unknown Team is on Offence"_) are available for spectators of the match.
* Skirmish/Create Server:
  * For Weapon Rulesets in Round-based games, removed duplicate _"All Weapons (Elimination)"_ and shortened to _"All Weapons (Elim.)"_. Also shortened _"All Weapons (Standard)"_ to _"All Weapons (Default)"_.
  * Changed the item descriptions for "Award Pushing".
* "Apply" button added to the Sound menu. (Credit: @Bishop-333)
* DD now displays "X team dominates!" on top of the counter.
* Timer, Movement Speed and Last Attacker are now rendered on top of the gametype-specific strings.
* A certain string modification was wrongly applied to 1FCTF when it should have been applied to CTF. (Fixes OpenArena/gamecode#163)
* `g_classicMode` is now a serverside cvar.
* Fixes for some bad gametype comparisons that made `capturelimit` affect Classic SP.
* "You Have Been Mined" message now displays correctly. (Fixes OpenArena/gamecode#266)
* Fix for bad default for `cg_hitSound` (Credit: @leilei-)
* Many other changes under the hood.

## oaxB53

**Release date:** March 22, 2024

* New cvars:
  * `cg_bob`: Controls how bobbing behaves in-game. It accepts the following values: (Credits: @ldrone (options 0/1) and @leilei- (options 2-6))
    * `cg_bob 0`: Disables it.
    * `cg_bob 1`: Pitch/Roll/Up Bob.
    * `cg_bob 2`: Pitch/Up-only Bob.
    * `cg_bob 3`: Roll/Up-only Bob.
    * `cg_bob 4`: Up-only Bob.
    * `cg_bob 6`: "Fake '99" a.k.a. "Sweeny Bob". (We cannot write the actual name because [writing around trademarks](https://tvtropes.org/Main/WritingAroundTrademarks))
  * `cg_kickScale`: Controls how much the screen shakes when hit. (Credit: @ldrone)
  * `cg_muzzleFlashStyle`: Controls how the muzzle flash is rendered. (Credit: @leilei-)
  * `cg_obituaryOutput`: Controls how and where the death messages are rendered. (Credit: @ldrone)
  * `g_classicMode`. It replaces `g_runes` by extending it. In addition to removing the Runes, it also replaces the Nailgun with the Shotgun, the Chaingun with the Machinegun ammo, the Prox Launcher with the Grenade Launcher, the Kamikaze holdable with the Personal Teleporter and the Invulnerability holdable with the Medkit. In Round-based gamemodes, it prevents the Nailgun, Chaingun and Prox Launcher from being added to the player's starting inventory.
  * `g_ddCaptureTime` and `g_ddRespawnDelay` for Double Domination. Both control the amount of holding time required to score, and the time between the scoring and start of a new round.
  * `g_grapple`. It replaces `elimination_grapple` by extending it so it works with all gamemodes.
  * `g_harvesterFromBodies` for Harvester. If enabled, skulls spawn from bodies rather than a central skull generator. Allows Harvester matches to take place in maps without a skull generator. However, the placement of a skull generator is still recommended for those cases where the players commit suicide or are killed via level hazard.
  * `g_weaponArena` and `g_weaponArenaWeapon`. The former replaces `g_rockets` by extending it so it can be used with any weapon, not just the Rocket Launcher. The latter controls which weapon is spawned by `g_weaponArena`.
  * `missionpackChecks`. Enables/disables missionpack checkers in both Classic UI and UI3. It's enabled by default for the latter, and disabled for the former.
* New bindings:
  * `+acc`. Reveals your weapon accuracy after matches.
  * `dropRune`. Frees the rune for another teammate to use it.
  * `weapbest`. Selects the "best" weapon out of all those that the player carry which have ammo. The cvar controlling this priority is `cg_weaponOrder`; unlike the command, the cvar still needs to be set via console.
* New _dmflag_: `dmflag &4096`, allows faster underwater movement a la Q2.
* `bot_minplayers` now work for LMS and Possession.
* `capturelimit` no longer affects Team Deathmatch.
* `cg_teamChatsOnly` now only affects team-based gamemodes.
* `cg_weaponBarStyle` now returns the default weapon bar if its value falls outside its limits.
* `developer` mode has been extended with several cvars now requiring the mode to be enabled. All these cvars are also cheat-protected.
  * New cheat-protected development cvars: `cg_debugOrbit`, `cg_debugAccuracy`, `bot_debugChat`, `bot_debugLTG` and `bot_debugPaths`.
  * Restored `cg_debugDelag`.
  * Other cvars that require developer 1 include `cg_debugAnim`, `cg_debugPosition`, `cg_debugEvents`, `cg_leiDebug`, `g_debugMove`, `g_debugDamage`, `g_debugAlloc`, `ui_debug` (MPUI/UI3) and `ui_leiDebug` (MPUI/UI3).
* `elimination_selfDamage` now takes four values: 0 (no damage to self or teammates), 1 (only damage to self), 2 (only damage to teammates a.k.a. Friendly Fire) and 3 (self and teammate damage).
* New option for `g_awardPushing`: `g_awardPushing 2`. [If a player commits suicide, the last player who attacked them scores a point.](https://openarena.ws/board/index.php?topic=5289.msg54334#msg54334)
* Spectators can now use the `noclip` cheat code.
* `shuffle` command has been removed and reimplemented, with the solution present in Aftershock being used here.
* Classic UI:
  * Skirmish/Create Server/Game Options/H.U.D.: All options now have small helper texts. (Status bar) Some options will display what each selected option does rather than a general help text.
  * Skirmish/Create Server:
    * Server options are now displayed in the left column, while Gametype options are displayed at the right.
    * New options:
      * `g_grapple` (_"Grappling Hook"_).
      * "Weapon Ruleset", allows picking up between the _"All Weapons (Standard)"_, _"Instantgib"_ (`g_instantgib 1`), _"Single Weapon Arena"_ (`g_weaponArena 1`), _"Classic Arena"_ (`g_classicMode 1`) and _"All Weapons (Elimination)"_ (`g_elimination 1`) rulesets for non-Round-based gamemodes, and between _"All Weapons (Elimination)"_, _"Instantgib"_, _"Single Weapon Arena"_ and _"Classic Arena"_ rulesets for Round-based gamemodes.
      * `g_weaponArenaWeapon` (_"SWA Mode Weapon"_).
      * `g_awardPushing` (_"Award Pushing Rule"_).
      * `g_harvesterFromBodies` (_"Skulls From Bodies"_) for Harvester.
      * `g_obeliskRespawnRate` (_"Time Between Rounds"_) for Overload.
      * `elimination_roundTime` (_"Round Time Limit"_) for Elimination, eCTF and LMS.
      * `elimination_selfDamage` (_"Damage To"_) for Elimination and eCTF.
      * `g_lms_lives` (_"Lives Per Player"_) for LMS.
    * Transplanted from Network: `sv_allowDownload` (split from "Autodownload" as _"Clients Can Download"_).
    * "Physics" is now named _"Physics Ruleset"_.
    * "One-Way Mode" for CTF Elimination (`elimination_ctf_oneway`) is now named _"Attack vs. Defense"_.
  * New H.U.D. menu, accessible from Setup, controls the elements that are displayed in, well, the HUD, including the crosshair and weapon bar. Among the new options added and not mentioned later in this changelog:
    * Transplanted from Video: `cg_drawFPS` (_"Draw FPS"_).
    * Transplanted from Game Options: `cg_drawCrosshair` (_"Crosshair Style"_), `cg_crosshairHealth` (_"Crosshair Shows Health"_), `cg_crosshairColorRed` (_"Crosshair Color (Red)"_), `cg_crosshairColorGreen` (_"Crosshair Color (Green)"_), `cg_crosshairColorBlue` (_"Crosshair Color (Blue)"_), `cg_alwaysWeaponBar` (_"Always Show Weapon Bar"_), `cg_drawCrosshairNames` (_"Show Crosshair Target Name"_ f.k.a. "Identify Target") and `cg_drawTeamOverlay` (_"Show Team Overlay"_).
    * New options: `cg_crosshairPulse` (_"Crosshair Pickup Pulse"_), `cg_draw3DIcons` (_"Weapon Bar Icon Rendering"_ in _"2D"_ or _"3D"_), `cg_weaponBarStyle` (_"Weapon Bar Style"_), `cg_obituaryOutput` (_"Death Messages"_), `cg_drawTimer` (_"Show Timer"_), `cg_drawStatus` (_"Show Player Status"_), `cg_drawAmmoWarning` (_"Show Low Ammo Warning"_), `cg_drawAttacker` (_"Show Last Attacker"_), `cg_drawSpeed` (_"Show Movement Speed"_) and `cg_drawRewards` (_"Show Medal(s) Earned"_).
  * Game Options:
    * Options transplanted to many menus as mentioned above and below.
    * Adjusted item positioning.
    * New options: `cg_drawGun` (_"Weapon Hand"_: _"Hidden"_, _"Left"_, _"Right"_, _"Center"_), `cg_muzzleFlashStyle` (_"Muzzle Flash Style"_), `com_blood` (_"Show Blood"_), `cg_gibs` (_"Show Gibs"_), `cg_bob` (_"View Bobbing"_), `cg_bobModel` (_"Weapon Bobbing"_), `cg_railTrailTime` (_"Railgun Slug Trail Time"_) and `cg_kickScale` (_"Screen Shaking Rate"_).
    * Renamed "Simple Items" to _"In-Game Pickup Rendering"_ (with 2D and 3D options).
  * Network:
    * Transplanted from Game Options: `cl_allowDownload` (_"Download From Servers"_, f.k.a. "Autodownload") and `cg_delag` (_"Delag Hitscan Latency"_ f.k.a. "Unlag Hitscan").
  * Sound:
    * Transplanted from Game Options: `cl_chatBeep` (_"Beep on Chat"_) and `cg_teamChatBeep` (_"Beep on Team Chat"_).
    * New option: `cg_hitSound` (_"Enable Hitsound"_).
  * Video:
    * Transplanted from Game Options: `r_dynamicLight` (_"Dynamic Lights"_).
  * Find Servers:
    * The tab now remembers the value for _"Hide Private"_.
  * Controls:
    * `+acc` (_"Show Accuracy"_ under Misc)
    * `droprune` (_"Free Rune"_ under Misc)
    * `weapbest` (_"Select Best Weapon"_ under Shoot)
* Possession:
  * It can now be voted after matches. (Credit: @The-Gig)
  * Default flag holding time reduced to 120 (2 minutes).
  * Events and victories are now logged into the Challenges.
  * Improved AI.
* AI improvements:
  * Grappling Hook handling, they won't use the weapon nor switch to it if they don't have it.
  * Domination support.
  * Double Domination support.
  * Support for CTF Elimination in "One-Way" (a.k.a. "Attack vs. Defense") mode.
  * Holdable item handling. No support for the cut Portal item, though.
* New tools for modders in general: gametype checkers for better control on how gametype and options behave.
  * `CG_IsATeamGametype(int gametype)` (cgame), `G_IsATeamGametype(int gametype)` (game), and `UI_IsATeamGametype(int gametype)` (q3_ui and ui) return true if the gametype is team-based. (For OA, it would be TDM, CTF, 1FCTF, Harvester -HAR-, Overload, Elimination, CTF Elimination -eCTF-, Double Domination -DD- and Domination -DOM-)
  * `CG_UsesTeamFlags(int gametype)` (cgame), `G_UsesTeamFlags(int gametype)` (game), and `UI_UsesTeamFlags(int gametype)` (q3_ui and ui) return true if the gametype uses the team-colored flags. (For OA, it would be CTF, 1FCTF and eCTF)
  * `CG_UsesTheWhiteFlag(int gametype)` (cgame), `G_UsesTheWhiteFlag(int gametype)` (game), and `UI_UsesTheWhiteFlag(int gametype)` (q3_ui and ui) return true if the gametype uses the neutral flag. (For OA, it would be 1FCTF and Possession -POS-)
  * `CG_IsARoundBasedGametype(int gametype)` (cgame), `G_IsARoundBasedGametype(int gametype)` (game), and `UI_IsARoundBasedGametype(int gametype)` (q3_ui and ui) return true if the gametype has a rounds rule. (For OA, it would be Elimination, eCTF and Last Man Standing -LMS-)
  * `CG_GametypeUsesFragLimit(int gametype)` (cgame), `G_GametypeUsesFragLimit(int gametype)` (game), and `UI_GametypeUsesFragLimit(int gametype)` (q3_ui and ui) return true if the gametype uses frags as a score limit. (For OA, it would be FFA, 1on1, TDM, HAR, DOM and POS)
  * `CG_GametypeUsesCaptureLimit(int gametype)` (cgame), `CG_GametypeUsesCaptureLimit(int gametype)` (game), and `CG_GametypeUsesCaptureLimit(int gametype)` (q3_ui and ui) return true if the gametype uses captures as a score limit. Done this way in case other score limits are later introduced. (For OA, it would be CTF, 1FCTF, Overload, Elimination, eCTF, LMS and DD)
  * `G_IsANoPickupsMode()` (game) returns true if the match uses a no-pickup rule. So far, this is used for the "Instantgib", "Single Weapon Arena" and "All Weapons (Elimination-like)" Weapon Rulesets.
  * `G_GametypeUsesRunes(int gametype)` (game) returns true if the gametype belongs to a short list where Runes can be used.
* New AI-related tools for modders:
  * `BotTeamOwnsControlPoint(bot_state_t *bs,int point)` is used for both DD and DOM and returns true if the bot's team controls a Control Point.
  * `BotAreThereDOMPointsInTheMap()`, used for Domination, returns true if the match contains at least one Control Point (`domination_point`).
  * `BotIsThereDDPointAInTheMap()` and `BotIsThereDDPointBInTheMap()` return true if the match contain, respectively, the DD control point A (`team_ctf_redflag`) or the DD control point B (`team_ctf_blueflag`).
  * `BotIsThereABlueFlagInTheMap()`, `BotIsThereARedFlagInTheMap()` and `BotIsThereANeutralFlagInTheMap()` return true if the match contains a red flag (`team_ctf_redflag`), a blue flag (`team_ctf_blueflag`) or a neutral flag (`team_ctf_neutralflag`).
  * `BotIsThereABlueObeliskInTheMap()`, `BotIsThereARedObeliskInTheMap()` and `BotIsThereANeutralObeliskInTheMap()` return true if the match contains a red obelisk (`team_redobelisk`), a blue obelisk (`team_blueobelisk`) or a neutral obelisk (`team_neutralobelisk`).
  * `BotIsThereOurFlagInTheMap(bot_state_t *bs)` and `BotIsThereAnEnemyFlagInTheMap(bot_state_t *bs)` return true if, from the bot's own POV, there's their own team's flag or the enemy team's flag in the map.
  * `BotIsThereOurObeliskInTheMap(bot_state_t *bs)` and `BotIsThereAnEnemyObeliskInTheMap(bot_state_t *bs)` return true if, from the bot's own POV, there's their own team's obelisk or the enemy team's obelisk in the map.
  * `BotGetDominationPoint(bot_state_t *bs)` returns the actual control point the bot is taking care of. Only used for Domination.
  * `BotSetDominationPoint(bot_state_t *bs, int controlPoint)` and `BotSetOwnDominationPoint(bot_state_t *bs)` controls the control points the bot is taking care of. In the former case, it looks for a control point the bot's team doesn't own, or operate in a random point, provided the bot's team controls all the points. The latter searches among the control point the bot's team is already controlling, or assigns it a random one in case the bot's team doesn't control any point. Only used for Domination.
  * `BotMatch_AcknowledgeOrder(bot_state_t *bs, bot_match_t *match, int ltgType, float teamGoalTime)` is the general purpose order handler that replaces many functions with a similar working, some of which even called other functions, directly affecting the function stack.
  * `BotCanChat(bot_state_t *bs,float seed)` returns true if the bot is able to chat.
  * `BotCanAndWantsToUseTheGrapple(bot_state_t *bs)` returns true if the bot, well, is in a position to use the Grappling Hook.
  * `BotCanAndWantsToUseTheTeleporter(bot_state_t *bs)`, `BotCanAndWantsToUseTheMedkit(bot_state_t *bs)`, `BotCanAndWantsToUseTheKamikaze(bot_state_t *bs)` and `BotCanAndWantsToUseTheInvulnerability(bot_state_t *bs)` replace the old "BotBattleUseItems" function. They control in which situations the bots can use the Personal Teleporter, Medkit, Kamikaze and Invulnerability holdables.
  * `BotIsOnAttackingTeam(bot_state_t *bs)` is used for CTF Elimination in One-Way/Attack vs. Defense mode, and returns true if the bot is on the attacking team.
  * `BotIsFlagOnOurBase(bot_state_t *bs)` returns true if the bot's team's flag is in their team's base. Used for CTF and eCTF.
  * `BotIsEnemyFlagOnEnemyBase(bot_state_t *bs)` returns true if the bot's enemy team's flag is in the enemy team's base. Used for CTF and eCTF.
* New tools for modders for UI3:
  * `ui_randomIntToCvar(int number)` saves a random integer to a cvar.
  * `ui_randomStringToCvar(int number)` saves a random string from a list to a cvar.
  * `ui_randomFloatToCvar(int number)` saves a random floating point number to a cvar.
* New tool for mappers: added `state_targetname` key for `func_door`, it overrides `targetname` in round-based gametypes. [It allows the creation of doors for Elimination-based modes that open during the active time of the game](https://openarena.ws/board/index.php?topic=5437.0). In-game demonstration video [here](https://www.youtube.com/watch?v=lHq56Gx058w).
* New fields for .info map files:
  * `title` (Map title, i.e. for oasago2 it would display "Sago's Castle")
  * `mpBotCount` (Amount of randomly picked bots that should be summoned instead of calling them by name)
  * `redBots` (Bots to be added to the Red team in team-based games)
  * `redBotCount` (Amount of randomly picked bots that should be added to the Red team in team-based games)
  * `blueBots` (Bots to be added to the Blue team in team-based games)
  * `blueBotCount` (Amount of randomly picked bots that should be added to the Blue team in team-based games)
  * `recommendedTeamSize` (Amount of ideal team members in team games)
* The "You've Been Mined" message, shown whenever you're directly hit by a proxy mine, now begins its combustion countdown instantly.
* Default score limits are now consistent across all gamemodes.
* Compatibility fixes to make gamecode buidable under M1 Mac. (Thanks @Bishop-333!)
* Team Orders now display the proper team orders rather than those of Team Deathmatch for all team-based gamemodes.
* Challenges now account for TDM wins.
* Double Domination now displays a "New Round" message.
* Removed some #ifdef that blocked legit AI code out of Classic UI.
* Bots now use tourney6-like crushers against their enemies, and ONLY their enemies.
* Score bonus fixes for CTF and 1FCTF.
* UI3: Now displays a correct aspect in widescreen.
* UI3: List boxes now support the mouse wheel.
* Improved keyboard/joystick input in UI3.
* Vote Ballots are now logged in the console.
* Compatibility fixes to make gamecode buidable under GNU-Hurd and kFreeBSD (Credit: Cyril Brulebois, Svante Signell and @smcv).
* Round-based gametypes: Bots are now gradually killed if all human players were fragged.
* The Proxy Mine short fuse now works only on key objective-based gametypes.
* With cheats enabled, "Missing Spawnpoints" won't trigger. If cheats are disabled, the message that outputs is more explicit about what's exactly missing.
* Many other under-the-hood changes.

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

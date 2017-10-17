/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Open Arena source code.
Portions copied from Tremulous under GPL version 2 including any later version.

Open Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Open Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Open Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
 */
//
// bg_misc.c -- both games misc functions, all completely stateless

#include "../qcommon/q_shared.h"
#include "bg_public.h"

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
 */

gitem_t bg_itemlist[] ={
	{
		NULL,
		NULL,
		{ NULL,
			NULL,
			NULL, NULL},
		/* icon */ NULL,
		/* pickup */ NULL,
		0,
		0,
		0,
		/* precache */ "",
		/* sounds */ ""
	}, // leave index 0 alone

	//
	// ARMORS
	//

	/*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Armor Shard. Gives 5 Armor Points, up to a maximum of 200, which slowly wears out to 100.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_armor_shard",
		"sound/misc/ar1_pkup.wav",
		{ "models/powerups/armor/shard.md3",
			"models/powerups/armor/shard_sphere.md3",
			NULL, NULL},
		/* icon */ "icons/iconr_shard",
		/* pickup */ "Armor Shard",
		5,
		IT_ARMOR,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Combat Armor. Gives 50 Armor Points, up to a maximum of 200, which slowly wears out to 100.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_armor_combat",
		"sound/misc/ar2_pkup.wav",
		{ "models/powerups/armor/armor_yel.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconr_yellow",
		/* pickup */ "Armor",
		50,
		IT_ARMOR,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Body Armor. Gives 100 Armor Points, up to a maximum of 200, which slowly wears out to 100.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_armor_body",
		"sound/misc/ar2_pkup.wav",
		{ "models/powerups/armor/armor_red.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconr_red",
		/* pickup */ "Heavy Armor",
		100,
		IT_ARMOR,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// HEALTH ITEMS
	//

	/*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Small Health. Gives 5 Health Points, up to a maximum of 200, which slowly wears out to 100.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_health_small",
		"sound/items/s_health.wav",
		{ "models/powerups/health/small_cross.md3",
			"models/powerups/health/small_sphere.md3",
			NULL, NULL},
		/* icon */ "icons/iconh_green",
		/* pickup */ "5 Health",
		5,
		IT_HEALTH,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_health (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Medium Health. Gives 25 Health Points, up to a maximum of 100. If health > 100 then it cannot be picked up.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_health",
		"sound/items/n_health.wav",
		{ "models/powerups/health/medium_cross.md3",
			"models/powerups/health/medium_sphere.md3",
			NULL, NULL},
		/* icon */ "icons/iconh_yellow",
		/* pickup */ "25 Health",
		25,
		IT_HEALTH,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Large Health. Gives 50 Health Points, up to a maximum of 100. If health > 100 then it cannot be picked up.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_health_large",
		"sound/items/l_health.wav",
		{ "models/powerups/health/large_cross.md3",
			"models/powerups/health/large_sphere.md3",
			NULL, NULL},
		/* icon */ "icons/iconh_red",
		/* pickup */ "50 Health",
		50,
		IT_HEALTH,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_health_mega (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Mega Health. Gives 100 Health Points, up to a maximum of 200, which slowly wears out to 100.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        -------- NOTES --------
	First and foremost: it is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_health_mega",
		"sound/items/m_health.wav",
		{ "models/powerups/health/mega_cross.md3",
			"models/powerups/health/mega_sphere.md3",
			NULL, NULL},
		/* icon */ "icons/iconh_mega",
		/* pickup */ "Mega Health",
		100,
		IT_HEALTH,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// REGULAR WEAPONS 
	//

	/*QUAKED weapon_gauntlet (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Gauntlet weapon. Meleé weapon which consumes no ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Usually you don't need to include the gauntlet, as it's a "stock" weapon. 

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_gauntlet",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/gauntlet/gauntlet.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_gauntlet",
		/* pickup */ "Gauntlet",
		0,
		IT_WEAPON,
		WP_GAUNTLET,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Shotgun weapon. Does high damage at close quarters and can kill an unarmored player quickly. Comes with 10 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_shotgun",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/shotgun/shotgun.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_shotgun",
		/* pickup */ "Shotgun",
		10,
		IT_WEAPON,
		WP_SHOTGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_machinegun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Machinegun weapon. Hitscan weapon with starting ammo of 100 for regular modes, and 50 for team modes. Comes with 40 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 40.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Usually, there's no need to include the machinegun weapon in the map, but only its ammo, because by default all players spawn already having it. 

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_machinegun",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/machinegun/machinegun.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_machinegun",
		/* pickup */ "Machinegun",
		40,
		IT_WEAPON,
		WP_MACHINEGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Grenade Launcher weapon. Fires grenades which ricochet walls, ceilings and floors, and explodes after 2.5 seconds, dealing either direct or splash damage. Comes with 10 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_grenadelauncher",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/grenadel/grenadel.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_grenade",
		/* pickup */ "Grenade Launcher",
		10,
		IT_WEAPON,
		WP_GRENADE_LAUNCHER,
		/* precache */ "",
		/* sounds */ "sound/weapons/grenade/hgrenb1a.wav sound/weapons/grenade/hgrenb2a.wav"
	},

	/*QUAKED weapon_rocketlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Rocket Launcher weapon. Fires a rocket in a straight line, dealing direct or splash damage upon explosion. It's also used to do rocket jumps. Comes with 10 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_rocketlauncher",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/rocketl/rocketl.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_rocket",
		/* pickup */ "Rocket Launcher",
		10,
		IT_WEAPON,
		WP_ROCKET_LAUNCHER,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Lightning Gun weapon. Fires a limited-range beam of lightning that deals constant damage. Comes with 100 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 100.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_lightning",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/lightning/lightning.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_lightning",
		/* pickup */ "Lightning Gun",
		100,
		IT_WEAPON,
		WP_LIGHTNING,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Railgun weapon. Sniper's choice. Used for long-range combat, has the highest accuracy and the highest rate of fire as well, but leaves a trail, giving players a hint of where the shots came from. Comes with 10 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	If you have doubts about placing the Railgun in your map, remember that your map is going to be played in Instagib mode. Plan your map with limiting the weapon's powerfulness in mind. That said...

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_railgun",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/railgun/railgun.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_railgun",
		/* pickup */ "Railgun",
		10,
		IT_WEAPON,
		WP_RAILGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_plasmagun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Plasma Gun weapon. Fires a stream of plasma pulses inflicting minor splash damage. Can be used for plasma climbing. Comes with 50 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 50.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_plasmagun",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/plasma/plasma.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_plasma",
		/* pickup */ "Plasma Gun",
		50,
		IT_WEAPON,
		WP_PLASMAGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	BFG weapon. God-Tier Plasma Gun on steroids. Has a rapid rate of fire (but not as fast as the PG), and deals even more damage per hit. Comes with 20 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 20.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	BFG is considered a superweapon, so it is usually placed in difficult or dangerous areas, or not included at all.

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_bfg",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/bfg/bfg.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_bfg",
		/* pickup */ "BFG10K",
		20,
		IT_WEAPON,
		WP_BFG,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_grapplinghook (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Grappling Hook weapon. Can be seen as more of an utility rather than a weapon, since it deals little damage. It's main use is to reach places normally unreachable on foot or via weaponjumping. Consumes no ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_grapplinghook",
		"sound/misc/w_pkup.wav",
		{ "models/weapons2/grapple/grapple.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_grapple",
		/* pickup */ "Grappling Hook",
		0,
		IT_WEAPON,
		WP_GRAPPLING_HOOK,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// AMMO ITEMS
	//

	/*QUAKED ammo_shells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 10 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_shells",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/shotgunam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_shotgun",
		/* pickup */ "Shells",
		10,
		IT_AMMO,
		WP_SHOTGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_bullets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Machinegun weapon. Default: 50 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 50.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_bullets",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/machinegunam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_machinegun",
		/* pickup */ "Bullets",
		50,
		IT_AMMO,
		WP_MACHINEGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 5 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 5.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_grenades",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/grenadeam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_grenade",
		/* pickup */ "Grenades",
		5,
		IT_AMMO,
		WP_GRENADE_LAUNCHER,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_cells (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 30 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 30.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_cells",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/plasmaam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_plasma",
		/* pickup */ "Cells",
		30,
		IT_AMMO,
		WP_PLASMAGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_lightning (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 60 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 60.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_lightning",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/lightningam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_lightning",
		/* pickup */ "Lightning",
		60,
		IT_AMMO,
		WP_LIGHTNING,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 5 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 5.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_rockets",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/rocketam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_rocket",
		/* pickup */ "Rockets",
		5,
		IT_AMMO,
		WP_ROCKET_LAUNCHER,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_slugs (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 10 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_slugs",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/railgunam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_railgun",
		/* pickup */ "Slugs",
		10,
		IT_AMMO,
		WP_RAILGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the BFG weapon. Default: 15 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 15.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_bfg",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/bfgam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_bfg",
		/* pickup */ "Bfg Ammo",
		15,
		IT_AMMO,
		WP_BFG,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// HOLDABLE ITEMS
	//

	/*QUAKED holdable_teleporter (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Personal Teleporter holdable item. Teleports the player to a random spawnpoint, but at the cost of losing vital objectives such as flags or skulls.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 60. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Holdable items are one-use items which must be manually activated by the player. Players can carry only one holdable item at the time.
	
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"holdable_teleporter",
		"sound/items/holdable.wav",
		{ "models/powerups/holdable/teleporter.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/teleporter",
		/* pickup */ "Personal Teleporter",
		60,
		IT_HOLDABLE,
		HI_TELEPORTER,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED holdable_medkit (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Personal Medkit holdable item. Heals the player by 125 HP after activation, doesn't work if the player already has a health count higher than 125.
	The Guard rune boosts up its effect to 225 HP.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 60. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Holdable items are one-use items which must be manually activated by the player. Players can carry only one holdable item at the time.
	
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"holdable_medkit",
		"sound/items/holdable.wav",
		{
			"models/powerups/holdable/medkit.md3",
			"models/powerups/holdable/medkit_sphere.md3",
			NULL, NULL
		},
		/* icon */ "icons/medkit",
		/* pickup */ "Medkit",
		60,
		IT_HOLDABLE,
		HI_MEDKIT,
		/* precache */ "",
		/* sounds */ "sound/items/use_medkit.wav"
	},

	//
	// POWERUPS
	//

	/*QUAKED item_quad (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Quad Damage powerup. Despite what its name says, the weapons do 3x damage under the effect of this powerup. It also partially nullifies the Invisibility effect. If used together with the Doubler rune, the damage goes up to 6x.
        By default, it lasts 30 seconds and takes 120 seconds for respawning.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 120. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
        count: How many seconds the powerup effect lasts. Default: 30.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Powerups are instantly activated and have a limited time, after which they wear out. Players can carry more than one powerup at a time. Be cautious with their placement. You may not want a player to become practically unbeatable for several seconds.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. This also counts for the powerup duration time. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
         */
	{
		"item_quad",
		"sound/items/quaddamage.wav",
		{ "models/powerups/instant/quad.md3",
			"models/powerups/instant/quad_ring.md3",
			NULL, NULL},
		/* icon */ "icons/quad",
		/* pickup */ "Quad Damage",
		30,
		IT_POWERUP,
		PW_QUAD,
		/* precache */ "",
		/* sounds */ "sound/items/damage2.wav sound/items/damage3.wav"
	},

	/*QUAKED item_enviro (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Battle Suit powerup. Cuts damage by half, and prevents damage from slime, lava, drowning while underwater, and splash damage.
        By default, it lasts 30 seconds and takes 120 seconds for respawning.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 120. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
        count: How many seconds the powerup effect lasts. Default: 30.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Powerups are instantly activated and have a limited time, after which they wear out. Players can carry more than one powerup at a time. Be cautious with their placement. You may not want a player to become practically unbeatable for several seconds.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. This also counts for the powerup duration time. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_enviro",
		"sound/items/protect.wav",
		{ "models/powerups/instant/enviro.md3",
			"models/powerups/instant/enviro_ring.md3",
			NULL, NULL},
		/* icon */ "icons/envirosuit",
		/* pickup */ "Battle Suit",
		30,
		IT_POWERUP,
		PW_BATTLESUIT,
		/* precache */ "",
		/* sounds */ "sound/items/airout.wav sound/items/protect3.wav"
	},

	/*QUAKED item_haste (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Haste powerup. When picked up, the player runs at 130% speed during its duration time. It also partially nullifies the Invisibility effect.
        By default, it lasts 30 seconds and takes 120 seconds for respawning.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 120. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
        count: How many seconds the powerup effect lasts. Default: 30.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Powerups are instantly activated and have a limited time, after which they wear out. Players can carry more than one powerup at a time. Be cautious with their placement. You may not want a player to become practically unbeatable for several seconds.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. This also counts for the powerup duration time. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_haste",
		"sound/items/haste.wav",
		{ "models/powerups/instant/haste.md3",
			"models/powerups/instant/haste_ring.md3",
			NULL, NULL},
		/* icon */ "icons/haste",
		/* pickup */ "Speed",
		30,
		IT_POWERUP,
		PW_HASTE,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_invis (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Invisibility powerup. Like its name implies, it turns a player invisible. The effect is partially nullified by Quad Damage and Haste effects.
        By default, it lasts 30 seconds and takes 120 seconds for respawning.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 120. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
        count: How many seconds the powerup effect lasts. Default: 30.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Powerups are instantly activated and have a limited time, after which they wear out. Players can carry more than one powerup at a time. Be cautious with their placement. You may not want a player to become practically unbeatable for several seconds.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. This also counts for the powerup duration time. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_invis",
		"sound/items/invisibility.wav",
		{ "models/powerups/instant/invis.md3",
			"models/powerups/instant/invis_ring.md3",
			NULL, NULL},
		/* icon */ "icons/invis",
		/* pickup */ "Invisibility",
		30,
		IT_POWERUP,
		PW_INVIS,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_regen (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Regeneration powerup. Boosts the holder's health and armor by a rate of 5HP and 5AP per second up to a total of 200 or until the item's duration is finished, whatever happens first. After that, both slowly wears out to 100.
        By default, it lasts 30 seconds and takes 120 seconds for respawning.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 120. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
        count: How many seconds the powerup effect lasts. Default: 30.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Powerups are instantly activated and have a limited time, after which they wear out. Players can carry more than one powerup at a time. Be cautious with their placement. You may not want a player to become practically unbeatable for several seconds.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. This also counts for the powerup duration time. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_regen",
		"sound/items/regeneration.wav",
		{ "models/powerups/instant/regen.md3",
			"models/powerups/instant/regen_ring.md3",
			NULL, NULL},
		/* icon */ "icons/regen",
		/* pickup */ "Regeneration",
		30,
		IT_POWERUP,
		PW_REGEN,
		/* precache */ "",
		/* sounds */ "sound/items/regen.wav"
	},

	/*QUAKED item_flight (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
        Flight powerup. Allows the user to fly all over the arena until it wears off. The downside is that you cannot use jump and launchpads and gain speed by strafing.
        By default, it lasts 60 seconds and takes 120 seconds for respawning.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 120. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
        count: How many seconds the powerup effect lasts. Default: 60.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Powerups are instantly activated and have a limited time, after which they wear out. Players can carry more than one powerup at a time. Be cautious with their placement. You may not want a player to become practically unbeatable for several seconds.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. This also counts for the powerup duration time. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_flight",
		"sound/items/flight.wav",
		{ "models/powerups/instant/flight.md3",
			"models/powerups/instant/flight_ring.md3",
			NULL, NULL},
		/* icon */ "icons/flight",
		/* pickup */ "Flight",
		60,
		IT_POWERUP,
		PW_FLIGHT,
		/* precache */ "",
		/* sounds */ "sound/items/flight.wav"
	},

	//
	// GAME OBJECTIVES
	//

	/*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
	Red flag. Used in Capture The Flag, One Flag CTF, CTF Elimination and Double Domination (to mark the A point).
	-------- KEYS --------
	target: Activates these entities when picked up.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- NOTES --------
	First and foremost, refrain from placing vital game objectives in zones only reachable with trickjumping. That creates an unnecessary imbalance, giving an unfair advantage to humans. Key game objectives being essential items to their gametypes must be reachable on foot by everyone, regardless of skill level.
        
        Also, take care with the gametype and !gametype keys. Your map might not be able to be playable at all on some mods.
	 */
	{
		"team_CTF_redflag",
		NULL,
		{ "models/flags/r_flag.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconf_red1",
		/* pickup */ "Red Flag",
		0,
		IT_TEAM,
		PW_REDFLAG,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
	Blue flag. Used in Capture The Flag, One Flag CTF, CTF Elimination and Double Domination (to mark the B point).
	-------- KEYS --------
	target: Activates these entities when picked up.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- NOTES --------
	First and foremost, refrain from placing vital game objectives in zones only reachable with trickjumping. That creates an unnecessary imbalance, giving an unfair advantage to humans. Key game objectives being essential items to their gametypes must be reachable on foot by everyone, regardless of skill level.
        
        Also, take care with the gametype and !gametype keys. Your map might not be able to be playable at all on some mods.
	 */
	{
		"team_CTF_blueflag",
		NULL,
		{ "models/flags/b_flag.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconf_blu1",
		/* pickup */ "Blue Flag",
		0,
		IT_TEAM,
		PW_BLUEFLAG,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// MISSIONPACK HOLDABLE ITEMS
	//

	/*QUAKED holdable_kamikaze (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Kamikaze holdable item. Blows the player up creating an earthquake across the arena and a big explosion which frags anyone in its radius. If the holder is fragged but not gibbed, then after 3 seconds it causes the same explosion. The explosion can also push and damage allies.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 60. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Holdable items are one-use items which must be manually activated by the player. Players can carry only one holdable item at the time.
	
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"holdable_kamikaze",
		"sound/items/holdable.wav",
		{ "models/powerups/kamikazi.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/kamikaze",
		/* pickup */ "Kamikaze",
		60,
		IT_HOLDABLE,
		HI_KAMIKAZE,
		/* precache */ "",
		/* sounds */ "sound/items/kamikazerespawn.wav"
	},

	/*holdable_portal (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Personal Portal holdable item. Players can spawn two of them, source and destination. Works like a portable teleporter, though it's broken.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 60. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Holdable items are one-use items which must be manually activated by the player. Players can carry only one holdable item at the time.
	
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"holdable_portal",
		"sound/items/holdable.wav",
		{ "models/powerups/holdable/porter.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/portal",
		/* pickup */ "Portal",
		60,
		IT_HOLDABLE,
		HI_PORTAL,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED holdable_invulnerability (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Invulnerability holdable item. Stops the player from moving (can only look around and shoot) and creates a forcefield around them. Mines can bypass the shield, if one hits the protected player, then they explode and fill the field with blood.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 60. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	Holdable items are one-use items which must be manually activated by the player. Players can carry only one holdable item at the time.
	
	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"holdable_invulnerability",
		"sound/items/holdable.wav",
		{ "models/powerups/holdable/invulnerability.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/invulnerability",
		/* pickup */ "Invulnerability",
		60,
		IT_HOLDABLE,
		HI_INVULNERABILITY,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// MISSIONPACK AMMO ITEMS
	//

	/*QUAKED ammo_nails (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 20 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 20.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_nails",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/nailgunam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_nailgun",
		/* pickup */ "Nails",
		20,
		IT_AMMO,
		WP_NAILGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_mines (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 10 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_mines",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/proxmineam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_proxlauncher",
		/* pickup */ "Proximity Mines",
		10,
		IT_AMMO,
		WP_PROX_LAUNCHER,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED ammo_belt (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Ammo box for the Chaingun weapon. Default: 100 ammo.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo given by the item. Default: 100.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	First and foremost: It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"ammo_belt",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/ammo/chaingunam.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_chaingun",
		/* pickup */ "Chaingun Belt",
		100,
		IT_AMMO,
		WP_CHAINGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// RUNES (a.k.a. Persistant Powerups)
	//

	/*QUAKED item_scout (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
        Scout rune. Boosts the player speed by a rate of 2x, and increases the rate of fire for all weapons. The downside is that the players cannot pick up armor items, and Haste doesn't stack up for extra speed.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 0. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        REDTEAM: Only players belonging to the Red Team can pick up this item.
        BLUETEAM: Only players belonging to the Blue Team can pick up this item.
        -------- NOTES --------
	Runes are meant to respawn instantly. They can be placed on each team's base and set so only players from that specific team can pick them up, or set without the team flags so any player can pick them up.
        
        It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
 */
	{
		"item_scout",
		"sound/items/scout.wav",
		{ "models/powerups/scout.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/scout",
		/* pickup */ "Scout",
		30,
		IT_PERSISTANT_POWERUP,
		PW_SCOUT,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_guard (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
        Guard rune. Gives the holder 200 HP and 200 AP. Health is regenerated to this maximum if below. If coupled with the Portable Medkit, the holdable's effect is boosted up to 225 HP.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 0. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        REDTEAM: Only players belonging to the Red Team can pick up this item.
        BLUETEAM: Only players belonging to the Blue Team can pick up this item.
        -------- NOTES --------
	Runes are meant to respawn instantly. They can be placed on each team's base and set so only players from that specific team can pick them up, or set without the team flags so any player can pick them up.
        
        It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_guard",
		"sound/items/guard.wav",
		{ "models/powerups/guard.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/guard",
		/* pickup */ "Guard",
		30,
		IT_PERSISTANT_POWERUP,
		PW_GUARD,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_doubler (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
        Doubler rune. Doubles the damage of the player's weapons. If used together with the Quad Damage powerup, the damage goes up to 6x.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 0. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        REDTEAM: Only players belonging to the Red Team can pick up this item.
        BLUETEAM: Only players belonging to the Blue Team can pick up this item.
        -------- NOTES --------
	Runes are meant to respawn instantly. They can be placed on each team's base and set so only players from that specific team can pick them up, or set without the team flags so any player can pick them up.
        
        It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_doubler",
		"sound/items/doubler.wav",
		{ "models/powerups/doubler.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/doubler",
		/* pickup */ "Doubler",
		30,
		IT_PERSISTANT_POWERUP,
		PW_DOUBLER,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED item_ammoregen (.3 .3 1) (-16 -16 -16) (16 16 16) suspended redTeam blueTeam
        Ammo Regen rune. Regenerates ammo for every weapon the player possess.
        -------- KEYS --------
	wait: Seconds before the item respawns. Default: 0. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
        -------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
        REDTEAM: Only players belonging to the Red Team can pick up this item.
        BLUETEAM: Only players belonging to the Blue Team can pick up this item.
        -------- NOTES --------
	Runes are meant to respawn instantly. They can be placed on each team's base and set so only players from that specific team can pick them up, or set without the team flags so any player can pick them up.
        
        It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"item_ammoregen",
		"sound/items/ammoregen.wav",
		{ "models/powerups/ammo.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/ammo_regen",
		/* pickup */ "Ammo Regen",
		30,
		IT_PERSISTANT_POWERUP,
		PW_AMMOREGEN,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// MISSIONPACK GAME OBJECTIVES
	//

	/*QUAKED team_CTF_neutralflag (0 0 1) (-16 -16 -16) (16 16 16)
	Neutral flag. Used in One Flag CTF and Possession.
	-------- KEYS --------
	target: Activates these entities when picked up.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- NOTES --------
	First and foremost, refrain from placing vital game objectives in zones only reachable with trickjumping. That creates an unnecessary imbalance, giving an unfair advantage to humans. Key game objectives being essential items to their gametypes must be reachable on foot by everyone, regardless of skill level.
        
        Also, take care with the gametype and !gametype keys. Your map might not be able to be playable at all on some mods.
        */
	{
		"team_CTF_neutralflag",
		NULL,
		{ "models/flags/n_flag.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconf_neutral1",
		/* pickup */ "Neutral Flag",
		0,
		IT_TEAM,
		PW_NEUTRALFLAG,
		/* precache */ "",
		/* sounds */ ""
	},

	//
	// MISSIONPACK UNUSABLE GAME OBJECTIVES
	//

	/* item_redcube - NOT USED IN THE EDITOR
	Red Skull for Harvester	*/
	{
		"item_redcube",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/orb/r_orb.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconh_rorb",
		/* pickup */ "Red Cube",
		0,
		IT_TEAM,
		0,
		/* precache */ "",
		/* sounds */ ""
	},

	/* item_bluecube - NOT USED IN THE EDITOR
	Blue Skull for Harvester	*/
	{
		"item_bluecube",
		"sound/misc/am_pkup.wav",
		{ "models/powerups/orb/b_orb.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconh_borb",
		/* pickup */ "Blue Cube",
		0,
		IT_TEAM,
		0,
		/* precache */ "",
		/* sounds */ ""
	},
	
	//
	// MISSIONPACK WEAPONS 
	//

	/*QUAKED weapon_nailgun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Nailgun weapon. Comes with 10 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 10.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	If you don't plan on adding the Runes, treat the weapon as a Superweapon on par with the BFG.

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_nailgun",
		"sound/misc/w_pkup.wav",
		{ "models/weapons/nailgun/nailgun.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_nailgun",
		/* pickup */ "Nailgun",
		10,
		IT_WEAPON,
		WP_NAILGUN,
		/* precache */ "",
		/* sounds */ ""
	},

	/*QUAKED weapon_prox_launcher (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Prox Launcher weapon. Comes with 5 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 5.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	The Prox Launcher is used to counteract the Invulnerability sphere, as only mines can trespass the Invul shield. Mines can be triggered with any other weapon which produces splash damage. You might want to bear this in mind when including either the PL or the Invul on your maps. 

	If you don't plan on adding the Runes, treat the weapon as a Superweapon on par with the BFG.

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_prox_launcher",
		"sound/misc/w_pkup.wav",
		{ "models/weapons/proxmine/proxmine.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_proxlauncher",
		/* pickup */ "Prox Launcher",
		5,
		IT_WEAPON,
		WP_PROX_LAUNCHER,
		/* precache */ "",
		/* sounds */ "sound/weapons/proxmine/wstbtick.wav "
		"sound/weapons/proxmine/wstbactv.wav "
		"sound/weapons/proxmine/wstbimpl.wav "
		"sound/weapons/proxmine/wstbimpm.wav "
		"sound/weapons/proxmine/wstbimpd.wav "
		"sound/weapons/proxmine/wstbactv.wav"
	},

	/*QUAKED weapon_chaingun (.3 .3 1) (-16 -16 -16) (16 16 16) suspended
	Chaingun weapon. Comes with 80 ammo units.
	-------- KEYS --------
	wait: Seconds before the item respawns. Default: 40. Set to -1 for never respawn.
	random: How many seconds the respawn time varies from the Respawn Delay value. Default: 0. See Notes.
	count: Quantity of ammo preloaded in the weapon. If the player already has that amount or more, he will get only one more, except in TDM mode, where he will still get the full amount. Default: 80.
	team: Makes the item spawn randomly with other items with the same "team" value. See Notes.
	target: When this item is picked up, it triggers the entity specified here, if set.
	targetname: If used as destination for a target_give, the item can be given to players (e.g. at player spawn time).
	notbot: If set to 1, the entity won't attract bots.
	notfree: If set to 1, prevents the entity from spawning in all non-team-based modes (ffa, single player deathmatch, etc.).
	notteam: If set to 1, prevents the entity from spawning in all team-based modes (ctf, 1flag, dom, etc.).
	notsingle: If set to 1, prevents the entity from spawning in Single Player Deathmatch mode (g_gametype 2) only.
	gametype: If set, makes the entity spawn ONLY in the specified gametypes. Separate gametypes with commas. Valid values: ffa, tournament, single, team, ctf, oneflag, obelisk, harvester, elimination, ctfelimination, lms, dd, dom, pos.
	!gametype: Usage is similar to Game Type key, with opposite effect. If set, causes the entity to NOT spawn in the specified gametypes. Not supported in gamecode older than OA 0.8.8: entity will always spawn in old mods for Q3A!
	-------- SPAWNFLAGS --------
	SUSPENDED: If set to 1, the item will remain floating instead of appearing on the ground. Bots do not look for "suspended" items, unless they are reachable by jumppads or in water.
	-------- NOTES --------
	If you don't plan on adding the Runes, treat the weapon as a Superweapon on par with the BFG.

	It is NOT advisable to change the respawn time of an item, or the amount of ammo given by a weapon/box, because you are messing with the player's natural timing of the items. You're doing well with the default values. That said...

	The "wait" value of a "teamed" item will determine the respawn time of the following one. Setting one of the items to -1 (no respawn) will prevent further items from spawning, after that one.

	Using "random" key will make the respawn time variable, to the maximum limits given by "random" added or subtracted to "wait" seconds. Example: "wait 10" with "random 2" will make the item respawn between 8 and 12 seconds of delay.
	 */
	{
		"weapon_chaingun",
		"sound/misc/w_pkup.wav",
		{ "models/weapons/vulcan/vulcan.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconw_chaingun",
		/* pickup */ "Chaingun",
		80,
		IT_WEAPON,
		WP_CHAINGUN,
		/* precache */ "",
		/* sounds */ "sound/weapons/vulcan/wvulwind.wav"
	},

	//
	// UNUSED EDITOR ENTITIES
	//

	/* team_DD_pointAblue - NOT USED IN THE EDITOR
	Only in DD games
	 */
	{
		"team_DD_pointAblue",
		NULL,
		{ "models/dpoints/a_blue.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_blue",
		/* pickup */ "Point A (Blue)",
		0,
		IT_TEAM,
		DD_POINTABLUE,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_DD_pointBblue - NOT USED IN THE EDITOR
	Only in DD games
	 */
	{
		"team_DD_pointBblue",
		NULL,
		{ "models/dpoints/b_blue.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconb_blue",
		/* pickup */ "Point B (Blue)",
		0,
		IT_TEAM,
		DD_POINTBBLUE,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_DD_pointAred - NOT USED IN THE EDITOR
	Only in DD games
	 */
	{
		"team_DD_pointAred",
		NULL,
		{ "models/dpoints/a_red.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_red",
		/* pickup */ "Point A (Red)",
		0,
		IT_TEAM,
		DD_POINTARED,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_DD_pointBred - NOT USED IN THE EDITOR
	Only in DD games
	 */
	{
		"team_DD_pointBred",
		NULL,
		{ "models/dpoints/b_red.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconb_red",
		/* pickup */ "Point B (Red)",
		0,
		IT_TEAM,
		DD_POINTBRED,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_DD_pointAwhite - NOT USED IN THE EDITOR
	Only in DD games
	 */
	{
		"team_DD_pointAwhite",
		NULL,
		{ "models/dpoints/a_white.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/icona_white",
		/* pickup */ "Point A (White)",
		0,
		IT_TEAM,
		DD_POINTAWHITE,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_DD_pointBwhite - NOT USED IN THE EDITOR
	Only in DD games
	 */
	{
		"team_DD_pointBwhite",
		NULL,
		{ "models/dpoints/b_white.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconb_white",
		/* pickup */ "Point B (White)",
		0,
		IT_TEAM,
		DD_POINTBWHITE,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_dom_pointWhite - NOT USED IN THE EDITOR
	Untaken DOM point.
	Only in Domination games
	 */
	{
		"team_dom_pointWhite",
		NULL,
		{ "models/flags/n_flag.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconf_neutral1",
		/* pickup */ "Neutral domination point",
		0,
		IT_TEAM,
		DOM_POINTWHITE,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_dom_pointRed - NOT USED IN THE EDITOR
	Red Team-owned DOM point.
	Only in Domination games
	 */
	{
		"team_dom_pointRed",
		NULL,
		{ "models/flags/r_flag.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconf_red1",
		/* pickup */ "Red domination point",
		0,
		IT_TEAM,
		DOM_POINTRED,
		/* precache */ "",
		/* sounds */ ""
	},

	/* team_dom_pointBlue - NOT USED IN THE EDITOR
	Blue Team-owned DOM point.
	Only in Domination games
	 */
	{
		"team_dom_pointBlue",
		NULL,
		{ "models/flags/b_flag.md3",
			NULL, NULL, NULL},
		/* icon */ "icons/iconf_blu1",
		/* pickup */ "Blue domination point",
		0,
		IT_TEAM,
		DOM_POINTBLUE,
		/* precache */ "",
		/* sounds */ ""
	},
	// end of list marker
	{NULL}
};

int bg_numItems = sizeof (bg_itemlist) / sizeof (bg_itemlist[0]) - 1;

/*
==============
BG_FindItemForPowerup
==============
 */
gitem_t *BG_FindItemForPowerup(powerup_t pw) {
	int i;

	for (i = 0; i < bg_numItems; i++) {
		if ((bg_itemlist[i].giType == IT_POWERUP ||
				bg_itemlist[i].giType == IT_TEAM ||
				bg_itemlist[i].giType == IT_PERSISTANT_POWERUP) &&
				bg_itemlist[i].giTag == pw) {
			return &bg_itemlist[i];
		}
	}

	return NULL;
}

/*
==============
BG_FindItemForHoldable
==============
 */
gitem_t *BG_FindItemForHoldable(holdable_t pw) {
	int i;

	for (i = 0; i < bg_numItems; i++) {
		if (bg_itemlist[i].giType == IT_HOLDABLE && bg_itemlist[i].giTag == pw) {
			return &bg_itemlist[i];
		}
	}

	Com_Error(ERR_DROP, "HoldableItem not found");

	return NULL;
}

/*
===============
BG_FindItemForWeapon

===============
 */
gitem_t *BG_FindItemForWeapon(weapon_t weapon) {
	gitem_t *it;

	for (it = bg_itemlist + 1; it->classname; it++) {
		if (it->giType == IT_WEAPON && it->giTag == weapon) {
			return it;
		}
	}

	Com_Error(ERR_DROP, "Couldn't find item for weapon %i", weapon);
	return NULL;
}

/*
===============
BG_FindItem

===============
 */
gitem_t *BG_FindItem(const char *pickupName) {
	gitem_t *it;

	for (it = bg_itemlist + 1; it->classname; it++) {
		if (Q_strequal(it->pickup_name, pickupName))
			return it;
	}

	return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
 */
qboolean BG_PlayerTouchesItem(playerState_t *ps, entityState_t *item, int atTime) {
	vec3_t origin;

	BG_EvaluateTrajectory(&item->pos, atTime, origin);

	// we are ignoring ducked differences here
	if (ps->origin[0] - origin[0] > 44
			|| ps->origin[0] - origin[0] < -50
			|| ps->origin[1] - origin[1] > 36
			|| ps->origin[1] - origin[1] < -36
			|| ps->origin[2] - origin[2] > 36
			|| ps->origin[2] - origin[2] < -36) {
		return qfalse;
	}

	return qtrue;
}

/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
 */
qboolean BG_CanItemBeGrabbed(int gametype, const entityState_t *ent, const playerState_t *ps) {
	gitem_t *item;
	int upperBound;

	if (ent->modelindex < 1 || ent->modelindex >= bg_numItems) {
		Com_Error(ERR_DROP, "BG_CanItemBeGrabbed: index out of range");
	}

	item = &bg_itemlist[ent->modelindex];

	switch (item->giType) {
		case IT_WEAPON:
			return qtrue; // weapons are always picked up

		case IT_AMMO:
			if (ps->ammo[ item->giTag ] >= 200) {
				return qfalse; // can't hold any more
			}
			return qtrue;

		case IT_ARMOR:
			if (bg_itemlist[ps->stats[STAT_PERSISTANT_POWERUP]].giTag == PW_SCOUT) {
				return qfalse;
			}

			// we also clamp armor to the maxhealth for handicapping
			if (bg_itemlist[ps->stats[STAT_PERSISTANT_POWERUP]].giTag == PW_GUARD) {
				upperBound = ps->stats[STAT_MAX_HEALTH];
			} else {
				upperBound = ps->stats[STAT_MAX_HEALTH] * 2;
			}

			if (ps->stats[STAT_ARMOR] >= upperBound) {
				return qfalse;
			}
			return qtrue;

		case IT_HEALTH:
			// small and mega healths will go over the max, otherwise
			// don't pick up if already at max
			if (bg_itemlist[ps->stats[STAT_PERSISTANT_POWERUP]].giTag == PW_GUARD) {
				upperBound = ps->stats[STAT_MAX_HEALTH];
			} else
				if (item->quantity == 5 || item->quantity == 100) {
				if (ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] * 2) {
					return qfalse;
				}
				return qtrue;
			}

			if (ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH]) {
				return qfalse;
			}
			return qtrue;

		case IT_POWERUP:
			return qtrue; // powerups are always picked up

		case IT_PERSISTANT_POWERUP:

			//In Double D we don't want persistant Powerups (or maybe, can be discussed)
			if (gametype == GT_DOUBLE_D)
				return qfalse;

			// can only hold one item at a time
			if (ps->stats[STAT_PERSISTANT_POWERUP]) {
				return qfalse;
			}

			// check team only
			if ((ent->generic1 & 2) && (ps->persistant[PERS_TEAM] != TEAM_RED)) {
				return qfalse;
			}
			if ((ent->generic1 & 4) && (ps->persistant[PERS_TEAM] != TEAM_BLUE)) {
				return qfalse;
			}

			return qtrue;

		case IT_TEAM: // team items, such as flags	
			if (gametype == GT_POSSESSION) {
				if (item->giTag == PW_NEUTRALFLAG) {
					return qtrue;
				}
			}
			if (gametype == GT_1FCTF) {
				// neutral flag can always be picked up
				if (item->giTag == PW_NEUTRALFLAG) {
					return qtrue;
				}
				if (ps->persistant[PERS_TEAM] == TEAM_RED) {
					if (item->giTag == PW_BLUEFLAG && ps->powerups[PW_NEUTRALFLAG]) {
						return qtrue;
					}
				} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
					if (item->giTag == PW_REDFLAG && ps->powerups[PW_NEUTRALFLAG]) {
						return qtrue;
					}
				}
			}
			if (gametype == GT_CTF || gametype == GT_CTF_ELIMINATION) {
				// ent->modelindex2 is non-zero on items if they are dropped
				// we need to know this because we can pick up our dropped flag (and return it)
				// but we can't pick up our flag at base
				if (ps->persistant[PERS_TEAM] == TEAM_RED) {
					if (item->giTag == PW_BLUEFLAG ||
							(item->giTag == PW_REDFLAG && ent->modelindex2) ||
							(item->giTag == PW_REDFLAG && ps->powerups[PW_BLUEFLAG]))
						return qtrue;
				} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
					if (item->giTag == PW_REDFLAG ||
							(item->giTag == PW_BLUEFLAG && ent->modelindex2) ||
							(item->giTag == PW_BLUEFLAG && ps->powerups[PW_REDFLAG]))
						return qtrue;
				}
			}

			if (gametype == GT_DOUBLE_D) {
				//We can touch both flags
				if (item->giTag == PW_BLUEFLAG || item->giTag == PW_REDFLAG)
					return qtrue;
			}

			if (gametype == GT_DOMINATION) {
				if (item->giTag == DOM_POINTWHITE)
					return qtrue;
				if (ps->persistant[PERS_TEAM] == TEAM_RED) {
					if (item->giTag == DOM_POINTBLUE)
						return qtrue;
				} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
					if (item->giTag == DOM_POINTRED)
						return qtrue;
				}
			}

			if (gametype == GT_HARVESTER) {
				return qtrue;
			}
			return qfalse;

		case IT_HOLDABLE:
			// can only hold one item at a time
			if (ps->stats[STAT_HOLDABLE_ITEM]) {
				return qfalse;
			}
			return qtrue;

		case IT_BAD:
			Com_Error(ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD");
		default:
#ifndef Q3_VM
#ifndef NDEBUG // bk0001204
			Com_Printf("BG_CanItemBeGrabbed: unknown enum %d\n", item->giType);
#endif
#endif
			break;
	}

	return qfalse;
}

//======================================================================

/*
================
BG_EvaluateTrajectory

================
 */
void BG_EvaluateTrajectory(const trajectory_t *tr, int atTime, vec3_t result) {
	float deltaTime;
	float phase;

	switch (tr->trType) {
		case TR_STATIONARY:
		case TR_INTERPOLATE:
			VectorCopy(tr->trBase, result);
			break;
		case TR_LINEAR:
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
			break;
		case TR_SINE:
			deltaTime = (atTime - tr->trTime) / (float) tr->trDuration;
			phase = sin(deltaTime * M_PI * 2);
			VectorMA(tr->trBase, phase, tr->trDelta, result);
			break;
		case TR_LINEAR_STOP:
			if (atTime > tr->trTime + tr->trDuration) {
				atTime = tr->trTime + tr->trDuration;
			}
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			if (deltaTime < 0) {
				deltaTime = 0;
			}
			VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
			break;
		case TR_GRAVITY:
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
			result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime; // FIXME: local gravity...
			break;
		default:
			Com_Error(ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime);
			break;
	}
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
 */
void BG_EvaluateTrajectoryDelta(const trajectory_t *tr, int atTime, vec3_t result) {
	float deltaTime;
	float phase;

	switch (tr->trType) {
		case TR_STATIONARY:
		case TR_INTERPOLATE:
			VectorClear(result);
			break;
		case TR_LINEAR:
			VectorCopy(tr->trDelta, result);
			break;
		case TR_SINE:
			deltaTime = (atTime - tr->trTime) / (float) tr->trDuration;
			phase = cos(deltaTime * M_PI * 2); // derivative of sin = cos
			phase *= 0.5;
			VectorScale(tr->trDelta, phase, result);
			break;
		case TR_LINEAR_STOP:
			if (atTime > tr->trTime + tr->trDuration) {
				VectorClear(result);
				return;
			}
			VectorCopy(tr->trDelta, result);
			break;
		case TR_GRAVITY:
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			VectorCopy(tr->trDelta, result);
			result[2] -= DEFAULT_GRAVITY * deltaTime; // FIXME: local gravity...
			break;
		default:
			Com_Error(ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime);
			break;
	}
}

char *eventnames[] = {
	"EV_NONE",

	"EV_FOOTSTEP",
	"EV_FOOTSTEP_METAL",
	"EV_FOOTSPLASH",
	"EV_FOOTWADE",
	"EV_SWIM",

	"EV_STEP_4",
	"EV_STEP_8",
	"EV_STEP_12",
	"EV_STEP_16",

	"EV_FALL_SHORT",
	"EV_FALL_MEDIUM",
	"EV_FALL_FAR",

	"EV_JUMP_PAD", // boing sound at origin", jump sound on player

	"EV_JUMP",
	"EV_WATER_TOUCH", // foot touches
	"EV_WATER_LEAVE", // foot leaves
	"EV_WATER_UNDER", // head touches
	"EV_WATER_CLEAR", // head leaves

	"EV_ITEM_PICKUP", // normal item pickups are predictable
	"EV_GLOBAL_ITEM_PICKUP", // powerup / team sounds are broadcast to everyone

	"EV_NOAMMO",
	"EV_CHANGE_WEAPON",
	"EV_FIRE_WEAPON",

	"EV_USE_ITEM0",
	"EV_USE_ITEM1",
	"EV_USE_ITEM2",
	"EV_USE_ITEM3",
	"EV_USE_ITEM4",
	"EV_USE_ITEM5",
	"EV_USE_ITEM6",
	"EV_USE_ITEM7",
	"EV_USE_ITEM8",
	"EV_USE_ITEM9",
	"EV_USE_ITEM10",
	"EV_USE_ITEM11",
	"EV_USE_ITEM12",
	"EV_USE_ITEM13",
	"EV_USE_ITEM14",
	"EV_USE_ITEM15",

	"EV_ITEM_RESPAWN",
	"EV_ITEM_POP",
	"EV_PLAYER_TELEPORT_IN",
	"EV_PLAYER_TELEPORT_OUT",

	"EV_GRENADE_BOUNCE", // eventParm will be the soundindex

	"EV_GENERAL_SOUND",
	"EV_GLOBAL_SOUND", // no attenuation
	"EV_GLOBAL_TEAM_SOUND",

	"EV_BULLET_HIT_FLESH",
	"EV_BULLET_HIT_WALL",

	"EV_MISSILE_HIT",
	"EV_MISSILE_MISS",
	"EV_MISSILE_MISS_METAL",
	"EV_RAILTRAIL",
	"EV_SHOTGUN",
	"EV_BULLET", // otherEntity is the shooter

	"EV_PAIN",
	"EV_DEATH1",
	"EV_DEATH2",
	"EV_DEATH3",
	"EV_OBITUARY",

	"EV_POWERUP_QUAD",
	"EV_POWERUP_BATTLESUIT",
	"EV_POWERUP_REGEN",

	"EV_GIB_PLAYER", // gib a previously living player
	"EV_SCOREPLUM", // score plum

	//Not all of these are used in baseoa but we keep them to gurantie event numbers between version
	"EV_PROXIMITY_MINE_STICK",
	"EV_PROXIMITY_MINE_TRIGGER",
	"EV_KAMIKAZE", // kamikaze explodes
	"EV_OBELISKEXPLODE", // obelisk explodes
	"EV_INVUL_IMPACT", // invulnerability sphere impact
	"EV_JUICED", // invulnerability juiced effect
	"EV_LIGHTNINGBOLT", // lightning bolt bounced of invulnerability sphere


	"EV_DEBUG_LINE",
	"EV_STOPLOOPINGSOUND",
	"EV_TAUNT"

};

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
 */

void trap_Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);

void BG_AddPredictableEventToPlayerstate(int newEvent, int eventParm, playerState_t *ps) {

#ifdef _DEBUG
	{
		char buf[256];
		trap_Cvar_VariableStringBuffer("showevents", buf, sizeof (buf));
		if (atof(buf) != 0) {
#ifdef QAGAME
			Com_Printf(" game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
			Com_Printf("Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
		}
	}
#endif
	ps->events[ps->eventSequence & (MAX_PS_EVENTS - 1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS - 1)] = eventParm;
	ps->eventSequence++;
}

/*
========================
BG_TouchJumpPad
========================
 */
void BG_TouchJumpPad(playerState_t *ps, entityState_t *jumppad) {
	vec3_t angles;
	float p;
	int effectNum;

	// spectators don't use jump pads
	if (ps->pm_type != PM_NORMAL) {
		return;
	}

	// flying characters don't hit bounce pads
	if (ps->powerups[PW_FLIGHT]) {
		return;
	}

	// if we didn't hit this same jumppad the previous frame
	// then don't play the event sound again if we are in a fat trigger
	if (ps->jumppad_ent != jumppad->number) {

		vectoangles(jumppad->origin2, angles);
		p = fabs(AngleNormalize180(angles[PITCH]));
		if (p < 45) {
			effectNum = 0;
		} else {
			effectNum = 1;
		}
		BG_AddPredictableEventToPlayerstate(EV_JUMP_PAD, effectNum, ps);
	}
	// remember hitting this jumppad this frame
	ps->jumppad_ent = jumppad->number;
	ps->jumppad_frame = ps->pmove_framecount;
	// give the player the velocity from the jumppad
	VectorCopy(jumppad->origin2, ps->velocity);
}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
 */
void BG_PlayerStateToEntityState(playerState_t *ps, entityState_t *s, qboolean snap) {
	int i;

	if (ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR) {
		s->eType = ET_INVISIBLE;
	} else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy(ps->origin, s->pos.trBase);
	if (snap) {
		SnapVector(s->pos.trBase);
	}
	// set the trDelta for flag direction
	VectorCopy(ps->velocity, s->pos.trDelta);

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy(ps->viewangles, s->apos.trBase);
	if (snap) {
		SnapVector(s->apos.trBase);
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum; // ET_PLAYER looks here instead of at number
	// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if (ps->stats[STAT_HEALTH] <= 0) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if (ps->externalEvent) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if (ps->entityEventSequence < ps->eventSequence) {
		int seq;

		if (ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS - 1);
		s->event = ps->events[ seq ] | ((ps->entityEventSequence & 3) << 8);
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (ps->powerups[ i ]) {
			s->powerups |= 1 << i;
		}
	}

	s->loopSound = ps->loopSound;
	s->generic1 = ps->generic1;
}

/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
 */
void BG_PlayerStateToEntityStateExtraPolate(playerState_t *ps, entityState_t *s, int time, qboolean snap) {
	int i;

	if (ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR) {
		s->eType = ET_INVISIBLE;
	} else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;

	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy(ps->origin, s->pos.trBase);
	if (snap) {
		SnapVector(s->pos.trBase);
	}
	// set the trDelta for flag direction and linear prediction
	VectorCopy(ps->velocity, s->pos.trDelta);
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy(ps->viewangles, s->apos.trBase);
	if (snap) {
		SnapVector(s->apos.trBase);
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum; // ET_PLAYER looks here instead of at number
	// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if (ps->stats[STAT_HEALTH] <= 0) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if (ps->externalEvent) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if (ps->entityEventSequence < ps->eventSequence) {
		int seq;

		if (ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS - 1);
		s->event = ps->events[ seq ] | ((ps->entityEventSequence & 3) << 8);
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (ps->powerups[ i ]) {
			s->powerups |= 1 << i;
		}
	}

	s->loopSound = ps->loopSound;
	s->generic1 = ps->generic1;
}

/*
============
BG_TeamName
KK-OAX Copied from Tremulous
============
 */
const char *BG_TeamName(team_t team) {
	if (team == TEAM_NONE)
		return "spectator";
	if (team == TEAM_RED)
		return "Red";
	if (team == TEAM_BLUE)
		return "Blue";
	if (team == TEAM_FREE)
		return "Free For All";
	return "<team>";
}

int trap_FS_FOpenFile(const char *qpath, fileHandle_t *f, fsMode_t mode);
void trap_FS_Read(void *buffer, int len, fileHandle_t f);
void trap_FS_FCloseFile(fileHandle_t f);

qboolean MatchesGametype(int gametype, const char* gametypeName) {
	qboolean mayRead = qfalse;
	switch (gametype) {
		case GT_FFA:
			if (Q_strequal(gametypeName, "dm")) mayRead = qtrue;
			break;
		case GT_TEAM:
			if (Q_strequal(gametypeName, "team")) mayRead = qtrue;
			break;
		case GT_TOURNAMENT:
			if (Q_strequal(gametypeName, "tourney")) mayRead = qtrue;
			break;
		case GT_CTF:
			if (Q_strequal(gametypeName, "ctf")) mayRead = qtrue;
			break;
		case GT_1FCTF:
			if (Q_strequal(gametypeName, "1fctf")) mayRead = qtrue;
			break;
		case GT_OBELISK:
			if (Q_strequal(gametypeName, "obelisk")) mayRead = qtrue;
			break;
		case GT_HARVESTER:
			if (Q_strequal(gametypeName, "harvester")) mayRead = qtrue;
			break;
		case GT_ELIMINATION:
			if (Q_strequal(gametypeName, "elimination")) mayRead = qtrue;
			break;
		case GT_CTF_ELIMINATION:
			if (Q_strequal(gametypeName, "ctfelim")) mayRead = qtrue;
			break;
		case GT_LMS:
			if (Q_strequal(gametypeName, "lms")) mayRead = qtrue;
			break;
		case GT_DOUBLE_D:
			if (Q_strequal(gametypeName, "dd")) mayRead = qtrue;
			break;
		case GT_DOMINATION:
			if (Q_strequal(gametypeName, "dom")) mayRead = qtrue;
			break;
		case GT_POSSESSION:
			if (Q_strequal(gametypeName, "pos")) mayRead = qtrue;
			break;
	};
	return mayRead;
}

void MapInfoGet(const char* mapname, int gametype, mapinfo_result_t *result) {
	fileHandle_t file;
	char buffer[4 * 1024];
	char keyBuffer[MAX_TOKEN_CHARS];
	char *token;
	char *pointer;
	int mayRead;
	int i;

	memset(result, 0, sizeof (*result));
	Com_sprintf(buffer, sizeof (buffer), "maps/%s.info", mapname);
	Q_strlwr(buffer);

	trap_FS_FOpenFile(buffer, &file, FS_READ);

	if (!file) {
		Com_Printf("File %s not found\n", buffer);
		return;
	}

	memset(&buffer, 0, sizeof (buffer));

	trap_FS_Read(&buffer, sizeof (buffer), file);

	pointer = buffer;
	mayRead = qtrue;

	while (qtrue) {
		token = COM_Parse(&pointer);
		if (!token[0]) {
			break;
		}
		Q_strncpyz(keyBuffer, token, sizeof (keyBuffer));
		token = COM_Parse(&pointer);

		Com_Printf("Token \"%s\" found, with value: \"%s\"\n", keyBuffer, token);

		if (Q_strequal(keyBuffer, "gametype")) {
			mayRead = qfalse;
			if (!token[0]) {
				break;
			}
			if (Q_strequal(token, "*")) {
				mayRead = qtrue;
			}
			else {
				mayRead = MatchesGametype(gametype, token);
			}
		}
		if (Q_strequal(keyBuffer, "author")) {
			if (mayRead) Q_strncpyz(result->author, token, sizeof (result->author));
		}
		if (Q_strequal(keyBuffer, "description")) {
			if (mayRead) Q_strncpyz(result->description, token, sizeof (result->description));
		}
		if (Q_strequal(keyBuffer, "mpBots")) {
			if (mayRead) Q_strncpyz(result->mpBots, token, sizeof (result->mpBots));
		}
		if (Q_strequal(keyBuffer, "captureLimit")) {
			if (mayRead) result->captureLimit = atoi(token);
		}
		if (Q_strequal(keyBuffer, "fragLimit")) {
			if (mayRead) result->fragLimit = atoi(token);
		}
		if (Q_strequal(keyBuffer, "maxPlayers")) {
			if (mayRead) result->maxPlayers = atoi(token);
		}
		if (Q_strequal(keyBuffer, "maxTeamSize")) {
			if (mayRead) result->maxTeamSize = atoi(token);
		}
		if (Q_strequal(keyBuffer, "minPlayers")) {
			if (mayRead) result->minPlayers = atoi(token);
		}
		if (Q_strequal(keyBuffer, "minTeamSize")) {
			if (mayRead) result->minTeamSize = atoi(token);
		}
		if (Q_strequal(keyBuffer, "recommendedPlayers")) {
			if (mayRead) result->recommendedPlayers = atoi(token);
		}
		if (Q_strequal(keyBuffer, "timeLimit")) {
			if (mayRead) result->timeLimit = atoi(token);
		}
#define SUPPORT_GAMETYPE_PREFIX "support_"
		if (Q_strequaln(keyBuffer, SUPPORT_GAMETYPE_PREFIX, sizeof (SUPPORT_GAMETYPE_PREFIX) - 1)) {
			//Com_Printf("Saw %s, gametype part: %s\n", token, &token[sizeof(SUPPORT_GAMETYPE_PREFIX)-1]);
			for (i = 0; i < GT_MAX_GAME_TYPE; ++i) {
				if (MatchesGametype(i, &keyBuffer[sizeof (SUPPORT_GAMETYPE_PREFIX) - 1])) {
					//Com_Printf("Matched gametype %d, %s\n", i, token);
					result->gametypeSupported[i] = token[0];
					break;
				}
			}
		}

	}

	trap_FS_FCloseFile(file);
}

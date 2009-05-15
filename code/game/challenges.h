/*
===========================================================================
Copyright (C) 2008-2009 Poul Sander

This file is part of Open Arena source code.

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

/**
 * This is a list of challenges that a player can only complete while playing
 * against human opponents, either LAN or Internet.
 *
 * Hopefully this will make it a little more fun to play online even if you
 * are not good enough to win you can still track your progress.
 */

#define CHALLENGES_MAX_COUNT	MAX_INT-1
#define CHALLENGES_MAX          2048

#define GENERAL_TEST		0
#define GENERAL_TOTALKILLS      1
#define GENERAL_TOTALDEATHS     2
#define GENERAL_TOTALGAMES      3

//Gametypes
#define GAMETYPES_FFA_WINS      101
#define GAMETYPES_TOURNEY_WINS  102
#define GAMETYPES_TDM_WINS      103
#define GAMETYPES_CTF_WINS      104
#define GAMETYPES_1FCTF_WINS    105
#define GAMETYPES_OVERLOAD_WINS 106
#define GAMETYPES_HARVESTER_WINS 107
#define GAMETYPES_ELIMINATION_WINS 108
#define GAMETYPES_CTF_ELIMINATION_WINS 109
#define GAMETYPES_LMS_WINS      110
#define GAMETYPES_DD_WINS       111
#define GAMETYPES_DOM_WINS      112

//Weapons
#define WEAPON_GAUNTLET_KILLS   201
#define WEAPON_MACHINEGUN_KILLS 202
#define WEAPON_SHOTGUN_KILLS    203
#define WEAPON_GRANADE_KILLS    204
#define WEAPON_ROCKET_KILLS     205
#define WEAPON_LIGHTNING_KILLS  206
#define WEAPON_PLASMA_KILLS     207
#define WEAPON_RAIL_KILLS       208
#define WEAPON_BFG_KILLS        209
#define WEAPON_GRAPPLE_KILLS    210
#define WEAPON_CHAINGUN_KILLS   211
#define WEAPON_NAILGUN_KILLS    212
#define WEAPON_MINE_KILLS       213
#define WEAPON_PUSH_KILLS       214
#define WEAPON_INSTANT_RAIL_KILLS 215
#define WEAPON_TELEFRAG_KILLS   216
#define WEAPON_CRUSH_KILLS      217

//Awards
//Gauntlet is not here as it is the same as WEAPON_GAUNTLET_KILLS
#define AWARD_IMPRESSIVE        301
#define AWARD_EXCELLENT          302
#define AWARD_CAPTURE           303
#define AWARD_ASSIST            304
#define AWARD_DEFENCE           305

//Powerups
#define POWERUP_QUAD_KILL       401
#define POWERUP_SPEED_KILL      402
#define POWERUP_FLIGHT_KILL     403
#define POWERUP_INVIS_KILL      404
#define POWERUP_MULTI_KILL      405
#define POWERUP_COUNTER_QUAD    406
#define POWERUP_COUNTER_SPEED   407
#define POWERUP_COUNTER_FLIGHT  408
#define POWERUP_COUNTER_INVIS   409
#define POWERUP_COUNTER_ENVIR   410
#define POWERUP_COUNTER_REGEN   411
#define POWERUP_COUNTER_MULTI   412

//FFA awards
#define FFA_TOP3                501
//From behind, enemy gets fraglimit-1, player has at most fraglimit-2 but wins anyway
#define FFA_FROMBEHIND          502
//BetterThan: loose a match but have a positive kill ratio against the winner
#define FFA_BETTERTHAN          503
//Get at least half of your kills for players in the best half of the scoreboard
#define FFA_JUDGE               504
//The oppesite
#define FFA_CHEAPKILLER         505

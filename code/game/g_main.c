/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//

#include "g_local.h"
#include "bg_public.h"

level_locals_t	level;

typedef struct {
	vmCvar_t *vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	int			modificationCount;  // for tracking changes
	qboolean	trackChange;	    // track this variable, and announce if changed
	qboolean teamShader;        // track and if changed, update shader state
} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t		g_clients[MAX_CLIENTS];

vmCvar_t g_gametype;
vmCvar_t g_dmflags;
vmCvar_t g_videoflags;
vmCvar_t g_elimflags;
vmCvar_t g_voteflags;
vmCvar_t g_fraglimit;
vmCvar_t g_timelimit;
vmCvar_t g_capturelimit;
vmCvar_t g_friendlyFire;
vmCvar_t g_password;
vmCvar_t g_needpass;
vmCvar_t g_maxclients;
vmCvar_t g_maxGameClients;
vmCvar_t g_dedicated;
vmCvar_t g_speed;
vmCvar_t g_gravity;
vmCvar_t g_gravityModifier;
vmCvar_t g_damageModifier;
vmCvar_t g_cheats;
vmCvar_t g_knockback;
vmCvar_t g_quadfactor;
vmCvar_t g_forcerespawn;
vmCvar_t g_respawntime;
vmCvar_t g_inactivity;
vmCvar_t g_debugMove;
vmCvar_t g_debugDamage;
vmCvar_t g_debugAlloc;
vmCvar_t g_weaponRespawn;
vmCvar_t g_weaponTeamRespawn;
vmCvar_t g_motd;
vmCvar_t g_motdfile;
vmCvar_t g_votemaps;
vmCvar_t g_votecustom;
vmCvar_t g_synchronousClients;
vmCvar_t g_warmup;
vmCvar_t g_doWarmup;
vmCvar_t g_restarted;
vmCvar_t g_logfile;
vmCvar_t g_logfileSync;
vmCvar_t g_blood;
vmCvar_t g_podiumDist;
vmCvar_t g_podiumDrop;
vmCvar_t g_allowVote;
vmCvar_t g_teamAutoJoin;
vmCvar_t g_teamForceBalance;
vmCvar_t g_banIPs;
vmCvar_t g_filterBan;
vmCvar_t g_smoothClients;
vmCvar_t pmove_fixed;
vmCvar_t pmove_msec;
vmCvar_t pmove_float;
vmCvar_t g_rankings;
vmCvar_t g_listEntity;
vmCvar_t g_localTeamPref;
vmCvar_t g_obeliskHealth;
vmCvar_t g_obeliskRegenPeriod;
vmCvar_t g_obeliskRegenAmount;
vmCvar_t g_obeliskRespawnDelay;
vmCvar_t g_cubeTimeout;
#ifdef MISSIONPACK
vmCvar_t g_redteam;
vmCvar_t g_blueteam;
vmCvar_t g_singlePlayer;
#endif
vmCvar_t g_enableDust;
vmCvar_t cg_enableQ;		// leilei
vmCvar_t g_enableFS;		// leilei
vmCvar_t g_enableBreath;
vmCvar_t g_proxMineTimeout;
vmCvar_t g_music;
vmCvar_t g_spawnprotect;
//Following for elimination:
vmCvar_t g_elimination_damage;
vmCvar_t g_elimination_startHealth;
vmCvar_t g_elimination_startArmor;
vmCvar_t g_elimination_bfg;
vmCvar_t g_elimination_roundtime;
vmCvar_t g_elimination_warmup;
vmCvar_t g_elimination_activewarmup;
vmCvar_t g_elimination_allgametypes;
vmCvar_t g_elimination_machinegun;
vmCvar_t g_elimination_shotgun;
vmCvar_t g_elimination_grenade;
vmCvar_t g_elimination_rocket;
vmCvar_t g_elimination_railgun;
vmCvar_t g_elimination_lightning;
vmCvar_t g_elimination_plasmagun;
vmCvar_t g_elimination_chain;
vmCvar_t g_elimination_mine;
vmCvar_t g_elimination_nail;
vmCvar_t g_elimination_lockspectator;
vmCvar_t g_weaponArena;
vmCvar_t g_weaponArenaWeapon;
//dmn_clowns suggestions (with my idea of implementing):
vmCvar_t g_instantgib;
vmCvar_t g_vampire;
vmCvar_t g_vampireMaxHealth;
//Regen
vmCvar_t g_regen;
vmCvar_t g_lms_lives;
vmCvar_t g_lms_mode;
vmCvar_t g_elimination_ctf_oneway;
vmCvar_t g_awardpushing; //The server can decide if players are awarded for pushing people in lave etc.
vmCvar_t g_catchup; //Favors the week players
vmCvar_t g_autonextmap; //Autochange map
vmCvar_t g_mappools; //mappools to be used for autochange
vmCvar_t g_voteNames;
vmCvar_t g_voteBan;
vmCvar_t g_voteGametypes;
vmCvar_t g_voteMinTimelimit;
vmCvar_t g_voteMaxTimelimit;
vmCvar_t g_voteMinFraglimit;
vmCvar_t g_voteMaxFraglimit;
vmCvar_t g_maxvotes;
vmCvar_t g_humanplayers;
//used for voIP
vmCvar_t g_redTeamClientNumbers;
vmCvar_t g_blueTeamClientNumbers;
//unlagged - server options
vmCvar_t g_delagHitscan;
vmCvar_t g_truePing;
vmCvar_t sv_fps;
vmCvar_t g_lagLightning; //Adds a little lag to the lightninggun to make it less powerfull
//unlagged - server options
//KK-OAX
vmCvar_t g_sprees;
vmCvar_t g_altExcellent;
vmCvar_t g_spreeDiv;
//Command/Chat spamming/flooding
vmCvar_t g_floodMaxDemerits;
vmCvar_t g_floodMinTime;
//Admin
vmCvar_t g_admin;
vmCvar_t g_adminLog;
vmCvar_t g_adminParseSay;
vmCvar_t g_adminNameProtect;
vmCvar_t g_adminTempBan;
vmCvar_t g_adminMaxBan;
vmCvar_t g_specChat;
vmCvar_t g_publicAdminMessages;
vmCvar_t g_maxWarnings;
vmCvar_t g_warningExpire;
vmCvar_t g_minNameChangePeriod;
vmCvar_t g_maxNameChanges;
vmCvar_t g_timestamp_startgame;
vmCvar_t g_execute_gametype_script;
vmCvar_t g_emptyCommand;
vmCvar_t g_emptyTime;
vmCvar_t g_grapple;
vmCvar_t g_harvesterFromBodies;
/* Neon_Knight: Double Domination-specific cvars */
vmCvar_t g_ddCaptureTime;
vmCvar_t g_ddRespawnDelay;
vmCvar_t g_developer;
vmCvar_t g_spSkill;
vmCvar_t g_bot_noChat;
vmCvar_t g_classicMode;

mapinfo_result_t mapinfo;

// bk001129 - made static to avoid aliasing
static cvarTable_t gameCvarTable[] = {
	// don't override the cheat state set by the system
	{ &g_cheats, "sv_cheats", "", 0, 0, qfalse },

	// noset vars
	{ NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
	{ NULL, "gamedate", PRODUCT_DATE , CVAR_ROM, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },

	// latched vars
	{ &g_gametype, "g_gametype", "0", CVAR_SERVERINFO | CVAR_USERINFO | CVAR_LATCH, 0, qfalse  },

	{ &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },

	// change anytime vars
	{ &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
	{ &g_videoflags, "videoflags", "7", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
	{ &g_elimflags, "elimflags", "0", CVAR_SERVERINFO, 0, qfalse  },
	{ &g_voteflags, "voteflags", "0", CVAR_SERVERINFO, 0, qfalse  },
	{ &g_fraglimit, "fraglimit", GT_FFA_DEFAULT_SCORELIMIT, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timelimit, "timelimit", GT_FFA_DEFAULT_TIMELIMIT, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_capturelimit, "capturelimit", GT_CTF_DEFAULT_SCORELIMIT, CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

	{ &g_friendlyFire, "g_friendlyFire", "0", CVAR_ARCHIVE, 0, qtrue  },

	{ &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE  },
	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE  },

	{ &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_doWarmup, "g_doWarmup", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },
	{ &g_logfile, "g_log", "games.log", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_logfileSync, "g_logsync", "0", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },

	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

	{ &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

	{ &g_speed, "g_speed", "320", 0, 0, qtrue  },
	{ &g_gravity, "g_gravity", "800", 0, 0, qtrue  },
	{ &g_gravityModifier, "g_gravityModifier", "1", 0, 0, qtrue  },
	{ &g_damageModifier, "g_damageModifier", "0", 0, 0, qtrue },
	{ &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
	{ &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  },
	{ &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
	{ &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "20", 0, 0, qtrue },
	{ &g_respawntime, "g_respawntime", "0", CVAR_ARCHIVE, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
	{ &g_debugMove, "g_debugMove", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_motd, "g_motd", "", 0, 0, qfalse },
	{ &g_motdfile, "g_motdfile", "motd.cfg", 0, 0, qfalse },
	{ &g_blood, "com_blood", "1", 0, 0, qfalse },

	{ &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
	{ &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

	//Votes start:
	{ &g_allowVote, "g_allowVote", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_maxvotes, "g_maxVotes", MAX_VOTE_COUNT, CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteNames, "g_voteNames", "/map_restart/nextmap/map/g_gametype/kick/clientkick/g_doWarmup/timelimit/fraglimit/", CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteBan, "g_voteBan", "0", CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteGametypes, "g_voteGametypes", "/0/1/3/4/5/6/7/8/9/10/11/12/13/", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteMaxTimelimit, "g_voteMaxTimelimit", "1000", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteMinTimelimit, "g_voteMinTimelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteMaxFraglimit, "g_voteMaxFraglimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_voteMinFraglimit, "g_voteMinFraglimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_votemaps, "g_votemapsfile", "votemaps.cfg", 0, 0, qfalse },
	{ &g_votecustom, "g_votecustomfile", "votecustom.cfg", 0, 0, qfalse },

	{ &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },

	{ &g_obeliskHealth, "g_obeliskHealth", "2500", 0, 0, qfalse },
	{ &g_obeliskRegenPeriod, "g_obeliskRegenPeriod", "1", 0, 0, qfalse },
	{ &g_obeliskRegenAmount, "g_obeliskRegenAmount", "15", 0, 0, qfalse },
	{ &g_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO, 0, qfalse },

	{ &g_cubeTimeout, "g_cubeTimeout", "30", 0, 0, qfalse },
#ifdef MISSIONPACK
	{ &g_redteam, "g_redteam", "Stroggs", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue },
	{ &g_blueteam, "g_blueteam", "Pagans", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO , 0, qtrue, qtrue  },
	{ &g_singlePlayer, "ui_singlePlayerActive", "", 0, 0, qfalse, qfalse  },
#endif

	{ &g_enableFS, "g_enableFS", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },		// leilei - bikinis
	{ &cg_enableQ, "g_enableQ", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },		// leilei - q scale
	{ &g_enableDust, "g_enableDust", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO, 0, qtrue, qfalse },
	{ &g_proxMineTimeout, "g_proxMineTimeout", "20000", 0, 0, qfalse },

	{ &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qtrue},
	{ &pmove_msec, "pmove_msec", "11", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qtrue},
	{ &pmove_float, "pmove_float", "1", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qtrue},

//unlagged - server options
	{ &g_delagHitscan, "g_delagHitscan", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qtrue },
	{ &g_truePing, "g_truePing", "0", CVAR_ARCHIVE, 0, qtrue },
	// it's CVAR_SYSTEMINFO so the client's sv_fps will be automagically set to its value
	{ &sv_fps, "sv_fps", "20", CVAR_SYSTEMINFO | CVAR_ARCHIVE, 0, qfalse },
	{ &g_lagLightning, "g_lagLightning", "1", CVAR_ARCHIVE, 0, qtrue },
//unlagged - server options

	{ &g_rankings, "g_rankings", "0", 0, 0, qfalse},
	{ &g_localTeamPref, "g_localTeamPref", "", 0, 0, qfalse },
	{ &g_music, "g_music", "", 0, 0, qfalse},
	{ &g_spawnprotect, "g_spawnprotect", "500", CVAR_ARCHIVE, 0, qtrue},
	//Now for elimination stuff:
	{ &g_elimination_damage, "elimination_selfdamage", "0", 0, 0, qtrue },
	{ &g_elimination_startHealth, "elimination_startHealth", "200", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_startArmor, "elimination_startArmor", "150", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_bfg, "elimination_bfg", "0", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_roundtime, "elimination_roundtime", "120", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_warmup, "elimination_warmup", "7", CVAR_ARCHIVE , 0, qtrue },
	{ &g_elimination_activewarmup, "elimination_activewarmup", "5", CVAR_ARCHIVE , 0, qtrue },
	{ &g_elimination_allgametypes, "g_elimination", "0", CVAR_LATCH, 0, qfalse },

	{ &g_elimination_machinegun, "elimination_machinegun", "500", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_shotgun, "elimination_shotgun", "500", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_grenade, "elimination_grenade", "100", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_rocket, "elimination_rocket", "50", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_railgun, "elimination_railgun", "20", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_lightning, "elimination_lightning", "300", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_plasmagun, "elimination_plasmagun", "200", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_chain, "elimination_chain", "0", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_mine, "elimination_mine", "0", CVAR_ARCHIVE, 0, qtrue },
	{ &g_elimination_nail, "elimination_nail", "0", CVAR_ARCHIVE, 0, qtrue },

	{ &g_elimination_ctf_oneway, "elimination_ctf_oneway", "0", CVAR_ARCHIVE, 0, qtrue },

	{ &g_elimination_lockspectator, "elimination_lockspectator", "0", 0, qtrue },

	{ &g_awardpushing, "g_awardpushing", "1", CVAR_ARCHIVE, 0, qtrue },

	//nexuiz style rocket arena
	{ &g_weaponArena, "g_weaponArena", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_weaponArenaWeapon, "g_weaponArenaWeapon", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },

	//Instantgib and Vampire thingies
	{ &g_instantgib, "g_instantgib", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_vampire, "g_vampire", "0.0", 0, 0, qtrue },
	{ &g_regen, "g_regen", "0", 0, 0, qtrue },
	{ &g_vampireMaxHealth, "g_vampire_max_health", "500", 0, 0, qtrue },
	{ &g_lms_lives, "g_lms_lives", "1", 0, 0, qtrue },
	{ &g_lms_mode, "g_lms_mode", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue },

	{ &g_catchup, "g_catchup", "0", CVAR_ARCHIVE, 0, qtrue},

	{ &g_autonextmap, "g_autonextmap", "0", CVAR_ARCHIVE, 0, qfalse},
	{
		&g_mappools, "g_mappools", "0\\maps_dm.cfg\\1\\maps_tourney.cfg\\3\\maps_tdm.cfg\\4\\maps_ctf.cfg\\5\\maps_oneflag.cfg\\6\\maps_obelisk.cfg\
\\7\\maps_harvester.cfg\\8\\maps_elimination.cfg\\9\\maps_ctf.cfg\\10\\maps_lms.cfg\\11\\maps_dd.cfg\\12\\maps_dom.cfg\\13\\maps_dm.cfg\\", CVAR_ARCHIVE, 0, qfalse
	},
	{ &g_humanplayers, "g_humanplayers", "0", CVAR_ROM, 0, qfalse },
//used for voIP
	{ &g_redTeamClientNumbers, "g_redTeamClientNumbers", "0",CVAR_ROM, 0, qfalse },
	{ &g_blueTeamClientNumbers, "g_blueTeamClientNumbers", "0",CVAR_ROM, 0, qfalse },

	//KK-OAX
	{ &g_sprees, "g_sprees", "sprees.dat", 0, 0, qfalse },
	{ &g_altExcellent, "g_altExcellent", "0", CVAR_SERVERINFO, 0, qtrue},
	{ &g_spreeDiv, "g_spreeDiv", "5", 0, 0, qfalse},

	//Used for command/chat flooding
	{ &g_floodMaxDemerits, "g_floodMaxDemerits", "5000", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_floodMinTime, "g_floodMinTime", "2000", CVAR_ARCHIVE, 0, qfalse  },

	//Admin
	{ &g_admin, "g_admin", "admin.dat", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_adminLog, "g_adminLog", "admin.log", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_adminParseSay, "g_adminParseSay", "1", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_adminNameProtect, "g_adminNameProtect", "1", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_adminTempBan, "g_adminTempBan", "2m", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_adminMaxBan, "g_adminMaxBan", "2w", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_specChat, "g_specChat", "1", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_publicAdminMessages, "g_publicAdminMessages", "1", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_maxWarnings, "g_maxWarnings", "3", CVAR_ARCHIVE, 0, qfalse },
	{ &g_warningExpire, "g_warningExpire", "3600", CVAR_ARCHIVE, 0, qfalse },

	{ &g_minNameChangePeriod, "g_minNameChangePeriod", "10", 0, 0, qfalse},
	{ &g_maxNameChanges, "g_maxNameChanges", "50", 0, 0, qfalse},

	{ &g_timestamp_startgame, "g_timestamp", "0001-01-01 00:00:00", CVAR_SERVERINFO, 0, qfalse},
	{ &g_execute_gametype_script, "g_egs", "0", CVAR_ROM, 0, qfalse },
	{ &g_emptyCommand, "g_emptyCommand", "map_restart", CVAR_ARCHIVE, 0, qfalse},
	{ &g_emptyTime, "g_emptytime", "0", CVAR_ARCHIVE, 0, qfalse},

	{ &g_grapple, "g_grapple", "0", CVAR_ARCHIVE, 0, qfalse},
	{ &g_harvesterFromBodies, "g_harvesterFromBodies", "0", CVAR_ARCHIVE, 0, qtrue},
	/* Neon_Knight: Double Domination-specific cvars*/
	{ &g_ddCaptureTime, "g_ddCaptureTime", "10", CVAR_ARCHIVE, 0, qtrue},
	{ &g_ddRespawnDelay, "g_ddRespawnDelay", "10", CVAR_ARCHIVE, 0, qtrue},
	/* Neon_Knight: Developer mode*/
	{ &g_developer, "developer", "0", CVAR_CHEAT, 0, qtrue},
	{ &g_spSkill, "g_spSkill", "2", 0, 0, qtrue},
	{ &g_bot_noChat, "bot_nochat", "0", 0, 0, qtrue},
	{ &g_classicMode, "g_classicMode", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue}

};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );
static void WriteAccForAllClients( void );
static void SendVictoryChallenge( void );


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  )
{
	switch ( command ) {
	case GAME_INIT:
		G_InitGame( arg0, arg1, arg2 );
		return 0;
	case GAME_SHUTDOWN:
		G_ShutdownGame( arg0 );
		return 0;
	case GAME_CLIENT_CONNECT:
		return (intptr_t)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
		ClientCommand( arg0 );
		return 0;
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
		return 0;
	case GAME_CONSOLE_COMMAND:
		return ConsoleCommand();
	case BOTAI_START_FRAME:
		return BotAIStartFrame( arg0 );
	}

	return -1;
}


void QDECL G_Printf( const char *fmt, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Printf( text );
}

void QDECL G_Error( const char *fmt, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, fmt);
	Q_vsnprintf (text, sizeof(text), fmt, argptr);
	va_end (argptr);

	trap_Error( text );
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void )
{
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=MAX_CLIENTS, e=g_entities+i ; i < level.num_entities ; i++,e++ ) {
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++) {
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (strequals(e->team, e2->team)) {
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;
					e2->targetname = NULL;
				}
			}
		}
	}
	G_Printf ("%i teams with %i entities\n", c, c2);
}

void G_RemapTeamShaders( void )
{
#ifdef MISSIONPACK
	char string[1024];
	float f = level.time * 0.001;
	Com_sprintf( string, sizeof(string), "team_icon/%s_red", g_redteam.string );
	AddRemap("textures/ctf2/redteam01", string, f);
	AddRemap("textures/ctf2/redteam02", string, f);
	Com_sprintf( string, sizeof(string), "team_icon/%s_blue", g_blueteam.string );
	AddRemap("textures/ctf2/blueteam01", string, f);
	AddRemap("textures/ctf2/blueteam02", string, f);
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
#endif
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
		                    cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar )
			cv->modificationCount = cv->vmCvar->modificationCount;

		if (cv->teamShader) {
			remapped = qtrue;
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}

	// check some things
	if ( g_gametype.integer < 0 || g_gametype.integer >= GT_MAX_GAME_TYPE ) {
		G_Printf( "g_gametype %i is out of range, defaulting to 0\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "0" );
	}

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void )
{
	int			i;
	cvarTable_t	*cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if(cv->vmCvar == &g_gametype) {
					//gametype modified
					trap_Cvar_Set("g_egs","1");
				}

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"",
					                               cv->cvarName, cv->vmCvar->string ) );
				}

				if ( cv->vmCvar == &g_votecustom )
					VoteParseCustomVotes();

				//Here comes the cvars that must trigger a map_restart
				if (cv->vmCvar == &g_instantgib || cv->vmCvar == &g_weaponArena || cv->vmCvar == &g_elimination_allgametypes || cv->vmCvar == &g_classicMode) {
					trap_Cvar_Set("sv_dorestart","1");
				}

				if ( cv->vmCvar == &g_voteNames ) {
					//Set vote flags
					int voteflags=0;
					if( allowedVote("map_restart") )
						voteflags|=VF_map_restart;

					if( allowedVote("map") )
						voteflags|=VF_map;

					if( allowedVote("clientkick") )
						voteflags|=VF_clientkick;

					if( allowedVote("shuffle") )
						voteflags|=VF_shuffle;

					if( allowedVote("nextmap") )
						voteflags|=VF_nextmap;

					if( allowedVote("g_gametype") )
						voteflags|=VF_g_gametype;

					if( allowedVote("g_doWarmup") )
						voteflags|=VF_g_doWarmup;

					if( allowedVote("timelimit") )
						voteflags|=VF_timelimit;

					if( allowedVote("fraglimit") )
						voteflags|=VF_fraglimit;

					if( allowedVote("custom") )
						voteflags|=VF_custom;

					trap_Cvar_Set("voteflags",va("%i",voteflags));
				}

				if (cv->teamShader) {
					remapped = qtrue;
				}
			}
		}
	}

	if (remapped) {
		G_RemapTeamShaders();
	}
}

/*
 Sets the cvar g_timestamp. Return 0 if success or !0 for errors.
 */
static int G_UpdateTimestamp( void )
{
	int ret = 0;
	qtime_t timestamp;
	ret = trap_RealTime(&timestamp);
	trap_Cvar_Set("g_timestamp",va("%04i-%02i-%02i %02i:%02i:%02i",
	                               1900+timestamp.tm_year,1+timestamp.tm_mon, timestamp.tm_mday,
	                               timestamp.tm_hour,timestamp.tm_min,timestamp.tm_sec));
	return ret;
}

/**
 * Runs a script of it exists
 * @param script The script that is to be exec'ed
 * @return 0 if the script ran, 1 if it did not exist
 */
static int G_RunScript( const char* script )
{
	fileHandle_t	file;
	char buffer[100];
	Q_snprintf(buffer,sizeof(buffer),"%s",script);
	Q_StrToLower(buffer);
	G_LogPrintf("Looking for: %s\n",buffer);
	trap_FS_FOpenFile(buffer,&file,FS_READ);
	if(!file) {
		return 1; //script does not exist
	}
	trap_FS_FCloseFile(file);
	Q_snprintf(buffer,sizeof(buffer),"exec %s;\n",script);
	Q_StrToLower(buffer);
	trap_SendConsoleCommand( EXEC_APPEND, buffer );
	return 0;
}

static int G_CheckGametypeScripts( void )
{
	if(g_execute_gametype_script.integer) {
		G_RunScript(va("gametype_%i.cfg",g_gametype.integer));
		trap_Cvar_Set("g_egs","0"); //Don't run again on next restart
	}
	return 0;
}

/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart )
{
	int					i;
	char	mapname[MAX_CVAR_VALUE_STRING];


	G_Printf ("------- Game Initialization -------\n");
	G_Printf ("gamename: %s\n", GAMEVERSION);
	G_Printf ("gamedate: %s\n", PRODUCT_DATE);

	srand( randomSeed );

	G_RegisterCvars();

	G_UpdateTimestamp();

	//disable unwanted cvars
	if( g_gametype.integer == GT_SINGLE_PLAYER ) {
		g_instantgib.integer = 0;
		g_weaponArena.integer = 0;
		g_grapple.integer = 0;
		g_elimination_allgametypes.integer = 0;
		g_classicMode.integer = 0;
		g_regen.integer = 0;
		g_doWarmup.integer = 0;
		g_vampire.value = 0.0f;
	}

	G_ProcessIPBans();

	//KK-OAX Changed to Tremulous's BG_InitMemory
	BG_InitMemory();

	// set some level globals
	memset( &level, 0, sizeof( level ) );

	level.time = levelTime;
	level.startTime = levelTime;

	level.snd_fry = G_SoundIndex("sound/player/fry.wav");	// FIXME standing in lava / slime

	if ( g_gametype.integer != GT_SINGLE_PLAYER && g_logfile.string[0] ) {
		if ( g_logfileSync.integer ) {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND_SYNC );
		}
		else {
			trap_FS_FOpenFile( g_logfile.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_logfile.string );
		}
		else {
			char	serverinfo[MAX_INFO_STRING];
			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );
			G_LogPrintf("------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", serverinfo );
			G_LogPrintf("Info: ServerInfo length: %ld of %d\n", strlen(serverinfo), MAX_INFO_STRING );
		}
	}
	else {
		G_Printf( "Not logging to disk.\n" );
	}



	//Parse the custom vote names:
	VoteParseCustomVotes();

	G_InitWorldSession();

	//KK-OAX Get Admin Configuration
	G_admin_readconfig( NULL, 0 );
	//Let's Load up any killing sprees/multikills
	G_ReadAltKillSettings();

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	for ( i=0 ; i<MAX_CLIENTS ; i++ ) {
		g_entities[i].classname = "clientslot";
	}

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
	                     &level.clients[0].ps, sizeof( level.clients[0] ) );

	// reserve some spots for dead player bodies
	InitBodyQue();

	ClearRegisteredItems();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if (G_UsesTeamFlags(g_gametype.integer) || G_UsesTheWhiteFlag(g_gametype.integer) ||
			g_gametype.integer == GT_HARVESTER || g_gametype.integer == GT_OBELISK ||
			g_gametype.integer == GT_DOUBLE_D || g_gametype.integer == GT_DOMINATION) {
		G_CheckTeamItems();
	}

	SaveRegisteredItems();

	G_Printf ("-----------------------------------\n");

	if( g_gametype.integer == GT_SINGLE_PLAYER || trap_Cvar_VariableIntegerValue( "com_buildScript" ) ) {
		G_ModelIndex( SP_PODIUM_MODEL );
	}

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAISetup( restart );
		BotAILoadMap( restart );
		G_InitBots( restart );
	}

	G_RemapTeamShaders();

	trap_SetConfigstring( CS_INTERMISSION, "" );

	//elimination:
	level.roundNumber = 1;
	level.roundNumberStarted = 0;
	level.roundStartTime = level.time+g_elimination_warmup.integer*1000;
	level.roundRespawned = qfalse;
	level.eliminationSides = rand()%2; //0 or 1

	//Challenges:
	level.teamSize = 0;
	level.hadBots = qfalse;

	if(g_gametype.integer == GT_DOUBLE_D)
		Team_SpawnDoubleDominationPoints();

	if(g_gametype.integer == GT_DOMINATION ) {
		level.dom_scoreGiven = 0;
		for(i=0; i<MAX_DOMINATION_POINTS; i++)
			level.pointStatusDom[i] = TEAM_NONE;
		level.domination_points_count = 0; //make sure its not too big
	}

	PlayerStoreInit();

	//Set vote flags
	{
		int voteflags=0;
		if( allowedVote("map_restart") )
			voteflags|=VF_map_restart;

		if( allowedVote("map") )
			voteflags|=VF_map;

		if( allowedVote("clientkick") )
			voteflags|=VF_clientkick;

		if( allowedVote("shuffle") )
			voteflags|=VF_shuffle;

		if( allowedVote("nextmap") )
			voteflags|=VF_nextmap;

		if( allowedVote("g_gametype") )
			voteflags|=VF_g_gametype;

		if( allowedVote("g_doWarmup") )
			voteflags|=VF_g_doWarmup;

		if( allowedVote("timelimit") )
			voteflags|=VF_timelimit;

		if( allowedVote("fraglimit") )
			voteflags|=VF_fraglimit;

		if( allowedVote("custom") )
			voteflags|=VF_custom;

		trap_Cvar_Set("voteflags",va("%i",voteflags));
	}
	G_CheckGametypeScripts();
	trap_Cvar_VariableStringBuffer("mapname",mapname,sizeof(mapname));
	MapInfoGet(mapname,g_gametype.integer,&mapinfo);
	MapInfoPrint(&mapinfo);
	if(G_RunScript(va("mapscripts/g_%s.cfg",mapname)))
		G_RunScript("mapscripts/g_default.cfg");
	if(G_RunScript(va("mapscripts/g_%s_%i.cfg",mapname,g_gametype.integer)))
		G_RunScript(va("mapscripts/g_default_%i.cfg",g_gametype.integer) );
}



/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart )
{
	G_Printf ("==== ShutdownGame ====\n");

	if ( level.logFile ) {
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
		level.logFile = 0;
	}

	// write all the client session data so we can get it back
	G_WriteSessionData();

	//KK-OAX Admin Cleanup
	G_admin_cleanup( );
	G_admin_namelog_cleanup( );

	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
		BotAIShutdown( restart );
	}
}



//===================================================================

void QDECL Com_Error ( int level, const char *error, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, error);
	Q_vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	G_Error( "%s", text);
}

void QDECL Com_Printf( const char *msg, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	Q_vsnprintf (text, sizeof(text), msg, argptr);
	va_end (argptr);

	G_Printf ("%s", text);
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
AddTournamentPlayer

If there are less than two tournament players, put a
spectator in the game and restart
=============
*/
void AddTournamentPlayer( void )
{
	int			i;
	gclient_t	*client;
	gclient_t	*nextInLine;

	if ( level.numPlayingClients >= 2 ) {
		return;
	}

	// never change during intermission
	if ( level.intermissiontime ) {
		return;
	}

	nextInLine = NULL;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		// never select the dedicated follow or scoreboard clients
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD ||
		        client->sess.spectatorClient < 0  ) {
			continue;
		}

		if(!nextInLine || client->sess.spectatorNum > nextInLine->sess.spectatorNum) {
			nextInLine = client;
		}
	}

	if ( !nextInLine ) {
		return;
	}

	level.warmupTime = -1;

	// set them to free-for-all team
	SetTeam( &g_entities[ nextInLine - level.clients ], "f" );
}

/*
=======================
AddTournamentQueue

Add client to end of tournament queue
=======================
*/
void AddTournamentQueue(gclient_t *client)
{
	int index;
	gclient_t *curclient;
	for(index = 0; index < level.maxclients; index++) {
		curclient = &level.clients[index];
		if(curclient->pers.connected != CON_DISCONNECTED) {
			if(curclient == client)
				curclient->sess.spectatorNum = 0;
			else if(curclient->sess.sessionTeam == TEAM_SPECTATOR)
				curclient->sess.spectatorNum++;
		}
	}
}

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
void RemoveTournamentLoser( void )
{
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[1];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
RemoveTournamentWinner
=======================
*/
void RemoveTournamentWinner( void )
{
	int			clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[0];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s" );
}

/*
=======================
AdjustTournamentScores
=======================
*/
void AdjustTournamentScores( void )
{
	int			clientNum;

	clientNum = level.sortedClients[0];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED ) {
		level.clients[ clientNum ].sess.wins++;
		ClientUserinfoChanged( clientNum );
	}

	clientNum = level.sortedClients[1];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED ) {
		level.clients[ clientNum ].sess.losses++;
		ClientUserinfoChanged( clientNum );
	}

}

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b )
{
	gclient_t	*ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorNum > cb->sess.spectatorNum ) {
			return -1;
		}
		if ( ca->sess.spectatorNum < cb->sess.spectatorNum ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	//In elimination and CTF elimination, sort dead players last
	if(G_UsesTeamFlags(g_gametype.integer) && !G_UsesTheWhiteFlag(g_gametype.integer)
	        && level.roundNumber==level.roundNumberStarted && (ca->isEliminated != cb->isEliminated)) {
		if( ca->isEliminated )
			return 1;
		if( cb->isEliminated )
			return -1;
	}

	// then sort by score
	if ( ca->ps.persistant[PERS_SCORE]
	        > cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE]
	        < cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void )
{
	int		i;
	int		rank;
	int		score;
	int		newScore;
	int		humanplayers;
	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	humanplayers = 0; // don't count bots
	for ( i = 0; i < TEAM_NUM_TEAMS; i++ ) {
		level.numteamVotingClients[i] = 0;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			//We just set humanplayers to 0 during intermission
			if ( !level.intermissiontime && level.clients[i].pers.connected == CON_CONNECTED && !(g_entities[i].r.svFlags & SVF_BOT) ) {
				humanplayers++;
			}

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;
				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
						if ( level.clients[i].sess.sessionTeam == TEAM_RED )
							level.numteamVotingClients[0]++;
						else if ( level.clients[i].sess.sessionTeam == TEAM_BLUE )
							level.numteamVotingClients[1]++;
					}
					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					}
					else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients,
	       sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if (G_IsATeamGametype(g_gametype.integer)) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			}
			else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			}
			else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	}
	else {
		rank = -1;
		score = 0;
		for ( i = 0;  i < level.numPlayingClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			}
			else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if ( g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1 ) {
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if (G_IsATeamGametype(g_gametype.integer)) {
		trap_SetConfigstring( CS_SCORES1, va("%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va("%i", level.teamScores[TEAM_BLUE] ) );
	}
	else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", SCORE_NOT_PRESENT) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		}
		else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", SCORE_NOT_PRESENT) );
		}
		else {
			trap_SetConfigstring( CS_SCORES1, va("%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va("%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime ) {
		SendScoreboardMessageToAllClients();
	}

	if(g_humanplayers.integer != humanplayers) //Presume all spectators are humans!
		trap_Cvar_Set( "g_humanplayers", va("%i", humanplayers) );
	if(humanplayers > level.max_humanplayers) {
		G_LogPrintf( "Info: There has been at least %i humans now\n", humanplayers );
		level.max_humanplayers = humanplayers;
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void )
{
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
			EliminationMessage( g_entities + i );
		}
	}
}

static void WriteAccForAllClients( void )
{
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			LogAcc(i);
		}
	}
}

static void SendVictoryChallenge( void )
{
	int		i;
	int		award = 0;
	gclient_t		*cl;

	if(level.max_humanplayers < 2 || level.hadBots)
		return;

	switch(g_gametype.integer) {
	case GT_FFA:
		award = GAMETYPES_FFA_WINS;
		break;
	case GT_TEAM:
		award = GAMETYPES_TDM_WINS;
		break;
	case GT_TOURNAMENT:
		award = GAMETYPES_TOURNEY_WINS;
		break;
	case GT_LMS:
		award = GAMETYPES_LMS_WINS;
		break;
	case GT_CTF:
		award = GAMETYPES_CTF_WINS;
		break;
	case GT_1FCTF:
		award = GAMETYPES_1FCTF_WINS;
		break;
	case GT_OBELISK:
		award = GAMETYPES_OVERLOAD_WINS;
		break;
	case GT_HARVESTER:
		award = GAMETYPES_HARVESTER_WINS;
		break;
	case GT_ELIMINATION:
		award = GAMETYPES_ELIMINATION_WINS;
		break;
	case GT_CTF_ELIMINATION:
		award = GAMETYPES_CTF_ELIMINATION_WINS;
		break;
	case GT_DOUBLE_D:
		award = GAMETYPES_DD_WINS;
		break;
	case GT_DOMINATION:
		award = GAMETYPES_DOM_WINS;
		break;
	case GT_POSSESSION:
		award = GAMETYPES_POS_WINS;
		break;
	default:
		return;
	};
	if (G_IsATeamGametype(g_gametype.integer)) {
		//Team games
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			cl = &level.clients[i];

			if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
				continue;
			}
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}

			if( cl->sess.sessionTeam == TEAM_RED && level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				ChallengeMessage(g_entities + cl->ps.clientNum,award);
			}

			if( cl->sess.sessionTeam == TEAM_BLUE && level.teamScores[TEAM_RED] < level.teamScores[TEAM_BLUE] ) {
				ChallengeMessage(g_entities + cl->ps.clientNum,award);
			}
		}
	}
	else {
		//FFA games
		ChallengeMessage(g_entities + level.sortedClients[0],award);
	}
}

/*
========================
SendElimiantionMessageToAllClients

Used to send information important to Elimination
========================
*/
void SendEliminationMessageToAllClients( void )
{
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			EliminationMessage( g_entities + i );
		}
	}
}

/*
========================
SendDDtimetakenMessageToAllClients

Do this if a team just started dominating.
========================
*/
void SendDDtimetakenMessageToAllClients( void )
{
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DoubleDominationScoreTimeMessage( g_entities + i );
		}
	}
}


/*
========================
SendDominationPointsStatusMessageToAllClients

Used for Standard domination
========================
*/
void SendDominationPointsStatusMessageToAllClients( void )
{
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DominationPointStatusMessage( g_entities + i );
		}
	}
}
/*
========================
SendYourTeamMessageToTeam

Tell all players on a given team who there allies are. Used for VoIP
========================
*/
void SendYourTeamMessageToTeam( team_t team )
{
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED && level.clients[ i ].sess.sessionTeam == team ) {
			YourTeamMessage( g_entities + i );
		}
	}
}


/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent )
{
	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}

	FindIntermissionPoint();
	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void )
{
	gentity_t	*ent, *target;
	vec3_t		dir;

	// find the intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if ( !ent ) {	// the map creator forgot to put in an intermission point...
		SelectSpawnPoint ( vec3_origin, level.intermission_origin, level.intermission_angle, 0 );
	}
	else {
		VectorCopy (ent->s.origin, level.intermission_origin);
		VectorCopy (ent->s.angles, level.intermission_angle);
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void )
{
	int			i;
	gentity_t	*client;

	if ( level.intermissiontime ) {
		return;		// already active
	}

	// if in tournement mode, change the wins / losses
	if ( g_gametype.integer == GT_TOURNAMENT ) {
		AdjustTournamentScores();
	}

	level.intermissiontime = level.time;
	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++) {
		client = g_entities + i;
		if (!client->inuse)
			continue;
		// respawn if dead
		if (client->health <= 0) {
			ClientRespawn(client);
		}
		MoveClientToIntermission( client );
	}
#ifdef MISSIONPACK
	if (g_singlePlayer.integer) {
		trap_Cvar_Set("ui_singlePlayerActive", "0");
		UpdateTournamentInfo();
	}
#else
	// if single player game
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		UpdateTournamentInfo();
		SpawnModelsOnVictoryPads();
	}
#endif
	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel (void)
{
	int		i;
	gclient_t *cl;
	char nextmap[MAX_STRING_CHARS];
	char d1[MAX_STRING_CHARS];
	char	serverinfo[MAX_INFO_STRING];

	//bot interbreeding
	BotInterbreedEndMatch();

	// if we are running a tournement map, kick the loser to spectator status,
	// which will automatically grab the next spectator and restart
	if ( g_gametype.integer == GT_TOURNAMENT  ) {
		if ( !level.restarted ) {
			RemoveTournamentLoser();
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			level.changemap = NULL;
			level.intermissiontime = 0;
		}
		return;
	}

	trap_Cvar_VariableStringBuffer( "nextmap", nextmap, sizeof(nextmap) );
	trap_Cvar_VariableStringBuffer( "d1", d1, sizeof(d1) );

	trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

	//Here the game finds the nextmap if g_autonextmap is set
	if(g_autonextmap.integer ) {
		char filename[MAX_FILEPATH];
		fileHandle_t file,mapfile;
		//Look in g_mappools.string for the file to look for maps in
		Q_strncpyz(filename,Info_ValueForKey(g_mappools.string, va("%i",g_gametype.integer)),MAX_FILEPATH);
		//If we found a filename:
		if(filename[0]) {
			//Read the file:
			trap_FS_FOpenFile(filename, &file, FS_READ);
			if(!file)
				trap_FS_FOpenFile(va("%s.org",filename), &file, FS_READ);
			if(file) {
				char  buffer[4*1024]; // buffer to read file into
				char mapnames[1024][20]; // Array of mapnames in the map pool
				char *pointer;
				int choice, count=0; //The random choice from mapnames and count of mapnames
				int i;
				memset(&buffer,0,sizeof(buffer));
				trap_FS_Read(&buffer,sizeof(buffer),file);
				pointer = buffer;
				while ( qtrue ) {
					Q_strncpyz(mapnames[count],COM_Parse( &pointer ),20);
					if ( !mapnames[count][0] ) {
						break;
					}
					G_Printf("Mapname in mappool: %s\n",mapnames[count]);
					count++;
				}
				trap_FS_FCloseFile(file);
				//It is possible that the maps in the file read are flawed, so we try up to ten times:
				for(i=0; i<10; i++) {
					choice = (count > 0)? rand()%count : 0;
					if(Q_strequal(mapnames[choice],Info_ValueForKey(serverinfo,"mapname")))
						continue;
					//Now check that the map exists:
					trap_FS_FOpenFile(va("maps/%s.bsp",mapnames[choice]),&mapfile,FS_READ);
					if(mapfile) {
						G_Printf("Picked map number %i - %s\n",choice,mapnames[choice]);
						Q_strncpyz(nextmap,va("map %s",mapnames[choice]),sizeof(nextmap));
						trap_Cvar_Set("nextmap",nextmap);
						trap_FS_FCloseFile(mapfile);
						break;
					}
				}
			}
		}
	}

	if( Q_strequal( nextmap, "map_restart 0" ) && !Q_strequal( d1, "" ) ) {
		trap_Cvar_Set( "nextmap", "vstr d2" );
		trap_SendConsoleCommand( EXEC_APPEND, "vstr d1\n" );
	}
	else {
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	}

	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i=0 ; i< g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... )
{
	va_list		argptr;
	char		string[1024];
	int			min, tens, sec;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof(string), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	Q_vsnprintf(string + 7, sizeof(string) - 7, fmt, argptr);
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string )
{
	int				i, numSorted;
	gclient_t		*cl;
#ifdef MISSIONPACK
	qboolean won = qtrue;
	team_t team = TEAM_RED;
#endif
	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	WriteAccForAllClients();

	SendVictoryChallenge();

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	if (G_IsATeamGametype(g_gametype.integer)) {
		G_LogPrintf( "red:%i  blue:%i\n",
		             level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
	}

	for (i=0 ; i < numSorted ; i++) {
		int		ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],	cl->pers.netname );
#ifdef MISSIONPACK
		if (g_singlePlayer.integer && !(g_entities[cl - level.clients].r.svFlags & SVF_BOT)) {
			team = cl->sess.sessionTeam;
		}
		if (g_singlePlayer.integer && G_IsADMBasedGametype(g_gametype.integer)) {
			if (g_entities[cl - level.clients].r.svFlags & SVF_BOT && cl->ps.persistant[PERS_RANK] == 0) {
				won = qfalse;
			}
		}
#endif

	}

#ifdef MISSIONPACK
 	if (g_singlePlayer.integer) {
		if (G_IsATeamGametype(g_gametype.integer)) {
			if (team == TEAM_BLUE) {
				won = level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED];
			} else {
				won = level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE];
			}
 		}
		trap_SendConsoleCommand( EXEC_APPEND, (won) ? "spWin\n" : "spLose\n" );
	}
#endif


}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void )
{
	int			ready, notReady, playerCount;
	int			i;
	gclient_t	*cl;
	int			readyMask;

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		return;
	}

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	playerCount = 0;
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

		playerCount++;
		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		}
		else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// only test ready status when there are real players present
	if ( playerCount > 0 ) {
		// if nobody wants to go, clear timer
		if ( !ready ) {
			level.readyToExit = qfalse;
			return;
		}

		// if everyone wants to go, go now
		if ( !notReady ) {
			ExitLevel();
			return;
		}
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void )
{
	int		a, b;

	if ( level.numPlayingClients < 2 ) {
		return qfalse;
	}

	//Sago: In Elimination and Oneway Flag Capture teams must win by two points.
	if (G_IsARoundBasedGametype(g_gametype.integer) && G_IsATeamGametype(g_gametype.integer)) {
		return (level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ||
		        level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE]+1 ||
		        level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE]-1);
	}

	if (G_IsATeamGametype(g_gametype.integer)) {
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void )
{
	int			i;
	gclient_t	*cl;
	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) {
		CheckIntermissionExit ();
		return;
	}
	else {
		//sago: Find the reason for this to be neccesary.
		for (i=0 ; i< g_maxclients.integer ; i++) {
			cl = level.clients + i;
			if ( cl->pers.connected != CON_CONNECTED ) {
				continue;
			}
			cl->ps.stats[STAT_CLIENTS_READY] = 0;
		}
	}

	if ( level.intermissionQueued ) {
#ifdef MISSIONPACK
		int time = (g_singlePlayer.integer) ? SP_INTERMISSION_DELAY_TIME : INTERMISSION_DELAY_TIME;
		if ( level.time - level.intermissionQueued >= time ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#else
		if ( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
#endif
		return;
	}

	// check for sudden death
	if ( ScoreIsTied() ) {
		// always wait for sudden death
		return;
	}

	if ( g_timelimit.integer < 0 || g_timelimit.integer > INT_MAX / 60000 ) {
		G_Printf( "timelimit %i is out of range, defaulting to 0\n", g_timelimit.integer );
		trap_Cvar_Set( "timelimit", "0" );
		trap_Cvar_Update( &g_timelimit );
	}

	if ( g_timelimit.integer > 0 && !level.warmupTime ) {
		if ( (level.time - level.startTime)/60000 >= g_timelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Timelimit hit.\n\"");
			LogExit( "Timelimit hit." );
			return;
		}
	}

	if ( level.numPlayingClients < 2 ) {
		return;
	}

	if (G_GametypeUsesFragLimit(g_gametype.integer)) {
		if ( g_fraglimit.integer < 0 ) {
			G_Printf( "fraglimit %i is out of range, defaulting to 0\n", g_fraglimit.integer );
			trap_Cvar_Set( "fraglimit", "0" );
			trap_Cvar_Update( &g_fraglimit );
		}
		if ( g_fraglimit.integer ) {
			if ( G_IsATeamGametype(g_gametype.integer) ) {
				if ( level.teamScores[TEAM_RED] >= g_fraglimit.integer ) {
					trap_SendServerCommand( -1, "print \"Red hit the fraglimit.\n\"" );
					LogExit( "Fraglimit hit." );
					return;
				}

				if ( level.teamScores[TEAM_BLUE] >= g_fraglimit.integer ) {
					trap_SendServerCommand( -1, "print \"Blue hit the fraglimit.\n\"" );
					LogExit( "Fraglimit hit." );
					return;
				}
			}
			else {
				for ( i=0 ; i< g_maxclients.integer ; i++ ) {
					cl = level.clients + i;
					if ( cl->pers.connected != CON_CONNECTED ) {
						continue;
					}
					if ( cl->sess.sessionTeam != TEAM_FREE ) {
						continue;
					}

					if ( cl->ps.persistant[PERS_SCORE] >= g_fraglimit.integer ) {
						LogExit( "Fraglimit hit." );
						trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " hit the fraglimit.\n\"",
													   cl->pers.netname ) );
						return;
					}
				}
			}
		}
	}
	else if (G_GametypeUsesCaptureLimit(g_gametype.integer)) {
		if ( g_capturelimit.integer < 0 ) {
			G_Printf( "capturelimit %i is out of range, defaulting to 0\n", g_capturelimit.integer );
			trap_Cvar_Set( "capturelimit", "0" );
			trap_Cvar_Update( &g_capturelimit );
		}

		if ( g_capturelimit.integer ) {
			if (G_IsATeamGametype(g_gametype.integer)) {
				if ( level.teamScores[TEAM_RED] >= g_capturelimit.integer ) {
					trap_SendServerCommand( -1, "print \"Red hit the capturelimit.\n\"" );
					LogExit( "Capturelimit hit." );
					return;
				}

				if ( level.teamScores[TEAM_BLUE] >= g_capturelimit.integer ) {
					trap_SendServerCommand( -1, "print \"Blue hit the capturelimit.\n\"" );
					LogExit( "Capturelimit hit." );
					return;
				}
			}
			else {
				for ( i=0 ; i< g_maxclients.integer ; i++ ) {
					cl = level.clients + i;
					if ( cl->pers.connected != CON_CONNECTED ) {
						continue;
					}
					if ( cl->sess.sessionTeam != TEAM_FREE ) {
						continue;
					}

					if ( cl->ps.persistant[PERS_SCORE] >= g_capturelimit.integer ) {
						LogExit( "Capturelimit hit." );
						trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " hit the capturelimit.\n\"",
													   cl->pers.netname ) );
						return;
					}
				}
			}
		}
	}
}


/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/

/*
CheckDoubleDomination
*/

void CheckDoubleDomination( void )
{
	if ( level.numPlayingClients < 1 ) {
		return;
	}

	if ( level.warmupTime != 0) {
		if( ((level.pointStatusA == TEAM_BLUE && level.pointStatusB == TEAM_BLUE) ||
				(level.pointStatusA == TEAM_RED && level.pointStatusB == TEAM_RED)) &&
				level.timeTaken + g_ddCaptureTime.integer*1000 <= level.time ) {
			Team_RemoveDoubleDominationPoints();
			level.roundStartTime = level.time + g_ddRespawnDelay.integer*1000;
			SendScoreboardMessageToAllClients();
		}
		return;
	}

	if(g_gametype.integer != GT_DOUBLE_D)
		return;

	//Don't score if we are in intermission. Both points might have been taken when we went into intermission
	if(level.intermissiontime)
		return;

	if(level.pointStatusA == TEAM_RED && level.pointStatusB == TEAM_RED &&
			level.timeTaken + g_ddCaptureTime.integer*1000 <= level.time) {
		//Red scores
		trap_SendServerCommand( -1, "print \"Red team scores!\n\"");
		AddTeamScore(level.intermission_origin,TEAM_RED,1);
		G_LogPrintf( "DD: %i %i %i: %s scores!\n", -1, TEAM_RED, 2, TeamName(TEAM_RED) );
		Team_ForceGesture(TEAM_RED);
		Team_DD_bonusAtPoints(TEAM_RED);
		Team_RemoveDoubleDominationPoints();
		//We start again in 10 seconds:
		level.roundStartTime = level.time + g_ddRespawnDelay.integer*1000;
		SendScoreboardMessageToAllClients();
		CalculateRanks();
	}

	if(level.pointStatusA == TEAM_BLUE && level.pointStatusB == TEAM_BLUE &&
			level.timeTaken + g_ddCaptureTime.integer*1000 <= level.time) {
		//Blue scores
		trap_SendServerCommand( -1, "print \"Blue team scores!\n\"");
		AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
		G_LogPrintf( "DD: %i %i %i: %s scores!\n", -1, TEAM_BLUE, 2, TeamName(TEAM_BLUE) );
		Team_ForceGesture(TEAM_BLUE);
		Team_DD_bonusAtPoints(TEAM_BLUE);
		Team_RemoveDoubleDominationPoints();
		//We start again in 10 seconds:
		level.roundStartTime = level.time + g_ddRespawnDelay.integer*1000;
		SendScoreboardMessageToAllClients();
		CalculateRanks();
	}

	if((level.pointStatusA == TEAM_NONE || level.pointStatusB == TEAM_NONE) && level.time>level.roundStartTime) {
		Team_SpawnDoubleDominationPoints();
		SendScoreboardMessageToAllClients();
	}
}


/*
=============
CheckDomination
=============
*/
void CheckDomination(void)
{
	int i;
	int scoreFactor = 1;

	if ( (level.numPlayingClients < 1) || (g_gametype.integer != GT_DOMINATION) ) {
		return;
	}

	//Do nothing if warmup
	if(level.warmupTime != 0)
		return;

	//Don't score if we are in intermission. Just plain stupid
	if(level.intermissiontime)
		return;

	//Sago: I use if instead of while, since if the server stops for more than 2 seconds people should not be allowed to score anyway
	if(level.domination_points_count>3) {
		scoreFactor = 2; //score more slowly if there are many points
	}
	if(level.time>=level.dom_scoreGiven*DOM_SECSPERPOINT*scoreFactor) {
		for(i=0; i<level.domination_points_count; i++) {
			if ( level.pointStatusDom[i] == TEAM_RED ) {
				AddTeamScore(level.intermission_origin,TEAM_RED,1);
			}
			if ( level.pointStatusDom[i] == TEAM_BLUE ) {
				AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
			}
			G_LogPrintf( "DOM: %i %i %i %i: %s holds point %s for 1 point!\n",
			             -1,i,1,level.pointStatusDom[i],
			             TeamName(level.pointStatusDom[i]),level.domination_points_names[i]);
		}
		level.dom_scoreGiven++;
		while(level.time>level.dom_scoreGiven*DOM_SECSPERPOINT*scoreFactor) {
			level.dom_scoreGiven++;
		}
		CalculateRanks();
	}
}

/*
=============
CheckTournament

Once a frame, check for changes in tournement player state
=============
*/
void CheckTournament( void )
{
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( level.numPlayingClients == 0 ) {
		return;
	}

	if ( g_gametype.integer == GT_TOURNAMENT ) {

		// pull in a spectator if needed
		if ( level.numPlayingClients < 2 ) {
			AddTournamentPlayer();
		}

		// if we don't have two players, go back to "waiting for players"
		if ( level.numPlayingClients != 2 ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return;
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			if ( level.numPlayingClients == 2 ) {
				// fudge by -1 to account for extra delays
				if ( g_warmup.integer > 1 ) {
					level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
				}
				else {
					level.warmupTime = 0;
				}

				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			}
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			return;
		}
	}
	else if ( g_gametype.integer != GT_SINGLE_PLAYER && level.warmupTime != 0 ) {
		int		counts[TEAM_NUM_TEAMS];
		qboolean	notEnough = qfalse;

		if(G_IsATeamGametype(g_gametype.integer)) {
			counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
			counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

			if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1) {
				notEnough = qtrue;
			}
		}
		else if ( level.numPlayingClients < 2 ) {
			notEnough = qtrue;
		}

		if ( notEnough ) {
			if ( level.warmupTime != -1 ) {
				level.warmupTime = -1;
				trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
				G_LogPrintf( "Warmup:\n" );
			}
			return; // still waiting for team members
		}

		if ( level.warmupTime == 0 ) {
			return;
		}

		// if the warmup is changed at the console, restart it
		if ( g_warmup.modificationCount != level.warmupModificationCount ) {
			level.warmupModificationCount = g_warmup.modificationCount;
			level.warmupTime = -1;
		}

		// if all players have arrived, start the countdown
		if ( level.warmupTime < 0 ) {
			// fudge by -1 to account for extra delays
			if ( g_warmup.integer > 1 ) {
				level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
			}
			else {
				level.warmupTime = 0;
			}

			trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			return;
		}

		// if the warmup time has counted down, restart
		if ( level.time > level.warmupTime ) {
			level.warmupTime += 10000;
			trap_Cvar_Set( "g_restarted", "1" );
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			return;
		}
	}
}




/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, const char *message)
{
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		trap_SendServerCommand( i, message );
	}
}

/*
==================
SetLeader
==================
*/
void SetLeader(int team, int client)
{
	int i;

	if ( level.clients[client].pers.connected == CON_DISCONNECTED ) {
		PrintTeam(team, va("print \"%s is not connected\n\"", level.clients[client].pers.netname) );
		return;
	}
	if (level.clients[client].sess.sessionTeam != team) {
		PrintTeam(team, va("print \"%s is not on the team anymore\n\"", level.clients[client].pers.netname) );
		return;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team) {
			continue;
		}
		if (level.clients[i].sess.teamLeader) {
			level.clients[i].sess.teamLeader = qfalse;
			ClientUserinfoChanged(i);
		}
	}
	level.clients[client].sess.teamLeader = qtrue;
	ClientUserinfoChanged( client );
	PrintTeam(team, va("print \"%s is the new team leader\n\"", level.clients[client].pers.netname) );
}

/*
==================
CheckTeamLeader
==================
*/
void CheckTeamLeader( int team )
{
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if (level.clients[i].sess.sessionTeam != team)
			continue;
		if (level.clients[i].sess.teamLeader)
			break;
	}
	if (i >= level.maxclients) {
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			if (!(g_entities[i].r.svFlags & SVF_BOT)) {
				level.clients[i].sess.teamLeader = qtrue;
				break;
			}
		}
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if (level.clients[i].sess.sessionTeam != team)
				continue;
			level.clients[i].sess.teamLeader = qtrue;
			break;
		}
	}
}

/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote( int team )
{
	int cs_offset;

	if ( team == TEAM_RED )
		cs_offset = 0;
	else if ( team == TEAM_BLUE )
		cs_offset = 1;
	else
		return;

	if ( !level.teamVoteTime[cs_offset] ) {
		return;
	}
	if ( level.time - level.teamVoteTime[cs_offset] >= VOTE_TIME ) {
		trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
	}
	else {
		if ( level.teamVoteYes[cs_offset] > level.numteamVotingClients[cs_offset]/2 ) {
			// execute the command, then remove the vote
			trap_SendServerCommand( -1, "print \"Team vote passed.\n\"" );
			//
			if ( !Q_strncmp( "leader", level.teamVoteString[cs_offset], 6) ) {
				//set the team leader
				SetLeader(team, atoi(level.teamVoteString[cs_offset] + 7));
			}
			else {
				trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.teamVoteString[cs_offset] ) );
			}
		}
		else if ( level.teamVoteNo[cs_offset] >= level.numteamVotingClients[cs_offset]/2 ) {
			// same behavior as a timeout
			trap_SendServerCommand( -1, "print \"Team vote failed.\n\"" );
		}
		else {
			// still waiting for a majority
			return;
		}
	}
	level.teamVoteTime[cs_offset] = 0;
	trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, "" );

}

static void CheckEmpty ( void )
{
	if (!g_emptyTime.integer ) {
		return; //a time period must be set
	}
	if (g_humanplayers.integer > 0 || level.lastActiveTime==0) {
		level.lastActiveTime = level.time;
	}
	else {
		if (level.lastActiveTime+g_emptyTime.integer*1000 < level.time) {
			level.lastActiveTime = level.time;
			trap_SendConsoleCommand( EXEC_APPEND, va("%s\n",g_emptyCommand.string) );
		}
	}
}


/*
==================
CheckCvars
==================
*/
void CheckCvars( void )
{
	static int lastMod = -1;

	if ( g_password.modificationCount != lastMod ) {
		lastMod = g_password.modificationCount;
		if ( *g_password.string && !Q_strequal( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		}
		else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent)
{
	int	thinktime;

	thinktime = ent->nextthink;
	if (thinktime <= 0) {
		return;
	}
	if (thinktime > level.time) {
		return;
	}

	ent->nextthink = 0;
	if (!ent->think) {
		G_Error ( "NULL ent->think");
	}
	ent->think (ent);
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime )
{
	int			i;
	gentity_t	*ent;

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	}

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;
	//msec = level.time - level.previousTime;

	// get any cvar changes
	G_UpdateCvars();

	if( (G_IsARoundBasedGametype(g_gametype.integer) && G_IsATeamGametype(g_gametype.integer)) && !(g_elimflags.integer & EF_NO_FREESPEC) && g_elimination_lockspectator.integer>1)
		trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer|EF_NO_FREESPEC));
	else if( (g_elimflags.integer & EF_NO_FREESPEC) && g_elimination_lockspectator.integer<2)
		trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer&(~EF_NO_FREESPEC) ) );

	if( g_elimination_ctf_oneway.integer && !(g_elimflags.integer & EF_ONEWAY) ) {
		trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer|EF_ONEWAY ) );
		//If the server admin has enabled it midgame imidiantly braodcast attacking team
		SendAttackingTeamMessageToAllClients();
	}
	else if( !g_elimination_ctf_oneway.integer && (g_elimflags.integer & EF_ONEWAY) ) {
		trap_Cvar_Set("elimflags",va("%i",g_elimflags.integer&(~EF_ONEWAY) ) );
	}

	//
	// go through all allocated objects
	//
	//start = trap_Milliseconds();
	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
			if ( ent->s.event ) {
				ent->s.event = 0;	// &= EV_EVENT_BITS;
				if ( ent->client ) {
					ent->client->ps.externalEvent = 0;
					// predicted events should never be set to zero
					//ent->client->ps.events[0] = 0;
					//ent->client->ps.events[1] = 0;
				}
			}
			if ( ent->freeAfterEvent ) {
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			}
			else if ( ent->unlinkAfterEvent ) {
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( !ent->r.linked && ent->neverFree ) {
			continue;
		}

//unlagged - backward reconciliation #2
		// we'll run missiles separately to save CPU in backward reconciliation
		/*
				if ( ent->s.eType == ET_MISSILE ) {
					G_RunMissile( ent );
					continue;
				}
		*/
//unlagged - backward reconciliation #2

		if ( ent->s.eType == ET_ITEM || ent->physicsObject ) {
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER ) {
			G_RunMover( ent );
			continue;
		}

		if ( i < MAX_CLIENTS ) {
			G_RunClient( ent );
			continue;
		}

		G_RunThink( ent );
	}

//unlagged - backward reconciliation #2
	// NOW run the missiles, with all players backward-reconciled
	// to the positions they were in exactly 50ms ago, at the end
	// of the last server frame
	G_TimeShiftAllClients( level.previousTime, NULL );

	ent = &g_entities[0];
	for (i=0 ; i<level.num_entities ; i++, ent++) {
		if ( !ent->inuse ) {
			continue;
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( ent->s.eType == ET_MISSILE ) {
			G_RunMissile( ent );
		}
	}

	G_UnTimeShiftAllClients( NULL );
//unlagged - backward reconciliation #2

//end = trap_Milliseconds();

//start = trap_Milliseconds();
	// perform final fixups on the players
	ent = &g_entities[0];
	for (i=0 ; i < level.maxclients ; i++, ent++ ) {
		if ( ent->inuse ) {
			ClientEndFrame( ent );
		}
	}
//end = trap_Milliseconds();

	// see if it is time to do a tournement restart
	CheckTournament();

	//Check Elimination state
	CheckElimination();
	CheckLMS();

	//Check Double Domination
	CheckDoubleDomination();

	CheckDomination();

	//Sago: I just need to think why I placed this here... they should only spawn once
	if(g_gametype.integer == GT_DOMINATION)
		Team_Dom_SpawnPoints();


	if (g_gametype.integer == GT_POSSESSION && level.time > 5000) {
		Possession_SpawnFlag();
	}

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();

	// check team votes
	CheckTeamVote( TEAM_RED );
	CheckTeamVote( TEAM_BLUE );

	CheckEmpty();

	// for tracking changes
	CheckCvars();

	if (g_listEntity.integer) {
		for (i = 0; i < MAX_GENTITIES; i++) {
			G_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		trap_Cvar_Set("g_listEntity", "0");
	}

//unlagged - backward reconciliation #4
	// record the time at the end of this frame - it should be about
	// the time the next frame begins - when the server starts
	// accepting commands from connected clients
	level.frameStartTime = trap_Milliseconds();
//unlagged - backward reconciliation #4
}


void MapInfoPrint(mapinfo_result_t *info)
{
	G_Printf("Auther: %s\n",info->author);
	if (G_GametypeUsesCaptureLimit(g_gametype.integer)) {
		G_Printf("Capturelimit: %i\n",info->captureLimit);
	}
	else { /* (G_GametypeUsesCaptureLimit(g_gametype.integer)) */
		G_Printf("Fraglimit: %i\n",info->fragLimit);
	}
	G_Printf("minTeamSize: %i\n",info->minTeamSize);
}

/* Neon_Knight: Useful check in order to have code consistency. */
/*
===================
G_IsATeamGametype

Checks if the gametype is a team-based game.
===================
 */
qboolean G_IsATeamGametype(int check) {
	return GAMETYPE_IS_A_TEAM_GAME(check);
}
/*
===================
G_IsAFFAGametype

Checks if the gametype is NOT a team-based game.
===================
 */
qboolean G_IsADMBasedGametype(int check) {
	return !GAMETYPE_IS_A_TEAM_GAME(check);
}
/*
===================
G_UsesTeamFlags

Checks if the gametype makes use of the red and blue flags.
===================
 */
qboolean G_UsesTeamFlags(int check) {
	return GAMETYPE_USES_RED_AND_BLUE_FLAG(check);
}
/*
===================
G_UsesTheWhiteFlag

Checks if the gametype makes use of the neutral flag.
===================
 */
qboolean G_UsesTheWhiteFlag(int check) {
	return GAMETYPE_USES_WHITE_FLAG(check);
}
/*
===================
G_IsARoundBasedGametype

Checks if the gametype has a round-based system.
===================
 */
qboolean G_IsARoundBasedGametype(int check) {
	return GAMETYPE_IS_ROUND_BASED(check);
}
/*
===================
G_GametypeUsesFragLimit

Checks if the gametype has a frag-based scoring system.
===================
 */
qboolean G_GametypeUsesFragLimit(int check) {
	return GAMETYPE_USES_FRAG_LIMIT(check);
}
/*
===================
G_GametypeUsesCaptureLimit

Checks if the gametype has a capture-based scoring system.
===================
 */
qboolean G_GametypeUsesCaptureLimit(int check) {
	return GAMETYPE_USES_CAPTURE_LIMIT(check);
}
/*
===================
G_GetWeaponArenaWeapon

Returns the value of a weapon for g_weaponArena.
Weapons are sorted like this instead of actual weapon order because Weapon 1 (Gauntlet)
is the default option and Weapon 10 (Grappling Hook) is an additional weapon.
And for menu reasons. (*cough*classicui*cough*)
===================
 */
int G_GetWeaponArenaWeapon(int weapon) {
	switch (weapon) {
		case 1: return WP_MACHINEGUN;
			break;
		case 2: return WP_SHOTGUN;
			break;
		case 3: return WP_GRENADE_LAUNCHER;
			break;
		case 4: return WP_ROCKET_LAUNCHER;
			break;
		case 5: return WP_LIGHTNING;
			break;
		case 6: return WP_RAILGUN;
			break;
		case 7: return WP_PLASMAGUN;
			break;
		case 8: return WP_BFG;
			break;
		case 9: return WP_NAILGUN;
			break;
		case 10: return WP_CHAINGUN;
			break;
		case 11: return WP_PROX_LAUNCHER;
			break;
		default: return WP_GAUNTLET;
			break;
	}
}
/*
===================
G_IsANoPickupsMode

Returns true if the match has a "no pickups" rule.
===================
 */
qboolean G_IsANoPickupsMode(void) {
	// In Instagib mode, no pickups
	if (g_instantgib.integer) {
		return qtrue;
	}
	// In Weapon Arena mode, no pickups
	if (g_weaponArena.integer) {
		return qtrue;
	}
	// In Elimination mode for non-round-based modes, no pickups
	if (g_elimination_allgametypes.integer) {
		return qtrue;
	}
	return qfalse;
}
/*
===================
G_GametypeUsesRunes

Returns true if the match has a "no pickups" rule.
===================
 */
qboolean G_GametypeUsesRunes(int check) {
	return GAMETYPE_USES_RUNES(check);
}
/*
===================
G_GetAttackingTeam

Returns the team that's actually on offense in eCTF AvD matches.
===================
 */
int G_GetAttackingTeam(void) {
	if ((level.eliminationSides+level.roundNumber)%2 == 0) {
		return TEAM_RED;
	}
	else if ((level.eliminationSides+level.roundNumber)%2 == 1) {
		return TEAM_BLUE;
	}
	return TEAM_NONE;
}
/* /Neon_Knight */


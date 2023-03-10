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
/*
=======================================================================

USER INTERFACE MAIN

=======================================================================
*/


#include "ui_local.h"


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .qvm file
================
*/
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) {
	switch ( command ) {
	case UI_GETAPIVERSION:
		return UI_API_VERSION;

	case UI_INIT:
		UI_Init();
		return 0;

	case UI_SHUTDOWN:
		UI_Shutdown();
		return 0;

	case UI_KEY_EVENT:
		UI_KeyEvent( arg0, arg1 );
		return 0;

	case UI_MOUSE_EVENT:
		UI_MouseEvent( arg0, arg1 );
		return 0;

	case UI_REFRESH:
		UI_Refresh( arg0 );
		return 0;

	case UI_IS_FULLSCREEN:
		return UI_IsFullscreen();

	case UI_SET_ACTIVE_MENU:
		UI_SetActiveMenu( arg0 );
		return 0;

	case UI_CONSOLE_COMMAND:
		return UI_ConsoleCommand(arg0);

	case UI_DRAW_CONNECT_SCREEN:
		UI_DrawConnectScreen( arg0 );
		return 0;
	case UI_HASUNIQUECDKEY:				// mod authors need to observe this
		return qtrue;  // bk010117 - change this to qfalse for mods!
	}

	return -1;
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t *vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

vmCvar_t ui_ffa_fraglimit;
vmCvar_t ui_ffa_timelimit;
vmCvar_t ui_tourney_fraglimit;
vmCvar_t ui_tourney_timelimit;
vmCvar_t ui_team_fraglimit;
vmCvar_t ui_team_timelimit;
vmCvar_t ui_team_friendly;
vmCvar_t ui_ctf_capturelimit;
vmCvar_t ui_ctf_timelimit;
vmCvar_t ui_ctf_friendly;
vmCvar_t ui_1fctf_capturelimit;
vmCvar_t ui_1fctf_timelimit;
vmCvar_t ui_1fctf_friendly;
vmCvar_t ui_overload_capturelimit;
vmCvar_t ui_overload_timelimit;
vmCvar_t ui_overload_friendly;
vmCvar_t ui_harvester_capturelimit;
vmCvar_t ui_harvester_timelimit;
vmCvar_t ui_harvester_friendly;
vmCvar_t ui_elimination_capturelimit;
vmCvar_t ui_elimination_timelimit;
vmCvar_t ui_ctf_elimination_capturelimit;
vmCvar_t ui_ctf_elimination_timelimit;
vmCvar_t ui_lms_fraglimit;
vmCvar_t ui_lms_timelimit;
vmCvar_t ui_dd_capturelimit;
vmCvar_t ui_dd_timelimit;
vmCvar_t ui_dd_friendly;
vmCvar_t ui_dom_capturelimit;
vmCvar_t ui_dom_timelimit;
vmCvar_t ui_dom_friendly;
vmCvar_t ui_pos_scorelimit;
vmCvar_t ui_pos_timelimit;
vmCvar_t ui_arenasFile;
vmCvar_t ui_botsFile;
vmCvar_t ui_spScores1;
vmCvar_t ui_spScores2;
vmCvar_t ui_spScores3;
vmCvar_t ui_spScores4;
vmCvar_t ui_spScores5;
vmCvar_t ui_spAwards;
vmCvar_t ui_spVideos;
vmCvar_t ui_spSkill;
vmCvar_t ui_spSelection;
vmCvar_t ui_browserMaster;
vmCvar_t ui_browserGameType;
vmCvar_t ui_browserSortKey;
vmCvar_t ui_browserShowFull;
vmCvar_t ui_browserShowEmpty;
vmCvar_t ui_brassTime;
vmCvar_t ui_drawCrosshair;
vmCvar_t ui_drawCrosshairNames;
vmCvar_t ui_marks;
vmCvar_t ui_server1;
vmCvar_t ui_server2;
vmCvar_t ui_server3;
vmCvar_t ui_server4;
vmCvar_t ui_server5;
vmCvar_t ui_server6;
vmCvar_t ui_server7;
vmCvar_t ui_server8;
vmCvar_t ui_server9;
vmCvar_t ui_server10;
vmCvar_t ui_server11;
vmCvar_t ui_server12;
vmCvar_t ui_server13;
vmCvar_t ui_server14;
vmCvar_t ui_server15;
vmCvar_t ui_server16;
//vmCvar_t ui_cdkeychecked;
//new in beta 23:
vmCvar_t        ui_browserOnlyHumans;
//new in beta 37:
vmCvar_t        ui_setupchecked;
vmCvar_t ui_developer;

// bk001129 - made static to avoid aliasing.
static cvarTable_t cvarTable[] = {
	{ &ui_ffa_fraglimit, "ui_ffa_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_ffa_timelimit, "ui_ffa_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_tourney_fraglimit, "ui_tourney_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_tourney_timelimit, "ui_tourney_timelimit", "15", CVAR_ARCHIVE },

	{ &ui_team_fraglimit, "ui_team_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_team_timelimit, "ui_team_timelimit", "20", CVAR_ARCHIVE },
	{ &ui_team_friendly, "ui_team_friendly",  "1", CVAR_ARCHIVE },

	{ &ui_ctf_capturelimit, "ui_ctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_timelimit, "ui_ctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_ctf_friendly, "ui_ctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_1fctf_capturelimit, "ui_1fctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_1fctf_timelimit, "ui_1fctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_1fctf_friendly, "ui_1fctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_overload_capturelimit, "ui_overload_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_overload_timelimit, "ui_overload_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_overload_friendly, "ui_overload_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_harvester_capturelimit, "ui_harvester_capturelimit", "20", CVAR_ARCHIVE },
	{ &ui_harvester_timelimit, "ui_harvester_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_harvester_friendly, "ui_harvester_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_elimination_capturelimit, "ui_elimination_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_elimination_timelimit, "ui_elimination_timelimit", "20", CVAR_ARCHIVE },

	{ &ui_ctf_elimination_capturelimit, "ui_ctf_elimination_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_elimination_timelimit, "ui_ctf_elimination_timelimit", "30", CVAR_ARCHIVE },

	{ &ui_lms_fraglimit, "ui_lms_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_lms_timelimit, "ui_lms_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_dd_capturelimit, "ui_dd_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_dd_timelimit, "ui_dd_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_dd_friendly, "ui_dd_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_dom_capturelimit, "ui_dom_capturelimit", "500", CVAR_ARCHIVE },
	{ &ui_dom_timelimit, "ui_dom_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_dom_friendly, "ui_dom_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_pos_scorelimit, "ui_pos_scorelimit", "120", CVAR_ARCHIVE },
	{ &ui_pos_timelimit, "ui_pos_timelimit", "20", CVAR_ARCHIVE },

	{ &ui_arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_spScores1, "g_spScores1", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores2, "g_spScores2", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores3, "g_spScores3", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores4, "g_spScores4", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores5, "g_spScores5", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spAwards, "g_spAwards", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spVideos, "g_spVideos", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE | CVAR_LATCH },

	{ &ui_spSelection, "ui_spSelection", "", CVAR_ROM },

	{ &ui_browserMaster, "ui_browserMaster", "1", CVAR_ARCHIVE },
	{ &ui_browserGameType, "ui_browserGameType", "0", CVAR_ARCHIVE },
	{ &ui_browserSortKey, "ui_browserSortKey", "4", CVAR_ARCHIVE },
	{ &ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE },
	{ &ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE },

	{ &ui_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE },
	{ &ui_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE },
	{ &ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
	{ &ui_marks, "cg_marks", "1", CVAR_ARCHIVE },

	{ &ui_server1, "server1", "", CVAR_ARCHIVE },
	{ &ui_server2, "server2", "", CVAR_ARCHIVE },
	{ &ui_server3, "server3", "", CVAR_ARCHIVE },
	{ &ui_server4, "server4", "", CVAR_ARCHIVE },
	{ &ui_server5, "server5", "", CVAR_ARCHIVE },
	{ &ui_server6, "server6", "", CVAR_ARCHIVE },
	{ &ui_server7, "server7", "", CVAR_ARCHIVE },
	{ &ui_server8, "server8", "", CVAR_ARCHIVE },
	{ &ui_server9, "server9", "", CVAR_ARCHIVE },
	{ &ui_server10, "server10", "", CVAR_ARCHIVE },
	{ &ui_server11, "server11", "", CVAR_ARCHIVE },
	{ &ui_server12, "server12", "", CVAR_ARCHIVE },
	{ &ui_server13, "server13", "", CVAR_ARCHIVE },
	{ &ui_server14, "server14", "", CVAR_ARCHIVE },
	{ &ui_server15, "server15", "", CVAR_ARCHIVE },
	{ &ui_server16, "server16", "", CVAR_ARCHIVE },
	{ &ui_browserOnlyHumans, "ui_browserOnlyHumans", "0", CVAR_ARCHIVE },
	{ &ui_setupchecked, "ui_setupchecked", "0", CVAR_ARCHIVE },
	{ &ui_developer, "developer", "0", CVAR_CHEAT },
	{ NULL, "g_localTeamPref", "", 0 }
};

// bk001129 - made static to avoid aliasing
static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

/*
=================
UI_RegisterCvars
=================
*/
void UI_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}
}

/*
=================
UI_UpdateCvars
=================
*/
void UI_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		if ( !cv->vmCvar ) {
			continue;
		}
		trap_Cvar_Update( cv->vmCvar );
	}
}

/*
==================
 * UI_SetDefaultCvar
 * If the cvar is blank it will be set to value
 * This is only good for cvars that cannot naturally be blank
================== 
 */
void UI_SetDefaultCvar(const char* cvar, const char* value) {
	if(strlen(UI_Cvar_VariableString(cvar)) == 0) {
		trap_Cvar_Set(cvar,value);
	}
}

/* Neon_Knight: Useful check in order to have code consistency. */
/*
===================
UI_SP_IsATeamGametype

Checks if the gametype is a team-based game.
===================
 */
qboolean UI_SP_IsATeamGametype(int gametype, int subgametype) {
	if (gametype != GT_SINGLE_PLAYER && GAMETYPE_IS_A_TEAM_GAME(gametype)) {
		return qtrue;
	}
	else if (gametype == GT_SINGLE_PLAYER && GAMETYPE_IS_A_TEAM_GAME(subgametype)) {
		return qtrue;
	}
	return qfalse;
}
/*
===================
UI_SP_UsesTeamFlags

Checks if the gametype makes use of the red and blue flags.
===================
 */
qboolean UI_SP_UsesTeamFlags(int gametype, int subgametype) {
	if (gametype != GT_SINGLE_PLAYER && GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype)) {
		return qtrue;
	}
	else if (gametype == GT_SINGLE_PLAYER && GAMETYPE_USES_RED_AND_BLUE_FLAG(subgametype)) {
		return qtrue;
	}
	return qfalse;
}
/*
===================
UI_SP_UsesTheWhiteFlag

Checks if the gametype makes use of the neutral flag.
===================
 */
qboolean UI_SP_UsesTheWhiteFlag(int gametype, int subgametype) {
	if (gametype != GT_SINGLE_PLAYER && GAMETYPE_USES_WHITE_FLAG(gametype)) {
		return qtrue;
	}
	else if (gametype == GT_SINGLE_PLAYER && GAMETYPE_USES_WHITE_FLAG(subgametype)) {
		return qtrue;
	}
	return qfalse;
}
/*
===================
UI_SP_IsARoundBasedGametype

Checks if the gametype has a round-based system.
===================
 */
qboolean UI_SP_IsARoundBasedGametype(int gametype, int subgametype) {
	if (gametype != GT_SINGLE_PLAYER && GAMETYPE_IS_ROUND_BASED(gametype)) {
		return qtrue;
	}
	else if (gametype == GT_SINGLE_PLAYER && GAMETYPE_IS_ROUND_BASED(subgametype)) {
		return qtrue;
	}
	return qfalse;
}
/*
===================
UI_SP_SingleGametypeCheck

Checks if the game takes place in a particular gametype.
Replaces all direct gametype calls.
===================
 */
qboolean UI_SP_SingleGametypeCheck(int gametype, int subgametype, int check) {
	if (gametype != GT_SINGLE_PLAYER && gametype == check) {
		return qtrue;
	}
	else if (gametype == GT_SINGLE_PLAYER && subgametype != GT_SINGLE_PLAYER && subgametype == check) {
		return qtrue;
	}
	return qfalse;
}
/*
===================
UI_IsATeamGametype

Checks if the gametype is a team-based game.
===================
 */
qboolean UI_IsATeamGametype(int gametype) {
	return GAMETYPE_IS_A_TEAM_GAME(gametype);
}
/*
===================
UI_UsesTeamFlags

Checks if the gametype makes use of the red and blue flags.
===================
 */
qboolean UI_UsesTeamFlags(int gametype) {
	return GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype);
}
/*
===================
UI_UsesTheWhiteFlag

Checks if the gametype makes use of the neutral flag.
===================
 */
qboolean UI_UsesTheWhiteFlag(int gametype) {
	return GAMETYPE_USES_WHITE_FLAG(gametype);
}
/*
===================
UI_IsARoundBasedGametype

Checks if the gametype has a round-based system.
===================
 */
qboolean UI_IsARoundBasedGametype(int gametype) {
	return GAMETYPE_IS_ROUND_BASED(gametype);
}
/* /Neon_Knight */

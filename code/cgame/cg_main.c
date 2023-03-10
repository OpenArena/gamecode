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
// cg_main.c -- initialization and primary entry point for cgame
#include "cg_local.h"
#include "../../sound/announcer.h"

#ifdef MISSIONPACK
#include "../ui/ui_shared.h"
// display context for new ui stuff
displayContextDef_t cgDC;
#endif

int forceModelModificationCount = -1;

void CG_Init(int serverMessageNum, int serverCommandSequence, int clientNum);
void CG_Shutdown(void);


int realVidWidth;
int realVidHeight; // leilei - global video hack

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
 */
Q_EXPORT intptr_t vmMain(int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11) {

	switch (command) {
		case CG_INIT:
			CG_Init(arg0, arg1, arg2);
			return 0;
		case CG_SHUTDOWN:
			CG_Shutdown();
			return 0;
		case CG_CONSOLE_COMMAND:
			return CG_ConsoleCommand();
		case CG_DRAW_ACTIVE_FRAME:
			CG_DrawActiveFrame(arg0, arg1, arg2);
			CG_FairCvars();
			return 0;
		case CG_CROSSHAIR_PLAYER:
			return CG_CrosshairPlayer();
		case CG_LAST_ATTACKER:
			return CG_LastAttacker();
		case CG_KEY_EVENT:
			CG_KeyEvent(arg0, arg1);
			return 0;
		case CG_MOUSE_EVENT:
#ifdef MISSIONPACK
			cgDC.cursorx = cgs.cursorX;
			cgDC.cursory = cgs.cursorY;
#endif
			CG_MouseEvent(arg0, arg1);
			return 0;
		case CG_EVENT_HANDLING:
			CG_EventHandling(arg0);
			return 0;
		default:
			CG_Error("vmMain: unknown command %i", command);
			break;
	}
	return -1;
}


cg_t cg;
cgs_t cgs;
centity_t cg_entities[MAX_GENTITIES];
weaponInfo_t cg_weapons[MAX_WEAPONS];
itemInfo_t cg_items[MAX_ITEMS];


vmCvar_t cg_railTrailTime;
vmCvar_t cg_centertime;
vmCvar_t cg_runpitch;
vmCvar_t cg_runroll;
vmCvar_t cg_bob;
vmCvar_t cg_bobup;
vmCvar_t cg_bobpitch;
vmCvar_t cg_bobroll;
vmCvar_t cg_bobmodel; // leilei
vmCvar_t cg_kickScale;
vmCvar_t cg_swingSpeed;
vmCvar_t cg_shadows;
vmCvar_t cg_gibs;
vmCvar_t cg_drawTimer;
vmCvar_t cg_drawFPS;
vmCvar_t cg_drawSnapshot;
vmCvar_t cg_draw3dIcons;
vmCvar_t cg_drawIcons;
vmCvar_t cg_drawAmmoWarning;
vmCvar_t cg_drawCrosshair;
vmCvar_t cg_drawCrosshairNames;
vmCvar_t cg_drawRewards;
vmCvar_t cg_crosshairSize;
vmCvar_t cg_crosshairX;
vmCvar_t cg_crosshairY;
vmCvar_t cg_crosshairHealth;
vmCvar_t cg_draw2D;
vmCvar_t cg_drawStatus;
vmCvar_t cg_animSpeed;
vmCvar_t cg_debugAnim;
vmCvar_t cg_debugPosition;
vmCvar_t cg_debugEvents;
vmCvar_t cg_errorDecay;
vmCvar_t cg_nopredict;
vmCvar_t cg_noPlayerAnims;
vmCvar_t cg_showmiss;
vmCvar_t cg_footsteps;
vmCvar_t cg_addMarks;
vmCvar_t cg_brassTime;
vmCvar_t cg_viewsize;
vmCvar_t cg_viewnudge; // leilei
vmCvar_t cg_muzzleflashStyle;
vmCvar_t cg_drawGun;
vmCvar_t cg_gun_frame;
vmCvar_t cg_gun_x;
vmCvar_t cg_gun_y;
vmCvar_t cg_gun_z;
vmCvar_t cg_tracerChance;
vmCvar_t cg_tracerWidth;
vmCvar_t cg_tracerLength;
vmCvar_t cg_autoswitch;
vmCvar_t cg_ignore;
vmCvar_t cg_simpleItems;
vmCvar_t cg_fov;
vmCvar_t cg_zoomFov;
vmCvar_t cg_thirdPerson;
vmCvar_t cg_thirdPersonRange;
vmCvar_t cg_thirdPersonAngle;
vmCvar_t cg_lagometer;
vmCvar_t cg_drawAttacker;
vmCvar_t cg_drawSpeed;
vmCvar_t cg_synchronousClients;
vmCvar_t cg_teamChatTime;
vmCvar_t cg_teamChatHeight;
vmCvar_t cg_stats;
vmCvar_t cg_buildScript;
vmCvar_t cg_forceModel;
vmCvar_t cg_paused;
vmCvar_t cg_blood;
vmCvar_t cg_predictItems;
vmCvar_t cg_deferPlayers;
vmCvar_t cg_drawTeamOverlay;
vmCvar_t cg_teamOverlayUserinfo;
vmCvar_t cg_drawFriend;
vmCvar_t cg_teamChatsOnly;
vmCvar_t cg_noVoiceChats;
vmCvar_t cg_noVoiceText;
vmCvar_t cg_hudFiles;
vmCvar_t cg_scorePlum;
vmCvar_t cg_obituaryOutput;
//unlagged - smooth clients #2
// this is done server-side now
//vmCvar_t cg_smoothClients;
//unlagged - smooth clients #2
vmCvar_t pmove_fixed;
//vmCvar_t cg_pmove_fixed;
vmCvar_t pmove_msec;
vmCvar_t pmove_float;
vmCvar_t cg_pmove_msec;

vmCvar_t cg_cameraEyes;
vmCvar_t cg_cameraEyes_Fwd;
vmCvar_t cg_cameraEyes_Up;

vmCvar_t cg_modelEyes_Up;
vmCvar_t cg_modelEyes_Right;
vmCvar_t cg_modelEyes_Fwd;
vmCvar_t cg_cameraOrbit;
vmCvar_t cg_cameraOrbitDelay;
vmCvar_t cg_timescaleFadeEnd;
vmCvar_t cg_timescaleFadeSpeed;
vmCvar_t cg_timescale;
vmCvar_t cg_smallFont;
vmCvar_t cg_bigFont;
vmCvar_t cg_noTaunt;
vmCvar_t cg_noProjectileTrail;
vmCvar_t cg_oldRail;
vmCvar_t cg_oldRocket;
vmCvar_t cg_leiEnhancement; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_leiBrassNoise; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_leiGoreNoise; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_leiSuperGoreyAwesome; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_leiDebug; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_leiChibi; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_oldPlasma;
vmCvar_t cg_trueLightning;
vmCvar_t cg_music;
vmCvar_t cg_weaponOrder;

vmCvar_t cg_leiWidescreen; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_deathcam; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_cameramode; // ANOTHER LEILEI LINE!!!
vmCvar_t cg_cameraEyes; // ANOTHER LEILEI LINE!!!



#ifdef MISSIONPACK
vmCvar_t cg_redTeamName;
vmCvar_t cg_blueTeamName;
vmCvar_t cg_currentSelectedPlayer;
vmCvar_t cg_currentSelectedPlayerName;
vmCvar_t cg_singlePlayer;
vmCvar_t cg_singlePlayerActive;
vmCvar_t cg_recordSPDemo;
vmCvar_t cg_recordSPDemoName;
#endif
vmCvar_t cg_obeliskRespawnDelay;
vmCvar_t cg_enableDust;
vmCvar_t cg_enableBreath;
vmCvar_t cg_enableFS;
vmCvar_t cg_enableQ;

//unlagged - client options
vmCvar_t cg_delag;
//vmCvar_t cg_debugDelag;
//vmCvar_t cg_drawBBox;
vmCvar_t cg_cmdTimeNudge;
vmCvar_t sv_fps;
vmCvar_t cg_projectileNudge;
vmCvar_t cg_optimizePrediction;
vmCvar_t cl_timeNudge;
//vmCvar_t cg_latentSnaps;
//vmCvar_t cg_latentCmds;
//vmCvar_t cg_plOut;
//unlagged - client options

//elimination addition
vmCvar_t cg_alwaysWeaponBar;
vmCvar_t cg_hitsound;
vmCvar_t cg_voip_teamonly;
vmCvar_t cg_voteflags;
vmCvar_t cg_cyclegrapple;
vmCvar_t cg_vote_custom_commands;

vmCvar_t cg_autovertex;

vmCvar_t cg_fragmsgsize;

vmCvar_t cg_crosshairPulse;
vmCvar_t cg_differentCrosshairs;
vmCvar_t cg_ch1;
vmCvar_t cg_ch1size;
vmCvar_t cg_ch2;
vmCvar_t cg_ch2size;
vmCvar_t cg_ch3;
vmCvar_t cg_ch3size;
vmCvar_t cg_ch4;
vmCvar_t cg_ch4size;
vmCvar_t cg_ch5;
vmCvar_t cg_ch5size;
vmCvar_t cg_ch6;
vmCvar_t cg_ch6size;
vmCvar_t cg_ch7;
vmCvar_t cg_ch7size;
vmCvar_t cg_ch8;
vmCvar_t cg_ch8size;
vmCvar_t cg_ch9;
vmCvar_t cg_ch9size;
vmCvar_t cg_ch10;
vmCvar_t cg_ch10size;
vmCvar_t cg_ch11;
vmCvar_t cg_ch11size;
vmCvar_t cg_ch12;
vmCvar_t cg_ch12size;
vmCvar_t cg_ch13;
vmCvar_t cg_ch13size;

vmCvar_t cg_crosshairColorRed;
vmCvar_t cg_crosshairColorGreen;
vmCvar_t cg_crosshairColorBlue;

vmCvar_t cg_weaponBarStyle;
vmCvar_t cg_chatBeep;
vmCvar_t cg_teamChatBeep;

/* Neon_Knight: Missionpack checks toggle. Just because. */
vmCvar_t cg_missionpackChecks;
/* /Neon_Knight */
/* Neon_Knight: Developer mode. */
vmCvar_t cg_developer;
/* /Neon_Knight */
vmCvar_t cg_customAnnouncer;
vmCvar_t cg_announcePowerups;

typedef struct {
	vmCvar_t *vmCvar;
	char *cvarName;
	char *defaultString;
	int cvarFlags;
} cvarTable_t;

static cvarTable_t cvarTable[] = {// bk001129
	{ &cg_ignore, "cg_ignore", "0", 0}, // used for debugging
	{ &cg_autoswitch, "cg_autoswitch", "1", CVAR_ARCHIVE},
	{ &cg_drawGun, "cg_drawGun", "1", CVAR_ARCHIVE},
	{ &cg_zoomFov, "cg_zoomfov", "22.5", CVAR_ARCHIVE},
	{ &cg_fov, "cg_fov", "90", CVAR_ARCHIVE},
	{ &cg_viewsize, "cg_viewsize", "100", CVAR_ARCHIVE},
	{ &cg_viewnudge, "cg_viewnudge", "0", CVAR_ARCHIVE},
	{ &cg_shadows, "cg_shadows", "1", CVAR_ARCHIVE},
	{ &cg_gibs, "cg_gibs", "1", CVAR_ARCHIVE},
	{ &cg_draw2D, "cg_draw2D", "1", CVAR_ARCHIVE},
	{ &cg_drawStatus, "cg_drawStatus", "1", CVAR_ARCHIVE},
	{ &cg_drawTimer, "cg_drawTimer", "0", CVAR_ARCHIVE},
	{ &cg_drawFPS, "cg_drawFPS", "0", CVAR_ARCHIVE},
	{ &cg_drawSnapshot, "cg_drawSnapshot", "0", CVAR_ARCHIVE},
	{ &cg_draw3dIcons, "cg_draw3dIcons", "1", CVAR_ARCHIVE},
	{ &cg_drawIcons, "cg_drawIcons", "1", CVAR_ARCHIVE},
	{ &cg_drawAmmoWarning, "cg_drawAmmoWarning", "1", CVAR_ARCHIVE},
	{ &cg_drawAttacker, "cg_drawAttacker", "1", CVAR_ARCHIVE},
	{ &cg_drawSpeed, "cg_drawSpeed", "0", CVAR_ARCHIVE},
	{ &cg_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE},
	{ &cg_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE},
	{ &cg_drawRewards, "cg_drawRewards", "1", CVAR_ARCHIVE},
	{ &cg_crosshairSize, "cg_crosshairSize", "24", CVAR_ARCHIVE},
	{ &cg_crosshairHealth, "cg_crosshairHealth", "1", CVAR_ARCHIVE},
	{ &cg_crosshairX, "cg_crosshairX", "0", CVAR_ARCHIVE},
	{ &cg_crosshairY, "cg_crosshairY", "0", CVAR_ARCHIVE},
	{ &cg_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE},
	{ &cg_simpleItems, "cg_simpleItems", "0", CVAR_ARCHIVE},
	{ &cg_addMarks, "cg_marks", "1", CVAR_ARCHIVE},
	{ &cg_lagometer, "cg_lagometer", "1", CVAR_ARCHIVE},
	{ &cg_railTrailTime, "cg_railTrailTime", "600", CVAR_ARCHIVE},
	{ &cg_gun_x, "cg_gunX", "0", CVAR_CHEAT},
	{ &cg_gun_y, "cg_gunY", "0", CVAR_CHEAT},
	{ &cg_gun_z, "cg_gunZ", "0", CVAR_CHEAT},
	{ &cg_centertime, "cg_centertime", "3", CVAR_CHEAT},
	{ &cg_runpitch, "cg_runpitch", "0.002", CVAR_ARCHIVE},
	{ &cg_runroll, "cg_runroll", "0.005", CVAR_ARCHIVE},
	{ &cg_bob, "cg_bob", "1", CVAR_ARCHIVE},
	{ &cg_bobup, "cg_bobup", "0.005", CVAR_CHEAT},
	{ &cg_bobpitch, "cg_bobpitch", "0.002", CVAR_ARCHIVE},
	{ &cg_bobroll, "cg_bobroll", "0.002", CVAR_ARCHIVE},
	{ &cg_bobmodel, "cg_bobmodel", "0", CVAR_ARCHIVE}, // leilei
	{ &cg_kickScale, "cg_kickScale", "1.0", CVAR_ARCHIVE},
	{ &cg_swingSpeed, "cg_swingSpeed", "0.3", CVAR_CHEAT},
	{ &cg_animSpeed, "cg_animspeed", "1", CVAR_CHEAT},
	{ &cg_debugAnim, "cg_debuganim", "0", CVAR_CHEAT},
	{ &cg_debugPosition, "cg_debugposition", "0", CVAR_CHEAT},
	{ &cg_debugEvents, "cg_debugevents", "0", CVAR_CHEAT},
	{ &cg_errorDecay, "cg_errordecay", "100", 0},
	{ &cg_nopredict, "cg_nopredict", "0", 0},
	{ &cg_noPlayerAnims, "cg_noplayeranims", "0", CVAR_CHEAT},
	{ &cg_showmiss, "cg_showmiss", "0", 0},
	{ &cg_footsteps, "cg_footsteps", "1", CVAR_CHEAT},
	{ &cg_tracerChance, "cg_tracerchance", "0.4", CVAR_CHEAT},
	{ &cg_tracerWidth, "cg_tracerwidth", "1", CVAR_CHEAT},
	{ &cg_tracerLength, "cg_tracerlength", "100", CVAR_CHEAT},
	{ &cg_thirdPersonRange, "cg_thirdPersonRange", "40", CVAR_CHEAT},
	{ &cg_thirdPersonAngle, "cg_thirdPersonAngle", "0", CVAR_CHEAT},
	{ &cg_thirdPerson, "cg_thirdPerson", "0", 0},
	{ &cg_teamChatTime, "cg_teamChatTime", "3000", CVAR_ARCHIVE},
	{ &cg_teamChatHeight, "cg_teamChatHeight", "0", CVAR_ARCHIVE},
	{ &cg_forceModel, "cg_forceModel", "0", CVAR_ARCHIVE},
	{ &cg_predictItems, "cg_predictItems", "1", CVAR_ARCHIVE},
#ifdef MISSIONPACK
	{ &cg_deferPlayers, "cg_deferPlayers", "0", CVAR_ARCHIVE},
#else
	{ &cg_deferPlayers, "cg_deferPlayers", "1", CVAR_ARCHIVE},
#endif
	{ &cg_drawTeamOverlay, "cg_drawTeamOverlay", "0", CVAR_ARCHIVE},
	{ &cg_teamOverlayUserinfo, "teamoverlay", "0", CVAR_ROM | CVAR_USERINFO},
	{ &cg_stats, "cg_stats", "0", 0},
	{ &cg_drawFriend, "cg_drawFriend", "1", CVAR_ARCHIVE},
	{ &cg_teamChatsOnly, "cg_teamChatsOnly", "0", CVAR_ARCHIVE},
	{ &cg_noVoiceChats, "cg_noVoiceChats", "0", CVAR_ARCHIVE},
	{ &cg_noVoiceText, "cg_noVoiceText", "0", CVAR_ARCHIVE},
	// the following variables are created in other parts of the system,
	// but we also reference them here
	{ &cg_buildScript, "com_buildScript", "0", 0}, // force loading of all possible data amd error on failures
	{ &cg_paused, "cl_paused", "0", CVAR_ROM},
	{ &cg_blood, "com_blood", "1", CVAR_ARCHIVE},
	{ &cg_alwaysWeaponBar, "cg_alwaysWeaponBar", "0", CVAR_ARCHIVE}, //Elimination
	{ &cg_hitsound, "cg_hitsound", "0", CVAR_ARCHIVE},
	{ &cg_voip_teamonly, "cg_voipTeamOnly", "1", CVAR_ARCHIVE},
	{ &cg_voteflags, "cg_voteflags", "*", CVAR_ROM},
	{ &cg_cyclegrapple, "cg_cyclegrapple", "1", CVAR_ARCHIVE},
	{ &cg_vote_custom_commands, "cg_vote_custom_commands", "", CVAR_ROM},
	{ &cg_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO}, // communicated by systeminfo

	{ &cg_autovertex, "cg_autovertex", "0", CVAR_ARCHIVE},
#ifdef MISSIONPACK
	{ &cg_redTeamName, "g_redteam", DEFAULT_REDTEAM_NAME, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO},
	{ &cg_blueTeamName, "g_blueteam", DEFAULT_BLUETEAM_NAME, CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_USERINFO},
	{ &cg_currentSelectedPlayer, "cg_currentSelectedPlayer", "0", CVAR_ARCHIVE},
	{ &cg_currentSelectedPlayerName, "cg_currentSelectedPlayerName", "", CVAR_ARCHIVE},
	{ &cg_singlePlayer, "ui_singlePlayerActive", "0", CVAR_USERINFO},
	{ &cg_singlePlayerActive, "ui_singlePlayerActive", "0", CVAR_USERINFO},
	{ &cg_recordSPDemo, "ui_recordSPDemo", "0", CVAR_ARCHIVE},
	{ &cg_recordSPDemoName, "ui_recordSPDemoName", "", CVAR_ARCHIVE},
	{ &cg_hudFiles, "cg_hudFiles", "ui/hud.txt", CVAR_ARCHIVE},
#endif
	{ &cg_enableFS, "g_enableFS", "0", CVAR_SERVERINFO},
	{ &cg_enableQ, "g_enableQ", "0", CVAR_SERVERINFO},
	{ &cg_enableDust, "g_enableDust", "0", CVAR_SERVERINFO},
	{ &cg_enableBreath, "g_enableBreath", "0", CVAR_SERVERINFO},
	{ &cg_obeliskRespawnDelay, "g_obeliskRespawnDelay", "10", CVAR_SERVERINFO},

	{ &cg_cameraOrbit, "cg_cameraOrbit", "0", CVAR_CHEAT},
	{ &cg_cameraOrbitDelay, "cg_cameraOrbitDelay", "50", CVAR_ARCHIVE},
	{ &cg_timescaleFadeEnd, "cg_timescaleFadeEnd", "1", 0},
	{ &cg_timescaleFadeSpeed, "cg_timescaleFadeSpeed", "0", 0},
	{ &cg_timescale, "timescale", "1", 0},
	{ &cg_scorePlum, "cg_scorePlums", "1", CVAR_USERINFO | CVAR_ARCHIVE},
	{ &cg_obituaryOutput, "cg_obituaryOutput", "3", CVAR_ARCHIVE},
	//unlagged - smooth clients #2
	// this is done server-side now
	//	{ &cg_smoothClients, "cg_smoothClients", "0", CVAR_USERINFO | CVAR_ARCHIVE},
	//unlagged - smooth clients #2
	//	{ &cg_cameraMode, "com_cameraMode", "0", CVAR_CHEAT},

	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO},
	{ &pmove_msec, "pmove_msec", "11", CVAR_SYSTEMINFO},
	{ &pmove_float, "pmove_float", "1", CVAR_SYSTEMINFO},
	{ &cg_noTaunt, "cg_noTaunt", "0", CVAR_ARCHIVE},
	{ &cg_noProjectileTrail, "cg_noProjectileTrail", "0", CVAR_ARCHIVE},
	{ &cg_smallFont, "ui_smallFont", "0.25", CVAR_ARCHIVE},
	{ &cg_bigFont, "ui_bigFont", "0.4", CVAR_ARCHIVE},
	{ &cg_oldRail, "cg_oldRail", "0", CVAR_ARCHIVE},
	{ &cg_oldRocket, "cg_oldRocket", "1", CVAR_ARCHIVE},
	{ &cg_leiEnhancement, "cg_leiEnhancement", "0", CVAR_ARCHIVE}, // LEILEI default off (in case of whiner)
	{ &cg_leiGoreNoise, "cg_leiGoreNoise", "0", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_leiBrassNoise, "cg_leiBrassNoise", "0", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_leiSuperGoreyAwesome, "cg_leiSuperGoreyAwesome", "0", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_leiDebug, "cg_leiDebug", "0", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_leiChibi, "cg_leiChibi", "0", CVAR_CHEAT}, // LEILEI 
	{ &cg_leiWidescreen, "cg_leiWidescreen", "1", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_deathcam, "cg_deathcam", "1", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_cameramode, "cg_cameramode", "0", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_cameraEyes, "cg_cameraEyes", "0", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_cameraEyes_Fwd, "cg_cameraEyes_Fwd", "3", CVAR_CHEAT}, // LEILEI 
	{ &cg_cameraEyes_Up, "cg_cameraEyes_Up", "3", CVAR_CHEAT}, // LEILEI 

	{ &cg_modelEyes_Up, "cg_modelEyes_Up", "3", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_modelEyes_Right, "cg_modelEyes_Right", "3", CVAR_ARCHIVE}, // LEILEI 
	{ &cg_modelEyes_Fwd, "cg_modelEyes_Fwd", "3", CVAR_ARCHIVE}, // LEILEI 

	{ &cg_oldPlasma, "cg_oldPlasma", "1", CVAR_ARCHIVE},
	//unlagged - client options
	{ &cg_delag, "cg_delag", "1", CVAR_ARCHIVE | CVAR_USERINFO},
	//	{ &cg_debugDelag, "cg_debugDelag", "0", CVAR_USERINFO | CVAR_CHEAT },
	//	{ &cg_drawBBox, "cg_drawBBox", "0", CVAR_CHEAT },
	{ &cg_cmdTimeNudge, "cg_cmdTimeNudge", "0", CVAR_ARCHIVE | CVAR_USERINFO},
	// this will be automagically copied from the server
	{ &sv_fps, "sv_fps", "20", CVAR_SYSTEMINFO},
	{ &cg_projectileNudge, "cg_projectileNudge", "0", CVAR_ARCHIVE},
	{ &cg_optimizePrediction, "cg_optimizePrediction", "1", CVAR_ARCHIVE},
	{ &cl_timeNudge, "cl_timeNudge", "0", CVAR_ARCHIVE},
	//	{ &cg_latentSnaps, "cg_latentSnaps", "0", CVAR_USERINFO | CVAR_CHEAT },
	//	{ &cg_latentCmds, "cg_latentCmds", "0", CVAR_USERINFO | CVAR_CHEAT },
	//	{ &cg_plOut, "cg_plOut", "0", CVAR_USERINFO | CVAR_CHEAT },
	//unlagged - client options
	{ &cg_trueLightning, "cg_trueLightning", "0.0", CVAR_ARCHIVE},
	{ &cg_music, "cg_music", "", CVAR_ARCHIVE},
	//	{ &cg_pmove_fixed, "cg_pmove_fixed", "0", CVAR_USERINFO | CVAR_ARCHIVE }

	{ &cg_fragmsgsize, "cg_fragmsgsize", "1.0", CVAR_ARCHIVE},
	{ &cg_crosshairPulse, "cg_crosshairPulse", "1", CVAR_ARCHIVE},

	{ &cg_differentCrosshairs, "cg_differentCrosshairs", "0", CVAR_ARCHIVE},
	{ &cg_ch1, "cg_ch1", "1", CVAR_ARCHIVE},
	{ &cg_ch1size, "cg_ch1size", "24", CVAR_ARCHIVE},
	{ &cg_ch2, "cg_ch2", "1", CVAR_ARCHIVE},
	{ &cg_ch2size, "cg_ch2size", "24", CVAR_ARCHIVE},
	{ &cg_ch3, "cg_ch3", "1", CVAR_ARCHIVE},
	{ &cg_ch3size, "cg_ch3size", "24", CVAR_ARCHIVE},
	{ &cg_ch4, "cg_ch4", "1", CVAR_ARCHIVE},
	{ &cg_ch4size, "cg_ch4size", "24", CVAR_ARCHIVE},
	{ &cg_ch5, "cg_ch5", "1", CVAR_ARCHIVE},
	{ &cg_ch5size, "cg_ch5size", "24", CVAR_ARCHIVE},
	{ &cg_ch6, "cg_ch6", "1", CVAR_ARCHIVE},
	{ &cg_ch6size, "cg_ch6size", "24", CVAR_ARCHIVE},
	{ &cg_ch7, "cg_ch7", "1", CVAR_ARCHIVE},
	{ &cg_ch7size, "cg_ch7size", "24", CVAR_ARCHIVE},
	{ &cg_ch8, "cg_ch8", "1", CVAR_ARCHIVE},
	{ &cg_ch8size, "cg_ch8size", "24", CVAR_ARCHIVE},
	{ &cg_ch9, "cg_ch9", "1", CVAR_ARCHIVE},
	{ &cg_ch9size, "cg_ch9size", "24", CVAR_ARCHIVE},
	{ &cg_ch10, "cg_ch10", "1", CVAR_ARCHIVE},
	{ &cg_ch10size, "cg_ch10size", "24", CVAR_ARCHIVE},
	{ &cg_ch11, "cg_ch11", "1", CVAR_ARCHIVE},
	{ &cg_ch11size, "cg_ch11size", "24", CVAR_ARCHIVE},
	{ &cg_ch12, "cg_ch12", "1", CVAR_ARCHIVE},
	{ &cg_ch12size, "cg_ch12size", "24", CVAR_ARCHIVE},
	{ &cg_ch13, "cg_ch13", "1", CVAR_ARCHIVE},
	{ &cg_ch13size, "cg_ch13size", "24", CVAR_ARCHIVE},

	{ &cg_crosshairColorRed, "cg_crosshairColorRed", "1.0", CVAR_ARCHIVE},
	{ &cg_crosshairColorGreen, "cg_crosshairColorGreen", "1.0", CVAR_ARCHIVE},
	{ &cg_crosshairColorBlue, "cg_crosshairColorBlue", "1.0", CVAR_ARCHIVE},

	{ &cg_weaponBarStyle, "cg_weaponBarStyle", "0", CVAR_ARCHIVE},
	{ &cg_weaponOrder, "cg_weaponOrder", "/1/2/4/3/6/7/8/5/9/", CVAR_ARCHIVE},
	{&cg_chatBeep, "cg_chatBeep", "1", CVAR_ARCHIVE},
	{&cg_teamChatBeep, "cg_teamChatBeep", "1", CVAR_ARCHIVE},
	{ &cg_muzzleflashStyle, "cg_muzzleflashStyle", "1", CVAR_ARCHIVE},
/* Neon_Knight: Enables MP checks. */
	{ &cg_missionpackChecks, "missionpackChecks", "1", CVAR_ARCHIVE},
/* /Neon_Knight */
/* Neon_Knight: Enables MP checks. */
	{ &cg_developer, "developer", "0", CVAR_CHEAT},
/* /Neon_Knight */
	{ &cg_customAnnouncer, "cg_customAnnouncer", "0", CVAR_ARCHIVE},
	{ &cg_announcePowerups, "cg_announcePowerups", "0", CVAR_ARCHIVE}
};

static int cvarTableSize = sizeof ( cvarTable) / sizeof ( cvarTable[0]);

/*
=================
CG_RegisterCvars
=================
 */
void CG_RegisterCvars(void) {
	int i;
	cvarTable_t *cv;
	char var[MAX_TOKEN_CHARS];

	for (i = 0, cv = cvarTable; i < cvarTableSize; i++, cv++) {
		trap_Cvar_Register(cv->vmCvar, cv->cvarName,
				cv->defaultString, cv->cvarFlags);
	}

	// see if we are also running the server on this machine
	trap_Cvar_VariableStringBuffer("sv_running", var, sizeof ( var));
	cgs.localServer = atoi(var);

	forceModelModificationCount = cg_forceModel.modificationCount;

	trap_Cvar_Register(NULL, "model", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE);
	trap_Cvar_Register(NULL, "headmodel", DEFAULT_MODEL, CVAR_USERINFO | CVAR_ARCHIVE);
	/* Neon_Knight: Missionpack checks, if != 0, enables this. */
	if (cg_missionpackChecks.integer) {
		trap_Cvar_Register(NULL, "team_model", DEFAULT_TEAM_MODEL, CVAR_USERINFO | CVAR_ARCHIVE);
		trap_Cvar_Register(NULL, "team_headmodel", DEFAULT_TEAM_HEAD, CVAR_USERINFO | CVAR_ARCHIVE);
	}
}

/*																																			
===================
CG_ForceModelChange
===================
 */
static void CG_ForceModelChange(void) {
	int i;

	for (i = 0; i < MAX_CLIENTS; i++) {
		const char *clientInfo;

		clientInfo = CG_ConfigString(CS_PLAYERS + i);
		if (!clientInfo[0]) {
			continue;
		}
		CG_NewClientInfo(i);
	}
}

/*
=================
CG_UpdateCvars
=================
 */
void CG_UpdateCvars(void) {
	int i;
	cvarTable_t *cv;

	for (i = 0, cv = cvarTable; i < cvarTableSize; i++, cv++) {
		//unlagged - client options
		// clamp the value between 0 and 999
		// negative values would suck - people could conceivably shoot other
		// players *long* after they had left the area, on purpose
		if (cv->vmCvar == &cg_cmdTimeNudge) {
			CG_Cvar_ClampInt(cv->cvarName, cv->vmCvar, 0, 999);
		}			// cl_timenudge less than -50 or greater than 50 doesn't actually
			// do anything more than -50 or 50 (actually the numbers are probably
			// closer to -30 and 30, but 50 is nice and round-ish)
			// might as well not feed the myth, eh?
		else if (cv->vmCvar == &cl_timeNudge) {
			CG_Cvar_ClampInt(cv->cvarName, cv->vmCvar, -50, 50);
		}			// don't let this go too high - no point
			/*else if ( cv->vmCvar == &cg_latentSnaps ) {
				CG_Cvar_ClampInt( cv->cvarName, cv->vmCvar, 0, 10 );
			}*/
			// don't let this get too large
			/*else if ( cv->vmCvar == &cg_latentCmds ) {
				CG_Cvar_ClampInt( cv->cvarName, cv->vmCvar, 0, MAX_LATENT_CMDS - 1 );
			}*/
			// no more than 100% packet loss
			/*else if ( cv->vmCvar == &cg_plOut ) {
				CG_Cvar_ClampInt( cv->cvarName, cv->vmCvar, 0, 100 );
			}*/
			//unlagged - client options
		else if (cv->vmCvar == &cg_errorDecay) {
			CG_Cvar_ClampInt(cv->cvarName, cv->vmCvar, 0, 250);
		}
		trap_Cvar_Update(cv->vmCvar);
	}

	// check for modications here

	// If team overlay is on, ask for updates from the server.  If its off,
	// let the server know so we don't receive it
	if (drawTeamOverlayModificationCount != cg_drawTeamOverlay.modificationCount) {
		drawTeamOverlayModificationCount = cg_drawTeamOverlay.modificationCount;

		if (cg_drawTeamOverlay.integer > 0) {
			trap_Cvar_Set("teamoverlay", "1");
		} else {
			trap_Cvar_Set("teamoverlay", "0");
		}
	}

	// if force model changed
	if (forceModelModificationCount != cg_forceModel.modificationCount) {
		forceModelModificationCount = cg_forceModel.modificationCount;
		CG_ForceModelChange();
	}
}

int CG_CrosshairPlayer(void) {
	if (cg.time > (cg.crosshairClientTime + 1000)) {
		return -1;
	}
	return cg.crosshairClientNum;
}

int CG_LastAttacker(void) {
	if (!cg.attackerTime) {
		return -1;
	}
	return cg.snap->ps.persistant[PERS_ATTACKER];
}

void QDECL CG_Printf(const char *msg, ...) {
	va_list argptr;
	char text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof (text), msg, argptr);
	va_end(argptr);

	trap_Print(text);
}

void QDECL CG_Error(const char *msg, ...) {
	va_list argptr;
	char text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof (text), msg, argptr);
	va_end(argptr);

	trap_Error(text);
}

void QDECL Com_Error(int level, const char *error, ...) {
	va_list argptr;
	char text[1024];

	va_start(argptr, error);
	Q_vsnprintf(text, sizeof (text), error, argptr);
	va_end(argptr);

	CG_Error("%s", text);
}

void QDECL Com_Printf(const char *msg, ...) {
	va_list argptr;
	char text[1024];

	va_start(argptr, msg);
	Q_vsnprintf(text, sizeof (text), msg, argptr);
	va_end(argptr);

	CG_Printf("%s", text);
}

/*
================
CG_Argv
================
 */
const char *CG_Argv(int arg) {
	static char buffer[MAX_STRING_CHARS];

	trap_Argv(arg, buffer, sizeof ( buffer));

	return buffer;
}


//========================================================================

/*
=================
CG_RegisterItemSounds

The server says this item is used on this level
=================
 */
static void CG_RegisterItemSounds(int itemNum) {
	gitem_t *item;
	char data[MAX_QPATH];
	char *s, *start;
	int len;

	item = &bg_itemlist[ itemNum ];

	if (item->pickup_sound) {
		trap_S_RegisterSound(item->pickup_sound, qfalse);
	}

	// parse the space seperated precache string for other media
	s = item->sounds;
	if (!s || !s[0])
		return;

	while (*s) {
		start = s;
		while (*s && *s != ' ') {
			s++;
		}

		len = s - start;
		if (len >= MAX_QPATH || len < 5) {
			CG_Error("PrecacheItem: %s has bad precache string",
					item->classname);
			return;
		}
		memcpy(data, start, len);
		data[len] = 0;
		if (*s) {
			s++;
		}

		if (strequals(data + len - 3, "wav")) {
			trap_S_RegisterSound(data, qfalse);
		}
	}
}

/*
=================
CG_RegisterSounds

called during a precache command
=================
 */
static void CG_RegisterSounds(void) {
	int i;
	char items[MAX_ITEMS + 1];
	char name[MAX_QPATH];
	const char *soundName;

	// voice commands
#ifdef MISSIONPACK
	CG_LoadVoiceChats();
#endif

	if (cg_customAnnouncer.integer) {
		// General announcements
		// Prepare to fight/Prepare your team
		cgs.media.countPrepareSound = trap_S_RegisterSound(ANNOUNCER_PREPARETOFIGHT, qtrue);
		if (CG_IsATeamGametype(cgs.gametype)) {
			cgs.media.countPrepareTeamSound = trap_S_RegisterSound(ANNOUNCER_PREPAREYOURTEAM, qtrue);
			cgs.media.redLeadsSound = trap_S_RegisterSound(ANNOUNCER_REDLEADS, qtrue);
			cgs.media.blueLeadsSound = trap_S_RegisterSound(ANNOUNCER_BLUELEADS, qtrue);
			cgs.media.teamsTiedSound = trap_S_RegisterSound(ANNOUNCER_TEAMSARETIED, qtrue);
			cgs.media.redScoredSound = trap_S_RegisterSound(ANNOUNCER_REDSCORES, qtrue);
			cgs.media.blueScoredSound = trap_S_RegisterSound(ANNOUNCER_BLUESCORES, qtrue);
		}
		else {
			cgs.media.takenLeadSound = trap_S_RegisterSound(ANNOUNCER_YOUHAVETAKENTHELEAD, qtrue);
			cgs.media.tiedLeadSound = trap_S_RegisterSound(ANNOUNCER_YOURETIEDFORTHELEAD, qtrue);
			cgs.media.lostLeadSound = trap_S_RegisterSound(ANNOUNCER_YOUHAVELOSTTHELEAD, qtrue);
			cgs.media.leadSwitchSound = trap_S_RegisterSound(ANNOUNCER_LEADSWITCH, qtrue);
		}
		// Countdown
		cgs.media.count10Sound = trap_S_RegisterSound(ANNOUNCER_TEN, qtrue);
		cgs.media.count9Sound = trap_S_RegisterSound(ANNOUNCER_NINE, qtrue);
		cgs.media.count8Sound = trap_S_RegisterSound(ANNOUNCER_EIGHT, qtrue);
		cgs.media.count7Sound = trap_S_RegisterSound(ANNOUNCER_SEVEN, qtrue);
		cgs.media.count6Sound = trap_S_RegisterSound(ANNOUNCER_SIX, qtrue);
		cgs.media.count5Sound = trap_S_RegisterSound(ANNOUNCER_FIVE, qtrue);
		cgs.media.count4Sound = trap_S_RegisterSound(ANNOUNCER_FOUR, qtrue);
		cgs.media.count3Sound = trap_S_RegisterSound(ANNOUNCER_THREE, qtrue);
		cgs.media.count2Sound = trap_S_RegisterSound(ANNOUNCER_TWO, qtrue);
		cgs.media.count1Sound = trap_S_RegisterSound(ANNOUNCER_ONE, qtrue);
		// Events
		cgs.media.countFightSound = trap_S_RegisterSound(ANNOUNCER_FIGHT, qtrue);
		cgs.media.fiveMinuteSound = trap_S_RegisterSound(ANNOUNCER_FIVEMINUTESLEFT, qtrue);
		cgs.media.threeMinuteSound = trap_S_RegisterSound(ANNOUNCER_THREEMINUTESLEFT, qtrue);
		cgs.media.twoMinuteSound = trap_S_RegisterSound(ANNOUNCER_TWOMINUTESLEFT, qtrue);
		cgs.media.oneMinuteSound = trap_S_RegisterSound(ANNOUNCER_ONEMINUTELEFT, qtrue);
		cgs.media.threeFragSound = trap_S_RegisterSound(ANNOUNCER_THREEFRAGSLEFT, qtrue);
		cgs.media.twoFragSound = trap_S_RegisterSound(ANNOUNCER_TWOFRAGSLEFT, qtrue);
		cgs.media.oneFragSound = trap_S_RegisterSound(ANNOUNCER_ONEFRAGLEFT, qtrue);
		cgs.media.suddenDeathSound = trap_S_RegisterSound(ANNOUNCER_SUDDENDEATH, qtrue);
		cgs.media.voteNow = trap_S_RegisterSound(ANNOUNCER_VOTENOW, qtrue);
		cgs.media.votePassed = trap_S_RegisterSound(ANNOUNCER_VOTEPASSED, qtrue);
		cgs.media.voteFailed = trap_S_RegisterSound(ANNOUNCER_VOTEFAILED, qtrue);
		cgs.media.winnerSound = trap_S_RegisterSound(ANNOUNCER_YOUWIN, qfalse);
		cgs.media.loserSound = trap_S_RegisterSound(ANNOUNCER_YOULOSE, qfalse);
		cgs.media.youSuckSound = trap_S_RegisterSound(ANNOUNCER_YOUSUCK, qfalse);
		// Awards
		cgs.media.impressiveSound = trap_S_RegisterSound(ANNOUNCER_IMPRESSIVE, qtrue);
		cgs.media.excellentSound = trap_S_RegisterSound(ANNOUNCER_EXCELLENT, qtrue);
		cgs.media.deniedSound = trap_S_RegisterSound(ANNOUNCER_DENIED, qtrue);
		cgs.media.humiliationSound = trap_S_RegisterSound(ANNOUNCER_HUMILIATION, qtrue);
		cgs.media.assistSound = trap_S_RegisterSound(ANNOUNCER_ASSIST, qtrue);
		cgs.media.defendSound = trap_S_RegisterSound(ANNOUNCER_DEFENSE, qtrue);
		cgs.media.firstImpressiveSound = trap_S_RegisterSound(ANNOUNCER_IMPRESSIVE_FIRST, qtrue);
		cgs.media.firstExcellentSound = trap_S_RegisterSound(ANNOUNCER_EXCELLENT_FIRST, qtrue);
		cgs.media.firstHumiliationSound = trap_S_RegisterSound(ANNOUNCER_HUMILIATION_FIRST, qtrue);
		// Items
		if (cg_announcePowerups.integer) {
			cgs.media.quadAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_QUADDAMAGE, qfalse);
			cgs.media.enviroAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_BATTLESUIT, qfalse);
			cgs.media.invisAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_INVISIBILITY, qfalse);
			cgs.media.regenAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_REGENERATION, qfalse);
			cgs.media.flightAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_FLIGHT, qfalse);
			cgs.media.hasteAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_HASTE, qfalse);
			cgs.media.teleporterAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_TELEPORTER, qfalse);
			cgs.media.medkitAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_MEDKIT, qfalse);
			cgs.media.invulAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_INVULNERABILITY, qfalse);
			cgs.media.kamiAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_KAMIKAZE, qfalse);
			//cgs.media.portalAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_PORTAL, qfalse);
			cgs.media.ammoRegenAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_AMMOREGEN, qfalse);
			cgs.media.doublerAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_DOUBLER, qfalse);
			cgs.media.guardAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_GUARD, qfalse);
			cgs.media.scoutAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_SCOUT, qfalse);
		}
		else {
			cgs.media.quadSpawnSound = trap_S_RegisterSound("sound/items/quaddamage.wav", qfalse);
			cgs.media.enviroSpawnSound = trap_S_RegisterSound("sound/items/protect.wav", qfalse);
			cgs.media.invisSpawnSound = trap_S_RegisterSound("sound/items/invisibility.wav", qfalse);
			cgs.media.regenSpawnSound = trap_S_RegisterSound("sound/items/regeneration.wav", qfalse);
			cgs.media.flightSpawnSound = trap_S_RegisterSound("sound/items/flight.wav", qfalse);
			cgs.media.hasteSpawnSound = trap_S_RegisterSound("sound/items/haste.wav", qfalse);
			cgs.media.teleporterSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.medkitSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.invulSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.kamiSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			//cgs.media.portalSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.ammoRegenSpawnSound = trap_S_RegisterSound("sound/items/cl_ammoregen.wav", qfalse);
			cgs.media.doublerSpawnSound = trap_S_RegisterSound("sound/items/cl_doubler.wav", qfalse);
			cgs.media.guardSpawnSound = trap_S_RegisterSound("sound/items/cl_guard.wav", qfalse);
			cgs.media.scoutSpawnSound = trap_S_RegisterSound("sound/items/cl_scout.wav", qfalse);
		}
		// Flag-based gametype announcements
		if (CG_UsesTeamFlags(cgs.gametype) || CG_UsesTheWhiteFlag(cgs.gametype) || cg_buildScript.integer) {
			cgs.media.youHaveFlagSound = trap_S_RegisterSound(ANNOUNCER_YOUHAVETHEFLAG, qtrue);
			cgs.media.holyShitSound = trap_S_RegisterSound(ANNOUNCER_HOLYSHIT, qtrue);
		}
		// Team-based flag gametype announcements
		if ((CG_UsesTeamFlags(cgs.gametype) && !CG_UsesTheWhiteFlag(cgs.gametype)) || cg_buildScript.integer) {
			cgs.media.redFlagReturnedSound = trap_S_RegisterSound(ANNOUNCER_REDFLAGRETURNED, qtrue);
			cgs.media.blueFlagReturnedSound = trap_S_RegisterSound(ANNOUNCER_BLUEFLAGRETURNED, qtrue);
			cgs.media.enemyTookYourFlagSound = trap_S_RegisterSound(ANNOUNCER_THEENEMYHASYOURFLAG, qtrue);
			cgs.media.yourTeamTookEnemyFlagSound = trap_S_RegisterSound(ANNOUNCER_YOURTEAMHASTHEENEMYFLAG, qtrue);
			cgs.media.redTakesBlueFlagAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_REDHASTHEBLUEFLAG, qfalse);
			cgs.media.blueTakesRedFlagAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_BLUEHASTHEREDFLAG, qfalse);
		}
		// Neutral flag gametype announcements
		if (CG_UsesTheWhiteFlag(cgs.gametype) || cg_buildScript.integer) {
			cgs.media.neutralFlagReturnedSound = trap_S_RegisterSound(ANNOUNCER_THEFLAGHASRETURNED, qtrue);
			cgs.media.yourTeamTookTheFlagSound = trap_S_RegisterSound(ANNOUNCER_YOURTEAMHASTHEFLAG, qtrue);
			cgs.media.enemyTookTheFlagSound = trap_S_RegisterSound(ANNOUNCER_THEENEMYHASTHEFLAG, qtrue);
			cgs.media.redTakesFlagAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_REDHASTHEFLAG, qfalse);
			cgs.media.blueTakesFlagAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_BLUEHASTHEFLAG, qfalse);
		}
		// Overload announcements
		if (cgs.gametype == GT_OBELISK || cg_buildScript.integer) {
			cgs.media.yourBaseIsUnderAttackSound = trap_S_RegisterSound(ANNOUNCER_YOURBASEISUNDERATTACK, qtrue);
			cgs.media.obeliskRespawnSound = trap_S_RegisterSound(ANNOUNCER_OBELISKSRESPAWNED, qfalse);
			cgs.media.redBaseUnderAttackAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_REDBASEUNDERATTACK, qfalse);
			cgs.media.blueBaseUnderAttackAnnouncerSound = trap_S_RegisterSound(ANNOUNCER_BLUEBASEUNDERATTACK, qfalse);
		}
	}
	else {
		// General announcements
		// Prepare to fight/Prepare your team
		cgs.media.countPrepareSound = trap_S_RegisterSound("sound/feedback/prepare.wav", qtrue);
		if (CG_IsATeamGametype(cgs.gametype)) {
			cgs.media.countPrepareTeamSound = trap_S_RegisterSound("sound/feedback/prepare_team.wav", qtrue);
			cgs.media.redLeadsSound = trap_S_RegisterSound("sound/feedback/redleads.wav", qtrue);
			cgs.media.blueLeadsSound = trap_S_RegisterSound("sound/feedback/blueleads.wav", qtrue);
			cgs.media.teamsTiedSound = trap_S_RegisterSound("sound/feedback/teamstied.wav", qtrue);
			cgs.media.redScoredSound = trap_S_RegisterSound("sound/teamplay/voc_red_scores.wav", qtrue);
			cgs.media.blueScoredSound = trap_S_RegisterSound("sound/teamplay/voc_blue_scores.wav", qtrue);
		}
		else {
			cgs.media.takenLeadSound = trap_S_RegisterSound("sound/feedback/takenlead.wav", qtrue);
			cgs.media.tiedLeadSound = trap_S_RegisterSound("sound/feedback/tiedlead.wav", qtrue);
			cgs.media.lostLeadSound = trap_S_RegisterSound("sound/feedback/lostlead.wav", qtrue);
			cgs.media.leadSwitchSound = trap_S_RegisterSound("sound/feedback/leadswitch.wav", qtrue);
		}
		// Events
		cgs.media.count10Sound = trap_S_RegisterSound("sound/feedback/ten.wav", qtrue);
		cgs.media.count9Sound = trap_S_RegisterSound("sound/feedback/nine.wav", qtrue);
		cgs.media.count8Sound = trap_S_RegisterSound("sound/feedback/eight.wav", qtrue);
		cgs.media.count7Sound = trap_S_RegisterSound("sound/feedback/seven.wav", qtrue);
		cgs.media.count6Sound = trap_S_RegisterSound("sound/feedback/six.wav", qtrue);
		cgs.media.count5Sound = trap_S_RegisterSound("sound/feedback/five.wav", qtrue);
		cgs.media.count4Sound = trap_S_RegisterSound("sound/feedback/four.wav", qtrue);
		cgs.media.count3Sound = trap_S_RegisterSound("sound/feedback/three.wav", qtrue);
		cgs.media.count2Sound = trap_S_RegisterSound("sound/feedback/two.wav", qtrue);
		cgs.media.count1Sound = trap_S_RegisterSound("sound/feedback/one.wav", qtrue);
		cgs.media.countFightSound = trap_S_RegisterSound("sound/feedback/fight.wav", qtrue);
		cgs.media.fiveMinuteSound = trap_S_RegisterSound("sound/feedback/5_minute.wav", qtrue);
		cgs.media.threeMinuteSound = trap_S_RegisterSound("sound/feedback/3_minute.wav", qtrue);
		cgs.media.twoMinuteSound = trap_S_RegisterSound("sound/feedback/2_minute.wav", qtrue);
		cgs.media.oneMinuteSound = trap_S_RegisterSound("sound/feedback/1_minute.wav", qtrue);
		cgs.media.threeCaptureSound = trap_S_RegisterSound("sound/feedback/3_captures.wav", qtrue);
		cgs.media.twoCaptureSound = trap_S_RegisterSound("sound/feedback/2_captures.wav", qtrue);
		cgs.media.oneCaptureSound = trap_S_RegisterSound("sound/feedback/1_capture.wav", qtrue);
		cgs.media.threeFragSound = trap_S_RegisterSound("sound/feedback/3_frags.wav", qtrue);
		cgs.media.twoFragSound = trap_S_RegisterSound("sound/feedback/2_frags.wav", qtrue);
		cgs.media.oneFragSound = trap_S_RegisterSound("sound/feedback/1_frag.wav", qtrue);
		cgs.media.suddenDeathSound = trap_S_RegisterSound("sound/feedback/sudden_death.wav", qtrue);
		cgs.media.voteNow = trap_S_RegisterSound("sound/feedback/vote_now.wav", qtrue);
		cgs.media.votePassed = trap_S_RegisterSound("sound/feedback/vote_passed.wav", qtrue);
		cgs.media.voteFailed = trap_S_RegisterSound("sound/feedback/vote_failed.wav", qtrue);
		cgs.media.winnerSound = trap_S_RegisterSound("sound/feedback/voc_youwin.wav", qfalse);
		cgs.media.loserSound = trap_S_RegisterSound("sound/feedback/voc_youlose.wav", qfalse);
		cgs.media.youSuckSound = trap_S_RegisterSound("sound/misc/yousuck.wav", qfalse);
		// Awards
		cgs.media.impressiveSound = trap_S_RegisterSound("sound/feedback/impressive.wav", qtrue);
		cgs.media.excellentSound = trap_S_RegisterSound("sound/feedback/excellent.wav", qtrue);
		cgs.media.deniedSound = trap_S_RegisterSound("sound/feedback/denied.wav", qtrue);
		cgs.media.humiliationSound = trap_S_RegisterSound("sound/feedback/humiliation.wav", qtrue);
		cgs.media.assistSound = trap_S_RegisterSound("sound/feedback/assist.wav", qtrue);
		cgs.media.defendSound = trap_S_RegisterSound("sound/feedback/defense.wav", qtrue);
		cgs.media.firstImpressiveSound = trap_S_RegisterSound("sound/feedback/first_impressive.wav", qtrue);
		cgs.media.firstExcellentSound = trap_S_RegisterSound("sound/feedback/first_excellent.wav", qtrue);
		cgs.media.firstHumiliationSound = trap_S_RegisterSound("sound/feedback/first_gauntlet.wav", qtrue);
		// Items
		if (cg_announcePowerups.integer) {
			cgs.media.quadAnnouncerSound = trap_S_RegisterSound("sound/items/quaddamage.wav", qfalse);
			cgs.media.enviroAnnouncerSound = trap_S_RegisterSound("sound/items/protect.wav", qfalse);
			cgs.media.invisAnnouncerSound = trap_S_RegisterSound("sound/items/invisibility.wav", qfalse);
			cgs.media.regenAnnouncerSound = trap_S_RegisterSound("sound/items/regeneration.wav", qfalse);
			cgs.media.flightAnnouncerSound = trap_S_RegisterSound("sound/items/flight.wav", qfalse);
			cgs.media.hasteAnnouncerSound = trap_S_RegisterSound("sound/items/haste.wav", qfalse);
			cgs.media.teleporterAnnouncerSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.medkitAnnouncerSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.invulAnnouncerSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.kamiAnnouncerSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			//cgs.media.portalAnnouncerSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.ammoRegenAnnouncerSound = trap_S_RegisterSound("sound/items/cl_ammoregen.wav", qfalse);
			cgs.media.doublerAnnouncerSound = trap_S_RegisterSound("sound/items/cl_doubler.wav", qfalse);
			cgs.media.guardAnnouncerSound = trap_S_RegisterSound("sound/items/cl_guard.wav", qfalse);
			cgs.media.scoutAnnouncerSound = trap_S_RegisterSound("sound/items/cl_scout.wav", qfalse);
		}
		else {
			cgs.media.quadSpawnSound = trap_S_RegisterSound("sound/items/quaddamage.wav", qfalse);
			cgs.media.enviroSpawnSound = trap_S_RegisterSound("sound/items/protect.wav", qfalse);
			cgs.media.invisSpawnSound = trap_S_RegisterSound("sound/items/invisibility.wav", qfalse);
			cgs.media.regenSpawnSound = trap_S_RegisterSound("sound/items/regeneration.wav", qfalse);
			cgs.media.flightSpawnSound = trap_S_RegisterSound("sound/items/flight.wav", qfalse);
			cgs.media.hasteSpawnSound = trap_S_RegisterSound("sound/items/haste.wav", qfalse);
			cgs.media.teleporterSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.medkitSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.invulSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.kamiSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			//cgs.media.portalSpawnSound = trap_S_RegisterSound("sound/items/holdable.wav", qfalse);
			cgs.media.ammoRegenSpawnSound = trap_S_RegisterSound("sound/items/cl_ammoregen.wav", qfalse);
			cgs.media.doublerSpawnSound = trap_S_RegisterSound("sound/items/cl_doubler.wav", qfalse);
			cgs.media.guardSpawnSound = trap_S_RegisterSound("sound/items/cl_guard.wav", qfalse);
			cgs.media.scoutSpawnSound = trap_S_RegisterSound("sound/items/cl_scout.wav", qfalse);
		}
		// Flag-based gametype announcements
		if (CG_UsesTeamFlags(cgs.gametype) || CG_UsesTheWhiteFlag(cgs.gametype) || cg_buildScript.integer) {
			cgs.media.youHaveFlagSound = trap_S_RegisterSound("sound/teamplay/voc_you_flag.wav", qtrue);
			cgs.media.holyShitSound = trap_S_RegisterSound("sound/feedback/voc_holyshit.wav", qtrue);
		}
		// Team-based flag gametype announcements
		if ((CG_UsesTeamFlags(cgs.gametype) && !CG_UsesTheWhiteFlag(cgs.gametype)) || cg_buildScript.integer) {
			cgs.media.redFlagReturnedSound = trap_S_RegisterSound("sound/teamplay/voc_red_returned.wav", qtrue);
			cgs.media.blueFlagReturnedSound = trap_S_RegisterSound("sound/teamplay/voc_blue_returned.wav", qtrue);
			cgs.media.enemyTookYourFlagSound = trap_S_RegisterSound("sound/teamplay/voc_enemy_flag.wav", qtrue);
			cgs.media.yourTeamTookEnemyFlagSound = trap_S_RegisterSound("sound/teamplay/voc_team_flag.wav", qtrue);
			cgs.media.redTakesBlueFlagAnnouncerSound = trap_S_RegisterSound("sound/teamplay/voc_redtookblue.wav", qfalse);
			cgs.media.blueTakesRedFlagAnnouncerSound = trap_S_RegisterSound("sound/teamplay/voc_bluetookred.wav", qfalse);
		}
		// Neutral flag gametype announcements
		if (CG_UsesTheWhiteFlag(cgs.gametype) || cg_buildScript.integer) {
			cgs.media.neutralFlagReturnedSound = trap_S_RegisterSound("sound/teamplay/flagreturn_opponent.wav", qtrue);
			cgs.media.yourTeamTookTheFlagSound = trap_S_RegisterSound("sound/teamplay/voc_team_1flag.wav", qtrue);
			cgs.media.enemyTookTheFlagSound = trap_S_RegisterSound("sound/teamplay/voc_enemy_1flag.wav", qtrue);
			cgs.media.redTakesFlagAnnouncerSound = trap_S_RegisterSound("sound/teamplay/voc_redtookneutral.wav", qfalse);
			cgs.media.blueTakesFlagAnnouncerSound = trap_S_RegisterSound("sound/teamplay/voc_bluetookneutral.wav", qfalse);
		}
		// Overload announcements
		if (cgs.gametype == GT_OBELISK || cg_buildScript.integer) {
			cgs.media.yourBaseIsUnderAttackSound = trap_S_RegisterSound("sound/teamplay/voc_base_attack.wav", qtrue);
			cgs.media.obeliskRespawnSound = trap_S_RegisterSound("sound/items/obelisk_respawn.wav", qfalse);
			cgs.media.redBaseUnderAttackAnnouncerSound = trap_S_RegisterSound("sound/teamplay/voc_redunderattack.wav", qfalse);
			cgs.media.blueBaseUnderAttackAnnouncerSound = trap_S_RegisterSound("sound/teamplay/voc_blueunderattack.wav", qfalse);
		}
	}

	// loadingscreen
#ifdef SCRIPTHUD
	CG_UpdateSoundFraction(0.33f);
	CG_UpdateMediaFraction(0.20f);
#endif
	// end loadingscreen

	// N_G: Another condition that makes no sense to me, see for
	// yourself if you really meant this
	// Sago: Makes perfect sense: Load team game stuff if the gametype is a teamgame and not an exception (like GT_LMS)
	if (CG_IsATeamGametype(cgs.gametype) || cg_buildScript.integer) {

		cgs.media.captureAwardSound = trap_S_RegisterSound("sound/teamplay/flagcapture_yourteam.wav", qtrue);
		cgs.media.hitTeamSound = trap_S_RegisterSound("sound/feedback/hit_teammate.wav", qtrue);

		cgs.media.captureYourTeamSound = trap_S_RegisterSound("sound/teamplay/flagcapture_yourteam.wav", qtrue);
		cgs.media.captureOpponentSound = trap_S_RegisterSound("sound/teamplay/flagcapture_opponent.wav", qtrue);

		cgs.media.returnYourTeamSound = trap_S_RegisterSound("sound/teamplay/flagreturn_yourteam.wav", qtrue);
		cgs.media.returnOpponentSound = trap_S_RegisterSound("sound/teamplay/flagreturn_opponent.wav", qtrue);

		cgs.media.takenYourTeamSound = trap_S_RegisterSound("sound/teamplay/flagtaken_yourteam.wav", qtrue);
		cgs.media.takenOpponentSound = trap_S_RegisterSound("sound/teamplay/flagtaken_opponent.wav", qtrue);

#ifdef SCRIPTHUD
		if (cgs.gametype == GT_OBELISK || cg_buildScript.integer) {
			// loadingscreen
			CG_UpdateSoundFraction(0.60f);
			CG_UpdateMediaFraction(0.30f);
			// end loadingscreen
		}
#endif
	}

	cgs.media.tracerSound = trap_S_RegisterSound("sound/weapons/machinegun/buletby1.wav", qfalse);
	cgs.media.selectSound = trap_S_RegisterSound("sound/weapons/change.wav", qfalse);
	cgs.media.wearOffSound = trap_S_RegisterSound("sound/items/wearoff.wav", qfalse);
	cgs.media.useNothingSound = trap_S_RegisterSound("sound/items/use_nothing.wav", qfalse);
	cgs.media.gibSound = trap_S_RegisterSound("sound/player/gibsplt1.wav", qfalse);
	cgs.media.gibBounce1Sound = trap_S_RegisterSound("sound/player/gibimp1.wav", qfalse);
	cgs.media.gibBounce2Sound = trap_S_RegisterSound("sound/player/gibimp2.wav", qfalse);
	cgs.media.gibBounce3Sound = trap_S_RegisterSound("sound/player/gibimp3.wav", qfalse);


	// LEILEI

	cgs.media.lspl1Sound = trap_S_RegisterSound("sound/le/splat1.wav", qfalse);
	cgs.media.lspl2Sound = trap_S_RegisterSound("sound/le/splat2.wav", qfalse);
	cgs.media.lspl3Sound = trap_S_RegisterSound("sound/le/splat3.wav", qfalse);

	cgs.media.lbul1Sound = trap_S_RegisterSound("sound/le/bullet1.wav", qfalse);
	cgs.media.lbul2Sound = trap_S_RegisterSound("sound/le/bullet2.wav", qfalse);
	cgs.media.lbul3Sound = trap_S_RegisterSound("sound/le/bullet3.wav", qfalse);

	cgs.media.lshl1Sound = trap_S_RegisterSound("sound/le/shell1.wav", qfalse);
	cgs.media.lshl2Sound = trap_S_RegisterSound("sound/le/shell2.wav", qfalse);
	cgs.media.lshl3Sound = trap_S_RegisterSound("sound/le/shell3.wav", qfalse);

	cgs.media.useInvulnerabilitySound = trap_S_RegisterSound("sound/items/invul_activate.wav", qfalse);
	cgs.media.invulnerabilityImpactSound1 = trap_S_RegisterSound("sound/items/invul_impact_01.wav", qfalse);
	cgs.media.invulnerabilityImpactSound2 = trap_S_RegisterSound("sound/items/invul_impact_02.wav", qfalse);
	cgs.media.invulnerabilityImpactSound3 = trap_S_RegisterSound("sound/items/invul_impact_03.wav", qfalse);
	cgs.media.invulnerabilityJuicedSound = trap_S_RegisterSound("sound/items/invul_juiced.wav", qfalse);

	cgs.media.obeliskHitSound1 = trap_S_RegisterSound("sound/items/obelisk_hit_01.wav", qfalse);
	cgs.media.obeliskHitSound2 = trap_S_RegisterSound("sound/items/obelisk_hit_02.wav", qfalse);
	cgs.media.obeliskHitSound3 = trap_S_RegisterSound("sound/items/obelisk_hit_03.wav", qfalse);

	cgs.media.teleInSound = trap_S_RegisterSound("sound/world/telein.wav", qfalse);
	cgs.media.teleOutSound = trap_S_RegisterSound("sound/world/teleout.wav", qfalse);
	cgs.media.respawnSound = trap_S_RegisterSound("sound/items/respawn1.wav", qfalse);

	cgs.media.noAmmoSound = trap_S_RegisterSound("sound/weapons/noammo.wav", qfalse);

	cgs.media.talkSound = trap_S_RegisterSound("sound/player/talk.wav", qfalse);
	cgs.media.landSound = trap_S_RegisterSound("sound/player/land1.wav", qfalse);

	switch (cg_hitsound.integer) {

		case 0:
		default:
			cgs.media.hitSound = trap_S_RegisterSound("sound/feedback/hit.wav", qfalse);
	};

#ifdef MISSIONPACK
	cgs.media.hitSoundHighArmor = trap_S_RegisterSound("sound/feedback/hithi.wav", qfalse);
	cgs.media.hitSoundLowArmor = trap_S_RegisterSound("sound/feedback/hitlo.wav", qfalse);
#endif

	cgs.media.watrInSound = trap_S_RegisterSound("sound/player/watr_in.wav", qfalse);
	cgs.media.watrOutSound = trap_S_RegisterSound("sound/player/watr_out.wav", qfalse);
	cgs.media.watrUnSound = trap_S_RegisterSound("sound/player/watr_un.wav", qfalse);

	cgs.media.jumpPadSound = trap_S_RegisterSound("sound/world/jumppad.wav", qfalse);

	for (i = 0; i < 4; i++) {
		Com_sprintf(name, sizeof (name), "sound/player/footsteps/step%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_NORMAL][i] = trap_S_RegisterSound(name, qfalse);

		Com_sprintf(name, sizeof (name), "sound/player/footsteps/boot%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_BOOT][i] = trap_S_RegisterSound(name, qfalse);

		Com_sprintf(name, sizeof (name), "sound/player/footsteps/flesh%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_FLESH][i] = trap_S_RegisterSound(name, qfalse);

		Com_sprintf(name, sizeof (name), "sound/player/footsteps/mech%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_MECH][i] = trap_S_RegisterSound(name, qfalse);

		Com_sprintf(name, sizeof (name), "sound/player/footsteps/energy%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_ENERGY][i] = trap_S_RegisterSound(name, qfalse);

		Com_sprintf(name, sizeof (name), "sound/player/footsteps/splash%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_SPLASH][i] = trap_S_RegisterSound(name, qfalse);

		Com_sprintf(name, sizeof (name), "sound/player/footsteps/clank%i.wav", i + 1);
		cgs.media.footsteps[FOOTSTEP_METAL][i] = trap_S_RegisterSound(name, qfalse);
	}

	// only register the items that the server says we need
	Q_strncpyz(items, CG_ConfigString(CS_ITEMS), sizeof (items));

	for (i = 1; i < bg_numItems; i++) {
		//		if ( items[ i ] == '1' || cg_buildScript.integer ) {
		CG_RegisterItemSounds(i);
		//		}
	}

	for (i = 1; i < MAX_SOUNDS; i++) {
		soundName = CG_ConfigString(CS_SOUNDS + i);
		if (!soundName[0]) {
			break;
		}
		if (soundName[0] == '*') {
			continue; // custom sound
		}
		cgs.gameSounds[i] = trap_S_RegisterSound(soundName, qfalse);
	}

	// FIXME: only needed with item

	// loadingscreen
#ifdef SCRIPTHUD
	CG_UpdateSoundFraction(0.85f);
	CG_UpdateMediaFraction(0.50f);
#endif
	// end loadingscreen
	cgs.media.flightSound = trap_S_RegisterSound("sound/items/flight.wav", qfalse);
	cgs.media.medkitSound = trap_S_RegisterSound("sound/items/use_medkit.wav", qfalse);
	cgs.media.quadSound = trap_S_RegisterSound("sound/items/damage3.wav", qfalse);
	cgs.media.sfx_ric1 = trap_S_RegisterSound("sound/weapons/machinegun/ric1.wav", qfalse);
	cgs.media.sfx_ric2 = trap_S_RegisterSound("sound/weapons/machinegun/ric2.wav", qfalse);
	cgs.media.sfx_ric3 = trap_S_RegisterSound("sound/weapons/machinegun/ric3.wav", qfalse);
	cgs.media.sfx_railg = trap_S_RegisterSound("sound/weapons/railgun/railgf1a.wav", qfalse);
	cgs.media.sfx_rockexp = trap_S_RegisterSound("sound/weapons/rocket/rocklx1a.wav", qfalse);
	cgs.media.sfx_plasmaexp = trap_S_RegisterSound("sound/weapons/plasma/plasmx1a.wav", qfalse);
	cgs.media.sfx_proxexp = trap_S_RegisterSound("sound/weapons/proxmine/wstbexpl.wav", qfalse);
	cgs.media.sfx_nghit = trap_S_RegisterSound("sound/weapons/nailgun/wnalimpd.wav", qfalse);
	cgs.media.sfx_nghitflesh = trap_S_RegisterSound("sound/weapons/nailgun/wnalimpl.wav", qfalse);
	cgs.media.sfx_nghitmetal = trap_S_RegisterSound("sound/weapons/nailgun/wnalimpm.wav", qfalse);
	cgs.media.sfx_chghit = trap_S_RegisterSound("sound/weapons/vulcan/wvulimpd.wav", qfalse);
	cgs.media.sfx_chghitflesh = trap_S_RegisterSound("sound/weapons/vulcan/wvulimpl.wav", qfalse);
	cgs.media.sfx_chghitmetal = trap_S_RegisterSound("sound/weapons/vulcan/wvulimpm.wav", qfalse);
	cgs.media.weaponHoverSound = trap_S_RegisterSound("sound/weapons/weapon_hover.wav", qfalse);
	cgs.media.kamikazeExplodeSound = trap_S_RegisterSound("sound/items/kam_explode.wav", qfalse);
	cgs.media.kamikazeImplodeSound = trap_S_RegisterSound("sound/items/kam_implode.wav", qfalse);
	cgs.media.kamikazeFarSound = trap_S_RegisterSound("sound/items/kam_explode_far.wav", qfalse);

	cgs.media.wstbimplSound = trap_S_RegisterSound("sound/weapons/proxmine/wstbimpl.wav", qfalse);
	cgs.media.wstbimpmSound = trap_S_RegisterSound("sound/weapons/proxmine/wstbimpm.wav", qfalse);
	cgs.media.wstbimpdSound = trap_S_RegisterSound("sound/weapons/proxmine/wstbimpd.wav", qfalse);
	cgs.media.wstbactvSound = trap_S_RegisterSound("sound/weapons/proxmine/wstbactv.wav", qfalse);

	cgs.media.regenSound = trap_S_RegisterSound("sound/items/regen.wav", qfalse);
	cgs.media.protectSound = trap_S_RegisterSound("sound/items/protect3.wav", qfalse);
	cgs.media.n_healthSound = trap_S_RegisterSound("sound/items/n_health.wav", qfalse);
	cgs.media.hgrenb1aSound = trap_S_RegisterSound("sound/weapons/grenade/hgrenb1a.wav", qfalse);
	cgs.media.hgrenb2aSound = trap_S_RegisterSound("sound/weapons/grenade/hgrenb2a.wav", qfalse);

#ifdef MISSIONPACK
	trap_S_RegisterSound("sound/player/sergei/death1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/death2.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/death3.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/jump1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/pain25_1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/pain75_1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/pain100_1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/falling1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/gasp.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/drown.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/fall1.wav", qfalse);
	trap_S_RegisterSound("sound/player/sergei/taunt.wav", qfalse);

	trap_S_RegisterSound("sound/player/kyonshi/death1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/death2.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/death3.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/jump1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/pain25_1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/pain75_1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/pain100_1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/falling1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/gasp.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/drown.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/fall1.wav", qfalse);
	trap_S_RegisterSound("sound/player/kyonshi/taunt.wav", qfalse);
#endif

}


//===================================================================================

/*
=================
CG_RegisterGraphics

This function may execute for a couple of minutes with a slow disk.
=================
 */
static void CG_RegisterGraphics(void) {
	int i;
	char items[MAX_ITEMS + 1];
	static char *sb_nums[11] = {
		"gfx/2d/numbers/zero_32b",
		"gfx/2d/numbers/one_32b",
		"gfx/2d/numbers/two_32b",
		"gfx/2d/numbers/three_32b",
		"gfx/2d/numbers/four_32b",
		"gfx/2d/numbers/five_32b",
		"gfx/2d/numbers/six_32b",
		"gfx/2d/numbers/seven_32b",
		"gfx/2d/numbers/eight_32b",
		"gfx/2d/numbers/nine_32b",
		"gfx/2d/numbers/minus_32b",
	};

	// clear any references to old media
	memset(&cg.refdef, 0, sizeof ( cg.refdef));
	trap_R_ClearScene();

	CG_LoadingString(cgs.mapname);

	trap_R_LoadWorldMap(cgs.mapname);

	// precache status bar pics
	CG_LoadingString("game media");

	for (i = 0; i < 11; i++) {
		cgs.media.numberShaders[i] = trap_R_RegisterShader(sb_nums[i]);
	}

	cgs.media.botSkillShaders[0] = trap_R_RegisterShader("menu/art/skill1.tga");
	cgs.media.botSkillShaders[1] = trap_R_RegisterShader("menu/art/skill2.tga");
	cgs.media.botSkillShaders[2] = trap_R_RegisterShader("menu/art/skill3.tga");
	cgs.media.botSkillShaders[3] = trap_R_RegisterShader("menu/art/skill4.tga");
	cgs.media.botSkillShaders[4] = trap_R_RegisterShader("menu/art/skill5.tga");

	cgs.media.viewBloodShader = trap_R_RegisterShader("viewBloodBlend");

	cgs.media.deferShader = trap_R_RegisterShaderNoMip("gfx/2d/defer.tga");

	cgs.media.scoreboardName = trap_R_RegisterShaderNoMip("menu/tab/name.tga");
	cgs.media.scoreboardPing = trap_R_RegisterShaderNoMip("menu/tab/ping.tga");
	cgs.media.scoreboardScore = trap_R_RegisterShaderNoMip("menu/tab/score.tga");
	cgs.media.scoreboardTime = trap_R_RegisterShaderNoMip("menu/tab/time.tga");

	cgs.media.smokePuffShader = trap_R_RegisterShader("smokePuff");
	cgs.media.smokePuffRageProShader = trap_R_RegisterShader("smokePuffRagePro");
	cgs.media.shotgunSmokePuffShader = trap_R_RegisterShader("shotgunSmokePuff");
	cgs.media.nailPuffShader = trap_R_RegisterShader("nailtrail");
	cgs.media.blueProxMine = trap_R_RegisterModel("models/weaphits/proxmineb.md3");
	cgs.media.plasmaBallShader = trap_R_RegisterShader("sprites/plasma1");
	cgs.media.bloodTrailShader = trap_R_RegisterShader("bloodTrail");
	cgs.media.lagometerShader = trap_R_RegisterShader("lagometer");
	cgs.media.connectionShader = trap_R_RegisterShader("disconnected");

	cgs.media.waterBubbleShader = trap_R_RegisterShader("waterBubble");

	cgs.media.tracerShader = trap_R_RegisterShader("gfx/misc/tracer");
	cgs.media.selectShader = trap_R_RegisterShader("gfx/2d/select");

	cgs.media.bfgShader = trap_R_RegisterShader("icons/iconw_bfg");
	cgs.media.chaingunShader = trap_R_RegisterShader("icons/iconw_chaingun");
	cgs.media.gauntletShader = trap_R_RegisterShader("icons/iconw_gauntlet");
	cgs.media.grapplehookShader = trap_R_RegisterShader("icons/iconw_grapple");
	cgs.media.grenadeShader = trap_R_RegisterShader("icons/iconw_grenade");
	cgs.media.kamikazeShader = trap_R_RegisterShader("icons/iconw_kamikaze");
	cgs.media.lightninggunShader = trap_R_RegisterShader("icons/iconw_lightning");
	cgs.media.machinegunShader = trap_R_RegisterShader("icons/iconw_machinegun");
	cgs.media.nailgunShader = trap_R_RegisterShader("icons/iconw_nailgun");
	cgs.media.plasmaShader = trap_R_RegisterShader("icons/iconw_plasma");
	cgs.media.proxlauncherShader = trap_R_RegisterShader("icons/iconw_proxlauncher");
	cgs.media.railgunShader = trap_R_RegisterShader("icons/iconw_railgun");
	cgs.media.rocketShader = trap_R_RegisterShader("icons/iconw_rocket");
	cgs.media.shotgunShader = trap_R_RegisterShader("icons/iconw_shotgun");
	cgs.media.skullShader = trap_R_RegisterShader("icons/skull_red");

	for (i = 0; i < NUM_CROSSHAIRS; i++) {
		if (i < 10)
			cgs.media.crosshairShader[i] = trap_R_RegisterShader(va("gfx/2d/crosshair%c", 'a' + i));
		else
			cgs.media.crosshairShader[i] = trap_R_RegisterShader(va("gfx/2d/crosshair%02d", i - 10));
	}

	cgs.media.backTileShader = trap_R_RegisterShader("gfx/2d/backtile");
	cgs.media.noammoShader = trap_R_RegisterShader("icons/noammo");

	// powerup shaders
	cgs.media.quadShader = trap_R_RegisterShader("powerups/quad");
	cgs.media.quadWeaponShader = trap_R_RegisterShader("powerups/quadWeapon");
	cgs.media.battleSuitShader = trap_R_RegisterShader("powerups/battleSuit");
	cgs.media.battleWeaponShader = trap_R_RegisterShader("powerups/battleWeapon");
	cgs.media.invisShader = trap_R_RegisterShader("powerups/invisibility");
	cgs.media.regenShader = trap_R_RegisterShader("powerups/regen");
	cgs.media.hastePuffShader = trap_R_RegisterShader("hasteSmokePuff");

	if (CG_UsesTeamFlags(cgs.gametype) || cgs.gametype == GT_HARVESTER || cg_buildScript.integer) {
		cgs.media.redCubeModel = trap_R_RegisterModel("models/powerups/orb/r_orb.md3");
		cgs.media.blueCubeModel = trap_R_RegisterModel("models/powerups/orb/b_orb.md3");
		cgs.media.redCubeIcon = trap_R_RegisterShader("icons/skull_red");
		cgs.media.blueCubeIcon = trap_R_RegisterShader("icons/skull_blue");
	}

	if (CG_IsATeamGametype(cgs.gametype)) {
		cgs.media.redOverlay = trap_R_RegisterShader("playeroverlays/playerSuit1_Red");
		cgs.media.blueOverlay = trap_R_RegisterShader("playeroverlays/playerSuit1_Blue");
	} else {
		cgs.media.neutralOverlay = trap_R_RegisterShader("playeroverlays/playerSuit1_Neutral");
	}

	//For Double Domination:
	if (cgs.gametype == GT_DOUBLE_D) {
		cgs.media.ddPointSkinA[TEAM_RED] = trap_R_RegisterShaderNoMip("icons/icona_red");
		cgs.media.ddPointSkinA[TEAM_BLUE] = trap_R_RegisterShaderNoMip("icons/icona_blue");
		cgs.media.ddPointSkinA[TEAM_FREE] = trap_R_RegisterShaderNoMip("icons/icona_white");
		cgs.media.ddPointSkinA[TEAM_NONE] = trap_R_RegisterShaderNoMip("icons/noammo");

		cgs.media.ddPointSkinB[TEAM_RED] = trap_R_RegisterShaderNoMip("icons/iconb_red");
		cgs.media.ddPointSkinB[TEAM_BLUE] = trap_R_RegisterShaderNoMip("icons/iconb_blue");
		cgs.media.ddPointSkinB[TEAM_FREE] = trap_R_RegisterShaderNoMip("icons/iconb_white");
		cgs.media.ddPointSkinB[TEAM_NONE] = trap_R_RegisterShaderNoMip("icons/noammo");
	}

	if (CG_UsesTeamFlags(cgs.gametype) || CG_UsesTheWhiteFlag(cgs.gametype) || cgs.gametype == GT_HARVESTER || cg_buildScript.integer) {
		cgs.media.redFlagModel = trap_R_RegisterModel("models/flags/r_flag.md3");
		cgs.media.blueFlagModel = trap_R_RegisterModel("models/flags/b_flag.md3");
		cgs.media.neutralFlagModel = trap_R_RegisterModel("models/flags/n_flag.md3");
		cgs.media.redFlagShader[0] = trap_R_RegisterShaderNoMip("icons/iconf_red1");
		cgs.media.redFlagShader[1] = trap_R_RegisterShaderNoMip("icons/iconf_red2");
		cgs.media.redFlagShader[2] = trap_R_RegisterShaderNoMip("icons/iconf_red3");
		cgs.media.blueFlagShader[0] = trap_R_RegisterShaderNoMip("icons/iconf_blu1");
		cgs.media.blueFlagShader[1] = trap_R_RegisterShaderNoMip("icons/iconf_blu2");
		cgs.media.blueFlagShader[2] = trap_R_RegisterShaderNoMip("icons/iconf_blu3");
		cgs.media.flagPoleModel = trap_R_RegisterModel("models/flag2/flagpole.md3");
		cgs.media.flagFlapModel = trap_R_RegisterModel("models/flag2/flagflap3.md3");

		cgs.media.redFlagFlapSkin = trap_R_RegisterSkin("models/flag2/red.skin");
		cgs.media.blueFlagFlapSkin = trap_R_RegisterSkin("models/flag2/blue.skin");
		cgs.media.neutralFlagFlapSkin = trap_R_RegisterSkin("models/flag2/white.skin");

		cgs.media.redFlagBaseModel = trap_R_RegisterModel("models/mapobjects/flagbase/red_base.md3");
		cgs.media.blueFlagBaseModel = trap_R_RegisterModel("models/mapobjects/flagbase/blue_base.md3");
		cgs.media.neutralFlagBaseModel = trap_R_RegisterModel("models/mapobjects/flagbase/ntrl_base.md3");
	}

	if (CG_UsesTheWhiteFlag(cgs.gametype) || cg_buildScript.integer) {
		cgs.media.neutralFlagModel = trap_R_RegisterModel("models/flags/n_flag.md3");
		cgs.media.flagShader[0] = trap_R_RegisterShaderNoMip("icons/iconf_neutral1");
		cgs.media.flagShader[1] = trap_R_RegisterShaderNoMip("icons/iconf_red2");
		cgs.media.flagShader[2] = trap_R_RegisterShaderNoMip("icons/iconf_blu2");
		cgs.media.flagShader[3] = trap_R_RegisterShaderNoMip("icons/iconf_neutral3");
	}

	if (cgs.gametype == GT_OBELISK || cg_buildScript.integer) {
		cgs.media.rocketExplosionShader = trap_R_RegisterShader("rocketExplosion");
		cgs.media.overloadBaseModel = trap_R_RegisterModel("models/powerups/overload_base.md3");
		cgs.media.overloadTargetModel = trap_R_RegisterModel("models/powerups/overload_target.md3");
		cgs.media.overloadLightsModel = trap_R_RegisterModel("models/powerups/overload_lights.md3");
		cgs.media.overloadEnergyModel = trap_R_RegisterModel("models/powerups/overload_energy.md3");
	}

	if (cgs.gametype == GT_HARVESTER || cg_buildScript.integer) {
		cgs.media.harvesterModel = trap_R_RegisterModel("models/powerups/harvester/harvester.md3");
		cgs.media.harvesterRedSkin = trap_R_RegisterSkin("models/powerups/harvester/red.skin");
		cgs.media.harvesterBlueSkin = trap_R_RegisterSkin("models/powerups/harvester/blue.skin");
		cgs.media.harvesterNeutralModel = trap_R_RegisterModel("models/powerups/obelisk/obelisk.md3");
	}

	cgs.media.redKamikazeShader = trap_R_RegisterShader("models/weaphits/kamikred");
	cgs.media.dustPuffShader = trap_R_RegisterShader("hasteSmokePuff");

	if (CG_IsATeamGametype(cgs.gametype) || cg_buildScript.integer) {

		cgs.media.friendShader = trap_R_RegisterShader("sprites/foe");
		cgs.media.redQuadShader = trap_R_RegisterShader("powerups/blueflag");
		//cgs.media.teamStatusBar = trap_R_RegisterShader( "gfx/2d/colorbar.tga" ); - moved outside, used by accuracy
		cgs.media.blueKamikazeShader = trap_R_RegisterShader("models/weaphits/kamikblu");
	}
	cgs.media.teamStatusBar = trap_R_RegisterShader("gfx/2d/colorbar.tga");

	cgs.media.armorModel = trap_R_RegisterModel("models/powerups/armor/armor_yel.md3");
	cgs.media.armorIcon = trap_R_RegisterShaderNoMip("icons/iconr_yellow");

	cgs.media.machinegunBrassModel = trap_R_RegisterModel("models/weapons2/shells/m_shell.md3");
	cgs.media.shotgunBrassModel = trap_R_RegisterModel("models/weapons2/shells/s_shell.md3");

	cgs.media.gibAbdomen = trap_R_RegisterModel("models/gibs/abdomen.md3");
	cgs.media.gibArm = trap_R_RegisterModel("models/gibs/arm.md3");
	cgs.media.gibChest = trap_R_RegisterModel("models/gibs/chest.md3");
	cgs.media.gibFist = trap_R_RegisterModel("models/gibs/fist.md3");
	cgs.media.gibFoot = trap_R_RegisterModel("models/gibs/foot.md3");
	cgs.media.gibForearm = trap_R_RegisterModel("models/gibs/forearm.md3");
	cgs.media.gibIntestine = trap_R_RegisterModel("models/gibs/intestine.md3");
	cgs.media.gibLeg = trap_R_RegisterModel("models/gibs/leg.md3");
	cgs.media.gibSkull = trap_R_RegisterModel("models/gibs/skull.md3");
	cgs.media.gibBrain = trap_R_RegisterModel("models/gibs/brain.md3");

	cgs.media.smoke2 = trap_R_RegisterModel("models/weapons2/shells/s_shell.md3");

	cgs.media.balloonShader = trap_R_RegisterShader("sprites/balloon3");

	cgs.media.bloodExplosionShader = trap_R_RegisterShader("bloodExplosion");

	cgs.media.bulletFlashModel = trap_R_RegisterModel("models/weaphits/bullet.md3");
	cgs.media.ringFlashModel = trap_R_RegisterModel("models/weaphits/ring02.md3");
	cgs.media.dishFlashModel = trap_R_RegisterModel("models/weaphits/boom01.md3");
#ifdef MISSIONPACK
	cgs.media.teleportEffectModel = trap_R_RegisterModel("models/powerups/pop.md3");
#else
	cgs.media.teleportEffectModel = trap_R_RegisterModel("models/misc/telep.md3");
	cgs.media.teleportEffectShader = trap_R_RegisterShader("teleportEffect");
#endif
	// loadingscreen
#ifdef SCRIPTHUD
	CG_UpdateGraphicFraction(0.20f);
	CG_UpdateMediaFraction(0.66f);
#endif
	// end loadingscreen
	cgs.media.kamikazeEffectModel = trap_R_RegisterModel("models/weaphits/kamboom2.md3");
	cgs.media.kamikazeShockWave = trap_R_RegisterModel("models/weaphits/kamwave.md3");
	cgs.media.kamikazeHeadModel = trap_R_RegisterModel("models/powerups/kamikazi.md3");
	cgs.media.kamikazeHeadTrail = trap_R_RegisterModel("models/powerups/trailtest.md3");
	cgs.media.guardPowerupModel = trap_R_RegisterModel("models/powerups/guard_player.md3");
	cgs.media.scoutPowerupModel = trap_R_RegisterModel("models/powerups/scout_player.md3");
	cgs.media.doublerPowerupModel = trap_R_RegisterModel("models/powerups/doubler_player.md3");
	cgs.media.ammoRegenPowerupModel = trap_R_RegisterModel("models/powerups/ammo_player.md3");
	cgs.media.invulnerabilityImpactModel = trap_R_RegisterModel("models/powerups/shield/impact.md3");
	cgs.media.invulnerabilityJuicedModel = trap_R_RegisterModel("models/powerups/shield/juicer.md3");
	cgs.media.medkitUsageModel = trap_R_RegisterModel("models/powerups/regen.md3");
	cgs.media.heartShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/selectedhealth.tga");

	// loadingscreen
#ifdef SCRIPTHUD
	CG_UpdateGraphicFraction(0.70f);
	CG_UpdateMediaFraction(0.85f);
#endif
	// end loadingscreen

	cgs.media.invulnerabilityPowerupModel = trap_R_RegisterModel("models/powerups/shield/shield.md3");
	cgs.media.medalImpressive = trap_R_RegisterShaderNoMip("medal_impressive");
	cgs.media.medalExcellent = trap_R_RegisterShaderNoMip("medal_excellent");
	cgs.media.medalGauntlet = trap_R_RegisterShaderNoMip("medal_gauntlet");
	cgs.media.medalDefend = trap_R_RegisterShaderNoMip("medal_defend");
	cgs.media.medalAssist = trap_R_RegisterShaderNoMip("medal_assist");
	cgs.media.medalCapture = trap_R_RegisterShaderNoMip("medal_capture");

	// LEILEI SHADERS
	cgs.media.lsmkShader1 = trap_R_RegisterShader("leismoke1");
	cgs.media.lsmkShader2 = trap_R_RegisterShader("leismoke2");
	cgs.media.lsmkShader3 = trap_R_RegisterShader("leismoke3");
	cgs.media.lsmkShader4 = trap_R_RegisterShader("leismoke4");

	cgs.media.lsplShader = trap_R_RegisterShader("leisplash");
	cgs.media.lspkShader1 = trap_R_RegisterShader("leispark");
	cgs.media.lspkShader2 = trap_R_RegisterShader("leispark2");
	cgs.media.lbumShader1 = trap_R_RegisterShader("leiboom1");
	cgs.media.lfblShader1 = trap_R_RegisterShader("leifball");

	cgs.media.lbldShader1 = trap_R_RegisterShader("leiblood1");
	cgs.media.lbldShader2 = trap_R_RegisterShader("leiblood2"); // this is a mark, by the way

	// New Bullet Marks
	cgs.media.lmarkmetal1 = trap_R_RegisterShader("leimetalmark1");
	cgs.media.lmarkmetal2 = trap_R_RegisterShader("leimetalmark2");
	cgs.media.lmarkmetal3 = trap_R_RegisterShader("leimetalmark3");
	cgs.media.lmarkmetal4 = trap_R_RegisterShader("leimetalmark4");
	cgs.media.lmarkbullet1 = trap_R_RegisterShader("leibulletmark1");
	cgs.media.lmarkbullet2 = trap_R_RegisterShader("leibulletmark2");
	cgs.media.lmarkbullet3 = trap_R_RegisterShader("leibulletmark3");
	cgs.media.lmarkbullet4 = trap_R_RegisterShader("leibulletmark4");


	memset(cg_items, 0, sizeof ( cg_items));
	memset(cg_weapons, 0, sizeof ( cg_weapons));

	// only register the items that the server says we need
	Q_strncpyz(items, CG_ConfigString(CS_ITEMS), sizeof (items));

	for (i = 1; i < bg_numItems; i++) {
		if (items[ i ] == '1' || cg_buildScript.integer) {
			CG_LoadingItem(i);
			CG_RegisterItemVisuals(i);
		}
	}

	// wall marks
	cgs.media.bulletMarkShader = trap_R_RegisterShader("gfx/damage/bullet_mrk");
	cgs.media.burnMarkShader = trap_R_RegisterShader("gfx/damage/burn_med_mrk");
	cgs.media.holeMarkShader = trap_R_RegisterShader("gfx/damage/hole_lg_mrk");
	cgs.media.energyMarkShader = trap_R_RegisterShader("gfx/damage/plasma_mrk");
	cgs.media.shadowMarkShader = trap_R_RegisterShader("markShadow");
	cgs.media.wakeMarkShader = trap_R_RegisterShader("wake");
	cgs.media.bloodMarkShader = trap_R_RegisterShader("bloodMark");

	// register the inline models
	cgs.numInlineModels = trap_CM_NumInlineModels();
	for (i = 1; i < cgs.numInlineModels; i++) {
		char name[10];
		vec3_t mins, maxs;
		int j;

		Com_sprintf(name, sizeof (name), "*%i", i);
		cgs.inlineDrawModel[i] = trap_R_RegisterModel(name);
		trap_R_ModelBounds(cgs.inlineDrawModel[i], mins, maxs);
		for (j = 0; j < 3; j++) {
			cgs.inlineModelMidpoints[i][j] = mins[j] + 0.5 * (maxs[j] - mins[j]);
		}
	}

	// register all the server specified models
	for (i = 1; i < MAX_MODELS; i++) {
		const char *modelName;

		modelName = CG_ConfigString(CS_MODELS + i);
		if (!modelName[0]) {
			break;
		}
		cgs.gameModels[i] = trap_R_RegisterModel(modelName);
	}

#ifdef MISSIONPACK
	// new stuff
	cgs.media.patrolShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/patrol.tga");
	cgs.media.assaultShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/assault.tga");
	cgs.media.campShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/camp.tga");
	cgs.media.followShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/follow.tga");
	cgs.media.defendShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/defend.tga");
	cgs.media.teamLeaderShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/team_leader.tga");
	cgs.media.retrieveShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/retrieve.tga");
	cgs.media.escortShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/escort.tga");
	cgs.media.deathShader = trap_R_RegisterShaderNoMip("ui/assets/statusbar/death.tga");

	cgs.media.cursor = trap_R_RegisterShaderNoMip("menu/art/3_cursor2");
	cgs.media.sizeCursor = trap_R_RegisterShaderNoMip("ui/assets/sizecursor.tga");
	cgs.media.selectCursor = trap_R_RegisterShaderNoMip("ui/assets/selectcursor.tga");
	cgs.media.flagShaders[0] = trap_R_RegisterShaderNoMip("ui/assets/statusbar/flag_in_base.tga");
	cgs.media.flagShaders[1] = trap_R_RegisterShaderNoMip("ui/assets/statusbar/flag_capture.tga");
	cgs.media.flagShaders[2] = trap_R_RegisterShaderNoMip("ui/assets/statusbar/flag_missing.tga");

	trap_R_RegisterModel("models/players/sorceress/lower.mdr");
	trap_R_RegisterModel("models/players/sorceress/upper.mdr");
	trap_R_RegisterModel("models/players/sorceress/head.md3");

#endif
	//	CG_ClearParticles ();
	/*
		for (i=1; i<MAX_PARTICLES_AREAS; i++)
		{
			{
				int rval;

				rval = CG_NewParticleArea ( CS_PARTICLES + i);
				if (!rval)
					break;
			}
		}
	 */
}

/*																																			
=======================
CG_BuildSpectatorString

=======================
 */
void CG_BuildSpectatorString(void) {
	int i;
	cg.spectatorList[0] = 0;
	for (i = 0; i < MAX_CLIENTS; i++) {
		if (cgs.clientinfo[i].infoValid && cgs.clientinfo[i].team == TEAM_SPECTATOR) {
			Q_strcat(cg.spectatorList, sizeof (cg.spectatorList), va("%s     ", cgs.clientinfo[i].name));
		}
	}
	i = strlen(cg.spectatorList);
	if (i != cg.spectatorLen) {
		cg.spectatorLen = i;
		cg.spectatorWidth = -1;
	}
}

/*																																			
===================
CG_RegisterClients
===================
 */
static void CG_RegisterClients(void) {
	int i;

	CG_LoadingClient(cg.clientNum);
	CG_NewClientInfo(cg.clientNum);

	for (i = 0; i < MAX_CLIENTS; i++) {
		const char *clientInfo;

		if (cg.clientNum == i) {
			continue;
		}

		clientInfo = CG_ConfigString(CS_PLAYERS + i);
		if (!clientInfo[0]) {
			continue;
		}
		CG_LoadingClient(i);
		CG_NewClientInfo(i);
	}
	CG_BuildSpectatorString();
}

//===========================================================================

/*
=================
CG_ConfigString
=================
 */
const char *CG_ConfigString(int index) {
	if (index < 0 || index >= MAX_CONFIGSTRINGS) {
		CG_Error("CG_ConfigString: bad index: %i", index);
	}
	return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

//==================================================================

/*
======================
CG_StartMusic

======================
 */
void CG_StartMusic(void) {
	char *s;
	char parm1[MAX_QPATH], parm2[MAX_QPATH];

	// start the background music
	if (*cg_music.string && !Q_strequal(cg_music.string, "none")) {
		s = (char *) cg_music.string;
	} else {
		s = (char *) CG_ConfigString(CS_MUSIC);
		Q_strncpyz(parm1, COM_Parse(&s), sizeof ( parm1));
		Q_strncpyz(parm2, COM_Parse(&s), sizeof ( parm2));

		trap_S_StartBackgroundTrack(parm1, parm2);
	}
}
#ifdef MISSIONPACK

char *CG_GetMenuBuffer(const char *filename) {
	int len;
	fileHandle_t f;
	static char buf[MAX_MENUFILE];

	len = trap_FS_FOpenFile(filename, &f, FS_READ);
	if (!f) {
		trap_Print(va(S_COLOR_RED "menu file not found: %s, using default\n", filename));
		return NULL;
	}
	if (len >= MAX_MENUFILE) {
		trap_Print(va(S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", filename, len, MAX_MENUFILE));
		trap_FS_FCloseFile(f);
		return NULL;
	}

	trap_FS_Read(buf, len, f);
	buf[len] = 0;
	trap_FS_FCloseFile(f);

	return buf;
}

//
// ==============================
// new hud stuff ( mission pack )
// ==============================
//

qboolean CG_Asset_Parse(int handle) {
	pc_token_t token;
	const char *tempStr;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (Q_stricmp(token.string, "{") != 0) {
		return qfalse;
	}

	while (1) {
		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if (Q_stricmp(token.string, "}") == 0) {
			return qtrue;
		}

		// font
		if (Q_stricmp(token.string, "font") == 0) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
			cgDC.registerFont(tempStr, pointSize, &cgDC.Assets.textFont);
			continue;
		}

		// smallFont
		if (Q_stricmp(token.string, "smallFont") == 0) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
			cgDC.registerFont(tempStr, pointSize, &cgDC.Assets.smallFont);
			continue;
		}

		// font
		if (Q_stricmp(token.string, "bigfont") == 0) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
			cgDC.registerFont(tempStr, pointSize, &cgDC.Assets.bigFont);
			continue;
		}

		// gradientbar
		if (Q_stricmp(token.string, "gradientbar") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.gradientBar = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		// enterMenuSound
		if (Q_stricmp(token.string, "menuEnterSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.menuEnterSound = trap_S_RegisterSound(tempStr, qfalse);
			continue;
		}

		// exitMenuSound
		if (Q_stricmp(token.string, "menuExitSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.menuExitSound = trap_S_RegisterSound(tempStr, qfalse);
			continue;
		}

		// itemFocusSound
		if (Q_stricmp(token.string, "itemFocusSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.itemFocusSound = trap_S_RegisterSound(tempStr, qfalse);
			continue;
		}

		// menuBuzzSound
		if (Q_stricmp(token.string, "menuBuzzSound") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.menuBuzzSound = trap_S_RegisterSound(tempStr, qfalse);
			continue;
		}

		if (Q_stricmp(token.string, "cursor") == 0) {
			if (!PC_String_Parse(handle, &cgDC.Assets.cursorStr)) {
				return qfalse;
			}
			cgDC.Assets.cursor = trap_R_RegisterShaderNoMip(cgDC.Assets.cursorStr);
			continue;
		}

		if (Q_stricmp(token.string, "fadeClamp") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.fadeClamp)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeCycle") == 0) {
			if (!PC_Int_Parse(handle, &cgDC.Assets.fadeCycle)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeAmount") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.fadeAmount)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowX") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.shadowX)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowY") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.shadowY)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowColor") == 0) {
			if (!PC_Color_Parse(handle, &cgDC.Assets.shadowColor)) {
				return qfalse;
			}
			cgDC.Assets.shadowFadeClamp = cgDC.Assets.shadowColor[3];
			continue;
		}
		// Changed RD SCRIPTHUD
		if (Q_stricmp(token.string, "scrollbarSize") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.scrollbarsize)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderWidth") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.sliderwidth)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderHeight") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.sliderheight)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderthumbWidth") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.sliderthumbwidth)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderthumbHeight") == 0) {
			if (!PC_Float_Parse(handle, &cgDC.Assets.sliderthumbheight)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderBar") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.sliderBar = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "sliderThumb") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "sliderThumbSel") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.sliderThumb_sel = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarHorz") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarHorz = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarVert") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarVert = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarThumb") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowUp") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowDown") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowLeft") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowRight") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxBase") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxRed") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxYellow") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxGreen") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxTeal") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxBlue") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxCyan") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxWhite") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			cgDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}
		// end changed RD SCRIPTHUD
	}
	return qfalse; // bk001204 - why not?
}

void CG_ParseMenu(const char *menuFile) {
	pc_token_t token;
	int handle;

	handle = trap_PC_LoadSource(menuFile);
	if (!handle)
		handle = trap_PC_LoadSource("ui/testhud.menu");
	if (!handle)
		return;

	while (1) {
		if (!trap_PC_ReadToken(handle, &token)) {
			break;
		}

		//if ( !Q_strequal( token, "{" ) ) {
		//	Com_Printf( "Missing { in menu file\n" );
		//	break;
		//}

		//if ( menuCount == MAX_MENUS ) {
		//	Com_Printf( "Too many menus!\n" );
		//	break;
		//}

		if (token.string[0] == '}') {
			break;
		}

		if (Q_strequal(token.string, "assetGlobalDef")) {
			if (CG_Asset_Parse(handle)) {
				continue;
			} else {
				break;
			}
		}


		if (Q_strequal(token.string, "menudef")) {
			// start a new menu
			Menu_New(handle);
		}
	}
	trap_PC_FreeSource(handle);
}

qboolean CG_Load_Menu(char **p) {
	char *token;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	while (1) {

		token = COM_ParseExt(p, qtrue);

		if (Q_strequal(token, "}")) {
			return qtrue;
		}

		if (!token[0]) {
			return qfalse;
		}

		CG_ParseMenu(token);
	}
	return qfalse;
}

void CG_LoadMenus(const char *menuFile) {
	char *token;
	char *p;
	int len, start;
	fileHandle_t f;
	static char buf[MAX_MENUDEFFILE];

	start = trap_Milliseconds();

	len = trap_FS_FOpenFile(menuFile, &f, FS_READ);
	if (!f) {
		trap_Error(va(S_COLOR_YELLOW "menu file not found: %s, using default\n", menuFile));
		len = trap_FS_FOpenFile("ui/hud.txt", &f, FS_READ);
		if (!f) {
			trap_Error(va(S_COLOR_RED "default menu file not found: ui/hud.txt, unable to continue!\n"));
		}
	}

	if (len >= MAX_MENUDEFFILE) {
		trap_Error(va(S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", menuFile, len, MAX_MENUDEFFILE));
		trap_FS_FCloseFile(f);
		return;
	}

	trap_FS_Read(buf, len, f);
	buf[len] = 0;
	trap_FS_FCloseFile(f);

	COM_Compress(buf);

	Menu_Reset();

	p = buf;

	while (1) {
		token = COM_ParseExt(&p, qtrue);
		if (!token[0]) {
			break;
		}

		if (Q_strequal(token, "}")) {
			break;
		}

		if (Q_strequal(token, "loadmenu")) {
			if (CG_Load_Menu(&p)) {
				continue;
			} else {
				break;
			}
		}
	}

	Com_Printf("UI menu load time = %d milli seconds\n", trap_Milliseconds() - start);

}

static qboolean CG_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key) {
	return qfalse;
}

static int CG_FeederCount(float feederID) {
	int i, count;
	count = 0;
	if (feederID == FEEDER_REDTEAM_LIST) {
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == TEAM_RED) {
				count++;
			}
		}
	} else if (feederID == FEEDER_BLUETEAM_LIST) {
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == TEAM_BLUE) {
				count++;
			}
		}
	} else if (feederID == FEEDER_SCOREBOARD) {
		return cg.numScores;
	}
	return count;
}

void CG_SetScoreSelection(void *p) {
	menuDef_t *menu = (menuDef_t*) p;
	playerState_t *ps = &cg.snap->ps;
	int i, red, blue;
	red = blue = 0;
	for (i = 0; i < cg.numScores; i++) {
		if (cg.scores[i].team == TEAM_RED) {
			red++;
		} else if (cg.scores[i].team == TEAM_BLUE) {
			blue++;
		}
		if (ps->clientNum == cg.scores[i].client) {
			cg.selectedScore = i;
		}
	}

	if (menu == NULL) {
		// just interested in setting the selected score
		return;
	}

	if (CG_IsATeamGametype(cgs.gametype)) {
		int feeder = FEEDER_REDTEAM_LIST;
		i = red;
		if (cg.scores[cg.selectedScore].team == TEAM_BLUE) {
			feeder = FEEDER_BLUETEAM_LIST;
			i = blue;
		}
		Menu_SetFeederSelection(menu, feeder, i, NULL);
	} else {
		Menu_SetFeederSelection(menu, FEEDER_SCOREBOARD, cg.selectedScore, NULL);
	}
}

// FIXME: might need to cache this info

static clientInfo_t * CG_InfoFromScoreIndex(int index, int team, int *scoreIndex) {
	int i, count;
	if (CG_IsATeamGametype(cgs.gametype)) {
		count = 0;
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == team) {
				if (count == index) {
					*scoreIndex = i;
					return &cgs.clientinfo[cg.scores[i].client];
				}
				count++;
			}
		}
	}
	*scoreIndex = index;
	return &cgs.clientinfo[ cg.scores[index].client ];
}

static const char *CG_FeederItemText(float feederID, int index, int column, qhandle_t *handle) {
	gitem_t *item;
	int scoreIndex = 0;
	clientInfo_t *info = NULL;
	int team = -1;
	score_t *sp = NULL;

	*handle = -1;

	if (feederID == FEEDER_REDTEAM_LIST) {
		team = TEAM_RED;
	} else if (feederID == FEEDER_BLUETEAM_LIST) {
		team = TEAM_BLUE;
	}

	info = CG_InfoFromScoreIndex(index, team, &scoreIndex);
	sp = &cg.scores[scoreIndex];

	if (info && info->infoValid) {
		switch (column) {
			case 0:
				if (info->powerups & (1 << PW_NEUTRALFLAG)) {
					item = BG_FindItemForPowerup(PW_NEUTRALFLAG);
					*handle = cg_items[ ITEM_INDEX(item) ].icon;
				} else if (info->powerups & (1 << PW_REDFLAG)) {
					item = BG_FindItemForPowerup(PW_REDFLAG);
					*handle = cg_items[ ITEM_INDEX(item) ].icon;
				} else if (info->powerups & (1 << PW_BLUEFLAG)) {
					item = BG_FindItemForPowerup(PW_BLUEFLAG);
					*handle = cg_items[ ITEM_INDEX(item) ].icon;
				} else {
					if (info->botSkill > 0 && info->botSkill <= 5) {
						*handle = cgs.media.botSkillShaders[ info->botSkill - 1 ];
					} else if (info->handicap < 100) {
						return va("%i", info->handicap);
					}
				}
				break;
			case 1:
				if (team == -1) {
					return "";
				} else if (info->isDead) {
					*handle = cgs.media.deathShader;
				} else {
					*handle = CG_StatusHandle(info->teamTask);
				}
				break;
			case 2:
				if (cg.snap->ps.stats[ STAT_CLIENTS_READY ] & (1 << sp->client)) {
					return "Ready";
				}
				if (team == -1) {
					if (cgs.gametype == GT_TOURNAMENT) {
						return va("%i/%i", info->wins, info->losses);
					} else if (info->infoValid && info->team == TEAM_SPECTATOR) {
						return "Spectator";
					} else {
						return "";
					}
				} else {
					if (info->teamLeader) {
						return "Leader";
					}
				}
				break;
			case 3:
				return info->name;
				break;
			case 4:
				return va("%i", info->score);
				break;
			case 5:
				return va("%4i", sp->time);
				break;
			case 6:
				if (sp->ping == -1) {
					return "connecting";
				}
				return va("%4i", sp->ping);
				break;
		}
	}

	return "";
}

static qhandle_t CG_FeederItemImage(float feederID, int index) {
	return 0;
}

static void CG_FeederSelection(float feederID, int index) {
	if (CG_IsATeamGametype(cgs.gametype)) {
		int i, count;
		int team = (feederID == FEEDER_REDTEAM_LIST) ? TEAM_RED : TEAM_BLUE;
		count = 0;
		for (i = 0; i < cg.numScores; i++) {
			if (cg.scores[i].team == team) {
				if (index == count) {
					cg.selectedScore = i;
				}
				count++;
			}
		}
	} else {
		cg.selectedScore = index;
	}
}
#endif

float CG_GetCVar(const char *cvar) {
	char value[128];
	memset(value, 0, sizeof (value));
	trap_Cvar_VariableStringBuffer(cvar, value, sizeof(value));
	return atof(value);
}
#ifdef MISSIONPACK

void CG_Text_PaintWithCursor(float x, float y, float scale, vec4_t color, const char *text, int cursorPos, char cursor, int limit, int style) {
	CG_Text_Paint(x, y, scale, color, text, 0, limit, style);
}

static int CG_OwnerDrawWidth(int ownerDraw, float scale) {
	switch (ownerDraw) {
		case CG_GAME_TYPE:
			return CG_Text_Width(CG_GameTypeString(), scale, 0);
		case CG_GAME_STATUS:
			return CG_Text_Width(CG_GetGameStatusText(), scale, 0);
			break;
		case CG_KILLER:
			return CG_Text_Width(CG_GetKillerText(), scale, 0);
			break;
		case CG_RED_NAME:
			return CG_Text_Width(cg_redTeamName.string, scale, 0);
			break;
		case CG_BLUE_NAME:
			return CG_Text_Width(cg_blueTeamName.string, scale, 0);
			break;


	}
	return 0;
}

static int CG_PlayCinematic(const char *name, float x, float y, float w, float h) {
	return trap_CIN_PlayCinematic(name, x, y, w, h, CIN_loop);
}

static void CG_StopCinematic(int handle) {
	trap_CIN_StopCinematic(handle);
}

static void CG_DrawCinematic(int handle, float x, float y, float w, float h) {
	trap_CIN_SetExtents(handle, x, y, w, h);
	trap_CIN_DrawCinematic(handle);
}

static void CG_RunCinematicFrame(int handle) {
	trap_CIN_RunCinematic(handle);
}

/*
=================
CG_LoadHudMenu();

=================
 */
void CG_LoadHudMenu(void) {
	char buff[1024];
	const char *hudSet;

	cgDC.registerShaderNoMip = &trap_R_RegisterShaderNoMip;
	cgDC.setColor = &trap_R_SetColor;
	cgDC.drawHandlePic = &CG_DrawPic;
	cgDC.drawStretchPic = &trap_R_DrawStretchPic;
	cgDC.drawText = &CG_Text_Paint;
	cgDC.textWidth = &CG_Text_Width;
	cgDC.textHeight = &CG_Text_Height;
	cgDC.registerModel = &trap_R_RegisterModel;
	cgDC.modelBounds = &trap_R_ModelBounds;
	cgDC.fillRect = &CG_FillRect;
	cgDC.drawRect = &CG_DrawRect;
	cgDC.drawSides = &CG_DrawSides;
	cgDC.drawTopBottom = &CG_DrawTopBottom;
	cgDC.clearScene = &trap_R_ClearScene;
	cgDC.addRefEntityToScene = &trap_R_AddRefEntityToScene;
	cgDC.renderScene = &trap_R_RenderScene;
	cgDC.registerFont = &trap_R_RegisterFont;
	cgDC.ownerDrawItem = &CG_OwnerDraw;
	cgDC.getValue = &CG_GetValue;
	cgDC.ownerDrawVisible = &CG_OwnerDrawVisible;
	cgDC.runScript = &CG_RunMenuScript;
	cgDC.getTeamColor = &CG_GetTeamColor;
	cgDC.setCVar = trap_Cvar_Set;
	cgDC.getCVarString = trap_Cvar_VariableStringBuffer;
	cgDC.getCVarValue = CG_GetCVar;
	cgDC.drawTextWithCursor = &CG_Text_PaintWithCursor;
	//cgDC.setOverstrikeMode = &trap_Key_SetOverstrikeMode;
	//cgDC.getOverstrikeMode = &trap_Key_GetOverstrikeMode;
	cgDC.startLocalSound = &trap_S_StartLocalSound;
	cgDC.ownerDrawHandleKey = &CG_OwnerDrawHandleKey;
	cgDC.feederCount = &CG_FeederCount;
	cgDC.feederItemImage = &CG_FeederItemImage;
	cgDC.feederItemText = &CG_FeederItemText;
	cgDC.feederSelection = &CG_FeederSelection;
	//cgDC.setBinding = &trap_Key_SetBinding;
	//cgDC.getBindingBuf = &trap_Key_GetBindingBuf;
	//cgDC.keynumToStringBuf = &trap_Key_KeynumToStringBuf;
	//cgDC.executeText = &trap_Cmd_ExecuteText;
	cgDC.Error = &Com_Error;
	cgDC.Print = &Com_Printf;
	cgDC.ownerDrawWidth = &CG_OwnerDrawWidth;
	//cgDC.Pause = &CG_Pause;
	cgDC.registerSound = &trap_S_RegisterSound;
	cgDC.startBackgroundTrack = &trap_S_StartBackgroundTrack;
	cgDC.stopBackgroundTrack = &trap_S_StopBackgroundTrack;
	cgDC.playCinematic = &CG_PlayCinematic;
	cgDC.stopCinematic = &CG_StopCinematic;
	cgDC.drawCinematic = &CG_DrawCinematic;
	cgDC.runCinematicFrame = &CG_RunCinematicFrame;

	Init_Display(&cgDC);

	Menu_Reset();

	trap_Cvar_VariableStringBuffer("cg_hudFiles", buff, sizeof (buff));
	hudSet = buff;
	if (hudSet[0] == '\0') {
		hudSet = "ui/hud.txt";
	}

	CG_LoadMenus(hudSet);
}

void CG_AssetCache(void) {
	//if (Assets.textFont == NULL) {
	//  trap_R_RegisterFont("fonts/arial.ttf", 72, &Assets.textFont);
	//}
	//Assets.background = trap_R_RegisterShaderNoMip( ASSET_BACKGROUND );
	//Com_Printf("Menu Size: %i bytes\n", sizeof(Menus));
	/*
		cgDC.Assets.gradientBar = trap_R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
		cgDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
		cgDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
		cgDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
		cgDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
		cgDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
		cgDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
		cgDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
		cgDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
		cgDC.Assets.scrollBar = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
		cgDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
		cgDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
		cgDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
		cgDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
		cgDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
		cgDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
		cgDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );
	 */

	// Changed RD SCRIPTHUD
	cgDC.Assets.gradientBar = trap_R_RegisterShaderNoMip(ASSET_GRADIENTBAR);
	cgDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip(ART_FX_BASE);
	cgDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip(ART_FX_RED);
	cgDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip(ART_FX_YELLOW);
	cgDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip(ART_FX_GREEN);
	cgDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip(ART_FX_TEAL);
	cgDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip(ART_FX_BLUE);
	cgDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip(ART_FX_CYAN);
	cgDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip(ART_FX_WHITE);
	cgDC.Assets.scrollBarHorz = trap_R_RegisterShaderNoMip(ASSET_SCROLLBAR);
	cgDC.Assets.scrollBarVert = trap_R_RegisterShaderNoMip(ASSET_SCROLLBAR);
	cgDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip(ASSET_SCROLLBAR_ARROWDOWN);
	cgDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip(ASSET_SCROLLBAR_ARROWUP);
	cgDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip(ASSET_SCROLLBAR_ARROWLEFT);
	cgDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip(ASSET_SCROLLBAR_ARROWRIGHT);
	cgDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip(ASSET_SCROLL_THUMB);
	cgDC.Assets.sliderBar = trap_R_RegisterShaderNoMip(ASSET_SLIDER_BAR);
	cgDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip(ASSET_SLIDER_THUMB);
	cgDC.Assets.sliderThumb_sel = trap_R_RegisterShaderNoMip(ASSET_SLIDER_THUMB_SEL);
	cgDC.Assets.scrollbarsize = SCROLLBAR_SIZE;
	cgDC.Assets.sliderwidth = SLIDER_WIDTH;
	cgDC.Assets.sliderheight = SLIDER_HEIGHT;
	cgDC.Assets.sliderthumbwidth = SLIDER_THUMB_WIDTH;
	cgDC.Assets.sliderthumbheight = SLIDER_THUMB_HEIGHT;
	// end changed RD SCRIPTHUD

}
#endif

int wideAdjustX; // leilei - dirty widescreen hack

/*
=================
CG_Init

Called after every level change or subsystem restart
Will perform callbacks to make the loading info screen update.
=================
 */
void CG_Init(int serverMessageNum, int serverCommandSequence, int clientNum) {
	const char *s;

	// clear everything
	memset(&cgs, 0, sizeof ( cgs));
	memset(&cg, 0, sizeof ( cg));
	memset(cg_entities, 0, sizeof (cg_entities));
	memset(cg_weapons, 0, sizeof (cg_weapons));
	memset(cg_items, 0, sizeof (cg_items));

	cg.clientNum = clientNum;

	cgs.processedSnapshotNum = serverMessageNum;
	cgs.serverCommandSequence = serverCommandSequence;

	// load a few needed things before we do any screen updates
	cgs.media.charsetShader = trap_R_RegisterShader("gfx/2d/bigchars");
	cgs.media.whiteShader = trap_R_RegisterShader("white");
	cgs.media.charsetProp = trap_R_RegisterShaderNoMip("menu/art/font1_prop.tga");
	cgs.media.charsetPropGlow = trap_R_RegisterShaderNoMip("menu/art/font1_prop_glo.tga");
	cgs.media.charsetPropB = trap_R_RegisterShaderNoMip("menu/art/font2_prop.tga");

	CG_RegisterCvars();

	CG_InitConsoleCommands();
	// loadingscreen
#ifdef SCRIPTHUD
	String_Init();
	CG_AssetCache();
	CG_LoadHudMenu(); // load new hud stuff
	trap_Cvar_Set("ui_loading", "1");
#endif
	cg.weaponSelect = WP_MACHINEGUN;

	cgs.redflag = cgs.blueflag = -1; // For compatibily, default to unset for
	cgs.flagStatus = -1;
	// old servers

	// get the rendering configuration from the client system
	trap_GetGlconfig(&cgs.glconfig);
	cgs.screenXScale = cgs.glconfig.vidWidth / 640.0;
	cgs.screenYScale = cgs.glconfig.vidHeight / 480.0;

	realVidWidth = cgs.glconfig.vidWidth;
	realVidHeight = cgs.glconfig.vidHeight;

	// leilei - widescreen correction

	{
		float resbias;
		float rex, rey;
		int newresx, newresy;

		rex = 640.0f / realVidWidth;
		rey = 480.0f / realVidHeight;

		newresx = 640.0f * (rex);
		newresy = 480.0f * (rey);

		newresx = realVidWidth * rey;
		newresy = realVidHeight * rey;

		resbias = 0.5 * (newresx - (newresy * (640.0 / 480.0)));


		wideAdjustX = resbias;

	}
	if (cgs.glconfig.vidWidth * 480 > cgs.glconfig.vidHeight * 640) {
		// wide screen
		cgs.screenXBias = 0.5 * (cgs.glconfig.vidWidth - (cgs.glconfig.vidHeight * (640.0 / 480.0)));
		cgs.screenXScale = cgs.screenYScale;
	} else {
		// no wide screen
		cgs.screenXBias = 0;
	}



	// get the gamestate from the client system
	trap_GetGameState(&cgs.gameState);

	// check version
	s = CG_ConfigString(CS_GAME_VERSION);
	if (!strequals(s, GAME_VERSION)) {
		CG_Error("Client/Server game mismatch: %s/%s", GAME_VERSION, s);
	}

	s = CG_ConfigString(CS_LEVEL_START_TIME);
	cgs.levelStartTime = atoi(s);

	CG_ParseServerinfo();

	// load the new map
	// load the new map
#ifndef SCRIPTHUD
	CG_LoadingString("collision map");
#endif
	trap_CM_LoadMap(cgs.mapname);

#ifdef MISSIONPACK
	String_Init();
	CG_LoadHudMenu(); // load new hud stuff
#endif

	cg.loading = qtrue; // force players to load instead of defer
#ifdef SCRIPTHUD
	CG_RegisterSounds();
	CG_UpdateSoundFraction(1.0f);
	CG_UpdateMediaFraction(0.60f);
	CG_RegisterGraphics();
	CG_UpdateGraphicFraction(1.0f);
	CG_UpdateMediaFraction(0.90f);
	CG_RegisterClients(); // if low on memory, some clients will be deferred
	CG_UpdateMediaFraction(1.0f);
#else
	CG_LoadingString("sounds");

	CG_RegisterSounds();

	CG_LoadingString("graphics");

	CG_RegisterGraphics();

	CG_LoadingString("clients");

	CG_RegisterClients(); // if low on memory, some clients will be deferred
#endif

#ifdef MISSIONPACK
	CG_AssetCache();
	CG_LoadHudMenu(); // load new hud stuff
#endif

	cg.loading = qfalse; // future players will be deferred

	CG_InitLocalEntities();

	CG_InitMarkPolys();

	// remove the last loading update
	cg.infoScreenText[0] = 0;

	// Make sure we have update values (scores)
	CG_SetConfigValues();

	CG_StartMusic();

	CG_LoadingString("");

#ifdef MISSIONPACK
	CG_InitTeamChat();
#endif

	CG_ShaderStateChanged();

	//Init challenge system
	challenges_init();

	addChallenge(GENERAL_TEST);

	trap_S_ClearLoopingSounds(qtrue);
#ifdef SCRIPTHUD
	trap_Cvar_Set("ui_loading", "0");
	cg.consoleValid = qtrue;
#endif
	// end loadingscreen
}

/*
=================
CG_Shutdown

Called before every level change or subsystem restart
=================
 */
void CG_Shutdown(void) {
	// some mods may need to do cleanup work here,
	// like closing files or archiving session data
	challenges_save();
}


/*
==================
CG_EventHandling
==================
 type 0 - no event handling
	  1 - team menu
	  2 - hud editor

 */
#ifndef MISSIONPACK

void CG_EventHandling(int type) {
}

void CG_KeyEvent(int key, qboolean down) {
}

void CG_MouseEvent(int x, int y) {
}
#endif

//unlagged - attack prediction #3
// moved from g_weapon.c

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating 
into a wall.
======================
 */
void SnapVectorTowards(vec3_t v, vec3_t to) {
	int i;

	for (i = 0; i < 3; i++) {
		if (to[i] <= v[i]) {
			v[i] = (int) v[i];
		} else {
			v[i] = (int) v[i] + 1;
		}
	}
}
//unlagged - attack prediction #3

static qboolean do_vid_restart = qfalse;

void CG_FairCvars() {
	qboolean vid_restart_required = qfalse;
	char rendererinfos[128];

	if (cgs.gametype == GT_SINGLE_PLAYER) {
		trap_Cvar_VariableStringBuffer("r_vertexlight", rendererinfos, sizeof (rendererinfos));
		if (cg_autovertex.integer && atoi(rendererinfos) == 0) {
			trap_Cvar_Set("r_vertexlight", "1");
			vid_restart_required = qtrue;
		}
		return; //Don't do anything in single player
	}

	if (cgs.videoflags & VF_LOCK_CVARS_BASIC) {
		//Lock basic cvars.
		trap_Cvar_VariableStringBuffer("r_subdivisions", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) > 80) {
			trap_Cvar_Set("r_subdivisions", "80");
			vid_restart_required = qtrue;
		}

		trap_Cvar_VariableStringBuffer("cg_shadows", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) != 0 && atoi(rendererinfos) != 1) {
			trap_Cvar_Set("cg_shadows", "1");
		}
	}

	if (cgs.videoflags & VF_LOCK_CVARS_EXTENDED) {
		//Lock extended cvars.
		trap_Cvar_VariableStringBuffer("r_subdivisions", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) > 20) {
			trap_Cvar_Set("r_subdivisions", "20");
			vid_restart_required = qtrue;
		}

		trap_Cvar_VariableStringBuffer("r_picmip", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) > 3) {
			trap_Cvar_Set("r_picmip", "3");
			vid_restart_required = qtrue;
		} else if (atoi(rendererinfos) < 0) {
			trap_Cvar_Set("r_picmip", "0");
			vid_restart_required = qtrue;
		}

		trap_Cvar_VariableStringBuffer("r_intensity", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) > 2) {
			trap_Cvar_Set("r_intensity", "2");
			vid_restart_required = qtrue;
		} else if (atoi(rendererinfos) < 0) {
			trap_Cvar_Set("r_intensity", "0");
			vid_restart_required = qtrue;
		}

		trap_Cvar_VariableStringBuffer("r_mapoverbrightbits", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) > 2) {
			trap_Cvar_Set("r_mapoverbrightbits", "2");
			vid_restart_required = qtrue;
		} else if (atoi(rendererinfos) < 0) {
			trap_Cvar_Set("r_mapoverbrightbits", "0");
			vid_restart_required = qtrue;
		}

		trap_Cvar_VariableStringBuffer("r_overbrightbits", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) > 2) {
			trap_Cvar_Set("r_overbrightbits", "2");
			vid_restart_required = qtrue;
		} else if (atoi(rendererinfos) < 0) {
			trap_Cvar_Set("r_overbrightbits", "0");
			vid_restart_required = qtrue;
		}
	}

	if (cgs.videoflags & VF_LOCK_VERTEX) {
		trap_Cvar_VariableStringBuffer("r_vertexlight", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) != 0) {
			trap_Cvar_Set("r_vertexlight", "0");
			vid_restart_required = qtrue;
		}
	} else if (cg_autovertex.integer) {
		trap_Cvar_VariableStringBuffer("r_vertexlight", rendererinfos, sizeof (rendererinfos));
		if (atoi(rendererinfos) == 0) {
			trap_Cvar_Set("r_vertexlight", "1");
			vid_restart_required = qtrue;
		}
	}

	if (vid_restart_required && do_vid_restart)
		trap_SendConsoleCommand("vid_restart");

	do_vid_restart = qtrue;
}



/* Neon_Knight: Useful check in order to have code consistency. */
/*
===================
CG_IsATeamGametype

Checks if the gametype is a team-based game.
===================
 */
qboolean CG_IsATeamGametype(int gametype) {
	return GAMETYPE_IS_A_TEAM_GAME(gametype);
}
/*
===================
CG_UsesTeamFlags

Checks if the gametype makes use of the red and blue flags.
===================
 */
qboolean CG_UsesTeamFlags(int check) {
	return GAMETYPE_USES_RED_AND_BLUE_FLAG(check);
}
/*
===================
CG_UsesTheWhiteFlag

Checks if the gametype makes use of the neutral flag.
===================
 */
qboolean CG_UsesTheWhiteFlag(int check) {
	return GAMETYPE_USES_WHITE_FLAG(check);
}
/*
===================
CG_IsARoundBasedGametype

Checks if the gametype has a round-based system.
===================
 */
qboolean CG_IsARoundBasedGametype(int check) {
	return GAMETYPE_IS_ROUND_BASED(check);
}
/* /Neon_Knight */

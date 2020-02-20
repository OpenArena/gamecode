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
// cg_servercmds.c -- reliably sequenced text commands sent by the server
// these are processed at snapshot transition time, so there will definately
// be a valid snapshot this frame

#include "cg_local.h"
#include "../../ui/menudef.h"

typedef struct {
	const char *order;
	int taskNum;
} orderTask_t;

#ifdef MISSIONPACK // bk001204

static const orderTask_t validOrders[] = {
	{ VOICECHAT_GETFLAG,						TEAMTASK_OFFENSE },
	{ VOICECHAT_OFFENSE,						TEAMTASK_OFFENSE },
	{ VOICECHAT_DEFEND,							TEAMTASK_DEFENSE },
	{ VOICECHAT_DEFENDFLAG,					TEAMTASK_DEFENSE },
	{ VOICECHAT_PATROL,							TEAMTASK_PATROL },
	{ VOICECHAT_CAMP,								TEAMTASK_CAMP },
	{ VOICECHAT_FOLLOWME,						TEAMTASK_FOLLOW },
	{ VOICECHAT_RETURNFLAG,					TEAMTASK_RETRIEVE },
	{ VOICECHAT_FOLLOWFLAGCARRIER,	TEAMTASK_ESCORT }
};

static const int numValidOrders = sizeof(validOrders) / sizeof(orderTask_t);

static int CG_ValidOrder(const char *p) {
	int i;
	for (i = 0; i < numValidOrders; i++) {
		if (Q_strequal(p, validOrders[i].order) ) {
			return validOrders[i].taskNum;
		}
	}
	return -1;
}
#endif

/*
=================
CG_ParseScores

=================
*/
static void CG_ParseScores( void ) {
	int		i, powerups;

	cg.numScores = atoi( CG_Argv( 1 ) );
	if ( cg.numScores > MAX_CLIENTS ) {
		cg.numScores = MAX_CLIENTS;
	}

	cg.teamScores[0] = atoi( CG_Argv( 2 ) );
	cg.teamScores[1] = atoi( CG_Argv( 3 ) );

	cgs.roundStartTime = atoi( CG_Argv( 4 ) );

	//Update thing in lower-right corner
	if(CG_IsARoundBasedGametype(cgs.gametype) && CG_IsATeamGametype(cgs.gametype))
	{
		cgs.scores1 = cg.teamScores[0];
		cgs.scores2 = cg.teamScores[1];
	}

	memset( cg.scores, 0, sizeof( cg.scores ) );

#define NUM_DATA 15
#define FIRST_DATA 4

	for ( i = 0 ; i < cg.numScores ; i++ ) {
		//
		cg.scores[i].client = atoi( CG_Argv( i * NUM_DATA + FIRST_DATA + 1 ) );
		cg.scores[i].score = atoi( CG_Argv( i * NUM_DATA + FIRST_DATA + 2 ) );
		cg.scores[i].ping = atoi( CG_Argv( i * NUM_DATA + FIRST_DATA + 3 ) );
		cg.scores[i].time = atoi( CG_Argv( i * NUM_DATA + FIRST_DATA + 4 ) );
		cg.scores[i].scoreFlags = atoi( CG_Argv( i * NUM_DATA + FIRST_DATA + 5 ) );
		powerups = atoi( CG_Argv( i * NUM_DATA + FIRST_DATA + 6 ) );
		cg.scores[i].accuracy = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 7));
		cg.scores[i].impressiveCount = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 8));
		cg.scores[i].excellentCount = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 9));
		cg.scores[i].guantletCount = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 10));
		cg.scores[i].defendCount = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 11));
		cg.scores[i].assistCount = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 12));
		cg.scores[i].perfect = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 13));
		cg.scores[i].captures = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 14));
		cg.scores[i].isDead = atoi(CG_Argv(i * NUM_DATA + FIRST_DATA + 15));
		//cgs.roundStartTime = 

		if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
			cg.scores[i].client = 0;
		}
		cgs.clientinfo[ cg.scores[i].client ].score = cg.scores[i].score;
		cgs.clientinfo[ cg.scores[i].client ].powerups = powerups;
		cgs.clientinfo[ cg.scores[i].client ].isDead = cg.scores[i].isDead;

		cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;
	}
#ifdef MISSIONPACK
	CG_SetScoreSelection(NULL);
#endif

}

static void CG_ParseAccuracy( void ) {
	int		i;

	for ( i = WP_MACHINEGUN ; i < WP_NUM_WEAPONS ; i++ ) {
		cg.accuracys[i-WP_MACHINEGUN][0] = atoi( CG_Argv( (i-WP_MACHINEGUN)*2 + 1 ) );
		cg.accuracys[i-WP_MACHINEGUN][1] = atoi( CG_Argv( (i-WP_MACHINEGUN)*2 + 2 ) );
		if (cg_developer.integer & DEVMODE_FOR_CODERS) {
			CG_Printf("W: %i   shots: %i   Hits: %i\n", i,cg.accuracys[i][0], cg.accuracys[i][1]);
		}
	}

}


/*
=================
CG_ParseElimination

=================
*/
static void CG_ParseElimination( void ) {
	if(CG_IsARoundBasedGametype(cgs.gametype) && CG_IsATeamGametype(cgs.gametype))
	{
		cgs.scores1 = atoi( CG_Argv( 1 ) );
		cgs.scores2 = atoi( CG_Argv( 2 ) );
	}
	cgs.roundStartTime = atoi( CG_Argv( 3 ) );
}

/*
=================
CG_ParseMappage
Sago: This parses values from the server rather directly. Some checks are performed, but beware if you change it or new
security holes are found
=================
*/
static void CG_ParseMappage( void ) {
	char command[1024];
	const char *temp;
	const char*	c;
	int i;

	temp = CG_Argv( 1 );
for( c = temp; *c; ++c) {
		switch(*c) {
			case '\n':
			case '\r':
			case ';':
				//The server tried something bad!
				return;
			break;
		}
	}
	Q_strncpyz(command,va("ui_mappage %s",temp),1024);
	for(i=2;i<12;i++) {
		temp = CG_Argv( i );
		for( c = temp; *c; ++c) {
				switch(*c) {
					case '\n':
					case '\r':
					case ';':
						//The server tried something bad!
						return;
					break;
				}
			}
		if(strlen(temp)<1) {
			temp = "---";
		}
		Q_strcat(command,1024,va(" %s ",temp));
	}
	trap_SendConsoleCommand(command);

}

/*
=================
CG_ParseDDtimetaken

=================
*/
static void CG_ParseDDtimetakeAt( void ) {
	cgs.takeAt = atoi( CG_Argv( 1 ) );
}

/*
=================
CG_ParseDomPointNames
=================
*/

static void CG_ParseDomPointNames( void ) {
	int i,j;
	cgs.domination_points_count = atoi( CG_Argv( 1 ) );
	if(cgs.domination_points_count>=MAX_DOMINATION_POINTS) {
		cgs.domination_points_count = MAX_DOMINATION_POINTS;
	}
	for(i = 0;i<cgs.domination_points_count;i++) {
		Q_strncpyz(cgs.domination_points_names[i],CG_Argv(2)+i*MAX_DOMINATION_POINTS_NAMES,MAX_DOMINATION_POINTS_NAMES-1);
		for(j=MAX_DOMINATION_POINTS_NAMES-1; cgs.domination_points_names[i][j] < '0' && j>0; j--) {
			cgs.domination_points_names[i][j] = 0;
		}
	}
}

/*
=================
CG_ParseDomScores
=================
*/

static void CG_ParseDomStatus( void ) {
	int i;
	if( cgs.domination_points_count!=atoi( CG_Argv(1) ) ) {
		cgs.domination_points_count = 0;
		return;
	}
	for(i = 0;i<cgs.domination_points_count;i++) {
		cgs.domination_points_status[i] = atoi( CG_Argv(2+i) );
	}
}

/*
=================
CG_ParseChallenge
=================
*/

static void CG_ParseChallenge( void ) {
	addChallenge(atoi( CG_Argv(1) ) );
}

static void CG_ParseObeliskHealth( void ) {
	cg.redObeliskHealth = atoi( CG_Argv(1) );
cg.blueObeliskHealth = atoi( CG_Argv(2) );
}

/**
 * Sets the respawn counter for the client.
 */
static void CG_ParseRespawnTime( void ) {
	cg.respawnTime = atoi( CG_Argv(1) );
}

/*
=================
CG_ParseTeam
=================
*/

static void CG_ParseTeam( void ) {
	//TODO: Add code here
	if(cg_voip_teamonly.integer) {
		trap_Cvar_Set("cl_voipSendTarget",CG_Argv(1));
	}
}

/*
=================
CG_ParseAttackingTeam

=================
*/
static void CG_ParseAttackingTeam( void ) {
	int temp;
	temp = atoi( CG_Argv( 1 ) );
	if(temp==TEAM_RED) {
		cgs.attackingTeam = TEAM_RED;
	}
	else if (temp==TEAM_BLUE) {
		cgs.attackingTeam = TEAM_BLUE;
	}
	else {
		cgs.attackingTeam = TEAM_NONE; //Should never happen.
	}
}

/*
=================
CG_ParseTeamInfo

=================
*/
static void CG_ParseTeamInfo( void ) {
	int		i;
	int		client;

	numSortedTeamPlayers = atoi( CG_Argv( 1 ) );
	if( numSortedTeamPlayers < 0 || numSortedTeamPlayers > TEAM_MAXOVERLAY )
	{
		CG_Error( "CG_ParseTeamInfo: numSortedTeamPlayers out of range (%d)",
				numSortedTeamPlayers );
		return;
	}

	for ( i = 0 ; i < numSortedTeamPlayers ; i++ ) {
		client = atoi( CG_Argv( i * 6 + 2 ) );
		if( client < 0 || client >= MAX_CLIENTS ) {
			CG_Error( "CG_ParseTeamInfo: bad client number: %d", client );
			return;
		}


		sortedTeamPlayers[i] = client;

		cgs.clientinfo[ client ].location = atoi( CG_Argv( i * 6 + 3 ) );
		cgs.clientinfo[ client ].health = atoi( CG_Argv( i * 6 + 4 ) );
		cgs.clientinfo[ client ].armor = atoi( CG_Argv( i * 6 + 5 ) );
		cgs.clientinfo[ client ].curWeapon = atoi( CG_Argv( i * 6 + 6 ) );
		cgs.clientinfo[ client ].powerups = atoi( CG_Argv( i * 6 + 7 ) );
	}
}


/*
================
CG_ParseServerinfo

This is called explicitly when the gamestate is first received,
and whenever the server updates any serverinfo flagged cvars
================
*/
void CG_ParseServerinfo( void ) {
	const char	*info;
	char	*mapname;

	info = CG_ConfigString( CS_SERVERINFO );
	cgs.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );
	trap_Cvar_Set("g_gametype", va("%i", cgs.gametype));
	cgs.dmflags = atoi( Info_ValueForKey( info, "dmflags" ) );
	cgs.videoflags = atoi( Info_ValueForKey( info, "videoflags" ) );
	cgs.elimflags = atoi( Info_ValueForKey( info, "elimflags" ) );
	cgs.teamflags = atoi( Info_ValueForKey( info, "teamflags" ) );
	cgs.fraglimit = atoi( Info_ValueForKey( info, "fraglimit" ) );
	cgs.capturelimit = atoi( Info_ValueForKey( info, "capturelimit" ) );
	cgs.timelimit = atoi( Info_ValueForKey( info, "timelimit" ) );
	cgs.maxclients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	cgs.roundtime = atoi( Info_ValueForKey( info, "elimination_roundtime" ) );
	cgs.nopickup = atoi( Info_ValueForKey( info, "g_rockets" ) ) + atoi( Info_ValueForKey( info, "g_instantgib" ) ) + atoi( Info_ValueForKey( info, "g_elimination" ) );
	cgs.lms_mode = atoi( Info_ValueForKey( info, "g_lms_mode" ) );
	cgs.altExcellent = atoi( Info_ValueForKey( info, "g_altExcellent" ) );
	mapname = Info_ValueForKey( info, "mapname" );
	Com_sprintf( cgs.mapname, sizeof( cgs.mapname ), "maps/%s.bsp", mapname );
	Q_strncpyz( cgs.redTeam, Info_ValueForKey( info, "g_redTeam" ), sizeof(cgs.redTeam) );
	trap_Cvar_Set("g_redTeam", cgs.redTeam);
	Q_strncpyz( cgs.blueTeam, Info_ValueForKey( info, "g_blueTeam" ), sizeof(cgs.blueTeam) );
	trap_Cvar_Set("g_blueTeam", cgs.blueTeam);

//unlagged - server options
	// we'll need this for deciding whether or not to predict weapon effects
	cgs.delagHitscan = atoi( Info_ValueForKey( info, "g_delagHitscan" ) );
	trap_Cvar_Set("g_delagHitscan", va("%i", cgs.delagHitscan));
//unlagged - server options

	//Copy allowed votes directly to the client:
	trap_Cvar_Set("cg_voteflags",Info_ValueForKey( info, "voteflags" ) );
}

/*
==================
CG_ParseWarmup
==================
*/
static void CG_ParseWarmup( void ) {
	const char	*info;
	int			warmup;

	info = CG_ConfigString( CS_WARMUP );

	warmup = atoi( info );
	cg.warmupCount = -1;

	if ( warmup == 0 && cg.warmup ) {

	} else if ( warmup > 0 && cg.warmup <= 0 ) {
#ifdef MISSIONPACK
		if (CG_IsATeamGametype(cgs.gametype)) {
			trap_S_StartLocalSound( cgs.media.countPrepareTeamSound, CHAN_ANNOUNCER );
		} else
#endif
		{
			trap_S_StartLocalSound( cgs.media.countPrepareSound, CHAN_ANNOUNCER );
		}
	}

	cg.warmup = warmup;
}

/*
================
CG_SetConfigValues

Called on load to set the initial values from configure strings
================
*/
void CG_SetConfigValues( void ) {
	const char *s;

	cgs.scores1 = atoi( CG_ConfigString( CS_SCORES1 ) );
	cgs.scores2 = atoi( CG_ConfigString( CS_SCORES2 ) );
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );
	if((CG_UsesTeamFlags(cgs.gametype) && !CG_UsesTheWhiteFlag(cgs.gametype)) || cgs.gametype == GT_DOUBLE_D) {
		s = CG_ConfigString( CS_FLAGSTATUS );
		cgs.redflag = s[0] - '0';
		cgs.blueflag = s[1] - '0';
	}
//#ifdef MISSIONPACK
	else if( CG_UsesTheWhiteFlag(cgs.gametype) ) {
		s = CG_ConfigString( CS_FLAGSTATUS );
		cgs.flagStatus = s[0] - '0';
	}
//#endif
	cg.warmup = atoi( CG_ConfigString( CS_WARMUP ) );
}

/*
=====================
CG_ShaderStateChanged
=====================
*/
void CG_ShaderStateChanged(void) {
	char originalShader[MAX_QPATH];
	char newShader[MAX_QPATH];
	char timeOffset[16];
	const char *o;
	char *n,*t;

	o = CG_ConfigString( CS_SHADERSTATE );
	while (o && *o) {
		n = strstr(o, "=");
		if (n && *n) {
			strncpy(originalShader, o, n-o);
			originalShader[n-o] = 0;
			n++;
			t = strstr(n, ":");
			if (t && *t) {
				strncpy(newShader, n, t-n);
				newShader[t-n] = 0;
			} else {
				break;
			}
			t++;
			o = strstr(t, "@");
			if (o) {
				strncpy(timeOffset, t, o-t);
				timeOffset[o-t] = 0;
				o++;
				trap_R_RemapShader( originalShader, newShader, timeOffset );
			}
		} else {
			break;
		}
	}
}

/*
================
CG_ConfigStringModified

================
*/
static void CG_ConfigStringModified( void ) {
	const char	*str;
	int		num;

	num = atoi( CG_Argv( 1 ) );

	// get the gamestate from the client system, which will have the
	// new configstring already integrated
	trap_GetGameState( &cgs.gameState );

	// look up the individual string that was modified
	str = CG_ConfigString( num );

	// do something with it if necessary
	if ( num == CS_MUSIC ) {
		CG_StartMusic();	
	} else if ( num == CS_SERVERINFO ) {
		CG_ParseServerinfo();
	} else if ( num == CS_WARMUP ) {
		CG_ParseWarmup();
	} else if ( num == CS_SCORES1 ) {
		cgs.scores1 = atoi( str );
	} else if ( num == CS_SCORES2 ) {
		cgs.scores2 = atoi( str );
	} else if ( num == CS_LEVEL_START_TIME ) {
		cgs.levelStartTime = atoi( str );
	} else if ( num == CS_VOTE_TIME ) {
		cgs.voteTime = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_YES ) {
		cgs.voteYes = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_NO ) {
		cgs.voteNo = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_STRING ) {
		Q_strncpyz( cgs.voteString, str, sizeof( cgs.voteString ) );
#ifdef MISSIONPACK
		trap_S_StartLocalSound( cgs.media.voteNow, CHAN_ANNOUNCER );
#endif //MISSIONPACK
	} else if ( num >= CS_TEAMVOTE_TIME && num <= CS_TEAMVOTE_TIME + 1) {
		cgs.teamVoteTime[num-CS_TEAMVOTE_TIME] = atoi( str );
		cgs.teamVoteModified[num-CS_TEAMVOTE_TIME] = qtrue;
	} else if ( num >= CS_TEAMVOTE_YES && num <= CS_TEAMVOTE_YES + 1) {
		cgs.teamVoteYes[num-CS_TEAMVOTE_YES] = atoi( str );
		cgs.teamVoteModified[num-CS_TEAMVOTE_YES] = qtrue;
	} else if ( num >= CS_TEAMVOTE_NO && num <= CS_TEAMVOTE_NO + 1) {
		cgs.teamVoteNo[num-CS_TEAMVOTE_NO] = atoi( str );
		cgs.teamVoteModified[num-CS_TEAMVOTE_NO] = qtrue;
	} else if ( num >= CS_TEAMVOTE_STRING && num <= CS_TEAMVOTE_STRING + 1) {
		Q_strncpyz( cgs.teamVoteString[num-CS_TEAMVOTE_STRING], str, sizeof( cgs.teamVoteString[0] ) );
#ifdef MISSIONPACK
		trap_S_StartLocalSound( cgs.media.voteNow, CHAN_ANNOUNCER );
#endif	
	} else if ( num == CS_INTERMISSION ) {
		cg.intermissionStarted = atoi( str );
	} else if ( num >= CS_MODELS && num < CS_MODELS+MAX_MODELS ) {
		cgs.gameModels[ num-CS_MODELS ] = trap_R_RegisterModel( str );
	} else if ( num >= CS_SOUNDS && num < CS_SOUNDS+MAX_SOUNDS ) {
		if ( str[0] != '*' ) {	// player specific sounds don't register here
			cgs.gameSounds[ num-CS_SOUNDS] = trap_S_RegisterSound( str, qfalse );
		}
	} else if ( num >= CS_PLAYERS && num < CS_PLAYERS+MAX_CLIENTS ) {
		CG_NewClientInfo( num - CS_PLAYERS );
		CG_BuildSpectatorString();
	} else if ( num == CS_FLAGSTATUS ) {
		if((CG_UsesTeamFlags(cgs.gametype) && CG_UsesTheWhiteFlag(cgs.gametype)) || cgs.gametype == GT_DOUBLE_D) {
			// format is rb where its red/blue, 0 is at base, 1 is taken, 2 is dropped
			cgs.redflag = str[0] - '0';
			cgs.blueflag = str[1] - '0';
		}
		else if( CG_UsesTheWhiteFlag(cgs.gametype) ) {
			cgs.flagStatus = str[0] - '0';
		}
	}
	else if ( num == CS_SHADERSTATE ) {
		CG_ShaderStateChanged();
	}
		
}


/*
=======================
CG_AddToTeamChat

=======================
*/
static void CG_AddToTeamChat( const char *str ) {
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;

	if (cg_teamChatHeight.integer < TEAMCHAT_HEIGHT) {
		chatHeight = cg_teamChatHeight.integer;
	} else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

	if (chatHeight <= 0 || cg_teamChatTime.integer <= 0) {
		// team chat disabled, dump into normal chat
		cgs.teamChatPos = cgs.teamLastChatPos = 0;
		return;
	}

	len = 0;

	p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while (*str) {
		if (len > TEAMCHAT_WIDTH - 1) {
			if (ls) {
				str -= (p - ls);
				str++;
				p -= (p - ls);
			}
			*p = 0;

			cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;

			cgs.teamChatPos++;
			p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if (*str == ' ') {
			ls = p;
		}
		*p++ = *str++;
		len++;
	}
	*p = 0;

	cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
	cgs.teamChatPos++;

	if (cgs.teamChatPos - cgs.teamLastChatPos > chatHeight) {
		cgs.teamLastChatPos = cgs.teamChatPos - chatHeight;
	}
}

/*
===============
CG_MapRestart

The server has issued a map_restart, so the next snapshot
is completely new and should not be interpolated to.

A tournement restart will clear everything, but doesn't
require a reload of all the media
===============
*/
static void CG_MapRestart( void ) {
	if ( cg_showmiss.integer ) {
		CG_Printf( "CG_MapRestart\n" );
	}

	CG_InitLocalEntities();
	CG_InitMarkPolys();

	// make sure the "3 frags left" warnings play again
	cg.fraglimitWarnings = 0;

	cg.timelimitWarnings = 0;

	cg.intermissionStarted = qfalse;

	cgs.voteTime = 0;

	cg.mapRestart = qtrue;

	CG_StartMusic();

	trap_S_ClearLoopingSounds(qtrue);

	// we really should clear more parts of cg here and stop sounds

	// play the "fight" sound if this is a restart without warmup
	if ( cg.warmup == 0 /* && cgs.gametype == GT_TOURNAMENT */) {
		trap_S_StartLocalSound( cgs.media.countFightSound, CHAN_ANNOUNCER );
		CG_CenterPrint( "FIGHT!", 120, GIANTCHAR_WIDTH*2 );
	}
#ifdef MISSIONPACK
	if (cg_singlePlayerActive.integer) {
		trap_Cvar_Set("ui_matchStartTime", va("%i", cg.time));
		if (cg_recordSPDemo.integer && cg_recordSPDemoName.string[0] ) {
			trap_SendConsoleCommand(va("set g_synchronousclients 1 ; record %s \n", cg_recordSPDemoName.string));
		}
	}
#endif
	trap_Cvar_Set("cg_thirdPerson", "0");
}

#define MAX_VOICEFILESIZE	16384
#define MAX_VOICEFILES		8
#define MAX_VOICECHATS		64
#define MAX_VOICESOUNDS		64
#define MAX_CHATSIZE		64
#define MAX_HEADMODELS		64

typedef struct voiceChat_s
{
	char id[64];
	int numSounds;
	sfxHandle_t sounds[MAX_VOICESOUNDS];
	char chats[MAX_VOICESOUNDS][MAX_CHATSIZE];
} voiceChat_t;

typedef struct voiceChatList_s
{
	char name[64];
	int gender;
	int numVoiceChats;
	voiceChat_t voiceChats[MAX_VOICECHATS];
} voiceChatList_t;

typedef struct headModelVoiceChat_s
{
	char headmodel[64];
	int voiceChatNum;
} headModelVoiceChat_t;

voiceChatList_t voiceChatLists[MAX_VOICEFILES];
headModelVoiceChat_t headModelVoiceChat[MAX_HEADMODELS];

/*
=================
CG_ParseVoiceChats
=================
*/
int CG_ParseVoiceChats( const char *filename, voiceChatList_t *voiceChatList, int maxVoiceChats ) {
	int	len, i;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;
	voiceChat_t *voiceChats;
	qboolean compress;
	sfxHandle_t sound;

	compress = qtrue;
	if (cg_buildScript.integer) {
		compress = qfalse;
	}

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "voice chat file not found: %s\n", filename ) );
		return qfalse;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i\n", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return qfalse;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	Com_sprintf(voiceChatList->name, sizeof(voiceChatList->name), "%s", filename);
	voiceChats = voiceChatList->voiceChats;
	for ( i = 0; i < maxVoiceChats; i++ ) {
		voiceChats[i].id[0] = 0;
	}
	token = COM_ParseExt(p, qtrue);
	if (!token[0]) {
		return qtrue;
	}
	if (Q_strequal(token, "female")) {
		voiceChatList->gender = GENDER_FEMALE;
	}
	else if (Q_strequal(token, "male")) {
		voiceChatList->gender = GENDER_MALE;
	}
	else if (Q_strequal(token, "neuter")) {
		voiceChatList->gender = GENDER_NEUTER;
	}
	else {
		trap_Print( va( S_COLOR_RED "expected gender not found in voice chat file: %s\n", filename ) );
		return qfalse;
	}

	voiceChatList->numVoiceChats = 0;
	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);
		if (!token[0]) {
			return qtrue;
		}
		Com_sprintf(voiceChats[voiceChatList->numVoiceChats].id, sizeof( voiceChats[voiceChatList->numVoiceChats].id ), "%s", token);
		token = COM_ParseExt(p, qtrue);
		if ( !Q_strequal(token, "{")) {
			trap_Print( va( S_COLOR_RED "expected { found %s in voice chat file: %s\n", token, filename ) );
			return qfalse;
		}
		voiceChats[voiceChatList->numVoiceChats].numSounds = 0;
		while(1) {
			token = COM_ParseExt(p, qtrue);
			if (!token[0]) {
				return qtrue;
			}
			if (Q_strequal(token, "}"))
				break;
			sound = trap_S_RegisterSound( token, compress );
			voiceChats[voiceChatList->numVoiceChats].sounds[voiceChats[voiceChatList->numVoiceChats].numSounds] = sound;
			token = COM_ParseExt(p, qtrue);
			if (!token[0]) {
				return qtrue;
			}
			Com_sprintf(voiceChats[voiceChatList->numVoiceChats].chats[
							voiceChats[voiceChatList->numVoiceChats].numSounds], MAX_CHATSIZE, "%s", token);
			if (sound)
				voiceChats[voiceChatList->numVoiceChats].numSounds++;
			if (voiceChats[voiceChatList->numVoiceChats].numSounds >= MAX_VOICESOUNDS)
				break;
		}
		voiceChatList->numVoiceChats++;
		if (voiceChatList->numVoiceChats >= maxVoiceChats)
			return qtrue;
	}
	return qtrue;
}

/*
=================
CG_LoadVoiceChats
=================
*/
void CG_LoadVoiceChats( void ) {
	int size;

	size = trap_MemoryRemaining();
	CG_ParseVoiceChats( "scripts/female1.voice", &voiceChatLists[0], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/female2.voice", &voiceChatLists[1], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/female3.voice", &voiceChatLists[2], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/male1.voice", &voiceChatLists[3], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/male2.voice", &voiceChatLists[4], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/male3.voice", &voiceChatLists[5], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/male4.voice", &voiceChatLists[6], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/male5.voice", &voiceChatLists[7], MAX_VOICECHATS );
	CG_Printf("voice chat memory size = %d\n", size - trap_MemoryRemaining());
}

/*
=================
CG_HeadModelVoiceChats
=================
*/
int CG_HeadModelVoiceChats( char *filename ) {
	int	len, i;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		//trap_Print( va( "voice chat file not found: %s\n", filename ) );
		return -1;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i\n", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return -1;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	token = COM_ParseExt(p, qtrue);
	if (!token[0]) {
		return -1;
	}

	for ( i = 0; i < MAX_VOICEFILES; i++ ) {
		if ( Q_strequal(token, voiceChatLists[i].name) ) {
			return i;
		}
	}

	//FIXME: maybe try to load the .voice file which name is stored in token?

	return -1;
}


/*
=================
CG_GetVoiceChat
=================
*/
int CG_GetVoiceChat( voiceChatList_t *voiceChatList, const char *id, sfxHandle_t *snd, char **chat) {
	int i, rnd;

	for ( i = 0; i < voiceChatList->numVoiceChats; i++ ) {
		if ( Q_strequal( id, voiceChatList->voiceChats[i].id ) ) {
			rnd = random() * voiceChatList->voiceChats[i].numSounds;
			*snd = voiceChatList->voiceChats[i].sounds[rnd];
			*chat = voiceChatList->voiceChats[i].chats[rnd];
			return qtrue;
		}
	}
	return qfalse;
}

/*
=================
CG_VoiceChatListForClient
=================
*/
voiceChatList_t *CG_VoiceChatListForClient( int clientNum ) {
	clientInfo_t *ci;
	int voiceChatNum, i, j, k, gender;
	char filename[MAX_QPATH], headModelName[MAX_QPATH];

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	for ( k = 0; k < 2; k++ ) {
		if ( k == 0 ) {
			if (ci->headModelName[0] == '*') {
				Com_sprintf( headModelName, sizeof(headModelName), "%s/%s", ci->headModelName+1, ci->headSkinName );
			}
			else {
				Com_sprintf( headModelName, sizeof(headModelName), "%s/%s", ci->headModelName, ci->headSkinName );
			}
		}
		else {
			if (ci->headModelName[0] == '*') {
				Com_sprintf( headModelName, sizeof(headModelName), "%s", ci->headModelName+1 );
			}
			else {
				Com_sprintf( headModelName, sizeof(headModelName), "%s", ci->headModelName );
			}
		}
		// find the voice file for the head model the client uses
		for ( i = 0; i < MAX_HEADMODELS; i++ ) {
			if (Q_strequal(headModelVoiceChat[i].headmodel, headModelName)) {
				break;
			}
		}
		if (i < MAX_HEADMODELS) {
			return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
		}
		// find a <headmodelname>.vc file
		for ( i = 0; i < MAX_HEADMODELS; i++ ) {
			if (!strlen(headModelVoiceChat[i].headmodel)) {
				Com_sprintf(filename, sizeof(filename), "scripts/%s.vc", headModelName);
				voiceChatNum = CG_HeadModelVoiceChats(filename);
				if (voiceChatNum == -1)
					break;
				Com_sprintf(headModelVoiceChat[i].headmodel, sizeof ( headModelVoiceChat[i].headmodel ),
							"%s", headModelName);
				headModelVoiceChat[i].voiceChatNum = voiceChatNum;
				return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
			}
		}
	}
	gender = ci->gender;
	for (k = 0; k < 2; k++) {
		// just pick the first with the right gender
		for ( i = 0; i < MAX_VOICEFILES; i++ ) {
			if (strlen(voiceChatLists[i].name)) {
				if (voiceChatLists[i].gender == gender) {
					// store this head model with voice chat for future reference
					for ( j = 0; j < MAX_HEADMODELS; j++ ) {
						if (!strlen(headModelVoiceChat[j].headmodel)) {
							Com_sprintf(headModelVoiceChat[j].headmodel, sizeof ( headModelVoiceChat[j].headmodel ),
									"%s", headModelName);
							headModelVoiceChat[j].voiceChatNum = i;
							break;
						}
					}
					return &voiceChatLists[i];
				}
			}
		}
		// fall back to male gender because we don't have neuter in the mission pack
		if (gender == GENDER_MALE)
			break;
		gender = GENDER_MALE;
	}
	// store this head model with voice chat for future reference
	for ( j = 0; j < MAX_HEADMODELS; j++ ) {
		if (!strlen(headModelVoiceChat[j].headmodel)) {
			Com_sprintf(headModelVoiceChat[j].headmodel, sizeof ( headModelVoiceChat[j].headmodel ),
					"%s", headModelName);
			headModelVoiceChat[j].voiceChatNum = 0;
			break;
		}
	}
	// just return the first voice chat list
	return &voiceChatLists[0];
}

#define MAX_VOICECHATBUFFER		32

typedef struct bufferedVoiceChat_s
{
	int clientNum;
	sfxHandle_t snd;
	int voiceOnly;
	char cmd[MAX_SAY_TEXT];
	char message[MAX_SAY_TEXT];
} bufferedVoiceChat_t;

bufferedVoiceChat_t voiceChatBuffer[MAX_VOICECHATBUFFER];

/*
=================
CG_PlayVoiceChat
=================
*/
void CG_PlayVoiceChat( bufferedVoiceChat_t *vchat ) {
#ifdef MISSIONPACK
	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted ) {
		return;
	}

	if ( !cg_noVoiceChats.integer ) {
		trap_S_StartLocalSound( vchat->snd, CHAN_VOICE);
		if (vchat->clientNum != cg.snap->ps.clientNum) {
			int orderTask = CG_ValidOrder(vchat->cmd);
			if (orderTask > 0) {
				cgs.acceptOrderTime = cg.time + 5000;
				Q_strncpyz(cgs.acceptVoice, vchat->cmd, sizeof(cgs.acceptVoice));
				cgs.acceptTask = orderTask;
				cgs.acceptLeader = vchat->clientNum;
			}
			// see if this was an order
			CG_ShowResponseHead();
		}
	}
	if (!vchat->voiceOnly && !cg_noVoiceText.integer) {
		CG_AddToTeamChat( vchat->message );
		CG_Printf( "%s\n", vchat->message );
	}
	voiceChatBuffer[cg.voiceChatBufferOut].snd = 0;
#endif
}

/*
=====================
CG_PlayBufferedVoieChats
=====================
*/
void CG_PlayBufferedVoiceChats( void ) {
#ifdef MISSIONPACK
	if ( cg.voiceChatTime < cg.time ) {
		if (cg.voiceChatBufferOut != cg.voiceChatBufferIn && voiceChatBuffer[cg.voiceChatBufferOut].snd) {
			//
			CG_PlayVoiceChat(&voiceChatBuffer[cg.voiceChatBufferOut]);
			//
			cg.voiceChatBufferOut = (cg.voiceChatBufferOut + 1) % MAX_VOICECHATBUFFER;
			cg.voiceChatTime = cg.time + 1000;
		}
	}
#endif
}

/*
=====================
CG_AddBufferedVoiceChat
=====================
*/
void CG_AddBufferedVoiceChat( bufferedVoiceChat_t *vchat ) {
#ifdef MISSIONPACK
	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted ) {
		return;
	}

	memcpy(&voiceChatBuffer[cg.voiceChatBufferIn], vchat, sizeof(bufferedVoiceChat_t));
	cg.voiceChatBufferIn = (cg.voiceChatBufferIn + 1) % MAX_VOICECHATBUFFER;
	if (cg.voiceChatBufferIn == cg.voiceChatBufferOut) {
		CG_PlayVoiceChat( &voiceChatBuffer[cg.voiceChatBufferOut] );
		cg.voiceChatBufferOut++;
	}
#endif
}

/*
=================
CG_VoiceChatLocal
=================
*/
void CG_VoiceChatLocal( int mode, qboolean voiceOnly, int clientNum, int color, const char *cmd ) {
#ifdef MISSIONPACK
	char *chat;
	voiceChatList_t *voiceChatList;
	clientInfo_t *ci;
	sfxHandle_t snd;
	bufferedVoiceChat_t vchat;

	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted ) {
		return;
	}

	if ( mode == SAY_ALL && CG_IsATeamGametype(cgs.gametype) && cg_teamChatsOnly.integer ) {
		return;
	}

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	cgs.currentVoiceClient = clientNum;

	voiceChatList = CG_VoiceChatListForClient( clientNum );

	if ( CG_GetVoiceChat( voiceChatList, cmd, &snd, &chat ) ) {
		vchat.clientNum = clientNum;
		vchat.snd = snd;
		vchat.voiceOnly = voiceOnly;
		Q_strncpyz(vchat.cmd, cmd, sizeof(vchat.cmd));
		if ( mode == SAY_TELL ) {
			Com_sprintf(vchat.message, sizeof(vchat.message), "[%s]: %c%c%s", ci->name, Q_COLOR_ESCAPE, color, chat);
		} else if ( mode == SAY_TEAM ) {
			Com_sprintf(vchat.message, sizeof(vchat.message), "(%s): %c%c%s", ci->name, Q_COLOR_ESCAPE, color, chat);
		} else {
			Com_sprintf(vchat.message, sizeof(vchat.message), "%s: %c%c%s", ci->name, Q_COLOR_ESCAPE, color, chat);
		}
		CG_AddBufferedVoiceChat(&vchat);
	}
#endif
}

/*
=================
CG_VoiceChat
=================
*/
void CG_VoiceChat( int mode ) {
#ifdef MISSIONPACK
	const char *cmd;
	int clientNum, color;
	qboolean voiceOnly;

	voiceOnly = atoi(CG_Argv(1));
	clientNum = atoi(CG_Argv(2));
	color = atoi(CG_Argv(3));
	cmd = CG_Argv(4);

	if (cg_noTaunt.integer != 0) {
		if (strequals(cmd, VOICECHAT_KILLINSULT)  || strequals(cmd, VOICECHAT_TAUNT) || 
			strequals(cmd, VOICECHAT_DEATHINSULT) || strequals(cmd, VOICECHAT_KILLGAUNTLET) || 
			strequals(cmd, VOICECHAT_PRAISE)) {
			return;
		}
	}

	CG_VoiceChatLocal( mode, voiceOnly, clientNum, color, cmd );
#endif
}

/*
=================
CG_RemoveChatEscapeChar
=================
*/
static void CG_RemoveChatEscapeChar( char *text ) {
	int i, l;

	l = 0;
	for ( i = 0; text[i]; i++ ) {
		if (text[i] == '\x19')
			continue;
		text[l++] = text[i];
	}
	text[l] = '\0';
}

/*
=================
CG_ServerCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
static void CG_ServerCommand( void ) {
	const char	*cmd;
	char		text[MAX_SAY_TEXT];

	cmd = CG_Argv(0);

	if ( !cmd[0] ) {
		// server claimed the command
		return;
	}

	if ( strequals( cmd, "cp" ) ) {
		CG_CenterPrint( CG_Argv(1), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
		return;
	}

	if ( strequals( cmd, "cs" ) ) {
		CG_ConfigStringModified();
		return;
	}

	if ( strequals( cmd, "print" ) ) {
		CG_Printf( "%s", CG_Argv(1) );
#ifdef MISSIONPACK
		cmd = CG_Argv(1);			// yes, this is obviously a hack, but so is the way we hear about
									// votes passing or failing
		if ( Q_strequaln( cmd, "vote failed", 11 ) || Q_strequaln( cmd, "team vote failed", 16 )) {
			trap_S_StartLocalSound( cgs.media.voteFailed, CHAN_ANNOUNCER );
		} else if ( Q_strequaln( cmd, "vote passed", 11 ) || Q_strequaln( cmd, "team vote passed", 16 ) ) {
			trap_S_StartLocalSound( cgs.media.votePassed, CHAN_ANNOUNCER );
		}
#endif
		return;
	}

	if ( strequals( cmd, "chat" ) ) {
		if ( CG_IsATeamGametype(cgs.gametype) && cg_teamChatsOnly.integer ) {
			return;
		}
		if( cg_chatBeep.integer ) {
			trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		}
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		Q_strncpyz( text, CG_Argv(1), MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );
		CG_Printf( "%s\n", text );
		return;
	}

	if ( strequals( cmd, "tchat" ) ) {
		if( cg_teamChatBeep.integer ) {
			trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		}
		Q_strncpyz( text, CG_Argv(1), MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );
		CG_AddToTeamChat( text );
		CG_Printf( "%s\n", text );
		return;
	}
	if ( strequals( cmd, "vchat" ) ) {
		CG_VoiceChat( SAY_ALL );
		return;
	}

	if ( strequals( cmd, "vtchat" ) ) {
		CG_VoiceChat( SAY_TEAM );
		return;
	}

	if ( strequals( cmd, "vtell" ) ) {
		CG_VoiceChat( SAY_TELL );
		return;
	}

	if ( strequals( cmd, "scores" ) ) {
		CG_ParseScores();
		return;
	}


	if ( strequals( cmd, "accs" ) ) {
		CG_ParseAccuracy();
		return;
	}


	if ( strequals( cmd, "ddTakeAt" ) ) {
		CG_ParseDDtimetakeAt();
		return;
	}

	if ( strequals( cmd, "dompointnames" ) ) {
		CG_ParseDomPointNames();
		return;
	}

	if ( strequals( cmd, "domStatus" ) ) {
		CG_ParseDomStatus();
		return;
	}

	if ( strequals( cmd, "elimination" ) ) {
		CG_ParseElimination();
		return;
	}

	if ( strequals( cmd, "mappage" ) ) {
		CG_ParseMappage();
		return;
	}

	if ( strequals( cmd, "attackingteam" ) ) {
		CG_ParseAttackingTeam();
		return;
	}

	if ( strequals( cmd, "tinfo" ) ) {
		CG_ParseTeamInfo();
		return;
	}

	if ( strequals( cmd, "map_restart" ) ) {
		CG_MapRestart();
		return;
	}

	if ( Q_strequal (cmd, "remapShader") )
	{
		if (trap_Argc() == 4)
		{
			char shader1[MAX_QPATH];
			char shader2[MAX_QPATH];
			char shader3[MAX_QPATH];

			Q_strncpyz(shader1, CG_Argv(1), sizeof(shader1));
			Q_strncpyz(shader2, CG_Argv(2), sizeof(shader2));
			Q_strncpyz(shader3, CG_Argv(3), sizeof(shader3));

			trap_R_RemapShader(shader1, shader2, shader3);
		}
		
		return;
	}

	// loaddeferred can be both a servercmd and a consolecmd
	if ( strequals( cmd, "loaddefered" ) ) {	// FIXME: spelled wrong, but not changing for demo
		CG_LoadDeferredPlayers();
		return;
	}

	// clientLevelShot is sent before taking a special screenshot for
	// the menu system during development
	if ( strequals( cmd, "clientLevelShot" ) ) {
		cg.levelShot = qtrue;
		return;
	}

	// challenge completed is determened by the server. A client should consider this message valid:
	if ( strequals( cmd, "ch" ) ) {
		CG_ParseChallenge();
		return;
	}

	if ( strequals (cmd, "oh") ) {
		CG_ParseObeliskHealth();
		return;
	}

	if ( strequals( cmd, "respawn" ) ) {
		CG_ParseRespawnTime();
		return;
	}

	if ( strequals( cmd, "team" ) ) {
		CG_ParseTeam();
		return;
	}

	if ( strequals( cmd, "customvotes" ) ) {
		char infoString[1024];
		int i;
		//TODO: Create a ParseCustomvotes function
		memset(&infoString,0,sizeof(infoString));
		for(i=1;i<=12;i++) {
			Q_strcat(infoString,sizeof(infoString),CG_Argv( i ));
			Q_strcat(infoString,sizeof(infoString)," ");
		}
		trap_Cvar_Set("cg_vote_custom_commands",infoString);
		return;
	}

	CG_Printf( "Unknown client game command: %s\n", cmd );
}


/*
====================
CG_ExecuteNewServerCommands

Execute all of the server commands that were received along
with this this snapshot.
====================
*/
void CG_ExecuteNewServerCommands( int latestSequence ) {
	while ( cgs.serverCommandSequence < latestSequence ) {
		if ( trap_GetServerCommand( ++cgs.serverCommandSequence ) ) {
			CG_ServerCommand();
		}
	}
}

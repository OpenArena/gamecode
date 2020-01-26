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

/*****************************************************************************
 * name:		ai_vcmd.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /MissionPack/code/game/ai_vcmd.c $
 *
 *****************************************************************************/

#include "g_local.h"
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ea.h"
#include "../botlib/be_ai_char.h"
#include "../botlib/be_ai_chat.h"
#include "../botlib/be_ai_gen.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "../botlib/be_ai_weap.h"
//
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_chat.h"
#include "ai_cmd.h"
#include "ai_dmnet.h"
#include "ai_team.h"
#include "ai_vcmd.h"
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars

// for the voice chats
#include "../../ui/menudef.h"


typedef struct voiceCommand_s
{
	char *cmd;
	void (*func)(bot_state_t *bs, int client, int mode);
} voiceCommand_t;

/*
==================
BotVoiceChat_GetFlag
==================
*/
void BotVoiceChat_GetFlag(bot_state_t *bs, int client, int mode) {
	//
	if (!(GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype) || GAMETYPE_USES_WHITE_FLAG(gametype))) {
		return;
	}
	if (GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype)) {
		if (!ctf_redflag.areanum || !ctf_blueflag.areanum)
			return;
	}
	else if (GAMETYPE_USES_WHITE_FLAG(gametype)) {
		if (!ctf_neutralflag.areanum)
			return;
	}
	else {
		return;
	}
	// Get an alternate route towards the enemy base in CTF/eCTF.
	if (GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype) && !GAMETYPE_USES_WHITE_FLAG(gametype)) {
 		BotGetAlternateRouteGoal(bs, BotOppositeTeam(bs));
 	}
	BotChat_SetAction(bs, client, LTG_GETFLAG, CTF_GETFLAG_TIME);
}

/*
==================
BotVoiceChat_Offense
==================
*/
void BotVoiceChat_Offense(bot_state_t *bs, int client, int mode) {
	if(!(GAMETYPE_IS_A_TEAM_GAME(gametype) || GAMETYPE_USES_KEY_OBJECTIVES(gametype))) {
		return;
	}
	if (GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype)) {
		BotVoiceChat_GetFlag(bs, client, mode);
		return;
	}
	if (gametype == GT_HARVESTER) {
		//away from harvesting
 		bs->harvestaway_time = 0;
		BotChat_SetAction(bs, client, LTG_HARVEST, TEAM_HARVEST_TIME);
	}
	if (gametype == GT_OBELISK) {
		//away from attacking
 		bs->attackaway_time = 0;
		BotChat_SetAction(bs, client, LTG_ATTACKENEMYBASE, TEAM_ATTACKENEMYBASE_TIME);
	}
	if (gametype == GT_DOUBLE_D) {
		if (bs->ltgtype == LTG_POINTA) {
			//away from controlling point A
			bs->dd_pointaaway_time = 0;
			BotChat_SetAction(bs, client, LTG_POINTA, DD_POINTA);
		}
		else if (bs->ltgtype == LTG_POINTB) {
			//away from controlling point B
			bs->dd_pointbaway_time = 0;
			BotChat_SetAction(bs, client, LTG_POINTB, DD_POINTB);
		}
	}
}

/*
==================
BotVoiceChat_Defend
==================
*/
void BotVoiceChat_Defend(bot_state_t *bs, int client, int mode) {
	if(!GAMETYPE_IS_A_TEAM_GAME(gametype) && !GAMETYPE_USES_KEY_OBJECTIVES(gametype)) {
		return;
	}
	if (GAMETYPE_USES_OBELISKS(gametype)) {
		//
		switch(BotTeam(bs)) {
			case TEAM_RED: memcpy(&bs->teamgoal, &redobelisk, sizeof(bot_goal_t)); break;
			case TEAM_BLUE: memcpy(&bs->teamgoal, &blueobelisk, sizeof(bot_goal_t)); break;
			default: return;
		}
	}
	else
		if (GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype)) {
                    //
                    switch(BotTeam(bs)) {
			case TEAM_RED: memcpy(&bs->teamgoal, &ctf_redflag, sizeof(bot_goal_t)); break;
			case TEAM_BLUE: memcpy(&bs->teamgoal, &ctf_blueflag, sizeof(bot_goal_t)); break;
			default: return;
		}
	}
	else if (gametype == GT_DOUBLE_D) {
		switch(BotTeam(bs)) {
			case TEAM_RED:
				if(level.pointStatusA == TEAM_RED) {
					memcpy(&bs->teamgoal, &ctf_redflag, sizeof(bot_goal_t));
				}
				else if(level.pointStatusB == TEAM_RED) {
					memcpy(&bs->teamgoal, &ctf_blueflag, sizeof(bot_goal_t));
				}
				break;
			case TEAM_BLUE:
				if(level.pointStatusA == TEAM_BLUE) {
					memcpy(&bs->teamgoal, &ctf_redflag, sizeof(bot_goal_t));
				}
				else if(level.pointStatusB == TEAM_BLUE) {
					memcpy(&bs->teamgoal, &ctf_blueflag, sizeof(bot_goal_t));
				}
				break;
			default:
				break;
		}
	}
	else {
		return;
	}
	//away from defending
	bs->defendaway_time = 0;
	BotChat_SetAction(bs, client, LTG_DEFENDKEYAREA, TEAM_DEFENDKEYAREA_TIME);
}

/*
==================
BotVoiceChat_DefendFlag
==================
*/
void BotVoiceChat_DefendFlag(bot_state_t *bs, int client, int mode) {
	BotVoiceChat_Defend(bs, client, mode);
}

/*
==================
BotVoiceChat_Patrol
==================
*/
void BotVoiceChat_Patrol(bot_state_t *bs, int client, int mode) {
	//
	bs->decisionmaker = client;
	//
	bs->ltgtype = 0;
	bs->lead_time = 0;
	bs->lastgoal_ltgtype = 0;
	//
	BotAI_BotInitialChat(bs, "dismissed", NULL);
	trap_BotEnterChat(bs->cs, client, CHAT_TELL);
	BotVoiceChatOnly(bs, -1, VOICECHAT_ONPATROL);
	//
	BotSetTeamStatus(bs);
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

/*
==================
BotVoiceChat_Camp
==================
*/
void BotVoiceChat_Camp(bot_state_t *bs, int client, int mode) {
	int areanum;
	aas_entityinfo_t entinfo;
	char netname[MAX_NETNAME];

	//
	bs->teamgoal.entitynum = -1;
	BotEntityInfo(client, &entinfo);
	//if info is valid (in PVS)
	if (entinfo.valid) {
		areanum = BotPointAreaNum(entinfo.origin);
		if (areanum) { /* && trap_AAS_AreaReachability(areanum)) {
			//NOTE: just assume the bot knows where the person is
			//if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, client)) { */
				bs->teamgoal.entitynum = client;
				bs->teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			/* } */
		}
	}
	//if the other is not visible
	if (bs->teamgoal.entitynum < 0) {
		BotAI_BotInitialChat(bs, "whereareyou", EasyClientName(client, netname, sizeof(netname)), NULL);
		trap_BotEnterChat(bs->cs, client, CHAT_TELL);
		return;
	}
 	//the teammate that requested the camping
	bs->teammate = client;
	//not arrived yet
	bs->arrive_time = 0;
	BotChat_SetAction(bs, client, LTG_CAMPORDER, TEAM_CAMP_TIME);
}

/*
==================
BotVoiceChat_FollowMe
==================
*/
void BotVoiceChat_FollowMe(bot_state_t *bs, int client, int mode) {
	int areanum;
	aas_entityinfo_t entinfo;
	char netname[MAX_NETNAME];

	bs->teamgoal.entitynum = -1;
	BotEntityInfo(client, &entinfo);
	//if info is valid (in PVS)
	if (entinfo.valid) {
		areanum = BotPointAreaNum(entinfo.origin);
		if (areanum) { // && trap_AAS_AreaReachability(areanum)) {
			bs->teamgoal.entitynum = client;
			bs->teamgoal.areanum = areanum;
			VectorCopy(entinfo.origin, bs->teamgoal.origin);
			VectorSet(bs->teamgoal.mins, -8, -8, -8);
			VectorSet(bs->teamgoal.maxs, 8, 8, 8);
		}
	}
	//if the other is not visible
	if (bs->teamgoal.entitynum < 0) {
		BotAI_BotInitialChat(bs, "whereareyou", EasyClientName(client, netname, sizeof(netname)), NULL);
		trap_BotEnterChat(bs->cs, client, CHAT_TELL);
		return;
	}
	//the team mate
	bs->teammate = client;
	//last time the team mate was assumed visible
	bs->teammatevisible_time = FloatTime();
	bs->formation_dist = 3.5 * 32;		//3.5 meter
	bs->arrive_time = 0;
	BotChat_SetAction(bs, client, LTG_TEAMACCOMPANY, TEAM_ACCOMPANY_TIME);
}

/*
==================
BotVoiceChat_FollowFlagCarrier
==================
*/
void BotVoiceChat_FollowFlagCarrier(bot_state_t *bs, int client, int mode) {
	int carrier;

	carrier = BotTeamFlagCarrier(bs);
	if (carrier >= 0)
		BotVoiceChat_FollowMe(bs, carrier, mode);
#ifdef DEBUG
	BotPrintTeamGoal(bs);
#endif //DEBUG
}

/*
==================
BotVoiceChat_ReturnFlag
==================
*/
void BotVoiceChat_ReturnFlag(bot_state_t *bs, int client, int mode) {
	//if not in CTF mode
	if (!GAMETYPE_USES_RED_AND_BLUE_FLAG(gametype)) {
		return;
	}
	//Also not valid for 1FCTF
	if (GAMETYPE_USES_WHITE_FLAG(gametype)) {
		return;
	}
	bs->rushbaseaway_time = 0;
	BotChat_SetAction(bs, client, LTG_RETURNFLAG, CTF_RETURNFLAG_TIME);
}

/*
==================
BotVoiceChat_StartLeader
==================
*/
void BotVoiceChat_StartLeader(bot_state_t *bs, int client, int mode) {
	ClientName(client, bs->teamleader, sizeof(bs->teamleader));
}

/*
==================
BotVoiceChat_StopLeader
==================
*/
void BotVoiceChat_StopLeader(bot_state_t *bs, int client, int mode) {
	char netname[MAX_MESSAGE_SIZE];

	if (Q_strequal(bs->teamleader, ClientName(client, netname, sizeof(netname)))) {
		bs->teamleader[0] = '\0';
		notleader[client] = qtrue;
	}
}

/*
==================
BotVoiceChat_WhoIsLeader
==================
*/
void BotVoiceChat_WhoIsLeader(bot_state_t *bs, int client, int mode) {
	char netname[MAX_MESSAGE_SIZE];

	// Only in team-based games.
	if (!GAMETYPE_IS_A_TEAM_GAME(gametype)) {
		return;
	}

	ClientName(bs->client, netname, sizeof(netname));
	//if this bot IS the team leader
	if (Q_strequal(netname, bs->teamleader)) {
		BotAI_BotInitialChat(bs, "iamteamleader", NULL);
		trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
		BotVoiceChatOnly(bs, -1, VOICECHAT_STARTLEADER);
	}
}

/*
==================
BotVoiceChat_WantOnDefense
==================
*/
void BotVoiceChat_WantOnDefense(bot_state_t *bs, int client, int mode) {
	char netname[MAX_NETNAME];
	int preference;

	preference = BotGetTeamMateTaskPreference(bs, client);
	preference &= ~TEAMTP_ATTACKER;
	preference |= TEAMTP_DEFENDER;
	BotSetTeamMateTaskPreference(bs, client, preference);
	//
	EasyClientName(client, netname, sizeof(netname));
	BotAI_BotInitialChat(bs, "keepinmind", netname, NULL);
	trap_BotEnterChat(bs->cs, client, CHAT_TELL);
	BotVoiceChatOnly(bs, client, VOICECHAT_YES);
	trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
}

/*
==================
BotVoiceChat_WantOnOffense
==================
*/
void BotVoiceChat_WantOnOffense(bot_state_t *bs, int client, int mode) {
	char netname[MAX_NETNAME];
	int preference;

	preference = BotGetTeamMateTaskPreference(bs, client);
	preference &= ~TEAMTP_DEFENDER;
	preference |= TEAMTP_ATTACKER;
	BotSetTeamMateTaskPreference(bs, client, preference);
	//
	EasyClientName(client, netname, sizeof(netname));
	BotAI_BotInitialChat(bs, "keepinmind", netname, NULL);
	trap_BotEnterChat(bs->cs, client, CHAT_TELL);
	BotVoiceChatOnly(bs, client, VOICECHAT_YES);
	trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
}

void BotVoiceChat_Dummy(bot_state_t *bs, int client, int mode) {
}

voiceCommand_t voiceCommands[] = {
	{VOICECHAT_GETFLAG, BotVoiceChat_GetFlag},
	{VOICECHAT_OFFENSE, BotVoiceChat_Offense },
	{VOICECHAT_DEFEND, BotVoiceChat_Defend },
	{VOICECHAT_DEFENDFLAG, BotVoiceChat_DefendFlag },
	{VOICECHAT_PATROL, BotVoiceChat_Patrol },
	{VOICECHAT_CAMP, BotVoiceChat_Camp },
	{VOICECHAT_FOLLOWME, BotVoiceChat_FollowMe },
	{VOICECHAT_FOLLOWFLAGCARRIER, BotVoiceChat_FollowFlagCarrier },
	{VOICECHAT_RETURNFLAG, BotVoiceChat_ReturnFlag },
	{VOICECHAT_STARTLEADER, BotVoiceChat_StartLeader },
	{VOICECHAT_STOPLEADER, BotVoiceChat_StopLeader },
	{VOICECHAT_WHOISLEADER, BotVoiceChat_WhoIsLeader },
	{VOICECHAT_WANTONDEFENSE, BotVoiceChat_WantOnDefense },
	{VOICECHAT_WANTONOFFENSE, BotVoiceChat_WantOnOffense },
	{NULL, BotVoiceChat_Dummy}
};

int BotVoiceChatCommand(bot_state_t *bs, int mode, char *voiceChat) {
	int i, clientNum;
	char *ptr, buf[MAX_MESSAGE_SIZE], *cmd;

	if (!GAMETYPE_IS_A_TEAM_GAME(gametype)) {
		return qfalse;
	}

	if ( mode == SAY_ALL ) {
		return qfalse;	// don't do anything with voice chats to everyone
	}

	Q_strncpyz(buf, voiceChat, sizeof(buf));
	cmd = buf;
	for (ptr = cmd; *cmd && *cmd > ' '; cmd++);
	while (*cmd && *cmd <= ' ') *cmd++ = '\0';
	for (ptr = cmd; *cmd && *cmd > ' '; cmd++);
	while (*cmd && *cmd <= ' ') *cmd++ = '\0';
	clientNum = atoi(ptr);
	for (ptr = cmd; *cmd && *cmd > ' '; cmd++);
	while (*cmd && *cmd <= ' ') *cmd++ = '\0';

	if (!BotSameTeam(bs, clientNum)) {
		return qfalse;
	}

	for (i = 0; voiceCommands[i].cmd; i++) {
		if (Q_strequal(cmd, voiceCommands[i].cmd)) {
			voiceCommands[i].func(bs, clientNum, mode);
			return qtrue;
		}
	}
	return qfalse;
}

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
 * name:		ai_cmd.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /MissionPack/code/game/ai_cmd.c $
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
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars

// for the voice chats
#include "../../ui/menudef.h"

int notleader[MAX_CLIENTS];

/*
==================
BotPrintTeamGoal
==================
*/
void BotPrintTeamGoal(bot_state_t *bs) {
	char netname[MAX_NETNAME];
	float t;

	// Exit if not in the adequate devmode for bots.
	if (!bot_developer.integer && !bot_debugLTG.integer) {
		return;
	}

	ClientName(bs->client, netname, sizeof(netname));
	t = bs->teamgoal_time - FloatTime();
	switch(bs->ltgtype) {
		case LTG_TEAMHELP:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna help a team mate for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_TEAMACCOMPANY:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna accompany a team mate for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_GETFLAG:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna get the flag for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_RUSHBASE:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna rush to the base for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_RETURNFLAG:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna try to return the flag for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_ATTACKENEMYBASE:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna attack the enemy base for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_HARVEST:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna harvest for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_DEFENDKEYAREA:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna defend a key area for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_GETITEM:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna get an item for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_KILL:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna kill someone for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_CAMP:
		case LTG_CAMPORDER:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna camp for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_PATROL:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna patrol for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_HOLDPOINTA:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna take care of point A for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_HOLDPOINTB:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna take care of point B for %1.0f secs\n", netname, t);
			break;
		}
		case LTG_HOLDDOMPOINT:
		{
			BotAI_Print(PRT_MESSAGE, "%s: I'm gonna defend DOM point %i for %1.0f secs\n", netname, BotGetDominationPoint(bs), t);
			break;
		}
		default:
		{
			if (bs->ctfroam_time > FloatTime()) {
				t = bs->ctfroam_time - FloatTime();
				BotAI_Print(PRT_MESSAGE, "%s: I'm gonna roam for %1.0f secs\n", netname, t);
			}
			else {
				BotAI_Print(PRT_MESSAGE, "%s: I've got a regular goal\n", netname);
			}
		}
	}
}

/*
==================
BotGetItemTeamGoal

FIXME: add stuff like "upper rocket launcher"
"the rl near the railgun", "lower grenade launcher" etc.
==================
*/
int BotGetItemTeamGoal(char *goalname, bot_goal_t *goal) {
	int i;

	if (!strlen(goalname)) return qfalse;
	i = -1;
	do {
		i = trap_BotGetLevelItemGoal(i, goalname, goal);
		if (i > 0) {
			//do NOT defend dropped items
			if (goal->flags & GFL_DROPPED)
				continue;
			return qtrue;
		}
	} while(i > 0);
	return qfalse;
}

/*
==================
BotGetMessageTeamGoal
==================
*/
int BotGetMessageTeamGoal(bot_state_t *bs, char *goalname, bot_goal_t *goal) {
	bot_waypoint_t *cp;

	if (BotGetItemTeamGoal(goalname, goal)) return qtrue;

	cp = BotFindWayPoint(bs->checkpoints, goalname);
	if (cp) {
		memcpy(goal, &cp->goal, sizeof(bot_goal_t));
		return qtrue;
	}
	return qfalse;
}

/*
==================
BotGetTime
==================
*/
float BotGetTime(bot_match_t *match) {
	bot_match_t timematch;
	char timestring[MAX_MESSAGE_SIZE];
	float t;

	//if the matched string has a time
	if (match->subtype & ST_TIME) {
		//get the time string
		trap_BotMatchVariable(match, TIME, timestring, MAX_MESSAGE_SIZE);
		//match it to find out if the time is in seconds or minutes
		if (trap_BotFindMatch(timestring, &timematch, MTCONTEXT_TIME)) {
			if (timematch.type == MSG_FOREVER) {
				t = 99999999.0f;
			}
			else if (timematch.type == MSG_FORAWHILE) {
				t = 10 * 60; // 10 minutes
			}
			else if (timematch.type == MSG_FORALONGTIME) {
				t = 30 * 60; // 30 minutes
			}
			else {
				trap_BotMatchVariable(&timematch, TIME, timestring, MAX_MESSAGE_SIZE);
				if (timematch.type == MSG_MINUTES) t = atof(timestring) * 60;
				else if (timematch.type == MSG_SECONDS) t = atof(timestring);
				else t = 0;
			}
			//if there's a valid time
			if (t > 0) return FloatTime() + t;
		}
	}
	return 0;
}

/*
==================
FindClientByName
==================
*/
int FindClientByName(char *name) {
	int i;
	char buf[MAX_INFO_STRING];

	for (i = 0; i < level.maxclients; i++) {
		ClientName(i, buf, sizeof(buf));
		if (Q_strequal(buf, name)) return i;
	}
	for (i = 0; i < level.maxclients; i++) {
		ClientName(i, buf, sizeof(buf));
		if (stristr(buf, name)) return i;
	}
	return -1;
}

/*
==================
FindEnemyByName
==================
*/
int FindEnemyByName(bot_state_t *bs, char *name) {
	int i;
	char buf[MAX_INFO_STRING];

	for (i = 0; i < level.maxclients; i++) {
		if (BotSameTeam(bs, i)) continue;
		ClientName(i, buf, sizeof(buf));
		if (Q_strequal(buf, name)) return i;
	}
	for (i = 0; i < level.maxclients; i++) {
		if (BotSameTeam(bs, i)) continue;
		ClientName(i, buf, sizeof(buf));
		if (stristr(buf, name)) return i;
	}
	return -1;
}

/*
==================
NumPlayersOnSameTeam
==================
*/
int NumPlayersOnSameTeam(bot_state_t *bs) {
	int i, num;
	char buf[MAX_INFO_STRING];

	num = 0;
	for (i = 0; i < level.maxclients; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, MAX_INFO_STRING);
		if (strlen(buf)) {
			if (BotSameTeam(bs, i+1)) num++;
		}
	}
	return num;
}

/*
==================
TeamPlayIsOn
==================
*/
int BotGetPatrolWaypoints(bot_state_t *bs, bot_match_t *match) {
	char keyarea[MAX_MESSAGE_SIZE];
	int patrolflags;
	bot_waypoint_t *wp, *newwp, *newpatrolpoints;
	bot_match_t keyareamatch;
	bot_goal_t goal;

	newpatrolpoints = NULL;
	patrolflags = 0;
	//
	trap_BotMatchVariable(match, KEYAREA, keyarea, MAX_MESSAGE_SIZE);
	//
	while(1) {
		if (!trap_BotFindMatch(keyarea, &keyareamatch, MTCONTEXT_PATROLKEYAREA)) {
                            trap_EA_SayTeam(bs->client, "what do you say?");
			BotFreeWaypoints(newpatrolpoints);
			bs->patrolpoints = NULL;
			return qfalse;
		}
		trap_BotMatchVariable(&keyareamatch, KEYAREA, keyarea, MAX_MESSAGE_SIZE);
		if (!BotGetMessageTeamGoal(bs, keyarea, &goal)) {
			//BotAI_BotInitialChat(bs, "cannotfind", keyarea, NULL);
			//trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
			BotFreeWaypoints(newpatrolpoints);
			bs->patrolpoints = NULL;
			return qfalse;
		}
		//create a new waypoint
		newwp = BotCreateWayPoint(keyarea, goal.origin, goal.areanum);
		if (!newwp)
			break;
		//add the waypoint to the patrol points
		newwp->next = NULL;
		for (wp = newpatrolpoints; wp && wp->next; wp = wp->next);
		if (!wp) {
			newpatrolpoints = newwp;
			newwp->prev = NULL;
		}
		else {
			wp->next = newwp;
			newwp->prev = wp;
		}
		//
		if (keyareamatch.subtype & ST_BACK) {
			patrolflags = PATROL_LOOP;
			break;
		}
		else if (keyareamatch.subtype & ST_REVERSE) {
			patrolflags = PATROL_REVERSE;
			break;
		}
		else if (keyareamatch.subtype & ST_MORE) {
			trap_BotMatchVariable(&keyareamatch, MORE, keyarea, MAX_MESSAGE_SIZE);
		}
		else {
			break;
		}
	}
	//
	if (!newpatrolpoints || !newpatrolpoints->next) {
		trap_EA_SayTeam(bs->client, "I need more key points to patrol\n");
		BotFreeWaypoints(newpatrolpoints);
		newpatrolpoints = NULL;
		return qfalse;
	}
	//
	BotFreeWaypoints(bs->patrolpoints);
	bs->patrolpoints = newpatrolpoints;
	//
	bs->curpatrolpoint = bs->patrolpoints;
	bs->patrolflags = patrolflags;
	//
	return qtrue;
}

/*
==================
BotAddressedToBot
==================
*/
int BotAddressedToBot(bot_state_t *bs, bot_match_t *match) {
	char addressedto[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	char name[MAX_MESSAGE_SIZE];
	char botname[128];
	int client;
	bot_match_t addresseematch;

	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = ClientOnSameTeamFromName(bs, netname);
	if (client < 0) return qfalse;
	//if the message is addressed to someone
	if (match->subtype & ST_ADDRESSED) {
		trap_BotMatchVariable(match, ADDRESSEE, addressedto, sizeof(addressedto));
		//the name of this bot
		ClientName(bs->client, botname, 128);
		//
		while(trap_BotFindMatch(addressedto, &addresseematch, MTCONTEXT_ADDRESSEE)) {
			if (addresseematch.type == MSG_EVERYONE) {
				return qtrue;
			}
			else if (addresseematch.type == MSG_MULTIPLENAMES) {
				trap_BotMatchVariable(&addresseematch, TEAMMATE, name, sizeof(name));
				if (strlen(name)) {
					if (stristr(botname, name)) return qtrue;
					if (stristr(bs->subteam, name)) return qtrue;
				}
				trap_BotMatchVariable(&addresseematch, MORE, addressedto, MAX_MESSAGE_SIZE);
			}
			else {
				trap_BotMatchVariable(&addresseematch, TEAMMATE, name, MAX_MESSAGE_SIZE);
				if (strlen(name)) {
					if (stristr(botname, name)) return qtrue;
					if (stristr(bs->subteam, name)) return qtrue;
				}
				break;
			}
		}
		//Com_sprintf(buf, sizeof(buf), "not addressed to me but %s", addressedto);
		//trap_EA_Say(bs->client, buf);
		return qfalse;
	}
	else {
		bot_match_t tellmatch;

		tellmatch.type = 0;
		//if this message wasn't directed solely to this bot
		if (!trap_BotFindMatch(match->string, &tellmatch, MTCONTEXT_REPLYCHAT) ||
				tellmatch.type != MSG_CHATTELL) {
			//make sure not everyone reacts to this message
			if (random() > (float ) 1.0 / (NumPlayersOnSameTeam(bs)-1)) return qfalse;
		}
	}
	return qtrue;
}

/*
==================
BotGPSToPosition
==================
*/
int BotGPSToPosition(char *buf, vec3_t position) {
	int i, j = 0;
	int num, sign;

	for (i = 0; i < 3; i++) {
		num = 0;
		while(buf[j] == ' ') j++;
		if (buf[j] == '-') {
			j++;
			sign = -1;
		}
		else {
			sign = 1;
		}
		while (buf[j]) {
			if (buf[j] >= '0' && buf[j] <= '9') {
				num = num * 10 + buf[j] - '0';
				j++;
			}
			else {
				j++;
				break;
			}
		}
		BotAI_Print(PRT_MESSAGE, "%d\n", sign * num);
		position[i] = (float) sign * num;
	}
	return qtrue;
}

/*
==================
BotGetDominationPoint
Returns the actual Domination point the bot is acting on. Prevents currentPoint from being accessed directly.
==================
*/
int BotGetDominationPoint(bot_state_t *bs) {
	return bs->currentPoint;
}

/*
==================
BotSetDominationPoint
Selects a point for Domination that the bot's team doesn't own. It's saved in the field currentPoint.
==================
*/
void BotSetDominationPoint(bot_state_t *bs, int controlPoint) {
	int i;
	qboolean allPointsOwnedByOurTeam = qtrue;

	// Domination only
	if (gametype != GT_DOMINATION) return;
	// If there are no points, just assign the neutral one.
	if (!BotAreThereDOMPointsInTheMap()) {
		bs->currentPoint = 0;
		return;
	}
	// If a control point has been passed, and falls inside
	// of both gametype AND map limits, assign it.
	if (controlPoint >= 0 &&
			controlPoint < level.domination_points_count &&
			controlPoint < MAX_DOMINATION_POINTS) {
		bs->currentPoint = controlPoint;
		return;
	}
	// Search for points our team don't own.
	for (i=1;i<level.domination_points_count;i++) {
		if (!BotTeamOwnsControlPoint(bs,level.pointStatusDom[i])) {
			allPointsOwnedByOurTeam = qfalse;
			bs->currentPoint = i;
			break;
		}
	}
	// If we own all of the points, operate in a random one.
	if (allPointsOwnedByOurTeam == qtrue) {
		bs->currentPoint = rand() % level.domination_points_count;
	}
	return;
}

/*
==================
BotSetOwnDominationPoint
Selects a point for Domination that the bot's team owns. It's saved in the field currentPoint.
==================
*/
void BotSetOwnDominationPoint(bot_state_t *bs) {
	int i;
	qboolean noPointOwnedByOurTeam = qtrue;

	// Domination only
	if (gametype != GT_DOMINATION) return;
	// If there are no points, just assign the neutral one.
	if (!BotAreThereDOMPointsInTheMap()) {
		bs->currentPoint = 0;
		return;
	}
	// Search for points our team already owns.
	for (i=1;i<level.domination_points_count;i++) {
		if (BotTeamOwnsControlPoint(bs,level.pointStatusDom[i])) {
			noPointOwnedByOurTeam = qfalse;
			bs->currentPoint = i;
			break;
		}
	}
	// If we don't own any of the points, operate in a random one.
	if (noPointOwnedByOurTeam == qtrue) {
		bs->currentPoint = rand() % level.domination_points_count;
	}
	return;
}

/*
==================
BotMatch_TaskPreference
==================
*/
void BotMatch_TaskPreference(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_NETNAME];
	char teammatename[MAX_MESSAGE_SIZE];
	int teammate, preference;

	ClientName(bs->client, netname, sizeof(netname));
	if ( !Q_strequal(netname, bs->teamleader)) {
		return;
	}

	trap_BotMatchVariable(match, NETNAME, teammatename, sizeof(teammatename));
	teammate = ClientFromName(teammatename);
	if (teammate < 0) return;

	preference = BotGetTeamMateTaskPreference(bs, teammate);
	switch(match->subtype)
	{
		case ST_DEFENDER:
		{
			preference &= ~TEAMTP_ATTACKER;
			preference |= TEAMTP_DEFENDER;
			break;
		}
		case ST_ATTACKER:
		{
			preference &= ~TEAMTP_DEFENDER;
			preference |= TEAMTP_ATTACKER;
			break;
		}
		case ST_ROAMER:
		{
			preference &= ~(TEAMTP_ATTACKER|TEAMTP_DEFENDER);
			break;
		}
	}
	BotSetTeamMateTaskPreference(bs, teammate, preference);
	//
	EasyClientName(teammate, teammatename, sizeof(teammatename));
	BotAI_BotInitialChat(bs, "keepinmind", teammatename, NULL);
	trap_BotEnterChat(bs->cs, teammate, CHAT_TELL);
	BotVoiceChatOnly(bs, teammate, VOICECHAT_YES);
	trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
}

/*
==================
BotMatch_JoinSubteam
==================
*/
void BotMatch_JoinSubteam(bot_state_t *bs, bot_match_t *match) {
	char teammate[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	int client;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//get the sub team name
	trap_BotMatchVariable(match, TEAMNAME, teammate, sizeof(teammate));
	//set the sub team name
	Q_strncpyz(bs->subteam, teammate, 32);
	//
	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	BotAI_BotInitialChat(bs, "joinedteam", teammate, NULL);
	client = ClientFromName(netname);
	trap_BotEnterChat(bs->cs, client, CHAT_TELL);
}

/*
==================
BotMatch_LeaveSubteam
==================
*/
void BotMatch_LeaveSubteam(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];
	int client;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	if (strlen(bs->subteam))
	{
		BotAI_BotInitialChat(bs, "leftteam", bs->subteam, NULL);
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = ClientFromName(netname);
		trap_BotEnterChat(bs->cs, client, CHAT_TELL);
	} //end if
	strcpy(bs->subteam, "");
}

/*
==================
BotMatch_WhichTeam
==================
*/
void BotMatch_WhichTeam(bot_state_t *bs, bot_match_t *match) {
	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;

	//
	if (strlen(bs->subteam)) {
		BotAI_BotInitialChat(bs, "inteam", bs->subteam, NULL);
	}
	else {
		BotAI_BotInitialChat(bs, "noteam", NULL);
	}
	trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
}

/*
==================
BotMatch_CheckPoint
==================
*/
void BotMatch_CheckPoint(bot_state_t *bs, bot_match_t *match) {
	int areanum, client;
	char buf[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	vec3_t position;
	bot_waypoint_t *cp;

	if (!G_IsATeamGametype(gametype)) return;
	//
	trap_BotMatchVariable(match, POSITION, buf, MAX_MESSAGE_SIZE);
	VectorClear(position);
	//
	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = ClientFromName(netname);
	//BotGPSToPosition(buf, position);
	sscanf(buf, "%f %f %f", &position[0], &position[1], &position[2]);
	position[2] += 0.5;
	areanum = BotPointAreaNum(position);
	if (!areanum) {
		if (BotAddressedToBot(bs, match)) {
			BotAI_BotInitialChat(bs, "checkpoint_invalid", NULL);
			trap_BotEnterChat(bs->cs, client, CHAT_TELL);
		}
		return;
	}
	//
	trap_BotMatchVariable(match, NAME, buf, MAX_MESSAGE_SIZE);
	//check if there already exists a checkpoint with this name
	cp = BotFindWayPoint(bs->checkpoints, buf);
	if (cp) {
		if (cp->next) cp->next->prev = cp->prev;
		if (cp->prev) cp->prev->next = cp->next;
		else bs->checkpoints = cp->next;
		cp->inuse = qfalse;
	}
	//create a new check point
	cp = BotCreateWayPoint(buf, position, areanum);
	//add the check point to the bot's known chech points
	cp->next = bs->checkpoints;
	if (bs->checkpoints) bs->checkpoints->prev = cp;
	bs->checkpoints = cp;
	//
	if (BotAddressedToBot(bs, match)) {
		Com_sprintf(buf, sizeof(buf), "%1.0f %1.0f %1.0f", cp->goal.origin[0],
													cp->goal.origin[1],
													cp->goal.origin[2]);

		BotAI_BotInitialChat(bs, "checkpoint_confirm", cp->name, buf, NULL);
		trap_BotEnterChat(bs->cs, client, CHAT_TELL);
	}
}

/*
==================
BotMatch_FormationSpace
==================
*/
void BotMatch_FormationSpace(bot_state_t *bs, bot_match_t *match) {
	char buf[MAX_MESSAGE_SIZE];
	float space;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	trap_BotMatchVariable(match, NUMBER, buf, MAX_MESSAGE_SIZE);
	//if it's the distance in feet
	if (match->subtype & ST_FEET) space = 0.3048 * 32 * atof(buf);
	//else it's in meters
	else space = 32 * atof(buf);
	//check if the formation intervening space is valid
	if (space < 48 || space > 500) space = 100;
	bs->formation_dist = space;
}

/*
==================
BotMatch_Dismiss
==================
*/
void BotMatch_Dismiss(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];
	int client;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = ClientFromName(netname);
	//
	bs->decisionmaker = client;
	//
	bs->ltgtype = 0;
	bs->lead_time = 0;
	bs->lastgoal_ltgtype = 0;
	//
	BotAI_BotInitialChat(bs, "dismissed", NULL);
	trap_BotEnterChat(bs->cs, client, CHAT_TELL);
}

/*
==================
BotMatch_Suicide
==================
*/
void BotMatch_Suicide(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];
	int client;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	trap_EA_Command(bs->client, "kill");
	//
	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = ClientFromName(netname);
	//
	BotVoiceChat(bs, client, VOICECHAT_TAUNT);
	trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
}

/*
==================
BotMatch_StartTeamLeaderShip
==================
*/
void BotMatch_StartTeamLeaderShip(bot_state_t *bs, bot_match_t *match) {
	int client;
	char teammate[MAX_MESSAGE_SIZE];

	if (!G_IsATeamGametype(gametype)) return;
	//if chats for him or herself
	if (match->subtype & ST_I) {
		//get the team mate that will be the team leader
		trap_BotMatchVariable(match, NETNAME, teammate, sizeof(teammate));
		Q_strncpyz(bs->teamleader, teammate, sizeof(bs->teamleader));
	}
	//chats for someone else
	else {
		//get the team mate that will be the team leader
		trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
		client = FindClientByName(teammate);
		if (client >= 0) ClientName(client, bs->teamleader, sizeof(bs->teamleader));
	}
}

/*
==================
BotMatch_StopTeamLeaderShip
==================
*/
void BotMatch_StopTeamLeaderShip(bot_state_t *bs, bot_match_t *match) {
	int client;
	char teammate[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];

	if (!G_IsATeamGametype(gametype)) return;
	//get the team mate that stops being the team leader
	trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
	//if chats for him or herself
	if (match->subtype & ST_I) {
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = FindClientByName(netname);
	}
	//chats for someone else
	else {
		client = FindClientByName(teammate);
	} //end else
	if (client >= 0) {
		if (Q_strequal(bs->teamleader, ClientName(client, netname, sizeof(netname)))) {
			bs->teamleader[0] = '\0';
			notleader[client] = qtrue;
		}
	}
}

/*
==================
BotMatch_WhoIsTeamLeader
==================
*/
void BotMatch_WhoIsTeamLeader(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];

	if (!G_IsATeamGametype(gametype)) return;

	ClientName(bs->client, netname, sizeof(netname));
	//if this bot IS the team leader
	if (Q_strequal(netname, bs->teamleader)) {
		trap_EA_SayTeam(bs->client, "I'm the team leader\n");
	}
}

/*
==================
BotMatch_WhatAreYouDoing
==================
*/
void BotMatch_WhatAreYouDoing(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_MESSAGE_SIZE];
	char goalname[MAX_MESSAGE_SIZE];
	int client;

	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//
	switch(bs->ltgtype) {
		case LTG_TEAMHELP:
		{
			EasyClientName(bs->teammate, netname, sizeof(netname));
			BotAI_BotInitialChat(bs, "helping", netname, NULL);
			break;
		}
		case LTG_TEAMACCOMPANY:
		{
			EasyClientName(bs->teammate, netname, sizeof(netname));
			BotAI_BotInitialChat(bs, "accompanying", netname, NULL);
			break;
		}
		case LTG_DEFENDKEYAREA:
		{
			trap_BotGoalName(bs->teamgoal.number, goalname, sizeof(goalname));
			BotAI_BotInitialChat(bs, "defending", goalname, NULL);
			break;
		}
		case LTG_GETITEM:
		{
			trap_BotGoalName(bs->teamgoal.number, goalname, sizeof(goalname));
			BotAI_BotInitialChat(bs, "gettingitem", goalname, NULL);
			break;
		}
		case LTG_KILL:
		{
			ClientName(bs->teamgoal.entitynum, netname, sizeof(netname));
			BotAI_BotInitialChat(bs, "killing", netname, NULL);
			break;
		}
		case LTG_CAMP:
		case LTG_CAMPORDER:
		{
			BotAI_BotInitialChat(bs, "camping", NULL);
			break;
		}
		case LTG_PATROL:
		{
			BotAI_BotInitialChat(bs, "patrolling", NULL);
			break;
		}
		case LTG_GETFLAG:
		{
			BotAI_BotInitialChat(bs, "capturingflag", NULL);
			break;
		}
		case LTG_RUSHBASE:
		{
			BotAI_BotInitialChat(bs, "rushingbase", NULL);
			break;
		}
		case LTG_RETURNFLAG:
		{
			BotAI_BotInitialChat(bs, "returningflag", NULL);
			break;
		}
		case LTG_ATTACKENEMYBASE:
		{
			BotAI_BotInitialChat(bs, "attackingenemybase", NULL);
			break;
		}
		case LTG_HARVEST:
		{
			BotAI_BotInitialChat(bs, "harvesting", NULL);
			break;
		}
//#endif
		case LTG_HOLDPOINTA:
		{
			BotAI_BotInitialChat(bs, "dd_holdingpointa", NULL);
			break;
		}
		case LTG_HOLDPOINTB:
		{
			BotAI_BotInitialChat(bs, "dd_holdingpointb", NULL);
			break;
		}
		case LTG_HOLDDOMPOINT:
		{
			BotAI_BotInitialChat(bs, "dom_holdpoint", NULL);
			break;
		}
		default:
		{
			BotAI_BotInitialChat(bs, "roaming", NULL);
			break;
		}
	}
	//chat what the bot is doing
	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = ClientFromName(netname);
	trap_BotEnterChat(bs->cs, client, CHAT_TELL);
}

/*
==================
BotMatch_WhatIsMyCommand
==================
*/
void BotMatch_WhatIsMyCommand(bot_state_t *bs, bot_match_t *match) {
	char netname[MAX_NETNAME];

	ClientName(bs->client, netname, sizeof(netname));
	if ( !Q_strequal(netname, bs->teamleader) ) {
		return;
	}
	bs->forceorders = qtrue;
}

/*
==================
BotNearestVisibleItem
==================
*/
float BotNearestVisibleItem(bot_state_t *bs, char *itemname, bot_goal_t *goal) {
	int i;
	char name[64];
	bot_goal_t tmpgoal;
	float dist, bestdist;
	vec3_t dir;
	bsp_trace_t trace;

	bestdist = 999999;
	i = -1;
	do {
		i = trap_BotGetLevelItemGoal(i, itemname, &tmpgoal);
		trap_BotGoalName(tmpgoal.number, name, sizeof(name));
		if ( !Q_strequal(itemname, name) ) {
			continue;
		}
		VectorSubtract(tmpgoal.origin, bs->origin, dir);
		dist = VectorLength(dir);
		if (dist < bestdist) {
			//trace from start to end
			BotAI_Trace(&trace, bs->eye, NULL, NULL, tmpgoal.origin, bs->client, CONTENTS_SOLID|CONTENTS_PLAYERCLIP);
			if (trace.fraction >= 1.0) {
				bestdist = dist;
				memcpy(goal, &tmpgoal, sizeof(bot_goal_t));
			}
		}
	} while(i > 0);
	return bestdist;
}

/*
==================
BotMatch_WhereAreYou
==================
*/
void BotMatch_WhereAreYou(bot_state_t *bs, bot_match_t *match) {
	float dist, bestdist;
	int i, bestitem, redtt, bluett, client;
	bot_goal_t goal;
	char netname[MAX_MESSAGE_SIZE];
	char *nearbyitems[] = {
		"Shotgun",
		"Grenade Launcher",
		"Rocket Launcher",
		"Plasmagun",
		"Railgun",
		"Lightning Gun",
		"BFG10K",
		"Quad Damage",
		"Regeneration",
		"Battle Suit",
		"Speed",
		"Invisibility",
		"Flight",
		"Armor",
		"Heavy Armor",
		"Red Flag",
		"Blue Flag",
		"Nailgun",
		"Prox Launcher",
		"Chaingun",
		"Scout",
		"Guard",
		"Doubler",
		"Ammo Regen",
		"Neutral Flag",
		"Red Obelisk",
		"Blue Obelisk",
		"Neutral Obelisk",
		NULL
	};
	//
	if (!G_IsATeamGametype(gametype))
		return;

	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match))
		return;

	bestitem = -1;
	bestdist = 999999;
	for (i = 0; nearbyitems[i]; i++) {
		dist = BotNearestVisibleItem(bs, nearbyitems[i], &goal);
		if (dist < bestdist) {
			bestdist = dist;
			bestitem = i;
		}
	}
	if (bestitem != -1) {
		if (G_UsesTeamFlags(gametype)) {
			redtt = trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, ctf_redflag.areanum, TFL_DEFAULT);
			bluett = trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, ctf_blueflag.areanum, TFL_DEFAULT);
			if (redtt < (redtt + bluett) * 0.4) {
				BotAI_BotInitialChat(bs, "teamlocation", nearbyitems[bestitem], "red", NULL);
			}
			else if (bluett < (redtt + bluett) * 0.4) {
				BotAI_BotInitialChat(bs, "teamlocation", nearbyitems[bestitem], "blue", NULL);
			}
			else {
				BotAI_BotInitialChat(bs, "location", nearbyitems[bestitem], NULL);
			}
		}
		else if (gametype == GT_HARVESTER || gametype == GT_OBELISK) {
			redtt = trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, redobelisk.areanum, TFL_DEFAULT);
			bluett = trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, blueobelisk.areanum, TFL_DEFAULT);
			if (redtt < (redtt + bluett) * 0.4) {
				BotAI_BotInitialChat(bs, "teamlocation", nearbyitems[bestitem], "red", NULL);
			}
			else if (bluett < (redtt + bluett) * 0.4) {
				BotAI_BotInitialChat(bs, "teamlocation", nearbyitems[bestitem], "blue", NULL);
			}
			else {
				BotAI_BotInitialChat(bs, "location", nearbyitems[bestitem], NULL);
			}
		}
		else {
			BotAI_BotInitialChat(bs, "location", nearbyitems[bestitem], NULL);
		}
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = ClientFromName(netname);
		trap_BotEnterChat(bs->cs, client, CHAT_TELL);
	}
}

/*
==================
BotMatch_LeadTheWay
==================
*/
void BotMatch_LeadTheWay(bot_state_t *bs, bot_match_t *match) {
	aas_entityinfo_t entinfo;
	char netname[MAX_MESSAGE_SIZE], teammate[MAX_MESSAGE_SIZE];
	int client, areanum, other;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	//if someone asks for someone else
	if (match->subtype & ST_SOMEONE) {
		//get the team mate name
		trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
		client = FindClientByName(teammate);
		//if this is the bot self
		if (client == bs->client) {
			other = qfalse;
		}
		else if (!BotSameTeam(bs, client)) {
			//FIXME: say "I don't help the enemy"
			return;
		}
		else {
			other = qtrue;
		}
	}
	else {
		//get the netname
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = ClientFromName(netname);
		other = qfalse;
	}
	//if the bot doesn't know who to help (FindClientByName returned -1)
	if (client < 0) {
		BotAI_BotInitialChat(bs, "whois", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	//
	bs->lead_teamgoal.entitynum = -1;
	BotEntityInfo(client, &entinfo);
	//if info is valid (in PVS)
	if (entinfo.valid) {
		areanum = BotPointAreaNum(entinfo.origin);
		if (areanum) { // && trap_AAS_AreaReachability(areanum)) {
			bs->lead_teamgoal.entitynum = client;
			bs->lead_teamgoal.areanum = areanum;
			VectorCopy(entinfo.origin, bs->lead_teamgoal.origin);
			VectorSet(bs->lead_teamgoal.mins, -8, -8, -8);
			VectorSet(bs->lead_teamgoal.maxs, 8, 8, 8);
		}
	}

	if (bs->teamgoal.entitynum < 0) {
		if (other) BotAI_BotInitialChat(bs, "whereis", teammate, NULL);
		else BotAI_BotInitialChat(bs, "whereareyou", netname, NULL);
		trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
		return;
	}
	bs->lead_teammate = client;
	bs->lead_time = FloatTime() + TEAM_LEAD_TIME;
	bs->leadvisible_time = 0;
	bs->leadmessage_time = -(FloatTime() + 2 * random());
}

/*
==================
BotMatch_Kill
==================
*/
void BotMatch_Kill(bot_state_t *bs, bot_match_t *match) {
	char enemy[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	int client;

	if (!G_IsATeamGametype(gametype)) return;
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;

	trap_BotMatchVariable(match, ENEMY, enemy, sizeof(enemy));
	//
	client = FindEnemyByName(bs, enemy);
	if (client < 0) {
		BotAI_BotInitialChat(bs, "whois", enemy, NULL);
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		client = ClientFromName(netname);
		trap_BotEnterChat(bs->cs, client, CHAT_TELL);
		return;
	}
	bs->teamgoal.entitynum = client;
	//set the time to send a message to the team mates
	bs->teammessage_time = FloatTime() + 2 * random();
	//set the ltg type
	bs->ltgtype = LTG_KILL;
	//set the team goal time
	bs->teamgoal_time = FloatTime() + TEAM_KILL_SOMEONE;
	//
	BotSetTeamStatus(bs);
	// Actual outputting of the message requires developer mode.
	if (bot_developer.integer && bot_debugLTG.integer) {
		BotPrintTeamGoal(bs);
	}
}

/*
==================
BotMatch_CTF
==================
*/
void BotMatch_CTF(bot_state_t *bs, bot_match_t *match) {

	char flag[128], netname[MAX_NETNAME];

	if (G_UsesTeamFlags(gametype) && !G_UsesTheWhiteFlag(gametype)) {
		trap_BotMatchVariable(match, FLAG, flag, sizeof(flag));
		if (match->subtype & ST_GOTFLAG) {
			if (Q_strequal(flag, "red")) {
				bs->redflagstatus = 1;
				if (BotTeam(bs) == TEAM_BLUE) {
					trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
					bs->flagcarrier = ClientFromName(netname);
				}
			}
			else {
				bs->blueflagstatus = 1;
				if (BotTeam(bs) == TEAM_RED) {
					trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
					bs->flagcarrier = ClientFromName(netname);
				}
			}
			bs->flagstatuschanged = 1;
			bs->lastflagcapture_time = FloatTime();
		}
		else if (match->subtype & ST_CAPTUREDFLAG) {
			bs->redflagstatus = 0;
			bs->blueflagstatus = 0;
			bs->flagcarrier = 0;
			bs->flagstatuschanged = 1;
		}
		else if (match->subtype & ST_RETURNEDFLAG) {
			if (Q_strequal(flag, "red")) bs->redflagstatus = 0;
			else bs->blueflagstatus = 0;
			bs->flagstatuschanged = 1;
		}
	}
	else if (G_UsesTheWhiteFlag(gametype)) {
		if (match->subtype & ST_1FCTFGOTFLAG) {
			trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
			bs->flagcarrier = ClientFromName(netname);
		}
	}
}

void BotMatch_EnterGame(bot_state_t *bs, bot_match_t *match) {
	int client;
	char netname[MAX_NETNAME];

	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = FindClientByName(netname);
	if (client >= 0) {
		notleader[client] = qfalse;
	}
	if (gametype == GT_DOMINATION) {
		// Assign a control point at start.
		BotSetDominationPoint(bs,-1);
	}
	//NOTE: eliza chats will catch this
	//Com_sprintf(buf, sizeof(buf), "heya %s", netname);
	//EA_Say(bs->client, buf);
}

void BotMatch_NewLeader(bot_state_t *bs, bot_match_t *match) {
	int client;
	char netname[MAX_NETNAME];

	trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
	client = FindClientByName(netname);
	if (!BotSameTeam(bs, client))
		return;
	Q_strncpyz(bs->teamleader, netname, sizeof(bs->teamleader));
}

/*
==================
BotMatchMessage
==================
*/
int BotMatchMessage(bot_state_t *bs, char *message) {
	bot_match_t match;

	match.type = 0;
	//if it is an unknown message
	if (!trap_BotFindMatch(message, &match, MTCONTEXT_MISC
	|MTCONTEXT_INITIALTEAMCHAT
	|MTCONTEXT_CTF
	|MTCONTEXT_DD)) {
		return qfalse;
	}
	//react to the found message
	switch(match.type)
	{
		case MSG_HELP:					//someone calling for help
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_TEAMHELP, TEAM_HELP_TIME);
			break;
		}
		case MSG_ACCOMPANY:				//someone calling for company
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_TEAMACCOMPANY, TEAM_ACCOMPANY_TIME);
			break;
		}
		case MSG_DEFENDKEYAREA:			//teamplay defend a key area
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_DEFENDKEYAREA, TEAM_DEFENDKEYAREA_TIME);
			break;
		}
		case MSG_CAMP:					//camp somewhere
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_CAMP, TEAM_CAMP_TIME);
			break;
		}
		case MSG_PATROL:				//patrol between several key areas
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_PATROL, TEAM_PATROL_TIME);
			break;
		}
		//CTF & 1FCTF
		case MSG_GETFLAG:				//ctf get the enemy flag
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_GETFLAG, CTF_GETFLAG_TIME);
			break;
		}
		//CTF & 1FCTF & Obelisk & Harvester
		case MSG_ATTACKENEMYBASE:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_ATTACKENEMYBASE, TEAM_ATTACKENEMYBASE_TIME);
			break;
		}
		//Harvester
		case MSG_HARVEST:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_HARVEST, TEAM_HARVEST_TIME);
			break;
		}
		//CTF & 1FCTF & Harvester
		case MSG_RUSHBASE:				//ctf rush to the base
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_RUSHBASE, CTF_RUSHBASE_TIME);
			break;
		}
		//CTF & 1FCTF
		case MSG_RETURNFLAG:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_RETURNFLAG, CTF_RETURNFLAG_TIME);
			break;
		}
		case MSG_GETITEM:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_GETITEM, TEAM_GETITEM_TIME);
			break;
		}
		case MSG_HOLDPOINTA:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_HOLDPOINTA, TEAM_HOLDPOINTA_TIME);
			break;
		}
		case MSG_HOLDPOINTB:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_HOLDPOINTB, TEAM_HOLDPOINTB_TIME);
			break;
		}
		case MSG_HOLDDOMPOINT:
		{
			BotMatch_AcknowledgeOrder(bs, &match, LTG_HOLDDOMPOINT, TEAM_HOLDDOMPOINT_TIME);
			break;
		}
		//CTF & 1FCTF & Obelisk & Harvester
		case MSG_TASKPREFERENCE:
		{
			BotMatch_TaskPreference(bs, &match);
			break;
		}
		//CTF & 1FCTF
		case MSG_CTF:
		{
			BotMatch_CTF(bs, &match);
			break;
		}
		case MSG_JOINSUBTEAM:			//join a sub team
		{
			BotMatch_JoinSubteam(bs, &match);
			break;
		}
		case MSG_LEAVESUBTEAM:			//leave a sub team
		{
			BotMatch_LeaveSubteam(bs, &match);
			break;
		}
		case MSG_WHICHTEAM:
		{
			BotMatch_WhichTeam(bs, &match);
			break;
		}
		case MSG_CHECKPOINT:			//remember a check point
		{
			BotMatch_CheckPoint(bs, &match);
			break;
		}
		case MSG_CREATENEWFORMATION:	//start the creation of a new formation
		{
			trap_EA_SayTeam(bs->client, "the part of my brain to create formations has been damaged");
			break;
		}
		case MSG_FORMATIONPOSITION:		//tell someone his/her position in the formation
		{
			trap_EA_SayTeam(bs->client, "the part of my brain to create formations has been damaged");
			break;
		}
		case MSG_FORMATIONSPACE:		//set the formation space
		{
			BotMatch_FormationSpace(bs, &match);
			break;
		}
		case MSG_DOFORMATION:			//form a certain formation
		{
			break;
		}
		case MSG_DISMISS:				//dismiss someone
		{
			BotMatch_Dismiss(bs, &match);
			break;
		}
		case MSG_STARTTEAMLEADERSHIP:	//someone will become the team leader
		{
			BotMatch_StartTeamLeaderShip(bs, &match);
			break;
		}
		case MSG_STOPTEAMLEADERSHIP:	//someone will stop being the team leader
		{
			BotMatch_StopTeamLeaderShip(bs, &match);
			break;
		}
		case MSG_WHOISTEAMLAEDER:
		{
			BotMatch_WhoIsTeamLeader(bs, &match);
			break;
		}
		case MSG_WHATAREYOUDOING:		//ask a bot what he/she is doing
		{
			BotMatch_WhatAreYouDoing(bs, &match);
			break;
		}
		case MSG_WHATISMYCOMMAND:
		{
			BotMatch_WhatIsMyCommand(bs, &match);
			break;
		}
		case MSG_WHEREAREYOU:
		{
			BotMatch_WhereAreYou(bs, &match);
			break;
		}
		case MSG_LEADTHEWAY:
		{
			BotMatch_LeadTheWay(bs, &match);
			break;
		}
		case MSG_KILL:
		{
			BotMatch_Kill(bs, &match);
			break;
		}
		case MSG_ENTERGAME:				//someone entered the game
		{
			BotMatch_EnterGame(bs, &match);
			break;
		}
		case MSG_NEWLEADER:
		{
			BotMatch_NewLeader(bs, &match);
			break;
		}
		case MSG_WAIT:
		{
			break;
		}
		case MSG_SUICIDE:
		{
			BotMatch_Suicide(bs, &match);
			break;
		}
		default:
		{
			BotAI_Print(PRT_MESSAGE, "unknown match type\n");
			break;
		}
	}
	return qtrue;
}

/*
==================
BotMatch_AcknowledgeOrder

General purpose handling of bot orders. Done this way since previously there were many different functions that did mostly the same thing.
It has A LOT OF ROOM for optimization.
==================
*/
void BotMatch_AcknowledgeOrder(bot_state_t *bs, bot_match_t *match, int ltgType, float teamGoalTime) {
	char teammate[MAX_MESSAGE_SIZE];
	char itemname[MAX_MESSAGE_SIZE];
	char netname[MAX_MESSAGE_SIZE];
	int client, other, areanum, ltgTypeInternal;
	aas_entityinfo_t entinfo;
	bot_match_t teammatematch;

	// Orders can only be given in team-based modes.
	if (!G_IsATeamGametype(gametype)) {
		return;
	}
	// Gametype order restriction zone.
	// We need to set a LTG. Where a bot cannot accomplish an order, patrols.
	ltgTypeInternal = LTG_PATROL;
	switch (ltgType) {
		case LTG_DEFENDKEYAREA: {
			// in eCTF AvD mode, only defenders defend. Attackers patrol.
			if (gametype == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer && !BotIsOnAttackingTeam(bs)) {
				ltgTypeInternal = LTG_DEFENDKEYAREA;
			}
			// In CTF, 1FCTF and regular eCTF, we need a flag to defend.
			else if (G_UsesTeamFlags(gametype) && BotIsThereOurFlagInTheMap(bs)) {
				ltgTypeInternal = LTG_DEFENDKEYAREA;
			}
			// In Harvester and Overload, we need an obelisk to defend.
			else if ((gametype == GT_HARVESTER || gametype == GT_OBELISK) && BotIsThereOurObeliskInTheMap(bs)) {
				ltgTypeInternal = LTG_DEFENDKEYAREA;
			}
			// In Double DOM, we send the bot to defend one of the points we already own.
			else if (gametype == GT_DOUBLE_D) {
				if (BotTeamOwnsControlPoint(bs,level.pointStatusA)) {
					ltgTypeInternal = LTG_HOLDPOINTA;
				}
				else if (BotTeamOwnsControlPoint(bs,level.pointStatusB)) {
					ltgTypeInternal = LTG_HOLDPOINTB;
				}
				// If we don't own points, pick one at random and hold it.
				else {
					if (random() * 10 >= 5) {
						ltgTypeInternal = LTG_HOLDPOINTA;
					}
					else {
						ltgTypeInternal = LTG_HOLDPOINTB;
					}
				}
			}
			// In Domination, we pick a point we own, and hold onto it.
			else if (gametype == GT_DOMINATION) {
				BotSetOwnDominationPoint(bs);
				ltgTypeInternal = LTG_HOLDDOMPOINT;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_CAMP:
		case LTG_CAMPORDER: {
			// in eCTF AvD mode, only defenders can camp.
			if (gametype == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer && !BotIsOnAttackingTeam(bs)) {
				if (trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CAMPER, 0, 1) > random()) {
					ltgTypeInternal = LTG_CAMP;
				}
				else {
					ltgTypeInternal = LTG_DEFENDKEYAREA;
				}
			}
			// In Double DOM, we send the bot to defend one of the points we already own.
			else if (gametype == GT_DOUBLE_D) {
				if (BotTeamOwnsControlPoint(bs,level.pointStatusA)) {
					ltgTypeInternal = LTG_HOLDPOINTA;
				}
				else if (BotTeamOwnsControlPoint(bs,level.pointStatusB)) {
					ltgTypeInternal = LTG_HOLDPOINTB;
				}
				// If we don't own points, pick one at random and hold it.
				else {
					float rnd = random() * 10;
					if (rnd >= 5) {
						ltgTypeInternal = LTG_HOLDPOINTA;
					}
					else {
						ltgTypeInternal = LTG_HOLDPOINTB;
					}
				}
			}
			// In Domination, we pick a point we own, and hold onto it.
			else if (gametype == GT_DOMINATION) {
				BotSetOwnDominationPoint(bs);
				ltgTypeInternal = LTG_HOLDDOMPOINT;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_GETFLAG: {
			// in eCTF AvD mode, only attackers go for the flag.
			if (gametype == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer && BotIsOnAttackingTeam(bs)) {
				ltgTypeInternal = LTG_GETFLAG;
			}
			// In CTF and regular eCTF, we need an enemy flag to attack.
			else if (G_UsesTeamFlags(gametype) && BotIsThereAnEnemyFlagInTheMap(bs)) {
				ltgTypeInternal = LTG_GETFLAG;
			}
			// In 1FCTF, we need the neutral flag to attack.
			else if (G_UsesTheWhiteFlag(gametype) && BotIsThereANeutralFlagInTheMap()) {
				ltgTypeInternal = LTG_GETFLAG;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_ATTACKENEMYBASE: {
			// in eCTF AvD mode, only attackers attack.
			if (gametype == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer && BotIsOnAttackingTeam(bs)) {
				ltgTypeInternal = LTG_GETFLAG;
			}
			// In other flag-based modes, they attack.
			else if (G_UsesTeamFlags(gametype)) {
				ltgTypeInternal = LTG_GETFLAG;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_HARVEST: {
			// We need both an obelisk to attack and (if harvesterFromBodies is disabled) a neutral obelisk.
			if (gametype == GT_HARVESTER && BotIsThereANeutralObeliskInTheMap()) {
				ltgTypeInternal = LTG_HARVEST;
			}
			else if (gametype == GT_HARVESTER && !BotIsThereANeutralObeliskInTheMap() && (g_harvesterFromBodies.integer <= 0)) {
				ltgTypeInternal = LTG_HARVEST;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_RUSHBASE: {
			// in eCTF AvD mode, only attackers go for the flag.
			if (gametype == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer && BotIsOnAttackingTeam(bs)) {
				ltgTypeInternal = LTG_RUSHBASE;
			}
			// In CTF, 1FCTF and regular eCTF, we need an enemy flag to defend.
			else if (G_UsesTeamFlags(gametype) && BotIsThereAnEnemyFlagInTheMap(bs)) {
				ltgTypeInternal = LTG_RUSHBASE;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_RETURNFLAG: {
			// in eCTF AvD mode, only defenders can recover the flag.
			if (gametype == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer && !BotIsOnAttackingTeam(bs) && !BotIsFlagOnOurBase(bs)) {
				ltgTypeInternal = LTG_RETURNFLAG;
			}
			// In CTF and eCTF, if the flag was lost, recover it.
			else if (G_UsesTeamFlags(gametype) && !G_UsesTheWhiteFlag(gametype) && !BotIsFlagOnOurBase(bs)) {
				ltgTypeInternal = LTG_RETURNFLAG;
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_HOLDPOINTA: {
			// In addition to the gamemode, we also need both points.
			if (gametype == GT_DOUBLE_D) {
				if (BotIsThereDDPointAInTheMap() && BotIsThereDDPointBInTheMap()) {
					ltgTypeInternal = LTG_HOLDPOINTA;
				}
				else {
					ltgTypeInternal = LTG_PATROL;
				}
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_HOLDPOINTB: {
			// In addition to the gamemode, we also need both points.
			if (gametype == GT_DOUBLE_D) {
				if (BotIsThereDDPointAInTheMap() && BotIsThereDDPointBInTheMap()) {
					ltgTypeInternal = LTG_HOLDPOINTB;
				}
				else {
					ltgTypeInternal = LTG_PATROL;
				}
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_HOLDDOMPOINT: {
			if (gametype == GT_DOMINATION) {
				// In addition to the gamemode, we also need at least one point.
				if (BotAreThereDOMPointsInTheMap()) {
					BotSetDominationPoint(bs,-1);
					ltgTypeInternal = LTG_HOLDDOMPOINT;
				}
				else {
					ltgTypeInternal = LTG_PATROL;
				}
			}
			else {
				ltgTypeInternal = LTG_PATROL;
			}
			break;
		}
		case LTG_GETITEM: {
			ltgTypeInternal = LTG_GETITEM;
			break;
		}
		case LTG_TEAMACCOMPANY: {
			ltgTypeInternal = LTG_TEAMACCOMPANY;
			break;
		}
		case LTG_TEAMHELP: {
			ltgTypeInternal = LTG_TEAMHELP;
			break;
		}
		case LTG_PATROL:
		default: {
			ltgTypeInternal = LTG_PATROL;
			break;
		}
	}
	//if not addressed to this bot
	if (!BotAddressedToBot(bs, match)) return;
	if (ltgTypeInternal == LTG_CAMP || ltgTypeInternal == LTG_CAMPORDER) {
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		//asked for someone else
		client = FindClientByName(netname);
		//if there's no valid client with this name
		if (client < 0) {
			BotAI_BotInitialChat(bs, "whois", netname, NULL);
			trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
			return;
		}
		//get the match variable
		trap_BotMatchVariable(match, KEYAREA, itemname, sizeof(itemname));
		//in CTF it could be the base
		if (match->subtype & ST_THERE) {
			//camp at the spot the bot is currently standing
			bs->teamgoal.entitynum = bs->entitynum;
			bs->teamgoal.areanum = bs->areanum;
			VectorCopy(bs->origin, bs->teamgoal.origin);
			VectorSet(bs->teamgoal.mins, -8, -8, -8);
			VectorSet(bs->teamgoal.maxs, 8, 8, 8);
		}
		else if (match->subtype & ST_HERE) {
			//if this is the bot self
			if (client == bs->client) return;
			//
			bs->teamgoal.entitynum = -1;
			BotEntityInfo(client, &entinfo);
			//if info is valid (in PVS)
			if (entinfo.valid) {
				areanum = BotPointAreaNum(entinfo.origin);
				if (areanum) {/* && trap_AAS_AreaReachability(areanum)) { */
					//NOTE: just assume the bot knows where the person is
					/* if (BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, client)) { */
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
				BotAI_BotInitialChat(bs, "whereareyou", netname, NULL);
				client = ClientFromName(netname);
				trap_BotEnterChat(bs->cs, client, CHAT_TELL);
				return;
			}
		}
		else if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
			//BotAI_BotInitialChat(bs, "cannotfind", itemname, NULL);
			//client = ClientFromName(netname);
			//trap_BotEnterChat(bs->cs, client, CHAT_TELL);
			return;
		}
	}
	else if (ltgTypeInternal == LTG_TEAMACCOMPANY || ltgTypeInternal == LTG_TEAMHELP) {
		//get the team mate name
		trap_BotMatchVariable(match, TEAMMATE, teammate, sizeof(teammate));
		//get the client to help
		if (trap_BotFindMatch(teammate, &teammatematch, MTCONTEXT_TEAMMATE) &&
				//if someone asks for him or herself
				teammatematch.type == MSG_ME) {
			//get the netname
			trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
			client = ClientFromName(netname);
			other = qfalse;
		}
		else {
			//asked for someone else
			client = FindClientByName(teammate);
			//if this is the bot self
			if (client == bs->client) {
				other = qfalse;
			}
			else if (!BotSameTeam(bs, client)) {
				//FIXME: say "I don't help the enemy"
				return;
			}
			else {
				other = qtrue;
			}
		}
		//if the bot doesn't know who to help (FindClientByName returned -1)
		if (client < 0) {
			if (other) BotAI_BotInitialChat(bs, "whois", teammate, NULL);
			else BotAI_BotInitialChat(bs, "whois", netname, NULL);
			client = ClientFromName(netname);
			trap_BotEnterChat(bs->cs, client, CHAT_TELL);
			return;
		}
		//don't help or accompany yourself
		if (client == bs->client) {
			return;
		}
		//
		bs->teamgoal.entitynum = -1;
		BotEntityInfo(client, &entinfo);
		//if info is valid (in PVS)
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);
			if (areanum) {// && trap_AAS_AreaReachability(areanum)) {
				bs->teamgoal.entitynum = client;
				bs->teamgoal.areanum = areanum;
				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}
		//if no teamgoal yet
		if (bs->teamgoal.entitynum < 0) {
			//if near an item
			if (match->subtype & ST_NEARITEM) {
				//get the match variable
				trap_BotMatchVariable(match, ITEM, itemname, sizeof(itemname));
				//
				if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
					//BotAI_BotInitialChat(bs, "cannotfind", itemname, NULL);
					//trap_BotEnterChat(bs->cs, bs->client, CHAT_TEAM);
					return;
				}
			}
		}
		//
		if (bs->teamgoal.entitynum < 0) {
			if (other) BotAI_BotInitialChat(bs, "whereis", teammate, NULL);
			else BotAI_BotInitialChat(bs, "whereareyou", netname, NULL);
			client = ClientFromName(netname);
			trap_BotEnterChat(bs->cs, client, CHAT_TEAM);
			return;
		}
		//the team mate
		bs->teammate = client;
		//
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		//
		client = ClientFromName(netname);
	}
	else {
		if (ltgTypeInternal == LTG_PATROL) {
			//get the patrol waypoints
			if (!BotGetPatrolWaypoints(bs, match)) return;
		}
		else if (ltgTypeInternal == LTG_DEFENDKEYAREA || ltgTypeInternal == LTG_HOLDDOMPOINT ||
				ltgTypeInternal == LTG_HOLDPOINTA || ltgTypeInternal == LTG_HOLDPOINTB ||
				ltgTypeInternal == LTG_GETITEM) {
			if (!BotGetMessageTeamGoal(bs, itemname, &bs->teamgoal)) {
				return;
			}
		}
		trap_BotMatchVariable(match, NETNAME, netname, sizeof(netname));
		if (ltgTypeInternal == LTG_GETITEM) {
			client = ClientOnSameTeamFromName(bs, netname);
		}
		else if (ltgTypeInternal == LTG_DEFENDKEYAREA || ltgTypeInternal == LTG_HOLDDOMPOINT ||
				ltgTypeInternal == LTG_HOLDPOINTA || ltgTypeInternal == LTG_HOLDPOINTB) {
			client = ClientFromName(netname);
		}
		else {
			client = FindClientByName(netname);
		}
	}

	//the team mate who ordered
	bs->decisionmaker = client;
	bs->ordered = qtrue;
	bs->order_time = FloatTime();

	if (ltgTypeInternal == LTG_TEAMACCOMPANY || ltgTypeInternal == LTG_TEAMHELP) {
		//last time the team mate was assumed visible
		bs->teammatevisible_time = FloatTime();
	}
	//set the time to send a message to the team mates
	bs->teammessage_time = FloatTime() + 2 * random();
	//set the ltg type
	bs->ltgtype = ltgTypeInternal;

	if (ltgTypeInternal == LTG_GETITEM || ltgTypeInternal == LTG_GETFLAG ||
			ltgTypeInternal == LTG_ATTACKENEMYBASE || ltgTypeInternal == LTG_HARVEST) {
		//set the team goal time
		bs->teamgoal_time = FloatTime() + teamGoalTime;
	}
	else {
		//get the team goal time
		bs->teamgoal_time = BotGetTime(match);
		//set the team goal time
		if (!bs->teamgoal_time) bs->teamgoal_time = FloatTime() + teamGoalTime;
	}

	if (ltgTypeInternal == LTG_CAMP || ltgTypeInternal == LTG_CAMPORDER) {
		//not arrived yet
		bs->arrive_time = 0;
	}
	else if (ltgTypeInternal == LTG_GETFLAG) {
		// get an alternate route in ctf
		if (G_UsesTeamFlags(gametype)) {
			//get an alternative route goal towards the enemy base
			BotGetAlternateRouteGoal(bs, BotOppositeTeam(bs));
		}
	}
	else if (ltgTypeInternal == LTG_ATTACKENEMYBASE) {
		bs->attackaway_time = 0;
	}
	else if (ltgTypeInternal == LTG_HARVEST) {
		bs->harvestaway_time = 0;
	}
	else if (ltgTypeInternal == LTG_RUSHBASE || ltgTypeInternal == LTG_RETURNFLAG) {
		bs->rushbaseaway_time = 0;
	}
	else if (ltgTypeInternal == LTG_TEAMACCOMPANY) {
		bs->formation_dist = 3.5 * 32;		//3.5 meter
		bs->arrive_time = 0;
	}
	else {
		//away from defending
		bs->defendaway_time = 0;
	}

	if (ltgTypeInternal != LTG_TEAMHELP) {
		BotSetTeamStatus(bs);
	}

	if (ltgTypeInternal == LTG_DEFENDKEYAREA || ltgTypeInternal == LTG_HOLDDOMPOINT ||
			ltgTypeInternal == LTG_HOLDPOINTA || ltgTypeInternal == LTG_HOLDPOINTB ||
			ltgTypeInternal == LTG_CAMP || ltgTypeInternal == LTG_CAMPORDER ||
			ltgTypeInternal == LTG_PATROL || ltgTypeInternal == LTG_TEAMACCOMPANY) {
		// remember last ordered task
		BotRememberLastOrderedTask(bs);
	}

	// Actual outputting of the message requires developer mode.
	if (bot_developer.integer && bot_debugLTG.integer) {
		BotPrintTeamGoal(bs);
	}
}


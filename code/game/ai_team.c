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
 * name:		ai_team.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /MissionPack/code/game/ai_team.c $
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

#include "match.h"

// for the voice chats
#include "../../ui/menudef.h"

//ctf task preferences for a client
typedef struct bot_ctftaskpreference_s
{
	char		name[36];
	int			preference;
} bot_ctftaskpreference_t;

bot_ctftaskpreference_t ctftaskpreferences[MAX_CLIENTS];


/*
==================
BotValidTeamLeader
==================
*/
int BotValidTeamLeader(bot_state_t *bs) {
	if (!strlen(bs->teamleader)) return qfalse;
	if (ClientFromName(bs->teamleader) == -1) return qfalse;
	return qtrue;
}

/*
==================
BotNumTeamMates
==================
*/
int BotNumTeamMates(bot_state_t *bs) {
	int i, numplayers;
	char buf[MAX_INFO_STRING];
	static int maxclients;

	if (!maxclients)
		maxclients = trap_Cvar_VariableIntegerValue("sv_maxclients");

	numplayers = 0;
	for (i = 0; i < level.maxclients; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "n"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		if (BotSameTeam(bs, i)) {
			numplayers++;
		}
	}
	return numplayers;
}

/*
==================
BotClientTravelTimeToGoal
==================
*/
int BotClientTravelTimeToGoal(int client, bot_goal_t *goal) {
	playerState_t ps;
	int areanum;

	if (BotAI_GetClientState(client, &ps)) {
		areanum = BotPointAreaNum(ps.origin);
	} else {
		areanum = 0;
	}
	if (!areanum) return 1;
	return trap_AAS_AreaTravelTimeToGoalArea(areanum, ps.origin, goal->areanum, TFL_DEFAULT);
}

/*
==================
BotSortTeamMatesByBaseTravelTime
==================
*/
int BotSortTeamMatesByBaseTravelTime(bot_state_t *bs, int *teammates, int maxteammates) {

	int i, j, k, numteammates, traveltime;
	char buf[MAX_INFO_STRING];
	int traveltimes[MAX_CLIENTS];
	bot_goal_t *goal = NULL;

	if (G_UsesTeamFlags(gametype)) {
		if (BotTeam(bs) == TEAM_RED)
			goal = &ctf_redflag;
		else
			goal = &ctf_blueflag;
	}
	else {
		if (BotTeam(bs) == TEAM_RED)
			goal = &redobelisk;
		else
			goal = &blueobelisk;
	}
	numteammates = 0;
	for (i = 0; i < level.maxclients; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "n"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		if (BotSameTeam(bs, i)) {
			//
			traveltime = BotClientTravelTimeToGoal(i, goal);
			//
			for (j = 0; j < numteammates; j++) {
				if (traveltime < traveltimes[j]) {
					for (k = numteammates; k > j; k--) {
						traveltimes[k] = traveltimes[k-1];
						teammates[k] = teammates[k-1];
					}
					break;
				}
			}
			traveltimes[j] = traveltime;
			teammates[j] = i;
			numteammates++;
			if (numteammates >= maxteammates) break;
		}
	}
	return numteammates;
}

/*
==================
BotSortTeamMatesByReletiveTravelTime2ddA
For Double Domination
==================
*/
int BotSortTeamMatesByRelativeTravelTime2ddA(bot_state_t *bs, int *teammates, int maxteammates) {
	int i, j, k, numteammates;
	double traveltime, traveltime2b;
	char buf[MAX_INFO_STRING];
	static int maxclients;
	double traveltimes[MAX_CLIENTS];
	//int traveltimes2b[MAX_CLIENTS];
	bot_goal_t *goalA = &ctf_redflag;
	bot_goal_t *goalB = &ctf_blueflag;

	if (!maxclients)
		maxclients = trap_Cvar_VariableIntegerValue("sv_maxclients");

	numteammates = 0;

	for (i = 0; i < maxclients && i < MAX_CLIENTS; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "n"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		if (BotSameTeam(bs, i)) {
			traveltime = (double)BotClientTravelTimeToGoal(i, goalA);
			traveltime2b = (double)BotClientTravelTimeToGoal(i, goalB);
			traveltime = traveltime/traveltime2b;

			for (j = 0; j < numteammates; j++) {
				if (traveltime < traveltimes[j]) {
					for (k = numteammates; k > j; k--) {
						traveltimes[k] = traveltimes[k-1];
						teammates[k] = teammates[k-1];
					}
					break;
				}
			}
			traveltimes[j] = traveltime;
			teammates[j] = i;
			numteammates++;
			if (numteammates >= maxteammates) break;
		}
	}

	return numteammates;
}

/*
==================
BotSetTeamMateTaskPreference
==================
*/
void BotSetTeamMateTaskPreference(bot_state_t *bs, int teammate, int preference) {
	char teammatename[MAX_NETNAME];

	ctftaskpreferences[teammate].preference = preference;
	ClientName(teammate, teammatename, sizeof(teammatename));
	strcpy(ctftaskpreferences[teammate].name, teammatename);
}

/*
==================
BotGetTeamMateTaskPreference
==================
*/
int BotGetTeamMateTaskPreference(bot_state_t *bs, int teammate) {
	char teammatename[MAX_NETNAME];
	if ( !ctftaskpreferences[teammate].preference) {
		return 0;
	}
	ClientName(teammate, teammatename, sizeof(teammatename));
	if ( !Q_strequal(teammatename, ctftaskpreferences[teammate].name)) {
		return 0;
	}
	return ctftaskpreferences[teammate].preference;
}

/*
==================
BotSortTeamMatesByTaskPreference
==================
*/
int BotSortTeamMatesByTaskPreference(bot_state_t *bs, int *teammates, int numteammates) {
	int defenders[MAX_CLIENTS], numdefenders;
	int attackers[MAX_CLIENTS], numattackers;
	int roamers[MAX_CLIENTS], numroamers;
	int i, preference;

	numdefenders = numattackers = numroamers = 0;
	for (i = 0; i < numteammates; i++) {
		preference = BotGetTeamMateTaskPreference(bs, teammates[i]);
		if (preference & TEAMTP_DEFENDER) {
			defenders[numdefenders++] = teammates[i];
		}
		else if (preference & TEAMTP_ATTACKER) {
			attackers[numattackers++] = teammates[i];
		}
		else {
			roamers[numroamers++] = teammates[i];
		}
	}
	numteammates = 0;
	//defenders at the front of the list
	memcpy(&teammates[numteammates], defenders, numdefenders * sizeof(int));
	numteammates += numdefenders;
	//roamers in the middle
	memcpy(&teammates[numteammates], roamers, numroamers * sizeof(int));
	numteammates += numroamers;
	//attacker in the back of the list
	memcpy(&teammates[numteammates], attackers, numattackers * sizeof(int));
	numteammates += numattackers;

	return numteammates;
}

/*
==================
BotSayTeamOrders
==================
*/
void BotSayTeamOrderAlways(bot_state_t *bs, int toclient) {
	char teamchat[MAX_MESSAGE_SIZE];
	char buf[MAX_MESSAGE_SIZE];
	char name[MAX_NETNAME];

	if (bot_nochat.integer>2) return;

	//if the bot is talking to itself
	if (bs->client == toclient) {
		//don't show the message just put it in the console message queue
		trap_BotGetChatMessage(bs->cs, buf, sizeof(buf));
		ClientName(bs->client, name, sizeof(name));
		Com_sprintf(teamchat, sizeof(teamchat), EC"(%s"EC")"EC": %s", name, buf);
		trap_BotQueueConsoleMessage(bs->cs, CMS_CHAT, teamchat);
	}
	else {
		trap_BotEnterChat(bs->cs, toclient, CHAT_TELL);
	}
}

/*
==================
BotSayTeamOrders
==================
*/
void BotSayTeamOrder(bot_state_t *bs, int toclient) {
#ifdef MISSIONPACK
	// voice chats only
	char buf[MAX_MESSAGE_SIZE];

	trap_BotGetChatMessage(bs->cs, buf, sizeof(buf));
#else
	BotSayTeamOrderAlways(bs, toclient);
#endif
}

/*
==================
BotVoiceChat
==================
*/
void BotVoiceChat(bot_state_t *bs, int toclient, char *voicechat) {
#ifdef MISSIONPACK
	if (toclient == -1) {
		// voice only say team
		trap_EA_Command(bs->client, va("vsay_team %s", voicechat));
	}
	else {
		// voice only tell single player
		trap_EA_Command(bs->client, va("vtell %d %s", toclient, voicechat));
	}
#endif
}

/*
==================
BotVoiceChatOnly
==================
*/
void BotVoiceChatOnly(bot_state_t *bs, int toclient, char *voicechat) {

#ifdef MISSIONPACK
	if (toclient == -1) {
		// voice only say team
		trap_EA_Command(bs->client, va("vosay_team %s", voicechat));
	}
	else {
		// voice only tell single player
		trap_EA_Command(bs->client, va("votell %d %s", toclient, voicechat));
	}
#endif
}

/*
==================
BotSayVoiceTeamOrder
==================
*/
void BotSayVoiceTeamOrder(bot_state_t *bs, int toclient, char *voicechat) {

#ifdef MISSIONPACK
	BotVoiceChat(bs, toclient, voicechat);
#endif
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_BothFlagsNotAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i, other;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME], carriername[MAX_NETNAME];

	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//different orders based on the number of team mates
	switch(bs->numteammates) {
		case 1: break;
		case 2:
		{
			if (teammates[0] != bs->flagcarrier) {
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_GETTHEFLAG, NULL);
			} else {
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
			}
			break;
		}
		case 3:
		{
			//tell the one closest to the base not carrying the flag to accompany the flag carrier
			if (teammates[0] != bs->flagcarrier) other = teammates[0];
			else other = teammates[1];
			if ( bs->flagcarrier != -1 ) {
				if (bs->flagcarrier == bs->client) {
					BotAI_GiveRoleToTheBot(bs, bs->flagcarrier, carriername, ORDER_FOLLOWME, NULL);
				} else {
					BotAI_GiveRoleToTheBot(bs, bs->flagcarrier, name, ORDER_FOLLOWTEAMMATE, carriername);
				}
			}
			else {
				BotAI_GiveRoleToTheBot(bs, other, name, ORDER_GETTHEFLAG, NULL);
			}
			if (teammates[2] != bs->flagcarrier) {
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_RETURNOURFLAG, NULL);
			} else {
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_RETURNOURFLAG, NULL);
			}
			break;
		}
		default:
		{
			defenders = (int) (float) numteammates * 0.4 + 0.5;
			if (defenders > 4) defenders = 4;
			attackers = (int) (float) numteammates * 0.5 + 0.5;
			if (attackers > 5) attackers = 5;
			if (bs->flagcarrier != -1) {
				ClientName(bs->flagcarrier, carriername, sizeof(carriername));
				for (i = 0; i < defenders; i++) {
					//
					if (teammates[i] == bs->flagcarrier) {
						continue;
					}
					//
					if (bs->flagcarrier == bs->client) {
						BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_FOLLOWME, NULL);
					}
					else {
						BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_FOLLOWTEAMMATE, carriername);
					}
				}
			}
			else {
				for (i = 0; i < defenders; i++) {
					//
					if (teammates[i] == bs->flagcarrier) {
						continue;
					}
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_GETTHEFLAG, NULL);
				}
			}
			for (i = 0; i < attackers; i++) {
				//
				if (teammates[numteammates - i - 1] == bs->flagcarrier) {
					continue;
				}
				//
				BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_RETURNOURFLAG, NULL);
			}
			//
			break;
		}
	}
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_FlagNotAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];
	if (bot_nochat.integer>2) return;
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//In oneway ctf we must all move out of the base (only one strategi, maybe we can also send some to the enemy base  to meet the flag carier?)
	//We must be defending
	if(g_elimination_ctf_oneway.integer > 0) {
		for (i = 0; i < numteammates; i++) {
			//
			BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_GETTHEFLAG, NULL);
		}
		return;
	}

	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(bs->numteammates) {
			case 1: break;
			case 2:
			{
				// keep one near the base for when the flag is returned
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//keep one near the base for when the flag is returned
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other two get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				//keep some people near the base for when the flag is returned
				defenders = (int) (float) numteammates * 0.3 + 0.5;
				if (defenders > 3) defenders = 3;
				attackers = (int) (float) numteammates * 0.6 + 0.5;
				if (attackers > 6) attackers = 6;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
	else {
		//different orders based on the number of team mates
		switch(bs->numteammates) {
			case 1: break;
			case 2:
			{
				//both will go for the enemy flag
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//everyone go for the flag
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				//keep some people near the base for when the flag is returned
				defenders = (int) (float) numteammates * 0.2 + 0.5;
				if (defenders > 2) defenders = 2;
				attackers = (int) (float) numteammates * 0.7 + 0.5;
				if (attackers > 7) attackers = 7;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
BotCTFOrders_EnemyFlagNotAtBase
==================
*/
void BotCTFOrders_EnemyFlagNotAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME], carriername[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//different orders based on the number of team mates
	switch(numteammates) {
		case 1: break;
		case 2:
		{
			//tell the one not carrying the flag to defend the base
			if (teammates[0] == bs->flagcarrier) {
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
			} else {
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
			}
			break;
		}
		case 3:
		{
			//tell the one closest to the base not carrying the flag to defend the base
			if (teammates[0] != bs->flagcarrier) {
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
			} else {
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
			}
			//tell the other also to defend the base
			if (teammates[2] != bs->flagcarrier) {
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_DEFENDTHEBASE, NULL);
			} else {
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
			}
			break;
		}
		default:
		{
			//60% will defend the base
			defenders = (int) (float) numteammates * 0.6 + 0.5;
			if (defenders > 6) defenders = 6;
			//30% accompanies the flag carrier
			attackers = (int) (float) numteammates * 0.3 + 0.5;
			if (attackers > 3) attackers = 3;
			for (i = 0; i < defenders; i++) {
				//
				if (teammates[i] == bs->flagcarrier) {
					continue;
				}
				BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
			}
			// if we have a flag carrier
			if ( bs->flagcarrier != -1 ) {
				ClientName(bs->flagcarrier, carriername, sizeof(carriername));
				for (i = 0; i < attackers; i++) {
					//
					if (teammates[numteammates - i - 1] == bs->flagcarrier) {
						continue;
					}
					//
					if (bs->flagcarrier == bs->client) {
						BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_FOLLOWME, NULL);
					} else {
						BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_FOLLOWTEAMMATE, carriername);
					}
				}
			}
			else {
				for (i = 0; i < attackers; i++) {
					//
					if (teammates[numteammates - i - 1] == bs->flagcarrier) {
						continue;
					}
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
			}
			//
			break;
		}
	}
}

/*
==================
BotDDorders
==================
*/

void BotDDorders_Standard(bot_state_t *bs) {
	int numteammates, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByRelativeTravelTime2ddA(bs, teammates, sizeof(teammates));

	switch(numteammates) {
		case 1: break;
		/*case 2: 
		{
			//the one closest to point A will take that
			ClientName(teammates[0], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_takea", name, NULL);
			BotSayTeamOrder(bs, teammates[0]);
			//BotSayVoiceTeamOrder(bs, teammates[0], VOICECHAT_TAKEA);
			//the other goes for point B
			ClientName(teammates[1], name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_takeb", name, NULL);
			BotSayTeamOrder(bs, teammates[1]);
			//BotSayVoiceTeamOrder(bs, teammates[1], VOICECHAT_TAKEB);
			break;
		}*/
		default:
		{
			for(i=0;i<numteammates/2;i++) { //Half take point A
				ClientName(teammates[i], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_takea", name, NULL);
				BotSayTeamOrder(bs, teammates[i]);
				//BotSayVoiceTeamOrder(bs, teammates[0], VOICECHAT_TAKEA);
			}
			for(i=numteammates/2+1;i<numteammates;i++) { //Rest takes point B
				ClientName(teammates[i], name, sizeof(name));
				BotAI_BotInitialChat(bs, "cmd_takeb", name, NULL);
				BotSayTeamOrder(bs, teammates[i]);
				//BotSayVoiceTeamOrder(bs, teammates[0], VOICECHAT_TAKEB);
			}
			break;
		}
	}
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders_BothFlagsAtBase(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];
	qboolean weAreAttacking;

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);

	weAreAttacking = qfalse;

	if(g_elimination_ctf_oneway.integer > 0) {
		//See if we are attacking:
		if( ( (level.eliminationSides+level.roundNumber)%2 == 0 ) && (BotTeam(bs) == TEAM_RED))
			weAreAttacking = qtrue;
		
		if(weAreAttacking) {
			for (i = 0; i < numteammates; i++) {
				//
				BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_GETTHEFLAG, NULL);
			}
		} else {
			for (i = 0; i < numteammates; i++) {
				//
				BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
			}
		}
		return; //Sago: Or the leader will make a counter order.
	}

	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the second one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				defenders = (int) (float) numteammates * 0.5 + 0.5;
				if (defenders > 5) defenders = 5;
				attackers = (int) (float) numteammates * 0.4 + 0.5;
				if (attackers > 4) attackers = 4;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
	else {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the others should go for the enemy flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				defenders = (int) (float) numteammates * 0.4 + 0.5;
				if (defenders > 4) defenders = 4;
				attackers = (int) (float) numteammates * 0.5 + 0.5;
				if (attackers > 5) attackers = 5;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
BotCTFOrders
==================
*/
void BotCTFOrders(bot_state_t *bs) {
	int flagstatus;

	//
	if (BotTeam(bs) == TEAM_RED) flagstatus = bs->redflagstatus * 2 + bs->blueflagstatus;
	else flagstatus = bs->blueflagstatus * 2 + bs->redflagstatus;
	//
	switch(flagstatus) {
		case 0: BotCTFOrders_BothFlagsAtBase(bs); break;
		case 1: BotCTFOrders_EnemyFlagNotAtBase(bs); break;
		case 2: BotCTFOrders_FlagNotAtBase(bs); break;
		case 3: BotCTFOrders_BothFlagsNotAtBase(bs); break;
	}
}

/*
==================
BotDDorders
==================
*/
void BotDDorders(bot_state_t *bs) {
	BotDDorders_Standard(bs);	
}


/*
==================
BotCreateGroup
==================
*/
void BotCreateGroup(bot_state_t *bs, int *teammates, int groupsize) {
	char name[MAX_NETNAME], leadername[MAX_NETNAME];
	int i;

        if (bot_nochat.integer>2) return;

	// the others in the group will follow the teammates[0]
	for (i = 1; i < groupsize; i++)
	{
		if (teammates[0] == bs->client) {
			BotAI_GiveRoleToTheBot(bs, teammates[0], leadername, ORDER_FOLLOWME, NULL);
		} else {
			BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_FOLLOWTEAMMATE, leadername);
		}
	}
}

/*
==================
BotTeamOrders

  FIXME: defend key areas?
==================
*/
void BotTeamOrders(bot_state_t *bs) {
	int teammates[MAX_CLIENTS];
	int numteammates, i;
	char buf[MAX_INFO_STRING];

	numteammates = 0;
	for (i = 0; i < level.maxclients; i++) {
		trap_GetConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "n"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		if (BotSameTeam(bs, i)) {
			teammates[numteammates] = i;
			numteammates++;
		}
	}
	//
	switch(numteammates) {
		case 1: break;
		case 2:
		{
			//nothing special
			break;
		}
		case 3:
		{
			//have one follow another and one free roaming
			BotCreateGroup(bs, teammates, 2);
			break;
		}
		case 4:
		{
			BotCreateGroup(bs, teammates, 2);		//a group of 2
			BotCreateGroup(bs, &teammates[2], 2);	//a group of 2
			break;
		}
		case 5:
		{
			BotCreateGroup(bs, teammates, 2);		//a group of 2
			BotCreateGroup(bs, &teammates[2], 3);	//a group of 3
			break;
		}
		default:
		{
			if (numteammates <= 10) {
				for (i = 0; i < numteammates / 2; i++) {
					BotCreateGroup(bs, &teammates[i*2], 2);	//groups of 2
				}
			}
			break;
		}
	}
}

/*
==================
Bot1FCTFOrders_FlagAtCenter

  X% defend the base, Y% get the flag
==================
*/
void Bot1FCTFOrders_FlagAtCenter(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the second one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				//50% defend the base
				defenders = (int) (float) numteammates * 0.5 + 0.5;
				if (defenders > 5) defenders = 5;
				//40% get the flag
				attackers = (int) (float) numteammates * 0.4 + 0.5;
				if (attackers > 4) attackers = 4;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
	else { //agressive
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the others should go for the enemy flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				//30% defend the base
				defenders = (int) (float) numteammates * 0.3 + 0.5;
				if (defenders > 3) defenders = 3;
				//60% get the flag
				attackers = (int) (float) numteammates * 0.6 + 0.5;
				if (attackers > 6) attackers = 6;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
Bot1FCTFOrders_TeamHasFlag

  X% towards neutral flag, Y% go towards enemy base and accompany flag carrier if visible
==================
*/
void Bot1FCTFOrders_TeamHasFlag(bot_state_t *bs) {
	int numteammates, defenders, attackers, i, other;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME], carriername[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//tell the one not carrying the flag to attack the enemy base
				if (teammates[0] == bs->flagcarrier) {
					BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				} else {
					BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				}
				break;
			}
			case 3:
			{
				//tell the one closest to the base not carrying the flag to defend the base
				if (teammates[0] != bs->flagcarrier) {
					BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				} else {
					BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				}
				//tell the one furthest from the base not carrying the flag to accompany the flag carrier
				if (teammates[2] != bs->flagcarrier) {
					other = teammates[2];
				} else {
					other = teammates[1];
				}
				if ( bs->flagcarrier != -1 ) {
					if (bs->flagcarrier == bs->client) {
						BotAI_GiveRoleToTheBot(bs, bs->flagcarrier, carriername, ORDER_FOLLOWME, NULL);
					}
					else {
						BotAI_GiveRoleToTheBot(bs, bs->flagcarrier, name, ORDER_FOLLOWTEAMMATE, carriername);
					}
				}
				else {
					//
					BotAI_GiveRoleToTheBot(bs, other, name, ORDER_GETTHEFLAG, NULL);
				}
				break;
			}
			default:
			{
				//30% will defend the base
				defenders = (int) (float) numteammates * 0.3 + 0.5;
				if (defenders > 3) defenders = 3;
				//70% accompanies the flag carrier
				attackers = (int) (float) numteammates * 0.7 + 0.5;
				if (attackers > 7) attackers = 7;
				for (i = 0; i < defenders; i++) {
					//
					if (teammates[i] == bs->flagcarrier) {
						continue;
					}
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				if (bs->flagcarrier != -1) {
					ClientName(bs->flagcarrier, carriername, sizeof(carriername));
					for (i = 0; i < attackers; i++) {
						//
						if (teammates[numteammates - i - 1] == bs->flagcarrier) {
							continue;
						}
						//
						if (bs->flagcarrier == bs->client) {
							BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_FOLLOWME, NULL);
						} else {
							BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_FOLLOWTEAMMATE, carriername);
						}
					}
				}
				else {
					for (i = 0; i < attackers; i++) {
						//
						if (teammates[numteammates - i - 1] == bs->flagcarrier) {
							continue;
						}
						//
						BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
					}
				}
				//
				break;
			}
		}
	}
	else { //agressive
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//tell the one not carrying the flag to defend the base
				if (teammates[0] == bs->flagcarrier) {
					BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				} else {
					BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				}
				break;
			}
			case 3:
			{
				//tell the one closest to the base not carrying the flag to defend the base
				if (teammates[0] != bs->flagcarrier) {
					BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				} else {
					BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				}
				//tell the one furthest from the base not carrying the flag to accompany the flag carrier
				if (teammates[2] != bs->flagcarrier) {
					ClientName(teammates[2], name, sizeof(name));
				} else {
					ClientName(teammates[1], name, sizeof(name));
				}
				if (bs->flagcarrier == bs->client) {
					BotAI_GiveRoleToTheBot(bs, bs->flagcarrier, carriername, ORDER_FOLLOWME, NULL);
				} else {
					BotAI_GiveRoleToTheBot(bs, bs->flagcarrier, name, ORDER_FOLLOWTEAMMATE, carriername);
				}
				break;
			}
			default:
			{
				//20% will defend the base
				defenders = (int) (float) numteammates * 0.2 + 0.5;
				if (defenders > 2) defenders = 2;
				//80% accompanies the flag carrier
				attackers = (int) (float) numteammates * 0.8 + 0.5;
				if (attackers > 8) attackers = 8;
				for (i = 0; i < defenders; i++) {
					//
					if (teammates[i] == bs->flagcarrier) {
						continue;
					}
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				ClientName(bs->flagcarrier, carriername, sizeof(carriername));
				for (i = 0; i < attackers; i++) {
					//
					if (teammates[numteammates - i - 1] == bs->flagcarrier) {
						continue;
					}
					//
					if (bs->flagcarrier == bs->client) {
						BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_FOLLOWME, NULL);
					}
					else {
						BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_FOLLOWTEAMMATE, carriername);
					}
				}
				//
				break;
			}
		}
	}
}

/*
==================
Bot1FCTFOrders_EnemyHasFlag

  X% defend the base, Y% towards neutral flag
==================
*/
void Bot1FCTFOrders_EnemyHasFlag(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//both defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the second one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will also defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_DEFENDTHEBASE, NULL);
				break;
			}
			default:
			{
				//80% will defend the base
				defenders = (int) (float) numteammates * 0.8 + 0.5;
				if (defenders > 8) defenders = 8;
				//10% will try to return the flag
				attackers = (int) (float) numteammates * 0.1 + 0.5;
				if (attackers > 1) attackers = 1;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_RETURNOURFLAG, NULL);
				}
				//
				break;
			}
		}
	}
	else { //agressive
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the others should go for the enemy flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_RETURNOURFLAG, NULL);
				break;
			}
			default:
			{
				//70% defend the base
				defenders = (int) (float) numteammates * 0.7 + 0.5;
				if (defenders > 7) defenders = 7;
				//20% try to return the flag
				attackers = (int) (float) numteammates * 0.2 + 0.5;
				if (attackers > 2) attackers = 2;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_RETURNOURFLAG, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
Bot1FCTFOrders_EnemyDroppedFlag

  X% defend the base, Y% get the flag
==================
*/
void Bot1FCTFOrders_EnemyDroppedFlag(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the second one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				//50% defend the base
				defenders = (int) (float) numteammates * 0.5 + 0.5;
				if (defenders > 5) defenders = 5;
				//40% get the flag
				attackers = (int) (float) numteammates * 0.4 + 0.5;
				if (attackers > 4) attackers = 4;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
	else { //agressive
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will get the flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the others should go for the enemy flag
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_GETTHEFLAG, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_GETTHEFLAG, NULL);
				break;
			}
			default:
			{
				//30% defend the base
				defenders = (int) (float) numteammates * 0.3 + 0.5;
				if (defenders > 3) defenders = 3;
				//60% get the flag
				attackers = (int) (float) numteammates * 0.6 + 0.5;
				if (attackers > 6) attackers = 6;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_GETTHEFLAG, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
Bot1FCTFOrders
==================
*/
void Bot1FCTFOrders(bot_state_t *bs) {
	switch(bs->neutralflagstatus) {
		case 0: Bot1FCTFOrders_FlagAtCenter(bs); break;
		case 1: Bot1FCTFOrders_TeamHasFlag(bs); break;
		case 2: Bot1FCTFOrders_EnemyHasFlag(bs); break;
		case 3: Bot1FCTFOrders_EnemyDroppedFlag(bs); break;
	}
}

/*
==================
BotObeliskOrders

  X% in defence Y% in offence
==================
*/
void BotObeliskOrders(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will attack the enemy base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the one second closest to the base also defends the base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//the other one attacks the enemy base
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				break;
			}
			default:
			{
				//50% defend the base
				defenders = (int) (float) numteammates * 0.5 + 0.5;
				if (defenders > 5) defenders = 5;
				//40% attack the enemy base
				attackers = (int) (float) numteammates * 0.4 + 0.5;
				if (attackers > 4) attackers = 4;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				}
				//
				break;
			}
		}
	}
	else {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will attack the enemy base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the others attack the enemy base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				break;
			}
			default:
			{
				//30% defend the base
				defenders = (int) (float) numteammates * 0.3 + 0.5;
				if (defenders > 3) defenders = 3;
				//70% attack the enemy base
				attackers = (int) (float) numteammates * 0.7 + 0.5;
				if (attackers > 7) attackers = 7;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_ATTACKTHEENEMYBASE, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
BotHarvesterOrders

  X% defend the base, Y% harvest
==================
*/
void BotHarvesterOrders(bot_state_t *bs) {
	int numteammates, defenders, attackers, i;
	int teammates[MAX_CLIENTS];
	char name[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//sort team mates by travel time to base
	numteammates = BotSortTeamMatesByBaseTravelTime(bs, teammates, sizeof(teammates));
	//sort team mates by CTF preference
	BotSortTeamMatesByTaskPreference(bs, teammates, numteammates);
	//passive strategy
	if (!(bs->ctfstrategy & CTFS_AGRESSIVE)) {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will harvest
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_HARVESTSOMESKULLS, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the one second closest to the base also defends the base
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_DEFENDTHEBASE, NULL);
				//the other one goes harvesting
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_HARVESTSOMESKULLS, NULL);
				break;
			}
			default:
			{
				//50% defend the base
				defenders = (int) (float) numteammates * 0.5 + 0.5;
				if (defenders > 5) defenders = 5;
				//40% goes harvesting
				attackers = (int) (float) numteammates * 0.4 + 0.5;
				if (attackers > 4) attackers = 4;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_HARVESTSOMESKULLS, NULL);
				}
				//
				break;
			}
		}
	}
	else {
		//different orders based on the number of team mates
		switch(numteammates) {
			case 1: break;
			case 2:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the other will harvest
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_HARVESTSOMESKULLS, NULL);
				break;
			}
			case 3:
			{
				//the one closest to the base will defend the base
				BotAI_GiveRoleToTheBot(bs, teammates[0], name, ORDER_DEFENDTHEBASE, NULL);
				//the others go harvesting
				BotAI_GiveRoleToTheBot(bs, teammates[1], name, ORDER_HARVESTSOMESKULLS, NULL);
				//
				BotAI_GiveRoleToTheBot(bs, teammates[2], name, ORDER_HARVESTSOMESKULLS, NULL);
				break;
			}
			default:
			{
				//30% defend the base
				defenders = (int) (float) numteammates * 0.3 + 0.5;
				if (defenders > 3) defenders = 3;
				//70% go harvesting
				attackers = (int) (float) numteammates * 0.7 + 0.5;
				if (attackers > 7) attackers = 7;
				for (i = 0; i < defenders; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[i], name, ORDER_DEFENDTHEBASE, NULL);
				}
				for (i = 0; i < attackers; i++) {
					//
					BotAI_GiveRoleToTheBot(bs, teammates[numteammates - i - 1], name, ORDER_HARVESTSOMESKULLS, NULL);
				}
				//
				break;
			}
		}
	}
}

/*
==================
FindHumanTeamLeader
==================
*/
int FindHumanTeamLeader(bot_state_t *bs) {
	int i;

	for (i = 0; i < MAX_CLIENTS; i++) {
		if ( g_entities[i].inuse ) {
			// if this player is not a bot
			if ( !(g_entities[i].r.svFlags & SVF_BOT) ) {
				// if this player is ok with being the leader
				if (!notleader[i]) {
					// if this player is on the same team
					if ( BotSameTeam(bs, i) ) {
						ClientName(i, bs->teamleader, sizeof(bs->teamleader));
						// if not yet ordered to do anything
						if ( !BotSetLastOrderedTask(bs) ) {
							// go on defense by default
							if (bot_nochat.integer<3)BotVoiceChat_Defend(bs, i, SAY_TELL);
						}
						return qtrue;
					}
				}
			}
		}
	}
	return qfalse;
}

int lastRoundNumber; //used to give new orders every round

/*
==================
BotTeamAI
==================
*/
void BotTeamAI(bot_state_t *bs) {
	int numteammates;
	char netname[MAX_NETNAME];

        if (bot_nochat.integer>2) return;

	//
	if (!GAMETYPE_IS_A_TEAM_GAME(gametype))
		return;
	// make sure we've got a valid team leader
	if (!BotValidTeamLeader(bs)) {
		//
		if (!FindHumanTeamLeader(bs)) {
			//
			if (!bs->askteamleader_time && !bs->becometeamleader_time) {
				if (bs->entergame_time + 10 > FloatTime()) {
					bs->askteamleader_time = FloatTime() + 5 + random() * 10;
				}
				else {
					bs->becometeamleader_time = FloatTime() + 5 + random() * 10;
				}
			}
			if (bs->askteamleader_time && bs->askteamleader_time < FloatTime()) {
				// if asked for a team leader and no response
				BotAI_BotInitialChat(bs, "whoisteamleader", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				bs->askteamleader_time = 0;
				bs->becometeamleader_time = FloatTime() + 8 + random() * 10;
			}
			if (bs->becometeamleader_time && bs->becometeamleader_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "iamteamleader", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotSayVoiceTeamOrder(bs, -1, VOICECHAT_STARTLEADER);
				ClientName(bs->client, netname, sizeof(netname));
				strncpy(bs->teamleader, netname, sizeof(bs->teamleader));
				bs->teamleader[sizeof(bs->teamleader)-1] = '\0';
				bs->becometeamleader_time = 0;
			}
			return;
		}
	}
	bs->askteamleader_time = 0;
	bs->becometeamleader_time = 0;

	//return if this bot is NOT the team leader
	ClientName(bs->client, netname, sizeof(netname));
	if ( !Q_strequal(netname, bs->teamleader) ) {
		return;
	}
	//
	numteammates = BotNumTeamMates(bs);
	//give orders
	switch(gametype) {
		case GT_TEAM:
		case GT_ELIMINATION:
		{
			if (bs->numteammates != numteammates || bs->forceorders) {
				bs->teamgiveorders_time = FloatTime();
				bs->numteammates = numteammates;
				bs->forceorders = qfalse;
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < FloatTime() - 5) {
				BotTeamOrders(bs);
				//give orders again after 120 seconds
				bs->teamgiveorders_time = FloatTime() + 120;
			}
			break;
		}
		case GT_CTF:
		case GT_CTF_ELIMINATION:
		{
			//if the number of team mates changed or the flag status changed
			//or someone wants to know what to do
			if (bs->numteammates != numteammates || bs->flagstatuschanged || bs->forceorders || lastRoundNumber != level.roundNumber) {
				bs->teamgiveorders_time = FloatTime();
				bs->numteammates = numteammates;
				bs->flagstatuschanged = qfalse;
				bs->forceorders = qfalse;
				lastRoundNumber = level.roundNumber;
			}
			//if there were no flag captures the last 3 minutes
			if (bs->lastflagcapture_time < FloatTime() - 240) {
				bs->lastflagcapture_time = FloatTime();
				//randomly change the CTF strategy
				if (random() < 0.4) {
					bs->ctfstrategy ^= CTFS_AGRESSIVE;
					bs->teamgiveorders_time = FloatTime();
				}
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < FloatTime() - 3) {
				BotCTFOrders(bs);
				//
				bs->teamgiveorders_time = 0;
			}
			break;
		}
		case GT_DOUBLE_D:
		{
			//if the number of team mates changed or the domination point status changed
			//or someone wants to know what to do
			if (bs->numteammates != numteammates || bs->flagstatuschanged || bs->forceorders) {
				bs->teamgiveorders_time = FloatTime();
				bs->numteammates = numteammates;
				bs->flagstatuschanged = qfalse;
				bs->forceorders = qfalse;
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < FloatTime() - 3) {
				BotDDorders(bs);
				//
				bs->teamgiveorders_time = 0;
			}
			break;
		}
		case GT_1FCTF:
		{
			if (bs->numteammates != numteammates || bs->flagstatuschanged || bs->forceorders) {
				bs->teamgiveorders_time = FloatTime();
				bs->numteammates = numteammates;
				bs->flagstatuschanged = qfalse;
				bs->forceorders = qfalse;
			}
			//if there were no flag captures the last 4 minutes
			if (bs->lastflagcapture_time < FloatTime() - 240) {
				bs->lastflagcapture_time = FloatTime();
				//randomly change the CTF strategy
				if (random() < 0.4) {
					bs->ctfstrategy ^= CTFS_AGRESSIVE;
					bs->teamgiveorders_time = FloatTime();
				}
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < FloatTime() - 2) {
				Bot1FCTFOrders(bs);
				//
				bs->teamgiveorders_time = 0;
			}
			break;
		}
		case GT_OBELISK:
		{
			if (bs->numteammates != numteammates || bs->forceorders) {
				bs->teamgiveorders_time = FloatTime();
				bs->numteammates = numteammates;
				bs->forceorders = qfalse;
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < FloatTime() - 5) {
				BotObeliskOrders(bs);
				//give orders again after 30 seconds
				bs->teamgiveorders_time = FloatTime() + 30;
			}
			break;
		}
		case GT_HARVESTER:
		{
			if (bs->numteammates != numteammates || bs->forceorders) {
				bs->teamgiveorders_time = FloatTime();
				bs->numteammates = numteammates;
				bs->forceorders = qfalse;
			}
			//if it's time to give orders
			if (bs->teamgiveorders_time && bs->teamgiveorders_time < FloatTime() - 5) {
				BotHarvesterOrders(bs);
				//give orders again after 30 seconds
				bs->teamgiveorders_time = FloatTime() + 30;
			}
			break;
		}
	}
}

/*
==================
BotAI_GiveRoleToTheBot
==================
*/
void BotAI_GiveRoleToTheBot(bot_state_t *bs, int teammate, char *name, int order, char *botToFollow) {
	char* initialchat;
	char* voiceorder;

	if (bot_nochat.integer >= 3) {
		return;
	}

	ClientName(teammate, name, sizeof(name));
	switch(order) {
		case ORDER_GETTHEFLAG:
		{
			initialchat = "cmd_getflag";
			voiceorder = VOICECHAT_GETFLAG;
			break;
		}
		case ORDER_FOLLOWME:
		{
			initialchat = "cmd_accompanyme";
			voiceorder = VOICECHAT_FOLLOWME;
			break;
		}
		case ORDER_FOLLOWTEAMMATE:
		{
			if (!(botToFollow)) {
				return;
			}
			ClientName(teammate, name, sizeof(name));
			BotAI_BotInitialChat(bs, "cmd_accompany", name, botToFollow, NULL);
			BotSayTeamOrder(bs, teammate);
			BotSayVoiceTeamOrder(bs, teammate, VOICECHAT_FOLLOWFLAGCARRIER);
			break;
		}
		case ORDER_DEFENDTHEBASE:
		{
			initialchat = "cmd_defendbase";
			voiceorder = VOICECHAT_DEFEND;
			break;
		}
		case ORDER_TAKEPOINTA:
		{
			initialchat = "cmd_takea";
			// voiceorder = VOICECHAT_TAKEA;
			voiceorder = VOICECHAT_OFFENSE;
			break;
		}
		case ORDER_TAKEPOINTB:
		{
			initialchat = "cmd_takeb";
			// voiceorder = VOICECHAT_TAKEB;
			voiceorder = VOICECHAT_OFFENSE;
			break;
		}
		case ORDER_ATTACKTHEENEMYBASE:
		{
			initialchat = "cmd_attackenemybase";
			voiceorder = VOICECHAT_OFFENSE;
			break;
		}
		case ORDER_RETURNOURFLAG:
		{
			initialchat = "cmd_returnflag";
			// voiceorder = VOICECHAT_RETURNOURFLAG;
			voiceorder = VOICECHAT_GETFLAG;
			break;
		}
		case ORDER_HARVESTSOMESKULLS:
		{
			initialchat = "cmd_harvest";
			// voiceorder = VOICECHAT_HARVESTSKULLS;
			voiceorder = VOICECHAT_OFFENSE;
			break;
		}
		default:
		{
			// Here goes the command for roaming.
			break;
		}
	}

	if(order != ORDER_FOLLOWTEAMMATE) {
		BotAI_BotInitialChat(bs, initialchat, name, NULL);
		BotSayTeamOrder(bs, teammate);
		BotSayVoiceTeamOrder(bs, teammate, voiceorder);
	}
}

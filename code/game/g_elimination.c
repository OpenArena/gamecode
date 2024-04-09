/*
===========================================================================
Copyright (C) 2008-2016 Poul Sander

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

#include "g_local.h"


static void CloseEliminationDoors( void ) {
	gentity_t* next;
	next = NULL;
	for (next = G_Find(next, FOFS(classname), "func_door"); next ; next = G_Find(next, FOFS(classname), "func_door") ) {
		if ( strequals(next->targetname, ELIMINATION_ACTIVE_TARGETNAME) ) {
			if (next->moverState != MOVER_2TO1 && next->moverState != MOVER_POS1 ) {
				MatchTeam(next, MOVER_2TO1, level.time);
			}
		}
	}
}

static void CloseEliminationDoorsInstantly( void ) {
	gentity_t* next;
	next = NULL;
	for (next = G_Find(next, FOFS(classname), "func_door"); next ; next = G_Find(next, FOFS(classname), "func_door") ) {
		if ( strequals(next->targetname, ELIMINATION_ACTIVE_TARGETNAME) ) {
			if ( next->moverState != MOVER_POS1 ) {
				MatchTeam(next, MOVER_POS1, level.time);
			}
		}
	}
}

static void OpenEliminationDoors( void ) {
	gentity_t* next;
	next = NULL;
	for (next = G_Find(next, FOFS(classname), "func_door"); next ; next = G_Find(next, FOFS(classname), "func_door") ) {
		if ( strequals(next->targetname, ELIMINATION_ACTIVE_TARGETNAME) ) {
			if (next->moverState != MOVER_1TO2 && next->moverState != MOVER_POS2 ) {
				MatchTeam(next, MOVER_1TO2, level.time);
			}
		}
	}
}


/*
========================
SendAttackingTeamMessageToAllClients

Used for CTF Elimination oneway
========================
*/
void SendAttackingTeamMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			AttackingTeamMessage( g_entities + i );
		}
	}
}

//the elimination start function
void StartEliminationRound(void) {

	int		countsLiving[TEAM_NUM_TEAMS];
	countsLiving[TEAM_BLUE] = TeamLivingCount( -1, TEAM_BLUE );
	countsLiving[TEAM_RED] = TeamLivingCount( -1, TEAM_RED );
	if((countsLiving[TEAM_BLUE]==0) || (countsLiving[TEAM_RED]==0))
	{
		trap_SendServerCommand( -1, "print \"Not enough players to start the round\n\"");
		level.roundNumberStarted = level.roundNumber-1;
		level.roundRespawned = qfalse;
		//Remember that one of the teams is empty!
		level.roundRedPlayers = countsLiving[TEAM_RED];
		level.roundBluePlayers = countsLiving[TEAM_BLUE];
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}
	
	//If we are enough to start a round:
	level.roundNumberStarted = level.roundNumber; //Set numbers
	level.roundRedPlayers = countsLiving[TEAM_RED];
	level.roundBluePlayers = countsLiving[TEAM_BLUE];
	if(g_gametype.integer == GT_CTF_ELIMINATION) {
		Team_ReturnFlag( TEAM_RED );
		Team_ReturnFlag( TEAM_BLUE );
	}
	if(g_gametype.integer == GT_ELIMINATION) {
		G_LogPrintf( "ELIMINATION: %i %i %i: Round %i has started!\n", level.roundNumber, -1, 0, level.roundNumber );
	} else if(g_gametype.integer == GT_CTF_ELIMINATION) {
		G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: Round %i has started!\n", level.roundNumber, -1, -1, 4, level.roundNumber );
	}
	SendEliminationMessageToAllClients();
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients(); //Ensure that evaryone know who should attack.
	EnableWeapons();
	OpenEliminationDoors();
}

//things to do at end of round:
void EndEliminationRound(void)
{
	DisableWeapons();
	CloseEliminationDoors();
	level.roundNumber++;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	CalculateRanks();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

//Things to do if we don't want to move the roundNumber
void RestartEliminationRound(void) {
	DisableWeapons();
	CloseEliminationDoors();
	level.roundNumberStarted = level.roundNumber-1;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	if(!level.intermissiontime)
		SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}

//Things to do during match warmup
void WarmupEliminationRound(void) {
	EnableWeapons();
	//OpenEliminationDoors();
	level.roundNumberStarted = level.roundNumber-1;
	level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
	SendEliminationMessageToAllClients();
	level.roundRespawned = qfalse;
	if(g_elimination_ctf_oneway.integer)
		SendAttackingTeamMessageToAllClients();
}


//LMS - Last man Stading functions:
void StartLMSRound(void) {
	int		countsLiving;
	countsLiving = TeamLivingCount( -1, TEAM_FREE );
	if(countsLiving<2) {
		trap_SendServerCommand( -1, "print \"Not enough players to start the round\n\"");
		level.roundNumberStarted = level.roundNumber-1;
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}

	//If we are enough to start a round:
	level.roundNumberStarted = level.roundNumber; //Set numbers

	G_LogPrintf( "LMS: %i %i %i: Round %i has started!\n", level.roundNumber, -1, 0, level.roundNumber );

	SendEliminationMessageToAllClients();
	EnableWeapons();
}

/*
=============
CheckElimination
=============
*/
void CheckElimination(void) {
	int counts[TEAM_NUM_TEAMS];
	int countsLiving[TEAM_NUM_TEAMS];
	int countsHealth[TEAM_NUM_TEAMS];
	int countLivingHumans;
	int countHumans;

	if ( level.numPlayingClients < 1 ) {
		if( G_IsARoundBasedGametype(g_gametype.integer) && G_IsATeamGametype(g_gametype.integer) && ( level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime )) {
			RestartEliminationRound(); //For spectators
		}
		return;
	}	

	//We don't want to do anything in itnermission
	if(level.intermissiontime) {
		if(level.roundRespawned) {
			RestartEliminationRound();
		}
		level.roundStartTime = level.time+1000*g_elimination_warmup.integer;
		return;
	}	
	
	if(!(G_IsARoundBasedGametype(g_gametype.integer) && G_IsATeamGametype(g_gametype.integer))) {
		return;
	}

	
	counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE );
	counts[TEAM_RED] = TeamCount( -1, TEAM_RED );

	countsLiving[TEAM_BLUE] = TeamLivingCount( -1, TEAM_BLUE );
	countsLiving[TEAM_RED] = TeamLivingCount( -1, TEAM_RED );

	countsHealth[TEAM_BLUE] = TeamHealthCount( -1, TEAM_BLUE );
	countsHealth[TEAM_RED] = TeamHealthCount( -1, TEAM_RED );

	countHumans = TeamHumanParticipantsCount(-1);
	countLivingHumans = TeamLivingHumanCount(-1);

	if (countLivingHumans == 0 && countHumans > 0) {
		level.humansEliminated = qtrue;
	}
	else {
		level.humansEliminated = qfalse;
	}

	if(level.roundBluePlayers != 0 && level.roundRedPlayers != 0) { //Cannot score if one of the team never got any living players
		if((countsLiving[TEAM_BLUE]==0)&&(level.roundNumber==level.roundNumberStarted))
		{
			//Blue team has been eliminated!
			trap_SendServerCommand( -1, "print \"Blue Team eliminated!\n\"");
			AddTeamScore(level.intermission_origin,TEAM_RED,1);
			if(g_gametype.integer == GT_ELIMINATION) {
				G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, TEAM_RED, 1, TeamName(TEAM_RED), level.roundNumber );
			} else {
				G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, -1, TEAM_RED, 6, TeamName(TEAM_RED), level.roundNumber );
			}
			EndEliminationRound();
			Team_ForceGesture(TEAM_RED);
		}
		else if((countsLiving[TEAM_RED]==0)&&(level.roundNumber==level.roundNumberStarted))
		{
			//Red team eliminated!
			trap_SendServerCommand( -1, "print \"Red Team eliminated!\n\"");
			AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
			if(g_gametype.integer == GT_ELIMINATION) {
				G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, TEAM_BLUE, 1, TeamName(TEAM_BLUE), level.roundNumber );
			} else {
				G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by eleminating the enemy team!\n", level.roundNumber, -1, TEAM_BLUE, 6, TeamName(TEAM_BLUE), level.roundNumber );
			}
			EndEliminationRound();
			Team_ForceGesture(TEAM_BLUE);
		}
	}

	//Time up
	if((level.roundNumber==level.roundNumberStarted)&&(g_elimination_roundtime.integer)&&(level.time>=level.roundStartTime+1000*g_elimination_roundtime.integer))
	{
		trap_SendServerCommand( -1, "print \"No teams eliminated.\n\"");

		if(level.roundBluePlayers != 0 && level.roundRedPlayers != 0) {//We don't want to divide by zero. (should not be possible)
			if(g_gametype.integer == GT_CTF_ELIMINATION && g_elimination_ctf_oneway.integer) {
				//One way CTF, make defensice team the winner.
				if ( (level.eliminationSides+level.roundNumber)%2 == 0 ) { //Red was attacking
					trap_SendServerCommand( -1, "print \"Blue team defended the base\n\"");
					AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
					G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by defending the flag!\n", level.roundNumber, -1, TEAM_BLUE, 5, TeamName(TEAM_BLUE), level.roundNumber );
				}
				else {
					trap_SendServerCommand( -1, "print \"Red team defended the base\n\"");
					AddTeamScore(level.intermission_origin,TEAM_RED,1);
					G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i by defending the flag!\n", level.roundNumber, -1, TEAM_RED, 5, TeamName(TEAM_RED), level.roundNumber );
				}
			}
			else if(((double)countsLiving[TEAM_RED])/((double)level.roundRedPlayers)>((double)countsLiving[TEAM_BLUE])/((double)level.roundBluePlayers))
			{
				//Red team has higher procentage survivors
				trap_SendServerCommand( -1, "print \"Red team has most survivers!\n\"");
				AddTeamScore(level.intermission_origin,TEAM_RED,1);
				if(g_gametype.integer == GT_ELIMINATION) {
					G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, TEAM_RED, 2, TeamName(TEAM_RED), level.roundNumber );
				} else {
					G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, -1, TEAM_RED, 7, TeamName(TEAM_RED), level.roundNumber );
				}
			}
			else if(((double)countsLiving[TEAM_RED])/((double)level.roundRedPlayers)<((double)countsLiving[TEAM_BLUE])/((double)level.roundBluePlayers))
			{
				//Blue team has higher procentage survivors
				trap_SendServerCommand( -1, "print \"Blue team has most survivers!\n\"");
				AddTeamScore(level.intermission_origin,TEAM_BLUE,1);	
				if(g_gametype.integer == GT_ELIMINATION) {
					G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, TEAM_BLUE, 2, TeamName(TEAM_BLUE), level.roundNumber );
				} else {
					G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more survivors!\n", level.roundNumber, -1, TEAM_BLUE, 7, TeamName(TEAM_BLUE), level.roundNumber );
				}
			}
			else if(countsHealth[TEAM_RED]>countsHealth[TEAM_BLUE])
			{
				//Red team has more health
				trap_SendServerCommand( -1, "print \"Red team has more health left!\n\"");
				AddTeamScore(level.intermission_origin,TEAM_RED,1);
				if(g_gametype.integer == GT_ELIMINATION) {
					G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, TEAM_RED, 3, TeamName(TEAM_RED), level.roundNumber );
				} else {
					G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, -1, TEAM_RED, 8, TeamName(TEAM_RED), level.roundNumber );
				}
			}
			else if(countsHealth[TEAM_RED]<countsHealth[TEAM_BLUE])
			{
				//Blue team has more health
				trap_SendServerCommand( -1, "print \"Blue team has more health left!\n\"");
				AddTeamScore(level.intermission_origin,TEAM_BLUE,1);
				if(g_gametype.integer == GT_ELIMINATION) {
					G_LogPrintf( "ELIMINATION: %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, TEAM_BLUE, 3, TeamName(TEAM_BLUE), level.roundNumber );
				} else {
					G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: %s wins round %i due to more health left!\n", level.roundNumber, -1, TEAM_BLUE, 8, TeamName(TEAM_BLUE), level.roundNumber );
				}
			}
		}
		//Draw
		if(g_gametype.integer == GT_ELIMINATION) {
			G_LogPrintf( "ELIMINATION: %i %i %i: Round %i ended in a draw!\n", level.roundNumber, -1, 4, level.roundNumber );
		} else {
			G_LogPrintf( "CTF_ELIMINATION: %i %i %i %i: Round %i ended in a draw!\n", level.roundNumber, -1, -1, 9, level.roundNumber );
		}
		EndEliminationRound();
	}

	//This might be better placed another place:
	if(g_elimination_activewarmup.integer<1)
		g_elimination_activewarmup.integer=1; //We need at least 1 second to spawn all players
	if(g_elimination_activewarmup.integer >= g_elimination_warmup.integer) //This must not be true
		g_elimination_warmup.integer = g_elimination_activewarmup.integer+1; //Increase warmup

	//Force respawn
	if(level.roundNumber!=level.roundNumberStarted && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer && !level.roundRespawned)
	{
		level.roundRespawned = qtrue;
		RespawnAll();
		CloseEliminationDoorsInstantly();
		SendEliminationMessageToAllClients();
	}

	if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
	{
		RespawnDead();
	}


	if((level.roundNumber>level.roundNumberStarted)&&(level.time>=level.roundStartTime))
		StartEliminationRound();

	if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime) {
		if(counts[TEAM_BLUE]<1 || counts[TEAM_RED]<1) {
			RespawnDead(); //Allow players to run around anyway
			OpenEliminationDoors();
			WarmupEliminationRound(); //Start over
			return;
		}
	}

	if(level.warmupTime != 0) {
		if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
		{
			RespawnDead();
			WarmupEliminationRound();
		}
	}
}


/*
CheckLMS
*/

void CheckLMS(void) {
	int mode;
	int countsLiving[TEAM_NUM_TEAMS];
	int countHumans;
	int countLivingHumans;
	mode = g_lms_mode.integer;
	if ( level.numPlayingClients < 1 ) {
		return;
	}

	//We don't want to do anything in intermission
	if(level.intermissiontime) {
		if(level.roundRespawned) {
			RestartEliminationRound();
		}
		level.roundStartTime = level.time; //so that a player might join at any time to fix the bots+no humans+autojoin bug
		return;
	}
	
	if (g_gametype.integer != GT_LMS) {
		return;
	}
	
	//trap_SendServerCommand( -1, "print \"This is LMS!\n\"");
	countsLiving[TEAM_FREE] = TeamLivingCount( -1, TEAM_FREE );

	countHumans = TeamHumanParticipantsCount(-1);
	countLivingHumans = TeamLivingHumanCount(-1);

	if (countLivingHumans == 0 && countHumans > 0) {
		level.humansEliminated = qtrue;
	}
	else {
		level.humansEliminated = qfalse;
	}

	if(countsLiving[TEAM_FREE]==1 && level.roundNumber==level.roundNumberStarted)
	{
		if(mode <=1 )
		LMSpoint();
		trap_SendServerCommand( -1, "print \"We have a winner!\n\"");
		EndEliminationRound();
		Team_ForceGesture(TEAM_FREE);
	}

	if(countsLiving[TEAM_FREE]==0 && level.roundNumber==level.roundNumberStarted)
	{
		trap_SendServerCommand( -1, "print \"All death... how sad\n\"");
		EndEliminationRound();
	}

	if((g_elimination_roundtime.integer) && (level.roundNumber==level.roundNumberStarted)&&(g_lms_mode.integer == 1 || g_lms_mode.integer==3)&&(level.time>=level.roundStartTime+1000*g_elimination_roundtime.integer))
	{
		trap_SendServerCommand( -1, "print \"Time up - Overtime disabled\n\"");
		if(mode <=1 )
		LMSpoint();
		EndEliminationRound();
	}

	//This might be better placed another place:
	if(g_elimination_activewarmup.integer<2)
		g_elimination_activewarmup.integer=2; //We need at least 2 seconds to spawn all players
	if(g_elimination_activewarmup.integer >= g_elimination_warmup.integer) //This must not be true
		g_elimination_warmup.integer = g_elimination_activewarmup.integer+1; //Increase warmup

	//Force respawn
	if(level.roundNumber != level.roundNumberStarted && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer && !level.roundRespawned)
	{
		level.roundRespawned = qtrue;
		RespawnAll();
		DisableWeapons();
		SendEliminationMessageToAllClients();
	}

	if(level.time<=level.roundStartTime && level.time>level.roundStartTime-1000*g_elimination_activewarmup.integer)
	{
		RespawnDead();
		//DisableWeapons();
	}

	if(level.roundNumber == level.roundNumberStarted)
	{
		EnableWeapons();
	}

	if((level.roundNumber>level.roundNumberStarted)&&(level.time>=level.roundStartTime))
		StartLMSRound();

	if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime && level.numPlayingClients < 2)
	{
		RespawnDead(); //Allow player to run around anyway
		WarmupEliminationRound(); //Start over
		return;
	}

	if(level.warmupTime != 0) {
		if(level.time+1000*g_elimination_warmup.integer-500>level.roundStartTime)
		{
			RespawnDead();
			WarmupEliminationRound();
		}
	}
}

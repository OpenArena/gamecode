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

// g_client.c -- client functions that don't happen every frame

static vec3_t	playerMins = {-15, -15, -24};
static vec3_t	playerMaxs = {15, 15, 32};

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
"nobots" will prevent bots from using this spot.
"nohumans" will prevent non-bots from using this spot.
*/
void SP_info_player_deathmatch( gentity_t *ent ) {
	int		i;

	G_SpawnInt( "nobots", "0", &i);
	if ( i ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_HUMANS;
	}
}

/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
equivelant to info_player_deathmatch
*/
void SP_info_player_start(gentity_t *ent) {
	ent->classname = "info_player_deathmatch";
	SP_info_player_deathmatch( ent );
}

//Three for Double_D
void SP_info_player_dd(gentity_t *ent) {
}
void SP_info_player_dd_red(gentity_t *ent) {
}
void SP_info_player_dd_blue(gentity_t *ent) {
}

//One for Standard Domination, not really a player spawn point
void SP_domination_point(gentity_t *ent) {
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32)
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent ) {

}



/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
SpotWouldTelefrag

================
*/
qboolean SpotWouldTelefrag( gentity_t *spot ) {
	int			i, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;
	vec3_t		mins, maxs;

	VectorAdd( spot->s.origin, playerMins, mins );
	VectorAdd( spot->s.origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) {
		hit = &g_entities[touch[i]];
		//if ( hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 ) {
		if ( hit->client) {
			return qtrue;
		}

	}

	return qfalse;
}

/*
================
SelectNearestDeathmatchSpawnPoint

Find the spot that we DON'T want to use
================
*/
#define	MAX_SPAWN_POINTS	128
gentity_t *SelectNearestDeathmatchSpawnPoint( vec3_t from ) {
	gentity_t	*spot;
	vec3_t		delta;
	float		dist, nearestDist;
	gentity_t	*nearestSpot;

	nearestDist = 999999;
	nearestSpot = NULL;
	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {

		VectorSubtract( spot->s.origin, from, delta );
		dist = VectorLength( delta );
		if ( dist < nearestDist ) {
			nearestDist = dist;
			nearestSpot = spot;
		}
	}

	return nearestSpot;
}


/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define	MAX_SPAWN_POINTS	128
gentity_t *SelectRandomDeathmatchSpawnPoint( void ) {
	gentity_t	*spot;
	int			count;
	int			selection;
	gentity_t	*spots[MAX_SPAWN_POINTS];

	count = 0;
	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		count++;
	}

	if ( !count ) {	// no spots that won't telefrag
		return G_Find( NULL, FOFS(classname), "info_player_deathmatch");
	}

	selection = rand() % count;
	return spots[ selection ];
}

/*
===========
SelectRandomFurthestSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *SelectRandomFurthestSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles ) {
	gentity_t	*spot;
	vec3_t		delta;
	float		dist;
	float		list_dist[64];
	gentity_t	*list_spot[64];
	int			numSpots, rnd, i, j;

	numSpots = 0;
	spot = NULL;

	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		VectorSubtract( spot->s.origin, avoidPoint, delta );
		dist = VectorLength( delta );
		for (i = 0; i < numSpots; i++) {
			if ( dist > list_dist[i] ) {
				if ( numSpots >= 64 )
					numSpots = 64-1;
				for (j = numSpots; j > i; j--) {
					list_dist[j] = list_dist[j-1];
					list_spot[j] = list_spot[j-1];
				}
				list_dist[i] = dist;
				list_spot[i] = spot;
				numSpots++;
				if (numSpots > 64)
					numSpots = 64;
				break;
			}
		}
		if (i >= numSpots && numSpots < 64) {
			list_dist[numSpots] = dist;
			list_spot[numSpots] = spot;
			numSpots++;
		}
	}
	if (!numSpots) {
		spot = G_Find( NULL, FOFS(classname), "info_player_deathmatch");
		if (!spot)
			G_Error( "Couldn't find a spawn point" );
		VectorCopy (spot->s.origin, origin);
		origin[2] += 9;
		VectorCopy (spot->s.angles, angles);
		return spot;
	}

	// select a random spot from the spawn points furthest away
	rnd = random() * (numSpots / 2);

	VectorCopy (list_spot[rnd]->s.origin, origin);
	origin[2] += 9;
	VectorCopy (list_spot[rnd]->s.angles, angles);

	return list_spot[rnd];
}

/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *SelectSpawnPoint ( vec3_t avoidPoint, vec3_t origin, vec3_t angles ) {
	//return SelectRandomFurthestSpawnPoint( avoidPoint, origin, angles );

	
	gentity_t	*spot;
	gentity_t	*nearestSpot;

	nearestSpot = SelectNearestDeathmatchSpawnPoint( avoidPoint );

	spot = SelectRandomDeathmatchSpawnPoint ( );
	if ( spot == nearestSpot ) {
		// roll again if it would be real close to point of death
		spot = SelectRandomDeathmatchSpawnPoint ( );
		if ( spot == nearestSpot ) {
			// last try
			spot = SelectRandomDeathmatchSpawnPoint ( );
		}		
	}

	// find a single player start spot
	if (!spot) {
		G_Error( "Couldn't find a spawn point" );
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*
===========
SelectInitialSpawnPoint

Try to find a spawn point marked 'initial', otherwise
use normal spawn selection.
============
*/
gentity_t *SelectInitialSpawnPoint( vec3_t origin, vec3_t angles ) {
	gentity_t	*spot;

	spot = NULL;
	while ((spot = G_Find (spot, FOFS(classname), "info_player_deathmatch")) != NULL) {
		if ( spot->spawnflags & 1 ) {
			break;
		}
	}

	if ( !spot || SpotWouldTelefrag( spot ) ) {
		return SelectSpawnPoint( vec3_origin, origin, angles );
	}

	VectorCopy (spot->s.origin, origin);
	origin[2] += 9;
	VectorCopy (spot->s.angles, angles);

	return spot;
}

/*
===========
SelectSpectatorSpawnPoint

============
*/
gentity_t *SelectSpectatorSpawnPoint( vec3_t origin, vec3_t angles ) {
	//gentity_t	*spot;

	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, origin );
	VectorCopy( level.intermission_angle, angles );



	//for some reason we need to return an specific point in elimination (this might not be neccecary anymore but to be sure...)
	//if(g_gametype.integer == GT_ELIMINATION)
	//	return SelectSpawnPoint( vec3_origin, origin, angles );

	//VectorCopy (origin,spot->s.origin);
	//spot->s.origin[2] += 9;
	//VectorCopy (angles, spot->s.angles);

	return NULL; //spot;
}

/*
=======================================================================

BODYQUE

=======================================================================
*/

/*
===============
InitBodyQue
===============
*/
void InitBodyQue (void) {
	int		i;
	gentity_t	*ent;

	level.bodyQueIndex = 0;
	for (i=0; i<BODY_QUEUE_SIZE ; i++) {
		ent = G_Spawn();
		ent->classname = "bodyque";
		ent->neverFree = qtrue;
		level.bodyQue[i] = ent;
	}
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent ) {
	if ( level.time - ent->timestamp > 6500 ) {


		// the body ques are never actually freed, they are just unlinked
		trap_UnlinkEntity( ent );
		ent->physicsObject = qfalse;
		return;	
	}
	ent->nextthink = level.time + 100;
	ent->s.pos.trBase[2] -= 1;
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent ) {
	gentity_t	*e;
	int i;
	gentity_t		*body;
	int			contents;

	trap_UnlinkEntity (ent);

	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->s.origin, -1 );
	if ( (contents & CONTENTS_NODROP) && !(ent->s.eFlags & EF_KAMIKAZE) ) { //the check for kamikaze is a workaround for ctf4ish
            return;
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = (level.bodyQueIndex + 1) % BODY_QUEUE_SIZE;

        //Check if the next body has the kamikaze, in that case skip it.
        for(i=0;(level.bodyQue[ level.bodyQueIndex ]->s.eFlags & EF_KAMIKAZE) && (i<10);i++) {
            level.bodyQueIndex = (level.bodyQueIndex + 1) % BODY_QUEUE_SIZE;
        }

	body->s = ent->s;
	body->s.eFlags = EF_DEAD;		// clear EF_TALK, etc
	if ( ent->s.eFlags & EF_KAMIKAZE ) {
                ent->s.eFlags &= ~EF_KAMIKAZE;
		body->s.eFlags |= EF_KAMIKAZE;

		// check if there is a kamikaze timer around for this owner
		for (i = 0; i < MAX_GENTITIES; i++) {
			e = &g_entities[i];
			if (!e->inuse)
				continue;
			if (e->activator != ent)
				continue;
			if (strcmp(e->classname, "kamikaze timer"))
				continue;
			e->activator = body;
			break;
		}
	}
	body->s.powerups = 0;	// clear powerups
	body->s.loopSound = 0;	// clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;		// don't bounce
	if ( body->s.groundEntityNum == ENTITYNUM_NONE ) {
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	} else {
		body->s.pos.trType = TR_STATIONARY;
	}
	body->s.event = 0;

	// change the animation to the last-frame only, so the sequence
	// doesn't repeat anew for the body
	switch ( body->s.legsAnim & ~ANIM_TOGGLEBIT ) {
	case BOTH_DEATH1:
	case BOTH_DEAD1:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD1;
		break;
	case BOTH_DEATH2:
	case BOTH_DEAD2:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD2;
		break;
	case BOTH_DEATH3:
	case BOTH_DEAD3:
	default:
		body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD3;
		break;
	}

	body->r.svFlags = ent->r.svFlags;
	VectorCopy (ent->r.mins, body->r.mins);
	VectorCopy (ent->r.maxs, body->r.maxs);
	VectorCopy (ent->r.absmin, body->r.absmin);
	VectorCopy (ent->r.absmax, body->r.absmax);

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	body->r.contents = CONTENTS_CORPSE;
	body->r.ownerNum = ent->s.number;

	body->nextthink = level.time + 5000;
	body->think = BodySink;

	body->die = body_die;

	// don't take more damage if already gibbed
	if ( ent->health <= GIB_HEALTH ) {
		body->takedamage = qfalse;
	} else {
		body->takedamage = qtrue;
	}


	VectorCopy ( body->s.pos.trBase, body->r.currentOrigin );
	trap_LinkEntity (body);
}

//======================================================================


/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle ) {
	int			i;

	// set the delta angle
	for (i=0 ; i<3 ; i++) {
		int		cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}
	VectorCopy( angle, ent->s.angles );
	VectorCopy (ent->s.angles, ent->client->ps.viewangles);
}

/*
================
respawn
================
*/
void ClientRespawn( gentity_t *ent ) {
	gentity_t	*tent;

	if((g_gametype.integer!=GT_ELIMINATION && g_gametype.integer!=GT_CTF_ELIMINATION && g_gametype.integer !=GT_LMS) && !ent->client->isEliminated)
	{
		ent->client->isEliminated = qtrue; //must not be true in warmup
		//Tried moving CopyToBodyQue
	} else {
                //Must always be false in other gametypes
                ent->client->isEliminated = qfalse;
        }
        CopyToBodyQue (ent); //Unlinks ent

	if(g_gametype.integer==GT_LMS) {
		if(ent->client->pers.livesLeft>0)
		{
			//ent->client->pers.livesLeft--; Coutned down somewhere else
			ent->client->isEliminated = qfalse;
		}
		else //We have used all our lives
		{
			if( ent->client->isEliminated!=qtrue) {
				ent->client->isEliminated = qtrue;
				if((g_lms_mode.integer == 2 || g_lms_mode.integer == 3) && level.roundNumber == level.roundNumberStarted)
					LMSpoint();	
                                //Sago: This is really bad
                                //TODO: Try not to make people spectators here
				ent->client->sess.spectatorState = PM_SPECTATOR;
                                //We have to force spawn imidiantly to prevent lag.
                                ClientSpawn(ent);
			}
			return;
		}
	}

	if((g_gametype.integer==GT_ELIMINATION || g_gametype.integer==GT_CTF_ELIMINATION || g_gametype.integer==GT_LMS) 
			&& ent->client->ps.pm_type == PM_SPECTATOR && ent->client->ps.stats[STAT_HEALTH] > 0)
		return;
		ClientSpawn(ent);

		// add a teleportation effect
		if(g_gametype.integer!=GT_ELIMINATION && g_gametype.integer!=GT_CTF_ELIMINATION && g_gametype.integer!=GT_LMS)
		{	
			tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
			tent->s.clientNum = ent->s.clientNum;
		}
}

/*
================
respawnRound
================
*/
void respawnRound( gentity_t *ent ) {
	gentity_t	*tent;

	//if(g_gametype.integer!=GT_ELIMINATION || !ent->client->isEliminated)
	//{
	//	ent->client->isEliminated  = qtrue;
		//CopyToBodyQue (ent);
	//}
        

	//if(g_gametype.integer==GT_ELIMINATION && ent->client->ps.pm_type == PM_SPECTATOR && ent->client->ps.stats[STAT_HEALTH] > 0)
	//	return;
        if(ent->client->hook)
                Weapon_HookFree(ent->client->hook);

        trap_UnlinkEntity (ent);

	ClientSpawn(ent);

        // add a teleportation effect
        if(g_gametype.integer!=GT_ELIMINATION && g_gametype.integer!=GT_CTF_ELIMINATION && g_gametype.integer!=GT_LMS)
        {
                tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
                tent->s.clientNum = ent->s.clientNum;
        }
}

/*
================
TeamCvarSet

Sets the red and blue team client number cvars.
================
 */
void TeamCvarSet( void )
{
    int i;
    qboolean redChanged = qfalse;
    qboolean blueChanged = qfalse;
    qboolean first = qtrue;
    char* temp = NULL;

    for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
                    if(first) {
                        temp = va("%i",i);
                        first = qfalse;
                    }
                    else
                        temp = va("%s,%i",temp,i);
		}
	}

    if(Q_stricmp(g_redTeamClientNumbers.string,temp))
        redChanged = qtrue;
    trap_Cvar_Set("g_redTeamClientNumbers",temp); //Set it right
    first= qtrue;

    for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
                    if(first) {
                        temp = va("%i",i);
                        first = qfalse;
                    }
                    else
                        temp = va("%s,%i",temp,i);
		}
	}
    if(Q_stricmp(g_blueTeamClientNumbers.string,temp))
        blueChanged = qtrue;
    trap_Cvar_Set("g_blueTeamClientNumbers",temp);

    //Note: We need to force update of the cvar or SendYourTeamMessage will send the old cvar value!
    if(redChanged) {
        trap_Cvar_Update(&g_redTeamClientNumbers); //Force update of CVAR
        SendYourTeamMessageToTeam(TEAM_RED);
    }
    if(blueChanged) {
        trap_Cvar_Update(&g_blueTeamClientNumbers);
        SendYourTeamMessageToTeam(TEAM_BLUE); //Force update of CVAR
    }
}

/*
================
TeamCount

Returns number of players on a team
================
*/
team_t TeamCount( int ignoreClientNum, int team ) {
	int		i;
	int		count = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}

                if ( level.clients[i].pers.connected == CON_CONNECTING) {
                        continue;
                }

		if ( level.clients[i].sess.sessionTeam == team ) {
			count++;
		}
	}

	return count;
}

/*
================
TeamLivingCount

Returns number of living players on a team
================
*/
team_t TeamLivingCount( int ignoreClientNum, int team ) {
	int		i;
	int		count = 0;
	qboolean	LMS = (g_gametype.integer==GT_LMS);

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}

                if ( level.clients[i].pers.connected == CON_CONNECTING) {
                        continue;
                }
		//crash if g_gametype.integer is used here, why?
		if ( level.clients[i].sess.sessionTeam == team && (level.clients[i].ps.stats[STAT_HEALTH]>0 || LMS) && !(level.clients[i].isEliminated)) {
			count++;
		}
	}

	return count;
}

/*
================
TeamHealthCount

Count total number of healthpoints on teh teams used for draws in Elimination
================
*/

team_t TeamHealthCount(int ignoreClientNum, int team ) {
	int 		i;
	int 		count = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}

                if ( level.clients[i].pers.connected == CON_CONNECTING) {
                        continue;
                }

		//only count clients with positive health
		if ( level.clients[i].sess.sessionTeam == team && (level.clients[i].ps.stats[STAT_HEALTH]>0)&& !(level.clients[i].isEliminated)) {
			count+=level.clients[i].ps.stats[STAT_HEALTH];
		}
	}

	return count;
}


/*
================
RespawnAll

Forces all clients to respawn.
================
*/

void RespawnAll(void)
{
	int i;
	gentity_t	*client;
	for(i=0;i<level.maxclients;i++)
	{
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}

                if ( level.clients[i].pers.connected == CON_CONNECTING) {
                        continue;
                }

		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		client = g_entities + i;
		client->client->ps.pm_type = PM_NORMAL;
		client->client->pers.livesLeft = g_lms_lives.integer;
		respawnRound(client);
	}
	return;
}

/*
================
RespawnDead

Forces all *DEAD* clients to respawn.
================
*/

void RespawnDead(void)
{
	int i;
	gentity_t	*client;
	for(i=0;i<level.maxclients;i++)
	{
		
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
                if ( level.clients[i].pers.connected == CON_CONNECTING) {
                        continue;
                }
                client = g_entities + i;
                client->client->pers.livesLeft = g_lms_lives.integer-1;
		if ( level.clients[i].isEliminated == qfalse ){
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		
		client->client->pers.livesLeft = g_lms_lives.integer;
                
		respawnRound(client);
	}
	return;
}

/*
================
DisableWeapons

disables all weapons
================
*/

void DisableWeapons(void)
{
	int i;
	gentity_t	*client;
	for(i=0;i<level.maxclients;i++)
	{
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
                if ( level.clients[i].pers.connected == CON_CONNECTING) {
                        continue;
                }

		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		client = g_entities + i;
		client->client->ps.pm_flags |= PMF_ELIMWARMUP;
	}
        ProximityMine_RemoveAll(); //Remove all the prox mines
	return;
}

/*
================
EnableWeapons

enables all weapons
================
*/

void EnableWeapons(void)
{
	int i;
	gentity_t	*client;
	for(i=0;i<level.maxclients;i++)
	{
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}

		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		/*if ( level.clients[i].isEliminated == qtrue ){
			continue;
		}*/

		client = g_entities + i;
		client->client->ps.pm_flags &= ~PMF_ELIMWARMUP;
	}
	return;
}

/*
================
LMSpoint

Gives a point to the lucky survivor
================
*/

void LMSpoint(void)
{
	int i;
	gentity_t	*client;
	for(i=0;i<level.maxclients;i++)
	{
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}

		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}

		if ( level.clients[i].isEliminated ){
			continue;
		}
		
		client = g_entities + i;
		/*
		Not good in mode 2 & 3
		if ( client->health <= 0 ){
			continue;
		}
		*/
	
		client->client->ps.persistant[PERS_SCORE] += 1;
                        G_LogPrintf("PlayerScore: %i %i: %s now has %d points\n",
		i, client->client->ps.persistant[PERS_SCORE], client->client->pers.netname, client->client->ps.persistant[PERS_SCORE] );
	}
	
	CalculateRanks();
	return;
}

/*
================
TeamLeader

Returns the client number of the team leader
================
*/
int TeamLeader( int team ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == team ) {
			if ( level.clients[i].sess.teamLeader )
				return i;
		}
	}

	return -1;
}


/*
================
PickTeam

================
*/
team_t PickTeam( int ignoreClientNum ) {
	int		counts[TEAM_NUM_TEAMS];

	counts[TEAM_BLUE] = TeamCount( ignoreClientNum, TEAM_BLUE );
	counts[TEAM_RED] = TeamCount( ignoreClientNum, TEAM_RED );
    
    //KK-OAX Both Teams locked...forget about it, print an error message, keep as spec
    if ( level.RedTeamLocked && level.BlueTeamLocked ) {
        G_Printf( "Both teams have been locked by the Admin! \n" );
        return TEAM_NONE;
    }	
	if ( ( counts[TEAM_BLUE] > counts[TEAM_RED] ) && ( !level.RedTeamLocked ) ) {
		return TEAM_RED;
	}
	if ( ( counts[TEAM_RED] > counts[TEAM_BLUE] ) && ( !level.BlueTeamLocked ) ) {
		return TEAM_BLUE;
	}
	// equal team count, so join the team with the lowest score
	if ( ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] ) && ( !level.RedTeamLocked ) ) {
		return TEAM_RED;
	}
	if ( ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) && ( !level.BlueTeamLocked ) ) {  
	    return TEAM_BLUE;
    }
    //KK-OAX Force Team Blue?
    return TEAM_BLUE;
}

/*
===========
ForceClientSkin

Forces a client's skin (for teamplay)
===========
*/
/*
static void ForceClientSkin( gclient_t *client, char *model, const char *skin ) {
	char *p;

	if ((p = strrchr(model, '/')) != 0) {
		*p = 0;
	}

	Q_strcat(model, MAX_QPATH, "/");
	Q_strcat(model, MAX_QPATH, skin);
}
*/

/*
===========
ClientCheckName
============
*/
static void ClientCleanName(const char *in, char *out, int outSize, int clientNum)
{
    int outpos = 0, colorlessLen = 0, spaces = 0, notblack=0;
    qboolean black = qfalse;

    // discard leading spaces
    for(; *in == ' '; in++);

    for(; *in && outpos < outSize - 1; in++)
    {
        out[outpos] = *in;

        if(*in == ' ')
        {
            // don't allow too many consecutive spaces
            if(spaces > 2)
                continue;

            spaces++;
        }
        else if(outpos > 0 && out[outpos - 1] == Q_COLOR_ESCAPE)
        {
            if(Q_IsColorString(&out[outpos - 1]))
            {
                colorlessLen--;

                if(ColorIndex(*in) == 0)
                {
                    // Disallow color black in names to prevent players
                    // from getting advantage playing in front of black backgrounds
                    //outpos--;
                    //continue;
                    black = qtrue;
                }
                else
                    black = qfalse;
            }
            else
            {
                spaces = 0;
                colorlessLen++;
            }
        }
        else
        {
            spaces = 0;
            colorlessLen++;
            if(!black && (Q_isalpha(*in) || (*in>='0' && *in<='9') ) )
                notblack++;
        }

        outpos++;
    }

    out[outpos] = '\0';

    //There was none not-black alphanum chars. Remove all colors
    if(notblack<1)
        Q_CleanStr(out);

    // don't allow empty names
    if( *out == '\0' || colorlessLen == 0)
        Q_strncpyz(out, va("Nameless%i",clientNum), outSize );
}



/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum ) {
	gentity_t *ent;
	int		teamTask, teamLeader, team, health;
	char	*s;
	char	model[MAX_QPATH];
	char	headModel[MAX_QPATH];
	char	oldname[MAX_STRING_CHARS];
	//KK-OAX
	char        err[MAX_STRING_CHARS];
	qboolean    revertName = qfalse;
	
	gclient_t	*client;
	char	c1[MAX_INFO_STRING];
	char	c2[MAX_INFO_STRING];
	char	redTeam[MAX_INFO_STRING];
	char	blueTeam[MAX_INFO_STRING];
	char	userinfo[MAX_INFO_STRING];

	ent = g_entities + clientNum;
	client = ent->client;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// check for malformed or illegal info strings
	if ( !Info_Validate(userinfo) ) {
		strcpy (userinfo, "\\name\\badinfo");
	}

	// check for local client
	s = Info_ValueForKey( userinfo, "ip" );
	if ( !strcmp( s, "localhost" ) ) {
		client->pers.localClient = qtrue;
	}

	// check the item prediction
	s = Info_ValueForKey( userinfo, "cg_predictItems" );
	if ( !atoi( s ) ) {
		client->pers.predictItemPickup = qfalse;
	} else {
		client->pers.predictItemPickup = qtrue;
	}

//unlagged - client options
	// see if the player has opted out
	s = Info_ValueForKey( userinfo, "cg_delag" );
	if ( !atoi( s ) ) {
		client->pers.delag = 0;
	} else {
		client->pers.delag = atoi( s );
	}

	// see if the player is nudging his shots
	s = Info_ValueForKey( userinfo, "cg_cmdTimeNudge" );
	client->pers.cmdTimeNudge = atoi( s );

	// see if the player wants to debug the backward reconciliation
	/*s = Info_ValueForKey( userinfo, "cg_debugDelag" );
	if ( !atoi( s ) ) {
		client->pers.debugDelag = qfalse;
	}
	else {
		client->pers.debugDelag = qtrue;
	}*/

	// see if the player is simulating incoming latency
	//s = Info_ValueForKey( userinfo, "cg_latentSnaps" );
	//client->pers.latentSnaps = atoi( s );

	// see if the player is simulating outgoing latency
	//s = Info_ValueForKey( userinfo, "cg_latentCmds" );
	//client->pers.latentCmds = atoi( s );

	// see if the player is simulating outgoing packet loss
	//s = Info_ValueForKey( userinfo, "cg_plOut" );
	//client->pers.plOut = atoi( s );
//unlagged - client options

	// set name
	Q_strncpyz ( oldname, client->pers.netname, sizeof( oldname ) );
	s = Info_ValueForKey (userinfo, "name");
	ClientCleanName( s, client->pers.netname, sizeof(client->pers.netname), clientNum );

    //KK-OAPub Added From Tremulous-Control Name Changes
    if( strcmp( oldname, client->pers.netname ) )
    {
        if( client->pers.nameChangeTime &&
            ( level.time - client->pers.nameChangeTime )
            <= ( g_minNameChangePeriod.value * 1000 ) )
        {
            trap_SendServerCommand( ent - g_entities, va(
            "print \"Name change spam protection (g_minNameChangePeriod = %d)\n\"",
            g_minNameChangePeriod.integer ) );
            revertName = qtrue;
        }
        else if( g_maxNameChanges.integer > 0
            && client->pers.nameChanges >= g_maxNameChanges.integer  )
        {
            trap_SendServerCommand( ent - g_entities, va(
                "print \"Maximum name changes reached (g_maxNameChanges = %d)\n\"",
                g_maxNameChanges.integer ) );
            revertName = qtrue;
        }
        else if( client->pers.muted )
        {
            trap_SendServerCommand( ent - g_entities,
                "print \"You cannot change your name while you are muted\n\"" );
            revertName = qtrue;
        }
        else if( !G_admin_name_check( ent, client->pers.netname, err, sizeof( err ) ) )
        {
            trap_SendServerCommand( ent - g_entities, va( "print \"%s\n\"", err ) );
            revertName = qtrue;
        }

        //Never revert a bots name... just to bad if it hapens... but the bot will always be expendeble :-)
        if (ent->r.svFlags & SVF_BOT)
            revertName = qfalse;

        if( revertName )
        {
            Q_strncpyz( client->pers.netname, *oldname ? oldname : "UnnamedPlayer",
                sizeof( client->pers.netname ) );
            Info_SetValueForKey( userinfo, "name", oldname );
            trap_SetUserinfo( clientNum, userinfo );
        }
        else
        {
            if( client->pers.connected == CON_CONNECTED )
            {
                client->pers.nameChangeTime = level.time;
                client->pers.nameChanges++;
            }
        }
    }
	// N_G: this condition makes no sense to me and I'm not going to
	// try finding out what it means, I've added parentheses according to
	// evaluation rules of the original code so grab a
	// parentheses pairing highlighting text editor and see for yourself
	// if you got it right
	//Sago: One redundant check and CTF Elim and LMS was missing. Not an important function and I might never have noticed, should properly be ||
	if ( ( ( client->sess.sessionTeam == TEAM_SPECTATOR ) ||
		( ( ( client->isEliminated ) /*||
		( client->ps.pm_type == PM_SPECTATOR )*/ ) &&   //Sago: If this is true client.isEliminated or TEAM_SPECTATOR is true to and this is redundant
		( g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) ) ) &&
		( client->sess.spectatorState == SPECTATOR_SCOREBOARD ) ) {

		Q_strncpyz( client->pers.netname, "scoreboard", sizeof(client->pers.netname) );
	}

	if ( client->pers.connected == CON_CONNECTED ) {
		if ( strcmp( oldname, client->pers.netname ) ) {
			trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " renamed to %s\n\"", oldname, 
				client->pers.netname) );
		}
	}

	// set max health
	if (client->ps.powerups[PW_GUARD]) {
		client->pers.maxHealth = 200;
	} else {
		health = atoi( Info_ValueForKey( userinfo, "handicap" ) );
		client->pers.maxHealth = health;
		if ( client->pers.maxHealth < 1 || client->pers.maxHealth > 100 ) {
			client->pers.maxHealth = 100;
		}
	}
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;

	// set model
	if( g_gametype.integer >= GT_TEAM && g_ffa_gt==0) {
		Q_strncpyz( model, Info_ValueForKey (userinfo, "team_model"), sizeof( model ) );
		Q_strncpyz( headModel, Info_ValueForKey (userinfo, "team_headmodel"), sizeof( headModel ) );
	} else {
		Q_strncpyz( model, Info_ValueForKey (userinfo, "model"), sizeof( model ) );
		Q_strncpyz( headModel, Info_ValueForKey (userinfo, "headmodel"), sizeof( headModel ) );
	}

	// bots set their team a few frames later
	if (g_gametype.integer >= GT_TEAM && g_ffa_gt==0 && g_entities[clientNum].r.svFlags & SVF_BOT) {
		s = Info_ValueForKey( userinfo, "team" );
		if ( !Q_stricmp( s, "red" ) || !Q_stricmp( s, "r" ) ) {
			team = TEAM_RED;
		} else if ( !Q_stricmp( s, "blue" ) || !Q_stricmp( s, "b" ) ) {
			team = TEAM_BLUE;
		} else {
			// pick the team with the least number of players
			team = PickTeam( clientNum );
		}
                client->sess.sessionTeam = team;
	}
	else {
		team = client->sess.sessionTeam;
	}

/*	NOTE: all client side now
Sago: I am not happy with this exception
 
	// team
	switch( team ) {
	case TEAM_RED:
		ForceClientSkin(client, model, "red");
//		ForceClientSkin(client, headModel, "red");
		break;
	case TEAM_BLUE:
		ForceClientSkin(client, model, "blue");
//		ForceClientSkin(client, headModel, "blue");
		break;
	}
	// don't ever use a default skin in teamplay, it would just waste memory
	// however bots will always join a team but they spawn in as spectator
	if ( g_gametype.integer >= GT_TEAM && team == TEAM_SPECTATOR) {
		ForceClientSkin(client, model, "red");
//		ForceClientSkin(client, headModel, "red");
	}
*/

	if (g_gametype.integer >= GT_TEAM && g_ffa_gt!=1) {
		client->pers.teamInfo = qtrue;
	} else {
		s = Info_ValueForKey( userinfo, "teamoverlay" );
		if ( ! *s || atoi( s ) != 0 ) {
			client->pers.teamInfo = qtrue;
		} else {
			client->pers.teamInfo = qfalse;
		}
	}
	/*
	s = Info_ValueForKey( userinfo, "cg_pmove_fixed" );
	if ( !*s || atoi( s ) == 0 ) {
		client->pers.pmoveFixed = qfalse;
	}
	else {
		client->pers.pmoveFixed = qtrue;
	}
	*/

	// team task (0 = none, 1 = offence, 2 = defence)
	teamTask = atoi(Info_ValueForKey(userinfo, "teamtask"));
	// team Leader (1 = leader, 0 is normal player)
	teamLeader = client->sess.teamLeader;

	// colors
        if( g_gametype.integer >= GT_TEAM && g_ffa_gt==0 && g_instantgib.integer) {
            switch(team) {
                case TEAM_RED:
                    c1[0] = COLOR_BLUE;
                    c2[0] = COLOR_BLUE;
                    c1[1] = 0;
                    c2[1] = 0;
                    break;
                case TEAM_BLUE:
                    c1[0] = COLOR_RED;
                    c2[0] = COLOR_RED;
                    c1[1] = 0;
                    c2[1] = 0;
                    break;
                default:
                    break;
            }
        } else {
            strcpy(c1, Info_ValueForKey( userinfo, "color1" ));
            strcpy(c2, Info_ValueForKey( userinfo, "color2" ));
        }

	strcpy(redTeam, Info_ValueForKey( userinfo, "g_redteam" ));
	strcpy(blueTeam, Info_ValueForKey( userinfo, "g_blueteam" ));

	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds
	if ( ent->r.svFlags & SVF_BOT ) {
		s = va("n\\%s\\t\\%i\\model\\%s\\hmodel\\%s\\c1\\%s\\c2\\%s\\hc\\%i\\w\\%i\\l\\%i\\skill\\%s\\tt\\%d\\tl\\%d",
			client->pers.netname, team, model, headModel, c1, c2, 
			client->pers.maxHealth, client->sess.wins, client->sess.losses,
			Info_ValueForKey( userinfo, "skill" ), teamTask, teamLeader );
	} else {
		s = va("n\\%s\\t\\%i\\model\\%s\\hmodel\\%s\\g_redteam\\%s\\g_blueteam\\%s\\c1\\%s\\c2\\%s\\hc\\%i\\w\\%i\\l\\%i\\tt\\%d\\tl\\%d",
			client->pers.netname, client->sess.sessionTeam, model, headModel, redTeam, blueTeam, c1, c2, 
			client->pers.maxHealth, client->sess.wins, client->sess.losses, teamTask, teamLeader);
	}

	trap_SetConfigstring( CS_PLAYERS+clientNum, s );

	// this is not the userinfo, more like the configstring actually
	G_LogPrintf( "ClientUserinfoChanged: %i %s\\id\\%s\n", clientNum, s, Info_ValueForKey(userinfo, "cl_guid") );
}


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or tournement restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and tournement
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot ) {
	char		*value;
//	char		*areabits;
	gclient_t	*client;
	char		userinfo[MAX_INFO_STRING];
	gentity_t	*ent;
	char        reason[ MAX_STRING_CHARS ] = {""};
	int         i;
    
    //KK-OAX I moved these up so userinfo could be assigned/used. 
	ent = &g_entities[ clientNum ];
	client = &level.clients[ clientNum ];
	ent->client = client;
	memset( client, 0, sizeof(*client) );

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

 	value = Info_ValueForKey( userinfo, "cl_guid" );
 	Q_strncpyz( client->pers.guid, value, sizeof( client->pers.guid ) );
 	

 	// IP filtering //KK-OAX Has this been obsoleted? 
 	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=500
 	// recommanding PB based IP / GUID banning, the builtin system is pretty limited
 	// check to see if they are on the banned IP list
	value = Info_ValueForKey (userinfo, "ip");
	Q_strncpyz( client->pers.ip, value, sizeof( client->pers.ip ) );
	
	if ( G_FilterPacket( value ) && !Q_stricmp(value,"localhost") ) {
            G_Printf("Player with IP: %s is banned\n",value);
		return "You are banned from this server.";
	}
	
    if( G_admin_ban_check( userinfo, reason, sizeof( reason ) ) ) {    
 	    return va( "%s", reason );
 	}
 	 
  //KK-OAX
  // we don't check GUID or password for bots and local client
  // NOTE: local client <-> "ip" "localhost"
  //   this means this client is not running in our current process
	if ( !isBot && (strcmp(value, "localhost") != 0)) {
		// check for a password
		value = Info_ValueForKey (userinfo, "password");
		if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) &&
			strcmp( g_password.string, value) != 0) {
			return "Invalid password";
		}
		for( i = 0; i < sizeof( client->pers.guid ) - 1 &&
		    isxdigit( client->pers.guid[ i ] ); i++ );
		if( i < sizeof( client->pers.guid ) - 1 )
		    return "Invalid GUID";
		    
		for( i = 0; i < level.maxclients; i++ ) {
		
		    if( level.clients[ i ].pers.connected == CON_DISCONNECTED )
		        continue;
		        
		    if( !Q_stricmp( client->pers.guid, level.clients[ i ].pers.guid ) ) {
		        if( !G_ClientIsLagging( level.clients + i ) ) {
		            trap_SendServerCommand( i, "cp \"Your GUID is not secure\"" );
		                return "Duplicate GUID";
		        }
		        trap_DropClient( i, "Ghost" );
		    }
		}
		    
	}

    //Check for local client
    if( !strcmp( client->pers.ip, "localhost" ) )
        client->pers.localClient = qtrue;
        client->pers.adminLevel = G_admin_level( ent );

	client->pers.connected = CON_CONNECTING;

	// read or initialize the session data
	if ( firstTime || level.newSession ) {
		G_InitSessionData( client, userinfo );
	}
	G_ReadSessionData( client );

	if( isBot ) {
		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;
		if( !G_BotConnect( clientNum, !firstTime ) ) {
			return "BotConnectfailed";
		}
	}

	//KK-OAX Swapped these in order...seemed to help the connection process.
	// get and distribute relevent paramters
	ClientUserinfoChanged( clientNum );
	G_LogPrintf( "ClientConnect: %i\n", clientNum );
	

	// don't do the "xxx connected" messages if they were caried over from previous level
	if ( firstTime ) {
		trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " connected\n\"", client->pers.netname) );
	}

	if ( g_gametype.integer >= GT_TEAM &&
		client->sess.sessionTeam != TEAM_SPECTATOR ) {
		BroadcastTeamChange( client, -1 );
	}

	// count current clients and rank for scoreboard
	CalculateRanks();

	// for statistics
//	client->areabits = areabits;
//	if ( !client->areabits )
//		client->areabits = G_Alloc( (trap_AAS_PointReachabilityAreaIndex( NULL ) + 7) / 8 );

//Sago: Changed the message
//unlagged - backward reconciliation #5
	// announce it
	if ( g_delagHitscan.integer ) {
		trap_SendServerCommand( clientNum, "print \"Full lag compensation is ON!\n\"" );
	}
	else {
		trap_SendServerCommand( clientNum, "print \"Full lag compensation is OFF!\n\"" );
	}

//unlagged - backward reconciliation #5
    G_admin_namelog_update( client, qfalse );
	return NULL;
}

void motd (gentity_t *ent)
{
	char motd[1024];
	fileHandle_t motdFile;
	int motdLen;
	int fileLen;

	strcpy (motd, "cp \"");
	fileLen = trap_FS_FOpenFile(g_motdfile.string, &motdFile, FS_READ);
	if(motdFile)
	{
		char * p;

		motdLen = strlen(motd);
		if((motdLen + fileLen) > (sizeof(motd) - 2))
			fileLen = (sizeof(motd) - 2 - motdLen);
		trap_FS_Read(motd + motdLen, fileLen, motdFile);
		motd[motdLen + fileLen] = '"';
		motd[motdLen + fileLen + 1] = 0;
		trap_FS_FCloseFile(motdFile);

		while((p = strchr(motd, '\r'))) //Remove carrier return. 0x0D
		memmove(p, p + 1, motdLen + fileLen - (p - motd));
	}
	trap_SendServerCommand(ent - g_entities, motd);
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum ) {
	gentity_t	*ent;
	gclient_t	*client;
	gentity_t       *tent;
	int			flags;
	int		countRed, countBlue, countFree;
        char		userinfo[MAX_INFO_STRING];

        trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	ent = g_entities + clientNum;

	client = level.clients + clientNum;

	if ( ent->r.linked ) {
		trap_UnlinkEntity( ent );
	}
	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	client->pers.connected = CON_CONNECTED;
	client->pers.enterTime = level.time;
	client->pers.teamState.state = TEAM_BEGIN;

	//Elimination:
	client->pers.roundReached = 0; //We will spawn in next round
	if(g_gametype.integer == GT_LMS) {
		client->isEliminated = qtrue; //So player does not give a point in gamemode 2 and 3
		//trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " will start dead\n\"", client->pers.netname) );
	}

	//player is a bot:
	if( ent->r.svFlags & SVF_BOT )
	{
		if(!level.hadBots)
		{
			G_LogPrintf( "Info: There has been at least 1 bot now\n" );
			level.hadBots = qtrue;
		}
	}

	//Count smallest team
	countFree = TeamCount(-1,TEAM_FREE);
	countRed = TeamCount(-1,TEAM_RED);
	countBlue = TeamCount(-1,TEAM_BLUE);
	if(g_gametype.integer < GT_TEAM || g_ffa_gt)
	{
		if(countFree>level.teamSize)
			level.teamSize=countFree;
	}
	else
	if(countRed>countBlue)
	{
		if(countBlue>level.teamSize)
			level.teamSize=countBlue;
	}
	else
	{
		if(countRed>level.teamSize)
			level.teamSize=countRed;
	}

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	flags = client->ps.eFlags;
	memset( &client->ps, 0, sizeof( client->ps ) );
        if( client->sess.sessionTeam != TEAM_SPECTATOR )
            PlayerStore_restore(Info_ValueForKey(userinfo,"cl_guid"),&(client->ps));
	client->ps.eFlags = flags;

	// locate ent at a spawn point
	ClientSpawn( ent );

	if( ( client->sess.sessionTeam != TEAM_SPECTATOR ) &&
		( ( !( client->isEliminated ) /*&&
		( ( !client->ps.pm_type ) == PM_SPECTATOR ) */ ) || //Sago: Yes, it made no sense 
		( ( g_gametype.integer != GT_ELIMINATION || level.intermissiontime) &&
		( g_gametype.integer != GT_CTF_ELIMINATION || level.intermissiontime) &&
		( g_gametype.integer != GT_LMS || level.intermissiontime ) ) ) ) {
		// send event
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
		tent->s.clientNum = ent->s.clientNum;

		if ( g_gametype.integer != GT_TOURNAMENT  ) {
			trap_SendServerCommand( -1, va("print \"%s" S_COLOR_WHITE " entered the game\n\"", client->pers.netname) );
		}
	}
        
        motd ( ent );
        
	G_LogPrintf( "ClientBegin: %i\n", clientNum );

	//Send domination point names:
	if(g_gametype.integer == GT_DOMINATION) {
		DominationPointNamesMessage(ent);
		DominationPointStatusMessage(ent);
	}

        TeamCvarSet();

	// count current clients and rank for scoreboard
	CalculateRanks();

        //Send the list of custom vote options:
        if(strlen(custom_vote_info))
            SendCustomVoteCommands(clientNum);
}

/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn(gentity_t *ent) {
	int		index;
	vec3_t	spawn_origin, spawn_angles;
	gclient_t	*client;
	int		i;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	int		persistant[MAX_PERSISTANT];
	gentity_t	*spawnPoint;
	//gentity_t *tent;
	int		flags;
	int		savedPing;
//	char	*savedAreaBits;
	int		accuracy_hits, accuracy_shots,vote;
        int		accuracy[WP_NUM_WEAPONS][2];
	int		eventSequence;
	char	userinfo[MAX_INFO_STRING];

	index = ent - g_entities;
	client = ent->client;

	//In Elimination the player should not spawn if he have already spawned in the round (but not for spectators)
	// N_G: You've obviously wanted something ELSE
	//Sago: Yes, the !level.intermissiontime is currently redundant but it might still be the bast place to make the test, CheckElimination in g_main makes sure the next if will fail and the rest of the things this block does might not affect if in Intermission (I'll just test that)
	if( 
	( 
		( 
			g_gametype.integer == GT_ELIMINATION ||
			g_gametype.integer == GT_CTF_ELIMINATION || (g_gametype.integer == GT_LMS && client->isEliminated)) &&
			(!level.intermissiontime || level.warmupTime != 0)
		) &&
		( client->sess.sessionTeam != TEAM_SPECTATOR ) 
	)
	{
		// N_G: Another condition that makes no sense to me, see for
		// yourself if you really meant this
		// Sago: I beleive the TeamCount is to make sure people can join even if the game can't start
		if( ( level.roundNumber == level.roundNumberStarted ) ||
			( (level.time < level.roundStartTime - g_elimination_activewarmup.integer*1000 ) &&
			TeamCount( -1, TEAM_BLUE ) &&
			TeamCount( -1, TEAM_RED )  ) )
		{	
			client->sess.spectatorState = SPECTATOR_FREE;
			client->isEliminated = qtrue;
                        if(g_gametype.integer == GT_LMS)
                                G_LogPrintf( "LMS: %i %i %i: Player \"%s^7\" eliminated!\n", level.roundNumber, index, 1, client->pers.netname );
			client->ps.pm_type = PM_SPECTATOR;
                        CalculateRanks();
			return;
		}
		else
		{
			client->pers.roundReached = level.roundNumber+1;
			client->sess.spectatorState = SPECTATOR_NOT;
			client->ps.pm_type = PM_NORMAL;
			client->isEliminated = qfalse;
                        CalculateRanks();
		}
	} else {
            //Force false.
            if(client->isEliminated) {
                client->isEliminated = qfalse;
                CalculateRanks();
            }
        }

	if(g_gametype.integer == GT_LMS && client->sess.sessionTeam != TEAM_SPECTATOR && (!level.intermissiontime || level.warmupTime != 0))
	{
		if(level.roundNumber==level.roundNumberStarted /*|| level.time<level.roundStartTime-g_elimination_activewarmup.integer*1000*/ && 1>client->pers.livesLeft)
		{	
			client->sess.spectatorState = SPECTATOR_FREE;
			if( ent->client->isEliminated!=qtrue) {
				client->isEliminated = qtrue;
				if((g_lms_mode.integer == 2 || g_lms_mode.integer == 3) && level.roundNumber == level.roundNumberStarted)
					LMSpoint();
                                G_LogPrintf( "LMS: %i %i %i: Player \"%s^7\" eliminated!\n", level.roundNumber, index, 1, client->pers.netname );
			}
			client->ps.pm_type = PM_SPECTATOR;
			return;
		}
		
		client->sess.spectatorState = SPECTATOR_NOT;
		client->ps.pm_type = PM_NORMAL;
		client->isEliminated = qfalse;
		if(client->pers.livesLeft>0)
			client->pers.livesLeft--;
	}

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client
	if ((client->sess.sessionTeam == TEAM_SPECTATOR) 
			|| ( (client->ps.pm_type == PM_SPECTATOR || client->isEliminated )  && (g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION) ) ) {
		spawnPoint = SelectSpectatorSpawnPoint ( spawn_origin, spawn_angles);
	} else if (g_gametype.integer == GT_DOUBLE_D) {
		//Double Domination uses special spawn points:
		spawnPoint = SelectDoubleDominationSpawnPoint (client->sess.sessionTeam, spawn_origin, spawn_angles);
	} else if (g_gametype.integer >= GT_CTF && g_ffa_gt==0 && g_gametype.integer!= GT_DOMINATION) {
		// all base oriented team games use the CTF spawn points
		spawnPoint = SelectCTFSpawnPoint ( 
						client->sess.sessionTeam, 
						client->pers.teamState.state, 
						spawn_origin, spawn_angles);
	} else {
		do {
			// the first spawn should be at a good looking spot
			if ( !client->pers.initialSpawn && client->pers.localClient ) {
				client->pers.initialSpawn = qtrue;
				spawnPoint = SelectInitialSpawnPoint( spawn_origin, spawn_angles );
			} else {
				// don't spawn near existing origin if possible
				spawnPoint = SelectSpawnPoint ( 
					client->ps.origin, 
					spawn_origin, spawn_angles);
			}

			// Tim needs to prevent bots from spawning at the initial point
			// on q3dm0...
			if ( ( spawnPoint->flags & FL_NO_BOTS ) && ( ent->r.svFlags & SVF_BOT ) ) {
                            //Sago: The game has a tendency to select the furtest spawn point
                            //This is a problem if the fursest spawnpoint keeps being NO_BOTS and it does!
                            //This is a hot fix that seeks a spawn point faraway from the the currently found one
                            vec3_t old_origin;
                            VectorCopy(spawn_origin,old_origin);
                            spawnPoint = SelectSpawnPoint (old_origin, spawn_origin, spawn_angles);
                            if ( ( spawnPoint->flags & FL_NO_BOTS ) && ( ent->r.svFlags & SVF_BOT ) ) {
				continue;	// try again
                            }
			}
			// just to be symetric, we have a nohumans option...
			if ( ( spawnPoint->flags & FL_NO_HUMANS ) && !( ent->r.svFlags & SVF_BOT ) ) {
				continue;	// try again
			}

			break;

		} while ( 1 );
	}
	client->pers.teamState.state = TEAM_ACTIVE;

	// always clear the kamikaze flag
	ent->s.eFlags &= ~EF_KAMIKAZE;

	// toggle the teleport bit so the client knows to not lerp
	// and never clear the voted flag
	flags = ent->client->ps.eFlags & (EF_TELEPORT_BIT | EF_VOTED | EF_TEAMVOTED);
	flags ^= EF_TELEPORT_BIT;

//unlagged - backward reconciliation #3
	// we don't want players being backward-reconciled to the place they died
	G_ResetHistory( ent );
	// and this is as good a time as any to clear the saved state
	ent->client->saved.leveltime = 0;
//unlagged - backward reconciliation #3

	// clear everything but the persistant data

	saved = client->pers;
	savedSess = client->sess;
	savedPing = client->ps.ping;
	vote = client->vote;
//	savedAreaBits = client->areabits;
	accuracy_hits = client->accuracy_hits;
	accuracy_shots = client->accuracy_shots;
    memcpy(accuracy,client->accuracy,sizeof(accuracy));

    memcpy(persistant,client->ps.persistant,MAX_PERSISTANT*sizeof(int));
	eventSequence = client->ps.eventSequence;

	Com_Memset (client, 0, sizeof(*client));

	client->pers = saved;
	client->sess = savedSess;
	client->ps.ping = savedPing;
	client->vote = vote;
//	client->areabits = savedAreaBits;
	client->accuracy_hits = accuracy_hits;
	client->accuracy_shots = accuracy_shots;
        for( i = 0 ; i < WP_NUM_WEAPONS ; i++ ){
		client->accuracy[i][0] = accuracy[i][0];
		client->accuracy[i][1] = accuracy[i][1];
	}

	client->lastkilled_client = -1;

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		client->ps.persistant[i] = persistant[i];
	}
	client->ps.eventSequence = eventSequence;
	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;

	client->airOutTime = level.time + 12000;

	trap_GetUserinfo( index, userinfo, sizeof(userinfo) );
	// set max health
	client->pers.maxHealth = atoi( Info_ValueForKey( userinfo, "handicap" ) );
	if ( client->pers.maxHealth < 1 || client->pers.maxHealth > 100 ) {
		client->pers.maxHealth = 100;
	}
	// clear entity values
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
	client->ps.eFlags = flags;

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
	ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;
        
        //Sago: No one has hit the client yet!
        client->lastSentFlying = -1;
	
	VectorCopy (playerMins, ent->r.mins);
	VectorCopy (playerMaxs, ent->r.maxs);

	client->ps.clientNum = index;

if(g_gametype.integer != GT_ELIMINATION && g_gametype.integer != GT_CTF_ELIMINATION && g_gametype.integer != GT_LMS && !g_elimination_allgametypes.integer)
{
	client->ps.stats[STAT_WEAPONS] = ( 1 << WP_MACHINEGUN );
	if ( g_gametype.integer == GT_TEAM ) {
		client->ps.ammo[WP_MACHINEGUN] = 50;
	} else {
		client->ps.ammo[WP_MACHINEGUN] = 100;
	}

	client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GAUNTLET );
	client->ps.ammo[WP_GAUNTLET] = -1;
	client->ps.ammo[WP_GRAPPLING_HOOK] = -1;

	// health will count down towards max_health
	ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH] + 25;
}
else
{
	client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GAUNTLET );
	client->ps.ammo[WP_GAUNTLET] = -1;
	client->ps.ammo[WP_GRAPPLING_HOOK] = -1;
	if (g_elimination_machinegun.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_MACHINEGUN );
		client->ps.ammo[WP_MACHINEGUN] = g_elimination_machinegun.integer;
	}
	if (g_elimination_shotgun.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_SHOTGUN );
		client->ps.ammo[WP_SHOTGUN] = g_elimination_shotgun.integer;
	}
	if (g_elimination_grenade.integer > 0) {	
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GRENADE_LAUNCHER );
		client->ps.ammo[WP_GRENADE_LAUNCHER] = g_elimination_grenade.integer;
	}
	if (g_elimination_rocket.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_ROCKET_LAUNCHER );
		client->ps.ammo[WP_ROCKET_LAUNCHER] = g_elimination_rocket.integer;
	}
	if (g_elimination_lightning.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_LIGHTNING );
		client->ps.ammo[WP_LIGHTNING] = g_elimination_lightning.integer;
	}
	if (g_elimination_railgun.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_RAILGUN );
		client->ps.ammo[WP_RAILGUN] = g_elimination_railgun.integer;
	}
	if (g_elimination_plasmagun.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_PLASMAGUN );
		client->ps.ammo[WP_PLASMAGUN] = g_elimination_plasmagun.integer;
	}
	if (g_elimination_bfg.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_BFG );
		client->ps.ammo[WP_BFG] = g_elimination_bfg.integer;
	}
        if (g_elimination_grapple.integer) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GRAPPLING_HOOK );
	}
	if (g_elimination_nail.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NAILGUN );
		client->ps.ammo[WP_NAILGUN] = g_elimination_nail.integer;
	}
	if (g_elimination_mine.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_PROX_LAUNCHER );
		client->ps.ammo[WP_PROX_LAUNCHER] = g_elimination_mine.integer;
	}
	if (g_elimination_chain.integer > 0) {
		client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_CHAINGUN );
		client->ps.ammo[WP_CHAINGUN] = g_elimination_chain.integer;
	}
	
	ent->health = client->ps.stats[STAT_ARMOR] = g_elimination_startArmor.integer; //client->ps.stats[STAT_MAX_HEALTH]*2;
	ent->health = client->ps.stats[STAT_HEALTH] = g_elimination_startHealth.integer; //client->ps.stats[STAT_MAX_HEALTH]*2;	

	
	//	ent->health = client->ps.stats[STAT_HEALTH] = 0;
}
	//Instantgib mode, replace weapons with rail (and maybe gauntlet)
	if(g_instantgib.integer)
	{
		client->ps.stats[STAT_WEAPONS] = ( 1 << WP_RAILGUN );
		client->ps.ammo[WP_RAILGUN] = 999; //Don't display any ammo
		if(g_instantgib.integer>1)
		{
			 client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_GAUNTLET );
	              	client->ps.ammo[WP_GAUNTLET] = -1;
		}
	}

	//nexuiz style rocket arena (rocket launcher only)
	if(g_rockets.integer) 
	{
		client->ps.stats[STAT_WEAPONS] = ( 1 << WP_ROCKET_LAUNCHER );
		client->ps.ammo[WP_ROCKET_LAUNCHER] = 999;
	}

	G_SetOrigin( ent, spawn_origin );
	VectorCopy( spawn_origin, client->ps.origin );

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;
	if(g_gametype.integer==GT_ELIMINATION || g_gametype.integer==GT_CTF_ELIMINATION || g_gametype.integer==GT_LMS)	
		client->ps.pm_flags |= PMF_ELIMWARMUP;

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );
	SetClientViewAngle( ent, spawn_angles );

	if ( (ent->client->sess.sessionTeam == TEAM_SPECTATOR) || ((client->ps.pm_type == PM_SPECTATOR || client->isEliminated) && 
		(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) ) ) {
                //Sago: Lets see if this fixes the bots only bug - loose all point on dead bug. (It didn't)
            /*if(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) {
                G_KillBox( ent );
		trap_LinkEntity (ent);
            }*/
	} else {
		G_KillBox( ent );
		trap_LinkEntity (ent);

		// force the base weapon up
		client->ps.weapon = WP_MACHINEGUN;
		client->ps.weaponstate = WEAPON_READY;

	}

	// don't allow full run speed for a bit
	client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	client->ps.pm_time = 100;

	client->respawnTime = level.time;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;

	// set default animations
	client->ps.torsoAnim = TORSO_STAND;
	client->ps.legsAnim = LEGS_IDLE;

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	} else {
		// fire the targets of the spawn point
		G_UseTargets( spawnPoint, ent );

		// select the highest weapon number available, after any
		// spawn given items have fired
		client->ps.weapon = 1;
		for ( i = WP_NUM_WEAPONS - 1 ; i > 0 ; i-- ) {
			if ( client->ps.stats[STAT_WEAPONS] & ( 1 << i ) && i !=WP_GRAPPLING_HOOK ) {
				client->ps.weapon = i;
				break;
			}
		}
	}

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( (ent->client->sess.sessionTeam != TEAM_SPECTATOR) || ( (!client->isEliminated || client->ps.pm_type != PM_SPECTATOR)&& 
		(g_gametype.integer == GT_ELIMINATION || g_gametype.integer == GT_CTF_ELIMINATION || g_gametype.integer == GT_LMS) ) ) {
		BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );

        if(g_spawnprotect.integer)
            client->spawnprotected = qtrue;

        RespawnTimeMessage(ent,0);
}


/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum ) {
	gentity_t	*ent;
	int			i;
        char	userinfo[MAX_INFO_STRING];

	// cleanup if we are kicking a bot that
	// hasn't spawned yet
	G_RemoveQueuedBotBegin( clientNum );

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;
	}

        ClientLeaving( clientNum);
    //KK-OAX Admin
    G_admin_namelog_update( ent->client, qtrue );
    
        trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// stop any following clients
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( (level.clients[i].sess.sessionTeam == TEAM_SPECTATOR || level.clients[i].ps.pm_type == PM_SPECTATOR)
			&& level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW
			&& level.clients[i].sess.spectatorClient == clientNum ) {
			StopFollowing( &g_entities[i] );
		}
	}

	// send effect if they were completely connected
        /*
         *Sago: I have removed this. A little dangerous but I make him suicide in a moment.
         */
	/*if ( ent->client->pers.connected == CON_CONNECTED
		&& ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_OUT );
		tent->s.clientNum = ent->s.clientNum;

		// They don't get to take powerups with them!
		// Especially important for stuff like CTF flags
		TossClientItems( ent );
		TossClientPersistantPowerups( ent );
                if( g_gametype.integer == GT_HARVESTER ) {
			TossClientCubes( ent );
		}
//#endif

	}*/

        //Is the player alive?
        i = (ent->client->ps.stats[STAT_HEALTH]>0);
        //Commit suicide!
        if ( ent->client->pers.connected == CON_CONNECTED
		&& ent->client->sess.sessionTeam != TEAM_SPECTATOR && i ) {
                //Prevent a team from loosing point because of player leaving
                int teamscore = 0;
                if(g_gametype.integer == GT_TEAM)
                    teamscore = level.teamScores[ ent->client->sess.sessionTeam ];
		// Kill him (makes sure he loses flags, etc)
		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
		player_die (ent, ent, g_entities + ENTITYNUM_WORLD, 100000, MOD_SUICIDE);
                if(g_gametype.integer == GT_TEAM)
                    level.teamScores[ ent->client->sess.sessionTeam ] = teamscore;
	}



        if ( ent->client->pers.connected == CON_CONNECTED && ent->client->sess.sessionTeam != TEAM_SPECTATOR)
            PlayerStore_store(Info_ValueForKey(userinfo,"cl_guid"),ent->client->ps);

	G_LogPrintf( "ClientDisconnect: %i\n", clientNum );

	// if we are playing in tourney mode and losing, give a win to the other player
	if ( (g_gametype.integer == GT_TOURNAMENT )
		&& !level.intermissiontime
		&& !level.warmupTime && level.sortedClients[1] == clientNum ) {
		level.clients[ level.sortedClients[0] ].sess.wins++;
		ClientUserinfoChanged( level.sortedClients[0] );
	}

	if( g_gametype.integer == GT_TOURNAMENT &&
		ent->client->sess.sessionTeam == TEAM_FREE &&
		level.intermissiontime ) {

		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		level.changemap = NULL;
		level.intermissiontime = 0;
	}

	trap_UnlinkEntity (ent);
	ent->s.modelindex = 0;
	ent->inuse = qfalse;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	ent->client->sess.sessionTeam = TEAM_FREE;

	trap_SetConfigstring( CS_PLAYERS + clientNum, "");

	CalculateRanks();
        CountVotes();

	if ( ent->r.svFlags & SVF_BOT ) {
		BotAIShutdownClient( clientNum, qfalse );
	}
}



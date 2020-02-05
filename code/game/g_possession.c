/*
===========================================================================
Copyright (C) 2016 Poul Sander

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


static gentity_t	*possessionFlag;

static void Possession_create_neutral_obelisk( gentity_t *target ) {
	gitem_t			*it;
	if (possessionFlag) {
		return;
	}
	it = BG_FindItem("Neutral Flag");
	possessionFlag = G_Spawn();
	VectorCopy( target->r.currentOrigin, possessionFlag->s.origin );
	possessionFlag->classname = it->classname;
	G_SpawnItem(possessionFlag, it);
	FinishSpawningItem(possessionFlag );
	Team_SetFlagStatus( TEAM_FREE, FLAG_ATBASE );
}

static qboolean EntityFilterNoBotsOrHumanOnly(const gentity_t* item) {
	if (item->flags & FL_NO_BOTS || item->flags & FL_NO_HUMANS) {
		//We cannot select a nobots or nohuman spawnpoint. These can be outside the map
		return qfalse;
	}
	return qtrue;
}

void Possession_SpawnFlag( void ) {
	gentity_t	*ent = NULL;
	if (possessionFlag) {
		return;
	}
	ent = SelectRandomEntity("team_CTF_neutralflag");
	if (ent) {
		possessionFlag = ent;
		G_LogPrintf("Info: The map contains a white flag. We do not spawn a new one.\n");
		return;
	}
	if (!ent) {
		//If not pick a random domination point
		ent = SelectRandomEntity("domination_point");
	}
	if (!ent) {
		//Else pick a random deathmatch point
		ent = SelectRandomEntityFilter("info_player_deathmatch", EntityFilterNoBotsOrHumanOnly);
	}
	if (!ent) {
		//If nothing found just use the first entity.
		ent = &g_entities[0];
	}
	Possession_create_neutral_obelisk(ent);
}

int Possession_TouchFlag(gentity_t *other) {
	gclient_t *cl = other->client;
	cl->ps.powerups[PW_NEUTRALFLAG] = INT_MAX; // flags never expire
	cl->pers.teamState.flagsince = level.time;
	G_LogPrintf("POS: %i %i: %s^7 took the flag\n", other->s.number, 3, cl->pers.netname);
	return -1;
}


// Need a blank line

/*
===========================================================================
Copyright (C) 2008 Poul Sander

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

//#include "cg_local.h"
#include "../qcommon/q_shared.h"
#include "../renderer/tr_types.h"
#include "../game/bg_public.h"
#include "../game/challenges.h"
#ifdef PARTofUI
#include "../q3_ui/ui_local.h"
#else
#include "../cgame/cg_local.h"
#endif

#define FILENAME    "challenges.dat"

//First two static variables that may only be accessed throgh functions in this file
//The challengeTable is of constant size so there is room for more challenges being added in the furture
static unsigned int challengeTable[CHALLENGES_MAX];
//We have a variable to see if the system is initialized
static qboolean challengesInitialized = qfalse;

//This function initializes the challenge system. It has no effect if the system is already intialized.
void challenges_init(void)
{
    fileHandle_t file;
    int fileLen;
    int i;

//If already initialized then do nothing
    if(challengesInitialized)
        return;

//Else open file
    fileLen = trap_FS_FOpenFile(FILENAME, &file, FS_READ);

//If the file not is big enough to contain all challenges then initialize from zero
    if(fileLen<sizeof(challengeTable))
    {
	trap_FS_FCloseFile(file); //Lets remeber to close the file before all returns
	for(i=0;i<CHALLENGES_MAX;i++)
		challengeTable[i]=0; //Init all challenges to zero
	challengesInitialized = qtrue; //Still consider the system loaded!
        return; //Not enough data to actually read
    }

//If the file is big enough:
	trap_FS_Read(challengeTable,sizeof(challengeTable),file);

trap_FS_FCloseFile(file); //Lets remember to close the file
    challengesInitialized = qtrue; //We are initialized!
    return;
}

//This functions saves the challenges. It only needs to be called once for each game. It should be called from the shutdown function
void challenges_save(void)
{
	fileHandle_t file;
	int fileStatus;
	int i;
	if(!challengesInitialized)
	{
		//The challenge system has not been initialized and therefore cannot be saved
		//CG_Printf("Challenge system not initialized.\n");
        	return;
	}
	//Open the challenges file for writing:
	fileStatus = trap_FS_FOpenFile(FILENAME, &file, FS_WRITE);
	if(fileStatus<0)
	{
		//For some reason file opening failed
                #ifndef PARTofUI
		CG_Printf("Failed to open challenges.dat for writing\n");
                #endif
		return;
	}
	for(i=0;i<CHALLENGES_MAX;i++)
	{
		//Write the table to disk
		trap_FS_Write((const void*)&challengeTable[i],sizeof(unsigned int),file);
	}

	//Always close the file in id tech 3
	trap_FS_FCloseFile(file);
	//Lets make the challenges system uninitialized since changes after this will most likely not be saved anyway.
	challengesInitialized = qfalse;
        #ifndef PARTofUI
	CG_Printf("Wrote challenges.cfg\n");
        #endif
}

//Get the number of times a given challenge has been completed
unsigned int getChallenge(int challenge)
{
	challenges_init();
	if(challenge>=CHALLENGES_MAX)
		return 0;
	return challengeTable[challenge];
}

void addChallenge(int challenge)
{
	if(challenge>=CHALLENGES_MAX)
	{
                #ifndef PARTofUI
		CG_Printf("Challenge #%u is >=CHALLENGES_MAX\n",challenge);
                #endif
		return; //Maybe also print an error?	
	}
	challenges_init();
	challengeTable[challenge]++;
	//CG_Printf("Increased challenge #%u by one. Is now %u\n",challenge,challengeTable[challenge]);
}

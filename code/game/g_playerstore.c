/*
===========================================================================
Copyright (C) 2008-2009 Poul Sander

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
//KK-OAX moved this out to g_playerstore.h
//I know this might mean a performance hit and what not...
//I can go back and optimize once I get it functioning. 
/*
#define MAX_PLAYERS_STORED 32

#define GUID_SIZE 32


typedef struct {
    char guid[GUID_SIZE+1]; //The guid is 32 chars long
    int age; //int that grows each time a new player is stored. The lowest number is always replaced. Reset to 0 then retrieved.
    int	persistant[MAX_PERSISTANT]; //This is the only information we need to save
} playerstore_t;*/

static playerstore_t *playerstore[MAX_PLAYERS_STORED];

static int nextAge;

/*
 *Resets the player store. Should be called everytime game.qvm is loaded.
 */
void PlayerStoreInit( void ) {
    //memset(playerstore,0,sizeof(playerstore));
    playerstore_t *p;
    int i, j;

    for( i = 0; i < MAX_PLAYERS_STORED && playerstore[ i ]; i++ )
    {
        BG_Free( playerstore[ i ] );
        playerstore[ i ] = NULL;
    }
    
    for( i = 0; i <= MAX_PLAYERS_STORED; i++ )
    {
        p = BG_Alloc( sizeof( playerstore_t ) );
        for( j = 0; j<= MAX_PERSISTANT; j++ )
        {
            p->persistant[ j ] = 0;
        }
        p->age = 0;
        *p->guid = '\0';
        playerstore[ i ] = p;
    }
    nextAge = 1;
}

void PlayerStore_store(char* guid, playerState_t ps) {
    int place2store = -1;
    int lowestAge = 32000;
    int i;
    if(strlen(guid)<32)
    {
        G_LogPrintf("Playerstore: Failed to store player. Invalid guid: %s\n",guid);
        return;
    }
    for(i=0;i<MAX_PLAYERS_STORED;i++) {
        if(!Q_stricmp(guid,playerstore[i]->guid)) {
            place2store=i;
        }
    }

    if(place2store<0)
    for(i=0;i<MAX_PLAYERS_STORED;i++) {
        if(playerstore[i]->age < lowestAge) {
            place2store = i;
            lowestAge = playerstore[i]->age;
        }
    }

    //Make sure we don't store in -1
    if(place2store<0)
        place2store = 0;
    //place2store is now the place to store to.
    playerstore[place2store]->age = nextAge++;
    Q_strncpyz(playerstore[place2store]->guid,guid,GUID_SIZE+1);
    memcpy(playerstore[place2store]->persistant,ps.persistant,sizeof(int[MAX_PERSISTANT]));
    //SharpestTool
    //This is sort of hackeriffic...but oh well...
    playerstore[place2store]->writtenTo = 1;
    G_LogPrintf("Playerstore: Stored player with guid: %s in %u\n", playerstore[place2store]->guid,place2store);
}

void PlayerStore_restore(char* guid, playerState_t *ps)  {
    int i;
    if(strlen(guid)<32)
    {
        G_LogPrintf("Playerstore: Failed to restore player. Invalid guid: %s\n",guid);
        return;
    }
    for(i=0;i<MAX_PLAYERS_STORED;i++) {
        if(!Q_stricmpn(guid,playerstore[i]->guid,GUID_SIZE) && playerstore[i]->age != -1) {
            memcpy(ps->persistant,playerstore[i]->persistant,sizeof(int[MAX_PERSISTANT]));
            //Never ever restore a player with negative score
            if(ps->persistant[PERS_SCORE]<0)
                ps->persistant[PERS_SCORE]=0;
            playerstore[i]->age = -1;
            G_LogPrintf("Restored player with guid: %s\n",guid);
            return;
        }
    }
    G_LogPrintf("Playerstore: Nothing to restore. Guid: %s\n",guid);
}


/*
====================
G_dumpPlayersToFile
KK-OAX 
Writes all players in playerstore to file.  
====================
*/
void G_dumpPlayersToFile( void )
{
    fileHandle_t f;
    int len, i;
    int t;
    char buffer[MAX_STRING_CHARS];
    char map[MAX_STRING_CHARS] = "mapname";

    if( !g_playerLog.string[ 0 ] )
    {
        G_Printf( S_COLOR_YELLOW "WARNING: g_playerLog is not set. "
        " players for this map will not be logged.\n" );
        return;
    }
    
    //For performance reasons I'm not gonna try and see if the file exists...
    //We've got the "non-existent file" exception handled, so let's assume it's there
    //and we will just slap our info on the end of it. 
    len = trap_FS_FOpenFile( g_playerLog.string, &f, FS_APPEND );
    if( len < 0 )
    {
        G_Printf( "G_dumpPlayersToFile: could not open g_playerLog file \"%s\"\n",
                g_playerLog.string );
        return;
    }
    
    //Let's grab the time the map change/game shutdown is called. 
    t = trap_RealTime( NULL );
    //Grabbing the map name from the engine. 
    trap_Cvar_VariableStringBuffer( map, buffer, MAX_STRING_CHARS );   
    
    //I called it game, it can be match or whatever else 
    trap_FS_Write( "[Game]\n", 7, f );
   
    trap_FS_Write( "gametype = ", 11, f );
    writeFile_int( g_gametype.integer, f );
    
    trap_FS_Write( "date     = ", 11, f );
    writeFile_int( t, f );
    
    trap_FS_Write( "Map      = ", 11, f );
    writeFile_string( buffer, f );
  
    //Go through the list of players stored, writing each one to
    //the file.  
    for( i = 0; i < MAX_PLAYERS_STORED && playerstore[ i ]; i++ )
    {
        if( playerstore[i]->writtenTo != 0 )
        {
            trap_FS_Write( "player   = ", 11, f );
            writeFile_string( playerstore[ i ]->guid, f );
        } 
        else 
        {
            break;
        }
    }
    trap_FS_Write( "\n", 1, f );  //Make sure there is a line break
    trap_FS_FCloseFile( f );
    return;
}

void PlayerStoreCleanup( void )
{
    int i;

    for( i = 0; i < MAX_PLAYERS_STORED && playerstore[ i ]; i++ )
    {
        BG_Free( playerstore[ i ] );
        playerstore[ i ] = NULL;
    }
}


/*
===========================================================================
Copyright (C) 2008-2009 Poul Sander
Copyright (C) 2009 Karl Kuglin

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

#ifndef _G_PLAYERSTORE_H
#define _G_PLAYERSTORE_H

//KK-OAX If we ever get more than 1024 unique players on a server during 1 level, I will 
//give up everything I own and become a priest.
#define MAX_PLAYERS_STORED 1024
#define GUID_SIZE 32

//KK-OAX Added the writtenTo field, useful for cutting down searches while reading.
typedef struct playerstore
{
    char guid[GUID_SIZE+1]; //The guid is 32 chars long
    int writtenTo;
    int age; //int that grows each time a new player is stored. The lowest number is always replaced. Reset to 0 then retrieved.
    int	persistant[MAX_PERSISTANT]; //This is the only information we need to save
    
} playerstore_t;

void PlayerStoreInit( void );
void PlayerStore_store(char* guid, playerState_t ps);
void PlayerStore_restore(char* guid, playerState_t *ps);
void G_dumpPlayersToFile( void );
void PlayerStoreCleanup( void );

#endif /* ifndef _G_ADMIN_H */


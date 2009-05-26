/*
===========================================================================
Copyright (C) 2009 Karl Kuglin

This file is part of the Open Arena source code.
Parts of this file utilize code originally written for Tremulous by 
Tony J. White.
Use of that code is governed by GPL version 2 and any later versions. 

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

killspree_t *killSprees[ MAX_KSPREE ];
deathspree_t *deathSprees[ MAX_DSPREE ];

static void disableMultiKills( void )
{
    //If multikills are enabled, disable them to keep stock excellent sound
    //Haven't added multikills yet so this "does" nothing.     
    if(g_altExcellent.integer) {
        g_altExcellent.integer = 0;
    }
}
/*
=================
readAltKillSettings
Since this is cvar dependent, it has to be placed in G_InitGame after cvarss are registered.
=================
*/
qboolean readAltKillSettings( gentity_t *ent, int skiparg )
{
    //Let's Initialize some Spree structs/objects
    killspree_t     *k = NULL;
    deathspree_t    *d = NULL;
    //spree counters
    int             ksc = 0, dsc = 0;
    int             spreeDivisor;
    
    //Give us an int to use in "for" loops
    int             loopStep = 0;
    
    //File Stuff
    fileHandle_t    file;
    int             length;
    qboolean        kspree_read;
    qboolean        dspree_read;
    char            *cnf, *cnf2;
    char            *t;

    
    // These must be done first to avoid a small memory leak 
    // from initializing the Spree structs/objects.
    for( loopStep = 0; killSprees[ loopStep ]; loopStep++ ) {
        BG_Free( killSprees[ loopStep ] );
            killSprees[ loopStep ] = NULL;
    }
        
    for( loopStep = 0; deathSprees[ loopStep ]; loopStep++ ) {
        BG_Free( deathSprees[ loopStep ] );
            deathSprees[ loopStep ] = NULL;
    }
    
    // If the config file is not defined...forget reading/loading
    if(!g_sprees.string[0]) {
        //Let's disable multikills to keep stock excellent sound
        disableMultiKills();
        return qfalse;
    }
    
    // Set the spreeDivisor from cvar g_spreeDiv
    // If the g_spreeDiv is set to 0 we need to revert to default
    if( ( !g_spreeDiv.integer ) || ( g_spreeDiv.integer == 0 ) ) {
        spreeDivisor = 5;
    } else {
        spreeDivisor = g_spreeDiv.integer;
    }
    
    length = trap_FS_FOpenFile( g_sprees.string, &file, FS_READ );
    
    //If the file can't be accessed/opened. 
    if( length < 0 ) {
        G_Printf( "Could not open configuration file for Sprees and Multikills %s\n", g_sprees.string );
        disableMultiKills();
        return qfalse;
    }
    //Let's "Dynamically" allocate some memory!!! :)
    cnf = BG_Alloc( length + 1 );
    cnf2 = cnf;
    
    //Load the whole file up. 
    trap_FS_Read( cnf, length, file );
    *( cnf + length ) = '\0';
    trap_FS_FCloseFile( file );
    
    kspree_read = dspree_read = qfalse;
    
    //Let's start parsing em. 
    COM_BeginParseSession( g_sprees.string );
    while( 1 )
    {
        t = COM_Parse( &cnf );
        if( !*t )
            break;
        if( !Q_stricmp( t, "[kspree]" ) ) {
            
            if( ksc >= MAX_KSPREE )
                return qfalse;
            k = BG_Alloc( sizeof( killspree_t ) );
            killSprees[ ksc++ ] = k;
            kspree_read = qtrue;
            dspree_read = qfalse;
            
        } else if ( !Q_stricmp( t, "[dspree]" ) ) {
        
            if( dsc >= MAX_DSPREE )
                return qfalse;
            d = BG_Alloc( sizeof( deathspree_t ) );
            deathSprees[ dsc++ ] = d;
            dspree_read = qfalse;
            kspree_read = qtrue;
        //Parse a killing spree    
        } else if ( kspree_read ) {
            if( !Q_stricmp( t, "level" ) ) {
                readFile_int( &cnf, &k->spreeLevel );
                //Let's take the spreeLevel and multiply it by the spreeDivisor to give us our count
                k->streakCount = k->spreeLevel*spreeDivisor;
            } else if ( !Q_stricmp( t, "message" ) ) {
                readFile_string( &cnf, k->spreeMsg, sizeof( k->spreeMsg ) );
            } else if ( !Q_stricmp( t, "printpos" ) ) {
                readFile_int( &cnf, &k->position );
            } else if ( !Q_stricmp( t, "sound" ) ) {
                readFile_string( &cnf, k->sound2Play, sizeof( k->sound2Play ) );
            } else {
                COM_ParseError( "Killing Spree unrecognized token \"%s\"", t );
            }
        } else if ( dspree_read ) {
            if( !Q_stricmp( t, "level" ) ) {
                readFile_int( &cnf, &d->spreeLevel );
                //Let's take the spreeLevel and multiply it by the spreeDivisor to give us our count
                d->streakCount = d->spreeLevel*spreeDivisor;
            } else if ( !Q_stricmp( t, "message" ) ) {
                readFile_string( &cnf, d->spreeMsg, sizeof( d->spreeMsg ) );
            } else if ( !Q_stricmp( t, "print" ) ) {
                readFile_int( &cnf, &d->position );
            } else if ( !Q_stricmp( t, "sound" ) ) {
                readFile_string( &cnf, d->sound2Play, sizeof( d->sound2Play ) );
            } else {
                COM_ParseError( "Death Spree unrecognized token \"%s\"", t );
            }
        } else {
            COM_ParseError( "unexpected token \"%s\"", t );
        }
    }
    //Let's "free" some memory now.
    BG_Free( cnf2 );
    G_Printf("Sprees/Kills: loaded %d killing sprees and %d death sprees\n", ksc, dsc );
    
    return qtrue;
}

static char *CreateMessage( gentity_t *ent, char *message )
{
    static char output[ MAX_SAY_TEXT ] = { "" };
    char  name[ MAX_NAME_LENGTH ] = { "UNKNOWN" };
    
    //No message, give em nothing back.    
    if( !*message )
        return output;    
    
    //Let's get the name for the client. 
    Q_strncpyz( name, ent->client->pers.netname, sizeof( name ));
    Q_strcat( output, sizeof( name ), name );
    Q_strcat( output, sizeof( message ), message );
    return output;
}


     
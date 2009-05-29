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
// NOTE: This code is by no means complete. 

#include "g_local.h"

killspree_t *killSprees[ MAX_KSPREE ];
deathspree_t *deathSprees[ MAX_DSPREE ];
multikill_t *multiKills[ MAX_MULTIKILLS ];


/*
=================
G_ReadAltKillSettings
Since this is cvar dependent, it has to be placed in G_InitGame after cvars are registered.
=================
*/
qboolean G_ReadAltKillSettings( gentity_t *ent, int skiparg )
{
    //Let's Initialize some Spree structs/objects
    killspree_t     *k = NULL;
    deathspree_t    *d = NULL;
    multikill_t     *m = NULL;
    //spree counters
    int             ksc = 0, dsc = 0, mc = 0;
    int             spreeDivisor;
    
    //Give us an int to use in "for" loops
    int             loopStep = 0;
    
    //File Stuff
    fileHandle_t    file;
    int             length;
    qboolean        kspree_read;
    qboolean        dspree_read;
    qboolean        mkill_read;
    char            *cnf, *cnf2;
    char            *t;

    //Let's clear out any existing killing sprees/death sprees. YAYY BG_FREE!!!!!
    for( loopStep = 0; loopStep < MAX_KSPREE && killSprees[ loopStep ]; loopStep++ ) {
        BG_Free( killSprees[ loopStep ] );
        killSprees[ loopStep ] = NULL;
    }
        
    for( loopStep = 0; loopStep < MAX_KSPREE && deathSprees[ loopStep ]; loopStep++ ) {
        BG_Free( deathSprees[ loopStep ] );
        deathSprees[ loopStep ] = NULL;
    }
    
    for( loopStep = 0; loopStep < MAX_MULTIKILLS && multiKills[ loopStep ]; loopStep++ ) {
        BG_Free( multiKills[ loopStep ] );
        multiKills[ loopStep ] = NULL;
    }    
    
    // If the config file is not defined...forget reading/loading
    if( !g_sprees.string[0] ) {
        //Let's disable multikills to keep stock excellent sound
        g_altExcellent.integer = 0;
        return qfalse;
    }
    /* 
    only set spreeDivisor to cvar g_spreeDiv if g_spreeDiv is >= "2".
    0 will cause problems and having 1 is just a fool who wants to hear something
    on every kill. 
    
    FIXME: This works for beta tests, but when multikills are added
    it will really make server configuration inflexible and confusing.     
    */  
    if( g_spreeDiv.integer >= 2 ) {
        spreeDivisor = g_spreeDiv.integer;
    } else {
        g_spreeDiv.integer = 5;
        spreeDivisor = 5; 
        G_Printf( "Error: cvar g_spreeDiv must not be set to 0 or 1, reverting to default!\n" );
    } 
    
    length = trap_FS_FOpenFile( g_sprees.string, &file, FS_READ );
    
    //If the file can't be accessed/opened. 
    if( length < 0 ) {
        G_Printf( "Could not open configuration file for Sprees and Multikills %s\n", g_sprees.string );
        g_altExcellent.integer = 0;
        return qfalse;
    }
    //Allocate some memory.
    cnf = BG_Alloc( length + 1 );
    cnf2 = cnf;
    
    //Load the whole file up. 
    trap_FS_Read( cnf, length, file );
    *( cnf + length ) = '\0';
    trap_FS_FCloseFile( file );
    
    kspree_read = dspree_read = mkill_read = qfalse;
    
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
            mkill_read = qfalse;
            
        } else if ( !Q_stricmp( t, "[dspree]" ) ) {
        
            if( dsc >= MAX_DSPREE )
                return qfalse;
            d = BG_Alloc( sizeof( deathspree_t ) );
            deathSprees[ dsc++ ] = d;
            dspree_read = qtrue;
            kspree_read = qfalse;
            mkill_read = qfalse;
        } else if ( !Q_stricmp( t, "[mkill]" ) ) {
            
            if( mc >= MAX_MULTIKILLS )
                return qfalse;
            m = BG_Alloc( sizeof( multikill_t ) );
            multiKills[ mc++ ] = m;
            mkill_read = qtrue;
            kspree_read = qfalse;
            dspree_read = qfalse; 
        //Parse a killing spree    
        } else if ( kspree_read ) {
            if( !Q_stricmp( t, "level" ) ) {
                readFile_int( &cnf, &k->spreeLevel );
                //Let's take the spreeLevel and multiply it by the spreeDivisor to give us our count
                k->streakCount = ( ( k->spreeLevel ) * ( spreeDivisor ) );
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
                d->streakCount = ( ( d->spreeLevel ) * ( spreeDivisor ) );
            } else if ( !Q_stricmp( t, "message" ) ) {
                readFile_string( &cnf, d->spreeMsg, sizeof( d->spreeMsg ) );
            } else if ( !Q_stricmp( t, "printpos" ) ) {
                readFile_int( &cnf, &d->position );
            } else if ( !Q_stricmp( t, "sound" ) ) {
                readFile_string( &cnf, d->sound2Play, sizeof( d->sound2Play ) );
            } else {
                COM_ParseError( "Death Spree unrecognized token \"%s\"", t );
            }
        } else if ( mkill_read ) {
            if ( !Q_stricmp( t, "kills" ) ) {
                readFile_int( &cnf, &m->kills );
            } else if ( !Q_stricmp( t, "message" ) ) {
                readFile_string( &cnf, m->killMsg, sizeof( m->killMsg ) );
            } else if ( !Q_stricmp( t, "sound" ) ) {
                readFile_string( &cnf, m->sound2Play, sizeof( m->sound2Play ) );
            } else {
                COM_ParseError( "Multikill unrecognized token \"%s\"", t );
            }
        } else {
            COM_ParseError( "unexpected token \"%s\"", t );
        }
    }
    //Let's "free" some memory now.
    BG_Free( cnf2 );
    G_Printf("Sprees/Kills: loaded %d killing sprees, %d death sprees, and %d multikills.\n", ksc, dsc, mc );
    //Mark the Upper Bounds of the Arrays (Since they start numbering at 0, We subtract 1 )
    if( ksc > 0 ) {
        level.kSpreeUBound = ( ksc - 1 );        
    } else {
        level.kSpreeUBound = -1;
    }
    if( dsc > 0 ) {
        level.dSpreeUBound = ( dsc - 1 );
    } else {
        level.dSpreeUBound = -1;
    }
    if( mc > 0 ) {
        level.mKillUBound = ( dsc - 1 );
    } else {
        level.mKillUBound = -1;
    }
    return qtrue;
}


static char *fillPlaceHolder( char *stringToSearch, char *placeHolder, char *replaceWith )
{
  static char output[ MAX_SAY_TEXT ];
  char *p;

  if( !( p = strstr( stringToSearch, placeHolder ) ) )
    return stringToSearch;
    
  strncpy( output, stringToSearch, p - stringToSearch ); 
  output[ p - stringToSearch ] = '\0';
 
  Q_snprintf( output + ( p - stringToSearch ), output - stringToSearch,  "%s%s", replaceWith, p + strlen( placeHolder ) ); 
  
  return output;
}

//This concatenate's the message to broadcast to the clients. 
static char *CreateMessage( gentity_t *ent, char *message, char *spreeNumber )
{
    static char output[ MAX_SAY_TEXT ] = { "" };
  
    char    name[ MAX_NAME_LENGTH ];
    //Do some sanity checks
    if( !ent ) {
        return output;
    } else if ( !*message ) {
        return output;
    } else if ( !spreeNumber ) {
        return output;
    }
    //Get the player name.
    Q_strncpyz( name, ent->client->pers.netname, sizeof( name ) );
    //Do Our Replacements
    Q_strncpyz( output, fillPlaceHolder( message, "[n]", name ), sizeof( output ) ); 
    Q_strncpyz( output, fillPlaceHolder( output, "[k]", spreeNumber ), sizeof( output ) );
    return output; 
}


/*
================
G_RunStreakLogic
KK-OAX This is called from player_die.
It does all the adding resetting of kill/death streaks
to be compared against the kill/death spree levels.
================
*/
void G_RunStreakLogic( gentity_t *attacker, gentity_t *victim ) 
{
    //We only want to sanity check for the victim at this point. 
    if( !victim || !victim->client )
        return;
    
    //We will reset the victim's killstreak counter    
    victim->client->pers.killstreak = 0;
    //Add one to the death streak counter
    victim->client->pers.deathstreak++;
    
    //Let's check for a deathspree for the victim
    G_CheckForSpree( victim, victim->client->pers.deathstreak, qfalse );

    //Move on to the attacker
    //Make sure they are a client and that the attacker and victim are not the same client
    //We don't want suicide to count towards a killstreak...
    if( ( attacker )
        && ( attacker->client )
        && ( attacker != victim ) ) {
        
        //Check the gametype--If FFA enabled, everybody's on the same team. 
        if( g_gametype.integer >= GT_TEAM && g_ffa_gt!= 1 ) {
            //If they are on the same team we don't want to count it towards a killing spree. 
            if( OnSameTeam( victim, attacker ) ) {
                return;
            }   
        }
        //Add to the killstreak, reset the deathstreak
        attacker->client->pers.deathstreak = 0;
        attacker->client->pers.killstreak++;
        
        //Let's check for a killingspree for the attacker
        G_CheckForSpree( attacker, attacker->client->pers.killstreak, qtrue );
        
    }     
}

//This is basically a hackjob...but I don't care
static qboolean TestSpreeWhole( int streak2Test ) {
    float   float2Test;
    float   spreeFDiv;
    float   resultf;
    int     spreeDiv;
    int     result;

    float2Test = streak2Test;
    spreeFDiv = g_spreeDiv.integer;
    spreeDiv = g_spreeDiv.integer;
    result = ( streak2Test / spreeDiv );
    resultf = ( float2Test / spreeFDiv );
    
    if( result == resultf ) {  
        return qtrue;
    } else {
        return qfalse;
    }
}

/*
==================
G_CheckForSpree
==================
*/
void G_CheckForSpree( gentity_t *ent, int streak2Test, qboolean checkKillSpree )
{
    int     loopStep;
    char    *returnedString;
    //If somebody want's to award killing sprees above 99 kills he/she can mod this his/herself!!! :)
    char    streakcount[ 3 ];
    char    *sound;
    int     position;
    int     soundIndex;
    
    qboolean    isSpree = qfalse;
    int         divisionHolder;
    
    //Probably Not Needed, but to protect Server Ops from Crashing their Stuff MidMatch
    if( g_spreeDiv.integer == 0 ) {
                return;
    }
    divisionHolder = ( streak2Test / g_spreeDiv.integer );
    //if it's a deathspree
    if( checkKillSpree == qfalse ) {
        //Is the streak higher than the largest level defined?
        if( divisionHolder > level.dSpreeUBound ) {
            //Let's make sure it's a whole number to mimic the other sprees
            isSpree = TestSpreeWhole( streak2Test );
            if( !isSpree ) {
                return;
            }
            //We've made it this far...now do the largest spree defined.
            Q_snprintf( streakcount, sizeof( streakcount ), "%i", streak2Test );
            returnedString = CreateMessage( ent, deathSprees[ level.dSpreeUBound ]->spreeMsg, streakcount ); 
            position = deathSprees[ level.dSpreeUBound ]->position;
            sound = deathSprees[ level.dSpreeUBound ]->sound2Play;
            soundIndex = G_SoundIndex( sound );
            G_GlobalSound( soundIndex );
            if( position == CENTER_PRINT ) {
                AP( va("cp \"%s\"", returnedString ) );
            } else {
                AP( va("chat \"%s\"", returnedString ) );
            } 
        } else {
            for( loopStep = 0; deathSprees[ loopStep ]; loopStep++ ) {            
                //If the deathSpree is equal to the streak to test
                if( deathSprees[ loopStep ]->streakCount == streak2Test ) {
                    //Using Q_snprintf to change the int into a char for replacement. 
                    Q_snprintf( streakcount, sizeof( streakcount ), "%i", deathSprees[ loopStep ]->streakCount );
                    //Let's grab the message to show, fill up the placeholders and concatenate it. 
                    returnedString = CreateMessage ( ent, deathSprees[ loopStep ]->spreeMsg, streakcount ); 
                    //Grab the Print Position ( 1 for Center Printing, 2 for Chat ) 
                    position = deathSprees[ loopStep ]->position;
                    //Grab the Sound
                    sound = deathSprees[ loopStep ]->sound2Play;
                    //Index the Sound
                    soundIndex = G_SoundIndex( sound );
                    //Play the Sound
                    G_GlobalSound( soundIndex );
                    //Print the Message
                    if( position == CENTER_PRINT ) {
                        AP( va("cp \"%s\"", returnedString ) );
                    } else {
                        AP( va("chat \"%s\"", returnedString ) );
                    }    
                    break;
                }
            }
        }   
    } else if( checkKillSpree == qtrue ) {
        //Is the streak higher than the largest level defined?
        if( divisionHolder > level.kSpreeUBound ) {
            //Let's make sure it's a whole number to mimic the other sprees
            isSpree = TestSpreeWhole( streak2Test );
            if( !isSpree ) {
                return;
            }
            //We've made it this far...now do the largest spree defined.
            Q_snprintf( streakcount, sizeof( streakcount ), "%i", streak2Test );
            returnedString = CreateMessage( ent, killSprees[ level.kSpreeUBound ]->spreeMsg, streakcount ); 
            position = killSprees[ level.kSpreeUBound ]->position;
            sound = killSprees[ level.kSpreeUBound ]->sound2Play;
            soundIndex = G_SoundIndex( sound );
            G_GlobalSound( soundIndex );
            if( position == CENTER_PRINT ) {
                AP( va("cp \"%s\"", returnedString ) );
            } else {
                AP( va("chat \"%s\"", returnedString ) );
            }
        } else { 
            for( loopStep = 0; killSprees[ loopStep ]; loopStep++ ) {
                if( killSprees[ loopStep ]->streakCount == streak2Test ) {
                    Q_snprintf( streakcount, sizeof( streakcount ), "%i", killSprees[ loopStep ]->streakCount );
                    returnedString = CreateMessage ( ent, killSprees[ loopStep ]->spreeMsg, streakcount );                 
                    position = killSprees[ loopStep ]->position;
                    sound = killSprees[ loopStep ]->sound2Play;                
                    soundIndex = G_SoundIndex( sound );                
                    G_GlobalSound( soundIndex );
                    if( position == CENTER_PRINT ) {
                        AP( va("cp \"%s\"", returnedString ) );
                    } else {
                        AP( va("chat \"%s\"", returnedString ) );
                    }  
                    break;
                }
            }
        }    
    } else {
        G_Printf("Killing Spree Error in G_CheckForSpree\n");
        return;
    }
}

/*
===============
G_checkForMultiKill
===============
*/
void G_checkForMultiKill( gentity_t *ent ) {
    
    int     loopStep;
    char    *returnedString;
    char    *sound;
    int     soundIndex;
    int     multiKillCount;
    char    multiKillString[ 2 ];
    
    //Let's grab the multikill count for the player first
    multiKillCount = ent->client->pers.multiKillCount;
    
    if( multiKillCount > multiKills[ level.mKillUBound ]->kills ) {
        Q_snprintf( multiKillString, sizeof( multiKillString ), "%i", multiKillCount );
        returnedString = CreateMessage ( ent, multiKills[ level.mKillUBound ]->killMsg, multiKillString );
        sound = multiKills[ level.mKillUBound ]->sound2Play;
        soundIndex = G_SoundIndex( sound );
        G_GlobalSound( soundIndex );
        AP( va("chat \"%s\"", returnedString ) );
        return;
    } else {     
        for( loopStep = 0; multiKills[ loopStep ]; loopStep++ ) {
            //If the multikill count is equal to a killLevel let's do this. 
            
            if( multiKills[ loopStep ]->kills == multiKillCount ) {            
                Q_snprintf( multiKillString, sizeof( multiKillString ), "%i", multiKills[ loopStep ]->kills );
                //Build the Message
                returnedString = CreateMessage ( ent, multiKills[ loopStep ]->killMsg, multiKillString );
                //Grab the sound
                sound = multiKills[ loopStep ]->sound2Play;
                //Index the sound
                soundIndex = G_SoundIndex( sound );
                //Play the sound
                G_GlobalSound( soundIndex );
                /* Print the String
                Since we don't want to clutter screens (the player is already going to get the excellent icon)
                we won't give them an option to centerprint. 
                */
                AP( va("chat \"%s\"", returnedString ) );
                break;
            }
        }   

    }
      
}


     
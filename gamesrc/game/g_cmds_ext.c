/*
===========================================================================
Copyright (C) 2004-2006 Tony J. White

This file is part of the Open Arena source code.

Copied from Tremulous under GPL version 2 including any later version.

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

#include  "g_local.h"

/*
=================
G_SayArgc
G_SayArgv
G_SayConcatArgs

trap_Argc, trap_Argv, and ConcatArgs consider say text as a single argument
These functions assemble the text and re-parse it on word boundaries
=================
*/
int G_SayArgc( void )
{
	int c = 0;
	char *s;

	s = ConcatArgs( 0 );
	while( 1 ) {
		while( *s == ' ' )
			s++;
		if( !*s )
			break;
		c++;
		while( *s && *s != ' ' )
			s++;
	}
	return c;
}

qboolean G_SayArgv( int n, char *buffer, int bufferLength )
{
	char *s;

	if( bufferLength < 1 )
		return qfalse;
	if( n < 0 )
		return qfalse;
	s = ConcatArgs( 0 );
	while( 1 ) {
		while( *s == ' ' )
			s++;
		if( !*s || n == 0 )
			break;
		n--;
		while( *s && *s != ' ' )
			s++;
	}
	if( n > 0 )
		return qfalse;
	//memccpy( buffer, s, ' ', bufferLength );
	while( *s && *s != ' ' && bufferLength > 1 ) {
		*buffer++ = *s++;
		bufferLength--;
	}
	*buffer = 0;
	return qtrue;
}

char *G_SayConcatArgs( int start )
{
	char *s;

	s = ConcatArgs( 0 );
	while( 1 ) {
		while( *s == ' ' )
			s++;
		if( !*s || start == 0 )
			break;
		start--;
		while( *s && *s != ' ' )
			s++;
	}
	return s;
}

void G_DecolorString( char *in, char *out, int len )
{
	len--;

	while( *in && len > 0 ) {
		if( Q_IsColorString( in ) ) {
			in++;
			if( *in )
				in++;
			continue;
		}
		*out++ = *in++;
		len--;
	}
	*out = '\0';
}

/*
==================
G_MatchOnePlayer

This is a companion function to G_ClientNumbersFromString()

err will be populated with an error message.
==================
*/
void G_MatchOnePlayer( int *plist, int num, char *err, int len )
{
	gclient_t *cl;
	int i;
	char line[ MAX_NAME_LENGTH + 10 ] = {""};

	err[ 0 ] = '\0';
	if( num == 0 ) {
		Q_strcat( err, len, "no connected player by that name or slot #" );
	}
	else if( num > 1 ) {
		Q_strcat( err, len, "more than one player name matches. Be more specific or use the slot #:\n" );
		for( i = 0; i < num; i++ ) {
			cl = &level.clients[ plist[ i ] ];
			if( cl->pers.connected == CON_DISCONNECTED )
				continue;
			Com_sprintf( line, sizeof( line ), "%2i - %s^7\n",
			             plist[ i ], cl->pers.netname );
			if( strlen( err ) + strlen( line ) > len )
				break;
			Q_strcat( err, len, line );
		}
	}
}

/*
==================
G_SanitiseString

Remove case and control characters from a string
==================
*/
void G_SanitiseString( char *in, char *out, int len )
{
	qboolean skip = qtrue;
	int spaces = 0;

	len--;

	while( *in && len > 0 ) {
		// strip leading white space
		if( *in == ' ' ) {
			if( skip ) {
				in++;
				continue;
			}
			spaces++;
		}
		else {
			spaces = 0;
			skip = qfalse;
		}

		if( Q_IsColorString( in ) ) {
			in += 2;    // skip color code
			continue;
		}

		if( *in < 32 ) {
			in++;
			continue;
		}

		*out++ = tolower( *in++ );
		len--;
	}
	out -= spaces;
	*out = 0;
}


/*
==================
G_ClientNumbersFromString

Sets plist to an array of integers that represent client numbers that have
names that are a partial match for s.

Returns number of matching clientids up to max.
==================
*/
int G_ClientNumbersFromString( char *s, int *plist, int max )
{
	gclient_t *p;
	int i, found = 0;
	char n2[ MAX_NAME_LENGTH ] = {""};
	char s2[ MAX_NAME_LENGTH ] = {""};

	if( max == 0 )
		return 0;

	// if a number is provided, it is a clientnum
	for( i = 0; s[ i ] && isdigit( s[ i ] ); i++ );
	if( !s[ i ] ) {
		i = atoi( s );
		if ( i >= 0 && i < level.maxclients ) {
			p = &level.clients[ i ];
			if( p->pers.connected != CON_DISCONNECTED ) {
				*plist = i;
				return 1;
			}
		}
		// we must assume that if only a number is provided, it is a clientNum
		return 0;
	}

	// now look for name matches
	G_SanitiseString( s, s2, sizeof( s2 ) );
	if( strlen( s2 ) < 1 )
		return 0;
	for ( i = 0; i < level.maxclients && found < max; i++ ) {
		p = &level.clients[ i ];
		if( p->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		G_SanitiseString( p->pers.netname, n2, sizeof( n2 ) );
		if ( strstr( n2, s2 ) ) {
			*plist++ = i;
			found++;
		}
	}
	return found;
}


/*
==================
G_FloodLimited

Determine whether a user is flood limited, and adjust their flood demerits
Print them a warning message if they are over the limit
Return is time in msec until the user can speak again
==================
*/
int G_FloodLimited( gentity_t *ent )
{
	int deltatime, ms;

	if( g_floodMinTime.integer <= 0 )
		return 0;

	// handles !ent
	if( G_admin_permission( ent, ADMF_NOCENSORFLOOD ) )
		return 0;

	deltatime = level.time - ent->client->pers.floodTime;

	ent->client->pers.floodDemerits += g_floodMinTime.integer - deltatime;
	if( ent->client->pers.floodDemerits < 0 )
		ent->client->pers.floodDemerits = 0;
	ent->client->pers.floodTime = level.time;

	ms = ent->client->pers.floodDemerits - g_floodMaxDemerits.integer;
	if( ms <= 0 )
		return 0;
	trap_SendServerCommand( ent - g_entities, va( "print \"You are flooding: please wait %d second%s before trying again\n",
	                        ( ms + 999 ) / 1000, ( ms > 1000 ) ? "s" : "" ) );
	return ms;
}

static void QDECL G_AdminMessage( const char *prefix, const char *fmt, ... )  __attribute__ ((format (printf, 2, 3)));

/*
=================
G_AdminMessage

Print to all active server admins, and to the logfile, and to the server console
Prepend *prefix, or '[SERVER]' if no *prefix is given
=================
*/
static void QDECL G_AdminMessage( const char *prefix, const char *fmt, ... )
{
	va_list argptr;
	char    string[ 1024 ];
	char    outstring[ 1024 ];
	int     i;

	// Format the text
	va_start( argptr, fmt );
	Q_vsnprintf( string, sizeof( string ), fmt, argptr );
	va_end( argptr );

	// If there is no prefix, assume that this function was called directly
	// and we should add one
	if( !prefix || !prefix[ 0 ] ) {
		prefix = "[SERVER]:";
	}

	// Create the final string
	Com_sprintf( outstring, sizeof( outstring ), "%s " S_COLOR_MAGENTA "%s",
	             prefix, string );

	// Send to all appropriate clients
	for( i = 0; i < level.maxclients; i++ )
		if( G_admin_permission( &g_entities[ i ], ADMF_ADMINCHAT ) )
			trap_SendServerCommand( i, va( "chat \"%s\"", outstring ) );

	// Send to the logfile and server console
	G_LogPrintf("adminmsg: %s\n", outstring );
}

/*
=================
Cmd_AdminMessage_f

Send a message to all active admins
=================
*/
void Cmd_AdminMessage_f( gentity_t *ent )
{
	char cmd[ sizeof( "say_team" ) ];
	char prefix[ 50 ];
	char *msg;
	int skiparg = 0;

	// Check permissions and add the appropriate user [prefix]
	if( !ent ) {
		Com_sprintf( prefix, sizeof( prefix ), "[CONSOLE]:" );
	}
	else if( !G_admin_permission( ent, ADMF_ADMINCHAT ) ) {
		if( !g_publicAdminMessages.integer ) {
			ADMP( "Sorry, but use of /a by non-admins has been disabled.\n" );
			return;
		}
		else {
			Com_sprintf( prefix, sizeof( prefix ), "[PLAYER] %s" S_COLOR_WHITE ":",
			             ent->client->pers.netname );
			ADMP( "Your message has been sent to any available admins and to the server logs.\n" );
		}
	}
	else {
		Com_sprintf( prefix, sizeof( prefix ), "[ADMIN] %s" S_COLOR_WHITE ":",
		             ent->client->pers.netname );
	}

	// Skip say/say_team if this was used from one of those
	G_SayArgv( 0, cmd, sizeof( cmd ) );
	if( Q_strequal( cmd, "say" ) || Q_strequal( cmd, "say_team" ) ) {
		skiparg = 1;
		G_SayArgv( 1, cmd, sizeof( cmd ) );
	}
	if( G_SayArgc( ) < 2 + skiparg ) {
		ADMP( va( "usage: %s [message]\n", cmd ) );
		return;
	}

	msg = G_SayConcatArgs( 1 + skiparg );

	// Send it
	G_AdminMessage( prefix, "%s", msg );
}


/*
==================
G_ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int G_ClientNumberFromString( char *s )
{
	gclient_t *cl;
	int       i;
	char      s2[ MAX_NAME_LENGTH ];
	char      n2[ MAX_NAME_LENGTH ];

	// numeric values are just slot numbers
	for( i = 0; s[ i ] && isdigit( s[ i ] ); i++ );
	if( !s[ i ] ) {
		i = atoi( s );

		if( i < 0 || i >= level.maxclients )
			return -1;

		cl = &level.clients[ i ];

		if( cl->pers.connected == CON_DISCONNECTED )
			return -1;

		return i;
	}

	// check for a name match
	G_SanitiseString( s, s2, sizeof( s2 ) );

	for( i = 0, cl = level.clients; i < level.maxclients; i++, cl++ ) {
		if( cl->pers.connected == CON_DISCONNECTED )
			continue;

		G_SanitiseString( cl->pers.netname, n2, sizeof( n2 ) );

		if( strequals( n2, s2 ) )
			return i;
	}

	return -1;
}

//KK-OAX Used to !spec999'ers
/*
=============
G_ClientIsLagging
=============
*/
qboolean G_ClientIsLagging( gclient_t *client )
{
	if( client ) {
		if( client->ps.ping >= 999 )
			return qtrue;
		else
			return qfalse;
	}

	return qfalse; //is a non-existant client lagging? woooo zen
}

/*
==================
SanitizeString

Remove case and control characters
==================
*/
void SanitizeString( char *in, char *out )
{
	while ( *in ) {
		if ( *in == 27 ) {
			in += 2;		// skip color code
			continue;
		}
		if ( *in < 32 ) {
			in++;
			continue;
		}
		*out++ = tolower( *in++ );
	}

	*out = 0;
}


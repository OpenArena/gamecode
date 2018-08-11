/*
===========================================================================
Copyright (C) 2009 Poul Sander

This file is part of the Open Arena source code.

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

#include "ui_local.h"

#define VOTEMENU_BACK0      "menu/" MENU_ART_DIR "/back_0"
#define VOTEMENU_BACK1      "menu/" MENU_ART_DIR "/back_1"
#define ART_FIGHT0          "menu/" MENU_ART_DIR "/accept_0"
#define ART_FIGHT1          "menu/" MENU_ART_DIR "/accept_1"
#define ART_BACKGROUND      "menu/" MENU_ART_DIR "/addbotframe"

static char* votemenu_artlist[] =
{
	VOTEMENU_BACK0,
	VOTEMENU_BACK1,
	ART_FIGHT0,
	ART_FIGHT1,
	NULL
};

#define ID_BACK     100
#define ID_GO       101
#define ID_NEXTMAP  102
#define ID_RESTART  103
#define ID_DOWARMUP 104
#define ID_MAP      105
#define ID_KICK     106
#define ID_FRAG     107
#define ID_TIME     108
#define ID_GAMETYPE 109
#define ID_CUSTOM   110
#define ID_SHUFFLE  111
#define ID_YES      112
#define ID_NO       113


//This sorta dependend on number of vote options
#define VOTEMENU_MENU_VERTICAL_SPACING	22

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menubitmap_s	back;
        menubitmap_s	go;

        //Allowed votes:
        qboolean        map_restart;
        qboolean        nextmap;
        qboolean        g_doWarmup;
        qboolean        g_doWarmupEnabled;
        qboolean        clientkick;
        qboolean        map;
        qboolean        gametype;
        qboolean        fraglimit;
        qboolean        timelimit;
        qboolean        custom;
        qboolean        shuffle;

        //Buttons:
        menutext_s      bMapRestart;
        menutext_s      bNextmap;
        menutext_s      bDoWarmup;
        menutext_s      bKick;
        menutext_s      bMap;
        menutext_s      bGametype;
        menutext_s      bTimelimit;
        menutext_s      bFraglimit;
        menutext_s      bShuffle;
        menutext_s      bCustom;
        menutext_s      bYes;
        menutext_s      bNo;

        int             selection;
} votemenu_t;

static votemenu_t	s_votemenu;

void UI_VoteMenuMenuInternal( void );


/*
=================
VoteMenu_CheckVoteNames
=================
*/

static void VoteMenu_CheckVoteNames( void ) {
    int voteflags;
    voteflags = trap_Cvar_VariableValue("cg_voteflags");
    s_votemenu.map_restart = voteflags&VF_map_restart;
    s_votemenu.nextmap = voteflags&VF_nextmap;
    s_votemenu.map = voteflags&VF_map;
    s_votemenu.gametype = voteflags&VF_g_gametype;
    //We never use "kick" in menues, always clientkick
    s_votemenu.clientkick = voteflags&VF_clientkick;
    s_votemenu.g_doWarmup = voteflags&VF_g_doWarmup;
    s_votemenu.timelimit = voteflags&VF_timelimit;
    s_votemenu.fraglimit = voteflags&VF_fraglimit;
    s_votemenu.custom = voteflags&VF_custom;
    s_votemenu.shuffle = voteflags&VF_shuffle;
}

/*
=================
VoteMenu_Event
=================
*/
static void VoteMenu_Event( void* ptr, int event )
{
	if (event != QM_ACTIVATED)
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_BACK:
			if (event != QM_ACTIVATED)
				return;

			UI_PopMenu();
			break;
		case ID_NEXTMAP:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote nextmap" );
			UI_PopMenu();
			break;
		case ID_RESTART:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote map_restart" );
			UI_PopMenu();
			break;
		case ID_DOWARMUP:
			if(s_votemenu.g_doWarmupEnabled)
				trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_doWarmup 0" );
			else
				trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_doWarmup 1" );
			UI_PopMenu();
			break;
		case ID_SHUFFLE:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote shuffle" );
			UI_PopMenu();
			break;
		case ID_FRAG:
			UI_VoteFraglimitMenu();
			//Don't pop! It will do a double pop if successfull
			break;
		case ID_CUSTOM:
			UI_VoteCustomMenu();
			//Don't pop! It will do a double pop if successfull
			break;
		case ID_TIME:
			UI_VoteTimelimitMenu();
			//Don't pop! It will do a double pop if successfull
			break;
		case ID_GAMETYPE:
			UI_VoteGametypeMenu();
			//Don't pop! It will do a double pop if successfull
			break;
		case ID_KICK:
			UI_VoteKickMenu();
			//Don't pop! It will do a double pop if successfull
			break;
		case ID_MAP:
			UI_VoteMapMenu();
			//Don't pop! It will do a double pop if successfull
			break;
		case ID_YES:
			trap_Cmd_ExecuteText( EXEC_APPEND, "vote yes" );
			UI_PopMenu();
			break;
		case ID_NO:
			trap_Cmd_ExecuteText( EXEC_APPEND, "vote no" );
			UI_PopMenu();
			break;
	}
}

/*
=================
VoteMenu_Cache
=================
*/
static void VoteMenu_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!votemenu_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(votemenu_artlist[i]);
	}

	//Check all names
	VoteMenu_CheckVoteNames();
}

/*
=================
UI_VoteMenu_Draw
=================
*/
static void UI_VoteMenu_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu.menu );
}

static void UI_SetGeneticDefaults( menucommon_s* generic, int type) {
	generic->type = type;
	generic->x = 320;
	generic->flags = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
}

static void UI_SetMenuTextDefaults (menutext_s* mt) {
	UI_SetGeneticDefaults(&mt->generic, MTYPE_PTEXT);
	mt->style = UI_CENTER|UI_SMALLFONT;
	mt->color = color_red;
}

static void UI_SetTextEffect(menutext_s* mt, qboolean enabled, int currentSelection) {
	if(!enabled) {
		s_votemenu.bMapRestart.generic.flags |= QMF_INACTIVE|QMF_GRAYED;
	}
	else {
		if(currentSelection == mt->generic.id) {
			mt->color = color_orange;
		}
	}
}

/*
=================
UI_VoteMenuMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteMenuMenuInternal( void )
{
	int y;

	VoteMenu_Cache();

	s_votemenu.menu.wrapAround = qtrue;
	s_votemenu.menu.fullscreen = qfalse;
	s_votemenu.menu.draw = UI_VoteMenu_Draw;

	s_votemenu.banner.generic.type  = MTYPE_BTEXT;
	s_votemenu.banner.generic.x	  = 320;
	s_votemenu.banner.generic.y	  = 16;
	s_votemenu.banner.string		  = "CALL VOTE";
	s_votemenu.banner.color	      = color_white;
	s_votemenu.banner.style	      = UI_CENTER;

	y = 98;
	s_votemenu.bNextmap.generic.id          = ID_NEXTMAP;
	UI_SetMenuTextDefaults(&s_votemenu.bNextmap );
	UI_SetTextEffect(&s_votemenu.bNextmap, s_votemenu.nextmap, s_votemenu.selection);
	s_votemenu.bNextmap.generic.y           = y;
	s_votemenu.bNextmap.generic.callback    = VoteMenu_Event;
	s_votemenu.bNextmap.string              = "Next map";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bMapRestart.generic.id       = ID_RESTART;
	UI_SetMenuTextDefaults(&s_votemenu.bMapRestart );
	UI_SetTextEffect(&s_votemenu.bMapRestart, s_votemenu.map_restart, s_votemenu.selection);
	s_votemenu.bMapRestart.generic.y        = y;
	s_votemenu.bMapRestart.generic.callback = VoteMenu_Event;
	s_votemenu.bMapRestart.string           = "Restart match";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bShuffle.generic.id       = ID_SHUFFLE;
	UI_SetMenuTextDefaults(&s_votemenu.bShuffle );
	UI_SetTextEffect(&s_votemenu.bShuffle, s_votemenu.shuffle, s_votemenu.selection);
	s_votemenu.bShuffle.generic.y        = y;
	s_votemenu.bShuffle.generic.callback = VoteMenu_Event;
	s_votemenu.bShuffle.string           = "Shuffle teams";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bMap.generic.id          = ID_MAP;
	UI_SetMenuTextDefaults(&s_votemenu.bMap );
	UI_SetTextEffect(&s_votemenu.bMap, s_votemenu.map, s_votemenu.selection);
	s_votemenu.bMap.generic.y           = y;
	s_votemenu.bMap.generic.callback    = VoteMenu_Event;
	s_votemenu.bMap.string              = "Change map";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bGametype.generic.id          = ID_GAMETYPE;
	UI_SetMenuTextDefaults(&s_votemenu.bGametype );
	UI_SetTextEffect(&s_votemenu.bGametype, s_votemenu.gametype, s_votemenu.selection);
	s_votemenu.bGametype.generic.y           = y;
	s_votemenu.bGametype.generic.callback    = VoteMenu_Event;
	s_votemenu.bGametype.string              = "Change gametype";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bKick.generic.id          = ID_KICK;
	UI_SetMenuTextDefaults(&s_votemenu.bKick );
	UI_SetTextEffect(&s_votemenu.bKick, s_votemenu.clientkick, s_votemenu.selection);
	s_votemenu.bKick.generic.y           = y;
	s_votemenu.bKick.generic.callback    = VoteMenu_Event;
	s_votemenu.bKick.string              = "Kick player";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bDoWarmup.generic.id          = ID_DOWARMUP;
	UI_SetMenuTextDefaults(&s_votemenu.bDoWarmup );
	UI_SetTextEffect(&s_votemenu.bDoWarmup, s_votemenu.g_doWarmup, s_votemenu.selection);
	s_votemenu.bDoWarmup.generic.y           = y;
	s_votemenu.bDoWarmup.generic.callback    = VoteMenu_Event;
	if(s_votemenu.g_doWarmupEnabled)
		s_votemenu.bDoWarmup.string              = "Disable warmup";
	else
		s_votemenu.bDoWarmup.string              = "Enable warmup";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bFraglimit.generic.id          = ID_FRAG;
	UI_SetMenuTextDefaults(&s_votemenu.bFraglimit );
	UI_SetTextEffect(&s_votemenu.bFraglimit, s_votemenu.fraglimit, s_votemenu.selection);
	s_votemenu.bFraglimit.generic.y           = y;
	s_votemenu.bFraglimit.generic.callback    = VoteMenu_Event;
	s_votemenu.bFraglimit.string              = "Change fraglimit";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bTimelimit.generic.id          = ID_TIME;
	UI_SetMenuTextDefaults(&s_votemenu.bTimelimit );
	UI_SetTextEffect(&s_votemenu.bTimelimit, s_votemenu.timelimit, s_votemenu.selection);
	s_votemenu.bTimelimit.generic.y           = y;
	s_votemenu.bTimelimit.generic.callback    = VoteMenu_Event;
	s_votemenu.bTimelimit.string              = "Change timelimit";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bCustom.generic.id          = ID_CUSTOM;
	UI_SetMenuTextDefaults(&s_votemenu.bCustom );
	UI_SetTextEffect(&s_votemenu.bCustom, s_votemenu.custom, s_votemenu.selection);
	s_votemenu.bCustom.generic.y           = y;
	s_votemenu.bCustom.generic.callback    = VoteMenu_Event;
	s_votemenu.bCustom.string              = "Custom vote";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bYes.generic.id       = ID_YES;
	UI_SetMenuTextDefaults(&s_votemenu.bYes );
	UI_SetTextEffect(&s_votemenu.bYes, qtrue, s_votemenu.selection);
	s_votemenu.bYes.generic.y        = y;
	s_votemenu.bYes.generic.callback = VoteMenu_Event;
	s_votemenu.bYes.string           = "Vote yes";

	y+=VOTEMENU_MENU_VERTICAL_SPACING;
	s_votemenu.bNo.generic.id       = ID_NO;
	UI_SetMenuTextDefaults(&s_votemenu.bNo );
	UI_SetTextEffect(&s_votemenu.bNo, qtrue, s_votemenu.selection);
	s_votemenu.bNo.generic.y        = y;
	s_votemenu.bNo.generic.callback = VoteMenu_Event;
	s_votemenu.bNo.string           = "Vote no";

	s_votemenu.back.generic.type	   = MTYPE_BITMAP;
	s_votemenu.back.generic.name     = VOTEMENU_BACK0;
	s_votemenu.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_votemenu.back.generic.callback = VoteMenu_Event;
	s_votemenu.back.generic.id	   = ID_BACK;
	s_votemenu.back.generic.x		   = 320-128;
	s_votemenu.back.generic.y		   = 256+128-64 + VOTEMENU_MENU_VERTICAL_SPACING * 2;
	s_votemenu.back.width  		   = 128;
	s_votemenu.back.height  		   = 64;
	s_votemenu.back.focuspic         = VOTEMENU_BACK1;

	s_votemenu.go.generic.type	   = MTYPE_BITMAP;
	s_votemenu.go.generic.name     = ART_FIGHT0;
	s_votemenu.go.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_votemenu.go.generic.callback = VoteMenu_Event;
	s_votemenu.go.generic.id	   = ID_GO;
	s_votemenu.go.generic.x		   = 320;
	s_votemenu.go.generic.y		   = 256+128-64 + VOTEMENU_MENU_VERTICAL_SPACING * 2;
	s_votemenu.go.width  		   = 128;
	s_votemenu.go.height  		   = 64;
	s_votemenu.go.focuspic         = ART_FIGHT1;

}

/*
=================
UI_VoteMenuMenu
 *Called from outside
=================
*/
void UI_VoteMenuMenu( void ) {
	// zero set all our globals
	char serverinfo[MAX_INFO_STRING];
	memset( &s_votemenu, 0 ,sizeof(votemenu_t) );
	trap_GetConfigString( CS_SERVERINFO, serverinfo, MAX_INFO_STRING );
	s_votemenu.g_doWarmupEnabled = atoi(Info_ValueForKey(serverinfo,"g_doWarmup"));
	UI_VoteMenuMenuInternal();

	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.banner );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.back );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bNextmap );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bMapRestart );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bShuffle );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bMap );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bGametype );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bKick );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bDoWarmup );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bFraglimit );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bTimelimit );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bCustom );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bYes );
	Menu_AddItem( &s_votemenu.menu, (void*) &s_votemenu.bNo );

	UI_PushMenu( &s_votemenu.menu );
}

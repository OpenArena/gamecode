
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

#define VOTEMENU_BACK0          "menu/" MENU_ART_DIR "/back_0"
#define VOTEMENU_BACK1          "menu/" MENU_ART_DIR "/back_1"
#define ART_FIGHT0              "menu/" MENU_ART_DIR "/accept_0"
#define ART_FIGHT1              "menu/" MENU_ART_DIR "/accept_1"
#define ART_BACKGROUND          "menu/" MENU_ART_DIR "/addbotframe"

static char* votemenu_scorelimit_artlist[] =
{
	VOTEMENU_BACK0,
	VOTEMENU_BACK1,
	ART_FIGHT0,
	ART_FIGHT1,
	NULL
};

#define ID_BACK     100
#define ID_GO       101
#define ID_50       102
#define ID_10       103
#define ID_15       104
#define ID_20       105
#define ID_30       106
#define ID_40       107
#define ID_INF      108

#define SCORELIMIT_MENU_VERTICAL_SPACING	28

typedef struct
{
	menuframework_s menu;
	menutext_s      banner;
	menubitmap_s    back;
	menubitmap_s    go;

	//Buttons:
	menutext_s      bLimit50;
	menutext_s      bLimit10;
	menutext_s      bLimit15;
	menutext_s      bLimit20;
	menutext_s      bLimit30;
	menutext_s      bLimit40;
	menutext_s      bLimitInf;

	int min, max; //Values restricted by server
	int selection;
} votemenu_t;

static votemenu_t	s_votemenu_scorelimit;

void UI_VoteScorelimitMenuInternal( void );

/*
=================
VoteMenu_Scorelimit_Event
=================
*/
static void VoteMenu_Scorelimit_Event( void* ptr, int event )
{
	if( event != QM_ACTIVATED )
		return;

	switch (((menucommon_s*)ptr)->id)
	{
		case ID_BACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
			
		case ID_10:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 100" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_15:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 150" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_20:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 200" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_30:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 300" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_40:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 400" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_50:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 500" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_INF:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote scorelimit 0" );
			UI_PopMenu();
			UI_PopMenu();
			break;
	}
}

static void setMenutext_sl(menutext_s *menu,int y,int id,int value,char *text) {
	menu->generic.type        = MTYPE_PTEXT;
	menu->color               = color_red;
	menu->generic.flags       = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	if((s_votemenu_scorelimit.min > value && value!=0) || 
			(s_votemenu_scorelimit.max < value && s_votemenu_scorelimit.max!=0) ||
			(s_votemenu_scorelimit.max != 0 && value==0)) {
		menu->generic.flags |= QMF_INACTIVE|QMF_GRAYED;
	}
	else if(s_votemenu_scorelimit.selection == id) {
		menu->color = color_orange;
	}
	menu->generic.x           = 320;
	menu->generic.y           = y;
	menu->generic.id          = id;
	menu->generic.callback    = VoteMenu_Scorelimit_Event;
	menu->string              = text;
	menu->style               = UI_CENTER|UI_SMALLFONT;
}

/*
=================
VoteMenu_Scorelimit_Cache
=================
*/
static void VoteMenu_Scorelimit_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0;votemenu_scorelimit_artlist[i];i++) {
		trap_R_RegisterShaderNoMip(votemenu_scorelimit_artlist[i]);
	}
}

/*
=================
UI_VoteMenu_Scorelimit_Draw
=================
*/
static void UI_VoteMenu_Scorelimit_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE - SCORELIMIT", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu_scorelimit.menu );
}

/*
=================
UI_VoteScorelimitMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteScorelimitMenuInternal( void )
{
	int y;

	VoteMenu_Scorelimit_Cache();

	s_votemenu_scorelimit.menu.wrapAround = qtrue;
	s_votemenu_scorelimit.menu.fullscreen = qfalse;
	s_votemenu_scorelimit.menu.draw = UI_VoteMenu_Scorelimit_Draw;

	s_votemenu_scorelimit.banner.generic.type  = MTYPE_BTEXT;
	s_votemenu_scorelimit.banner.generic.x     = 320;
	s_votemenu_scorelimit.banner.generic.y     = 16;
	s_votemenu_scorelimit.banner.string        = "CALL VOTE - SCORELIMIT";
	s_votemenu_scorelimit.banner.color         = color_white;
	s_votemenu_scorelimit.banner.style         = UI_CENTER;

	y = 98;
	setMenutext_sl(&s_votemenu_scorelimit.bLimit10,y,ID_10,10,"100");
	y+=SCORELIMIT_MENU_VERTICAL_SPACING;
	setMenutext_sl(&s_votemenu_scorelimit.bLimit15,y,ID_15,15,"150");
	y+=SCORELIMIT_MENU_VERTICAL_SPACING;
	setMenutext_sl(&s_votemenu_scorelimit.bLimit20,y,ID_20,20,"200");
	y+=SCORELIMIT_MENU_VERTICAL_SPACING;
	setMenutext_sl(&s_votemenu_scorelimit.bLimit30,y,ID_30,30,"300");
	y+=SCORELIMIT_MENU_VERTICAL_SPACING;
	setMenutext_sl(&s_votemenu_scorelimit.bLimit40,y,ID_40,40,"400");
	y+=SCORELIMIT_MENU_VERTICAL_SPACING;
	setMenutext_sl(&s_votemenu_scorelimit.bLimit50,y,ID_50,50,"500");
	y+=SCORELIMIT_MENU_VERTICAL_SPACING;
	setMenutext_sl(&s_votemenu_scorelimit.bLimitInf,y,ID_INF,0,"No limit");

	s_votemenu_scorelimit.back.generic.type     = MTYPE_BITMAP;
	s_votemenu_scorelimit.back.generic.name     = VOTEMENU_BACK0;
	s_votemenu_scorelimit.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_votemenu_scorelimit.back.generic.callback = VoteMenu_Scorelimit_Event;
	s_votemenu_scorelimit.back.generic.id       = ID_BACK;
	s_votemenu_scorelimit.back.generic.x        = 320-128;
	s_votemenu_scorelimit.back.generic.y        = 256+128-64;
	s_votemenu_scorelimit.back.width            = 128;
	s_votemenu_scorelimit.back.height           = 64;
	s_votemenu_scorelimit.back.focuspic         = VOTEMENU_BACK1;

	s_votemenu_scorelimit.go.generic.type     = MTYPE_BITMAP;
	s_votemenu_scorelimit.go.generic.name     = ART_FIGHT0;
	s_votemenu_scorelimit.go.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_votemenu_scorelimit.go.generic.callback = VoteMenu_Scorelimit_Event;
	s_votemenu_scorelimit.go.generic.id       = ID_GO;
	s_votemenu_scorelimit.go.generic.x        = 320;
	s_votemenu_scorelimit.go.generic.y        = 256+128-64;
	s_votemenu_scorelimit.go.width            = 128;
	s_votemenu_scorelimit.go.height           = 64;
	s_votemenu_scorelimit.go.focuspic         = ART_FIGHT1;
}

/*
=================
UI_VoteScorelimitMenu
 *Called from outside
=================
*/
void UI_VoteScorelimitMenu( void ) {
	char serverinfo[MAX_INFO_STRING];
	// zero set all our globals
	memset( &s_votemenu_scorelimit, 0 ,sizeof(votemenu_t) );
	trap_GetConfigString( CS_SERVERINFO, serverinfo, MAX_INFO_STRING );
	s_votemenu_scorelimit.min = atoi(Info_ValueForKey(serverinfo,"g_voteMinScorelimit"));
	s_votemenu_scorelimit.max = atoi(Info_ValueForKey(serverinfo,"g_voteMaxScorelimit"));
	UI_VoteScorelimitMenuInternal();

	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.banner );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.back );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimit10 );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimit15 );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimit20 );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimit30 );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimit40 );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimit50 );
	Menu_AddItem( &s_votemenu_scorelimit.menu, (void*) &s_votemenu_scorelimit.bLimitInf );

	UI_PushMenu( &s_votemenu_scorelimit.menu );
}


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

static char* votemenu_fraglimit_artlist[] =
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

#define FRAGLIMIT_MENU_VERTICAL_SPACING	28

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

static votemenu_t	s_votemenu_fraglimit;

void UI_VoteFraglimitMenuInternal( void );

/*
=================
VoteMenu_Fraglimit_Event
=================
*/
static void VoteMenu_Fraglimit_Event( void* ptr, int event )
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
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 10" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_15:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 15" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_20:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 20" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_30:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 30" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_40:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 40" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_50:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 50" );
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_INF:
			trap_Cmd_ExecuteText( EXEC_APPEND, "callvote fraglimit 0" );
			UI_PopMenu();
			UI_PopMenu();
			break;
	}
}

static void setMenutext_fl(menutext_s *menu,int y,int id,int value,char *text) {
	menu->generic.type        = MTYPE_PTEXT;
	menu->color               = color_red;
	menu->generic.flags       = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
	if((s_votemenu_fraglimit.min > value && value!=0) || 
			(s_votemenu_fraglimit.max < value && s_votemenu_fraglimit.max!=0) ||
			(s_votemenu_fraglimit.max != 0 && value==0)) {
		menu->generic.flags |= QMF_INACTIVE|QMF_GRAYED;
	}
	else if(s_votemenu_fraglimit.selection == id) {
		menu->color = color_orange;
	}
	menu->generic.x           = 320;
	menu->generic.y           = y;
	menu->generic.id          = id;
	menu->generic.callback    = VoteMenu_Fraglimit_Event;
	menu->string              = text;
	menu->style               = UI_CENTER|UI_SMALLFONT;
}

/*
=================
VoteMenu_Fraglimit_Cache
=================
*/
static void VoteMenu_Fraglimit_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0;votemenu_fraglimit_artlist[i];i++) {
		trap_R_RegisterShaderNoMip(votemenu_fraglimit_artlist[i]);
	}
}

/*
=================
UI_VoteMenu_Fraglimit_Draw
=================
*/
static void UI_VoteMenu_Fraglimit_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE - FRAGLIMIT", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu_fraglimit.menu );
}

/*
=================
UI_VoteFraglimitMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteFraglimitMenuInternal( void )
{
	int y;

	VoteMenu_Fraglimit_Cache();

	s_votemenu_fraglimit.menu.wrapAround = qtrue;
	s_votemenu_fraglimit.menu.fullscreen = qfalse;
	s_votemenu_fraglimit.menu.draw = UI_VoteMenu_Fraglimit_Draw;

	s_votemenu_fraglimit.banner.generic.type  = MTYPE_BTEXT;
	s_votemenu_fraglimit.banner.generic.x     = 320;
	s_votemenu_fraglimit.banner.generic.y     = 16;
	s_votemenu_fraglimit.banner.string        = "CALL VOTE - FRAGLIMIT";
	s_votemenu_fraglimit.banner.color         = color_white;
	s_votemenu_fraglimit.banner.style         = UI_CENTER;

	y = 98;
	setMenutext_fl(&s_votemenu_fraglimit.bLimit10,y,ID_10,10,"10");
	y+=FRAGLIMIT_MENU_VERTICAL_SPACING;
	setMenutext_fl(&s_votemenu_fraglimit.bLimit15,y,ID_15,15,"15");
	y+=FRAGLIMIT_MENU_VERTICAL_SPACING;
	setMenutext_fl(&s_votemenu_fraglimit.bLimit20,y,ID_20,20,"20");
	y+=FRAGLIMIT_MENU_VERTICAL_SPACING;
	setMenutext_fl(&s_votemenu_fraglimit.bLimit30,y,ID_30,30,"30");
	y+=FRAGLIMIT_MENU_VERTICAL_SPACING;
	setMenutext_fl(&s_votemenu_fraglimit.bLimit40,y,ID_40,40,"40");
	y+=FRAGLIMIT_MENU_VERTICAL_SPACING;
	setMenutext_fl(&s_votemenu_fraglimit.bLimit50,y,ID_50,50,"50");
	y+=FRAGLIMIT_MENU_VERTICAL_SPACING;
	setMenutext_fl(&s_votemenu_fraglimit.bLimitInf,y,ID_INF,0,"No limit");

	s_votemenu_fraglimit.back.generic.type     = MTYPE_BITMAP;
	s_votemenu_fraglimit.back.generic.name     = VOTEMENU_BACK0;
	s_votemenu_fraglimit.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_votemenu_fraglimit.back.generic.callback = VoteMenu_Fraglimit_Event;
	s_votemenu_fraglimit.back.generic.id       = ID_BACK;
	s_votemenu_fraglimit.back.generic.x        = 320-128;
	s_votemenu_fraglimit.back.generic.y        = 256+128-64;
	s_votemenu_fraglimit.back.width            = 128;
	s_votemenu_fraglimit.back.height           = 64;
	s_votemenu_fraglimit.back.focuspic         = VOTEMENU_BACK1;

	s_votemenu_fraglimit.go.generic.type     = MTYPE_BITMAP;
	s_votemenu_fraglimit.go.generic.name     = ART_FIGHT0;
	s_votemenu_fraglimit.go.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_votemenu_fraglimit.go.generic.callback = VoteMenu_Fraglimit_Event;
	s_votemenu_fraglimit.go.generic.id       = ID_GO;
	s_votemenu_fraglimit.go.generic.x        = 320;
	s_votemenu_fraglimit.go.generic.y        = 256+128-64;
	s_votemenu_fraglimit.go.width            = 128;
	s_votemenu_fraglimit.go.height           = 64;
	s_votemenu_fraglimit.go.focuspic         = ART_FIGHT1;
}

/*
=================
UI_VoteFraglimitMenu
 *Called from outside
=================
*/
void UI_VoteFraglimitMenu( void ) {
	char serverinfo[MAX_INFO_STRING];
	// zero set all our globals
	memset( &s_votemenu_fraglimit, 0 ,sizeof(votemenu_t) );
	trap_GetConfigString( CS_SERVERINFO, serverinfo, MAX_INFO_STRING );
	s_votemenu_fraglimit.min = atoi(Info_ValueForKey(serverinfo,"g_voteMinFraglimit"));
	s_votemenu_fraglimit.max = atoi(Info_ValueForKey(serverinfo,"g_voteMaxFraglimit"));
	UI_VoteFraglimitMenuInternal();

	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.banner );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.back );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimit10 );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimit15 );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimit20 );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimit30 );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimit40 );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimit50 );
	Menu_AddItem( &s_votemenu_fraglimit.menu, (void*) &s_votemenu_fraglimit.bLimitInf );

	UI_PushMenu( &s_votemenu_fraglimit.menu );
}

/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
/*
=======================================================================

GAME OPTIONS MENU

=======================================================================
*/


#include "ui_local.h"


#define ART_FRAMEL				"menu/" MENU_ART_DIR "/frame2_l"
#define ART_FRAMER				"menu/" MENU_ART_DIR "/frame1_r"
#define ART_BACK0				"menu/" MENU_ART_DIR "/back_0"
#define ART_BACK1				"menu/" MENU_ART_DIR "/back_1"

#define HUDOPTIONS_X_POS		360

#define ID_BACK					127
#define ID_DRAWFPS				128


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menuradiobutton_s	drawFPS;

	menubitmap_s		back;
} hudOptions_t;

static hudOptions_t hudOptions_s;

static void HUDOptions_SetMenuItems( void ) {
	hudOptions_s.drawFPS.curvalue	= trap_Cvar_VariableValue( "cg_drawfps") != 0;
}

static void HUDOptions_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_DRAWFPS:
		trap_Cvar_SetValue( "cg_drawFPS", hudOptions_s.drawFPS.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

static void HUDOptions_MenuInit( void ) {
	int				y;

	UI_SetDefaultCvar("cg_drawFPS","0");

	memset( &hudOptions_s, 0 ,sizeof(hudOptions_t) );

	UI_HUDOptionsMenu_Cache();

	hudOptions_s.menu.wrapAround = qtrue;
	hudOptions_s.menu.fullscreen = qtrue;

	hudOptions_s.banner.generic.type	= MTYPE_BTEXT;
	hudOptions_s.banner.generic.x		= 320;
	hudOptions_s.banner.generic.y		= 16;
	hudOptions_s.banner.string			= "GAME OPTIONS";
	hudOptions_s.banner.color			= color_white;
	hudOptions_s.banner.style			= UI_CENTER;

	hudOptions_s.framel.generic.type	= MTYPE_BITMAP;
	hudOptions_s.framel.generic.name	= ART_FRAMEL;
	hudOptions_s.framel.generic.flags	= QMF_INACTIVE;
	hudOptions_s.framel.generic.x		= 0;
	hudOptions_s.framel.generic.y		= 78;
	hudOptions_s.framel.width			= 256;
	hudOptions_s.framel.height			= 329;

	hudOptions_s.framer.generic.type	= MTYPE_BITMAP;
	hudOptions_s.framer.generic.name	= ART_FRAMER;
	hudOptions_s.framer.generic.flags	= QMF_INACTIVE;
	hudOptions_s.framer.generic.x		= 376;
	hudOptions_s.framer.generic.y		= 76;
	hudOptions_s.framer.width			= 256;
	hudOptions_s.framer.height			= 334;

	y = 96;
	hudOptions_s.drawFPS.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.drawFPS.generic.name		= "Draw FPS:";
	hudOptions_s.drawFPS.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawFPS.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawFPS.generic.id			= ID_DRAWFPS;
	hudOptions_s.drawFPS.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawFPS.generic.y			= y;

	hudOptions_s.back.generic.type		= MTYPE_BITMAP;
	hudOptions_s.back.generic.name		= ART_BACK0;
	hudOptions_s.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	hudOptions_s.back.generic.callback	= HUDOptions_Event;
	hudOptions_s.back.generic.id		= ID_BACK;
	hudOptions_s.back.generic.x			= 0;
	hudOptions_s.back.generic.y			= 480-64;
	hudOptions_s.back.width				= 128;
	hudOptions_s.back.height			= 64;
	hudOptions_s.back.focuspic			= ART_BACK1;

	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.banner );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.framel );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.framer );

	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawFPS );

	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.back );

	HUDOptions_SetMenuItems();
}


/*
===============
UI_HUDOptionsMenu_Cache
===============
*/
void UI_HUDOptionsMenu_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
}


/*
===============
UI_HUDOptionsMenu
===============
*/
void UI_HUDOptionsMenu( void ) {
	HUDOptions_MenuInit();
	UI_PushMenu( &hudOptions_s.menu );
}

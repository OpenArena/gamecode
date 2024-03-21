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

#define PREFERENCES_X_POS		360

#define ID_SIMPLEITEMS			128
#define ID_HIGHQUALITYSKY		129
#define ID_EJECTINGBRASS		130
#define ID_WALLMARKS			131
#define ID_IDENTIFYTARGET		132
#define ID_SYNCEVERYFRAME		133
#define ID_FORCEMODEL			134
#define ID_DRAWTEAMOVERLAY		135
#define ID_MUZZLEFLASHSTYLE		136
#define ID_OBITUARYOUTPUT		137
#define ID_BACK					138
#define ID_CHATBEEP				139
#define ID_TEAMCHATBEEP			140

#undef NUM_CROSSHAIRS
#define	NUM_CROSSHAIRS			99


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menuradiobutton_s	simpleitems;
	menuradiobutton_s	brass;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	identifytarget;
	menuradiobutton_s	highqualitysky;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	forcemodel;
	menulist_s			drawteamoverlay;
	menulist_s			muzzleFlashStyle;
	menulist_s			obituaryOutput;
	menuradiobutton_s       chatbeep;
	menuradiobutton_s       teamchatbeep;
	menubitmap_s		back;
} preferences_t;

static preferences_t s_preferences;

static const char *teamoverlay_names[] =
{
	"off",
	"upper right",
	"lower right",
	"lower left",
	NULL
};

static const char *muzzleFlashStyle_names[] =
{
	"Off",
	"Default",
	"Q1-like",
	"99-like",
	"64-like",
	"Animesque",
	"03-like",
	NULL
};

static const char *obituaryOutput_names[] =
{
	"Disabled",
	"Only Console Text",
	"Only HUD Text",
	"Only HUD Icon",
	"HUD Icon + Console",
	NULL
};

static void Preferences_SetMenuItems( void ) {
	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
	s_preferences.brass.curvalue			= trap_Cvar_VariableValue( "cg_brassTime" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue ( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.drawteamoverlay.curvalue	= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	s_preferences.muzzleFlashStyle.curvalue	= trap_Cvar_VariableValue( "cg_muzzleFlashStyle" ) != 0;
	s_preferences.obituaryOutput.curvalue	= trap_Cvar_VariableValue( "cg_obituaryOutput" ) != 0;
	s_preferences.chatbeep.curvalue         = trap_Cvar_VariableValue( "cg_chatBeep" ) != 0;
	s_preferences.teamchatbeep.curvalue     = trap_Cvar_VariableValue( "cg_teamChatBeep" ) != 0;
}

static void Preferences_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_SIMPLEITEMS:
		trap_Cvar_SetValue( "cg_simpleItems", s_preferences.simpleitems.curvalue );
		break;

	case ID_HIGHQUALITYSKY:
		trap_Cvar_SetValue( "r_fastsky", !s_preferences.highqualitysky.curvalue );
		break;

	case ID_EJECTINGBRASS:
		if ( s_preferences.brass.curvalue )
			trap_Cvar_Reset( "cg_brassTime" );
		else
			trap_Cvar_SetValue( "cg_brassTime", 0 );
		break;

	case ID_WALLMARKS:
		trap_Cvar_SetValue( "cg_marks", s_preferences.wallmarks.curvalue );
		break;	

	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", s_preferences.identifytarget.curvalue );
		break;

	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;

	case ID_FORCEMODEL:
		trap_Cvar_SetValue( "cg_forcemodel", s_preferences.forcemodel.curvalue );
		break;

	case ID_DRAWTEAMOVERLAY:
		trap_Cvar_SetValue( "cg_drawTeamOverlay", s_preferences.drawteamoverlay.curvalue );
		break;

	case ID_MUZZLEFLASHSTYLE:
		trap_Cvar_SetValue( "cg_muzzleFlashStyle", s_preferences.muzzleFlashStyle.curvalue );
		break;

	case ID_OBITUARYOUTPUT:
		trap_Cvar_SetValue( "cg_obituaryOutput", s_preferences.obituaryOutput.curvalue );
		break;

	case ID_CHATBEEP:
		trap_Cvar_SetValue( "cg_chatBeep", s_preferences.chatbeep.curvalue );
		break;

	case ID_TEAMCHATBEEP:
		trap_Cvar_SetValue( "cg_teamChatBeep", s_preferences.teamchatbeep.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


static void Preferences_Menu_AddBoolean (menuradiobutton_s* menutext, int* y, int id, char* text ) { 
	*y += BIGCHAR_HEIGHT;
	menutext->generic.type        = MTYPE_RADIOBUTTON;
	menutext->generic.name	      = text;
	menutext->generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	menutext->generic.callback    = Preferences_Event;
	menutext->generic.id          = id;
	menutext->generic.x	          = PREFERENCES_X_POS;
	menutext->generic.y	          = *y;
}

static void Preferences_MenuInit( void ) {
	int				y;

	memset( &s_preferences, 0 ,sizeof(preferences_t) );

	Preferences_Cache();

	s_preferences.menu.wrapAround = qtrue;
	s_preferences.menu.fullscreen = qtrue;

	s_preferences.banner.generic.type  = MTYPE_BTEXT;
	s_preferences.banner.generic.x	   = 320;
	s_preferences.banner.generic.y	   = 16;
	s_preferences.banner.string		   = "GAME OPTIONS";
	s_preferences.banner.color         = color_white;
	s_preferences.banner.style         = UI_CENTER;

	s_preferences.framel.generic.type  = MTYPE_BITMAP;
	s_preferences.framel.generic.name  = ART_FRAMEL;
	s_preferences.framel.generic.flags = QMF_INACTIVE;
	s_preferences.framel.generic.x	   = 0;
	s_preferences.framel.generic.y	   = 78;
	s_preferences.framel.width  	   = 256;
	s_preferences.framel.height  	   = 329;

	s_preferences.framer.generic.type  = MTYPE_BITMAP;
	s_preferences.framer.generic.name  = ART_FRAMER;
	s_preferences.framer.generic.flags = QMF_INACTIVE;
	s_preferences.framer.generic.x	   = 376;
	s_preferences.framer.generic.y	   = 76;
	s_preferences.framer.width  	   = 256;
	s_preferences.framer.height  	   = 334;

	y = 96;
	Preferences_Menu_AddBoolean(&s_preferences.simpleitems, &y, ID_SIMPLEITEMS, "2D (Simple) Items:");
	
	Preferences_Menu_AddBoolean(&s_preferences.wallmarks, &y, ID_WALLMARKS, "Marks on Walls:");
	Preferences_Menu_AddBoolean(&s_preferences.brass, &y, ID_EJECTINGBRASS, "Ejecting Brass:");
	Preferences_Menu_AddBoolean(&s_preferences.identifytarget, &y, ID_IDENTIFYTARGET, "Identify Target:");
	Preferences_Menu_AddBoolean(&s_preferences.highqualitysky, &y, ID_HIGHQUALITYSKY, "High Quality Sky:");
	Preferences_Menu_AddBoolean(&s_preferences.synceveryframe, &y, ID_SYNCEVERYFRAME, "Sync Every Frame:");
	Preferences_Menu_AddBoolean(&s_preferences.forcemodel, &y, ID_FORCEMODEL, "Force Player Models:");
	
	y += BIGCHAR_HEIGHT+2;
	s_preferences.drawteamoverlay.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.drawteamoverlay.generic.name	   = "Draw Team Overlay:";
	s_preferences.drawteamoverlay.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawteamoverlay.generic.callback = Preferences_Event;
	s_preferences.drawteamoverlay.generic.id       = ID_DRAWTEAMOVERLAY;
	s_preferences.drawteamoverlay.generic.x	       = PREFERENCES_X_POS;
	s_preferences.drawteamoverlay.generic.y	       = y;
	s_preferences.drawteamoverlay.itemnames			= teamoverlay_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.muzzleFlashStyle.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.muzzleFlashStyle.generic.name	   = "Muzzle Flash Style:";
	s_preferences.muzzleFlashStyle.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.muzzleFlashStyle.generic.callback = Preferences_Event;
	s_preferences.muzzleFlashStyle.generic.id       = ID_MUZZLEFLASHSTYLE;
	s_preferences.muzzleFlashStyle.generic.x	       = PREFERENCES_X_POS;
	s_preferences.muzzleFlashStyle.generic.y	       = y;
	s_preferences.muzzleFlashStyle.itemnames			= muzzleFlashStyle_names;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.obituaryOutput.generic.type     = MTYPE_SPINCONTROL;
	s_preferences.obituaryOutput.generic.name	   = "Death Messages:";
	s_preferences.obituaryOutput.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.obituaryOutput.generic.callback = Preferences_Event;
	s_preferences.obituaryOutput.generic.id       = ID_OBITUARYOUTPUT;
	s_preferences.obituaryOutput.generic.x	       = PREFERENCES_X_POS;
	s_preferences.obituaryOutput.generic.y	       = y;
	s_preferences.obituaryOutput.itemnames			= obituaryOutput_names;

	Preferences_Menu_AddBoolean(&s_preferences.chatbeep, &y, ID_CHATBEEP, "Beep on Chat:");
	Preferences_Menu_AddBoolean(&s_preferences.teamchatbeep, &y, ID_TEAMCHATBEEP, "Beep on Team Chat:");
	
	s_preferences.back.generic.type	    = MTYPE_BITMAP;
	s_preferences.back.generic.name     = ART_BACK0;
	s_preferences.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.back.generic.callback = Preferences_Event;
	s_preferences.back.generic.id	    = ID_BACK;
	s_preferences.back.generic.x		= 0;
	s_preferences.back.generic.y		= 480-64;
	s_preferences.back.width  		    = 128;
	s_preferences.back.height  		    = 64;
	s_preferences.back.focuspic         = ART_BACK1;

	Menu_AddItem( &s_preferences.menu, &s_preferences.banner );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framer );

	Menu_AddItem( &s_preferences.menu, &s_preferences.simpleitems );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.brass );
	Menu_AddItem( &s_preferences.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_preferences.menu, &s_preferences.highqualitysky );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.forcemodel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawteamoverlay );
	Menu_AddItem( &s_preferences.menu, &s_preferences.muzzleFlashStyle );
	Menu_AddItem( &s_preferences.menu, &s_preferences.obituaryOutput );
	Menu_AddItem( &s_preferences.menu, &s_preferences.chatbeep );
	Menu_AddItem( &s_preferences.menu, &s_preferences.teamchatbeep );

	Menu_AddItem( &s_preferences.menu, &s_preferences.back );

	Preferences_SetMenuItems();
}


/*
===============
Preferences_Cache
===============
*/
void Preferences_Cache( void ) {
	int		n;

	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
}


/*
===============
UI_PreferencesMenu
===============
*/
void UI_PreferencesMenu( void ) {
	Preferences_MenuInit();
	UI_PushMenu( &s_preferences.menu );
}

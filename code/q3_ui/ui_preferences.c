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

#define PREFERENCES_X_POS		320

#define ID_BACK					127
#define ID_DRAWGUN				128
#define ID_SIMPLEITEMS			129
#define ID_EJECTINGBRASS		130
#define ID_WALLMARKS			131
#define ID_HIGHQUALITYSKY		132
#define ID_SYNCEVERYFRAME		133
#define ID_FORCEMODEL			134
#define ID_MUZZLEFLASHSTYLE		135
#define ID_SHOWBLOOD			136
#define ID_SHOWGIBS				137
#define ID_VIEWBOB				138
#define ID_WEAPONBOB			139
#define ID_RAILTRAILTIME		140
#define ID_KICKSCALE			141

#undef NUM_CROSSHAIRS
#define	NUM_CROSSHAIRS			99


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menulist_s			drawGun;
	menulist_s			simpleitems;
	menuradiobutton_s	brass;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	highqualitysky;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	forcemodel;
	menulist_s			muzzleFlashStyle;
	menuradiobutton_s	showBlood;
	menuradiobutton_s	showGibs;
	menulist_s			viewBob;
	menulist_s			weaponBob;
	menufield_s			railTrailTime;
	menufield_s			kickScale;

	menubitmap_s		back;
} preferences_t;

static preferences_t s_preferences;

static const char *drawGun_names[] =
{
	"Hidden",
	"Right",
	"Left",
	"Center",
	NULL
};

static const char *simpleItems_names[] =
{
	"3D",
	"2D",
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

static const char *viewBob_names[] =
{
	"Off",
	"Default",
	"No Roll",
	"No Pitch",
	"No Pitch/Roll",
	"Fake '99",
	NULL
};

static const char *weaponBob_names[] =
{
	"Normal",
	"Arc",
	"Thrust",
	"Figure 8",
	"Off",
	NULL
};

static void Preferences_SetMenuItems( void ) {
	int viewBob, weaponBob;

	s_preferences.drawGun.curvalue			= trap_Cvar_VariableValue( "cg_drawGun" ) != 0;
	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
	s_preferences.brass.curvalue			= trap_Cvar_VariableValue( "cg_brassTime" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.muzzleFlashStyle.curvalue	= trap_Cvar_VariableValue( "cg_muzzleFlashStyle" ) != 0;
	s_preferences.showBlood.curvalue		= trap_Cvar_VariableValue( "com_blood" ) != 0;
	s_preferences.showGibs.curvalue			= trap_Cvar_VariableValue( "cg_gibs" ) != 0;
	// Due to the irregular values used by cg_bob, we have to do things this way.
	viewBob									= trap_Cvar_VariableValue( "cg_bob" ) != 0;
	if (viewBob >= 0 && viewBob <= 4) {
		s_preferences.viewBob.curvalue = viewBob;
	}
	else if (viewBob == 6) {
		s_preferences.viewBob.curvalue = 5;
	}
	else { // default value
		s_preferences.viewBob.curvalue = 0;
	}
	// Due to the irregular values used by cg_bobModel, we have to do things this way.
	weaponBob								= trap_Cvar_VariableValue( "cg_bobModel" ) != 0;
	if (weaponBob >= 0 && weaponBob <= 3) {
		s_preferences.weaponBob.curvalue = weaponBob;
	}
	else if (weaponBob == -1) {
		s_preferences.weaponBob.curvalue = 4;
	}
	else { // default value
		s_preferences.weaponBob.curvalue = 0;
	}
	Q_strncpyz( s_preferences.railTrailTime.field.buffer, UI_Cvar_VariableString( "cg_railTrailTime" ), sizeof( s_preferences.railTrailTime.field.buffer ) );
	Q_strncpyz( s_preferences.kickScale.field.buffer, UI_Cvar_VariableString( "cg_kickScale" ), sizeof( s_preferences.kickScale.field.buffer ) );
}

static void Preferences_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {

	case ID_DRAWGUN:
		trap_Cvar_SetValue( "cg_drawGun", s_preferences.drawGun.curvalue );
		break;

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

	case ID_SYNCEVERYFRAME:
		trap_Cvar_SetValue( "r_finish", s_preferences.synceveryframe.curvalue );
		break;

	case ID_FORCEMODEL:
		trap_Cvar_SetValue( "cg_forcemodel", s_preferences.forcemodel.curvalue );
		break;

	case ID_MUZZLEFLASHSTYLE:
		trap_Cvar_SetValue( "cg_muzzleFlashStyle", s_preferences.muzzleFlashStyle.curvalue );
		break;

	case ID_SHOWBLOOD:
		trap_Cvar_SetValue( "com_blood", s_preferences.showBlood.curvalue );
		break;

	case ID_SHOWGIBS:
		trap_Cvar_SetValue( "cg_gibs", s_preferences.showGibs.curvalue );
		break;

	case ID_VIEWBOB:
		if (s_preferences.viewBob.curvalue >= 0 && s_preferences.viewBob.curvalue <= 4) {
			trap_Cvar_SetValue( "cg_bob", s_preferences.viewBob.curvalue);
		}
		else if (s_preferences.viewBob.curvalue == 5) {
			trap_Cvar_Set( "cg_bob", "6" );
		}
		else { // default value
			trap_Cvar_Set( "cg_bob", "0" );
		}
		break;

	case ID_WEAPONBOB:
		if (s_preferences.viewBob.curvalue >= 0 && s_preferences.viewBob.curvalue <= 3) {
			trap_Cvar_SetValue( "cg_bobModel", s_preferences.viewBob.curvalue);
		}
		else if (s_preferences.viewBob.curvalue == 4) {
			trap_Cvar_Set( "cg_bobModel", "-1" );
		}
		else { // default value
			trap_Cvar_Set( "cg_bobModel", "0" );
		}
		break;

	case ID_RAILTRAILTIME:
		trap_Cvar_Set("cg_railTrailTime", s_preferences.railTrailTime.field.buffer );
		break;

	case ID_KICKSCALE:
		trap_Cvar_Set("cg_kickScale", s_preferences.kickScale.field.buffer );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

/*
=================
Preferences_StatusBar_WeaponHand

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_WeaponHand( void* ptr ) {
	switch (s_preferences.drawGun.curvalue) {
		case 1:
			UI_DrawString( 320, 440, "Right: The weapon is rendered as carried", UI_CENTER|UI_SMALLFONT, colorWhite );
			UI_DrawString( 320, 460, "by a right-handed user.", UI_CENTER|UI_SMALLFONT, colorWhite );
			break;
		case 2:
			UI_DrawString( 320, 440, "Left: The weapon is rendered as carried", UI_CENTER|UI_SMALLFONT, colorWhite );
			UI_DrawString( 320, 460, "by a left-handed user.", UI_CENTER|UI_SMALLFONT, colorWhite );
			break;
		case 3:
			UI_DrawString( 320, 440, "Center: The weapon is rendered at", UI_CENTER|UI_SMALLFONT, colorWhite );
			UI_DrawString( 320, 460, "the very center of the screen.", UI_CENTER|UI_SMALLFONT, colorWhite );
			break;
		default: // case 0
			UI_DrawString( 320, 440, "Hidden: The weapon carrying", UI_CENTER|UI_SMALLFONT, colorWhite );
			UI_DrawString( 320, 460, "isn't rendered in the screen.", UI_CENTER|UI_SMALLFONT, colorWhite );
			break;
	}
}

/*
=================
Preferences_StatusBar_SimpleItems

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_SimpleItems( void* ptr ) {
	UI_DrawString( 320, 440, "Specifies if the icons in the pickup spawning", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "places should be rendered in 2D or 3D.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_MarksOnWalls

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_MarksOnWalls( void* ptr ) {
	UI_DrawString( 320, 440, "If set, weapons whose projectiles hit", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "walls will leave a mark on them.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_EjectingBrass

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_EjectingBrass( void* ptr ) {
	UI_DrawString( 320, 440, "If set, whenever the Shotgun fires, a shell", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "casing will stay in the floor for a while.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_HighQualitySky

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_HighQualitySky( void* ptr ) {
	UI_DrawString( 320, 440, "If set, the sky is rendered at full quality.", UI_CENTER|UI_SMALLFONT, colorWhite );
	// UI_DrawString( 320, 460, "", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_SyncEveryFrame

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_SyncEveryFrame( void* ptr ) {
	UI_DrawString( 320, 440, "If set, improves response between", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "the input and the on-screen action.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_ForceModels

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_ForceModels( void* ptr ) {
	UI_DrawString( 320, 440, "If set, makes every opponent", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "appear as your own character.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_MuzzleFlashStyle

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_MuzzleFlashStyle( void* ptr ) {
	UI_DrawString( 320, 440, "Selects a rendering style for the muzzle flash,", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "a.k.a. the flash a weapon makes when shooting.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_ShowBlood

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_ShowBlood( void* ptr ) {
	UI_DrawString( 320, 440, "If set, characters will drop blood when hit,", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "and leave a small chunk when hit hard.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_ShowGibs

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_ShowGibs( void* ptr ) {
	UI_DrawString( 320, 440, "If set, characters will explode in a shower", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "of gibs when dealt enough lethal damage.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_ViewBobbing

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_ViewBobbing( void* ptr ) {
	UI_DrawString( 320, 440, "Displays how the screen will bob", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "when the player is running/walking.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_WeaponBobbing

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_WeaponBobbing( void* ptr ) {
	UI_DrawString( 320, 440, "Displays how the weapon will bob", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "when the player is running/walking.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_RailTrailTime

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_RailTrailTime( void* ptr ) {
	UI_DrawString( 320, 440, "Specifies the seconds the trail left by a slug", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "after firing the Railgun stays on the screen.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
Preferences_StatusBar_KickScale

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void Preferences_StatusBar_KickScale( void* ptr ) {
	UI_DrawString( 320, 440, "Specifies how much the screen shakes", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "when your character is hit.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

static void Preferences_MenuInit( void ) {
	//int				y;

	UI_SetDefaultCvar("cg_drawGun","1");
	UI_SetDefaultCvar("cg_simpleItems","0");
	UI_SetDefaultCvar("r_fastsky","0");
	UI_SetDefaultCvar("cg_brassTime","0");
	UI_SetDefaultCvar("cg_railTrailTime","600");
	UI_SetDefaultCvar("cg_kickScale","1.0");
	UI_SetDefaultCvar("cg_bob","1");
	UI_SetDefaultCvar("cg_bobModel","0");

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

	//y = 128;
	s_preferences.drawGun.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.drawGun.generic.name		= "Weapon Hand:";
	s_preferences.drawGun.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.drawGun.generic.callback	= Preferences_Event;
	s_preferences.drawGun.generic.id		= ID_DRAWGUN;
	s_preferences.drawGun.generic.x			= PREFERENCES_X_POS;
	s_preferences.drawGun.generic.y			= 240 - 6.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.drawGun.itemnames			= drawGun_names;
	s_preferences.drawGun.generic.statusbar	= Preferences_StatusBar_WeaponHand;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.simpleitems.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.simpleitems.generic.name		= "In-Game Pickup Rendering:";
	s_preferences.simpleitems.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.simpleitems.generic.callback	= Preferences_Event;
	s_preferences.simpleitems.generic.id		= ID_SIMPLEITEMS;
	s_preferences.simpleitems.generic.x			= PREFERENCES_X_POS;
	s_preferences.simpleitems.generic.y			= 240 - 5.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.simpleitems.itemnames			= simpleItems_names;
	s_preferences.simpleitems.generic.statusbar	= Preferences_StatusBar_SimpleItems;
	
	//y += BIGCHAR_HEIGHT+2;
	s_preferences.wallmarks.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.wallmarks.generic.name		= "Show Marks on Walls:";
	s_preferences.wallmarks.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.wallmarks.generic.callback	= Preferences_Event;
	s_preferences.wallmarks.generic.id			= ID_WALLMARKS;
	s_preferences.wallmarks.generic.x			= PREFERENCES_X_POS;
	s_preferences.wallmarks.generic.y			= 240 - 4.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.wallmarks.generic.statusbar	= Preferences_StatusBar_MarksOnWalls;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.brass.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.brass.generic.name		= "Show Ejecting Brass:";
	s_preferences.brass.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.brass.generic.callback	= Preferences_Event;
	s_preferences.brass.generic.id			= ID_EJECTINGBRASS;
	s_preferences.brass.generic.x			= PREFERENCES_X_POS;
	s_preferences.brass.generic.y			= 240 - 3.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.brass.generic.statusbar	= Preferences_StatusBar_EjectingBrass;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.highqualitysky.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.highqualitysky.generic.name		= "Show High Quality Sky:";
	s_preferences.highqualitysky.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.highqualitysky.generic.callback	= Preferences_Event;
	s_preferences.highqualitysky.generic.id			= ID_HIGHQUALITYSKY;
	s_preferences.highqualitysky.generic.x			= PREFERENCES_X_POS;
	s_preferences.highqualitysky.generic.y			= 240 - 2.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.highqualitysky.generic.statusbar	= Preferences_StatusBar_HighQualitySky;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.synceveryframe.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.synceveryframe.generic.name		= "Sync Every Frame:";
	s_preferences.synceveryframe.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.synceveryframe.generic.callback	= Preferences_Event;
	s_preferences.synceveryframe.generic.id			= ID_SYNCEVERYFRAME;
	s_preferences.synceveryframe.generic.x			= PREFERENCES_X_POS;
	s_preferences.synceveryframe.generic.y			= 240 - 1.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.synceveryframe.generic.statusbar	= Preferences_StatusBar_SyncEveryFrame;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.forcemodel.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.forcemodel.generic.name		= "Force Player Models:";
	s_preferences.forcemodel.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.forcemodel.generic.callback	= Preferences_Event;
	s_preferences.forcemodel.generic.id			= ID_FORCEMODEL;
	s_preferences.forcemodel.generic.x			= PREFERENCES_X_POS;
	s_preferences.forcemodel.generic.y			= 240 - 0.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.forcemodel.generic.statusbar	= Preferences_StatusBar_ForceModels;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.muzzleFlashStyle.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.muzzleFlashStyle.generic.name		= "Muzzle Flash Style:";
	s_preferences.muzzleFlashStyle.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.muzzleFlashStyle.generic.callback	= Preferences_Event;
	s_preferences.muzzleFlashStyle.generic.id		= ID_MUZZLEFLASHSTYLE;
	s_preferences.muzzleFlashStyle.generic.x		= PREFERENCES_X_POS;
	s_preferences.muzzleFlashStyle.generic.y		= 240 + 0.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.muzzleFlashStyle.itemnames		= muzzleFlashStyle_names;
	s_preferences.muzzleFlashStyle.generic.statusbar	= Preferences_StatusBar_MuzzleFlashStyle;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.showBlood.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.showBlood.generic.name		= "Enable Blood:";
	s_preferences.showBlood.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.showBlood.generic.callback	= Preferences_Event;
	s_preferences.showBlood.generic.id			= ID_SHOWBLOOD;
	s_preferences.showBlood.generic.x			= PREFERENCES_X_POS;
	s_preferences.showBlood.generic.y			= 240 + 1.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.showBlood.generic.statusbar	= Preferences_StatusBar_ShowBlood;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.showGibs.generic.type		= MTYPE_RADIOBUTTON;
	s_preferences.showGibs.generic.name		= "Enable Gibs:";
	s_preferences.showGibs.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.showGibs.generic.callback	= Preferences_Event;
	s_preferences.showGibs.generic.id		= ID_SHOWGIBS;
	s_preferences.showGibs.generic.x		= PREFERENCES_X_POS;
	s_preferences.showGibs.generic.y		= 240 + 2.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.showGibs.generic.statusbar	= Preferences_StatusBar_ShowGibs;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.viewBob.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.viewBob.generic.name		= "View Bobbing:";
	s_preferences.viewBob.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.viewBob.generic.callback	= Preferences_Event;
	s_preferences.viewBob.generic.id		= ID_VIEWBOB;
	s_preferences.viewBob.generic.x			= PREFERENCES_X_POS;
	s_preferences.viewBob.generic.y			= 240 + 3.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.viewBob.itemnames			= viewBob_names;
	s_preferences.viewBob.generic.statusbar	= Preferences_StatusBar_ViewBobbing;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.weaponBob.generic.type		= MTYPE_SPINCONTROL;
	s_preferences.weaponBob.generic.name		= "Weapon Bobbing:";
	s_preferences.weaponBob.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.weaponBob.generic.callback	= Preferences_Event;
	s_preferences.weaponBob.generic.id			= ID_VIEWBOB;
	s_preferences.weaponBob.generic.x			= PREFERENCES_X_POS;
	s_preferences.weaponBob.generic.y			= 240 + 4.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.weaponBob.itemnames			= weaponBob_names;
	s_preferences.weaponBob.generic.statusbar	= Preferences_StatusBar_WeaponBobbing;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.railTrailTime.generic.type		= MTYPE_FIELD;
	s_preferences.railTrailTime.generic.name		= "Railgun Slug Trail Time:";
	s_preferences.railTrailTime.generic.flags		= QMF_SMALLFONT;
	s_preferences.railTrailTime.generic.id			= ID_RAILTRAILTIME;
	s_preferences.railTrailTime.generic.x			= PREFERENCES_X_POS;
	s_preferences.railTrailTime.generic.y			= 240 + 5.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.railTrailTime.field.widthInChars	= 6;
	s_preferences.railTrailTime.field.maxchars		= 6;
	s_preferences.railTrailTime.generic.callback	= Preferences_Event;
	s_preferences.railTrailTime.generic.statusbar	= Preferences_StatusBar_RailTrailTime;

	//y += BIGCHAR_HEIGHT+2;
	s_preferences.kickScale.generic.type		= MTYPE_FIELD;
	s_preferences.kickScale.generic.name		= "Screen Shaking Rate:";
	s_preferences.kickScale.generic.flags		= QMF_SMALLFONT;
	s_preferences.kickScale.generic.id			= ID_KICKSCALE;
	s_preferences.kickScale.generic.x			= PREFERENCES_X_POS;
	s_preferences.kickScale.generic.y			= 240 + 6.5 * (BIGCHAR_HEIGHT+2);
	s_preferences.kickScale.field.widthInChars	= 6;
	s_preferences.kickScale.field.maxchars		= 6;
	s_preferences.kickScale.generic.callback	= Preferences_Event;
	s_preferences.kickScale.generic.statusbar	= Preferences_StatusBar_KickScale;

	s_preferences.back.generic.type		= MTYPE_BITMAP;
	s_preferences.back.generic.name		= ART_BACK0;
	s_preferences.back.generic.flags	= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_preferences.back.generic.callback	= Preferences_Event;
	s_preferences.back.generic.id		= ID_BACK;
	s_preferences.back.generic.x		= 0;
	s_preferences.back.generic.y		= 480-64;
	s_preferences.back.width			= 128;
	s_preferences.back.height			= 64;
	s_preferences.back.focuspic			= ART_BACK1;

	Menu_AddItem( &s_preferences.menu, &s_preferences.banner );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.framer );

	Menu_AddItem( &s_preferences.menu, &s_preferences.drawGun );
	Menu_AddItem( &s_preferences.menu, &s_preferences.simpleitems );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.brass );
	Menu_AddItem( &s_preferences.menu, &s_preferences.highqualitysky );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.forcemodel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.muzzleFlashStyle );
	Menu_AddItem( &s_preferences.menu, &s_preferences.showBlood );
	Menu_AddItem( &s_preferences.menu, &s_preferences.showGibs );
	Menu_AddItem( &s_preferences.menu, &s_preferences.viewBob );
	Menu_AddItem( &s_preferences.menu, &s_preferences.weaponBob );
	Menu_AddItem( &s_preferences.menu, &s_preferences.railTrailTime );
	Menu_AddItem( &s_preferences.menu, &s_preferences.kickScale );

	Menu_AddItem( &s_preferences.menu, &s_preferences.back );

	Preferences_SetMenuItems();
}


/*
===============
Preferences_Cache
===============
*/
void Preferences_Cache( void ) {
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

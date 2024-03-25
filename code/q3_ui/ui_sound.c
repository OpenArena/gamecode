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

SOUND OPTIONS MENU

=======================================================================
*/

#include "ui_local.h"


#define ART_FRAMEL			"menu/" MENU_ART_DIR "/frame2_l"
#define ART_FRAMER			"menu/" MENU_ART_DIR "/frame1_r"
#define ART_BACK0			"menu/" MENU_ART_DIR "/back_0"
#define ART_BACK1			"menu/" MENU_ART_DIR "/back_1"
#define ART_ACCEPT0			"menu/" MENU_ART_DIR "/accept_0"
#define ART_ACCEPT1			"menu/" MENU_ART_DIR "/accept_1"

#define ID_GRAPHICS			10
#define ID_DISPLAY			11
#define ID_SOUND			12
#define ID_NETWORK			13
#define ID_EFFECTSVOLUME	14
#define ID_MUSICVOLUME		15
#define ID_QUALITY			16
//#define ID_A3D				17
//Sago: Here I do some stuff!
#define ID_OPENAL			17
#define ID_BACK				18
#define ID_APPLY			19
#define ID_CHATBEEP			20
#define ID_TEAMCHATBEEP		21
#define ID_HITSOUND			22

static const char *quality_items[] = {
	"Low", "High", NULL
};

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menutext_s			graphics;
	menutext_s			display;
	menutext_s			sound;
	menutext_s			network;

	menuslider_s		sfxvolume;
	menuslider_s		musicvolume;
	menulist_s			quality;
//	menuradiobutton_s	a3d;
	menuradiobutton_s	openal;
	menuradiobutton_s	chatBeep;
	menuradiobutton_s	teamChatBeep;
	menuradiobutton_s	hitSound;

	menubitmap_s		back;
	menubitmap_s		apply;
} soundOptionsInfo_t;

typedef struct {
	int quality;
	int openal;
} InitialSoundOptions_s;

static InitialSoundOptions_s	s_iso;
static soundOptionsInfo_t	soundOptionsInfo;


/*
=================
UI_SoundOptions_GetInitialSound
=================
*/
static void UI_SoundOptions_GetInitialSound( void )
{
	s_iso.quality = soundOptionsInfo.quality.curvalue;
	s_iso.openal = soundOptionsInfo.openal.curvalue;
}


/*
=================
UI_SoundOptions_UpdateMenuItems
=================
*/
static void UI_SoundOptions_UpdateMenuItems( void )
{
	soundOptionsInfo.apply.generic.flags |= QMF_HIDDEN|QMF_INACTIVE;

	if ( s_iso.quality != soundOptionsInfo.quality.curvalue )
	{
		soundOptionsInfo.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
	if ( s_iso.openal != soundOptionsInfo.openal.curvalue )
	{
		soundOptionsInfo.apply.generic.flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
	}
}


/*
=================
UI_SoundOptionsMenu_Event
=================
*/
static void UI_SoundOptionsMenu_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_GRAPHICS:
		UI_PopMenu();
		UI_GraphicsOptionsMenu();
		break;

	case ID_DISPLAY:
		UI_PopMenu();
		UI_DisplayOptionsMenu();
		break;

	case ID_SOUND:
		break;

	case ID_NETWORK:
		UI_PopMenu();
		UI_NetworkOptionsMenu();
		break;

	case ID_EFFECTSVOLUME:
		trap_Cvar_SetValue( "s_volume", soundOptionsInfo.sfxvolume.curvalue / 10 );
		break;

	case ID_MUSICVOLUME:
		trap_Cvar_SetValue( "s_musicvolume", soundOptionsInfo.musicvolume.curvalue / 10 );
		break;

	case ID_QUALITY:
		if( soundOptionsInfo.quality.curvalue ) {
			trap_Cvar_SetValue( "s_khz", 22 );
			trap_Cvar_SetValue( "s_compression", 0 );
		}
		else {
			trap_Cvar_SetValue( "s_khz", 11 );
			trap_Cvar_SetValue( "s_compression", 1 );
		}
		break;
/*
	case ID_A3D:
		if( soundOptionsInfo.a3d.curvalue ) {
			trap_Cmd_ExecuteText( EXEC_NOW, "s_enable_a3d\n" );
		}
		else {
			trap_Cmd_ExecuteText( EXEC_NOW, "s_disable_a3d\n" );
		}
		soundOptionsInfo.a3d.curvalue = (int)trap_Cvar_VariableValue( "s_usingA3D" );
		break;
*/

	case ID_OPENAL:
		if( soundOptionsInfo.openal.curvalue ) {
			trap_Cvar_SetValue( "s_useopenal", 1 );
		}
		else {
			trap_Cvar_SetValue( "s_useopenal", 0 );
		}
		break;

	case ID_CHATBEEP:
		trap_Cvar_SetValue( "cg_chatBeep", soundOptionsInfo.chatBeep.curvalue );
		break;

	case ID_TEAMCHATBEEP:
		trap_Cvar_SetValue( "cg_teamChatBeep", soundOptionsInfo.teamChatBeep.curvalue );
		break;

	case ID_HITSOUND:
		trap_Cvar_SetValue( "cg_hitSound", soundOptionsInfo.hitSound.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;

	case ID_APPLY:
		trap_Cmd_ExecuteText( EXEC_APPEND, "snd_restart\n" );
		break;
	}
}


/*
================
UI_SoundOptions_MenuDraw
================
*/
void UI_SoundOptions_MenuDraw (void)
{
	UI_SoundOptions_UpdateMenuItems();

	Menu_Draw( &soundOptionsInfo.menu );
}


/*
===============
UI_SoundOptionsMenu_Init
===============
*/
static void UI_SoundOptionsMenu_Init( void ) {
	//int				y;

	memset( &soundOptionsInfo, 0, sizeof(soundOptionsInfo) );

	UI_SoundOptionsMenu_Cache();
	soundOptionsInfo.menu.wrapAround = qtrue;
	soundOptionsInfo.menu.fullscreen = qtrue;
	soundOptionsInfo.menu.draw       = UI_SoundOptions_MenuDraw;

	soundOptionsInfo.banner.generic.type		= MTYPE_BTEXT;
	soundOptionsInfo.banner.generic.flags		= QMF_CENTER_JUSTIFY;
	soundOptionsInfo.banner.generic.x			= 320;
	soundOptionsInfo.banner.generic.y			= 16;
	soundOptionsInfo.banner.string				= "SYSTEM SETUP";
	soundOptionsInfo.banner.color				= color_white;
	soundOptionsInfo.banner.style				= UI_CENTER;

	soundOptionsInfo.framel.generic.type		= MTYPE_BITMAP;
	soundOptionsInfo.framel.generic.name		= ART_FRAMEL;
	soundOptionsInfo.framel.generic.flags		= QMF_INACTIVE;
	soundOptionsInfo.framel.generic.x			= 0;  
	soundOptionsInfo.framel.generic.y			= 78;
	soundOptionsInfo.framel.width				= 256;
	soundOptionsInfo.framel.height				= 329;

	soundOptionsInfo.framer.generic.type		= MTYPE_BITMAP;
	soundOptionsInfo.framer.generic.name		= ART_FRAMER;
	soundOptionsInfo.framer.generic.flags		= QMF_INACTIVE;
	soundOptionsInfo.framer.generic.x			= 376;
	soundOptionsInfo.framer.generic.y			= 76;
	soundOptionsInfo.framer.width				= 256;
	soundOptionsInfo.framer.height				= 334;

	soundOptionsInfo.graphics.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.graphics.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.graphics.generic.id		= ID_GRAPHICS;
	soundOptionsInfo.graphics.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.graphics.generic.x			= 216;
	soundOptionsInfo.graphics.generic.y			= 240 - 2 * PROP_HEIGHT;
	soundOptionsInfo.graphics.string			= "GRAPHICS";
	soundOptionsInfo.graphics.style				= UI_RIGHT;
	soundOptionsInfo.graphics.color				= color_red;

	soundOptionsInfo.display.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.display.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.display.generic.id			= ID_DISPLAY;
	soundOptionsInfo.display.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.display.generic.x			= 216;
	soundOptionsInfo.display.generic.y			= 240 - PROP_HEIGHT;
	soundOptionsInfo.display.string				= "DISPLAY";
	soundOptionsInfo.display.style				= UI_RIGHT;
	soundOptionsInfo.display.color				= color_red;

	soundOptionsInfo.sound.generic.type			= MTYPE_PTEXT;
	soundOptionsInfo.sound.generic.flags		= QMF_RIGHT_JUSTIFY;
	soundOptionsInfo.sound.generic.id			= ID_SOUND;
	soundOptionsInfo.sound.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.sound.generic.x			= 216;
	soundOptionsInfo.sound.generic.y			= 240;
	soundOptionsInfo.sound.string				= "SOUND";
	soundOptionsInfo.sound.style				= UI_RIGHT;
	soundOptionsInfo.sound.color				= color_red;

	soundOptionsInfo.network.generic.type		= MTYPE_PTEXT;
	soundOptionsInfo.network.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.network.generic.id			= ID_NETWORK;
	soundOptionsInfo.network.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.network.generic.x			= 216;
	soundOptionsInfo.network.generic.y			= 240 + PROP_HEIGHT;
	soundOptionsInfo.network.string				= "NETWORK";
	soundOptionsInfo.network.style				= UI_RIGHT;
	soundOptionsInfo.network.color				= color_red;

	//y = 240 - 3 * (BIGCHAR_HEIGHT + 2);
	soundOptionsInfo.sfxvolume.generic.type		= MTYPE_SLIDER;
	soundOptionsInfo.sfxvolume.generic.name		= "Effects Volume:";
	soundOptionsInfo.sfxvolume.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.sfxvolume.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.sfxvolume.generic.id		= ID_EFFECTSVOLUME;
	soundOptionsInfo.sfxvolume.generic.x		= 400;
	soundOptionsInfo.sfxvolume.generic.y		= 240 - 3 * (BIGCHAR_HEIGHT + 2);
	soundOptionsInfo.sfxvolume.minvalue			= 0;
	soundOptionsInfo.sfxvolume.maxvalue			= 10;

	//y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.musicvolume.generic.type		= MTYPE_SLIDER;
	soundOptionsInfo.musicvolume.generic.name		= "Music Volume:";
	soundOptionsInfo.musicvolume.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.musicvolume.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.musicvolume.generic.id			= ID_MUSICVOLUME;
	soundOptionsInfo.musicvolume.generic.x			= 400;
	soundOptionsInfo.musicvolume.generic.y			= 240 - 2 * (BIGCHAR_HEIGHT + 2);
	soundOptionsInfo.musicvolume.minvalue			= 0;
	soundOptionsInfo.musicvolume.maxvalue			= 10;

	//y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.quality.generic.type		= MTYPE_SPINCONTROL;
	soundOptionsInfo.quality.generic.name		= "Sound Quality:";
	soundOptionsInfo.quality.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.quality.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.quality.generic.id			= ID_QUALITY;
	soundOptionsInfo.quality.generic.x			= 400;
	soundOptionsInfo.quality.generic.y			= 240 - (BIGCHAR_HEIGHT + 2);
	soundOptionsInfo.quality.itemnames			= quality_items;
/*
	y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.a3d.generic.type			= MTYPE_RADIOBUTTON;
	soundOptionsInfo.a3d.generic.name			= "A3D:";
	soundOptionsInfo.a3d.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.a3d.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.a3d.generic.id				= ID_A3D;
	soundOptionsInfo.a3d.generic.x				= 400;
	soundOptionsInfo.a3d.generic.y				= y;
*/
	//y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.openal.generic.type			= MTYPE_RADIOBUTTON;
	soundOptionsInfo.openal.generic.name			= "OpenAL:";
	soundOptionsInfo.openal.generic.flags			= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.openal.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.openal.generic.id				= ID_OPENAL;
	soundOptionsInfo.openal.generic.x				= 400;
	soundOptionsInfo.openal.generic.y				= 240;

	//y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.chatBeep.generic.type		= MTYPE_RADIOBUTTON;
	soundOptionsInfo.chatBeep.generic.name		= "Beep on Chat:";
	soundOptionsInfo.chatBeep.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.chatBeep.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.chatBeep.generic.id		= ID_CHATBEEP;
	soundOptionsInfo.chatBeep.generic.x			= 400;
	soundOptionsInfo.chatBeep.generic.y			= 240 + (BIGCHAR_HEIGHT + 2);

	//y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.teamChatBeep.generic.type		= MTYPE_RADIOBUTTON;
	soundOptionsInfo.teamChatBeep.generic.name		= "Beep on Team Chat:";
	soundOptionsInfo.teamChatBeep.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.teamChatBeep.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.teamChatBeep.generic.id		= ID_TEAMCHATBEEP;
	soundOptionsInfo.teamChatBeep.generic.x			= 400;
	soundOptionsInfo.teamChatBeep.generic.y			= 240 + 2 * (BIGCHAR_HEIGHT + 2);

	//y += BIGCHAR_HEIGHT+2;
	soundOptionsInfo.hitSound.generic.type		= MTYPE_RADIOBUTTON;
	soundOptionsInfo.hitSound.generic.name		= "Enable Hitsound:";
	soundOptionsInfo.hitSound.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	soundOptionsInfo.hitSound.generic.callback	= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.hitSound.generic.id		= ID_HITSOUND;
	soundOptionsInfo.hitSound.generic.x			= 400;
	soundOptionsInfo.hitSound.generic.y			= 240 + 3 * (BIGCHAR_HEIGHT + 2);

	soundOptionsInfo.back.generic.type			= MTYPE_BITMAP;
	soundOptionsInfo.back.generic.name			= ART_BACK0;
	soundOptionsInfo.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	soundOptionsInfo.back.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.back.generic.id			= ID_BACK;
	soundOptionsInfo.back.generic.x				= 0;
	soundOptionsInfo.back.generic.y				= 480-64;
	soundOptionsInfo.back.width					= 128;
	soundOptionsInfo.back.height				= 64;
	soundOptionsInfo.back.focuspic				= ART_BACK1;

	soundOptionsInfo.apply.generic.type			= MTYPE_BITMAP;
	soundOptionsInfo.apply.generic.name			= ART_ACCEPT0;
	soundOptionsInfo.apply.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_HIDDEN|QMF_INACTIVE;
	soundOptionsInfo.apply.generic.callback		= UI_SoundOptionsMenu_Event;
	soundOptionsInfo.apply.generic.id			= ID_APPLY;
	soundOptionsInfo.apply.generic.x			= 640;
	soundOptionsInfo.apply.generic.y			= 480-64;
	soundOptionsInfo.apply.width				= 128;
	soundOptionsInfo.apply.height				= 64;
	soundOptionsInfo.apply.focuspic				= ART_ACCEPT1;

	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.banner );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.framel );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.framer );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.graphics );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.display );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.sound );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.network );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.sfxvolume );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.musicvolume );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.quality );
//	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.a3d );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.openal );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.chatBeep );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.teamChatBeep );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.hitSound );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.back );
	Menu_AddItem( &soundOptionsInfo.menu, ( void * ) &soundOptionsInfo.apply );

	soundOptionsInfo.sfxvolume.curvalue = trap_Cvar_VariableValue( "s_volume" ) * 10;
	soundOptionsInfo.musicvolume.curvalue = trap_Cvar_VariableValue( "s_musicvolume" ) * 10;
	soundOptionsInfo.quality.curvalue = !trap_Cvar_VariableValue( "s_compression" );
//	soundOptionsInfo.a3d.curvalue = (int)trap_Cvar_VariableValue( "s_usingA3D" );
	soundOptionsInfo.openal.curvalue = (int)trap_Cvar_VariableValue( "s_useopenal" );
	soundOptionsInfo.chatBeep.curvalue = (int)trap_Cvar_VariableValue( "cg_chatBeep" );
	soundOptionsInfo.teamChatBeep.curvalue = (int)trap_Cvar_VariableValue( "cg_teamChatBeep" );
	soundOptionsInfo.hitSound.curvalue = (int)trap_Cvar_VariableValue( "cg_hitSound" );

	UI_SoundOptions_GetInitialSound();
}


/*
===============
UI_SoundOptionsMenu_Cache
===============
*/
void UI_SoundOptionsMenu_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_ACCEPT0 );
	trap_R_RegisterShaderNoMip( ART_ACCEPT1 );
}


/*
===============
UI_SoundOptionsMenu
===============
*/
void UI_SoundOptionsMenu( void ) {
	UI_SoundOptionsMenu_Init();
	UI_PushMenu( &soundOptionsInfo.menu );
	Menu_SetCursorToItem( &soundOptionsInfo.menu, &soundOptionsInfo.sound );
}

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
#define ID_CROSSHAIR			128
#define ID_COLORRED				129
#define ID_COLORGREEN			130
#define ID_COLORBLUE			131
#define ID_CROSSHAIRHEALTH		132
#define ID_DRAWFPS				133


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menulist_s			crosshair;
	menuradiobutton_s	crosshairHealth;

	//Crosshair colors:
	menuslider_s		crosshairColorRed;
	menuslider_s		crosshairColorGreen;
	menuslider_s		crosshairColorBlue;

	menuradiobutton_s	alwaysWeaponBar;
	menulist_s			weaponBarStyle;

	menuradiobutton_s	drawFPS;

	menubitmap_s		back;

	qhandle_t			crosshairShader[NUM_CROSSHAIRS];
} hudOptions_t;

static hudOptions_t hudOptions_s;

static const char *weaponBarStyle_names[] =
{
	"Bottom Side (Default)",
	"Bottom Side + Ammo Bar",
	"Left Side + Ammo Count",
	"Left Side + Ammo Bar",
	"Left + Ammo Background",
	"Bottom + Ammo Count + BGD",
	"Bottom + Ammo Count + Bar",
	"Bottom + Ammo Background",
	NULL
};

static void HUDOptions_SetMenuItems( void ) {
	hudOptions_s.crosshair.curvalue				= (int)trap_Cvar_VariableValue( "cg_drawCrosshair" ) % NUM_CROSSHAIRS;
	hudOptions_s.crosshairHealth.curvalue		= trap_Cvar_VariableValue( "cg_crosshairHealth") != 0;
	hudOptions_s.crosshairColorRed.curvalue		= trap_Cvar_VariableValue( "cg_crosshairColorRed")*255.0f;
	hudOptions_s.crosshairColorGreen.curvalue	= trap_Cvar_VariableValue( "cg_crosshairColorGreen")*255.0f;
	hudOptions_s.crosshairColorBlue.curvalue	= trap_Cvar_VariableValue( "cg_crosshairColorBlue")*255.0f;
	hudOptions_s.alwaysWeaponBar.curvalue		= trap_Cvar_VariableValue( "cg_alwaysWeaponBar" ) != 0;
	hudOptions_s.weaponBarStyle.curvalue		= trap_Cvar_VariableValue( "cg_weaponBarStyle" ) != 0;
	hudOptions_s.drawFPS.curvalue				= trap_Cvar_VariableValue( "cg_drawfps") != 0;
}

static void HUDOptions_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_CROSSHAIR:
		hudOptions_s.crosshair.curvalue++;
		if( hudOptions_s.crosshair.curvalue == NUM_CROSSHAIRS || hudOptions_s.crosshairShader[hudOptions_s.crosshair.curvalue]==0 ) {
			hudOptions_s.crosshair.curvalue = 0;
		}
		trap_Cvar_SetValue( "cg_drawCrosshair", hudOptions_s.crosshair.curvalue );
		break;

	case ID_CROSSHAIRHEALTH:
		trap_Cvar_SetValue( "cg_crosshairHealth", hudOptions_s.crosshairHealth.curvalue );
		if(hudOptions_s.crosshairHealth.curvalue) {
			//If crosshairHealth is on: Don't allow color selection
			hudOptions_s.crosshairColorRed.generic.flags       |= QMF_INACTIVE;
			hudOptions_s.crosshairColorGreen.generic.flags     |= QMF_INACTIVE;
			hudOptions_s.crosshairColorBlue.generic.flags      |= QMF_INACTIVE;
		} else {
			//If crosshairHealth is off: Allow color selection
			hudOptions_s.crosshairColorRed.generic.flags       &= ~QMF_INACTIVE;
			hudOptions_s.crosshairColorGreen.generic.flags     &= ~QMF_INACTIVE;
			hudOptions_s.crosshairColorBlue.generic.flags      &= ~QMF_INACTIVE;
		}
		break;

	case ID_COLORRED:
		trap_Cvar_SetValue( "cg_crosshairColorRed", ((float)hudOptions_s.crosshairColorRed.curvalue)/255.f );
		break;

	case ID_COLORGREEN:
		trap_Cvar_SetValue( "cg_crosshairColorGreen", ((float)hudOptions_s.crosshairColorGreen.curvalue)/255.f );
		break;

	case ID_COLORBLUE:
		trap_Cvar_SetValue( "cg_crosshairColorBlue", ((float)hudOptions_s.crosshairColorBlue.curvalue)/255.f );
		break;

	case ID_ALWAYSWEAPONBAR:
		trap_Cvar_SetValue( "cg_alwaysWeaponBar", hudOptions_s.alwaysWeaponBar.curvalue );
		break;

	case ID_WEAPONBARSTYLE:
		trap_Cvar_SetValue( "cg_weaponBarStyle", hudOptions_s.weaponBarStyle.curvalue );
		break;

	case ID_DRAWFPS:
		trap_Cvar_SetValue( "cg_drawFPS", hudOptions_s.drawFPS.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}


/*
=================
Crosshair_Draw
=================
*/
static void Crosshair_Draw( void *self ) {
	menulist_s	*s;
	float		*color;
	int			x, y;
	int			style;
	qboolean	focus;
	vec4_t          color4;

	s = (menulist_s *)self;
	x = s->generic.x;
	y =	s->generic.y;

	style = UI_SMALLFONT;
	focus = (s->generic.parent->cursor == s->generic.menuPosition);

	if ( s->generic.flags & QMF_GRAYED ) {
		color = text_color_disabled;
	}
	else if ( focus )
	{
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK ) {
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else {
		color = text_color_normal;
	}

	if ( focus ) {
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	if( !s->curvalue ) {
		return;
	}
	color4[0]=((float)hudOptions_s.crosshairColorRed.curvalue)/255.f;
	color4[1]=((float)hudOptions_s.crosshairColorGreen.curvalue)/255.f;
	color4[2]=((float)hudOptions_s.crosshairColorBlue.curvalue)/255.f;
	color4[3]=1.0f;
	trap_R_SetColor( color4 );
	UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y - 4, 24, 24, hudOptions_s.crosshairShader[s->curvalue] );
}

static void HUDOptions_MenuInit( void ) {
	int				y;

	UI_SetDefaultCvar("cg_crosshairHealth","1");
	UI_SetDefaultCvar("cg_crosshairColorRed","1");
	UI_SetDefaultCvar("cg_crosshairColorBlue","1");
	UI_SetDefaultCvar("cg_crosshairColorGreen","1");
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
	hudOptions_s.crosshair.generic.type			= MTYPE_TEXT;
	hudOptions_s.crosshair.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
	hudOptions_s.crosshair.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshair.generic.y			= y;
	hudOptions_s.crosshair.generic.name			= "Crosshair Style:";
	hudOptions_s.crosshair.generic.callback		= HUDOptions_Event;
	hudOptions_s.crosshair.generic.ownerdraw	= Crosshair_Draw;
	hudOptions_s.crosshair.generic.id			= ID_CROSSHAIR;
	hudOptions_s.crosshair.generic.top			= y - 4;
	hudOptions_s.crosshair.generic.bottom		= y + 20;
	hudOptions_s.crosshair.generic.left			= HUDOPTIONS_X_POS - ( ( strlen(hudOptions_s.crosshair.generic.name) + 1 ) * SMALLCHAR_WIDTH );
	hudOptions_s.crosshair.generic.right		= HUDOPTIONS_X_POS + 48;

	y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairHealth.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.crosshairHealth.generic.name		= "Crosshair Shows Health:";
	hudOptions_s.crosshairHealth.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairHealth.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairHealth.generic.id			= ID_CROSSHAIRHEALTH;
	hudOptions_s.crosshairHealth.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairHealth.generic.y			= y;

	y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairColorRed.generic.type		= MTYPE_SLIDER;
	hudOptions_s.crosshairColorRed.generic.name		= "Crosshair Color (Red):";
	hudOptions_s.crosshairColorRed.generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairColorRed.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairColorRed.generic.id		= ID_COLORRED;
	hudOptions_s.crosshairColorRed.generic.x		= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairColorRed.generic.y		= y;
	hudOptions_s.crosshairColorRed.minvalue			= 0.0f;
	hudOptions_s.crosshairColorRed.maxvalue			= 255.0f;

	y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairColorGreen.generic.type		= MTYPE_SLIDER;
	hudOptions_s.crosshairColorGreen.generic.name		= "Crosshair Color (Green):";
	hudOptions_s.crosshairColorGreen.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairColorGreen.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairColorGreen.generic.id			= ID_COLORGREEN;
	hudOptions_s.crosshairColorGreen.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairColorGreen.generic.y			= y;
	hudOptions_s.crosshairColorGreen.minvalue			= 0.0f;
	hudOptions_s.crosshairColorGreen.maxvalue			= 255.0f;

	y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairColorBlue.generic.type		= MTYPE_SLIDER;
	hudOptions_s.crosshairColorBlue.generic.name		= "Crosshair Color (Blue):";
	hudOptions_s.crosshairColorBlue.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairColorBlue.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairColorBlue.generic.id			= ID_COLORBLUE;
	hudOptions_s.crosshairColorBlue.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairColorBlue.generic.y			= y;
	hudOptions_s.crosshairColorBlue.minvalue			= 0.0f;
	hudOptions_s.crosshairColorBlue.maxvalue			= 255.0f;

	if(hudOptions_s.crosshairHealth.curvalue) {
		hudOptions_s.crosshairColorRed.generic.flags	|= QMF_INACTIVE;
		hudOptions_s.crosshairColorGreen.generic.flags	|= QMF_INACTIVE;
		hudOptions_s.crosshairColorBlue.generic.flags	|= QMF_INACTIVE;
	}

	y += BIGCHAR_HEIGHT+2;
	hudOptions_s.alwaysWeaponBar.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.alwaysWeaponBar.generic.name		= "Always Show Weapon Bar:";
	hudOptions_s.alwaysWeaponBar.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.alwaysWeaponBar.generic.callback	= HUDOptions_Event;
	hudOptions_s.alwaysWeaponBar.generic.id			= ID_ALWAYSWEAPONBAR;
	hudOptions_s.alwaysWeaponBar.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.alwaysWeaponBar.generic.y			= y;
	
	y += BIGCHAR_HEIGHT+2;
	hudOptions_s.weaponBarStyle.generic.type		= MTYPE_SPINCONTROL;
	hudOptions_s.weaponBarStyle.generic.name		= "Weapon Bar Style:";
	hudOptions_s.weaponBarStyle.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.weaponBarStyle.generic.callback	= HUDOptions_Event;
	hudOptions_s.weaponBarStyle.generic.id			= ID_WEAPONBARSTYLE;
	hudOptions_s.weaponBarStyle.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.weaponBarStyle.generic.y			= y;
	hudOptions_s.weaponBarStyle.itemnames			= weaponBarStyle_names;

	y += BIGCHAR_HEIGHT+2;
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

	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshair );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairHealth );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairColorRed );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairColorGreen );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairColorBlue );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.alwaysWeaponBar );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.weaponBarStyle );
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
	int		n;

	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		if (n < 10) {
			hudOptions_s.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
		}
		else {
			hudOptions_s.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%02d", n - 10) );
		}
	}
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

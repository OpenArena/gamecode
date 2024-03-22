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

#define HUDOPTIONS_X_POS		320

#define ID_BACK					127
#define ID_CROSSHAIR			128
#define ID_COLORRED				129
#define ID_COLORGREEN			130
#define ID_COLORBLUE			131
#define ID_CROSSHAIRHEALTH		132
#define ID_ALWAYSWEAPONBAR		133
#define ID_WEAPONBARSTYLE		134
#define ID_IDENTIFYTARGET		135
#define ID_DRAWTEAMOVERLAY		136
#define ID_OBITUARYOUTPUT		137
#define ID_DRAWFPS				138
#define ID_DRAWTIMER			139
#define ID_DRAWSTATUS			140
#define ID_DRAWAMMOWARNING		141
#define ID_DRAWATTACKER			142
#define ID_DRAWSPEED			143
#define ID_DRAWREWARDS			144
#define ID_DRAW3DICONS			145
#define ID_CROSSHAIRPULSE		146

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menulist_s			crosshair;
	menuradiobutton_s	crosshairHealth;
	menuradiobutton_s	crosshairPulse;

	//Crosshair colors:
	menuslider_s		crosshairColorRed;
	menuslider_s		crosshairColorGreen;
	menuslider_s		crosshairColorBlue;

	menulist_s			draw3DIcons;
	menuradiobutton_s	alwaysWeaponBar;
	menulist_s			weaponBarStyle;
	menuradiobutton_s	identifyTarget;
	menulist_s			drawTeamOverlay;
	menulist_s			obituaryOutput;

	menuradiobutton_s	drawFPS;
	menuradiobutton_s	drawTimer;
	menuradiobutton_s	drawStatus;
	menuradiobutton_s	drawAmmoWarning;
	menuradiobutton_s	drawAttacker;
	menuradiobutton_s	drawSpeed;
	menuradiobutton_s	drawRewards;

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

static const char *teamOverlay_names[] =
{
	"Off",
	"Upper Right",
	"Lower Right",
	"Lower Left",
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

static const char *draw3DIcons_names[] =
{
	"2D",
	"3D",
	NULL
};

static void HUDOptions_SetMenuItems( void ) {
	hudOptions_s.crosshair.curvalue				= (int)trap_Cvar_VariableValue( "cg_drawCrosshair" ) % NUM_CROSSHAIRS;
	hudOptions_s.crosshairHealth.curvalue		= trap_Cvar_VariableValue( "cg_crosshairHealth") != 0;
	hudOptions_s.crosshairPulse.curvalue		= trap_Cvar_VariableValue( "cg_crosshairPulse") != 0;
	hudOptions_s.crosshairColorRed.curvalue		= trap_Cvar_VariableValue( "cg_crosshairColorRed")*255.0f;
	hudOptions_s.crosshairColorGreen.curvalue	= trap_Cvar_VariableValue( "cg_crosshairColorGreen")*255.0f;
	hudOptions_s.crosshairColorBlue.curvalue	= trap_Cvar_VariableValue( "cg_crosshairColorBlue")*255.0f;
	hudOptions_s.draw3DIcons.curvalue			= trap_Cvar_VariableValue( "cg_draw3DIcons") != 0;
	hudOptions_s.alwaysWeaponBar.curvalue		= trap_Cvar_VariableValue( "cg_alwaysWeaponBar" ) != 0;
	hudOptions_s.weaponBarStyle.curvalue		= trap_Cvar_VariableValue( "cg_weaponBarStyle" ) != 0;
	hudOptions_s.identifyTarget.curvalue		= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	hudOptions_s.drawTeamOverlay.curvalue		= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	hudOptions_s.obituaryOutput.curvalue		= trap_Cvar_VariableValue( "cg_obituaryOutput" ) != 0;
	hudOptions_s.drawFPS.curvalue				= trap_Cvar_VariableValue( "cg_drawfps") != 0;
	hudOptions_s.drawTimer.curvalue				= trap_Cvar_VariableValue( "cg_drawTimer") != 0;
	hudOptions_s.drawStatus.curvalue			= trap_Cvar_VariableValue( "cg_drawStatus") != 0;
	hudOptions_s.drawAmmoWarning.curvalue		= trap_Cvar_VariableValue( "cg_drawAmmoWarning") != 0;
	hudOptions_s.drawAttacker.curvalue			= trap_Cvar_VariableValue( "cg_drawAttacker") != 0;
	hudOptions_s.drawSpeed.curvalue				= trap_Cvar_VariableValue( "cg_drawSpeed") != 0;
	hudOptions_s.drawRewards.curvalue			= trap_Cvar_VariableValue( "cg_drawRewards") != 0;
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

	case ID_CROSSHAIRPULSE:
		trap_Cvar_SetValue( "cg_crosshairPulse", ((float)hudOptions_s.crosshairPulse.curvalue)/255.f );
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

	case ID_DRAW3DICONS:
		trap_Cvar_SetValue( "cg_draw3DIcons", hudOptions_s.draw3DIcons.curvalue );
		break;

	case ID_ALWAYSWEAPONBAR:
		trap_Cvar_SetValue( "cg_alwaysWeaponBar", hudOptions_s.alwaysWeaponBar.curvalue );
		break;

	case ID_WEAPONBARSTYLE:
		trap_Cvar_SetValue( "cg_weaponBarStyle", hudOptions_s.weaponBarStyle.curvalue );
		break;

	case ID_IDENTIFYTARGET:
		trap_Cvar_SetValue( "cg_drawCrosshairNames", hudOptions_s.identifyTarget.curvalue );
		break;

	case ID_DRAWTEAMOVERLAY:
		trap_Cvar_SetValue( "cg_drawTeamOverlay", hudOptions_s.drawTeamOverlay.curvalue );
		break;

	case ID_OBITUARYOUTPUT:
		trap_Cvar_SetValue( "cg_obituaryOutput", hudOptions_s.obituaryOutput.curvalue );
		break;

	case ID_DRAWFPS:
		trap_Cvar_SetValue( "cg_drawFPS", hudOptions_s.drawFPS.curvalue );
		break;

	case ID_DRAWTIMER:
		trap_Cvar_SetValue( "cg_drawTimer", hudOptions_s.drawTimer.curvalue );
		break;

	case ID_DRAWSTATUS:
		trap_Cvar_SetValue( "cg_drawStatus", hudOptions_s.drawStatus.curvalue );
		break;

	case ID_DRAWAMMOWARNING:
		trap_Cvar_SetValue( "cg_drawAmmoWarning", hudOptions_s.drawAmmoWarning.curvalue );
		break;

	case ID_DRAWATTACKER:
		trap_Cvar_SetValue( "cg_drawAttacker", hudOptions_s.drawAttacker.curvalue );
		break;

	case ID_DRAWSPEED:
		trap_Cvar_SetValue( "cg_drawSpeed", hudOptions_s.drawSpeed.curvalue );
		break;

	case ID_DRAWREWARDS:
		trap_Cvar_SetValue( "cg_drawRewards", hudOptions_s.drawRewards.curvalue );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

/*
=================
HUDOptions_StatusBar_Crosshair

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_Crosshair( void* ptr ) {
	UI_DrawString( 320, 440, "Select how the crosshair will look in-game.", UI_CENTER|UI_SMALLFONT, colorWhite );
	//UI_DrawString( 320, 460, "", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_CrosshairHealth

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_CrosshairHealth( void* ptr ) {
	UI_DrawString( 320, 440, "The crosshair coloration depends on your", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "health level. Disables coloring if selected.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_CrosshairPulse

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_CrosshairPulse( void* ptr ) {
	UI_DrawString( 320, 440, "If set, whenever you pick up an item,", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "the crosshair will emit a pulse.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_CrosshairColor

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_CrosshairColor( void* ptr ) {
	UI_DrawString( 320, 440, "Use the sliders to change the color.", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "of the selected crosshair.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_Draw3DIcons

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_Draw3DIcons( void* ptr ) {
	UI_DrawString( 320, 440, "Specifies if the items in the weapon bar", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "will be rendered as 2D icons or 3D models.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_AlwaysWeaponBar

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_AlwaysWeaponBar( void* ptr ) {
	UI_DrawString( 320, 440, "If set, the weapon bar will always", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "be displayed in the in-game HUD.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_WeaponBarStyle

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_WeaponBarStyle( void* ptr ) {
	UI_DrawString( 320, 440, "Specifies how the weapon bar", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "will be displayed in the HUD.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_IdentifyTarget

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_IdentifyTarget( void* ptr ) {
	UI_DrawString( 320, 440, "If set, the HUD will display the nickname", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "of the target behind your crosshair.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawTeamOverlay

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawTeamOverlay( void* ptr ) {
	UI_DrawString( 320, 440, "If set, wherever you decide to draw it, the game", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "will display the status of your team in a corner.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_ObituaryOutput

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_ObituaryOutput( void* ptr ) {
	UI_DrawString( 320, 440, "Specifies how and where will death messages", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "be rendered, and in which format.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawFPS

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawFPS( void* ptr ) {
	UI_DrawString( 320, 440, "If set, draws your actual Frames Per Second", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "(FPS) rate on the top-right corner.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawTimer

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawTimer( void* ptr ) {
	UI_DrawString( 320, 440, "If set, draws the elapsed match", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "time on the top-right corner.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawStatus

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawStatus( void* ptr ) {
	UI_DrawString( 320, 440, "If set, draws ammo, health and armor status", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "on the bottom of your screen.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawAmmoWarning

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawAmmoWarning( void* ptr ) {
	UI_DrawString( 320, 440, "If set, a warning ensues whenever you're about", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "to run out of ammo for all of your weapons.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawAttacker

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawAttacker( void* ptr ) {
	UI_DrawString( 320, 440, "If set, draws in the top-right", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "corner who last attacked you.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawSpeed

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawSpeed( void* ptr ) {
	UI_DrawString( 320, 440, "If set, draws in the top-right", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "corner your actual movement speed.", UI_CENTER|UI_SMALLFONT, colorWhite );
}

/*
=================
HUDOptions_StatusBar_DrawRewards

Descriptions should have 48 characters or less per line, and there can't be more than two lines.
=================
*/
static void HUDOptions_StatusBar_DrawRewards( void* ptr ) {
	UI_DrawString( 320, 440, "If set, whenever you do something awesome in", UI_CENTER|UI_SMALLFONT, colorWhite );
	UI_DrawString( 320, 460, "the game, you'll be rewarded a specific medal.", UI_CENTER|UI_SMALLFONT, colorWhite );
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
	//int				y;

	UI_SetDefaultCvar("cg_crosshairHealth","1");
	UI_SetDefaultCvar("cg_crosshairPulse","1");
	UI_SetDefaultCvar("cg_crosshairColorRed","1");
	UI_SetDefaultCvar("cg_crosshairColorBlue","1");
	UI_SetDefaultCvar("cg_crosshairColorGreen","1");
	UI_SetDefaultCvar("cg_draw3DIcons","1");
	UI_SetDefaultCvar("cg_drawFPS","0");
	UI_SetDefaultCvar("cg_drawTimer","0");
	UI_SetDefaultCvar("cg_drawStatus","1");
	UI_SetDefaultCvar("cg_drawAmmoWarning","1");
	UI_SetDefaultCvar("cg_drawAttacker","1");
	UI_SetDefaultCvar("cg_drawSpeed","0");
	UI_SetDefaultCvar("cg_drawRewards","0");

	memset( &hudOptions_s, 0 ,sizeof(hudOptions_t) );

	UI_HUDOptionsMenu_Cache();

	hudOptions_s.menu.wrapAround = qtrue;
	hudOptions_s.menu.fullscreen = qtrue;

	hudOptions_s.banner.generic.type	= MTYPE_BTEXT;
	hudOptions_s.banner.generic.x		= 320;
	hudOptions_s.banner.generic.y		= 16;
	hudOptions_s.banner.string			= "HEADS UP DISPLAY";
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

	//y = 80;
	hudOptions_s.crosshair.generic.type			= MTYPE_TEXT;
	hudOptions_s.crosshair.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
	hudOptions_s.crosshair.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshair.generic.y			= 240 - 9 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.crosshair.generic.name			= "Crosshair Style:";
	hudOptions_s.crosshair.generic.callback		= HUDOptions_Event;
	hudOptions_s.crosshair.generic.ownerdraw	= Crosshair_Draw;
	hudOptions_s.crosshair.generic.id			= ID_CROSSHAIR;
	hudOptions_s.crosshair.generic.top			= 240 - 9 * (BIGCHAR_HEIGHT+2) - 4;
	hudOptions_s.crosshair.generic.bottom		= 240 - 9 * (BIGCHAR_HEIGHT+2) + 20;
	hudOptions_s.crosshair.generic.left			= HUDOPTIONS_X_POS - ( ( strlen(hudOptions_s.crosshair.generic.name) + 1 ) * SMALLCHAR_WIDTH );
	hudOptions_s.crosshair.generic.right		= HUDOPTIONS_X_POS + 48;
	hudOptions_s.crosshair.generic.statusbar	= HUDOptions_StatusBar_Crosshair;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairHealth.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.crosshairHealth.generic.name		= "Crosshair Shows Health:";
	hudOptions_s.crosshairHealth.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairHealth.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairHealth.generic.id			= ID_CROSSHAIRHEALTH;
	hudOptions_s.crosshairHealth.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairHealth.generic.y			= 240 - 8 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.crosshairHealth.generic.statusbar	= HUDOptions_StatusBar_CrosshairHealth;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairPulse.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.crosshairPulse.generic.name		= "Crosshair Pickup Pulse:";
	hudOptions_s.crosshairPulse.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairPulse.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairPulse.generic.id			= ID_CROSSHAIRPULSE;
	hudOptions_s.crosshairPulse.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairPulse.generic.y			= 240 - 7 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.crosshairPulse.generic.statusbar	= HUDOptions_StatusBar_CrosshairPulse;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairColorRed.generic.type			= MTYPE_SLIDER;
	hudOptions_s.crosshairColorRed.generic.name			= "Crosshair Color (Red):";
	hudOptions_s.crosshairColorRed.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairColorRed.generic.callback		= HUDOptions_Event;
	hudOptions_s.crosshairColorRed.generic.id			= ID_COLORRED;
	hudOptions_s.crosshairColorRed.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairColorRed.generic.y			= 240 - 6 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.crosshairColorRed.minvalue				= 0.0f;
	hudOptions_s.crosshairColorRed.maxvalue				= 255.0f;
	hudOptions_s.crosshairColorRed.generic.statusbar	= HUDOptions_StatusBar_CrosshairColor;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairColorGreen.generic.type		= MTYPE_SLIDER;
	hudOptions_s.crosshairColorGreen.generic.name		= "Crosshair Color (Green):";
	hudOptions_s.crosshairColorGreen.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairColorGreen.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairColorGreen.generic.id			= ID_COLORGREEN;
	hudOptions_s.crosshairColorGreen.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairColorGreen.generic.y			= 240 - 5 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.crosshairColorGreen.minvalue			= 0.0f;
	hudOptions_s.crosshairColorGreen.maxvalue			= 255.0f;
	hudOptions_s.crosshairColorGreen.generic.statusbar	= HUDOptions_StatusBar_CrosshairColor;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.crosshairColorBlue.generic.type		= MTYPE_SLIDER;
	hudOptions_s.crosshairColorBlue.generic.name		= "Crosshair Color (Blue):";
	hudOptions_s.crosshairColorBlue.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.crosshairColorBlue.generic.callback	= HUDOptions_Event;
	hudOptions_s.crosshairColorBlue.generic.id			= ID_COLORBLUE;
	hudOptions_s.crosshairColorBlue.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.crosshairColorBlue.generic.y			= 240 - 4 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.crosshairColorBlue.minvalue			= 0.0f;
	hudOptions_s.crosshairColorBlue.maxvalue			= 255.0f;
	hudOptions_s.crosshairColorBlue.generic.statusbar	= HUDOptions_StatusBar_CrosshairColor;

	if(hudOptions_s.crosshairHealth.curvalue) {
		hudOptions_s.crosshairColorRed.generic.flags	|= QMF_INACTIVE;
		hudOptions_s.crosshairColorGreen.generic.flags	|= QMF_INACTIVE;
		hudOptions_s.crosshairColorBlue.generic.flags	|= QMF_INACTIVE;
	}

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.draw3DIcons.generic.type		= MTYPE_SPINCONTROL;
	hudOptions_s.draw3DIcons.generic.name		= "Weapon Bar Icon Rendering:";
	hudOptions_s.draw3DIcons.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.draw3DIcons.generic.callback	= HUDOptions_Event;
	hudOptions_s.draw3DIcons.generic.id			= ID_DRAW3DICONS;
	hudOptions_s.draw3DIcons.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.draw3DIcons.generic.y			= 240 - 3 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.draw3DIcons.itemnames			= draw3DIcons_names;
	hudOptions_s.draw3DIcons.generic.statusbar	= HUDOptions_StatusBar_Draw3DIcons;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.alwaysWeaponBar.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.alwaysWeaponBar.generic.name		= "Always Show Weapon Bar:";
	hudOptions_s.alwaysWeaponBar.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.alwaysWeaponBar.generic.callback	= HUDOptions_Event;
	hudOptions_s.alwaysWeaponBar.generic.id			= ID_ALWAYSWEAPONBAR;
	hudOptions_s.alwaysWeaponBar.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.alwaysWeaponBar.generic.y			= 240 - 2 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.alwaysWeaponBar.generic.statusbar	= HUDOptions_StatusBar_AlwaysWeaponBar;
	
	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.weaponBarStyle.generic.type		= MTYPE_SPINCONTROL;
	hudOptions_s.weaponBarStyle.generic.name		= "Weapon Bar Style:";
	hudOptions_s.weaponBarStyle.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.weaponBarStyle.generic.callback	= HUDOptions_Event;
	hudOptions_s.weaponBarStyle.generic.id			= ID_WEAPONBARSTYLE;
	hudOptions_s.weaponBarStyle.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.weaponBarStyle.generic.y			= 240 - (BIGCHAR_HEIGHT+2);
	hudOptions_s.weaponBarStyle.itemnames			= weaponBarStyle_names;
	hudOptions_s.weaponBarStyle.generic.statusbar	= HUDOptions_StatusBar_WeaponBarStyle;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.identifyTarget.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.identifyTarget.generic.name		= "Show Crosshair Target Name:";
	hudOptions_s.identifyTarget.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.identifyTarget.generic.callback	= HUDOptions_Event;
	hudOptions_s.identifyTarget.generic.id			= ID_IDENTIFYTARGET;
	hudOptions_s.identifyTarget.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.identifyTarget.generic.y			= 240;
	hudOptions_s.identifyTarget.generic.statusbar	= HUDOptions_StatusBar_IdentifyTarget;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawTeamOverlay.generic.type		= MTYPE_SPINCONTROL;
	hudOptions_s.drawTeamOverlay.generic.name		= "Show Team Overlay:";
	hudOptions_s.drawTeamOverlay.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawTeamOverlay.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawTeamOverlay.generic.id			= ID_DRAWTEAMOVERLAY;
	hudOptions_s.drawTeamOverlay.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawTeamOverlay.generic.y			= 240 + (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawTeamOverlay.itemnames			= teamOverlay_names;
	hudOptions_s.drawTeamOverlay.generic.statusbar	= HUDOptions_StatusBar_DrawTeamOverlay;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.obituaryOutput.generic.type		= MTYPE_SPINCONTROL;
	hudOptions_s.obituaryOutput.generic.name		= "Death Message Output:";
	hudOptions_s.obituaryOutput.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.obituaryOutput.generic.callback	= HUDOptions_Event;
	hudOptions_s.obituaryOutput.generic.id			= ID_OBITUARYOUTPUT;
	hudOptions_s.obituaryOutput.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.obituaryOutput.generic.y			= 240 + 2 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.obituaryOutput.itemnames			= obituaryOutput_names;
	hudOptions_s.obituaryOutput.generic.statusbar	= HUDOptions_StatusBar_ObituaryOutput;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawFPS.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.drawFPS.generic.name		= "Show FPS:";
	hudOptions_s.drawFPS.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawFPS.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawFPS.generic.id			= ID_DRAWFPS;
	hudOptions_s.drawFPS.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawFPS.generic.y			= 240 + 3 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawFPS.generic.statusbar	= HUDOptions_StatusBar_DrawFPS;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawTimer.generic.type			= MTYPE_RADIOBUTTON;
	hudOptions_s.drawTimer.generic.name			= "Show Timer:";
	hudOptions_s.drawTimer.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawTimer.generic.callback		= HUDOptions_Event;
	hudOptions_s.drawTimer.generic.id			= ID_DRAWTIMER;
	hudOptions_s.drawTimer.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawTimer.generic.y			= 240 + 4 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawTimer.generic.statusbar	= HUDOptions_StatusBar_DrawTimer;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawStatus.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.drawStatus.generic.name		= "Show Player Status:";
	hudOptions_s.drawStatus.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawStatus.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawStatus.generic.id			= ID_DRAWSTATUS;
	hudOptions_s.drawStatus.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawStatus.generic.y			= 240 + 5 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawStatus.generic.statusbar	= HUDOptions_StatusBar_DrawStatus;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawAmmoWarning.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.drawAmmoWarning.generic.name		= "Show Low Ammo Warning:";
	hudOptions_s.drawAmmoWarning.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawAmmoWarning.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawAmmoWarning.generic.id			= ID_DRAWAMMOWARNING;
	hudOptions_s.drawAmmoWarning.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawAmmoWarning.generic.y			= 240 + 6 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawAmmoWarning.generic.statusbar	= HUDOptions_StatusBar_DrawAmmoWarning;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawAttacker.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.drawAttacker.generic.name		= "Show Last Attacker:";
	hudOptions_s.drawAttacker.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawAttacker.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawAttacker.generic.id		= ID_DRAWATTACKER;
	hudOptions_s.drawAttacker.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawAttacker.generic.y			= 240 + 7 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawAttacker.generic.statusbar	= HUDOptions_StatusBar_DrawAttacker;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawSpeed.generic.type			= MTYPE_RADIOBUTTON;
	hudOptions_s.drawSpeed.generic.name			= "Show Movement Speed:";
	hudOptions_s.drawSpeed.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawSpeed.generic.callback		= HUDOptions_Event;
	hudOptions_s.drawSpeed.generic.id			= ID_DRAWSPEED;
	hudOptions_s.drawSpeed.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawSpeed.generic.y			= 240 + 8 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawSpeed.generic.statusbar	= HUDOptions_StatusBar_DrawSpeed;

	//y += BIGCHAR_HEIGHT+2;
	hudOptions_s.drawRewards.generic.type		= MTYPE_RADIOBUTTON;
	hudOptions_s.drawRewards.generic.name		= "Show Medal(s) Earned:";
	hudOptions_s.drawRewards.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	hudOptions_s.drawRewards.generic.callback	= HUDOptions_Event;
	hudOptions_s.drawRewards.generic.id			= ID_DRAWREWARDS;
	hudOptions_s.drawRewards.generic.x			= HUDOPTIONS_X_POS;
	hudOptions_s.drawRewards.generic.y			= 240 + 9 * (BIGCHAR_HEIGHT+2);
	hudOptions_s.drawRewards.generic.statusbar	= HUDOptions_StatusBar_DrawRewards;

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
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairPulse );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairColorRed );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairColorGreen );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.crosshairColorBlue );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.draw3DIcons );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.alwaysWeaponBar );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.weaponBarStyle );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.identifyTarget );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawTeamOverlay );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.obituaryOutput );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawFPS );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawTimer );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawStatus );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawAmmoWarning );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawAttacker );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawSpeed );
	Menu_AddItem( &hudOptions_s.menu, &hudOptions_s.drawRewards );

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

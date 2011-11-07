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


#define ART_FRAMEL				"menu/art_blueish/frame2_l"
#define ART_FRAMER				"menu/art_blueish/frame1_r"
#define ART_BACK0				"menu/art_blueish/back_0"
#define ART_BACK1				"menu/art_blueish/back_1"

#define PREFERENCES_X_POS		360

#define ID_CROSSHAIR			127
#define ID_SIMPLEITEMS			128
#define ID_HIGHQUALITYSKY		129
#define ID_EJECTINGBRASS		130
#define ID_WALLMARKS			131
#define ID_DYNAMICLIGHTS		132
#define ID_IDENTIFYTARGET		133
#define ID_SYNCEVERYFRAME		134
#define ID_FORCEMODEL			135
#define ID_DRAWTEAMOVERLAY		136
#define ID_ALLOWDOWNLOAD			137
#define ID_BACK					138
//Elimination
#define ID_WEAPONBAR                    139
#define ID_DELAGHITSCAN		140
#define ID_COLORRED             141
#define ID_COLORGREEN           142
#define ID_COLORBLUE            143
#define ID_CROSSHAIRHEALTH      144
#define ID_CHATBEEP             145
#define ID_TEAMCHATBEEP         146

#define	NUM_CROSSHAIRS			99


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menulist_s			crosshair;
        menuradiobutton_s	crosshairHealth;

        //Crosshair colors:
        menuslider_s            crosshairColorRed;
        menuslider_s            crosshairColorGreen;
        menuslider_s            crosshairColorBlue;

	menuradiobutton_s	simpleitems;
        menuradiobutton_s	alwaysweaponbar;
	menuradiobutton_s	brass;
	menuradiobutton_s	wallmarks;
	menuradiobutton_s	dynamiclights;
	menuradiobutton_s	identifytarget;
	menuradiobutton_s	highqualitysky;
	menuradiobutton_s	synceveryframe;
	menuradiobutton_s	forcemodel;
	menulist_s			drawteamoverlay;
        menuradiobutton_s	delaghitscan;
	menuradiobutton_s	allowdownload;
        menuradiobutton_s       chatbeep;
        menuradiobutton_s       teamchatbeep;
	menubitmap_s		back;

	qhandle_t			crosshairShader[NUM_CROSSHAIRS];
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

static void Preferences_SetMenuItems( void ) {
	s_preferences.crosshair.curvalue		= (int)trap_Cvar_VariableValue( "cg_drawCrosshair" ) % NUM_CROSSHAIRS;
        s_preferences.crosshairHealth.curvalue          = trap_Cvar_VariableValue( "cg_crosshairHealth") != 0;
        s_preferences.crosshairColorRed.curvalue        = trap_Cvar_VariableValue( "cg_crosshairColorRed")*255.0f;
        s_preferences.crosshairColorGreen.curvalue      = trap_Cvar_VariableValue( "cg_crosshairColorGreen")*255.0f;
        s_preferences.crosshairColorBlue.curvalue       = trap_Cvar_VariableValue( "cg_crosshairColorBlue")*255.0f;
	s_preferences.simpleitems.curvalue		= trap_Cvar_VariableValue( "cg_simpleItems" ) != 0;
        s_preferences.alwaysweaponbar.curvalue		= trap_Cvar_VariableValue( "cg_alwaysWeaponBar" ) != 0;
	s_preferences.brass.curvalue			= trap_Cvar_VariableValue( "cg_brassTime" ) != 0;
	s_preferences.wallmarks.curvalue		= trap_Cvar_VariableValue( "cg_marks" ) != 0;
	s_preferences.identifytarget.curvalue	= trap_Cvar_VariableValue( "cg_drawCrosshairNames" ) != 0;
	s_preferences.dynamiclights.curvalue	= trap_Cvar_VariableValue( "r_dynamiclight" ) != 0;
	s_preferences.highqualitysky.curvalue	= trap_Cvar_VariableValue ( "r_fastsky" ) == 0;
	s_preferences.synceveryframe.curvalue	= trap_Cvar_VariableValue( "r_finish" ) != 0;
	s_preferences.forcemodel.curvalue		= trap_Cvar_VariableValue( "cg_forcemodel" ) != 0;
	s_preferences.drawteamoverlay.curvalue	= Com_Clamp( 0, 3, trap_Cvar_VariableValue( "cg_drawTeamOverlay" ) );
	s_preferences.allowdownload.curvalue	= trap_Cvar_VariableValue( "cl_allowDownload" ) != 0;
        s_preferences.delaghitscan.curvalue	= trap_Cvar_VariableValue( "cg_delag" ) != 0;
        s_preferences.chatbeep.curvalue         = trap_Cvar_VariableValue( "cg_chatBeep" ) != 0;
        s_preferences.teamchatbeep.curvalue     = trap_Cvar_VariableValue( "cg_teamChatBeep" ) != 0;
}

static void Preferences_Event( void* ptr, int notification ) {
	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_CROSSHAIR:
		s_preferences.crosshair.curvalue++;
		if( s_preferences.crosshair.curvalue == NUM_CROSSHAIRS || s_preferences.crosshairShader[s_preferences.crosshair.curvalue]==0 ) {
			s_preferences.crosshair.curvalue = 0;
		}
		trap_Cvar_SetValue( "cg_drawCrosshair", s_preferences.crosshair.curvalue );
		break;

        case ID_CROSSHAIRHEALTH:
                trap_Cvar_SetValue( "cg_crosshairHealth", s_preferences.crosshairHealth.curvalue );
                if(s_preferences.crosshairHealth.curvalue) {
                    //If crosshairHealth is on: Don't allow color selection
                    s_preferences.crosshairColorRed.generic.flags       |= QMF_INACTIVE;
                    s_preferences.crosshairColorGreen.generic.flags     |= QMF_INACTIVE;
                    s_preferences.crosshairColorBlue.generic.flags      |= QMF_INACTIVE;
                } else {
                    //If crosshairHealth is off: Allow color selection
                    s_preferences.crosshairColorRed.generic.flags       &= ~QMF_INACTIVE;
                    s_preferences.crosshairColorGreen.generic.flags     &= ~QMF_INACTIVE;
                    s_preferences.crosshairColorBlue.generic.flags      &= ~QMF_INACTIVE;
                }
                break;

        case ID_COLORRED:
                trap_Cvar_SetValue( "cg_crosshairColorRed", ((float)s_preferences.crosshairColorRed.curvalue)/255.f );
                break;

        case ID_COLORGREEN:
                trap_Cvar_SetValue( "cg_crosshairColorGreen", ((float)s_preferences.crosshairColorGreen.curvalue)/255.f );
                break;

        case ID_COLORBLUE:
                trap_Cvar_SetValue( "cg_crosshairColorBlue", ((float)s_preferences.crosshairColorBlue.curvalue)/255.f );
                break;

	case ID_SIMPLEITEMS:
		trap_Cvar_SetValue( "cg_simpleItems", s_preferences.simpleitems.curvalue );
		break;
                
        case ID_WEAPONBAR:
		trap_Cvar_SetValue( "cg_alwaysWeaponBar", s_preferences.alwaysweaponbar.curvalue );
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

	case ID_DYNAMICLIGHTS:
		trap_Cvar_SetValue( "r_dynamiclight", s_preferences.dynamiclights.curvalue );
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

	case ID_ALLOWDOWNLOAD:
		trap_Cvar_SetValue( "cl_allowDownload", s_preferences.allowdownload.curvalue );
		trap_Cvar_SetValue( "sv_allowDownload", s_preferences.allowdownload.curvalue );
		break;
               
        case ID_DELAGHITSCAN:
                trap_Cvar_SetValue( "g_delagHitscan", s_preferences.delaghitscan.curvalue );
		trap_Cvar_SetValue( "cg_delag", s_preferences.delaghitscan.curvalue );
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

	if ( s->generic.flags & QMF_GRAYED )
		color = text_color_disabled;
	else if ( focus )
	{
		color = text_color_highlight;
		style |= UI_PULSE;
	}
	else if ( s->generic.flags & QMF_BLINK )
	{
		color = text_color_highlight;
		style |= UI_BLINK;
	}
	else
		color = text_color_normal;

	if ( focus )
	{
		// draw cursor
		UI_FillRect( s->generic.left, s->generic.top, s->generic.right-s->generic.left+1, s->generic.bottom-s->generic.top+1, listbar_color ); 
		UI_DrawChar( x, y, 13, UI_CENTER|UI_BLINK|UI_SMALLFONT, color);
	}

	UI_DrawString( x - SMALLCHAR_WIDTH, y, s->generic.name, style|UI_RIGHT, color );
	if( !s->curvalue ) {
		return;
	}
        color4[0]=((float)s_preferences.crosshairColorRed.curvalue)/255.f;
        color4[1]=((float)s_preferences.crosshairColorGreen.curvalue)/255.f;
        color4[2]=((float)s_preferences.crosshairColorBlue.curvalue)/255.f;
        color4[3]=1.0f;
	trap_R_SetColor( color4 );
	UI_DrawHandlePic( x + SMALLCHAR_WIDTH, y - 4, 24, 24, s_preferences.crosshairShader[s->curvalue] );
}


static void Preferences_MenuInit( void ) {
	int				y;
        
        UI_SetDefaultCvar("cg_crosshairHealth","1");
        UI_SetDefaultCvar("cg_crosshairColorRed","1");
        UI_SetDefaultCvar("cg_crosshairColorBlue","1");
        UI_SetDefaultCvar("cg_crosshairColorGreen","1");
        
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

	y = 82;
	s_preferences.crosshair.generic.type		= MTYPE_TEXT;
	s_preferences.crosshair.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
	s_preferences.crosshair.generic.x			= PREFERENCES_X_POS;
	s_preferences.crosshair.generic.y			= y;
	s_preferences.crosshair.generic.name		= "Crosshair:";
	s_preferences.crosshair.generic.callback	= Preferences_Event;
	s_preferences.crosshair.generic.ownerdraw	= Crosshair_Draw;
	s_preferences.crosshair.generic.id			= ID_CROSSHAIR;
	s_preferences.crosshair.generic.top			= y - 4;
	s_preferences.crosshair.generic.bottom		= y + 20;
	s_preferences.crosshair.generic.left		= PREFERENCES_X_POS - ( ( strlen(s_preferences.crosshair.generic.name) + 1 ) * SMALLCHAR_WIDTH );
	s_preferences.crosshair.generic.right		= PREFERENCES_X_POS + 48;

        y += BIGCHAR_HEIGHT+2;
	s_preferences.crosshairHealth.generic.type        = MTYPE_RADIOBUTTON;
	s_preferences.crosshairHealth.generic.name	      = "Crosshair shows health:";
	s_preferences.crosshairHealth.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.crosshairHealth.generic.callback    = Preferences_Event;
	s_preferences.crosshairHealth.generic.id          = ID_CROSSHAIRHEALTH;
	s_preferences.crosshairHealth.generic.x	          = PREFERENCES_X_POS;
	s_preferences.crosshairHealth.generic.y	          = y;

        y += BIGCHAR_HEIGHT;
        s_preferences.crosshairColorRed.generic.type		= MTYPE_SLIDER;
	s_preferences.crosshairColorRed.generic.name		= "Crosshair red:";
	s_preferences.crosshairColorRed.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.crosshairColorRed.generic.callback	= Preferences_Event;
	s_preferences.crosshairColorRed.generic.id		= ID_COLORRED;
	s_preferences.crosshairColorRed.generic.x			= PREFERENCES_X_POS;
	s_preferences.crosshairColorRed.generic.y			= y;
	s_preferences.crosshairColorRed.minvalue			= 0.0f;
	s_preferences.crosshairColorRed.maxvalue			= 255.0f;

        y += BIGCHAR_HEIGHT+2;
        s_preferences.crosshairColorGreen.generic.type		= MTYPE_SLIDER;
	s_preferences.crosshairColorGreen.generic.name		= "Crosshair green:";
	s_preferences.crosshairColorGreen.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.crosshairColorGreen.generic.callback	= Preferences_Event;
	s_preferences.crosshairColorGreen.generic.id		= ID_COLORGREEN;
	s_preferences.crosshairColorGreen.generic.x			= PREFERENCES_X_POS;
	s_preferences.crosshairColorGreen.generic.y			= y;
	s_preferences.crosshairColorGreen.minvalue			= 0.0f;
	s_preferences.crosshairColorGreen.maxvalue			= 255.0f;

        y += BIGCHAR_HEIGHT+2;
        s_preferences.crosshairColorBlue.generic.type		= MTYPE_SLIDER;
	s_preferences.crosshairColorBlue.generic.name		= "Crosshair blue:";
	s_preferences.crosshairColorBlue.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.crosshairColorBlue.generic.callback	= Preferences_Event;
	s_preferences.crosshairColorBlue.generic.id		= ID_COLORBLUE;
	s_preferences.crosshairColorBlue.generic.x			= PREFERENCES_X_POS;
	s_preferences.crosshairColorBlue.generic.y			= y;
	s_preferences.crosshairColorBlue.minvalue			= 0.0f;
	s_preferences.crosshairColorBlue.maxvalue			= 255.0f;


        if(s_preferences.crosshairHealth.curvalue) {
            s_preferences.crosshairColorRed.generic.flags       |= QMF_INACTIVE;
            s_preferences.crosshairColorGreen.generic.flags       |= QMF_INACTIVE;
            s_preferences.crosshairColorBlue.generic.flags       |= QMF_INACTIVE;
        }

	y += BIGCHAR_HEIGHT+2+4;
	s_preferences.simpleitems.generic.type        = MTYPE_RADIOBUTTON;
	s_preferences.simpleitems.generic.name	      = "Simple Items:";
	s_preferences.simpleitems.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.simpleitems.generic.callback    = Preferences_Event;
	s_preferences.simpleitems.generic.id          = ID_SIMPLEITEMS;
	s_preferences.simpleitems.generic.x	          = PREFERENCES_X_POS;
	s_preferences.simpleitems.generic.y	          = y;
        
        //Elimination
        y += BIGCHAR_HEIGHT;
	s_preferences.alwaysweaponbar.generic.type        = MTYPE_RADIOBUTTON;
	s_preferences.alwaysweaponbar.generic.name	      = "Always show weapons:";
	s_preferences.alwaysweaponbar.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.alwaysweaponbar.generic.callback    = Preferences_Event;
	s_preferences.alwaysweaponbar.generic.id          = ID_WEAPONBAR;
	s_preferences.alwaysweaponbar.generic.x	          = PREFERENCES_X_POS;
	s_preferences.alwaysweaponbar.generic.y	          = y;

	y += BIGCHAR_HEIGHT;
	s_preferences.wallmarks.generic.type          = MTYPE_RADIOBUTTON;
	s_preferences.wallmarks.generic.name	      = "Marks on Walls:";
	s_preferences.wallmarks.generic.flags	      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.wallmarks.generic.callback      = Preferences_Event;
	s_preferences.wallmarks.generic.id            = ID_WALLMARKS;
	s_preferences.wallmarks.generic.x	          = PREFERENCES_X_POS;
	s_preferences.wallmarks.generic.y	          = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.brass.generic.type              = MTYPE_RADIOBUTTON;
	s_preferences.brass.generic.name	          = "Ejecting Brass:";
	s_preferences.brass.generic.flags	          = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.brass.generic.callback          = Preferences_Event;
	s_preferences.brass.generic.id                = ID_EJECTINGBRASS;
	s_preferences.brass.generic.x	              = PREFERENCES_X_POS;
	s_preferences.brass.generic.y	              = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.dynamiclights.generic.type      = MTYPE_RADIOBUTTON;
	s_preferences.dynamiclights.generic.name	  = "Dynamic Lights:";
	s_preferences.dynamiclights.generic.flags     = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.dynamiclights.generic.callback  = Preferences_Event;
	s_preferences.dynamiclights.generic.id        = ID_DYNAMICLIGHTS;
	s_preferences.dynamiclights.generic.x	      = PREFERENCES_X_POS;
	s_preferences.dynamiclights.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.identifytarget.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.identifytarget.generic.name	  = "Identify Target:";
	s_preferences.identifytarget.generic.flags    = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.identifytarget.generic.callback = Preferences_Event;
	s_preferences.identifytarget.generic.id       = ID_IDENTIFYTARGET;
	s_preferences.identifytarget.generic.x	      = PREFERENCES_X_POS;
	s_preferences.identifytarget.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.highqualitysky.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.highqualitysky.generic.name	  = "High Quality Sky:";
	s_preferences.highqualitysky.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.highqualitysky.generic.callback = Preferences_Event;
	s_preferences.highqualitysky.generic.id       = ID_HIGHQUALITYSKY;
	s_preferences.highqualitysky.generic.x	      = PREFERENCES_X_POS;
	s_preferences.highqualitysky.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.synceveryframe.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.synceveryframe.generic.name	  = "Sync Every Frame:";
	s_preferences.synceveryframe.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.synceveryframe.generic.callback = Preferences_Event;
	s_preferences.synceveryframe.generic.id       = ID_SYNCEVERYFRAME;
	s_preferences.synceveryframe.generic.x	      = PREFERENCES_X_POS;
	s_preferences.synceveryframe.generic.y	      = y;

	y += BIGCHAR_HEIGHT+2;
	s_preferences.forcemodel.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.forcemodel.generic.name	  = "Force Player Models:";
	s_preferences.forcemodel.generic.flags	  = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.forcemodel.generic.callback = Preferences_Event;
	s_preferences.forcemodel.generic.id       = ID_FORCEMODEL;
	s_preferences.forcemodel.generic.x	      = PREFERENCES_X_POS;
	s_preferences.forcemodel.generic.y	      = y;

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
	s_preferences.delaghitscan.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.delaghitscan.generic.name	   = "Unlag hitscan:";
	s_preferences.delaghitscan.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.delaghitscan.generic.callback = Preferences_Event;
	s_preferences.delaghitscan.generic.id       = ID_DELAGHITSCAN;
	s_preferences.delaghitscan.generic.x	       = PREFERENCES_X_POS;
	s_preferences.delaghitscan.generic.y	       = y;
        
	y += BIGCHAR_HEIGHT+2;
	s_preferences.allowdownload.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.allowdownload.generic.name	   = "Automatic Downloading:";
	s_preferences.allowdownload.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.allowdownload.generic.callback = Preferences_Event;
	s_preferences.allowdownload.generic.id       = ID_ALLOWDOWNLOAD;
	s_preferences.allowdownload.generic.x	       = PREFERENCES_X_POS;
	s_preferences.allowdownload.generic.y	       = y;
        
        y += BIGCHAR_HEIGHT+2;
	s_preferences.chatbeep.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.chatbeep.generic.name	   = "Beep on chat:";
	s_preferences.chatbeep.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.chatbeep.generic.callback = Preferences_Event;
	s_preferences.chatbeep.generic.id       = ID_CHATBEEP;
	s_preferences.chatbeep.generic.x	       = PREFERENCES_X_POS;
	s_preferences.chatbeep.generic.y	       = y;
        
        y += BIGCHAR_HEIGHT+2;
	s_preferences.teamchatbeep.generic.type     = MTYPE_RADIOBUTTON;
	s_preferences.teamchatbeep.generic.name	   = "Beep on team chat:";
	s_preferences.teamchatbeep.generic.flags	   = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_preferences.teamchatbeep.generic.callback = Preferences_Event;
	s_preferences.teamchatbeep.generic.id       = ID_TEAMCHATBEEP;
	s_preferences.teamchatbeep.generic.x	       = PREFERENCES_X_POS;
	s_preferences.teamchatbeep.generic.y	       = y;

	y += BIGCHAR_HEIGHT+2;
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

	Menu_AddItem( &s_preferences.menu, &s_preferences.crosshair );
        Menu_AddItem( &s_preferences.menu, &s_preferences.crosshairHealth );
        Menu_AddItem( &s_preferences.menu, &s_preferences.crosshairColorRed );
        Menu_AddItem( &s_preferences.menu, &s_preferences.crosshairColorGreen );
        Menu_AddItem( &s_preferences.menu, &s_preferences.crosshairColorBlue );
	Menu_AddItem( &s_preferences.menu, &s_preferences.simpleitems );
        Menu_AddItem( &s_preferences.menu, &s_preferences.alwaysweaponbar );
	Menu_AddItem( &s_preferences.menu, &s_preferences.wallmarks );
	Menu_AddItem( &s_preferences.menu, &s_preferences.brass );
	Menu_AddItem( &s_preferences.menu, &s_preferences.dynamiclights );
	Menu_AddItem( &s_preferences.menu, &s_preferences.identifytarget );
	Menu_AddItem( &s_preferences.menu, &s_preferences.highqualitysky );
	Menu_AddItem( &s_preferences.menu, &s_preferences.synceveryframe );
	Menu_AddItem( &s_preferences.menu, &s_preferences.forcemodel );
	Menu_AddItem( &s_preferences.menu, &s_preferences.drawteamoverlay );
        Menu_AddItem( &s_preferences.menu, &s_preferences.delaghitscan );
	Menu_AddItem( &s_preferences.menu, &s_preferences.allowdownload );
        Menu_AddItem( &s_preferences.menu, &s_preferences.teamchatbeep );
        Menu_AddItem( &s_preferences.menu, &s_preferences.chatbeep );

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
	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
                if (n < 10)
			s_preferences.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
		else
			s_preferences.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%02d", n - 10) );
	
	}
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

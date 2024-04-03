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

#define DMFLAGS_X_POS				320

#define ID_BACK					127
#define ID_NO_FALLING				128
#define ID_FIXED_FOV				129
#define ID_NO_FOOTSTEPS				130
#define ID_INSTANT_WEAPON_CHANGE		131
#define ID_NO_BUNNY				132
#define ID_INVIS				133
#define ID_LIGHT_VOTING				134
#define ID_NO_SELF_DAMAGE			135


typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;

	menuradiobutton_s	noFalling;
	menuradiobutton_s	fixedFOV;
	menuradiobutton_s	noFootsteps;
	menuradiobutton_s	instantWeaponChange;
	menuradiobutton_s	noBunny;
	menuradiobutton_s	invis;
	menuradiobutton_s	lightVoting;
	menuradiobutton_s	noSelfDamage;
	menutext_s		bitfield;

	menubitmap_s		back;
} dmflagsOptions_t;

static dmflagsOptions_t dmflagsOptions_s;

static void DMFlags_DrawBitfield( void *self ) {
	menufield_s	*f;
	float		*color;
	int		x, y;

	f = (menufield_s*)self;
	x = f->generic.x;
	y = f->generic.y;

	if ( trap_Cvar_VariableValue( "dmflags" ) ) {
		color = text_color_normal;
	} else {
		color = text_color_disabled;
	}

	UI_DrawString( x - 8, y, f->generic.name, UI_RIGHT|UI_SMALLFONT, color );
	UI_DrawString( x + 48, y, UI_Cvar_VariableString( "dmflags" ), UI_SMALLFONT|UI_RIGHT, color );
}

static void DMflagsOptions_Event( void* ptr, int notification ) {
	int	bitfield;

	bitfield = trap_Cvar_VariableValue( "dmflags" );

	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_NO_FALLING:
		if ( dmflagsOptions_s.noFalling.curvalue ) {
			bitfield |= DF_NO_FALLING;
		} else {
			bitfield &= ~DF_NO_FALLING;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_FIXED_FOV:
		if ( dmflagsOptions_s.fixedFOV.curvalue ) {
			bitfield |= DF_FIXED_FOV;
		} else {
			bitfield &= ~DF_FIXED_FOV;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_NO_FOOTSTEPS:
		if ( dmflagsOptions_s.noFootsteps.curvalue ) {
			bitfield |= DF_NO_FOOTSTEPS;
		} else {
			bitfield &= ~DF_NO_FOOTSTEPS;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_INSTANT_WEAPON_CHANGE:
		if ( dmflagsOptions_s.instantWeaponChange.curvalue ) {
			bitfield |= DF_INSTANT_WEAPON_CHANGE;
		} else {
			bitfield &= ~DF_INSTANT_WEAPON_CHANGE;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_NO_BUNNY:
		if ( dmflagsOptions_s.noBunny.curvalue ) {
			bitfield |= DF_NO_BUNNY;
		} else {
			bitfield &= ~DF_NO_BUNNY;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_INVIS:
		if ( dmflagsOptions_s.invis.curvalue ) {
			bitfield |= DF_INVIS;
		} else {
			bitfield &= ~DF_INVIS;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_LIGHT_VOTING:
		if ( dmflagsOptions_s.lightVoting.curvalue ) {
			bitfield |= DF_LIGHT_VOTING;
		} else {
			bitfield &= ~DF_LIGHT_VOTING;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_NO_SELF_DAMAGE:
		if ( dmflagsOptions_s.noSelfDamage.curvalue ) {
			bitfield |= DF_NO_SELF_DAMAGE;
		} else {
			bitfield &= ~DF_NO_SELF_DAMAGE;
		}
		trap_Cvar_SetValue( "dmflags", bitfield );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

static void DMflagsOptions_MenuInit( void ) {
	int				bitfield;

	bitfield = trap_Cvar_VariableValue( "dmflags" );

	memset( &dmflagsOptions_s, 0 ,sizeof(dmflagsOptions_t) );

	UI_DMflagsOptionsMenu_Cache();

	dmflagsOptions_s.menu.wrapAround = qtrue;
	dmflagsOptions_s.menu.fullscreen = qtrue;

	dmflagsOptions_s.banner.generic.type	= MTYPE_BTEXT;
	dmflagsOptions_s.banner.generic.x		= 320;
	dmflagsOptions_s.banner.generic.y		= 16;
	dmflagsOptions_s.banner.string			= "DMFLAGS";
	dmflagsOptions_s.banner.color			= color_white;
	dmflagsOptions_s.banner.style			= UI_CENTER;

	dmflagsOptions_s.framel.generic.type	= MTYPE_BITMAP;
	dmflagsOptions_s.framel.generic.name	= ART_FRAMEL;
	dmflagsOptions_s.framel.generic.flags	= QMF_INACTIVE;
	dmflagsOptions_s.framel.generic.x		= 0;
	dmflagsOptions_s.framel.generic.y		= 78;
	dmflagsOptions_s.framel.width			= 256;
	dmflagsOptions_s.framel.height			= 329;

	dmflagsOptions_s.framer.generic.type	= MTYPE_BITMAP;
	dmflagsOptions_s.framer.generic.name	= ART_FRAMER;
	dmflagsOptions_s.framer.generic.flags	= QMF_INACTIVE;
	dmflagsOptions_s.framer.generic.x		= 376;
	dmflagsOptions_s.framer.generic.y		= 76;
	dmflagsOptions_s.framer.width			= 256;
	dmflagsOptions_s.framer.height			= 334;

	dmflagsOptions_s.noFalling.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noFalling.generic.name		= "No Falling Damage:";
	dmflagsOptions_s.noFalling.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noFalling.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noFalling.generic.id		= ID_NO_FALLING;
	dmflagsOptions_s.noFalling.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.noFalling.generic.y		= 240 - 6.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.fixedFOV.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.fixedFOV.generic.name		= "Fixed FOV:";
	dmflagsOptions_s.fixedFOV.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.fixedFOV.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.fixedFOV.generic.id		= ID_FIXED_FOV;
	dmflagsOptions_s.fixedFOV.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.fixedFOV.generic.y		= 240 - 5.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.noFootsteps.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noFootsteps.generic.name		= "No Footsteps:";
	dmflagsOptions_s.noFootsteps.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noFootsteps.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noFootsteps.generic.id		= ID_NO_FOOTSTEPS;
	dmflagsOptions_s.noFootsteps.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.noFootsteps.generic.y		= 240 - 4.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.instantWeaponChange.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.instantWeaponChange.generic.name		= "Instant Weapon Change:";
	dmflagsOptions_s.instantWeaponChange.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.instantWeaponChange.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.instantWeaponChange.generic.id		= ID_INSTANT_WEAPON_CHANGE;
	dmflagsOptions_s.instantWeaponChange.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.instantWeaponChange.generic.y		= 240 - 3.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.noBunny.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noBunny.generic.name		= "No Strafe Jumping:";
	dmflagsOptions_s.noBunny.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noBunny.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noBunny.generic.id		= ID_NO_BUNNY;
	dmflagsOptions_s.noBunny.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.noBunny.generic.y		= 240 - 2.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.invis.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.invis.generic.name		= "Total Invisibility:";
	dmflagsOptions_s.invis.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.invis.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.invis.generic.id		= ID_INVIS;
	dmflagsOptions_s.invis.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.invis.generic.y		= 240 - 1.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.lightVoting.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.lightVoting.generic.name		= "Light Voting:";
	dmflagsOptions_s.lightVoting.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.lightVoting.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.lightVoting.generic.id		= ID_LIGHT_VOTING;
	dmflagsOptions_s.lightVoting.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.lightVoting.generic.y		= 240 - 0.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.noSelfDamage.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noSelfDamage.generic.name		= "No Self Damage from Weapons:";
	dmflagsOptions_s.noSelfDamage.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noSelfDamage.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noSelfDamage.generic.id		= ID_NO_SELF_DAMAGE;
	dmflagsOptions_s.noSelfDamage.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.noSelfDamage.generic.y		= 240 + 0.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.bitfield.generic.type		= MTYPE_TEXT;
	dmflagsOptions_s.bitfield.generic.name	= "DMFlags:";
	dmflagsOptions_s.bitfield.generic.ownerdraw	= DMFlags_DrawBitfield;
	dmflagsOptions_s.bitfield.generic.x		= DMFLAGS_X_POS;
	dmflagsOptions_s.bitfield.generic.y		= 240 + 2.5 * (BIGCHAR_HEIGHT+2);

	dmflagsOptions_s.back.generic.type		= MTYPE_BITMAP;
	dmflagsOptions_s.back.generic.name		= ART_BACK0;
	dmflagsOptions_s.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	dmflagsOptions_s.back.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.back.generic.id		= ID_BACK;
	dmflagsOptions_s.back.generic.x			= 0;
	dmflagsOptions_s.back.generic.y			= 480-64;
	dmflagsOptions_s.back.width				= 128;
	dmflagsOptions_s.back.height			= 64;
	dmflagsOptions_s.back.focuspic			= ART_BACK1;

	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.banner );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.framel );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.framer );

	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noFalling );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.fixedFOV );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noFootsteps );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.instantWeaponChange );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noBunny );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.invis );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.lightVoting );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noSelfDamage );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.bitfield );

	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.back );

	dmflagsOptions_s.noFalling.curvalue = bitfield&DF_NO_FALLING ? 1 : 0;
	dmflagsOptions_s.fixedFOV.curvalue = bitfield&DF_FIXED_FOV ? 1 : 0;
	dmflagsOptions_s.noFootsteps.curvalue = bitfield&DF_NO_FOOTSTEPS ? 1 : 0;
	dmflagsOptions_s.instantWeaponChange.curvalue = bitfield&DF_INSTANT_WEAPON_CHANGE ? 1 : 0;
	dmflagsOptions_s.noBunny.curvalue = bitfield&DF_NO_BUNNY ? 1 : 0;
	dmflagsOptions_s.invis.curvalue = bitfield&DF_INVIS ? 1 : 0;
	dmflagsOptions_s.lightVoting.curvalue = bitfield&DF_LIGHT_VOTING ? 1 : 0;
	dmflagsOptions_s.noSelfDamage.curvalue = bitfield&DF_NO_SELF_DAMAGE ? 1 : 0;
}


/*
===============
UI_DMflagsOptionsMenu_Cache
===============
*/
void UI_DMflagsOptionsMenu_Cache( void ) {

	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
}


/*
===============
UI_DMflagsOptionsMenu
===============
*/
void UI_DMflagsOptionsMenu( void ) {
	DMflagsOptions_MenuInit();
	UI_PushMenu( &dmflagsOptions_s.menu );
}

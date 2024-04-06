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

#define ID_BACK						127
#define ID_NO_FALLING				128
#define ID_FIXED_FOV				129
#define ID_NO_FOOTSTEPS				130
#define ID_INSTANT_WEAPON_CHANGE	131
#define ID_NO_BUNNY					132
#define ID_INVIS					133
#define ID_LIGHT_VOTING				134
#define ID_NO_SELF_DAMAGE			135
#define ID_PHYSICS_RULESET			136
#define ID_GRAPPLING_HOOK			137
#define ID_NORMAL_WEAPONS_RULESET	138
#define ID_WEAPON_ARENA_WEAPON		139
#define ID_AWARD_PUSHING_RULE		140
#define ID_HARVESTER_FROM_BODIES	141
#define ID_OVERLOAD_OBELISK_HEALTH	142
#define ID_OVERLOAD_RESPAWN_DELAY	143
#define ID_ELIM_WEAPONS_RULESET		144
#define ID_ECTF_ONEWAY_MODE			145
#define ID_LMS_SCORING_RULE			146
#define ID_LMS_LIVES				147
#define ID_DD_CAPTURE_TIME			148
#define ID_DD_RESPAWN_DELAY			149


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

	// Moved from ui_startserver.c
	int					gametype;
	menulist_s			pmove;
	menuradiobutton_s	grapple;
	menulist_s			weaponMode;
	menulist_s			weaponArenaWeapon;
	menulist_s			awardPushing;
	menuradiobutton_s	harvesterFromBodies;
	menufield_s			overloadObeliskHealth;
	menufield_s			overloadRespawnDelay;
	menulist_s			eliminationWeaponMode;
	menuradiobutton_s	eCTFOneWay;
	menulist_s			lmsMode;
	menufield_s			lmsLives;
	menufield_s			ddCaptureTime;
	menufield_s			ddRespawnDelay;

	menubitmap_s		back;
} dmflagsOptions_t;

static dmflagsOptions_t dmflagsOptions_s;

//Elimiantion - LMS mode
static const char *lmsMode_list[] = {
	"Round Win + Overtime",
	"Round Win Only",
	"Kill + Overtime",
	"Kill Only",
	NULL
};

static const char *weaponArenaWeapon_list[] = {
	"Gauntlet",
	"Machinegun",
	"Shotgun",
	"Grenade L.",
	"Rocket L.",
	"Lightning",
	"Railgun",
	"Plasma Gun",
	"BFG",
	"Nailgun",
	"Chaingun",
	"Prox Mine",
	NULL
};

static const char *weaponMode_list[] = {
	"All Weapons (Default)",
	"Instantgib",
	"Single Weapon Arena",
	"Classic Arena",
	"All Weapons (Elim.)",
	NULL
};

static const char *weaponModeElimination_list[] = {
	"All Weapons (Elim.)",
	"Instantgib",
	"Single Weapon Arena",
	"Classic Arena",
	NULL
};

static const char *pmove_list[] = {
	"Framerate Dependant",
	"Fixed framerate 125Hz",
	"Fixed framerate 91Hz",
	"Accurate",
	NULL
};

static const char *awardPushing_list[] = {
	"Disabled",
	"Last Pusher",
	"Last Attacker",
	NULL
};

/*
=================
DMFlags_StatusBar
=================
*/
static void DMFlags_StatusBar( void *ptr ) {
	switch( ((menucommon_s*)ptr)->id ) {
	case ID_NO_FALLING:
		UI_DrawString(320, 400, "Players do not take damage when landing from", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "great heights. Doesn't affect traps.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_FIXED_FOV:
		UI_DrawString(320, 400, "Forces all players Field of View to look like", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "the default FOV. Zoom FOV is fixed too.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_NO_FOOTSTEPS:
		UI_DrawString(320, 400, "Disables footstep sounds. Jumping, pickups and", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "weapon sounds can still reveal players presence.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_INSTANT_WEAPON_CHANGE:
		UI_DrawString(320, 400, "Weapon switch takes no time. Weapons still need", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "to reload before being changed.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_NO_BUNNY:
		UI_DrawString(320, 400, "Limited speed, non-slippy movement. Be careful", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "on accelerator pads. Some maps may be broken.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_INVIS:
		UI_DrawString(320, 400, "Players with Invisibility powerup are completely", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "invisible, instead of being almost invisible.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_LIGHT_VOTING:
		UI_DrawString(320, 400, "Makes voting easier to pass.", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "Abstensions less likely cause vote fail.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_NO_SELF_DAMAGE:
		UI_DrawString(320, 400, "Players get no damage from their own weapons.", UI_SMALLFONT|UI_CENTER, colorWhite );
		UI_DrawString(320, 420, "Useful for rocket jump, plasma climb, etc.", UI_SMALLFONT|UI_CENTER, colorWhite );
		break;

	case ID_PHYSICS_RULESET:
		switch( ((menulist_s*)ptr)->curvalue ) {
			case 0:
				UI_DrawString( 320, 440, "Physics depends on players' framerates", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Not all players are equal", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 1:
			case 2:
				UI_DrawString( 320, 440, "Physics are calculated at fixed intervals", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "All players are equal", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 3:
				UI_DrawString( 320, 440, "Physics are calculated exactly", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "All players are equal", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			default:
				UI_DrawString( 320, 440, "Framerate dependent or not", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
		}
		break;

	case ID_GRAPPLING_HOOK:
		UI_DrawString( 320, 440, "Adds the Grappling Hook to", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "the players' spawning inventory.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;

	case ID_NORMAL_WEAPONS_RULESET:
		switch( ((menulist_s*)ptr)->curvalue ) {
			case 1:
				UI_DrawString( 320, 440, "Instantgib: All pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players spawn with a one-hit-frag Railgun.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 2:
				UI_DrawString( 320, 440, "Single Weapon Arena: All pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players will spawn with a specific weapon.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 3:
				UI_DrawString( 320, 440, "Classic Arena: No pickups removed. Replaces some", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "weapons and items to match the OG experience.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 4:
				UI_DrawString( 320, 440, "All Weapons (Elimination): All pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players spawn with all weapons and full HP/AP.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			default:
				UI_DrawString( 320, 440, "All Weapons (Default): No pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players spawn with Gauntlet and Machinegun.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
		}
		break;
	
	case ID_WEAPON_ARENA_WEAPON:
		UI_DrawString( 320, 440, "In Single Weapon Arena mode, this will", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "be the weapon players will spawn with.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	
	case ID_AWARD_PUSHING_RULE:
		switch( ((menulist_s*)ptr)->curvalue ) {
			case 1:
				UI_DrawString( 320, 440, "Last Pusher: If a player is pushed into a level", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "hazard and dies, their pusher scores a point.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 2:
				UI_DrawString( 320, 440, "Last Attacker: If a player suicides or dies", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "via level hazard, their last attacker scores.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			default: // case 0
				UI_DrawString( 320, 440, "Disabled: If a player dies due to self-damage", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "or level hazard, they lose a point.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
		}
		break;
	
	case ID_HARVESTER_FROM_BODIES:
		UI_DrawString( 320, 440, "Makes skulls spawn from fragged players", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "rather than a central skull generator.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	
	case ID_OVERLOAD_OBELISK_HEALTH:
		UI_DrawString( 320, 440, "Specifies the amount of damage taken by", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "an obelisk before it goes down.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	
	case ID_OVERLOAD_RESPAWN_DELAY:
		UI_DrawString( 320, 440, "Specifies the amount of time between", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "scoring and the start of the next round.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;

	case ID_ELIM_WEAPONS_RULESET:
		switch( ((menulist_s*)ptr)->curvalue ) {
			case 1:
				UI_DrawString( 320, 440, "Instantgib: All pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players spawn with a one-hit-frag Railgun.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 2:
				UI_DrawString( 320, 440, "Single Weapon Arena: All pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players will spawn with a specific weapon.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 3:
				UI_DrawString( 320, 440, "Classic Arena: No pickups removed. Replaces some", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "weapons and items to match the OG experience.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			default:
				UI_DrawString( 320, 440, "All Weapons (Elimination): All pickups removed.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "Players spawn with all weapons and full HP/AP.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
		}
		break;

	case ID_ECTF_ONEWAY_MODE:
		UI_DrawString( 320, 440, "Every round, teams switch between attacker and", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "defender roles. Only attackers can capture.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	
	case ID_LMS_SCORING_RULE:
		switch( ((menulist_s*)ptr)->curvalue ) {
			case 1:
				UI_DrawString( 320, 440, "Round Win Only Scoring: All surviving players", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "at the end of a round score a point.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 2:
				UI_DrawString( 320, 440, "Frag+Overtime Scoring: Every frag earns a point.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "In Overtime, players keep racking up frags.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			case 3:
				UI_DrawString( 320, 440, "Frag-Only Scoring: Only frags earn points.", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "It doesn't matter if they survive a round.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
			default:
				UI_DrawString( 320, 440, "Round+Overtime: Only the last one alive at round", UI_CENTER|UI_SMALLFONT, colorWhite );
				UI_DrawString( 320, 460, "end scores. Players are slowly eliminated in OT.", UI_CENTER|UI_SMALLFONT, colorWhite );
				break;
		}
		break;
	
	case ID_LMS_LIVES:
		UI_DrawString( 320, 440, "Specifies the amount of lives each player must", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "spend before being eliminated from the round.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	
	case ID_DD_CAPTURE_TIME:
		UI_DrawString( 320, 440, "Specifies the amount of time both points need", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "to be held in order for a team to score a point.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	
	case ID_DD_RESPAWN_DELAY:
		UI_DrawString( 320, 440, "Specifies the amount of time between", UI_CENTER|UI_SMALLFONT, colorWhite );
		UI_DrawString( 320, 460, "scoring and the start of the next round.", UI_CENTER|UI_SMALLFONT, colorWhite );
		break;
	}
}

/*
=================
DMFlags_DrawBitfield
=================
*/
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

/*
=================
DMflagsOptions_Event
=================
*/
static void DMflagsOptions_Event( void* ptr, int notification ) {
	int	dmFlagsBitfields;

	dmFlagsBitfields = trap_Cvar_VariableValue( "dmflags" );
	dmflagsOptions_s.pmove.curvalue = Com_Clamp(0, 1, trap_Cvar_VariableValue( "ui_advOptionsPmove" ));
	dmflagsOptions_s.grapple.curvalue = Com_Clamp(0, 1, trap_Cvar_VariableValue( "g_grapple" ));
	dmflagsOptions_s.weaponMode.curvalue = Com_Clamp(0, 1, trap_Cvar_VariableValue( "g_grapple" ));
	dmflagsOptions_s.weaponArenaWeapon.curvalue = Com_Clamp(0, 1, trap_Cvar_VariableValue( "g_weaponArenaWeapon" ));
	dmflagsOptions_s.awardPushing.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_awardPushing" ) );
	dmflagsOptions_s.harvesterFromBodies.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "g_harvesterFromBodies" ) );
	Q_strncpyz( dmflagsOptions_s.overloadObeliskHealth.field.buffer, UI_Cvar_VariableString( "g_obeliskHealth" ), sizeof( dmflagsOptions_s.overloadObeliskHealth.field.buffer ) );
	Q_strncpyz( dmflagsOptions_s.overloadRespawnDelay.field.buffer, UI_Cvar_VariableString( "g_obeliskRespawnDelay" ), sizeof( dmflagsOptions_s.overloadRespawnDelay.field.buffer ) );
	dmflagsOptions_s.eCTFOneWay.curvalue = Com_Clamp( 0, 1, trap_Cvar_VariableValue( "elimination_ctf_oneway" ) );
	dmflagsOptions_s.lmsMode.curvalue = Com_Clamp( 0, 3, trap_Cvar_VariableValue("g_lms_mode") );
	Q_strncpyz( dmflagsOptions_s.lmsLives.field.buffer, UI_Cvar_VariableString( "g_lms_lives" ), sizeof( dmflagsOptions_s.lmsLives.field.buffer ) );
	Q_strncpyz( dmflagsOptions_s.ddCaptureTime.field.buffer, UI_Cvar_VariableString( "g_ddCaptureTime" ), sizeof( dmflagsOptions_s.ddCaptureTime.field.buffer ) );
	Q_strncpyz( dmflagsOptions_s.ddRespawnDelay.field.buffer, UI_Cvar_VariableString( "g_ddRespawnDelay" ), sizeof( dmflagsOptions_s.ddRespawnDelay.field.buffer ) );

	if( notification != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_NO_FALLING:
		if ( dmflagsOptions_s.noFalling.curvalue ) {
			dmFlagsBitfields |= DF_NO_FALLING;
		} else {
			dmFlagsBitfields &= ~DF_NO_FALLING;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_FIXED_FOV:
		if ( dmflagsOptions_s.fixedFOV.curvalue ) {
			dmFlagsBitfields |= DF_FIXED_FOV;
		} else {
			dmFlagsBitfields &= ~DF_FIXED_FOV;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_NO_FOOTSTEPS:
		if ( dmflagsOptions_s.noFootsteps.curvalue ) {
			dmFlagsBitfields |= DF_NO_FOOTSTEPS;
		} else {
			dmFlagsBitfields &= ~DF_NO_FOOTSTEPS;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_INSTANT_WEAPON_CHANGE:
		if ( dmflagsOptions_s.instantWeaponChange.curvalue ) {
			dmFlagsBitfields |= DF_INSTANT_WEAPON_CHANGE;
		} else {
			dmFlagsBitfields &= ~DF_INSTANT_WEAPON_CHANGE;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_NO_BUNNY:
		if ( dmflagsOptions_s.noBunny.curvalue ) {
			dmFlagsBitfields |= DF_NO_BUNNY;
		} else {
			dmFlagsBitfields &= ~DF_NO_BUNNY;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_INVIS:
		if ( dmflagsOptions_s.invis.curvalue ) {
			dmFlagsBitfields |= DF_INVIS;
		} else {
			dmFlagsBitfields &= ~DF_INVIS;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_LIGHT_VOTING:
		if ( dmflagsOptions_s.lightVoting.curvalue ) {
			dmFlagsBitfields |= DF_LIGHT_VOTING;
		} else {
			dmFlagsBitfields &= ~DF_LIGHT_VOTING;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_NO_SELF_DAMAGE:
		if ( dmflagsOptions_s.noSelfDamage.curvalue ) {
			dmFlagsBitfields |= DF_NO_SELF_DAMAGE;
		} else {
			dmFlagsBitfields &= ~DF_NO_SELF_DAMAGE;
		}
		trap_Cvar_SetValue( "dmflags", dmFlagsBitfields );
		break;

	case ID_PHYSICS_RULESET:
		if(trap_Cvar_VariableValue( "pmove_fixed" )) {
			dmflagsOptions_s.pmove.curvalue = 1;
		}
		if(trap_Cvar_VariableValue( "pmove_fixed" ) && trap_Cvar_VariableValue( "pmove_msec" )==11) {
			dmflagsOptions_s.pmove.curvalue = 2;
		}
		if(trap_Cvar_VariableValue( "pmove_float" )) {
			dmflagsOptions_s.pmove.curvalue = 3;
		}
		else {
			dmflagsOptions_s.pmove.curvalue = 0;
		}
		trap_Cvar_SetValue( "ui_advOptionsPmove", dmflagsOptions_s.pmove.curvalue );
		break;

	case ID_GRAPPLING_HOOK:
		trap_Cvar_SetValue( "g_grapple", dmflagsOptions_s.grapple.curvalue );
		break;

	case ID_NORMAL_WEAPONS_RULESET:
		// Instantgib mode
		if(trap_Cvar_VariableValue("g_instantgib") != 0 && trap_Cvar_VariableValue("g_weaponArena") == 0 && trap_Cvar_VariableValue("g_elimination") == 0 && trap_Cvar_VariableValue("g_classicMode") == 0) {
			dmflagsOptions_s.weaponMode.curvalue = 1;
		}
		// Single Weapon mode
		else if(trap_Cvar_VariableValue("g_instantgib") == 0 && trap_Cvar_VariableValue("g_weaponArena") != 0 && trap_Cvar_VariableValue("g_elimination") == 0 && trap_Cvar_VariableValue("g_classicMode") == 0) {
			dmflagsOptions_s.weaponMode.curvalue = 2;
		}
		// Classic mode
		else if(trap_Cvar_VariableValue("g_instantgib") == 0 && trap_Cvar_VariableValue("g_weaponArena") == 0 && trap_Cvar_VariableValue("g_elimination") == 0 && trap_Cvar_VariableValue("g_classicMode") != 0) {
			dmflagsOptions_s.weaponMode.curvalue = 3;
		}
		// All Weapons Elimination mode
		else if(trap_Cvar_VariableValue("g_instantgib") == 0 && trap_Cvar_VariableValue("g_weaponArena") == 0 && trap_Cvar_VariableValue("g_elimination") != 0 && trap_Cvar_VariableValue("g_classicMode") == 0) {
			dmflagsOptions_s.weaponMode.curvalue = 4;
		}
		// All Weapons Standard mode
		else {
			dmflagsOptions_s.weaponMode.curvalue = 0;
		}
		trap_Cvar_SetValue( "ui_advOptionsWeaponRuleset", dmflagsOptions_s.weaponMode.curvalue );
		break;

	case ID_WEAPON_ARENA_WEAPON:
		if (trap_Cvar_VariableValue("g_weaponArena") == 0) {
			dmflagsOptions_s.weaponArenaWeapon.generic.flags |= QMF_GRAYED;
		}
		else {
			dmflagsOptions_s.weaponArenaWeapon.generic.flags &= ~QMF_GRAYED;
			trap_Cvar_SetValue( "g_weaponArenaWeapon", dmflagsOptions_s.weaponArenaWeapon.curvalue );
		}
		break;

	case ID_AWARD_PUSHING_RULE:
		trap_Cvar_SetValue( "g_grapple", dmflagsOptions_s.awardPushing.curvalue );
		break;

	case ID_HARVESTER_FROM_BODIES:
		trap_Cvar_SetValue( "g_harvesterFromBodies", dmflagsOptions_s.harvesterFromBodies.curvalue );
		break;

	case ID_OVERLOAD_OBELISK_HEALTH:
		trap_Cvar_Set( "g_obeliskHealth", dmflagsOptions_s.overloadObeliskHealth.field.buffer );
		break;

	case ID_OVERLOAD_RESPAWN_DELAY:
		trap_Cvar_Set( "g_obeliskRespawnDelay", dmflagsOptions_s.overloadRespawnDelay.field.buffer );
		break;

	case ID_ELIM_WEAPONS_RULESET:
		// Instantgib mode
		if(trap_Cvar_VariableValue("g_instantgib") != 0 && trap_Cvar_VariableValue("g_weaponArena") == 0 && trap_Cvar_VariableValue("g_classicMode") == 0) {
			dmflagsOptions_s.weaponMode.curvalue = 1;
		}
		// Single Weapon mode
		else if(trap_Cvar_VariableValue("g_instantgib") == 0 && trap_Cvar_VariableValue("g_weaponArena") != 0 && trap_Cvar_VariableValue("g_classicMode") == 0) {
			dmflagsOptions_s.weaponMode.curvalue = 2;
		}
		// Classic mode
		else if(trap_Cvar_VariableValue("g_instantgib") == 0 && trap_Cvar_VariableValue("g_weaponArena") == 0 && trap_Cvar_VariableValue("g_classicMode") != 0) {
			dmflagsOptions_s.weaponMode.curvalue = 3;
		}
		// All Weapons Elimination mode
		else {
			dmflagsOptions_s.weaponMode.curvalue = 0;
		}
		trap_Cvar_SetValue( "ui_advOptionsElimWeaponRuleset", dmflagsOptions_s.weaponMode.curvalue );
		break;

	case ID_ECTF_ONEWAY_MODE:
		trap_Cvar_SetValue( "elimination_ctf_oneway", dmflagsOptions_s.eCTFOneWay.curvalue );
		break;

	case ID_LMS_SCORING_RULE:
		trap_Cvar_SetValue( "g_lms_mode", dmflagsOptions_s.lmsMode.curvalue );
		break;

	case ID_LMS_LIVES:
		trap_Cvar_Set( "g_lms_lives", dmflagsOptions_s.lmsLives.field.buffer );
		break;

	case ID_DD_CAPTURE_TIME:
		trap_Cvar_Set( "g_ddCaptureTime", dmflagsOptions_s.ddCaptureTime.field.buffer );
		break;

	case ID_DD_RESPAWN_DELAY:
		trap_Cvar_Set( "g_ddRespawnDelay", dmflagsOptions_s.ddRespawnDelay.field.buffer );
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

/*
=================
DMflagsOptions_MenuInit
=================
*/
static void DMflagsOptions_MenuInit( void ) {
	int bitfield = trap_Cvar_VariableValue( "dmflags" );

	memset( &dmflagsOptions_s, 0 ,sizeof(dmflagsOptions_t) );
	dmflagsOptions_s.gametype = (int)Com_Clamp( 0, GT_MAX_GAME_TYPE - 1, trap_Cvar_VariableValue( "g_gameType" ) );

	UI_DMflagsOptionsMenu_Cache();

	dmflagsOptions_s.menu.wrapAround = qtrue;
	dmflagsOptions_s.menu.fullscreen = qtrue;

	dmflagsOptions_s.banner.generic.type	= MTYPE_BTEXT;
	dmflagsOptions_s.banner.generic.x		= 320;
	dmflagsOptions_s.banner.generic.y		= 16;
	dmflagsOptions_s.banner.string			= "Advanced Options";
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

	dmflagsOptions_s.pmove.generic.type			= MTYPE_SPINCONTROL;
	dmflagsOptions_s.pmove.generic.name			= "Physics Ruleset:";
	dmflagsOptions_s.pmove.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.pmove.generic.callback		= DMflagsOptions_Event;
	dmflagsOptions_s.pmove.generic.id			= ID_PHYSICS_RULESET;
	dmflagsOptions_s.pmove.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.pmove.generic.y			= 240 - 7 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.pmove.itemnames			= pmove_list;
	dmflagsOptions_s.pmove.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.grapple.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.grapple.generic.name		= "Grappling Hook:";
	dmflagsOptions_s.grapple.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.grapple.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.grapple.generic.id			= ID_GRAPPLING_HOOK;
	dmflagsOptions_s.grapple.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.grapple.generic.y			= 240 - 6 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.grapple.generic.statusbar	= DMFlags_StatusBar;

	if (UI_IsARoundBasedGametype(dmflagsOptions_s.gametype)) {
		dmflagsOptions_s.eliminationWeaponMode.generic.type			= MTYPE_SPINCONTROL;
		dmflagsOptions_s.eliminationWeaponMode.generic.name			= "Weapons Ruleset:";
		dmflagsOptions_s.eliminationWeaponMode.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		dmflagsOptions_s.eliminationWeaponMode.generic.callback		= DMflagsOptions_Event;
		dmflagsOptions_s.eliminationWeaponMode.generic.id			= ID_ELIM_WEAPONS_RULESET;
		dmflagsOptions_s.eliminationWeaponMode.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.eliminationWeaponMode.generic.y			= 240 - 5 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.eliminationWeaponMode.itemnames			= weaponModeElimination_list;
		dmflagsOptions_s.eliminationWeaponMode.generic.statusbar	= DMFlags_StatusBar;
	}
	else {
		dmflagsOptions_s.weaponMode.generic.type		= MTYPE_SPINCONTROL;
		dmflagsOptions_s.weaponMode.generic.name		= "Weapons Ruleset:";
		dmflagsOptions_s.weaponMode.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		dmflagsOptions_s.weaponMode.generic.callback	= DMflagsOptions_Event;
		dmflagsOptions_s.weaponMode.generic.id			= ID_NORMAL_WEAPONS_RULESET;
		dmflagsOptions_s.weaponMode.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.weaponMode.generic.y			= 240 - 5 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.weaponMode.itemnames			= weaponMode_list;
		dmflagsOptions_s.weaponMode.generic.statusbar	= DMFlags_StatusBar;
	}

	dmflagsOptions_s.weaponArenaWeapon.generic.type			= MTYPE_SPINCONTROL;
	dmflagsOptions_s.weaponArenaWeapon.generic.name			= "SWA Mode Weapon:";
	dmflagsOptions_s.weaponArenaWeapon.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.weaponArenaWeapon.generic.callback		= DMflagsOptions_Event;
	dmflagsOptions_s.weaponArenaWeapon.generic.id			= ID_WEAPON_ARENA_WEAPON;
	dmflagsOptions_s.weaponArenaWeapon.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.weaponArenaWeapon.generic.y			= 240 - 4 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.weaponArenaWeapon.itemnames			= weaponArenaWeapon_list;
	dmflagsOptions_s.weaponArenaWeapon.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.awardPushing.generic.type		= MTYPE_SPINCONTROL;
	dmflagsOptions_s.awardPushing.generic.name		= "Award Pushing Rule:";
	dmflagsOptions_s.awardPushing.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.awardPushing.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.awardPushing.generic.id		= ID_AWARD_PUSHING_RULE;
	dmflagsOptions_s.awardPushing.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.awardPushing.generic.y			= 240 - 3 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.awardPushing.itemnames			= awardPushing_list;
	dmflagsOptions_s.awardPushing.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.noFalling.generic.type			= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noFalling.generic.name			= "No Falling Damage:";
	dmflagsOptions_s.noFalling.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noFalling.generic.callback		= DMflagsOptions_Event;
	dmflagsOptions_s.noFalling.generic.id			= ID_NO_FALLING;
	dmflagsOptions_s.noFalling.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.noFalling.generic.y			= 240 - 2 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.noFalling.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.fixedFOV.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.fixedFOV.generic.name		= "Fixed FOV:";
	dmflagsOptions_s.fixedFOV.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.fixedFOV.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.fixedFOV.generic.id		= ID_FIXED_FOV;
	dmflagsOptions_s.fixedFOV.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.fixedFOV.generic.y			= 240 - 1 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.fixedFOV.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.noFootsteps.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noFootsteps.generic.name		= "No Footsteps:";
	dmflagsOptions_s.noFootsteps.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noFootsteps.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noFootsteps.generic.id			= ID_NO_FOOTSTEPS;
	dmflagsOptions_s.noFootsteps.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.noFootsteps.generic.y			= 240 - 0 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.noFootsteps.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.instantWeaponChange.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.instantWeaponChange.generic.name		= "Instant Weapon Change:";
	dmflagsOptions_s.instantWeaponChange.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.instantWeaponChange.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.instantWeaponChange.generic.id			= ID_INSTANT_WEAPON_CHANGE;
	dmflagsOptions_s.instantWeaponChange.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.instantWeaponChange.generic.y			= 240 + 1 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.instantWeaponChange.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.noBunny.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noBunny.generic.name		= "No Strafe Jumping:";
	dmflagsOptions_s.noBunny.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noBunny.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noBunny.generic.id			= ID_NO_BUNNY;
	dmflagsOptions_s.noBunny.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.noBunny.generic.y			= 240 + 2 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.noBunny.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.invis.generic.type			= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.invis.generic.name			= "Total Invisibility:";
	dmflagsOptions_s.invis.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.invis.generic.callback		= DMflagsOptions_Event;
	dmflagsOptions_s.invis.generic.id			= ID_INVIS;
	dmflagsOptions_s.invis.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.invis.generic.y			= 240 + 3 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.invis.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.lightVoting.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.lightVoting.generic.name		= "Light Voting:";
	dmflagsOptions_s.lightVoting.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.lightVoting.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.lightVoting.generic.id			= ID_LIGHT_VOTING;
	dmflagsOptions_s.lightVoting.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.lightVoting.generic.y			= 240 + 4 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.lightVoting.generic.statusbar	= DMFlags_StatusBar;

	dmflagsOptions_s.noSelfDamage.generic.type		= MTYPE_RADIOBUTTON;
	dmflagsOptions_s.noSelfDamage.generic.name		= "No Self Damage from Weapons:";
	dmflagsOptions_s.noSelfDamage.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	dmflagsOptions_s.noSelfDamage.generic.callback	= DMflagsOptions_Event;
	dmflagsOptions_s.noSelfDamage.generic.id		= ID_NO_SELF_DAMAGE;
	dmflagsOptions_s.noSelfDamage.generic.x			= DMFLAGS_X_POS;
	dmflagsOptions_s.noSelfDamage.generic.y			= 240 + 5 * (BIGCHAR_HEIGHT+2);
	dmflagsOptions_s.noSelfDamage.generic.statusbar	= DMFlags_StatusBar;

	if( dmflagsOptions_s.gametype == GT_HARVESTER) {
		dmflagsOptions_s.harvesterFromBodies.generic.type		= MTYPE_RADIOBUTTON;
		dmflagsOptions_s.harvesterFromBodies.generic.name		= "Skulls From Bodies:";
		dmflagsOptions_s.harvesterFromBodies.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		dmflagsOptions_s.harvesterFromBodies.generic.callback	= DMflagsOptions_Event;
		dmflagsOptions_s.harvesterFromBodies.generic.id			= ID_HARVESTER_FROM_BODIES;
		dmflagsOptions_s.harvesterFromBodies.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.harvesterFromBodies.generic.y			= 240 + 6 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.harvesterFromBodies.generic.statusbar	= DMFlags_StatusBar;
	}
	if( dmflagsOptions_s.gametype == GT_OBELISK ) {
		dmflagsOptions_s.overloadObeliskHealth.generic.type			= MTYPE_FIELD;
		dmflagsOptions_s.overloadObeliskHealth.generic.name			= "Obelisk Health:";
		dmflagsOptions_s.overloadObeliskHealth.generic.flags		= QMF_SMALLFONT;
		dmflagsOptions_s.overloadObeliskHealth.generic.callback		= DMflagsOptions_Event;
		dmflagsOptions_s.overloadObeliskHealth.generic.id			= ID_OVERLOAD_OBELISK_HEALTH;
		dmflagsOptions_s.overloadObeliskHealth.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.overloadObeliskHealth.generic.y			= 240 + 6 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.overloadObeliskHealth.field.widthInChars	= 6;
		dmflagsOptions_s.overloadObeliskHealth.field.maxchars		= 3;
		dmflagsOptions_s.overloadObeliskHealth.generic.statusbar	= DMFlags_StatusBar;

		dmflagsOptions_s.overloadRespawnDelay.generic.type			= MTYPE_FIELD;
		dmflagsOptions_s.overloadRespawnDelay.generic.name			= "Obelisk Respawn Time:";
		dmflagsOptions_s.overloadRespawnDelay.generic.flags			= QMF_SMALLFONT;
		dmflagsOptions_s.overloadRespawnDelay.generic.callback		= DMflagsOptions_Event;
		dmflagsOptions_s.overloadRespawnDelay.generic.id			= ID_OVERLOAD_RESPAWN_DELAY;
		dmflagsOptions_s.overloadRespawnDelay.generic.x				= DMFLAGS_X_POS;
		dmflagsOptions_s.overloadRespawnDelay.generic.y				= 240 + 7 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.overloadRespawnDelay.field.widthInChars	= 3;
		dmflagsOptions_s.overloadRespawnDelay.field.maxchars		= 3;
		dmflagsOptions_s.overloadRespawnDelay.generic.statusbar		= DMFlags_StatusBar;
	}
	if( dmflagsOptions_s.gametype == GT_CTF_ELIMINATION) {
		dmflagsOptions_s.eCTFOneWay.generic.type		= MTYPE_RADIOBUTTON;
		dmflagsOptions_s.eCTFOneWay.generic.name		= "Attack vs. Defense:";
		dmflagsOptions_s.eCTFOneWay.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		dmflagsOptions_s.eCTFOneWay.generic.callback	= DMflagsOptions_Event;
		dmflagsOptions_s.eCTFOneWay.generic.id			= ID_ECTF_ONEWAY_MODE;
		dmflagsOptions_s.eCTFOneWay.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.eCTFOneWay.generic.y			= 240 + 6 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.eCTFOneWay.generic.statusbar	= DMFlags_StatusBar;
	}
	if( dmflagsOptions_s.gametype == GT_LMS ) {
		dmflagsOptions_s.lmsMode.generic.type		= MTYPE_SPINCONTROL;
		dmflagsOptions_s.lmsMode.generic.name		= "Scoring Mode:";
		dmflagsOptions_s.lmsMode.generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT;
		dmflagsOptions_s.lmsMode.generic.callback	= DMflagsOptions_Event;
		dmflagsOptions_s.lmsMode.generic.id			= ID_LMS_SCORING_RULE;
		dmflagsOptions_s.lmsMode.generic.x			=  DMFLAGS_X_POS;
		dmflagsOptions_s.lmsMode.generic.y			= 240 + 6 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.lmsMode.itemnames			= lmsMode_list;
		dmflagsOptions_s.lmsMode.generic.statusbar	= DMFlags_StatusBar;

		dmflagsOptions_s.lmsLives.generic.type			= MTYPE_FIELD;
		dmflagsOptions_s.lmsLives.generic.name			= "Lives Per Player:";
		dmflagsOptions_s.lmsLives.generic.flags			= QMF_SMALLFONT;
		dmflagsOptions_s.lmsLives.generic.callback		= DMflagsOptions_Event;
		dmflagsOptions_s.lmsLives.generic.id			= ID_LMS_LIVES;
		dmflagsOptions_s.lmsLives.generic.x				= DMFLAGS_X_POS;
		dmflagsOptions_s.lmsLives.generic.y				= 240 + 7 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.lmsLives.field.widthInChars	= 3;
		dmflagsOptions_s.lmsLives.field.maxchars		= 3;
		dmflagsOptions_s.lmsLives.generic.statusbar		= DMFlags_StatusBar;
	}
	if( dmflagsOptions_s.gametype == GT_DOUBLE_D ) {
		dmflagsOptions_s.ddCaptureTime.generic.type			= MTYPE_FIELD;
		dmflagsOptions_s.ddCaptureTime.generic.name			= "Holding Time:";
		dmflagsOptions_s.ddCaptureTime.generic.flags		= QMF_SMALLFONT;
		dmflagsOptions_s.ddCaptureTime.generic.callback		= DMflagsOptions_Event;
		dmflagsOptions_s.ddCaptureTime.generic.id			= ID_DD_CAPTURE_TIME;
		dmflagsOptions_s.ddCaptureTime.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.ddCaptureTime.generic.y			= 240 + 6 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.ddCaptureTime.field.widthInChars	= 3;
		dmflagsOptions_s.ddCaptureTime.field.maxchars		= 3;
		dmflagsOptions_s.ddCaptureTime.generic.statusbar	= DMFlags_StatusBar;

		dmflagsOptions_s.ddRespawnDelay.generic.type		= MTYPE_FIELD;
		dmflagsOptions_s.ddRespawnDelay.generic.name		= "Time Between Rounds:";
		dmflagsOptions_s.ddRespawnDelay.generic.flags		= QMF_SMALLFONT;
		dmflagsOptions_s.ddRespawnDelay.generic.callback	= DMflagsOptions_Event;
		dmflagsOptions_s.ddRespawnDelay.generic.id			= ID_DD_RESPAWN_DELAY;
		dmflagsOptions_s.ddRespawnDelay.generic.x			= DMFLAGS_X_POS;
		dmflagsOptions_s.ddRespawnDelay.generic.y			= 240 + 7 * (BIGCHAR_HEIGHT+2);
		dmflagsOptions_s.ddRespawnDelay.field.widthInChars	= 3;
		dmflagsOptions_s.ddRespawnDelay.field.maxchars		= 3;
		dmflagsOptions_s.ddRespawnDelay.generic.statusbar	= DMFlags_StatusBar;
	}

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

	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.pmove );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.grapple );
	if (UI_IsARoundBasedGametype(dmflagsOptions_s.gametype)) {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.eliminationWeaponMode );
	}
	else {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.weaponMode );
	}
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.weaponArenaWeapon );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.awardPushing );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noFalling );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.fixedFOV );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noFootsteps );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.instantWeaponChange );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noBunny );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.invis );
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.lightVoting );
	if (dmflagsOptions_s.gametype == GT_HARVESTER) {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.harvesterFromBodies );		
	}
	if (dmflagsOptions_s.gametype == GT_OBELISK) {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.overloadObeliskHealth );		
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.overloadRespawnDelay );		
	}
	Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.noSelfDamage );
	if (dmflagsOptions_s.gametype == GT_CTF_ELIMINATION) {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.eCTFOneWay );		
	}
	if (dmflagsOptions_s.gametype == GT_LMS) {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.lmsMode );		
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.lmsLives );		
	}
	if (dmflagsOptions_s.gametype == GT_DOUBLE_D) {
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.ddCaptureTime );		
		Menu_AddItem( &dmflagsOptions_s.menu, &dmflagsOptions_s.ddRespawnDelay );		
	}

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

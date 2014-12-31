/*
===========================================================================
Copyright (C) 2008-2009 Poul Sander

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
#include "../game/challenges.h"

//This is an ugly way of syncing to cgame but it is platform compatible
#define PARTofUI 1
#include "../cgame/cg_challenges.c"

#define ID_BACK                         1

//Main menu:
#define ID_GENERAL			100
#define ID_GAMETYPES                    101
#define ID_WEAPONS                      102
#define ID_AWARDS                       103
#define ID_POWERUPS                     104
#define ID_FFA                          105

#define ART_BACK0		"menu/" MENU_ART_DIR "/back_0"
#define ART_BACK1		"menu/" MENU_ART_DIR "/back_1"
#define ART_FRAMEL		"menu/" MENU_ART_DIR "/frame2_l"
#define ART_FRAMER		"menu/" MENU_ART_DIR "/frame1_r"

#define MAX_ENTRIES                     18
#define MAX_INT_AS_STRING               8

typedef struct {
	menuframework_s	menu;

	menutext_s	banner;
	menubitmap_s	framel;
	menubitmap_s	framer;

	menutext_s headlines[5];

        int             numberOfEntries;
        menutext_s	entry[MAX_ENTRIES];
        menutext_s	entryIntText[MAX_ENTRIES];
        char            entryIntString[MAX_ENTRIES][MAX_INT_AS_STRING];
        int             entryInt[MAX_ENTRIES];

        menutext_s      notice;
        menutext_s      notice2;
	menubitmap_s	back;
} challenges_t;

static challenges_t challenges;

static int             mainSelection;

//This should only be accessed locally
void UI_ChallengesLocal( void );

/*
=================
UI_Challenges_Event
=================
*/
static void UI_Challenges_Event( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_GENERAL:
            mainSelection = 0;
            UI_PopMenu();
            UI_ChallengesLocal();
		break;

	case ID_GAMETYPES:
            mainSelection = 1;
            UI_PopMenu();
            UI_ChallengesLocal();
		break;

	case ID_WEAPONS:
            mainSelection = 2;
            UI_PopMenu();
            UI_ChallengesLocal();
		break;

	case ID_AWARDS:
            mainSelection = 3;
            UI_PopMenu();
            UI_ChallengesLocal();
		break;

        case ID_POWERUPS:
            mainSelection = 4;
            UI_PopMenu();
            UI_ChallengesLocal();
		break;

        case ID_FFA:
		break;

	case ID_BACK:
		UI_PopMenu();
		break;
	}
}

/*
===============
UI_Challenges_Cache
===============
*/
void UI_Challenges_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
}

/*
===============
UI_Challenges_Init
===============
*/
static void UI_Challenges_Init( void ) {
    int y,i;

	memset( &challenges, 0, sizeof(challenges) );

	UI_DisplayOptionsMenu_Cache();
	challenges.menu.wrapAround = qtrue;
	challenges.menu.fullscreen = qtrue;

	challenges.banner.generic.type		= MTYPE_BTEXT;
	challenges.banner.generic.flags		= QMF_CENTER_JUSTIFY;
	challenges.banner.generic.x			= 320;
	challenges.banner.generic.y			= 16;
	challenges.banner.string			= "STATISTICS";
	challenges.banner.color				= color_white;
	challenges.banner.style				= UI_CENTER;

	challenges.framel.generic.type		= MTYPE_BITMAP;
	challenges.framel.generic.name		= ART_FRAMEL;
	challenges.framel.generic.flags		= QMF_INACTIVE;
	challenges.framel.generic.x			= 0;
	challenges.framel.generic.y			= 78;
	challenges.framel.width				= 256;
	challenges.framel.height			= 329;

	challenges.framer.generic.type		= MTYPE_BITMAP;
	challenges.framer.generic.name		= ART_FRAMER;
	challenges.framer.generic.flags		= QMF_INACTIVE;
	challenges.framer.generic.x			= 376;
	challenges.framer.generic.y			= 76;
	challenges.framer.width				= 256;
	challenges.framer.height			= 334;

        challenges.headlines[0].generic.type		= MTYPE_PTEXT;
	challenges.headlines[0].generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.headlines[0].generic.id			= ID_GENERAL;
	challenges.headlines[0].generic.callback	= UI_Challenges_Event;
	challenges.headlines[0].generic.x			= 216;
	challenges.headlines[0].generic.y			= 240 - 2 * PROP_HEIGHT;
	challenges.headlines[0].string				= "GENERAL";
	challenges.headlines[0].style				= UI_RIGHT;
	challenges.headlines[0].color				= color_red;

	challenges.headlines[1].generic.type			= MTYPE_PTEXT;
	challenges.headlines[1].generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.headlines[1].generic.id			= ID_GAMETYPES;
	challenges.headlines[1].generic.callback		= UI_Challenges_Event;
	challenges.headlines[1].generic.x			= 216;
	challenges.headlines[1].generic.y			= 240 - PROP_HEIGHT;
	challenges.headlines[1].string				= "GAMETYPES";
	challenges.headlines[1].style				= UI_RIGHT;
	challenges.headlines[1].color				= color_red;

	challenges.headlines[2].generic.type			= MTYPE_PTEXT;
	challenges.headlines[2].generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.headlines[2].generic.id				= ID_WEAPONS;
	challenges.headlines[2].generic.callback		= UI_Challenges_Event;
	challenges.headlines[2].generic.x				= 216;
	challenges.headlines[2].generic.y				= 240;
	challenges.headlines[2].string					= "WEAPONS";
	challenges.headlines[2].style					= UI_RIGHT;
	challenges.headlines[2].color					= color_red;

	challenges.headlines[3].generic.type			= MTYPE_PTEXT;
	challenges.headlines[3].generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.headlines[3].generic.id			= ID_AWARDS;
	challenges.headlines[3].generic.callback		= UI_Challenges_Event;
	challenges.headlines[3].generic.x			= 216;
	challenges.headlines[3].generic.y			= 240 + PROP_HEIGHT;
	challenges.headlines[3].string				= "AWARDS";
	challenges.headlines[3].style				= UI_RIGHT;
	challenges.headlines[3].color				= color_red;

        challenges.headlines[4].generic.type			= MTYPE_PTEXT;
	challenges.headlines[4].generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.headlines[4].generic.id			= ID_POWERUPS;
	challenges.headlines[4].generic.callback		= UI_Challenges_Event;
	challenges.headlines[4].generic.x			= 216;
	challenges.headlines[4].generic.y			= 240 + PROP_HEIGHT*2;
	challenges.headlines[4].string				= "POWERUPS";
	challenges.headlines[4].style				= UI_RIGHT;
	challenges.headlines[4].color				= color_red;

        challenges.notice.generic.type          = MTYPE_TEXT;
        challenges.notice.generic.flags	= QMF_CENTER_JUSTIFY|QMF_INACTIVE|QMF_SMALLFONT;
        challenges.notice.generic.x     = 160;
        challenges.notice.generic.y     = 430;
        challenges.notice.string        = "Only results against";

        challenges.notice2.generic.type          = MTYPE_TEXT;
        challenges.notice2.generic.flags	= QMF_CENTER_JUSTIFY|QMF_INACTIVE|QMF_SMALLFONT;
        challenges.notice2.generic.x     = 160;
        challenges.notice2.generic.y     = 430+PROP_HEIGHT-10;
        challenges.notice2.string        = "humans are counted";

        challenges.back.generic.type		= MTYPE_BITMAP;
	challenges.back.generic.name		= ART_BACK0;
	challenges.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.back.generic.callback	= UI_Challenges_Event;
	challenges.back.generic.id			= ID_BACK;
	challenges.back.generic.x			= 0;
	challenges.back.generic.y			= 480-64;
	challenges.back.width				= 128;
	challenges.back.height				= 64;
	challenges.back.focuspic			= ART_BACK1;

	challenges.headlines[mainSelection].generic.flags     = QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
	
        switch( mainSelection ) {

case 0: 
//Overall statistics
challenges.entry[0].string = "Kills:";
challenges.entryInt[0] = getChallenge(GENERAL_TOTALKILLS);
challenges.entry[1].string = "Deaths:";
challenges.entryInt[1] = getChallenge(GENERAL_TOTALDEATHS);

challenges.numberOfEntries = 2;
break;

case 1: 
//Gametype wins
challenges.entry[0].string = "Deathmatch:";
challenges.entryInt[0] = getChallenge(GAMETYPES_FFA_WINS);
challenges.entry[1].string = "Tournament:";
challenges.entryInt[1] = getChallenge(GAMETYPES_TOURNEY_WINS);
challenges.entry[2].string = "Team Deathmatch:";
challenges.entryInt[2] = getChallenge(GAMETYPES_TDM_WINS);
challenges.entry[3].string = "Capture the flag:";
challenges.entryInt[3] = getChallenge(GAMETYPES_CTF_WINS);
challenges.entry[4].string = "One flag capture:";
challenges.entryInt[4] = getChallenge(GAMETYPES_1FCTF_WINS);
challenges.entry[5].string = "Obelisk:";
challenges.entryInt[5] = getChallenge(GAMETYPES_OVERLOAD_WINS);
challenges.entry[6].string = "Harvester:";
challenges.entryInt[6] = getChallenge(GAMETYPES_HARVESTER_WINS);
challenges.entry[7].string = "Elimination:";
challenges.entryInt[7] = getChallenge(GAMETYPES_ELIMINATION_WINS);
challenges.entry[8].string = "CTF Elimination:";
challenges.entryInt[8] = getChallenge(GAMETYPES_CTF_ELIMINATION_WINS);
challenges.entry[9].string = "Last man standing:";
challenges.entryInt[9] = getChallenge(GAMETYPES_LMS_WINS);
challenges.entry[10].string = "Double Domination:";
challenges.entryInt[10] = getChallenge(GAMETYPES_DD_WINS);
challenges.entry[11].string = "Domination:";
challenges.entryInt[11] = getChallenge(GAMETYPES_DOM_WINS);

challenges.numberOfEntries = 12;
break;

case 2: 
//Weapon kills
challenges.entry[0].string = "Gauntlet:";
challenges.entryInt[0] = getChallenge(WEAPON_GAUNTLET_KILLS);
challenges.entry[1].string = "Machinegun:";
challenges.entryInt[1] = getChallenge(WEAPON_MACHINEGUN_KILLS);
challenges.entry[2].string = "Shotgun:";
challenges.entryInt[2] = getChallenge(WEAPON_SHOTGUN_KILLS);
challenges.entry[3].string = "Granade launcher:";
challenges.entryInt[3] = getChallenge(WEAPON_GRANADE_KILLS);
challenges.entry[4].string = "Rocket launcher:";
challenges.entryInt[4] = getChallenge(WEAPON_ROCKET_KILLS);
challenges.entry[5].string = "Lightning gun:";
challenges.entryInt[5] = getChallenge(WEAPON_LIGHTNING_KILLS);
challenges.entry[6].string = "Plasma gun:";
challenges.entryInt[6] = getChallenge(WEAPON_PLASMA_KILLS);
challenges.entry[7].string = "Railgun:";
challenges.entryInt[7] = getChallenge(WEAPON_RAIL_KILLS);
challenges.entry[8].string = "BFG:";
challenges.entryInt[8] = getChallenge(WEAPON_BFG_KILLS);
challenges.entry[9].string = "Grapple:";
challenges.entryInt[9] = getChallenge(WEAPON_GRAPPLE_KILLS);
challenges.entry[10].string = "Chaingun:";
challenges.entryInt[10] = getChallenge(WEAPON_CHAINGUN_KILLS);
challenges.entry[11].string = "Nailgun:";
challenges.entryInt[11] = getChallenge(WEAPON_NAILGUN_KILLS);
challenges.entry[12].string = "Mines:";
challenges.entryInt[12] = getChallenge(WEAPON_MINE_KILLS);
challenges.entry[13].string = "Push kills:";
challenges.entryInt[13] = getChallenge(WEAPON_PUSH_KILLS);
challenges.entry[14].string = "Instant rail:";
challenges.entryInt[14] = getChallenge(WEAPON_INSTANT_RAIL_KILLS);
challenges.entry[15].string = "Telefrags:";
challenges.entryInt[15] = getChallenge(WEAPON_TELEFRAG_KILLS);
challenges.entry[16].string = "Crushing:";
challenges.entryInt[16] = getChallenge(WEAPON_CRUSH_KILLS);

challenges.numberOfEntries = 17;
break;

case 3: 
//Awards
challenges.entry[0].string = "Gauntlet:";
challenges.entryInt[0] = getChallenge(WEAPON_GAUNTLET_KILLS);
challenges.entry[1].string = "Impressive:";
challenges.entryInt[1] = getChallenge(AWARD_IMPRESSIVE);
challenges.entry[2].string = "Excelent:";
challenges.entryInt[2] = getChallenge(AWARD_EXCELLENT);
challenges.entry[3].string = "Capture:";
challenges.entryInt[3] = getChallenge(AWARD_CAPTURE);
challenges.entry[4].string = "Assist:";
challenges.entryInt[4] = getChallenge(AWARD_ASSIST);
challenges.entry[5].string = "Defence:";
challenges.entryInt[5] = getChallenge(AWARD_DEFENCE);

challenges.numberOfEntries = 6;
break;

case 4: 
//Power-ups
challenges.entry[0].string = "Quad kills:";
challenges.entryInt[0] = getChallenge(POWERUP_QUAD_KILL);
challenges.entry[1].string = "2 fast 4 U:";
challenges.entryInt[1] = getChallenge(POWERUP_SPEED_KILL);
challenges.entry[2].string = "I'm flying:";
challenges.entryInt[2] = getChallenge(POWERUP_FLIGHT_KILL);
challenges.entry[3].string = "They didn't see me:";
challenges.entryInt[3] = getChallenge(POWERUP_INVIS_KILL);
challenges.entry[4].string = "Killing machine:";
challenges.entryInt[4] = getChallenge(POWERUP_MULTI_KILL);
challenges.entry[5].string = "Counter Quad:";
challenges.entryInt[5] = getChallenge(POWERUP_COUNTER_QUAD);
challenges.entry[6].string = "Not fast enough:";
challenges.entryInt[6] = getChallenge(POWERUP_COUNTER_SPEED);
challenges.entry[7].string = "You cannot hide:";
challenges.entryInt[7] = getChallenge(POWERUP_COUNTER_INVIS);
challenges.entry[8].string = "Fall deep:";
challenges.entryInt[8] = getChallenge(POWERUP_COUNTER_FLIGHT);
challenges.entry[9].string = "Counter battlesuit:";
challenges.entryInt[9] = getChallenge(POWERUP_COUNTER_ENVIR);
challenges.entry[10].string = "Counter regen:";
challenges.entryInt[10] = getChallenge(POWERUP_COUNTER_REGEN);
challenges.entry[11].string = "Counter multi:";
challenges.entryInt[11] = getChallenge(POWERUP_COUNTER_MULTI);

challenges.numberOfEntries = 12;
break;

default:
challenges.numberOfEntries = 0;
};

        //Now write the challenges
        y = 240 - (int)((((float)challenges.numberOfEntries)/2.0) * (float)(BIGCHAR_HEIGHT + 2));
        for(i=0;i<MAX_ENTRIES && i < challenges.numberOfEntries;i++)
        {
            challenges.entry[i].color = color_white;
            challenges.entry[i].generic.type = MTYPE_TEXT;
            challenges.entry[i].generic.x        = 230;
            challenges.entry[i].generic.y        = y;

            challenges.entryIntText[i].color = color_white;
            challenges.entryIntText[i].generic.type = MTYPE_TEXT;
            challenges.entryIntText[i].generic.x        = 630;
            challenges.entryIntText[i].generic.y        = y;
            challenges.entryIntText[i].style = UI_RIGHT;
            challenges.entryIntText[i].string           = challenges.entryIntString[i];

            strncpy(challenges.entryIntString[i],va("%u",challenges.entryInt[i]),MAX_INT_AS_STRING);
            y += BIGCHAR_HEIGHT+2;
        }

        Menu_AddItem( &challenges.menu, ( void * ) &challenges.banner );
	Menu_AddItem( &challenges.menu, ( void * ) &challenges.framel );
	Menu_AddItem( &challenges.menu, ( void * ) &challenges.framer );

        Menu_AddItem( &challenges.menu, ( void * ) &challenges.headlines[0] );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.headlines[1] );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.headlines[2] );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.headlines[3] );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.headlines[4] );

        for(i=0;i<MAX_ENTRIES && i<challenges.numberOfEntries;i++)
        {
            Menu_AddItem( &challenges.menu, ( void * ) &challenges.entry[i] );
            Menu_AddItem( &challenges.menu, ( void * ) &challenges.entryIntText[i] );
        }
        Menu_AddItem( &challenges.menu, (void *) &challenges.notice);
        Menu_AddItem( &challenges.menu, (void *) &challenges.notice2);
	Menu_AddItem( &challenges.menu, ( void * ) &challenges.back );
}

/*
===============
UI_Challenges
===============
*/
void UI_Challenges( void ) {
    mainSelection = 0;
	UI_Challenges_Init();
	UI_PushMenu( &challenges.menu );
	Menu_SetCursorToItem( &challenges.menu, &challenges.back );
}

/*
===============
UI_ChallengesLocal
 *This is only used locally. Remembers selection
===============
*/
void UI_ChallengesLocal( void ) {
	UI_Challenges_Init();
	UI_PushMenu( &challenges.menu );
	Menu_SetCursorToItem( &challenges.menu, &challenges.back );
}

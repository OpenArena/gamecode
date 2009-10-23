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

#define ART_BACK0		"menu/art_blueish/back_0"
#define ART_BACK1		"menu/art_blueish/back_1"
#define ART_FRAMEL		"menu/art_blueish/frame2_l"
#define ART_FRAMER		"menu/art_blueish/frame1_r"

#define MAX_ENTRIES                     18
#define MAX_INT_AS_STRING               8

typedef struct {
	menuframework_s	menu;

	menutext_s	banner;
	menubitmap_s	framel;
	menubitmap_s	framer;

	menutext_s	general;
	menutext_s	gametypes;
	menutext_s      weapons;
        menutext_s      awards;
        menutext_s      powerups;

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

        challenges.general.generic.type		= MTYPE_PTEXT;
	challenges.general.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.general.generic.id			= ID_GENERAL;
	challenges.general.generic.callback	= UI_Challenges_Event;
	challenges.general.generic.x			= 216;
	challenges.general.generic.y			= 240 - 1 * PROP_HEIGHT;
	challenges.general.string				= "GENERAL";
	challenges.general.style				= UI_RIGHT;
	challenges.general.color				= color_red;

	/*challenges.gametypes.generic.type			= MTYPE_PTEXT;
	challenges.gametypes.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.gametypes.generic.id			= ID_GAMETYPES;
	challenges.gametypes.generic.callback		= UI_Challenges_Event;
	challenges.gametypes.generic.x			= 216;
	challenges.gametypes.generic.y			= 240 - PROP_HEIGHT;
	challenges.gametypes.string				= "GAMETYPES";
	challenges.gametypes.style				= UI_RIGHT;
	challenges.gametypes.color				= color_red;*/

	challenges.weapons.generic.type			= MTYPE_PTEXT;
	challenges.weapons.generic.flags			= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.weapons.generic.id				= ID_WEAPONS;
	challenges.weapons.generic.callback		= UI_Challenges_Event;
	challenges.weapons.generic.x				= 216;
	challenges.weapons.generic.y				= 240;
	challenges.weapons.string					= "WEAPONS";
	challenges.weapons.style					= UI_RIGHT;
	challenges.weapons.color					= color_red;

	challenges.awards.generic.type			= MTYPE_PTEXT;
	challenges.awards.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.awards.generic.id			= ID_AWARDS;
	challenges.awards.generic.callback		= UI_Challenges_Event;
	challenges.awards.generic.x			= 216;
	challenges.awards.generic.y			= 240 + PROP_HEIGHT;
	challenges.awards.string				= "AWARDS";
	challenges.awards.style				= UI_RIGHT;
	challenges.awards.color				= color_red;

        challenges.powerups.generic.type			= MTYPE_PTEXT;
	challenges.powerups.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	challenges.powerups.generic.id			= ID_POWERUPS;
	challenges.powerups.generic.callback		= UI_Challenges_Event;
	challenges.powerups.generic.x			= 216;
	challenges.powerups.generic.y			= 240 + PROP_HEIGHT*2;
	challenges.powerups.string				= "POWERUPS";
	challenges.powerups.style				= UI_RIGHT;
	challenges.powerups.color				= color_red;

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

        switch(mainSelection)
        {
            case 0:
                //generel
                challenges.entry[0].string          = "Total kills:";
                challenges.entry[1].string          = "Total deaths:";
                //challenges.entry[2].string          = "Total games:";
                challenges.numberOfEntries = 2;
                challenges.entryInt[0]              = getChallenge(GENERAL_TOTALKILLS);
                challenges.entryInt[1]              = getChallenge(GENERAL_TOTALDEATHS);
                //challenges.entryInt[2]              = getChallenge(GENERAL_TOTALGAMES);
                challenges.general.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
                break;
            case 1:
                //gametypes
                challenges.numberOfEntries = 0;
                challenges.gametypes.generic.flags     = QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
                break;
            case 2:
                //weapons
                challenges.entry[0].string          = "Gauntlet kills:";
                challenges.entry[1].string          = "Machinegun kills:";
                challenges.entry[2].string          = "Shotgun kills:";
                challenges.entry[3].string          = "Granade kills:";
                challenges.entry[4].string          = "Rocket kills:";
                challenges.entry[5].string          = "Lightning kills:";
                challenges.entry[6].string          = "Plasmagun kills:";
                challenges.entry[7].string          = "Railgun kills:";
                challenges.entry[8].string          = "Instant rail kills:";
                challenges.entry[9].string          = "BFG kills:";
                challenges.entry[10].string          = "Grapple kills:";
                challenges.entry[11].string          = "Chaingun kills:";
                challenges.entry[12].string          = "Nailgun kills:";
                challenges.entry[13].string          = "Proxy mine kills:";
                challenges.entry[14].string          = "Telefrags:";
                challenges.entry[15].string          = "Push kills:";
                challenges.entry[16].string          = "Crush kills:";
                challenges.numberOfEntries = 17;
                challenges.entryInt[0]              = getChallenge(WEAPON_GAUNTLET_KILLS);
                challenges.entryInt[1]              = getChallenge(WEAPON_MACHINEGUN_KILLS);
                challenges.entryInt[2]              = getChallenge(WEAPON_SHOTGUN_KILLS);
                challenges.entryInt[3]              = getChallenge(WEAPON_GRANADE_KILLS);
                challenges.entryInt[4]              = getChallenge(WEAPON_ROCKET_KILLS);
                challenges.entryInt[5]              = getChallenge(WEAPON_LIGHTNING_KILLS);
                challenges.entryInt[6]              = getChallenge(WEAPON_PLASMA_KILLS);
                challenges.entryInt[7]              = getChallenge(WEAPON_RAIL_KILLS);
                challenges.entryInt[8]              = getChallenge(WEAPON_INSTANT_RAIL_KILLS);
                challenges.entryInt[9]              = getChallenge(WEAPON_BFG_KILLS);
                challenges.entryInt[10]              = getChallenge(WEAPON_GRAPPLE_KILLS);
                challenges.entryInt[11]              = getChallenge(WEAPON_CHAINGUN_KILLS);
                challenges.entryInt[12]              = getChallenge(WEAPON_NAILGUN_KILLS);
                challenges.entryInt[13]              = getChallenge(WEAPON_MINE_KILLS);
                challenges.entryInt[14]              = getChallenge(WEAPON_TELEFRAG_KILLS);
                challenges.entryInt[15]              = getChallenge(WEAPON_PUSH_KILLS);
                challenges.entryInt[16]              = getChallenge(WEAPON_CRUSH_KILLS);
                challenges.weapons.generic.flags     = QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
                break;
            case 3:
                //awards
                challenges.entry[0].string          = "Gauntlet";
                challenges.entry[1].string          = "Impressive";
                challenges.entry[2].string          = "Excellent";
                challenges.entry[3].string          = "Capture";
                challenges.entry[4].string          = "Assist";
                challenges.entry[5].string          = "Defend";
                challenges.numberOfEntries = 6;
                challenges.entryInt[0]              = getChallenge(WEAPON_GAUNTLET_KILLS);
                challenges.entryInt[1]              = getChallenge(AWARD_IMPRESSIVE);
                challenges.entryInt[2]              = getChallenge(AWARD_EXCELLENT);
                challenges.entryInt[3]              = getChallenge(AWARD_CAPTURE);
                challenges.entryInt[4]              = getChallenge(AWARD_ASSIST);
                challenges.entryInt[5]              = getChallenge(AWARD_DEFENCE);
                challenges.awards.generic.flags     = QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
                break;
            case 4:
                challenges.entry[0].string          = "Quad kills";
                challenges.entry[1].string          = "2 fast 4 U";
                challenges.entry[2].string          = "They didn't see me";
                challenges.entry[3].string          = "I'm flying";
                challenges.entry[4].string          = "Killing machine";
                challenges.entry[5].string          = "Counter Quad";
                challenges.entry[6].string          = "Not fast enough";
                challenges.entry[7].string          = "You cannot hide";
                challenges.entry[8].string          = "Fall deep";
                challenges.entry[9].string          = "Counter battlesuit";
                challenges.entry[10].string          = "Counter regen";
                challenges.entry[11].string          = "Counter multi";
                challenges.numberOfEntries  = 12;

                challenges.entryInt[0]              = getChallenge(POWERUP_QUAD_KILL);
                challenges.entryInt[1]              = getChallenge(POWERUP_SPEED_KILL);
                challenges.entryInt[2]              = getChallenge(POWERUP_INVIS_KILL);
                challenges.entryInt[3]              = getChallenge(POWERUP_FLIGHT_KILL);
                challenges.entryInt[4]              = getChallenge(POWERUP_MULTI_KILL);
                challenges.entryInt[5]              = getChallenge(POWERUP_COUNTER_QUAD);
                challenges.entryInt[6]              = getChallenge(POWERUP_COUNTER_SPEED);
                challenges.entryInt[7]              = getChallenge(POWERUP_COUNTER_INVIS);
                challenges.entryInt[8]              = getChallenge(POWERUP_COUNTER_FLIGHT);
                challenges.entryInt[9]              = getChallenge(POWERUP_COUNTER_ENVIR);
                challenges.entryInt[10]              = getChallenge(POWERUP_COUNTER_REGEN);
                challenges.entryInt[11]              = getChallenge(POWERUP_COUNTER_MULTI);
                challenges.powerups.generic.flags     = QMF_RIGHT_JUSTIFY|QMF_INACTIVE;
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

        Menu_AddItem( &challenges.menu, ( void * ) &challenges.general );
        //Menu_AddItem( &challenges.menu, ( void * ) &challenges.gametypes );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.weapons );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.awards );
        Menu_AddItem( &challenges.menu, ( void * ) &challenges.powerups );

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

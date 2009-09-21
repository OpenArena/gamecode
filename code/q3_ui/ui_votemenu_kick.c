/*
===========================================================================
Copyright (C) 2009 Poul Sander

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


#define ART_BACK0			"menu/art_blueish/back_0"
#define ART_BACK1			"menu/art_blueish/back_1"
#define ART_FIGHT0			"menu/art_blueish/accept_0"
#define ART_FIGHT1			"menu/art_blueish/accept_1"
#define ART_BACKGROUND                  "menu/art_blueish/addbotframe"
#define ART_ARROWS			"menu/art_blueish/arrows_vert_0"
#define ART_ARROWUP			"menu/art_blueish/arrows_vert_top"
#define ART_ARROWDOWN                   "menu/art_blueish/arrows_vert_bot"

#define ID_BACK				10
#define ID_GO				11
#define ID_LIST				12
#define ID_UP				13
#define ID_DOWN				14
#define ID_PLAYERNAME0			20
#define ID_PLAYERNAME1			21
#define ID_PLAYERNAME2			22
#define ID_PLAYERNAME3			23
#define ID_PLAYERNAME4			24
#define ID_PLAYERNAME5			25
#define ID_PLAYERNAME6			26
#define ID_PLAYERNAME7			27
#define ID_PLAYERNAME8			28
#define ID_PLAYERNAME9			29
#define SIZE_OF_LIST                    10
//#define ID_PLAYERNAME10			30

#define SIZE_OF_NAME                    32

#define VOTEKICK_MENU_VERTICAL_SPACING	20

typedef struct {
    int id;
    char name[SIZE_OF_NAME];
} player;

typedef struct {
	menuframework_s	menu;
	menubitmap_s	arrows;
        menutext_s		banner;
	menubitmap_s	up;
	menubitmap_s	down;
	menutext_s	players[SIZE_OF_LIST];
	menubitmap_s	go;
	menubitmap_s	back;

	int		numPlayers;
	int		selected;
        int             startIndex;
        player          players_profiles[MAX_CLIENTS];
} votemenu_kick_t;

static votemenu_kick_t	s_votemenu_kick;

void UI_VoteKickMenuInternal( void );

static void populatePlayerList( void ) {
    int i;
    char playerinfo[MAX_INFO_STRING];
    s_votemenu_kick.numPlayers = 0;

    for(i=0;i<MAX_CLIENTS;i++) {
        trap_GetConfigString( CS_PLAYERS + i, playerinfo, MAX_INFO_STRING );
        if(strlen(playerinfo) ) {
            s_votemenu_kick.players_profiles[s_votemenu_kick.numPlayers].id = i;
            Q_strncpyz( s_votemenu_kick.players_profiles[s_votemenu_kick.numPlayers].name, Info_ValueForKey( playerinfo, "n" ), SIZE_OF_NAME - 2);
            s_votemenu_kick.numPlayers++;
        }
    }
}

/*
=================
VoteKickMenu_Event
=================
*/
static void VoteKickMenu_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
            case ID_BACK:
		if (event != QM_ACTIVATED)
                    return;
                UI_PopMenu();
                break;
            case ID_GO:
                if( event != QM_ACTIVATED ) {
                    return;
                }
                if(!s_votemenu_kick.selected)
                    return;
                trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote clientkick %d",s_votemenu_kick.players_profiles[(s_votemenu_kick.selected-20)+s_votemenu_kick.startIndex].id) );
                UI_PopMenu();
                UI_PopMenu();
                break;
            default:
                if( event != QM_ACTIVATED ) {
                    return;
                }
                if(s_votemenu_kick.selected != ((menucommon_s*)ptr)->id) {
                    s_votemenu_kick.selected = ((menucommon_s*)ptr)->id;
                    UI_VoteKickMenuInternal();
                }
                break;
         }
}

/*
=================
UI_VoteKickMenu_UpEvent
=================
*/
static void UI_VoteKickMenu_UpEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( s_votemenu_kick.startIndex > 0 ) {
            s_votemenu_kick.startIndex--;
	}

        UI_VoteKickMenuInternal();
}

/*
=================
UI_VoteKickMenu_DownEvent
=================
*/
static void UI_VoteKickMenu_DownEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

	if( s_votemenu_kick.startIndex+SIZE_OF_LIST < s_votemenu_kick.numPlayers ) {
            s_votemenu_kick.startIndex++;
	}

        UI_VoteKickMenuInternal();
}

/*
=================
UI_VoteKickMenu_Draw
=================
*/
static void UI_VoteKickMenu_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE KICK", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu_kick.menu );
}

/*
=================
VoteKickMenu_Cache
=================
*/
static void VoteKickMenu_Cache( void )
{
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
	trap_R_RegisterShaderNoMip( ART_FIGHT0 );
	trap_R_RegisterShaderNoMip( ART_FIGHT1 );
	trap_R_RegisterShaderNoMip( ART_BACKGROUND );
	trap_R_RegisterShaderNoMip( ART_ARROWS );
	trap_R_RegisterShaderNoMip( ART_ARROWUP );
	trap_R_RegisterShaderNoMip( ART_ARROWDOWN );
}

static void setKickMenutext(menutext_s *menu,int y,int id,qboolean active,char *text) {
    menu->generic.type            = MTYPE_PTEXT;
    menu->color               = color_red;
    menu->generic.flags       = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
    if(!active)
        menu->generic.flags   |= QMF_INACTIVE|QMF_GRAYED;
    else
    if(s_votemenu_kick.selected == id)
         menu->color       = color_orange;
    menu->generic.x           = 320;
    menu->generic.y           = y;
    menu->generic.id          = id;
    menu->generic.callback    = VoteKickMenu_Event;
    menu->string              = text;
    menu->style               = UI_CENTER|UI_SMALLFONT;
}

/*
=================
UI_VoteKickMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteKickMenuInternal( void )
{
    int y;

    s_votemenu_kick.menu.wrapAround = qtrue;
    s_votemenu_kick.menu.fullscreen = qfalse;
    s_votemenu_kick.menu.draw = UI_VoteKickMenu_Draw;

    s_votemenu_kick.banner.generic.type  = MTYPE_BTEXT;
    s_votemenu_kick.banner.generic.x	  = 320;
    s_votemenu_kick.banner.generic.y	  = 16;
    s_votemenu_kick.banner.string		  = "CALL VOTE KICK";
    s_votemenu_kick.banner.color	      = color_white;
    s_votemenu_kick.banner.style	      = UI_CENTER;

    s_votemenu_kick.arrows.generic.type  = MTYPE_BITMAP;
    s_votemenu_kick.arrows.generic.name  = ART_ARROWS;
    s_votemenu_kick.arrows.generic.flags = QMF_INACTIVE;
    s_votemenu_kick.arrows.generic.x	 = 200;
    s_votemenu_kick.arrows.generic.y	 = 128;
    s_votemenu_kick.arrows.width  	     = 64;
    s_votemenu_kick.arrows.height  	     = 128;

    y = 98;
    setKickMenutext(&s_votemenu_kick.players[0],y,ID_PLAYERNAME0,s_votemenu_kick.startIndex<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[1],y,ID_PLAYERNAME1,s_votemenu_kick.startIndex+1<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+1].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[2],y,ID_PLAYERNAME2,s_votemenu_kick.startIndex+2<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+2].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[3],y,ID_PLAYERNAME3,s_votemenu_kick.startIndex+3<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+3].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[4],y,ID_PLAYERNAME4,s_votemenu_kick.startIndex+4<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+4].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[5],y,ID_PLAYERNAME5,s_votemenu_kick.startIndex+5<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+5].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[6],y,ID_PLAYERNAME6,s_votemenu_kick.startIndex+6<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+6].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[7],y,ID_PLAYERNAME7,s_votemenu_kick.startIndex+7<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+7].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[8],y,ID_PLAYERNAME8,s_votemenu_kick.startIndex+8<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+8].name);
    y+=VOTEKICK_MENU_VERTICAL_SPACING;
    setKickMenutext(&s_votemenu_kick.players[9],y,ID_PLAYERNAME9,s_votemenu_kick.startIndex+9<s_votemenu_kick.numPlayers,s_votemenu_kick.players_profiles[s_votemenu_kick.startIndex+9].name);

    s_votemenu_kick.up.generic.type	    = MTYPE_BITMAP;
    s_votemenu_kick.up.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_kick.up.generic.x		= 200;
    s_votemenu_kick.up.generic.y		= 128;
    s_votemenu_kick.up.generic.id	    = ID_UP;
    s_votemenu_kick.up.generic.callback = UI_VoteKickMenu_UpEvent;
    s_votemenu_kick.up.width  		    = 64;
    s_votemenu_kick.up.height  		    = 64;
    s_votemenu_kick.up.focuspic         = ART_ARROWUP;

    s_votemenu_kick.down.generic.type	  = MTYPE_BITMAP;
    s_votemenu_kick.down.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_kick.down.generic.x		  = 200;
    s_votemenu_kick.down.generic.y		  = 128+64;
    s_votemenu_kick.down.generic.id	      = ID_DOWN;
    s_votemenu_kick.down.generic.callback = UI_VoteKickMenu_DownEvent;
    s_votemenu_kick.down.width  		  = 64;
    s_votemenu_kick.down.height  		  = 64;
    s_votemenu_kick.down.focuspic         = ART_ARROWDOWN;

    s_votemenu_kick.go.generic.type			= MTYPE_BITMAP;
    s_votemenu_kick.go.generic.name			= ART_FIGHT0;
    s_votemenu_kick.go.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_kick.go.generic.id			= ID_GO;
    s_votemenu_kick.go.generic.callback		= VoteKickMenu_Event;
    s_votemenu_kick.go.generic.x			= 320+128-128;
    s_votemenu_kick.go.generic.y			= 256+128-64;
    s_votemenu_kick.go.width  				= 128;
    s_votemenu_kick.go.height  				= 64;
    s_votemenu_kick.go.focuspic				= ART_FIGHT1;

    s_votemenu_kick.back.generic.type		= MTYPE_BITMAP;
    s_votemenu_kick.back.generic.name		= ART_BACK0;
    s_votemenu_kick.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_kick.back.generic.id			= ID_BACK;
    s_votemenu_kick.back.generic.callback	= VoteKickMenu_Event;
    s_votemenu_kick.back.generic.x			= 320-128;
    s_votemenu_kick.back.generic.y			= 256+128-64;
    s_votemenu_kick.back.width				= 128;
    s_votemenu_kick.back.height				= 64;
    s_votemenu_kick.back.focuspic			= ART_BACK1;
}

/*
=================
UI_VoteKickMenu
 *Called from outside
=================
*/
void UI_VoteKickMenu( void ) {
    int i;
    VoteKickMenu_Cache();
    memset( &s_votemenu_kick, 0 ,sizeof(votemenu_kick_t) );
    populatePlayerList();

    UI_VoteKickMenuInternal();

    Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.banner );
    Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.back );
    Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.go );
    Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.arrows );
    Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.down );
    Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.up );
    for(i=0;i<SIZE_OF_LIST;i++)
        Menu_AddItem( &s_votemenu_kick.menu, (void*) &s_votemenu_kick.players[i] );

    UI_PushMenu( &s_votemenu_kick.menu );
}


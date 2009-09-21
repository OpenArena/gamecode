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

#define VOTEMENU_BACK0          "menu/art_blueish/back_0"
#define VOTEMENU_BACK1          "menu/art_blueish/back_1"
#define ART_FIGHT0		"menu/art_blueish/accept_0"
#define ART_FIGHT1		"menu/art_blueish/accept_1"
#define ART_BACKGROUND          "menu/art_blueish/addbotframe"

static char* votemenu_Gametype_artlist[] =
{
	VOTEMENU_BACK0,
	VOTEMENU_BACK1,
        ART_FIGHT0,
        ART_FIGHT1,
	NULL
};

#define ID_BACK     100
#define ID_GO       101
#define ID_FFA        102
#define ID_Tourney       103
#define ID_TDM       104
#define ID_CTF       105
#define ID_1FCTF       106
#define ID_Overload       107
#define ID_Harvester      108
#define ID_Elimination  109
#define ID_CTFe         110
#define ID_LMS          111
#define ID_DOUBLED      112
#define ID_DOM          113

#define Gametype_MENU_VERTICAL_SPACING	19

typedef struct
{
    menuframework_s	menu;
    menutext_s		banner;
    menubitmap_s	back;
    menubitmap_s	go;

    //Buttons:
    menutext_s      bFFA;
    menutext_s      bTourney;
    menutext_s      bTDM;
    menutext_s      bCTF;
    menutext_s      b1FCTF;
    menutext_s      bOverload;
    menutext_s      bHarvester;
    menutext_s      bElimination;
    menutext_s      bCTFe;
    menutext_s      bLMS;
    menutext_s      bDOUBLED;
    menutext_s      bDOM;

    //Allowed:
    qboolean        FFA;
    qboolean        Tourney;
    qboolean        TDM;
    qboolean        CTF;
    qboolean        _1FCTF;
    qboolean        Overload;
    qboolean        Harvester;
    qboolean        Elimination;
    qboolean        CTFe;
    qboolean        LMS;
    qboolean        DOUBLED;
    qboolean        DOM;
    int selection;
} votemenu_t;

static votemenu_t	s_votemenu_Gametype;

void UI_VoteGametypeMenuInternal( void );

/*
=================
VoteMenu_Gametype_Event
=================
*/
static void VoteMenu_Gametype_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_BACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
            case ID_GO:
                if( event != QM_ACTIVATED ) {
                    return;
                }
                switch(s_votemenu_Gametype.selection) {
                    case ID_FFA:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 0" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_Tourney:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 1" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_TDM:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 3" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_CTF:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 4" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_1FCTF:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 5" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_Overload:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 6" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_Harvester:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 7" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_Elimination:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 8" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_CTFe:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 9" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_LMS:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 10" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_DOUBLED:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 11" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                        case ID_DOM:
                        trap_Cmd_ExecuteText( EXEC_APPEND, "callvote g_gametype 12" );
                        UI_PopMenu();
                        UI_PopMenu();
                        break;
                };
                break;
            default:
                if( event != QM_ACTIVATED ) {
                    return;
                }
                if(s_votemenu_Gametype.selection != ((menucommon_s*)ptr)->id) {
                    s_votemenu_Gametype.selection = ((menucommon_s*)ptr)->id;
                    UI_VoteGametypeMenuInternal();
                }
                break;
	}
}

static void setGametypeMenutext(menutext_s *menu,int y,int id,qboolean active,char *text) {
    menu->generic.type            = MTYPE_PTEXT;
    menu->color               = color_red;
    menu->generic.flags       = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
    if(!active)
        menu->generic.flags   |= QMF_INACTIVE|QMF_GRAYED;
    else
    if(s_votemenu_Gametype.selection == id)
         menu->color       = color_orange;
    menu->generic.x           = 320;
    menu->generic.y           = y;
    menu->generic.id          = id;
    menu->generic.callback    = VoteMenu_Gametype_Event;
    menu->string              = text;
    menu->style               = UI_CENTER|UI_SMALLFONT;
}

/*
=================
VoteMenu_Gametype_MenuKey
=================
*/
/*static sfxHandle_t VoteMenu_Gametype_MenuKey( int key )
{
	return ( Menu_DefaultKey( &s_votemenu_Gametype.menu, key ) );
}*/

/*
=================
VoteMenu_Gametype_Cache
=================
*/
static void VoteMenu_Gametype_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!votemenu_Gametype_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(votemenu_Gametype_artlist[i]);
	}
}

/*
=================
UI_VoteMenu_Gametype_Draw
=================
*/
static void UI_VoteMenu_Gametype_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE GAMETYPE", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu_Gametype.menu );
}

/*
=================
UI_VoteGametypeMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteGametypeMenuInternal( void )
{
    int y;

    VoteMenu_Gametype_Cache();

    s_votemenu_Gametype.menu.wrapAround = qtrue;
    s_votemenu_Gametype.menu.fullscreen = qfalse;
    s_votemenu_Gametype.menu.draw = UI_VoteMenu_Gametype_Draw;

    s_votemenu_Gametype.banner.generic.type  = MTYPE_BTEXT;
    s_votemenu_Gametype.banner.generic.x	  = 320;
    s_votemenu_Gametype.banner.generic.y	  = 16;
    s_votemenu_Gametype.banner.string		  = "CALL VOTE GAMETYPE";
    s_votemenu_Gametype.banner.color	      = color_white;
    s_votemenu_Gametype.banner.style	      = UI_CENTER;

    y = 98;
    setGametypeMenutext(&s_votemenu_Gametype.bFFA,y,ID_FFA,s_votemenu_Gametype.FFA,"Free for all");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bTourney,y,ID_Tourney,s_votemenu_Gametype.Tourney,"Tournament");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bTDM,y,ID_TDM,s_votemenu_Gametype.TDM,"Team Deathmatch");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bCTF,y,ID_CTF,s_votemenu_Gametype.CTF,"Capture the flag");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.b1FCTF,y,ID_1FCTF,s_votemenu_Gametype._1FCTF,"One flag capture");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bOverload,y,ID_Overload,s_votemenu_Gametype.Overload,"Overload");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bHarvester,y,ID_Harvester,s_votemenu_Gametype.Harvester,"Harvester");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bElimination,y,ID_Elimination,s_votemenu_Gametype.Elimination,"Elimination");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bCTFe,y,ID_CTFe,s_votemenu_Gametype.CTFe,"CTF Elimination");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bLMS,y,ID_LMS,s_votemenu_Gametype.LMS,"Last man standing");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bDOUBLED,y,ID_DOUBLED,s_votemenu_Gametype.DOUBLED,"Double Domination");
    y+=Gametype_MENU_VERTICAL_SPACING;
    setGametypeMenutext(&s_votemenu_Gametype.bDOM,y,ID_DOM,s_votemenu_Gametype.DOM,"Domination");

    s_votemenu_Gametype.back.generic.type	   = MTYPE_BITMAP;
    s_votemenu_Gametype.back.generic.name     = VOTEMENU_BACK0;
    s_votemenu_Gametype.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_Gametype.back.generic.callback = VoteMenu_Gametype_Event;
    s_votemenu_Gametype.back.generic.id	   = ID_BACK;
    s_votemenu_Gametype.back.generic.x		   = 320-128;
    s_votemenu_Gametype.back.generic.y		   = 256+128-64;
    s_votemenu_Gametype.back.width  		   = 128;
    s_votemenu_Gametype.back.height  		   = 64;
    s_votemenu_Gametype.back.focuspic         = VOTEMENU_BACK1;

    s_votemenu_Gametype.go.generic.type	   = MTYPE_BITMAP;
    s_votemenu_Gametype.go.generic.name     = ART_FIGHT0;
    s_votemenu_Gametype.go.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_Gametype.go.generic.callback = VoteMenu_Gametype_Event;
    s_votemenu_Gametype.go.generic.id	   = ID_GO;
    s_votemenu_Gametype.go.generic.x		   = 320;
    s_votemenu_Gametype.go.generic.y		   = 256+128-64;
    s_votemenu_Gametype.go.width  		   = 128;
    s_votemenu_Gametype.go.height  		   = 64;
    s_votemenu_Gametype.go.focuspic         = ART_FIGHT1;
}

/*
=================
UI_VoteGametypeMenu
 *Called from outside
=================
*/
void UI_VoteGametypeMenu( void ) {
        char serverinfo[MAX_INFO_STRING], *gametypeinfo;
        // zero set all our globals
	memset( &s_votemenu_Gametype, 0 ,sizeof(votemenu_t) );
        trap_GetConfigString( CS_SERVERINFO, serverinfo, MAX_INFO_STRING );
        gametypeinfo = Info_ValueForKey(serverinfo,"g_voteGametypes");
        if(!Q_stricmp(gametypeinfo,"*")) {
            s_votemenu_Gametype.FFA = qtrue;
            s_votemenu_Gametype.Tourney = qtrue;
            s_votemenu_Gametype.TDM = qtrue;
            s_votemenu_Gametype.CTF = qtrue;
            s_votemenu_Gametype._1FCTF = qtrue;
            s_votemenu_Gametype.Overload = qtrue;
            s_votemenu_Gametype.Harvester = qtrue;
            s_votemenu_Gametype.Elimination = qtrue;
            s_votemenu_Gametype.CTFe = qtrue;
            s_votemenu_Gametype.LMS = qtrue;
            s_votemenu_Gametype.DOUBLED = qtrue;
            s_votemenu_Gametype.DOM = qtrue;
        } else {
            s_votemenu_Gametype.FFA = (qboolean)Q_stristr(gametypeinfo,"/0/");
            s_votemenu_Gametype.Tourney = (qboolean)Q_stristr(gametypeinfo,"/1/");
            s_votemenu_Gametype.TDM = (qboolean)Q_stristr(gametypeinfo,"/3/");
            s_votemenu_Gametype.CTF = (qboolean)Q_stristr(gametypeinfo,"/4/");
            s_votemenu_Gametype._1FCTF = (qboolean)Q_stristr(gametypeinfo,"/5/");
            s_votemenu_Gametype.Overload = (qboolean)Q_stristr(gametypeinfo,"/6/");
            s_votemenu_Gametype.Harvester = (qboolean)Q_stristr(gametypeinfo,"/7/");
            s_votemenu_Gametype.Elimination = (qboolean)Q_stristr(gametypeinfo,"/8/");
            s_votemenu_Gametype.CTFe = (qboolean)Q_stristr(gametypeinfo,"/9/");
            s_votemenu_Gametype.LMS = (qboolean)Q_stristr(gametypeinfo,"/10/");
            s_votemenu_Gametype.DOUBLED = (qboolean)Q_stristr(gametypeinfo,"/11/");
            s_votemenu_Gametype.DOM = (qboolean)Q_stristr(gametypeinfo,"/12/");
        }

        UI_VoteGametypeMenuInternal();

	Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.banner );
	Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.back );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.go );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bFFA );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bTourney );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bTDM );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bCTF );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.b1FCTF );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bOverload );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bHarvester );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bElimination );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bCTFe );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bLMS );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bDOUBLED );
        Menu_AddItem( &s_votemenu_Gametype.menu, (void*) &s_votemenu_Gametype.bDOM );

	UI_PushMenu( &s_votemenu_Gametype.menu );
}

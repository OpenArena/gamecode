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
#define ID_MAPNAME0			20
#define ID_MAPNAME1			21
#define ID_MAPNAME2			22
#define ID_MAPNAME3			23
#define ID_MAPNAME4			24
#define ID_MAPNAME5			25
#define ID_MAPNAME6			26
#define ID_MAPNAME7			27
#define ID_MAPNAME8			28
#define ID_MAPNAME9			29
#define SIZE_OF_LIST                    10

#define SIZE_OF_NAME                    32

#define VOTEMAP_MENU_VERTICAL_SPACING	20

typedef struct {
	menuframework_s	menu;
	menubitmap_s	arrows;
        menutext_s		banner;
        menutext_s		info;
	menubitmap_s	up;
	menubitmap_s	down;
	menubitmap_s	go;
	menubitmap_s	back;

	menutext_s	maps[SIZE_OF_LIST];

	int		selected;
} votemenu_map_t;

static votemenu_map_t	s_votemenu_map;

/*
=================
VoteMapMenu_Event
=================
*/
static void VoteMapMenu_Event( void* ptr, int event )
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
                if(!s_votemenu_map.selected || mappage.mapname[s_votemenu_map.selected-ID_MAPNAME0][0] == 0)
                    return;
                if(!Q_stricmp(mappage.mapname[s_votemenu_map.selected-ID_MAPNAME0],"---"))
                    return; //Blank spaces have string "---"
                trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote map %s", mappage.mapname[s_votemenu_map.selected-ID_MAPNAME0]) );
                UI_PopMenu();
                UI_PopMenu();
                break;
            default:
                if( event != QM_ACTIVATED ) {
                    return;
                }
                if(s_votemenu_map.selected != ((menucommon_s*)ptr)->id) {
                    s_votemenu_map.selected = ((menucommon_s*)ptr)->id;
                    UI_VoteMapMenuInternal();
                }
                break;
         }
}

/*
=================
UI_VoteMapMenu_UpEvent
=================
*/
static void UI_VoteMapMenu_UpEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED  || mappage.pagenumber<1) {
		return;
	}

        trap_Cmd_ExecuteText( EXEC_APPEND,va("getmappage %d",mappage.pagenumber-1) );
}

/*
=================
UI_VoteMapMenu_DownEvent
=================
*/
static void UI_VoteMapMenu_DownEvent( void* ptr, int event ) {
	if (event != QM_ACTIVATED) {
		return;
	}

        trap_Cmd_ExecuteText( EXEC_APPEND,va("getmappage %d",mappage.pagenumber+1) );
}

/*
=================
UI_VoteMapMenu_Draw
=================
*/
static void UI_VoteMapMenu_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE MAP", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu_map.menu );
}

/*
=================
VoteMapMenu_Cache
=================
*/
static void VoteMapMenu_Cache( void )
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

static void setMapMenutext(menutext_s *menu,int y,int id,char * text) {
    menu->generic.type            = MTYPE_PTEXT;
    menu->color               = color_red;
    menu->generic.flags       = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    if(s_votemenu_map.selected == id)
         menu->color       = color_orange;
    menu->generic.x           = 320-80;
    menu->generic.y           = y;
    menu->generic.id          = id;
    menu->generic.callback    = VoteMapMenu_Event;
    menu->string              = text;
    menu->style               = UI_LEFT|UI_SMALLFONT;
}

/*
=================
UI_VoteMapMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteMapMenuInternal( void )
{
    int y,i;

    s_votemenu_map.menu.wrapAround = qtrue;
    s_votemenu_map.menu.fullscreen = qfalse;
    s_votemenu_map.menu.draw = UI_VoteMapMenu_Draw;

    s_votemenu_map.banner.generic.type  = MTYPE_BTEXT;
    s_votemenu_map.banner.generic.x	  = 320;
    s_votemenu_map.banner.generic.y	  = 16;
    s_votemenu_map.banner.string		  = "CALL VOTE MAP";
    s_votemenu_map.banner.color	      = color_white;
    s_votemenu_map.banner.style	      = UI_CENTER;

    s_votemenu_map.info.generic.type  = MTYPE_TEXT;
    s_votemenu_map.info.generic.x	  = 320;
    s_votemenu_map.info.generic.y	  = 48;
    s_votemenu_map.info.string		  = va("Page %d",mappage.pagenumber+1);
    s_votemenu_map.info.color	      = color_white;
    s_votemenu_map.info.style	      = UI_CENTER;

    s_votemenu_map.arrows.generic.type  = MTYPE_BITMAP;
    s_votemenu_map.arrows.generic.name  = ART_ARROWS;
    s_votemenu_map.arrows.generic.flags = QMF_INACTIVE;
    s_votemenu_map.arrows.generic.x	 = 200-40;
    s_votemenu_map.arrows.generic.y	 = 128+30;
    s_votemenu_map.arrows.width  	     = 64;
    s_votemenu_map.arrows.height  	     = 128;

    y = 98;
    setMapMenutext(&s_votemenu_map.maps[0],y,ID_MAPNAME0,mappage.mapname[0]);
    for(i=1;i<SIZE_OF_LIST;i++) {
    		y+=VOTEMAP_MENU_VERTICAL_SPACING;
    		setMapMenutext(&s_votemenu_map.maps[i],y,ID_MAPNAME0+i,mappage.mapname[i]);
	}
    y+=VOTEMAP_MENU_VERTICAL_SPACING+8;
    s_votemenu_map.info.generic.y	  = y;

    s_votemenu_map.up.generic.type	    = MTYPE_BITMAP;
    s_votemenu_map.up.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_map.up.generic.x		= 200-40;
    s_votemenu_map.up.generic.y		= 128+30;
    s_votemenu_map.up.generic.id	    = ID_UP;
    s_votemenu_map.up.generic.callback = UI_VoteMapMenu_UpEvent;
    s_votemenu_map.up.width  		    = 64;
    s_votemenu_map.up.height  		    = 64;
    s_votemenu_map.up.focuspic         = ART_ARROWUP;

    s_votemenu_map.down.generic.type	  = MTYPE_BITMAP;
    s_votemenu_map.down.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_map.down.generic.x		  = 200-40;
    s_votemenu_map.down.generic.y		  = 128+30+64;
    s_votemenu_map.down.generic.id	      = ID_DOWN;
    s_votemenu_map.down.generic.callback = UI_VoteMapMenu_DownEvent;
    s_votemenu_map.down.width  		  = 64;
    s_votemenu_map.down.height  		  = 64;
    s_votemenu_map.down.focuspic         = ART_ARROWDOWN;

    s_votemenu_map.go.generic.type			= MTYPE_BITMAP;
    s_votemenu_map.go.generic.name			= ART_FIGHT0;
    s_votemenu_map.go.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_map.go.generic.id			= ID_GO;
    s_votemenu_map.go.generic.callback		= VoteMapMenu_Event;
    s_votemenu_map.go.generic.x			= 320+128-128;
    s_votemenu_map.go.generic.y			= 256+128-64;
    s_votemenu_map.go.width  				= 128;
    s_votemenu_map.go.height  				= 64;
    s_votemenu_map.go.focuspic				= ART_FIGHT1;

    s_votemenu_map.back.generic.type		= MTYPE_BITMAP;
    s_votemenu_map.back.generic.name		= ART_BACK0;
    s_votemenu_map.back.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_map.back.generic.id			= ID_BACK;
    s_votemenu_map.back.generic.callback	= VoteMapMenu_Event;
    s_votemenu_map.back.generic.x			= 320-128;
    s_votemenu_map.back.generic.y			= 256+128-64;
    s_votemenu_map.back.width				= 128;
    s_votemenu_map.back.height				= 64;
    s_votemenu_map.back.focuspic			= ART_BACK1;
}

t_mappage mappage;

/*
=================
UI_VoteMapMenu
 *Called from outside
=================
*/
void UI_VoteMapMenu( void ) {
    int i;
    VoteMapMenu_Cache();
    memset( &s_votemenu_map, 0 ,sizeof(votemenu_map_t) );

    UI_VoteMapMenuInternal();

    //We need to initialize the list or it will be impossible to click on the items
    for(i=0;i<SIZE_OF_LIST;i++) {
        Q_strncpyz(mappage.mapname[i],"----",5);
    }
    trap_Cmd_ExecuteText( EXEC_APPEND,"getmappage 0" );
    trap_Cvar_Set( "cl_paused", "0" ); //We cannot send server commands while paused!

    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.banner );
    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.info );
    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.back );
    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.go );
    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.arrows );
    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.down );
    Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.up );
    for(i=0;i<SIZE_OF_LIST;i++)
        Menu_AddItem( &s_votemenu_map.menu, (void*) &s_votemenu_map.maps[i] );

    UI_PushMenu( &s_votemenu_map.menu );
}

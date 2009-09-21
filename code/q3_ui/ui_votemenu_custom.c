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

static char* votemenu_custom_artlist[] =
{
	VOTEMENU_BACK0,
	VOTEMENU_BACK1,
        ART_FIGHT0,
        ART_FIGHT1,
	NULL
};

#define ID_BACK     100
#define ID_GO       101
#define ID_CUSTOM0        102
//next 11 for ID_CUSTOM1-CUSTOM11

#define CUSTOM_MENU_VERTICAL_SPACING	19
#define CUSTOM_MENU_MAX_ENTRIES 12

typedef struct
{
    menuframework_s	menu;
    menutext_s		banner;
    menubitmap_s	back;
    menubitmap_s	go;

    //Buttons:
    menutext_s      bEntry[CUSTOM_MENU_MAX_ENTRIES];

    //The text:
    char            text[CUSTOM_MENU_MAX_ENTRIES][32];

    int selection;
} votemenu_t;

static votemenu_t	s_votemenu_custom;

void UI_VoteCustomMenuInternal( void );

/*
=================
VoteMenu_custom_Event
=================
*/
static void VoteMenu_custom_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
            case ID_BACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
            case ID_GO:
                if( event != QM_ACTIVATED || !s_votemenu_custom.selection) {
                    return;
                }
                trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote custom %s",s_votemenu_custom.bEntry[s_votemenu_custom.selection-ID_CUSTOM0].string ) );
                UI_PopMenu();
                UI_PopMenu();
                break;
            default:
                if( event != QM_ACTIVATED ) {
                    return;
                }
                if(s_votemenu_custom.selection != ((menucommon_s*)ptr)->id) {
                    s_votemenu_custom.selection = ((menucommon_s*)ptr)->id;
                    UI_VoteCustomMenuInternal();
                }
                break;
        }

}

static void setCustomMenutext(menutext_s *menu,int y,int id,char *text) {
    menu->generic.type            = MTYPE_PTEXT;
    menu->color               = color_red;
    menu->generic.flags       = QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS;
    if(strlen(text)<1)
        menu->generic.flags   |= QMF_INACTIVE|QMF_GRAYED;
    else
    if(s_votemenu_custom.selection == id)
         menu->color       = color_orange;
    menu->generic.x           = 320;
    menu->generic.y           = y;
    menu->generic.id          = id;
    menu->generic.callback    = VoteMenu_custom_Event;
    menu->string              = text;
    menu->style               = UI_CENTER|UI_SMALLFONT;
}

/*
=================
VoteMenu_Custom_Cache
=================
*/
static void VoteMenu_Custom_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!votemenu_custom_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(votemenu_custom_artlist[i]);
	}
}

/*
=================
UI_VoteMenu_Custom_Draw
=================
*/
static void UI_VoteMenu_Custom_Draw( void ) {
	UI_DrawBannerString( 320, 16, "CALL VOTE CUSTOM", UI_CENTER, color_white );
	UI_DrawNamedPic( 320-233, 240-166, 466, 332, ART_BACKGROUND );

	// standard menu drawing
	Menu_Draw( &s_votemenu_custom.menu );
}

/*
=================
UI_VoteCustomMenuInternal
 *Used then forcing a redraw
=================
*/
void UI_VoteCustomMenuInternal( void )
{
    int y,i;
    char custominfo[MAX_INFO_STRING], *token,*pointer;

    VoteMenu_Custom_Cache();

    memset( &custominfo, 0, sizeof(custominfo));
    trap_Cvar_VariableStringBuffer("cg_vote_custom_commands",custominfo,sizeof(custominfo));

    s_votemenu_custom.menu.wrapAround = qtrue;
    s_votemenu_custom.menu.fullscreen = qfalse;
    s_votemenu_custom.menu.draw = UI_VoteMenu_Custom_Draw;

    s_votemenu_custom.banner.generic.type  = MTYPE_BTEXT;
    s_votemenu_custom.banner.generic.x	  = 320;
    s_votemenu_custom.banner.generic.y	  = 16;
    s_votemenu_custom.banner.string		  = "CALL VOTE CUSTOM";
    s_votemenu_custom.banner.color	      = color_white;
    s_votemenu_custom.banner.style	      = UI_CENTER;

    pointer = custominfo;

    y = 98;
    for(i=0;i<CUSTOM_MENU_MAX_ENTRIES;i++) {
        token = COM_Parse( &pointer );
        Q_strncpyz(s_votemenu_custom.text[i],token,sizeof(s_votemenu_custom.text[0]));
        setCustomMenutext(&s_votemenu_custom.bEntry[i],y,ID_CUSTOM0+i,s_votemenu_custom.text[i]);
        y+=CUSTOM_MENU_VERTICAL_SPACING;
    }

    s_votemenu_custom.back.generic.type	   = MTYPE_BITMAP;
    s_votemenu_custom.back.generic.name     = VOTEMENU_BACK0;
    s_votemenu_custom.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_custom.back.generic.callback = VoteMenu_custom_Event;
    s_votemenu_custom.back.generic.id	   = ID_BACK;
    s_votemenu_custom.back.generic.x		   = 320-128;
    s_votemenu_custom.back.generic.y		   = 256+128-64;
    s_votemenu_custom.back.width  		   = 128;
    s_votemenu_custom.back.height  		   = 64;
    s_votemenu_custom.back.focuspic         = VOTEMENU_BACK1;

    s_votemenu_custom.go.generic.type	   = MTYPE_BITMAP;
    s_votemenu_custom.go.generic.name     = ART_FIGHT0;
    s_votemenu_custom.go.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
    s_votemenu_custom.go.generic.callback = VoteMenu_custom_Event;
    s_votemenu_custom.go.generic.id	   = ID_GO;
    s_votemenu_custom.go.generic.x		   = 320;
    s_votemenu_custom.go.generic.y		   = 256+128-64;
    s_votemenu_custom.go.width  		   = 128;
    s_votemenu_custom.go.height  		   = 64;
    s_votemenu_custom.go.focuspic         = ART_FIGHT1;
}

/*
=================
UI_VoteCustomMenu
 *Called from outside
=================
*/
void UI_VoteCustomMenu( void ) {
        int i;
        // zero set all our globals
	memset( &s_votemenu_custom, 0 ,sizeof(votemenu_t) );
        
        UI_VoteCustomMenuInternal();

	Menu_AddItem( &s_votemenu_custom.menu, (void*) &s_votemenu_custom.banner );
	Menu_AddItem( &s_votemenu_custom.menu, (void*) &s_votemenu_custom.back );
        Menu_AddItem( &s_votemenu_custom.menu, (void*) &s_votemenu_custom.go );
        for(i=0;i<CUSTOM_MENU_MAX_ENTRIES;i++)
            Menu_AddItem( &s_votemenu_custom.menu, (void*) &s_votemenu_custom.bEntry[i] );

	UI_PushMenu( &s_votemenu_custom.menu );
}

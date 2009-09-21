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
//
#include "ui_local.h"

/*********************************************************************************
	SPECIFY PASSWORD
*********************************************************************************/

#define SPECIFYPASSWORD_FRAMEL	"menu/art_blueish/frame2_l"
#define SPECIFYPASSWORD_FRAMER	"menu/art_blueish/frame1_r"
#define SPECIFYPASSWORD_BACK0		"menu/art_blueish/back_0"
#define SPECIFYPASSWORD_BACK1		"menu/art_blueish/back_1"
#define SPECIFYPASSWORD_FIGHT0	"menu/art_blueish/fight_0"
#define SPECIFYPASSWORD_FIGHT1	"menu/art_blueish/fight_1"

#define ID_SPECIFYPASSWORDBACK	102
#define ID_SPECIFYPASSWORDGO		103

static char* specifypassword_artlist[] =
{
	SPECIFYPASSWORD_FRAMEL,
	SPECIFYPASSWORD_FRAMER,
	SPECIFYPASSWORD_BACK0,	
	SPECIFYPASSWORD_BACK1,	
	SPECIFYPASSWORD_FIGHT0,
	SPECIFYPASSWORD_FIGHT1,
	NULL
};

typedef struct
{
	menuframework_s	menu;
	menutext_s	banner;
	menubitmap_s	framel;
	menubitmap_s	framer;
	menutext_s	info;
	menufield_s	password;
	menubitmap_s	go;
	menubitmap_s	back;

	char		*connectstring;
        char            servername[32];
} specifypassword_t;

static specifypassword_t	s_specifypassword;

/*
=================
SpecifyPassword_Event
=================
*/
static void SpecifyPassword_Event( void* ptr, int event ) 
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_SPECIFYPASSWORDGO:
			if (event != QM_ACTIVATED)
				break;

			if (s_specifypassword.password.field.buffer[0])
			{
				trap_Cvar_Set("password",s_specifypassword.password.field.buffer);
				trap_Cmd_ExecuteText( EXEC_APPEND, s_specifypassword.connectstring );
			}
			break;

		case ID_SPECIFYPASSWORDBACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
	}
}

/*
=================
SpecifyPassword_MenuInit
=================
*/
void SpecifyPassword_MenuInit( void )
{
	// zero set all our globals
	memset( &s_specifypassword, 0 ,sizeof(specifypassword_t) );

	SpecifyPassword_Cache();

	s_specifypassword.menu.wrapAround = qtrue;
	s_specifypassword.menu.fullscreen = qtrue;

	s_specifypassword.banner.generic.type	 = MTYPE_BTEXT;
	s_specifypassword.banner.generic.x     = 320;
	s_specifypassword.banner.generic.y     = 16;
	s_specifypassword.banner.string		 = "SPECIFY PASSWORD";
	s_specifypassword.banner.color  		 = color_white;
	s_specifypassword.banner.style  		 = UI_CENTER;

	s_specifypassword.framel.generic.type  = MTYPE_BITMAP;
	s_specifypassword.framel.generic.name  = SPECIFYPASSWORD_FRAMEL;
	s_specifypassword.framel.generic.flags = QMF_INACTIVE;
	s_specifypassword.framel.generic.x	 = 0;  
	s_specifypassword.framel.generic.y	 = 78;
	s_specifypassword.framel.width  	     = 256;
	s_specifypassword.framel.height  	     = 329;

	s_specifypassword.framer.generic.type  = MTYPE_BITMAP;
	s_specifypassword.framer.generic.name  = SPECIFYPASSWORD_FRAMER;
	s_specifypassword.framer.generic.flags = QMF_INACTIVE;
	s_specifypassword.framer.generic.x	 = 376;
	s_specifypassword.framer.generic.y	 = 76;
	s_specifypassword.framer.width  	     = 256;
	s_specifypassword.framer.height  	     = 334;

	s_specifypassword.info.generic.type	 = MTYPE_TEXT;
	s_specifypassword.info.generic.x     = 320;
	s_specifypassword.info.generic.y     = 160;
	s_specifypassword.info.color  		 = color_white;
	s_specifypassword.info.style  		 = UI_CENTER;
        s_specifypassword.info.string           = s_specifypassword.servername;

	s_specifypassword.password.generic.type       = MTYPE_FIELD;
	s_specifypassword.password.generic.name       = "Password:";
	s_specifypassword.password.generic.flags      = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_specifypassword.password.generic.x	      = 206;
	s_specifypassword.password.generic.y	      = 220;
	s_specifypassword.password.field.widthInChars = 38;
	s_specifypassword.password.field.maxchars     = 80;
	trap_Cvar_VariableStringBuffer("password",s_specifypassword.password.field.buffer,80);

	s_specifypassword.go.generic.type	    = MTYPE_BITMAP;
	s_specifypassword.go.generic.name     = SPECIFYPASSWORD_FIGHT0;
	s_specifypassword.go.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_specifypassword.go.generic.callback = SpecifyPassword_Event;
	s_specifypassword.go.generic.id	    = ID_SPECIFYPASSWORDGO;
	s_specifypassword.go.generic.x		= 640;
	s_specifypassword.go.generic.y		= 480-64;
	s_specifypassword.go.width  		    = 128;
	s_specifypassword.go.height  		    = 64;
	s_specifypassword.go.focuspic         = SPECIFYPASSWORD_FIGHT1;

	s_specifypassword.back.generic.type	  = MTYPE_BITMAP;
	s_specifypassword.back.generic.name     = SPECIFYPASSWORD_BACK0;
	s_specifypassword.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_specifypassword.back.generic.callback = SpecifyPassword_Event;
	s_specifypassword.back.generic.id	      = ID_SPECIFYPASSWORDBACK;
	s_specifypassword.back.generic.x		  = 0;
	s_specifypassword.back.generic.y		  = 480-64;
	s_specifypassword.back.width  		  = 128;
	s_specifypassword.back.height  		  = 64;
	s_specifypassword.back.focuspic         = SPECIFYPASSWORD_BACK1;

	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.banner );
	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.info );
	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.framel );
	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.framer );
	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.password );
	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.go );
	Menu_AddItem( &s_specifypassword.menu, &s_specifypassword.back );
}

/*
=================
SpecifyPassword_Cache
=================
*/
void SpecifyPassword_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!specifypassword_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(specifypassword_artlist[i]);
	}
}

/*
=================
UI_SpecifyPasswordMenu
=================
*/
void UI_SpecifyPasswordMenu( char* string, char *name )
{
	SpecifyPassword_MenuInit();
	s_specifypassword.connectstring = string;
        Q_strncpyz(s_specifypassword.servername,name,32);
	UI_PushMenu( &s_specifypassword.menu );
}



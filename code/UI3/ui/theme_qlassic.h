/*
===========================================================================
Copyright (C) 2015 Open Arena Team

This file is part of Open Arena.

Open Arena is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Open Arena is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Open Arena; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// 1999 menu color scheme. Black, orange, red
//
//
//#define	PLAYERPORTRAITS 	// show images of player portraits
//#define	USECINEMATICS			// show our stuff

#define MP_BACKGROUND "menuback" // background of the menu
#define MP_BACKGROUND1 "menuback" // background of the menu

#define MP_BACKGROUND2 "menubacknologo" // background of the menu

#define MP_TITLECOLOR 1 0 0 1 // Title colors
#define MP_TEXTCOLOR  		1.00 0.43 0.00 1.00 // regular color of menu button text
#define MP_FOCUSCOLOR 		1.00 1.00 0.00 1.00 // focus 
#define MP_BOX_HEADERCOLOR 	1.00 0.43 0.00 1.00 // for the text appearing above editboxes
#define MP_SMALLCOLOR 		1.00 0.43 0.00 1.00 // the smaller options colors

#define MP_BOX_BORDER 	0.0 0.0 0.0 1
#define MP_BOX_FORE 	1.00 0.43 0.00 1.00
#define MP_BOX_BACK 	0.00 0.00 0.00 0.30
#define MP_BOX_OUTLINE 	0 0 0 0

#define MP_BUTTONBGCOLOR	1 1.0 1.0 0
#define MP_BUTTON2BGCOLOR	1 1.0 1.0 0
#define MP_TABLONGBGCOLOR 	1 1.0 1.0 0
#define MP_UNTABLONGBGCOLOR 	1 1.0 1.0 0
#define MP_DIALOGBGCOLOR	0.0 0.0 0.0 0.5


#define MP_KEYBINDBGCOLOR  1.00 0.43 0.00 0.30
#define MP_KEYBINDSELCOLOR 1.00 0.43 0.00 1.00


// Context sensitive area at the bottom

#define TIP_BG 0.0 0.0 0.3 1.0
#define TIP_COLOR 1.00 0.43 0.00 1.00
#define TIP_SMALLTEXT 1.00 0.43 0.00 1.00





//
// Offsets and Alignments for main menus and the like (to adjust for like, background decoration images)
//


#define MMENU_ALIGNITEM ITEM_ALIGN_CENTER
#define MMENU_OFFSETITEMH 60
		
#define MP_KEYBINDMENU "ui/panels/keybind2.bit"
// uncomment this for simpler keybind choices
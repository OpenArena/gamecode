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
// Moe or cute - pink/blue on WHITE, pink/blue borders, kawaii~
//
//
//#define	PLAYERPORTRAITS 	// show images of player portraits
#define	USECINEMATICS			// show our stuff
#define TRANSITION_FADEOUT		// Enable fadeouts
#define TRANSITION_FADEIN		// Enable fadeins
//#define TRANSITION_LOGOZOOM		// Enable red logo zoomout for starting a game

#define MP_MENUMUSIC "music/menu_metal" // music to use for menu
#define MP_BACKGROUND "ui/assets/splash22.jpg" // background of the menu
#define MP_SND_SELECT "ui/metal/confirm.wav" // confirm noise
#define MP_SND_HOVER "ui/assets5/hover.wav" // hover noise

#define MP_CINBG "stuff"
#define MP_BACKGROUND1 "background_metal" // background of the menu
//#define MP_BACKGROUND1 "ui/metal/rays" // background of the menu
//#define MP_BACKGROUND1 "debugbg2" // background of the menu

#define MP_BACKGROUND2 "ui/moe/moebg" // background of the menu
#define MP_BACKGROUNDMODEL "models/ui/metal.md3"
#define MP_MAINBUTTONBG "ui/metal/buttonlonga" 
#define	MP_FATBUTTONBG "ui/metal/buttonlonga"
#define	MP_WIDEBUTTONBG "ui/metal/buttonlonga"	
#define	MP_WIDEBUTTONWONBG "ui/metal/buttonlonga"	
#define MP_FATBUTTONBG 	"ui/metal/buttonlonga"
#define	MP_LONGBUTTONBG "ui/metal/buttonlonga"
#define	MP_LONGBUTTONBGA "ui/metal/buttonlonga"

#define MP_DIALOGBG	"ui/metal/ray"	// used for every every and every box and every box and every box

#define MP_INGAMEBUTTONBARBG "ui/moe/moebutton" 
#define MP_INGAMEBUTTONBARBG2 "ui/moe/moebutton" 

#define MP_TITLECOLOR 1 1 1 1 // Title colors
#define MP_TEXTCOLOR  1.0 1.0 1.0 1 // regular color of menu button text
#define MP_FOCUSCOLOR 1.0 1.0 0.3 1 // focus 
#define MP_BOX_HEADERCOLOR 1 1 1 1 // for the text appearing above editboxes
#define MP_SMALLCOLOR 0.85 0.85 0.85 1 // the smaller options colors

#define MP_BOX_BORDER 	0.0 0.0 0.0 1
#define MP_BOX_FORE 	1   1   1   1
#define MP_BOX_BACK 	0.0 0.0 0.1 1
#define MP_BOX_OUTLINE 	0.01   0.5   1   0.3

#define MP_BUTTONBGCOLOR	1 1.0 1.0 0
#define MP_BUTTON2BGCOLOR	1 1.0 1.0 0
#define MP_TABLONGBGCOLOR 	1 1.0 1.0 0
#define MP_UNTABLONGBGCOLOR 	1 1.0 1.0 0
#define MP_DIALOGBGCOLOR	0.0 0.0 0.0 0.5


#define MP_KEYBINDBGCOLOR  1.0 1.0 1.0 1.0
#define MP_KEYBINDSELCOLOR 0.4 0.6 1.0 1.0


// Context sensitive area at the bottom

#define TIP_BG 0.0 0.0 0.3 1.0
#define TIP_COLOR 0.5 0.5 0.5 1.0
#define TIP_SMALLTEXT 0.5 0.5 0.5 0.6





//
// Offsets and Alignments for main menus and the like (to adjust for like, background decoration images)
//


#define MMENU_ALIGNITEM ITEM_ALIGN_LEFT
#define MMENU_OFFSETITEMH -240
		
#define MP_KEYBINDMENU "ui/panels/keybind2.bit"
// uncomment this for simpler keybind choices
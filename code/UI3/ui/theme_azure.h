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

#define TRANSITION_LOGOZOOM		// Enable red logo zoomout for starting a game



#define MP_MENUMUSIC "music/azuremenu" // music to use for menu
#define MP_BACKGROUND "ui/assets/splash22.jpg" // background of the menu
#define MP_SND_SELECT "ui/assets5/select.wav" // confirm noise
#define MP_SND_HOVER "ui/assets5/hover.wav" // hover noise

#define MP_BACKGROUND1 "background_azure" // background of the menu
//#define MP_BACKGROUND1 "debugbg2" // background of the menu

#define MP_BACKGROUND2 "mpbscroll" // background of the menu
#define MP_MENUMUSIC "music/menu_azure.mod" // music to use for menu
#define MP_MAINBUTTONBG "azurebutton" 
#define	MP_FATBUTTONBG "azurebutton"
#define	MP_WIDEBUTTONBG "azurebuttonlong"	
#define	MP_WIDEBUTTONWONBG "azurebuttonlongwon"	
#define MP_FATBUTTONBG 	"azurebuttoncombo"
#define	MP_LONGBUTTONBG "azurebuttonreallylong"
#define	MP_LONGBUTTONBGA "azurebuttonreallylonga"

#define MP_DIALOGBG	"dialogbg_azure"	// used for every every and every box and every box and every box

#define MP_INGAMEBUTTONBARBG "ui/assets3/igtopbtn" 
#define MP_INGAMEBUTTONBARBG2 "ui/assets3/igtopbtn2" 
#define MP_BACKGROUNDMODEL "models/ui/bg.md3"
#define MP_TITLECOLOR 1 1 1 1 // Title colors
#define MP_TEXTCOLOR  0.24 0.70 0.935 1 // regular color of menu button text
#define MP_FOCUSCOLOR 0.04 0.8 0.835 1 // focus 
#define MP_BOX_HEADERCOLOR 1 1 1 1 // for the text appearing above editboxes
#define MP_SMALLCOLOR 0.25 0.45 0.85 1 // the smaller options colors

#define MP_BOX_BORDER 	0.25 0.25 0.25 1
#define MP_BOX_FORE 	0.01   0.5   1   1
#define MP_BOX_BACK 	0.0 0.0 0.1 1
#define MP_BOX_OUTLINE 	0.01   0.5   1   0.3

#define MP_BUTTONBGCOLOR	0.254902 0.290196 0.427451 1.0
#define MP_BUTTON2BGCOLOR	0.156863 0.188235 0.258824 1.0
#define MP_TABLONGBGCOLOR 	0.156863 0.188235 0.258824 1.0
#define MP_UNTABLONGBGCOLOR 	0.156863 0.188235 0.258824 1.0
#define MP_DIALOGBGCOLOR	0.0823529 0.101961 0.12549 0.7


#define MP_KEYBINDBGCOLOR .325 .07 .11 1.0
#define MP_KEYBINDSELCOLOR .18 .73 0.91 1.0

// Context sensitive area at the bottom

#define TIP_BG 0.0 0.0 0.0 1.0
#define TIP_COLOR 1.0 1.0 1.0 1.0
#define TIP_SMALLTEXT 1 1 1 0.1


// Text Scales and stuff
// 
// 48pt - 	1.0
// 24pt - 	0.5
// 9pt - 	0.1875

#define SMALLFONTSIZE 	12
#define MEDIUMFONTSIZE 	24
#define LARGEFONTSIZE 	24

#define KEYBINDBITTEXTSCALE .1875		// Controls menu columns

//
// Offsets and Alignments for main menus and the like (to adjust for like, background decoration images)
//


#define MMENU_ALIGNITEM ITEM_ALIGN_CENTER
#define MMENU_OFFSETITEMH 60


		
#define MP_KEYBINDMENU "ui/panels/keybind2.bit"
// uncomment this for simpler keybind choices
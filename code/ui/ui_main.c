/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
/*
=======================================================================

USER INTERFACE MAIN

=======================================================================
*/

// use this to get a demo build without an explicit demo build, i.e. to get the demo ui files to build
//#define PRE_RELEASE_TADEMO

#include "ui_local.h"

uiInfo_t uiInfo;

static const char *MonthAbbrev[] = {
	"Jan","Feb","Mar",
	"Apr","May","Jun",
	"Jul","Aug","Sep",
	"Oct","Nov","Dec"
};


static const char *skillLevels[] = {
	"I Can Win",
	"Bring It On",
	"Hurt Me Plenty",
	"Hardcore",
	"Nightmare"
};

static const int numSkillLevels = sizeof(skillLevels) / sizeof(const char*);

/*
 *These sources are currently hardcoded in the engine.
 *
 *Empty options will not be displayed or circled to.
 */
static const char *netSources[] = {
	"Local",
	"",
	"Internet",
	"Favorites"
};
static const int numNetSources = sizeof(netSources) / sizeof(const char*);

static const serverFilter_t serverFilters[] = {
	{"All", "" },
	{"OpenArena", "" },
	{"Missionpack", "missionpack" },
	{"Rocket Arena", "arena" },
	{"Alliance", "alliance20" },
	{"Weapons Factory Arena", "wfa" },
	{"OSP", "osp" },
};

static const char *teamArenaGameTypes[] = {
	"FFA",
	"TOURNAMENT",
	"SP",
	"TEAM DM",
	"CTF",
	"1FCTF",
	"OVERLOAD",
	"HARVESTER",
	"ELIMINATION",
	"CTFELIMINATION",
	"LMS",
	"DOUBLE D",
	"DOMINATION",
	"POSSESSION"
};

static int const numTeamArenaGameTypes = sizeof(teamArenaGameTypes) / sizeof(const char*);

static const int numServerFilters = sizeof(serverFilters) / sizeof(serverFilter_t);

static char* netnames[] = {
	"???",
	"UDP",
	NULL
};

#ifndef MISSIONPACK // bk001206
static char quake3worldMessage[] = "Visit www.openarena.ws - News, Community, Events, Files";
#endif

static int gamecodetoui[] = {4,2,3,0,5,1,6};
static int uitogamecode[] = {4,6,2,3,1,5,7};


static void UI_StartServerRefresh(qboolean full);
static void UI_StopServerRefresh( void );
static void UI_DoServerRefresh( void );
static void UI_FeederSelection(float feederID, int index);
static void UI_BuildServerDisplayList(qboolean force, qboolean doReset);
static void UI_BuildServerStatus(qboolean force);
static void UI_BuildFindPlayerList(qboolean force);
static int QDECL UI_ServersQsortCompare( const void *arg1, const void *arg2 );
static int UI_MapCountByGameType(qboolean singlePlayer);
static int UI_HeadCountByTeam( void );
static void UI_ParseGameInfo(const char *teamFile);
static void UI_ParseTeamInfo(const char *teamFile);
static const char *UI_SelectedMap(int index, int *actual);
static const char *UI_SelectedHead(int index, int *actual);
static int UI_GetIndexFromSelection(int actual);

int ProcessNewUI( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .qvm file
================
*/
vmCvar_t  ui_new;
vmCvar_t  ui_leidebug;
vmCvar_t  ui_debug;
vmCvar_t  ui_initialized;
vmCvar_t  ui_teamArenaFirstRun;

void _UI_Init( qboolean, int randomSeed );
void _UI_Shutdown( void );
void _UI_KeyEvent( int key, qboolean down );
void _UI_MouseEvent( int dx, int dy );
void _UI_Refresh( int realtime );
qboolean _UI_IsFullscreen( void );
Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  )
{
	switch ( command ) {
	case UI_GETAPIVERSION:
		return UI_API_VERSION;

	case UI_INIT:
		_UI_Init(arg0, arg1);
		return 0;

	case UI_SHUTDOWN:
		_UI_Shutdown();
		return 0;

	case UI_KEY_EVENT:
		_UI_KeyEvent( arg0, arg1 );
		return 0;

	case UI_MOUSE_EVENT:
		_UI_MouseEvent( arg0, arg1 );
		return 0;

	case UI_REFRESH:
		_UI_Refresh( arg0 );
		return 0;

	case UI_IS_FULLSCREEN:
		return _UI_IsFullscreen();

	case UI_SET_ACTIVE_MENU:
		_UI_SetActiveMenu( arg0 );
		return 0;

	case UI_CONSOLE_COMMAND:
		return UI_ConsoleCommand(arg0);

	case UI_DRAW_CONNECT_SCREEN:
		UI_DrawConnectScreen( arg0 );
		return 0;
	case UI_HASUNIQUECDKEY: // mod authors need to observe this
		return qtrue; // bk010117 - change this to qfalse for mods!

	}

	return -1;
}



void AssetCache( void )
{
	int n;
	//if (Assets.textFont == NULL) {
	//}
	//Assets.background = trap_R_RegisterShaderNoMip( ASSET_BACKGROUND );
	//Com_Printf("Menu Size: %i bytes\n", sizeof(Menus));
	uiInfo.uiDC.Assets.gradientBar = trap_R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
	uiInfo.uiDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
	uiInfo.uiDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	uiInfo.uiDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	uiInfo.uiDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	uiInfo.uiDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
	uiInfo.uiDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	uiInfo.uiDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	uiInfo.uiDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
	// Changed RD
	uiInfo.uiDC.Assets.scrollBarHorz = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	uiInfo.uiDC.Assets.scrollBarVert = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	// end changed RD
	uiInfo.uiDC.Assets.scrollBar = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	uiInfo.uiDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
	uiInfo.uiDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
	uiInfo.uiDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
	uiInfo.uiDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
	uiInfo.uiDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
	uiInfo.uiDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
	uiInfo.uiDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );

// Changed RD
	uiInfo.uiDC.Assets.sliderThumb_sel = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB_SEL );
	uiInfo.uiDC.Assets.checkbox = trap_R_RegisterShaderNoMip( ASSET_CHECKBOX );
	uiInfo.uiDC.Assets.checkbox_sel = trap_R_RegisterShaderNoMip( ASSET_CHECKBOX_SEL );
	uiInfo.uiDC.Assets.combo = trap_R_RegisterShaderNoMip( ASSET_COMBO );
	uiInfo.uiDC.Assets.combo_sel = trap_R_RegisterShaderNoMip( ASSET_COMBO_SEL );
// end changed RD
	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		uiInfo.uiDC.Assets.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
	}

	uiInfo.newHighScoreSound = trap_S_RegisterSound("sound/feedback/voc_newhighscore.wav", qfalse);
// Changed RD
	uiInfo.uiDC.Assets.mainname = String_Alloc("main");
	uiInfo.uiDC.Assets.ingamename = String_Alloc("ingame");
	uiInfo.uiDC.Assets.errorname = String_Alloc("error_popmenu");
	uiInfo.uiDC.Assets.connectname = String_Alloc("connect");
	uiInfo.uiDC.Assets.endname = String_Alloc("endofGame");
	uiInfo.uiDC.Assets.teamname = String_Alloc("team");

	uiInfo.uiDC.Assets.scrollbarsize = SCROLLBAR_SIZE;
	uiInfo.uiDC.Assets.sliderwidth = SLIDER_WIDTH;
	uiInfo.uiDC.Assets.sliderheight = SLIDER_HEIGHT;
	uiInfo.uiDC.Assets.sliderthumbwidth = SLIDER_THUMB_WIDTH;
	uiInfo.uiDC.Assets.sliderthumbheight = SLIDER_THUMB_HEIGHT;
	uiInfo.uiDC.Assets.checkboxwidth = CHECKBOX_WIDTH;
	uiInfo.uiDC.Assets.checkboxheight = CHECKBOX_HEIGHT;
	uiInfo.uiDC.Assets.combowidth = COMBO_WIDTH;
	uiInfo.uiDC.Assets.comboheight = COMBO_HEIGHT;
// end changed RD
}

void _UI_DrawSides(float x, float y, float w, float h, float size)
{
	UI_AdjustFrom640( &x, &y, &w, &h );
	size *= uiInfo.uiDC.xscale;
	trap_R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

void _UI_DrawTopBottom(float x, float y, float w, float h, float size)
{
	UI_AdjustFrom640( &x, &y, &w, &h );
	size *= uiInfo.uiDC.yscale;
	trap_R_DrawStretchPic( x, y, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x, y + h - size, w, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}
/*
================
UI_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void _UI_DrawRect( float x, float y, float width, float height, float size, const float *color )
{
	trap_R_SetColor( color );

	_UI_DrawTopBottom(x, y, width, height, size);
	_UI_DrawSides(x, y, width, height, size);

	trap_R_SetColor( NULL );
}

int Text_Width(const char *text, float scale, int limit)
{
	int count,len;
	float out;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;
	fontInfo_t *font = &uiInfo.uiDC.Assets.textFont;
	if (scale <= ui_smallFont.value) {
		font = &uiInfo.uiDC.Assets.smallFont;
	}
	else if (scale >= ui_bigFont.value) {
		font = &uiInfo.uiDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	out = 0;
	if (text) {
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			}
			else {
				glyph = &font->glyphs[*s & 255];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}
	return out * useScale;
}

int Text_Height(const char *text, float scale, int limit)
{
	int len, count;
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text; // bk001206 - unsigned
	fontInfo_t *font = &uiInfo.uiDC.Assets.textFont;
	if (scale <= ui_smallFont.value) {
		font = &uiInfo.uiDC.Assets.smallFont;
	}
	else if (scale >= ui_bigFont.value) {
		font = &uiInfo.uiDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	max = 0;
	if (text) {
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			}
			else {
				glyph = &font->glyphs[*s & 255];
				if (max < glyph->height) {
					max = glyph->height;
				}
				s++;
				count++;
			}
		}
	}
	return max * useScale;
}

void Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader)
{
	float w, h;
	w = width * scale;
	h = height * scale;
	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style)
{
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;
	float useScale;
	fontInfo_t *font = &uiInfo.uiDC.Assets.textFont;
	if (scale <= ui_smallFont.value) {
		font = &uiInfo.uiDC.Assets.smallFont;
	}
	else if (scale >= ui_bigFont.value) {
		font = &uiInfo.uiDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	if (text) {
		const char *s = text; // bk001206 - unsigned
		trap_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[*s & 255];
			//int yadj = Assets.textFont.glyphs[text[i]].bottom + Assets.textFont.glyphs[text[i]].top;
			//float yadj = scale * (Assets.textFont.glyphs[text[i]].imageHeight - Assets.textFont.glyphs[text[i]].height);
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			}
			else {
				float yadj = useScale * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					Text_PaintChar(x + ofs, y - yadj + ofs,
					               glyph->imageWidth,
					               glyph->imageHeight,
					               useScale,
					               glyph->s,
					               glyph->t,
					               glyph->s2,
					               glyph->t2,
					               glyph->glyph);
					trap_R_SetColor( newColor );
					colorBlack[3] = 1.0;
					// Changed RD
					// based on Tremulous code
				}
				else if( style == ITEM_TEXTSTYLE_NEON ) {
					vec4_t glow, outer, inner, white;

					glow[ 0 ] = newColor[ 0 ] * 0.5;
					glow[ 1 ] = newColor[ 1 ] * 0.5;
					glow[ 2 ] = newColor[ 2 ] * 0.5;
					glow[ 3 ] = newColor[ 3 ] * 0.2;

					outer[ 0 ] = newColor[ 0 ];
					outer[ 1 ] = newColor[ 1 ];
					outer[ 2 ] = newColor[ 2 ];
					outer[ 3 ] = newColor[ 3 ];

					inner[ 0 ] = newColor[ 0 ] * 1.5 > 1.0f ? 1.0f : newColor[ 0 ] * 1.5;
					inner[ 1 ] = newColor[ 1 ] * 1.5 > 1.0f ? 1.0f : newColor[ 1 ] * 1.5;
					inner[ 2 ] = newColor[ 2 ] * 1.5 > 1.0f ? 1.0f : newColor[ 2 ] * 1.5;
					inner[ 3 ] = newColor[ 3 ];

					white[ 0 ] = white[ 1 ] = white[ 2 ] = white[ 3 ] = 1.0f;

					trap_R_SetColor( glow );
					Text_PaintChar(  x - 3, y - yadj - 3,
					                 glyph->imageWidth + 6,
					                 glyph->imageHeight + 6,
					                 useScale,
					                 glyph->s,
					                 glyph->t,
					                 glyph->s2,
					                 glyph->t2,
					                 glyph->glyph );

					trap_R_SetColor( outer );
					Text_PaintChar(  x - 1, y - yadj - 1,
					                 glyph->imageWidth + 2,
					                 glyph->imageHeight + 2,
					                 useScale,
					                 glyph->s,
					                 glyph->t,
					                 glyph->s2,
					                 glyph->t2,
					                 glyph->glyph );

					trap_R_SetColor( inner );
					Text_PaintChar(  x - 0.5, y - yadj - 0.5,
					                 glyph->imageWidth + 1,
					                 glyph->imageHeight + 1,
					                 useScale,
					                 glyph->s,
					                 glyph->t,
					                 glyph->s2,
					                 glyph->t2,
					                 glyph->glyph );

					trap_R_SetColor( white );
					// end changed RD
				}
				Text_PaintChar(x, y - yadj,
				               glyph->imageWidth,
				               glyph->imageHeight,
				               useScale,
				               glyph->s,
				               glyph->t,
				               glyph->s2,
				               glyph->t2,
				               glyph->glyph);

				x += (glyph->xSkip * useScale) + adjust;
				s++;
				count++;
			}
		}
		trap_R_SetColor( NULL );
	}
}

void Text_PaintWithCursor(float x, float y, float scale, vec4_t color, const char *text, int cursorPos, char cursor, int limit, int style)
{
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph, *glyph2;
	float yadj;
	float useScale;
	fontInfo_t *font = &uiInfo.uiDC.Assets.textFont;
	if (scale <= ui_smallFont.value) {
		font = &uiInfo.uiDC.Assets.smallFont;
	}
	else if (scale >= ui_bigFont.value) {
		font = &uiInfo.uiDC.Assets.bigFont;
	}
	useScale = scale * font->glyphScale;
	if (text) {
		const char *s = text; // bk001206 - unsigned
		trap_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		glyph2 = &font->glyphs[ (int) cursor]; // bk001206 - possible signed char
		while (s && *s && count < len) {
			glyph = &font->glyphs[*s & 255];
			//int yadj = Assets.textFont.glyphs[text[i]].bottom + Assets.textFont.glyphs[text[i]].top;
			//float yadj = scale * (Assets.textFont.glyphs[text[i]].imageHeight - Assets.textFont.glyphs[text[i]].height);
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			}
			else {
				yadj = useScale * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					Text_PaintChar(x + ofs, y - yadj + ofs,
					               glyph->imageWidth,
					               glyph->imageHeight,
					               useScale,
					               glyph->s,
					               glyph->t,
					               glyph->s2,
					               glyph->t2,
					               glyph->glyph);
					colorBlack[3] = 1.0;
					trap_R_SetColor( newColor );
				}
				Text_PaintChar(x, y - yadj,
				               glyph->imageWidth,
				               glyph->imageHeight,
				               useScale,
				               glyph->s,
				               glyph->t,
				               glyph->s2,
				               glyph->t2,
				               glyph->glyph);

				yadj = useScale * glyph2->top;
				if (count == cursorPos && !((uiInfo.uiDC.realTime/BLINK_DIVISOR) & 1)) {
					Text_PaintChar(x, y - yadj,
					               glyph2->imageWidth,
					               glyph2->imageHeight,
					               useScale,
					               glyph2->s,
					               glyph2->t,
					               glyph2->s2,
					               glyph2->t2,
					               glyph2->glyph);
				}

				x += (glyph->xSkip * useScale);
				s++;
				count++;
			}
		}
		// need to paint cursor at end of text
		if (cursorPos == len && !((uiInfo.uiDC.realTime/BLINK_DIVISOR) & 1)) {
			yadj = useScale * glyph2->top;
			Text_PaintChar(x, y - yadj,
			               glyph2->imageWidth,
			               glyph2->imageHeight,
			               useScale,
			               glyph2->s,
			               glyph2->t,
			               glyph2->s2,
			               glyph2->t2,
			               glyph2->glyph);

		}

		trap_R_SetColor( NULL );
	}
}


static void Text_Paint_Limit(float *maxX, float x, float y, float scale, vec4_t color, const char* text, float adjust, int limit)
{
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;
	if (text) {
		const char *s = text; // bk001206 - unsigned
		float max = *maxX;
		float useScale;
		fontInfo_t *font = &uiInfo.uiDC.Assets.textFont;
		if (scale <= ui_smallFont.value) {
			font = &uiInfo.uiDC.Assets.smallFont;
		}
		else if (scale > ui_bigFont.value) {
			font = &uiInfo.uiDC.Assets.bigFont;
		}
		useScale = scale * font->glyphScale;
		trap_R_SetColor( color );
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[*s & 255];
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			}
			else {
				float yadj = useScale * glyph->top;
				if (Text_Width(s, scale, 1) + x > max) {
					*maxX = 0;
					break;
				}
				Text_PaintChar(x, y - yadj,
				               glyph->imageWidth,
				               glyph->imageHeight,
				               useScale,
				               glyph->s,
				               glyph->t,
				               glyph->s2,
				               glyph->t2,
				               glyph->glyph);
				x += (glyph->xSkip * useScale) + adjust;
				*maxX = x;
				count++;
				s++;
			}
		}
		trap_R_SetColor( NULL );
	}

}


void UI_ShowPostGame(qboolean newHigh)
{
	trap_Cvar_Set ("cg_cameraOrbit", "0");
	trap_Cvar_Set("cg_thirdPerson", "0");
	uiInfo.soundHighScore = newHigh;
	_UI_SetActiveMenu(UIMENU_POSTGAME);
}
/*
=================
_UI_Refresh
=================
*/

void UI_DrawCenteredPic(qhandle_t image, int w, int h)
{
	int x, y;
	x = (SCREEN_WIDTH - w) / 2;
	y = (SCREEN_HEIGHT - h) / 2;
	UI_DrawHandlePic(x, y, w, h, image);
}

int frameCount = 0;
int startTime;

#define	UI_FPS_FRAMES	4
void _UI_Refresh( int realtime )
{
	static int index;
	static int	previousTimes[UI_FPS_FRAMES];

	//if ( !( trap_Key_GetCatcher() & KEYCATCH_UI ) ) {
	//	return;
	//}

	uiInfo.uiDC.frameTime = realtime - uiInfo.uiDC.realTime;
	uiInfo.uiDC.realTime = realtime;

	previousTimes[index % UI_FPS_FRAMES] = uiInfo.uiDC.frameTime;
	index++;
	if ( index > UI_FPS_FRAMES ) {
		int i, total;
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < UI_FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		uiInfo.uiDC.FPS = 1000 * UI_FPS_FRAMES / total;
	}



	UI_UpdateCvars();

	if (Menu_Count() > 0) {
		// paint all the menus
		Menu_PaintAll();
// Changed RD
		Menu_PaintEnd();
// end changed RD
		// refresh server browser list
		UI_DoServerRefresh();
		// refresh server status
		UI_BuildServerStatus(qfalse);
		// refresh find player list
		UI_BuildFindPlayerList(qfalse);
	}

	// draw cursor
	UI_SetColor( NULL );
	// Changed RD
	if (Menu_Count() > 0 && !trap_Cvar_VariableValue( "ui_loading" )) {
		// end changed RD
		UI_DrawHandlePic( uiInfo.uiDC.cursorx-16, uiInfo.uiDC.cursory-16, 32, 32, uiInfo.uiDC.Assets.cursor);
	}

#ifndef NDEBUG
	if (uiInfo.uiDC.debug) {
		// cursor coordinates
		//FIXME
		//UI_DrawString( 0, 0, va("(%d,%d)",uis.cursorx,uis.cursory), UI_LEFT|UI_SMALLFONT, colorRed );
	}
#endif

}

/*
=================
_UI_Shutdown
=================
*/
void _UI_Shutdown( void )
{
	trap_LAN_SaveCachedServers();
}

char *defaultMenu = NULL;

char *GetMenuBuffer(const char *filename)
{
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUFILE];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "menu file not found: %s, using default\n", filename ) );
		return defaultMenu;
	}
	if ( len >= MAX_MENUFILE ) {
		trap_Print( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", filename, len, MAX_MENUFILE ) );
		trap_FS_FCloseFile( f );
		return defaultMenu;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );
	//COM_Compress(buf);
	return buf;

}

qboolean Asset_Parse(int handle)
{
	pc_token_t token;
	const char *tempStr;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (Q_stricmp(token.string, "{") != 0) {
		return qfalse;
	}

	while ( 1 ) {

		memset(&token, 0, sizeof(pc_token_t));

		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if (Q_strequal(token.string, "}") ) {
			return qtrue;
		}

		// font
		if (Q_strequal(token.string, "font") ) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			trap_R_RegisterFont(tempStr, pointSize, &uiInfo.uiDC.Assets.textFont);
			uiInfo.uiDC.Assets.fontRegistered = qtrue;
			continue;
		}

		if (Q_strequal(token.string, "smallFont") ) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			trap_R_RegisterFont(tempStr, pointSize, &uiInfo.uiDC.Assets.smallFont);
			continue;
		}

		if (Q_strequal(token.string, "bigFont") ) {
			int pointSize;
			if (!PC_String_Parse(handle, &tempStr) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			trap_R_RegisterFont(tempStr, pointSize, &uiInfo.uiDC.Assets.bigFont);
			continue;
		}


		// gradientbar
		if (Q_strequal(token.string, "gradientbar") ) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.gradientBar = trap_R_RegisterShaderNoMip(tempStr);
			continue;
		}

		// enterMenuSound
		if (Q_strequal(token.string, "menuEnterSound") ) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuEnterSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		// exitMenuSound
		if (Q_strequal(token.string, "menuExitSound") ) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuExitSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		// itemFocusSound
		if (Q_strequal(token.string, "itemFocusSound") ) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.itemFocusSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		// menuBuzzSound
		if (Q_strequal(token.string, "menuBuzzSound") ) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuBuzzSound = trap_S_RegisterSound( tempStr, qfalse );
			continue;
		}

		if (Q_strequal(token.string, "cursor") ) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.cursorStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.cursor = trap_R_RegisterShaderNoMip( uiInfo.uiDC.Assets.cursorStr);
			continue;
		}

		if (Q_strequal(token.string, "fadeClamp") ) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.fadeClamp)) {
				return qfalse;
			}
			continue;
		}

		if (Q_strequal(token.string, "fadeCycle") ) {
			if (!PC_Int_Parse(handle, &uiInfo.uiDC.Assets.fadeCycle)) {
				return qfalse;
			}
			continue;
		}

		if (Q_strequal(token.string, "fadeAmount") ) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.fadeAmount)) {
				return qfalse;
			}
			continue;
		}

		if (Q_strequal(token.string, "shadowX") ) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.shadowX)) {
				return qfalse;
			}
			continue;
		}

		if (Q_strequal(token.string, "shadowY") ) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.shadowY)) {
				return qfalse;
			}
			continue;
		}

		if (Q_strequal(token.string, "shadowColor") ) {
			if (!PC_Color_Parse(handle, &uiInfo.uiDC.Assets.shadowColor)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.shadowFadeClamp = uiInfo.uiDC.Assets.shadowColor[3];
			continue;
		}
// Changed RD
		if (Q_stricmp(token.string, "mainMenu") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.mainname)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "ingameMenu") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.ingamename)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "errorMenu") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.errorname)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "connectMenu") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.connectname)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "endgameMenu") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.endname)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "teamMenu") == 0) {
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.teamname)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "scrollbarSize") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.scrollbarsize)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderWidth") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.sliderwidth)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderHeight") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.sliderheight)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderthumbWidth") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.sliderthumbwidth)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderthumbHeight") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.sliderthumbheight)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "sliderBar") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "sliderThumb") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "sliderThumbSel") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.sliderThumb_sel = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarHorz") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarHorz = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarVert") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarVert = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarThumb") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowUp") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowDown") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowLeft") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "scrollBarArrowRight") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxBase") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxRed") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxYellow") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxGreen") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxTeal") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxBlue") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxCyan") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "fxWhite") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "checkbox") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.checkbox = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "checkboxsel") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.checkbox_sel = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "checkboxWidth") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.checkboxwidth)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "checkboxHeight") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.checkboxheight)) {
				return qfalse;
			}
			continue;
		}
		if (Q_stricmp(token.string, "combo") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.combo = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "combosel") == 0) {
			if (!PC_String_Parse(handle, &tempStr)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.combo_sel = trap_R_RegisterShaderNoMip( tempStr);
			continue;
		}

		if (Q_stricmp(token.string, "comboWidth") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.combowidth)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "comboHeight") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.comboheight)) {
				return qfalse;
			}
			continue;
		}
// end changed RD
	}
	return qfalse;
}

void Font_Report( void )
{
	int i;
	Com_Printf("Font Info\n");
	Com_Printf("=========\n");
	for ( i = 32; i < 96; i++) {
		Com_Printf("Glyph handle %i: %i\n", i, uiInfo.uiDC.Assets.textFont.glyphs[i].glyph);
	}
}

void UI_Report( void )
{
	String_Report();
	//Font_Report();

}

void UI_ParseMenu(const char *menuFile)
{
	int handle;
	pc_token_t token;

	Com_Printf("Parsing menu file:%s\n", menuFile);

	handle = trap_PC_LoadSource(menuFile);
	if (!handle) {
		return;
	}

	while ( 1 ) {
		memset(&token, 0, sizeof(pc_token_t));
		if (!trap_PC_ReadToken( handle, &token )) {
			break;
		}

		//if ( Q_stricmp( token, "{" ) ) {
		//	Com_Printf( "Missing { in menu file\n" );
		//	break;
		//}

		//if ( menuCount == MAX_MENUS ) {
		//	Com_Printf( "Too many menus!\n" );
		//	break;
		//}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_strequal(token.string, "assetGlobalDef") ) {
			if (Asset_Parse(handle)) {
				continue;
			}
			else {
				break;
			}
		}

		if (Q_strequal(token.string, "menudef") ) {
			// start a new menu
			Menu_New(handle);
		}
	}
	trap_PC_FreeSource(handle);
}

qboolean Load_Menu(int handle)
{
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] != '{') {
		return qfalse;
	}

	while ( 1 ) {

		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if ( token.string[0] == 0 ) {
			return qfalse;
		}

		if ( token.string[0] == '}' ) {
			return qtrue;
		}

		UI_ParseMenu(token.string);
	}
	return qfalse;
}

void UI_LoadMenus(const char *menuFile, qboolean reset)
{
	pc_token_t token;
	int handle;
	int start;

	start = trap_Milliseconds();

	handle = trap_PC_LoadSource( menuFile );
	if (!handle) {
		trap_Error( va( S_COLOR_YELLOW "menu file not found: %s, using default\n", menuFile ) );
		handle = trap_PC_LoadSource( "ui/menus.txt" );
		if (!handle) {
			trap_Error( va( S_COLOR_RED "default menu file not found: ui/menus.txt, unable to continue!\n") );
		}
	}

	ui_new.integer = 1;

	if (reset) {
		Menu_Reset();
	}

	while ( 1 ) {
		if (!trap_PC_ReadToken(handle, &token))
			break;
		if( token.string[0] == 0 || token.string[0] == '}') {
			break;
		}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "loadmenu") == 0) {
			if (Load_Menu(handle)) {
				continue;
			}
			else {
				break;
			}
		}
	}

	Com_Printf("UI menu load time = %d milli seconds\n", trap_Milliseconds() - start);

	trap_PC_FreeSource( handle );
}

void UI_Load(void)
{
	char lastName[1024];
	menuDef_t *menu = Menu_GetFocused();
	if (!ui_introPlayed.integer) { // load a menu with little overhead with information about legal notices etc
		char *menuSet = "ui/intromenus.txt";

		String_Init();

		UI_LoadMenus(menuSet, qtrue);
		Menus_CloseAll();
		Menus_ActivateByName(lastName);

	}
	else {	// intro skipped, or read, etc

		char *menuSet = UI_Cvar_VariableString("ui_menuFiles");
		if (menu && menu->window.name) {
			Q_strncpyz(lastName, menu->window.name, sizeof(lastName));
		}
		if (menuSet == NULL || menuSet[0] == '\0') {
			menuSet = "ui/menus.txt";
		}

		String_Init();

		UI_ParseGameInfo("gameinfo.txt");
		UI_LoadArenas();

		UI_LoadMenus(menuSet, qtrue);
		Menus_CloseAll();
		Menus_ActivateByName(lastName);
	}
}

static const char *handicapValues[] = {"None","95","90","85","80","75","70","65","60","55","50","45","40","35","30","25","20","15","10","5",NULL};
#ifndef MISSIONPACK // bk001206
static int numHandicaps = sizeof(handicapValues) / sizeof(const char*);
#endif

static void UI_DrawHandicap(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int i, h;

	h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
	i = 20 - h / 5;

	Text_Paint(rect->x, rect->y, scale, color, handicapValues[i], 0, 0, textStyle);
}

static void UI_DrawClanName(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	Text_Paint(rect->x, rect->y, scale, color, UI_Cvar_VariableString("ui_teamName"), 0, 0, textStyle);
}


static void UI_SetCapFragLimits(qboolean uiVars)
{
	int cap = 5;
	int frag = 10;
	if (uiVars) {
		if (uiVars == qtrue) {
			trap_Cvar_Set("ui_captureLimit", va("%d", cap));
			trap_Cvar_Set("ui_fragLimit", va("%d", frag));
		}
		else if (uiVars == qfalse) {
			trap_Cvar_Set("capturelimit", va("%d", cap));
			trap_Cvar_Set("fraglimit", va("%d", frag));
		}
	}
	else
	{
		if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_OBELISK) {
			cap = 4;
			frag = 0;
		}
		else if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_HARVESTER) {
			cap = 15;
			frag = 0;
		}
		else if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_DOMINATION) {
			cap = 125;
			frag = 0;
		}
		else if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_POSSESSION) {
			frag = 125;
			cap = 0;
		}
	}
}
// ui_gameType assumes gametype 0 is -1 ALL and will not show
static void UI_DrawGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	Text_Paint(rect->x, rect->y, scale, color, uiInfo.gameTypes[ui_gameType.integer].gameType, 0, 0, textStyle);
}

static void UI_DrawNetGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (ui_netGameType.integer < 0 || ui_netGameType.integer > uiInfo.numGameTypes) {
		trap_Cvar_Set("ui_netGameType", "0");
		trap_Cvar_Set("ui_actualNetGameType", "0");
	}
	Text_Paint(rect->x, rect->y, scale, color, uiInfo.gameTypes[ui_netGameType.integer].gameType , 0, 0, textStyle);
}

static void UI_DrawJoinGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (ui_joinGameType.integer < 0 || ui_joinGameType.integer > uiInfo.numJoinGameTypes) {
		trap_Cvar_Set("ui_joinGameType", "0");
	}
	Text_Paint(rect->x, rect->y, scale, color, uiInfo.joinGameTypes[ui_joinGameType.integer].gameType , 0, 0, textStyle);
}



static int UI_TeamIndexFromName(const char *name)
{
	int i;

	if (name && *name) {
		for (i = 0; i < uiInfo.teamCount; i++) {
			if (Q_strequal(name, uiInfo.teamList[i].teamName) ) {
				return i;
			}
		}
	}

	return 0;

}

static void UI_DrawClanLogo(rectDef_t *rect, float scale, vec4_t color)
{
	int i;
	i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	if (i >= 0 && i < uiInfo.teamCount) {
		trap_R_SetColor( color );

		if (uiInfo.teamList[i].teamIcon == -1) {
			uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
			uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
			uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
		}

		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon);
		trap_R_SetColor(NULL);
	}
}

static void UI_DrawClanCinematic(rectDef_t *rect, float scale, vec4_t color)
{
	int i;
	i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	if (i >= 0 && i < uiInfo.teamCount) {

		if (uiInfo.teamList[i].cinematic >= -2) {
			if (uiInfo.teamList[i].cinematic == -1) {
				uiInfo.teamList[i].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.teamList[i].imageName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
			}
			if (uiInfo.teamList[i].cinematic >= 0) {
				trap_CIN_RunCinematic(uiInfo.teamList[i].cinematic);
				trap_CIN_SetExtents(uiInfo.teamList[i].cinematic, rect->x, rect->y, rect->w, rect->h);
				trap_CIN_DrawCinematic(uiInfo.teamList[i].cinematic);
			}
			else {
				trap_R_SetColor( color );
				UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal);
				trap_R_SetColor(NULL);
				uiInfo.teamList[i].cinematic = -2;
			}
		}
		else {
			trap_R_SetColor( color );
			UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon);
			trap_R_SetColor(NULL);
		}
	}

}

static void UI_DrawPreviewCinematic(rectDef_t *rect, float scale, vec4_t color)
{
	if (uiInfo.previewMovie > -2) {
		uiInfo.previewMovie = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.movieList[uiInfo.movieIndex]), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		if (uiInfo.previewMovie >= 0) {
			trap_CIN_RunCinematic(uiInfo.previewMovie);
			trap_CIN_SetExtents(uiInfo.previewMovie, rect->x, rect->y, rect->w, rect->h);
			trap_CIN_DrawCinematic(uiInfo.previewMovie);
		}
		else {
			uiInfo.previewMovie = -2;
		}
	}

}



static void UI_DrawSkill(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int i;
	i = trap_Cvar_VariableValue( "g_spSkill" );
	if (i < 1 || i > numSkillLevels) {
		i = 1;
	}
	Text_Paint(rect->x, rect->y, scale, color, skillLevels[i-1],0, 0, textStyle);
}


static void UI_DrawTeamName(rectDef_t *rect, float scale, vec4_t color, qboolean blue, int textStyle)
{
	int i;
	i = UI_TeamIndexFromName(UI_Cvar_VariableString((blue) ? "ui_blueTeam" : "ui_redTeam"));
	if (i >= 0 && i < uiInfo.teamCount) {
		Text_Paint(rect->x, rect->y, scale, color, va("%s: %s", (blue) ? "Blue" : "Red", uiInfo.teamList[i].teamName),0, 0, textStyle);
	}
}

static void UI_DrawTeamMember(rectDef_t *rect, float scale, vec4_t color, qboolean blue, int num, int textStyle)
{
	// 0 - None
	// 1 - Human
	// 2..NumCharacters - Bot
	int value = trap_Cvar_VariableValue(va(blue ? "ui_blueteam%i" : "ui_redteam%i", num));
	const char *text;
	if (value <= 0) {
		text = "Closed";
	}
	else if (value == 1) {
		text = "Human";
	}
	else {
		value -= 2;

		if (UI_IsATeamGametype(ui_actualNetGameType.integer)) {
			if (value >= uiInfo.characterCount) {
				value = 0;
			}
			text = uiInfo.characterList[value].name;
		}
		else {
			if (value >= UI_GetNumBots()) {
				value = 0;
			}
			text = UI_GetBotNameByNumber(value);
		}
	}
	Text_Paint(rect->x, rect->y, scale, color, text, 0, 0, textStyle);
}

static void UI_DrawEffects(rectDef_t *rect, float scale, vec4_t color)
{
	UI_DrawHandlePic( rect->x, rect->y - 14, 128, 8, uiInfo.uiDC.Assets.fxBasePic );
	UI_DrawHandlePic( rect->x + uiInfo.effectsColor * 16 + 8, rect->y - 16, 16, 12, uiInfo.uiDC.Assets.fxPic[uiInfo.effectsColor] );
}

static void UI_DrawMapPreview(rectDef_t *rect, float scale, vec4_t color, qboolean net)
{
	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;
	if (map < 0 || map > uiInfo.mapCount) {
		if (net) {
			ui_currentNetMap.integer = 0;
			trap_Cvar_Set("ui_currentNetMap", "0");
		}
		else {
			ui_currentMap.integer = 0;
			trap_Cvar_Set("ui_currentMap", "0");
		}
		map = 0;
	}

	if (uiInfo.mapList[map].levelShot == -1) {
		uiInfo.mapList[map].levelShot = trap_R_RegisterShaderNoMip(uiInfo.mapList[map].imageName);
	}

	if (uiInfo.mapList[map].levelShot > 0) {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.mapList[map].levelShot);
	}
	else {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, trap_R_RegisterShaderNoMip("menu/art/unknownmap"));
	}
}


static void UI_DrawMapTimeToBeat(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int minutes, seconds, time;
	if (ui_currentMap.integer < 0 || ui_currentMap.integer > uiInfo.mapCount) {
		ui_currentMap.integer = 0;
		trap_Cvar_Set("ui_currentMap", "0");
	}

	time = uiInfo.mapList[ui_currentMap.integer].timeToBeat[uiInfo.gameTypes[ui_gameType.integer].gtEnum];

	minutes = time / 60;
	seconds = time % 60;

	Text_Paint(rect->x, rect->y, scale, color, va("%02i:%02i", minutes, seconds), 0, 0, textStyle);
}



static void UI_DrawMapCinematic(rectDef_t *rect, float scale, vec4_t color, qboolean net)
{

	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;
	if (map < 0 || map > uiInfo.mapCount) {
		if (net) {
			ui_currentNetMap.integer = 0;
			trap_Cvar_Set("ui_currentNetMap", "0");
		}
		else {
			ui_currentMap.integer = 0;
			trap_Cvar_Set("ui_currentMap", "0");
		}
		map = 0;
	}

	if (uiInfo.mapList[map].cinematic >= -1) {
		if (uiInfo.mapList[map].cinematic == -1) {
			uiInfo.mapList[map].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[map].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}
		if (uiInfo.mapList[map].cinematic >= 0) {
			trap_CIN_RunCinematic(uiInfo.mapList[map].cinematic);
			trap_CIN_SetExtents(uiInfo.mapList[map].cinematic, rect->x, rect->y, rect->w, rect->h);
			trap_CIN_DrawCinematic(uiInfo.mapList[map].cinematic);
		}
		else {
			uiInfo.mapList[map].cinematic = -2;
		}
	}
	else {
		UI_DrawMapPreview(rect, scale, color, net);
	}
}



static qboolean updateModel = qtrue;
static qboolean q3Model = qfalse;

static void UI_DrawPlayerModel(rectDef_t *rect)
{
	static playerInfo_t info;
	char model[MAX_QPATH];
	char team[256];
	char head[256];
	vec3_t	viewangles;
	vec3_t	moveangles;

	/* Neon_Knight: First pass the MP checker. */
	if(ui_missionpackChecks.integer != 0) {
		Q_strncpyz(model, UI_Cvar_VariableString("team_model"), sizeof(model));
		Q_strncpyz(head, UI_Cvar_VariableString("team_headmodel"), sizeof(head));
	}
	else {
		Q_strncpyz(model, UI_Cvar_VariableString("model"), sizeof(model));
		Q_strncpyz(head, UI_Cvar_VariableString("headmodel"), sizeof(head));
	}
	if (trap_Cvar_VariableValue("ui_Q3Model")) {
		if (!q3Model) {
			q3Model = qtrue;
			updateModel = qtrue;
		}
		team[0] = '\0';
	}
	else {
		Q_strncpyz(team, UI_Cvar_VariableString("ui_teamName"), sizeof(team));
		if (q3Model) {
			q3Model = qfalse;
			updateModel = qtrue;
		}
	}
	if (updateModel) {
		memset( &info, 0, sizeof(playerInfo_t) );
		viewangles[YAW]   = 180 - 10;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		VectorClear( moveangles );
		UI_PlayerInfo_SetModel( &info, model, head, team);
		UI_PlayerInfo_SetInfo( &info, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
//		UI_RegisterClientModelname( &info, model, head, team);
		updateModel = qfalse;
	}

	UI_DrawPlayer( rect->x, rect->y, rect->w, rect->h, &info, uiInfo.uiDC.realTime / 2);

}

static void UI_DrawPlayerModel2(rectDef_t *rect)
{
	static playerInfo_t info;
	char model[MAX_QPATH];
	char team[256];
	char head[256];
	vec3_t	viewangles;
	vec3_t	moveangles;

	/* Neon_Knight: First pass the MP checker. */
	if(ui_missionpackChecks.integer != 0) {
		Q_strncpyz(model, UI_Cvar_VariableString("team_model"), sizeof(model));
		Q_strncpyz(head, UI_Cvar_VariableString("team_headmodel"), sizeof(head));
	}
	else {
		Q_strncpyz(model, UI_Cvar_VariableString("model"), sizeof(model));
		Q_strncpyz(head, UI_Cvar_VariableString("headmodel"), sizeof(head));
	}
	if (trap_Cvar_VariableValue("ui_Q3Model")) {
		if (!q3Model) {
			q3Model = qtrue;
			updateModel = qtrue;
		}
		team[0] = '\0';
	}
	else {
		Q_strncpyz(team, UI_Cvar_VariableString("ui_teamName"), sizeof(team));
		if (q3Model) {
			q3Model = qfalse;
			updateModel = qtrue;
		}
	}
	if (updateModel) {
		// Changed RD
		viewangles[YAW]   = trap_Cvar_VariableValue("ui_PlayerViewAngleYaw");
		viewangles[PITCH] = trap_Cvar_VariableValue("ui_PlayerViewAnglePitch");
		viewangles[ROLL]  = 0;
		moveangles[YAW]   = trap_Cvar_VariableValue("ui_PlayerMoveAngleYaw");
		moveangles[PITCH] = trap_Cvar_VariableValue("ui_PlayerMoveAnglePitch");
		moveangles[ROLL]  = 0;
		UI_RegisterClientModelname( &uiInfo.info, model, head, team );
		UI_PlayerInfo_SetInfo( &uiInfo.info, trap_Cvar_VariableValue("ui_LowerAnim"),
		                       trap_Cvar_VariableValue("ui_UpperAnim"), viewangles, moveangles,
		                       trap_Cvar_VariableValue("ui_Weapon"), qfalse );
		// end changed RD
		updateModel = qfalse;
	}

	UI_DrawPlayerII( rect->x, rect->y, rect->w, rect->h, &info, uiInfo.uiDC.realTime / 2);

}



// leilei - OC system

static void UI_DrawOC(rectDef_t *rect)
{
	static playerInfo_t info;
	char model[MAX_QPATH];

	// OC crap
	char legsModel[MAX_QPATH];
	char torsoModel[MAX_QPATH];
	char team[256];
	char head[256];
	vec3_t	viewangles;
	vec3_t	moveangles;


	strcpy(legsModel, UI_Cvar_VariableString("oc_legs"));
	strcpy(torsoModel, UI_Cvar_VariableString("oc_torso"));

	if (updateModel) {
		memset( &info, 0, sizeof(playerInfo_t) );
		viewangles[YAW]   = 180;	// straight FORWARD
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		VectorClear( moveangles );
		UI_PlayerInfo_SetModel( &info, model, head, team);
		UI_PlayerInfo_SetInfo( &info, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, 0, qfalse );
//		UI_RegisterClientModelname( &info, model, head, team);

		// OC parts
		//info.oc_ClothesModel. torsoModel);
		//info.oc_PantsModel. legsModel);

		updateModel = qfalse;
	}

	UI_DrawPlayerOC( rect->x, rect->y, rect->w, rect->h, &info, uiInfo.uiDC.realTime / 2);

}


static void UI_DrawNetSource(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (ui_netSource.integer < 0 || ui_netSource.integer >= numNetSources) {
		ui_netSource.integer = 0;
	}
	// Changed RD
	Text_Paint(rect->x, rect->y, scale, color, va("%s", netSources[ui_netSource.integer]), 0, 0, textStyle);
	// end changed RD
	//Text_Paint(rect->x, rect->y, scale, color, va("Source: %s", netSources[ui_netSource.integer]), 0, 0, textStyle);
}

static void UI_DrawNetMapPreview(rectDef_t *rect, float scale, vec4_t color)
{

	if (uiInfo.serverStatus.currentServerPreview > 0) {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.serverStatus.currentServerPreview);
	}
	else {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, trap_R_RegisterShaderNoMip("menu/art/unknownmap"));
	}
}

static void UI_DrawNetMapCinematic(rectDef_t *rect, float scale, vec4_t color)
{
	if (ui_currentNetMap.integer < 0 || ui_currentNetMap.integer > uiInfo.mapCount) {
		ui_currentNetMap.integer = 0;
		trap_Cvar_Set("ui_currentNetMap", "0");
	}

	if (uiInfo.serverStatus.currentServerCinematic >= 0) {
		trap_CIN_RunCinematic(uiInfo.serverStatus.currentServerCinematic);
		trap_CIN_SetExtents(uiInfo.serverStatus.currentServerCinematic, rect->x, rect->y, rect->w, rect->h);
		trap_CIN_DrawCinematic(uiInfo.serverStatus.currentServerCinematic);
	}
	else {
		UI_DrawNetMapPreview(rect, scale, color);
	}
}



static void UI_DrawNetFilter(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (ui_serverFilterType.integer < 0 || ui_serverFilterType.integer >= numServerFilters) {
		ui_serverFilterType.integer = 0;
	}
	Text_Paint(rect->x, rect->y, scale, color, va("Filter: %s", serverFilters[ui_serverFilterType.integer].description), 0, 0, textStyle);
}


static void UI_DrawTier(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int i;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
	if (i < 0 || i >= uiInfo.tierCount) {
		i = 0;
	}
	Text_Paint(rect->x, rect->y, scale, color, va("Tier: %s", uiInfo.tierList[i].tierName),0, 0, textStyle);
}

static void UI_DrawTierMap(rectDef_t *rect, int index)
{
	int i;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
	if (i < 0 || i >= uiInfo.tierCount) {
		i = 0;
	}

	if (uiInfo.tierList[i].mapHandles[index] == -1) {
		uiInfo.tierList[i].mapHandles[index] = trap_R_RegisterShaderNoMip(va("levelshots/%s", uiInfo.tierList[i].maps[index]));
	}

	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.tierList[i].mapHandles[index]);
}

static const char *UI_EnglishMapName(const char *map)
{
	int i;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (Q_strequal(map, uiInfo.mapList[i].mapLoadName) ) {
			return uiInfo.mapList[i].mapName;
		}
	}
	return "";
}

static void UI_DrawTierMapName(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int i, j;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
	if (i < 0 || i >= uiInfo.tierCount) {
		i = 0;
	}
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j >= MAPS_PER_TIER) {
		j = 0;
	}

	Text_Paint(rect->x, rect->y, scale, color, UI_EnglishMapName(uiInfo.tierList[i].maps[j]), 0, 0, textStyle);
}

static void UI_DrawTierGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int i, j;
	i = trap_Cvar_VariableValue( "ui_currentTier" );
	if (i < 0 || i >= uiInfo.tierCount) {
		i = 0;
	}
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j > MAPS_PER_TIER) {
		j = 0;
	}

	Text_Paint(rect->x, rect->y, scale, color, uiInfo.gameTypes[uiInfo.tierList[i].gameTypes[j]].gameType , 0, 0, textStyle);
}


#ifndef MISSIONPACK // bk001206
static const char *UI_OpponentLeaderName(void)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
	return uiInfo.teamList[i].teamMembers[0];
}
#endif

static const char *UI_AIFromName(const char *name)
{
	int j;
	for (j = 0; j < uiInfo.aliasCount; j++) {
		if (Q_strequal(uiInfo.aliasList[j].name, name) ) {
			return uiInfo.aliasList[j].ai;
		}
	}
	return "sergei";
}

#ifndef MISSIONPACK // bk001206
static const int UI_AIIndex(const char *name)
{
	int j;
	for (j = 0; j < uiInfo.characterCount; j++) {
		if (Q_strequal(name, uiInfo.characterList[j].name) ) {
			return j;
		}
	}
	return 0;
}
#endif

#ifndef MISSIONPACK // bk001206
static const int UI_AIIndexFromName(const char *name)
{
	int j;
	for (j = 0; j < uiInfo.aliasCount; j++) {
		if (Q_strequal(uiInfo.aliasList[j].name, name) ) {
			return UI_AIIndex(uiInfo.aliasList[j].ai);
		}
	}
	return 0;
}
#endif


#ifndef MISSIONPACK // bk001206
static const char *UI_OpponentLeaderHead(void)
{
	const char *leader = UI_OpponentLeaderName();
	return UI_AIFromName(leader);
}
#endif

#ifndef MISSIONPACK // bk001206
static const char *UI_OpponentLeaderModel(void)
{
	int i;
	const char *head = UI_OpponentLeaderHead();
	for (i = 0; i < uiInfo.characterCount; i++) {
		if (Q_strequal(head, uiInfo.characterList[i].name) ) {
			return uiInfo.characterList[i].base;
		}
	}
	return "sergei";
}
#endif

static qboolean updateOpponentModel = qtrue;
static void UI_DrawOpponent(rectDef_t *rect)
{
	static playerInfo_t info2;
	char model[MAX_QPATH];
	char headmodel[MAX_QPATH];
	char team[256];
	vec3_t	viewangles;
	vec3_t	moveangles;

	if (updateOpponentModel) {

		Q_strncpyz(model, UI_Cvar_VariableString("ui_opponentModel"), sizeof(model));
		Q_strncpyz(headmodel, UI_Cvar_VariableString("ui_opponentModel"), sizeof(headmodel));
		team[0] = '\0';

		memset( &info2, 0, sizeof(playerInfo_t) );
		viewangles[YAW]   = 180 - 10;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		VectorClear( moveangles );
		UI_PlayerInfo_SetModel( &info2, model, headmodel, "");
		UI_PlayerInfo_SetInfo( &info2, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
		UI_RegisterClientModelname( &info2, model, headmodel, team);
		updateOpponentModel = qfalse;
	}

	UI_DrawPlayer( rect->x, rect->y, rect->w, rect->h, &info2, uiInfo.uiDC.realTime / 2);
}

static void UI_DrawOpponent2(rectDef_t *rect)
{
	static playerInfo_t info2;
	char model[MAX_QPATH];
	char headmodel[MAX_QPATH];
	char team[256];
	vec3_t	viewangles;
	vec3_t	moveangles;

	if (updateOpponentModel) {

		Q_strncpyz(model, UI_Cvar_VariableString("ui_opponentModel"), sizeof(model));
		Q_strncpyz(headmodel, UI_Cvar_VariableString("ui_opponentModel"), sizeof(headmodel));
		team[0] = '\0';

		memset( &info2, 0, sizeof(playerInfo_t) );
		viewangles[YAW]   = 180 - 10;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		VectorClear( moveangles );
		UI_PlayerInfo_SetModel( &info2, model, headmodel, "");
		UI_PlayerInfo_SetInfo( &info2, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
		UI_RegisterClientModelname( &info2, model, headmodel, team);
		updateOpponentModel = qfalse;
	}

	UI_DrawPlayerII( rect->x, rect->y, rect->w, rect->h, &info2, uiInfo.uiDC.realTime / 2);

}

static void UI_NextOpponent( void )
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
	int j = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	i++;
	if (i >= uiInfo.teamCount) {
		i = 0;
	}
	if (i == j) {
		i++;
		if ( i >= uiInfo.teamCount) {
			i = 0;
		}
	}
	trap_Cvar_Set( "ui_opponentName", uiInfo.teamList[i].teamName );
}

static void UI_PriorOpponent( void )
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
	int j = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	i--;
	if (i < 0) {
		i = uiInfo.teamCount - 1;
	}
	if (i == j) {
		i--;
		if ( i < 0) {
			i = uiInfo.teamCount - 1;
		}
	}
	trap_Cvar_Set( "ui_opponentName", uiInfo.teamList[i].teamName );
}

static void	UI_DrawPlayerLogo(rectDef_t *rect, vec3_t color)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));

	if (uiInfo.teamList[i].teamIcon == -1) {
		uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
		uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
		uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

	trap_R_SetColor( color );
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon );
	trap_R_SetColor( NULL );
}

static void	UI_DrawPlayerPortrait(rectDef_t *rect)
{
	static playerInfo_t info;
	char model[MAX_QPATH];
	float randomtaunter;
	char team[256];
	char head[256];
	vec3_t	viewangles;
	vec3_t	moveangles;

	{
		strcpy(model, UI_Cvar_VariableString("model"));
		strcpy(head, UI_Cvar_VariableString("headmodel"));
		if (!q3Model) {
			q3Model = qtrue;
			updateModel = qtrue;
		}
		team[0] = '\0';

	}
	randomtaunter = random() * 2000;

	if (randomtaunter < 1) {
		viewangles[YAW]   = 180;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		UI_PlayerInfo_SetInfo( &info, LEGS_IDLE, TORSO_GESTURE, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
	}


	if (updateModel) {
		memset( &info, 0, sizeof(playerInfo_t) );
		//viewangles[YAW]   = 180 - 10 + sin(uiInfo.uiDC.realTime * 55 / PULSE_DIVISOR); // (sin(uiInfo.uiDC.realTime  * 75)  * 75);
		viewangles[YAW]   = 180;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		VectorClear( moveangles );
		UI_PlayerInfo_SetModel( &info, model, head, team);


		UI_PlayerInfo_SetInfo( &info, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
		//		UI_RegisterClientModelname( &info, model, head, team);
		updateModel = qfalse;
	}
	//viewangles[YAW] = (float)((float)uiInfo.uiDC.cursorx-16 * 0.00005f);
	//viewangles[PITCH] = uiInfo.uiDC.cursory-16;

	// do some head stuff

	UI_DrawPlayersBust( rect->x, rect->y, rect->w, rect->h, &info, uiInfo.uiDC.realTime / 2);

}


static void	UI_DrawPlayerLogoMetal(rectDef_t *rect, vec3_t color)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
		uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
		uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
		uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

	trap_R_SetColor( color );
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal );
	trap_R_SetColor( NULL );
}

static void	UI_DrawPlayerLogoName(rectDef_t *rect, vec3_t color)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
		uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
		uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
		uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

	trap_R_SetColor( color );
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Name );
	trap_R_SetColor( NULL );
}

static void	UI_DrawOpponentLogo(rectDef_t *rect, vec3_t color)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
		uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
		uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
		uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

	trap_R_SetColor( color );
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon );
	trap_R_SetColor( NULL );
}

static void	UI_DrawOpponentLogoMetal(rectDef_t *rect, vec3_t color)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
		uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
		uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
		uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

	trap_R_SetColor( color );
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal );
	trap_R_SetColor( NULL );
}

static void	UI_DrawOpponentLogoName(rectDef_t *rect, vec3_t color)
{
	int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
	if (uiInfo.teamList[i].teamIcon == -1) {
		uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
		uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
		uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
	}

	trap_R_SetColor( color );
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Name );
	trap_R_SetColor( NULL );
}

static void UI_DrawAllMapsSelection(rectDef_t *rect, float scale, vec4_t color, int textStyle, qboolean net)
{
	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;
	if (map >= 0 && map < uiInfo.mapCount) {
		Text_Paint(rect->x, rect->y, scale, color, uiInfo.mapList[map].mapName, 0, 0, textStyle);
	}
}

static void UI_DrawOpponentName(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	Text_Paint(rect->x, rect->y, scale, color, UI_Cvar_VariableString("ui_opponentName"), 0, 0, textStyle);
}

static void UI_DrawMenuMapName(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	Text_Paint(rect->x, rect->y, scale, color, uiInfo.mapList[2].mapLoadName, 0, 0, textStyle);
	Com_Printf( "menumapname called\n");
}



static int UI_OwnerDrawWidth(int ownerDraw, float scale)
{
	int i, h, value;
	const char *text;
	const char *s = NULL;

	switch (ownerDraw) {
	case UI_HANDICAP:
		h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
		i = 20 - h / 5;
		s = handicapValues[i];
		break;
	case UI_CLANNAME:
		s = UI_Cvar_VariableString("ui_teamName");
		break;
	case UI_GAMETYPE:
		s = uiInfo.gameTypes[ui_gameType.integer].gameType;
		break;
	case UI_SKILL:
		i = trap_Cvar_VariableValue( "g_spSkill" );
		if (i < 1 || i > numSkillLevels) {
			i = 1;
		}
		s = skillLevels[i-1];
		break;
	case UI_BLUETEAMNAME:
		i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_blueTeam"));
		if (i >= 0 && i < uiInfo.teamCount) {
			s = va("%s: %s", "Blue", uiInfo.teamList[i].teamName);
		}
		break;
	case UI_REDTEAMNAME:
		i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_redTeam"));
		if (i >= 0 && i < uiInfo.teamCount) {
			s = va("%s: %s", "Red", uiInfo.teamList[i].teamName);
		}
		break;
	case UI_BLUETEAM1:
	case UI_BLUETEAM2:
	case UI_BLUETEAM3:
	case UI_BLUETEAM4:
	case UI_BLUETEAM5:
		value = trap_Cvar_VariableValue(va("ui_blueteam%i", ownerDraw-UI_BLUETEAM1 + 1));
		if (value <= 0) {
			text = "Closed";
		}
		else if (value == 1) {
			text = "Human";
		}
		else {
			value -= 2;
			if (value >= uiInfo.aliasCount) {
				value = 0;
			}
			text = uiInfo.aliasList[value].name;
		}
		s = va("%i. %s", ownerDraw-UI_BLUETEAM1 + 1, text);
		break;
	case UI_REDTEAM1:
	case UI_REDTEAM2:
	case UI_REDTEAM3:
	case UI_REDTEAM4:
	case UI_REDTEAM5:
		value = trap_Cvar_VariableValue(va("ui_redteam%i", ownerDraw-UI_REDTEAM1 + 1));
		if (value <= 0) {
			text = "Closed";
		}
		else if (value == 1) {
			text = "Human";
		}
		else {
			value -= 2;
			if (value >= uiInfo.aliasCount) {
				value = 0;
			}
			text = uiInfo.aliasList[value].name;
		}
		s = va("%i. %s", ownerDraw-UI_REDTEAM1 + 1, text);
		break;
	case UI_NETSOURCE:
		if (ui_netSource.integer < 0 || ui_netSource.integer >= uiInfo.numJoinGameTypes) {
			ui_netSource.integer = 0;
		}
		s = va("Source: %s", netSources[ui_netSource.integer]);
		break;
	case UI_NETFILTER:
		if (ui_serverFilterType.integer < 0 || ui_serverFilterType.integer >= numServerFilters) {
			ui_serverFilterType.integer = 0;
		}
		s = va("Filter: %s", serverFilters[ui_serverFilterType.integer].description );
		break;
	case UI_TIER:
		break;
	case UI_TIER_MAPNAME:
		break;
	case UI_TIER_GAMETYPE:
		break;
	case UI_ALLMAPS_SELECTION:
		break;
	case UI_OPPONENT_NAME:
		break;
	case UI_MENUMAPNAME:
		break;
	case UI_KEYBINDSTATUS:
		if (Display_KeyBindPending()) {
			s = "Waiting for new key... Press ESCAPE to cancel";
		}
		else {
			s = "Press ENTER or CLICK to change, Press BACKSPACE to clear";
		}
		break;
	case UI_SERVERREFRESHDATE:
		s = UI_Cvar_VariableString(va("ui_lastServerRefresh_%i", ui_netSource.integer));
		break;
	default:
		break;
	}

	if (s) {
		return Text_Width(s, scale, 0);
	}
	return 0;
}

static void UI_DrawBotName(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	int value = uiInfo.botIndex;
	int game = trap_Cvar_VariableValue("g_gametype");
	const char *text = "";
	if (UI_IsATeamGametype(game)) {
		if (value >= uiInfo.characterCount) {
			value = 0;
		}
		text = uiInfo.characterList[value].name;
	}
	else {
		if (value >= UI_GetNumBots()) {
			value = 0;
		}
		text = UI_GetBotNameByNumber(value);
	}
	Text_Paint(rect->x, rect->y, scale, color, text, 0, 0, textStyle);
}

static void UI_DrawBotSkill(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (uiInfo.skillIndex >= 0 && uiInfo.skillIndex < numSkillLevels) {
		Text_Paint(rect->x, rect->y, scale, color, skillLevels[uiInfo.skillIndex], 0, 0, textStyle);
	}
}

static void UI_DrawRedBlue(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	Text_Paint(rect->x, rect->y, scale, color, (uiInfo.redBlue == 0) ? "Red" : "Blue", 0, 0, textStyle);
}

static void UI_DrawCrosshair(rectDef_t *rect, float scale, vec4_t color)
{
	trap_R_SetColor( color );
	if (uiInfo.currentCrosshair < 0 || uiInfo.currentCrosshair >= NUM_CROSSHAIRS) {
		uiInfo.currentCrosshair = 0;
	}
	UI_DrawHandlePic( rect->x, rect->y - rect->h, rect->w, rect->h, uiInfo.uiDC.Assets.crosshairShader[uiInfo.currentCrosshair]);
	trap_R_SetColor( NULL );
}

/*
===============
UI_BuildPlayerList
===============
*/
static void UI_BuildPlayerList( void )
{
	uiClientState_t	cs;
	int		n, count, team, team2, playerTeamNumber;
	char	info[MAX_INFO_STRING];

	trap_GetClientState( &cs );
	trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );
	uiInfo.playerNumber = cs.clientNum;
	uiInfo.teamLeader = atoi(Info_ValueForKey(info, "tl"));
	team = atoi(Info_ValueForKey(info, "t"));
	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	count = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	uiInfo.playerCount = 0;
	uiInfo.myTeamCount = 0;
	playerTeamNumber = 0;
	for( n = 0; n < count; n++ ) {
		trap_GetConfigString( CS_PLAYERS + n, info, MAX_INFO_STRING );

		if (info[0]) {
			Q_strncpyz( uiInfo.playerNames[uiInfo.playerCount], Info_ValueForKey( info, "n" ), MAX_NAME_LENGTH );
			Q_CleanStr( uiInfo.playerNames[uiInfo.playerCount] );
			uiInfo.playerCount++;
			team2 = atoi(Info_ValueForKey(info, "t"));
			if (team2 == team) {
				Q_strncpyz( uiInfo.teamNames[uiInfo.myTeamCount], Info_ValueForKey( info, "n" ), MAX_NAME_LENGTH );
				Q_CleanStr( uiInfo.teamNames[uiInfo.myTeamCount] );
				uiInfo.teamClientNums[uiInfo.myTeamCount] = n;
				if (uiInfo.playerNumber == n) {
					playerTeamNumber = uiInfo.myTeamCount;
				}
				uiInfo.myTeamCount++;
			}
		}
	}

	if (!uiInfo.teamLeader) {
		trap_Cvar_Set("cg_selectedPlayer", va("%d", playerTeamNumber));
	}

	n = trap_Cvar_VariableValue("cg_selectedPlayer");
	if (n < 0 || n > uiInfo.myTeamCount) {
		n = 0;
	}
	if (n < uiInfo.myTeamCount) {
		trap_Cvar_Set("cg_selectedPlayerName", uiInfo.teamNames[n]);
	}
}


static void UI_DrawSelectedPlayer(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
		uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
		UI_BuildPlayerList();
	}
	Text_Paint(rect->x, rect->y, scale, color, (uiInfo.teamLeader) ? UI_Cvar_VariableString("cg_selectedPlayerName") : UI_Cvar_VariableString("name") , 0, 0, textStyle);
}

static void UI_DrawServerRefreshDate(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	if (uiInfo.serverStatus.refreshActive) {
		vec4_t lowLight, newColor;
		lowLight[0] = 0.8 * color[0];
		lowLight[1] = 0.8 * color[1];
		lowLight[2] = 0.8 * color[2];
		lowLight[3] = 0.8 * color[3];
		LerpColor(color,lowLight,newColor,0.5+0.5*sin(uiInfo.uiDC.realTime / PULSE_DIVISOR));
		Text_Paint(rect->x, rect->y, scale, newColor, va("Getting info for %d servers (ESC to cancel)", trap_LAN_GetServerCount(ui_netSource.integer)), 0, 0, textStyle);
		// Changed RD
		uiInfo.dorefresh = qtrue;
		// end changed RD
	}
	else {
		char buff[64];
		Q_strncpyz(buff, UI_Cvar_VariableString(va("ui_lastServerRefresh_%i", ui_netSource.integer)), 64);
		Text_Paint(rect->x, rect->y, scale, color, va("Refresh Time: %s", buff), 0, 0, textStyle);
		// Changed RD
		if(uiInfo.dorefresh) {
			uiInfo.dorefresh =  qfalse;
			trap_LAN_GetServerAddressString(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], uiInfo.serverStatusAddress, sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
			UI_FeederSelection(FEEDER_SERVERS, 0);
		}
		// end changed RD
	}
}

static void UI_DrawServerMOTD(rectDef_t *rect, float scale, vec4_t color)
{
	if (uiInfo.serverStatus.motdLen) {
		float maxX;

		if (uiInfo.serverStatus.motdWidth == -1) {
			uiInfo.serverStatus.motdWidth = 0;
			uiInfo.serverStatus.motdPaintX = rect->x + 1;
			uiInfo.serverStatus.motdPaintX2 = -1;
		}

		if (uiInfo.serverStatus.motdOffset > uiInfo.serverStatus.motdLen) {
			uiInfo.serverStatus.motdOffset = 0;
			uiInfo.serverStatus.motdPaintX = rect->x + 1;
			uiInfo.serverStatus.motdPaintX2 = -1;
		}

		if (uiInfo.uiDC.realTime > uiInfo.serverStatus.motdTime) {
			uiInfo.serverStatus.motdTime = uiInfo.uiDC.realTime + 10;
			if (uiInfo.serverStatus.motdPaintX <= rect->x + 2) {
				if (uiInfo.serverStatus.motdOffset < uiInfo.serverStatus.motdLen) {
					uiInfo.serverStatus.motdPaintX += Text_Width(&uiInfo.serverStatus.motd[uiInfo.serverStatus.motdOffset], scale, 1) - 1;
					uiInfo.serverStatus.motdOffset++;
				}
				else {
					uiInfo.serverStatus.motdOffset = 0;
					if (uiInfo.serverStatus.motdPaintX2 >= 0) {
						uiInfo.serverStatus.motdPaintX = uiInfo.serverStatus.motdPaintX2;
					}
					else {
						uiInfo.serverStatus.motdPaintX = rect->x + rect->w - 2;
					}
					uiInfo.serverStatus.motdPaintX2 = -1;
				}
			}
			else {
				//serverStatus.motdPaintX--;
				uiInfo.serverStatus.motdPaintX -= 2;
				if (uiInfo.serverStatus.motdPaintX2 >= 0) {
					//serverStatus.motdPaintX2--;
					uiInfo.serverStatus.motdPaintX2 -= 2;
				}
			}
		}

		maxX = rect->x + rect->w - 2;
		Text_Paint_Limit(&maxX, uiInfo.serverStatus.motdPaintX, rect->y + rect->h - 3, scale, color, &uiInfo.serverStatus.motd[uiInfo.serverStatus.motdOffset], 0, 0);
		if (uiInfo.serverStatus.motdPaintX2 >= 0) {
			float maxX2 = rect->x + rect->w - 2;
			Text_Paint_Limit(&maxX2, uiInfo.serverStatus.motdPaintX2, rect->y + rect->h - 3, scale, color, uiInfo.serverStatus.motd, 0, uiInfo.serverStatus.motdOffset);
		}
		if (uiInfo.serverStatus.motdOffset && maxX > 0) {
			// if we have an offset ( we are skipping the first part of the string ) and we fit the string
			if (uiInfo.serverStatus.motdPaintX2 == -1) {
				uiInfo.serverStatus.motdPaintX2 = rect->x + rect->w - 2;
			}
		}
		else {
			uiInfo.serverStatus.motdPaintX2 = -1;
		}

	}
}


// Changed RD
static void UI_DrawKeyBindStatus(rectDef_t *rect, float scale, vec4_t color, int textStyle, int ownerDrawFlags)
{
	if (Display_KeyBindPending()) {
		Text_Paint(rect->x, rect->y, scale, color, "Waiting for new key... Press ESCAPE to cancel", 0, 0, textStyle);
	}
	else {
		if(!(ownerDrawFlags & UI_BIND2CLICK)) {
			Text_Paint(rect->x, rect->y, scale, color, "Press ENTER or CLICK to change, Press BACKSPACE to clear", 0, 0, textStyle);
		}
	}
}
// end changed RD


static void UI_DrawGLInfo(rectDef_t *rect, float scale, vec4_t color, int textStyle)
{
	char * eptr;
	char buff[1024];
	const char *lines[64];
	int y, numLines, i;

	Text_Paint(rect->x + 2, rect->y, scale, color, va("VENDOR: %s", uiInfo.uiDC.glconfig.vendor_string), 0, 30, textStyle);
	Text_Paint(rect->x + 2, rect->y + 15, scale, color, va("VERSION: %s: %s", uiInfo.uiDC.glconfig.version_string,uiInfo.uiDC.glconfig.renderer_string), 0, 30, textStyle);
	Text_Paint(rect->x + 2, rect->y + 30, scale, color, va ("PIXELFORMAT: color(%d-bits) Z(%d-bits) stencil(%d-bits)", uiInfo.uiDC.glconfig.colorBits, uiInfo.uiDC.glconfig.depthBits, uiInfo.uiDC.glconfig.stencilBits), 0, 30, textStyle);

	// build null terminated extension strings
	// TTimo: https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=399
	// in TA this was not directly crashing, but displaying a nasty broken shader right in the middle
	// brought down the string size to 1024, there's not much that can be shown on the screen anyway
	Q_strncpyz(buff, uiInfo.uiDC.glconfig.extensions_string, 1024);
	eptr = buff;
	y = rect->y + 45;
	numLines = 0;
	while ( y < rect->y + rect->h && *eptr ) {
		while ( *eptr == ' ' )
			*eptr++ = '\0';

		// track start of valid string
		if (*eptr && *eptr != ' ') {
			lines[numLines++] = eptr;
		}

		while ( *eptr && *eptr != ' ' )
			eptr++;
	}

	i = 0;
	while (i < numLines) {
		Text_Paint(rect->x + 2, y, scale, color, lines[i++], 0, 20, textStyle);
		if (i < numLines) {
			Text_Paint(rect->x + rect->w / 2, y, scale, color, lines[i++], 0, 20, textStyle);
		}
		y += 10;
		if (y > rect->y + rect->h - 11) {
			break;
		}
	}


}

// FIXME: table drive
//
static void UI_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle)
{
	rectDef_t rect;
// Changed RD
	trap_R_SetColor( color );
// end changed RD
	rect.x = x + text_x;
	rect.y = y + text_y;
	rect.w = w;
	rect.h = h;

	switch (ownerDraw) {
	case UI_HANDICAP:
		UI_DrawHandicap(&rect, scale, color, textStyle);
		break;
	case UI_EFFECTS:
		UI_DrawEffects(&rect, scale, color);
		break;
	case UI_PLAYERMODEL:
		UI_DrawPlayerModel(&rect);
		break;
	case UI_PLAYERMODEL2:
		UI_DrawPlayerModel2(&rect);
		break;
	case UI_PLAYERMODEL_OC:
		UI_DrawOC(&rect);
		break;
	case UI_CLANNAME:
		UI_DrawClanName(&rect, scale, color, textStyle);
		break;
	case UI_CLANLOGO:
		UI_DrawClanLogo(&rect, scale, color);
		break;
	case UI_CLANCINEMATIC:
		UI_DrawClanCinematic(&rect, scale, color);
		break;
	case UI_PREVIEWCINEMATIC:
		UI_DrawPreviewCinematic(&rect, scale, color);
		break;
	case UI_GAMETYPE:
		UI_DrawGameType(&rect, scale, color, textStyle);
		break;
	case UI_NETGAMETYPE:
		UI_DrawNetGameType(&rect, scale, color, textStyle);
		break;
	case UI_JOINGAMETYPE:
		UI_DrawJoinGameType(&rect, scale, color, textStyle);
		break;
	case UI_MAPPREVIEW:
		UI_DrawMapPreview(&rect, scale, color, qtrue);
		break;
	case UI_MAP_TIMETOBEAT:
		UI_DrawMapTimeToBeat(&rect, scale, color, textStyle);
		break;
	case UI_MAPCINEMATIC:
		UI_DrawMapCinematic(&rect, scale, color, qfalse);
		break;
	case UI_STARTMAPCINEMATIC:
		UI_DrawMapCinematic(&rect, scale, color, qtrue);
		break;
	case UI_SKILL:
		UI_DrawSkill(&rect, scale, color, textStyle);
		break;
	case UI_BLUETEAMNAME:
		UI_DrawTeamName(&rect, scale, color, qtrue, textStyle);
		break;
	case UI_REDTEAMNAME:
		UI_DrawTeamName(&rect, scale, color, qfalse, textStyle);
		break;
	case UI_BLUETEAM1:
	case UI_BLUETEAM2:
	case UI_BLUETEAM3:
	case UI_BLUETEAM4:
	case UI_BLUETEAM5:
		UI_DrawTeamMember(&rect, scale, color, qtrue, ownerDraw - UI_BLUETEAM1 + 1, textStyle);
		break;
	case UI_REDTEAM1:
	case UI_REDTEAM2:
	case UI_REDTEAM3:
	case UI_REDTEAM4:
	case UI_REDTEAM5:
		UI_DrawTeamMember(&rect, scale, color, qfalse, ownerDraw - UI_REDTEAM1 + 1, textStyle);
		break;
	case UI_NETSOURCE:
		UI_DrawNetSource(&rect, scale, color, textStyle);
		break;
	case UI_NETMAPPREVIEW:
		UI_DrawNetMapPreview(&rect, scale, color);
		break;
	case UI_NETMAPCINEMATIC:
		UI_DrawNetMapCinematic(&rect, scale, color);
		break;
	case UI_NETFILTER:
		UI_DrawNetFilter(&rect, scale, color, textStyle);
		break;
	case UI_TIER:
		UI_DrawTier(&rect, scale, color, textStyle);
		break;
	case UI_OPPONENTMODEL:
		UI_DrawOpponent(&rect);
		break;
	case UI_OPPONENTMODEL2:
		UI_DrawOpponent2(&rect);
		break;
	case UI_TIERMAP1:
		UI_DrawTierMap(&rect, 0);
		break;
	case UI_TIERMAP2:
		UI_DrawTierMap(&rect, 1);
		break;
	case UI_TIERMAP3:
		UI_DrawTierMap(&rect, 2);
		break;
	case UI_PLAYERLOGO:
		UI_DrawPlayerLogo(&rect, color);
		break;
	case UI_PLAYERLOGO_METAL:
		UI_DrawPlayerLogoMetal(&rect, color);
		break;
	case UI_PLAYERLOGO_NAME:
		UI_DrawPlayerLogoName(&rect, color);
		break;
	case UI_OPPONENTLOGO:
		UI_DrawOpponentLogo(&rect, color);
		break;
	case UI_OPPONENTLOGO_METAL:
		UI_DrawOpponentLogoMetal(&rect, color);
		break;
	case UI_OPPONENTLOGO_NAME:
		UI_DrawOpponentLogoName(&rect, color);
		break;
	case UI_MENUMAPNAME:			// leilei
		UI_DrawMenuMapName(&rect, scale, color, textStyle);
		break;
	case UI_TIER_MAPNAME:
		UI_DrawTierMapName(&rect, scale, color, textStyle);
		break;
	case UI_TIER_GAMETYPE:
		UI_DrawTierGameType(&rect, scale, color, textStyle);
		break;
	case UI_ALLMAPS_SELECTION:
		UI_DrawAllMapsSelection(&rect, scale, color, textStyle, qtrue);
		break;
	case UI_MAPS_SELECTION:
		UI_DrawAllMapsSelection(&rect, scale, color, textStyle, qfalse);
		break;
	case UI_OPPONENT_NAME:
		UI_DrawOpponentName(&rect, scale, color, textStyle);
		break;
	case UI_BOTNAME:
		UI_DrawBotName(&rect, scale, color, textStyle);
		break;
	case UI_BOTSKILL:
		UI_DrawBotSkill(&rect, scale, color, textStyle);
		break;
	case UI_REDBLUE:
		UI_DrawRedBlue(&rect, scale, color, textStyle);
		break;
	case UI_CROSSHAIR:
		UI_DrawCrosshair(&rect, scale, color);
		break;
	case UI_SELECTEDPLAYER:
		UI_DrawSelectedPlayer(&rect, scale, color, textStyle);
		break;
	case UI_SERVERREFRESHDATE:
		UI_DrawServerRefreshDate(&rect, scale, color, textStyle);
		break;
	case UI_SERVERMOTD:
		UI_DrawServerMOTD(&rect, scale, color);
		break;
	case UI_GLINFO:
		UI_DrawGLInfo(&rect,scale, color, textStyle);
		break;
	case UI_KEYBINDSTATUS:
		// Changed RD
		UI_DrawKeyBindStatus(&rect,scale, color, textStyle, ownerDrawFlags);
		// end changed RD
		break;
	case UI_PLAYERPORTRAIT:
		UI_DrawPlayerPortrait(&rect);
		break;
	default:
		break;
	}
	// Changed RD
	trap_R_SetColor( NULL );
// end changed RD
}

static qboolean UI_OwnerDrawVisible(int flags)
{
	qboolean vis = qtrue;

	while (flags) {

		if (flags & UI_SHOW_FFA) {
			if (UI_IsATeamGametype(trap_Cvar_VariableValue("g_gametype")) || trap_Cvar_VariableValue("g_gametype") == GT_TOURNAMENT) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_FFA;
		}

		if (flags & UI_SHOW_NOTFFA) {
			if (!UI_IsATeamGametype(trap_Cvar_VariableValue("g_gametype")) && trap_Cvar_VariableValue("g_gametype") != GT_TOURNAMENT) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NOTFFA;
		}

		if (flags & UI_SHOW_LEADER) {
			// these need to show when this client can give orders to a player or a group
			if (!uiInfo.teamLeader) {
				vis = qfalse;
			}
			else {
				// if showing yourself
				if (ui_selectedPlayer.integer < uiInfo.myTeamCount && uiInfo.teamClientNums[ui_selectedPlayer.integer] == uiInfo.playerNumber) {
					vis = qfalse;
				}
			}
			flags &= ~UI_SHOW_LEADER;
		}
		if (flags & UI_SHOW_NOTLEADER) {
			// these need to show when this client is assigning their own status or they are NOT the leader
			if (uiInfo.teamLeader) {
				// if not showing yourself
				if (!(ui_selectedPlayer.integer < uiInfo.myTeamCount && uiInfo.teamClientNums[ui_selectedPlayer.integer] == uiInfo.playerNumber)) {
					vis = qfalse;
				}
				// these need to show when this client can give orders to a player or a group
			}
			flags &= ~UI_SHOW_NOTLEADER;
		}
		if (flags & UI_SHOW_FAVORITESERVERS) {
			// this assumes you only put this type of display flag on something showing in the proper context
			if (ui_netSource.integer != AS_FAVORITES) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_FAVORITESERVERS;
		}
		if (flags & UI_SHOW_NOTFAVORITESERVERS) {
			// this assumes you only put this type of display flag on something showing in the proper context
			if (ui_netSource.integer == AS_FAVORITES) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NOTFAVORITESERVERS;
		}
		if (flags & UI_SHOW_ANYTEAMGAME) {
			if (!UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYTEAMGAME;
		}
		if (flags & UI_SHOW_ANYNONTEAMGAME) {
			if (UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYNONTEAMGAME;
		}
		if (flags & UI_SHOW_NETANYTEAMGAME) {
			if (!UI_IsATeamGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYTEAMGAME;
		}
		if (flags & UI_SHOW_NETANYNONTEAMGAME) {
			if (UI_IsATeamGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYNONTEAMGAME;
		}
		if (flags & UI_SHOW_NEWHIGHSCORE) {
			if (uiInfo.newHighScoreTime < uiInfo.uiDC.realTime) {
				vis = qfalse;
			}
			else {
				if (uiInfo.soundHighScore) {
					if (trap_Cvar_VariableValue("sv_killserver") == 0) {
						// wait on server to go down before playing sound
						trap_S_StartLocalSound(uiInfo.newHighScoreSound, CHAN_ANNOUNCER);
						uiInfo.soundHighScore = qfalse;
					}
				}
			}
			flags &= ~UI_SHOW_NEWHIGHSCORE;
		}
		if (flags & UI_SHOW_NEWBESTTIME) {
			if (uiInfo.newBestTime < uiInfo.uiDC.realTime) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NEWBESTTIME;
		}
		if (flags & UI_SHOW_DEMOAVAILABLE) {
			if (!uiInfo.demoAvailable) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_DEMOAVAILABLE;
		}
		if (flags & UI_SHOW_ANYTEAMOBJECTIVEGAME) {
			if (!(UI_UsesKeyObjectives(uiInfo.gameTypes[ui_gameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum))) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYTEAMOBJECTIVEGAME;
		}
		if (flags & UI_SHOW_ANYNONTEAMOBJECTIVEGAME) {
			if (UI_UsesKeyObjectives(uiInfo.gameTypes[ui_gameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYNONTEAMOBJECTIVEGAME;
		}
		if (flags & UI_SHOW_NETANYTEAMOBJECTIVEGAME) {
			if (!(UI_UsesKeyObjectives(uiInfo.gameTypes[ui_netGameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum))) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYTEAMOBJECTIVEGAME;
		}
		if (flags & UI_SHOW_NETANYNONTEAMOBJECTIVEGAME) {
			if (UI_UsesKeyObjectives(uiInfo.gameTypes[ui_netGameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYNONTEAMOBJECTIVEGAME;
		}
		if (flags & UI_SHOW_ANYROUNDGAME) {
			if (!UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYROUNDGAME;
		}
		if (flags & UI_SHOW_ANYNONROUNDGAME) {
			if (UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYNONROUNDGAME;
		}
		if (flags & UI_SHOW_NETANYROUNDGAME) {
			if (!UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYROUNDGAME;
		}
		if (flags & UI_SHOW_NETANYNONROUNDGAME) {
			if (UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYNONROUNDGAME;
		}
		if (flags & UI_SHOW_ANYTEAMROUNDGAME) {
			if (!(UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum))) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYTEAMROUNDGAME;
		}
		if (flags & UI_SHOW_ANYNONTEAMROUNDGAME) {
			if (UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYNONTEAMROUNDGAME;
		}
		if (flags & UI_SHOW_NETANYTEAMROUNDGAME) {
			if (!(UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum))) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYTEAMROUNDGAME;
		}
		if (flags & UI_SHOW_NETANYNONTEAMROUNDGAME) {
			if (UI_IsARoundBasedGametype(uiInfo.gameTypes[ui_netGameType.integer].gtEnum) && UI_IsATeamGametype(uiInfo.gameTypes[ui_gameType.integer].gtEnum)) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYNONTEAMROUNDGAME;
		}
		else {
			flags = 0;
		}
	}
	return vis;
}

static qboolean UI_Handicap_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		int h;
		h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
		if (key == K_MOUSE2) {
			h -= 5;
		}
		else {
			h += 5;
		}
		if (h > 100) {
			h = 5;
		}
		else if (h < 0) {
			h = 100;
		}
		trap_Cvar_Set( "handicap", va( "%i", h) );
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_Effects_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {

		if (key == K_MOUSE2) {
			uiInfo.effectsColor--;
		}
		else {
			uiInfo.effectsColor++;
		}

		if( uiInfo.effectsColor > 6 ) {
			uiInfo.effectsColor = 0;
		}
		else if (uiInfo.effectsColor < 0) {
			uiInfo.effectsColor = 6;
		}

		trap_Cvar_SetValue( "color1", uitogamecode[uiInfo.effectsColor] );
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_ClanName_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		// Changed RD
		int i, j;
		// end changed RD
		i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
		if (uiInfo.teamList[i].cinematic >= 0) {
			trap_CIN_StopCinematic(uiInfo.teamList[i].cinematic);
			uiInfo.teamList[i].cinematic = -1;
		}
		if (key == K_MOUSE2) {
			i--;
		}
		else {
			i++;
		}
		if (i >= uiInfo.teamCount) {
			i = 0;
		}
		else if (i < 0) {
			i = uiInfo.teamCount - 1;
		}
		trap_Cvar_Set( "ui_teamName", uiInfo.teamList[i].teamName);
		UI_HeadCountByTeam();
		UI_FeederSelection(FEEDER_HEADS, 0);
		updateModel = qtrue;
		// Changed RD
		i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
		j = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
		if(i == j) {
			UI_NextOpponent();
		}
		// end changed RD
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_GameType_HandleKey(int flags, float *special, int key, qboolean resetMap)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		int oldCount = UI_MapCountByGameType(qtrue);

		// hard coded mess here
		// Changed RD
		if (key == K_MOUSE2) {
			do {
				ui_gameType.integer--;
				if (ui_gameType.integer < 0) {
					ui_gameType.integer = uiInfo.numGameTypes - 1;
				}
			}
			while (uiInfo.maskGameTypes[uiInfo.gameTypes[ui_gameType.integer].gtEnum]);
		}
		else {
			do {
				ui_gameType.integer++;
				if (ui_gameType.integer >= uiInfo.numGameTypes) {
					ui_gameType.integer = 0;
				}
			}
			while (uiInfo.maskGameTypes[uiInfo.gameTypes[ui_gameType.integer].gtEnum]);
		}
		// end changed RD

		if (uiInfo.gameTypes[ui_gameType.integer].gtEnum == GT_TOURNAMENT) {
			trap_Cvar_Set("ui_Q3Model", "1");
		}
		else {
			trap_Cvar_Set("ui_Q3Model", "0");
		}

		trap_Cvar_Set("ui_gameType", va("%d", ui_gameType.integer));
		UI_SetCapFragLimits(qtrue);
		UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);
		if (resetMap && oldCount != UI_MapCountByGameType(qtrue)) {
			trap_Cvar_Set( "ui_currentMap", "0");
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, 0, NULL);
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetGameType_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {

		if (key == K_MOUSE2) {
			do {
				ui_netGameType.integer--;
				if (ui_netGameType.integer < 0) {
					ui_netGameType.integer = uiInfo.numGameTypes - 1;
				}
			}
			while (uiInfo.maskGameTypes[uiInfo.gameTypes[ui_netGameType.integer].gtEnum]);
		}
		else {
			do {
				ui_netGameType.integer++;
				if (ui_netGameType.integer >= uiInfo.numGameTypes) {
					ui_netGameType.integer = 0;
				}
			}
			while (uiInfo.maskGameTypes[uiInfo.gameTypes[ui_netGameType.integer].gtEnum]);
		}

		trap_Cvar_Set( "ui_netGameType", va("%d", ui_netGameType.integer));
		trap_Cvar_Set( "ui_actualnetGameType", va("%d", uiInfo.gameTypes[ui_netGameType.integer].gtEnum));
		trap_Cvar_Set( "ui_currentNetMap", "0");
		UI_MapCountByGameType(qfalse);
		Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, NULL);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_JoinGameType_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {

		if (key == K_MOUSE2) {
			ui_joinGameType.integer--;
		}
		else {
			ui_joinGameType.integer++;
		}

		if (ui_joinGameType.integer < 0) {
			ui_joinGameType.integer = uiInfo.numJoinGameTypes - 1;
		}
		else if (ui_joinGameType.integer >= uiInfo.numJoinGameTypes) {
			ui_joinGameType.integer = 0;
		}

		trap_Cvar_Set( "ui_joinGameType", va("%d", ui_joinGameType.integer));
		UI_BuildServerDisplayList(qtrue, qtrue);
		return qtrue;
	}
	return qfalse;
}



static qboolean UI_Skill_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		int i = trap_Cvar_VariableValue( "g_spSkill" );

		if (key == K_MOUSE2) {
			i--;
		}
		else {
			i++;
		}

		if (i < 1) {
			i = numSkillLevels;
		}
		else if (i > numSkillLevels) {
			i = 1;
		}

		trap_Cvar_Set("g_spSkill", va("%i", i));
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_TeamName_HandleKey(int flags, float *special, int key, qboolean blue)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		int i;
		i = UI_TeamIndexFromName(UI_Cvar_VariableString((blue) ? "ui_blueTeam" : "ui_redTeam"));

		if (key == K_MOUSE2) {
			i--;
		}
		else {
			i++;
		}

		if (i >= uiInfo.teamCount) {
			i = 0;
		}
		else if (i < 0) {
			i = uiInfo.teamCount - 1;
		}

		trap_Cvar_Set( (blue) ? "ui_blueTeam" : "ui_redTeam", uiInfo.teamList[i].teamName);

		return qtrue;
	}
	return qfalse;
}

static qboolean UI_TeamMember_HandleKey(int flags, float *special, int key, qboolean blue, int num)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		// 0 - None
		// 1 - Human
		// 2..NumCharacters - Bot
		char *cvar = va(blue ? "ui_blueteam%i" : "ui_redteam%i", num);
		int value = trap_Cvar_VariableValue(cvar);

		if (key == K_MOUSE2) {
			value--;
		}
		else {
			value++;
		}
		if (UI_IsATeamGametype(ui_actualNetGameType.integer)) {
			if (value >= uiInfo.characterCount + 2) {
				value = 0;
			}
			else if (value < 0) {
				value = uiInfo.characterCount + 2 - 1;
			}
		}
		else {
			if (value >= UI_GetNumBots() + 2) {
				value = 0;
			}
			else if (value < 0) {
				value = UI_GetNumBots() + 2 - 1;
			}
		}

		trap_Cvar_Set(cvar, va("%i", value));
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetSource_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {

		if (key == K_MOUSE2) {
			ui_netSource.integer--;
			if(strlen(netSources[ui_netSource.integer])<1)
				ui_netSource.integer--;
		}
		else {
			ui_netSource.integer++;
			if(strlen(netSources[ui_netSource.integer])<1)
				ui_netSource.integer++;
		}

		if (ui_netSource.integer >= numNetSources) {
			ui_netSource.integer = 0;
		}
		else if (ui_netSource.integer < 0) {
			ui_netSource.integer = numNetSources - 1;
		}

		UI_BuildServerDisplayList(qtrue, qtrue);
		if (ui_netSource.integer != AS_GLOBAL) {
			UI_StartServerRefresh(qtrue);
		}
		trap_Cvar_Set( "ui_netSource", va("%d", ui_netSource.integer));
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetFilter_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {

		if (key == K_MOUSE2) {
			ui_serverFilterType.integer--;
		}
		else {
			ui_serverFilterType.integer++;
		}

		if (ui_serverFilterType.integer >= numServerFilters) {
			ui_serverFilterType.integer = 0;
		}
		else if (ui_serverFilterType.integer < 0) {
			ui_serverFilterType.integer = numServerFilters - 1;
		}
		UI_BuildServerDisplayList(qtrue, qtrue);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_OpponentName_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		if (key == K_MOUSE2) {
			UI_PriorOpponent();
		}
		else {
			UI_NextOpponent();
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_BotName_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		int game = trap_Cvar_VariableValue("g_gametype");
		int value = uiInfo.botIndex;

		if (key == K_MOUSE2) {
			value--;
		}
		else {
			value++;
		}

		if (UI_IsATeamGametype(game)) {
			if (value >= uiInfo.characterCount + 2) {
				value = 0;
			}
			else if (value < 0) {
				value = uiInfo.characterCount + 2 - 1;
			}
		}
		else {
			if (value >= UI_GetNumBots() + 2) {
				value = 0;
			}
			else if (value < 0) {
				value = UI_GetNumBots() + 2 - 1;
			}
		}
		uiInfo.botIndex = value;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_BotSkill_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		if (key == K_MOUSE2) {
			uiInfo.skillIndex--;
		}
		else {
			uiInfo.skillIndex++;
		}
		if (uiInfo.skillIndex >= numSkillLevels) {
			uiInfo.skillIndex = 0;
		}
		else if (uiInfo.skillIndex < 0) {
			uiInfo.skillIndex = numSkillLevels-1;
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_RedBlue_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		uiInfo.redBlue ^= 1;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_Crosshair_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		if (key == K_MOUSE2) {
			uiInfo.currentCrosshair--;
		}
		else {
			uiInfo.currentCrosshair++;
		}

		if (uiInfo.currentCrosshair >= NUM_CROSSHAIRS) {
			uiInfo.currentCrosshair = 0;
		}
		else if (uiInfo.currentCrosshair < 0) {
			uiInfo.currentCrosshair = NUM_CROSSHAIRS - 1;
		}
		trap_Cvar_Set("cg_drawCrosshair", va("%d", uiInfo.currentCrosshair));
		return qtrue;
	}
	return qfalse;
}



static qboolean UI_SelectedPlayer_HandleKey(int flags, float *special, int key)
{
	if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER) {
		int selected;

		UI_BuildPlayerList();
		if (!uiInfo.teamLeader) {
			return qfalse;
		}
		selected = trap_Cvar_VariableValue("cg_selectedPlayer");

		if (key == K_MOUSE2) {
			selected--;
		}
		else {
			selected++;
		}

		if (selected > uiInfo.myTeamCount) {
			selected = 0;
		}
		else if (selected < 0) {
			selected = uiInfo.myTeamCount;
		}

		if (selected == uiInfo.myTeamCount) {
			trap_Cvar_Set( "cg_selectedPlayerName", "Everyone");
		}
		else {
			trap_Cvar_Set( "cg_selectedPlayerName", uiInfo.teamNames[selected]);
		}
		trap_Cvar_Set( "cg_selectedPlayer", va("%d", selected));
	}
	return qfalse;
}


static qboolean UI_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key)
{
	switch (ownerDraw) {
	case UI_HANDICAP:
		return UI_Handicap_HandleKey(flags, special, key);
		break;
	case UI_EFFECTS:
		return UI_Effects_HandleKey(flags, special, key);
		break;
	case UI_CLANNAME:
		return UI_ClanName_HandleKey(flags, special, key);
		break;
	case UI_GAMETYPE:
		return UI_GameType_HandleKey(flags, special, key, qtrue);
		break;
	case UI_NETGAMETYPE:
		return UI_NetGameType_HandleKey(flags, special, key);
		break;
	case UI_JOINGAMETYPE:
		return UI_JoinGameType_HandleKey(flags, special, key);
		break;
	case UI_SKILL:
		return UI_Skill_HandleKey(flags, special, key);
		break;
	case UI_BLUETEAMNAME:
		return UI_TeamName_HandleKey(flags, special, key, qtrue);
		break;
	case UI_REDTEAMNAME:
		return UI_TeamName_HandleKey(flags, special, key, qfalse);
		break;
	case UI_BLUETEAM1:
	case UI_BLUETEAM2:
	case UI_BLUETEAM3:
	case UI_BLUETEAM4:
	case UI_BLUETEAM5:
		UI_TeamMember_HandleKey(flags, special, key, qtrue, ownerDraw - UI_BLUETEAM1 + 1);
		break;
	case UI_REDTEAM1:
	case UI_REDTEAM2:
	case UI_REDTEAM3:
	case UI_REDTEAM4:
	case UI_REDTEAM5:
		UI_TeamMember_HandleKey(flags, special, key, qfalse, ownerDraw - UI_REDTEAM1 + 1);
		break;
	case UI_NETSOURCE:
		UI_NetSource_HandleKey(flags, special, key);
		break;
	case UI_NETFILTER:
		UI_NetFilter_HandleKey(flags, special, key);
		break;
	case UI_OPPONENT_NAME:
		UI_OpponentName_HandleKey(flags, special, key);
		break;
	case UI_BOTNAME:
		return UI_BotName_HandleKey(flags, special, key);
		break;
	case UI_BOTSKILL:
		return UI_BotSkill_HandleKey(flags, special, key);
		break;
	case UI_REDBLUE:
		UI_RedBlue_HandleKey(flags, special, key);
		break;
	case UI_CROSSHAIR:
		UI_Crosshair_HandleKey(flags, special, key);
		break;
	case UI_SELECTEDPLAYER:
		UI_SelectedPlayer_HandleKey(flags, special, key);
		break;
	default:
		break;
	}

	return qfalse;
}


static float UI_GetValue(int ownerDraw)
{
	return 0;
}

/*
=================
UI_ServersQsortCompare
=================
*/
static int QDECL UI_ServersQsortCompare( const void *arg1, const void *arg2 )
{
	return trap_LAN_CompareServers( ui_netSource.integer, uiInfo.serverStatus.sortKey, uiInfo.serverStatus.sortDir, *(int*)arg1, *(int*)arg2);
}


/*
=================
UI_ServersSort
=================
*/
void UI_ServersSort(int column, qboolean force)
{

	if ( !force ) {
		if ( uiInfo.serverStatus.sortKey == column ) {
			return;
		}
	}

	uiInfo.serverStatus.sortKey = column;
	qsort( &uiInfo.serverStatus.displayServers[0], uiInfo.serverStatus.numDisplayServers, sizeof(int), UI_ServersQsortCompare);
}

/* Neon_Knight: Readding this here. May be useful in a future. */
/*
static void UI_StartSinglePlayer(void) {
	int i,j, k, skill;
	char buff[1024];
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= tierCount) {
    i = 0;
  }
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j >= MAPS_PER_TIER) {
		j = 0;
	}

 	trap_Cvar_SetValue( "singleplayer", 1 );
 	trap_Cvar_SetValue( "g_gametype", Com_Clamp( 0, GT_MAX_GAME_TYPE-1, tierList[i].gameTypes[j] ) );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", tierList[i].maps[j] ) );
	skill = trap_Cvar_VariableValue( "g_spSkill" );

	if (j == MAPS_PER_TIER-1) {
		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
		Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[0]), skill, "", teamList[k].teamMembers[0]);
	} else {
		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
		for (i = 0; i < PLAYERS_PER_TEAM; i++) {
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[i]), skill, "Blue", teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}

		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
		for (i = 1; i < PLAYERS_PER_TEAM; i++) {
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[i]), skill, "Red", teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; team Red\n" );
	}
	

}
*/


/*
===============
UI_LoadMods
===============
*/
static void UI_LoadMods( void )
{
	int		numdirs;
	char	dirlist[2048];
	char	*dirptr;
	char  *descptr;
	int		i;
	int		dirlen;

	uiInfo.modCount = 0;
	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for( i = 0; i < numdirs; i++ ) {
		dirlen = strlen( dirptr ) + 1;
		descptr = dirptr + dirlen;
		uiInfo.modList[uiInfo.modCount].modName = String_Alloc(dirptr);
		uiInfo.modList[uiInfo.modCount].modDescr = String_Alloc(descptr);
		dirptr += dirlen + strlen(descptr) + 1;
		uiInfo.modCount++;
		if (uiInfo.modCount >= MAX_MODS) {
			break;
		}
	}

}


/*
===============
UI_LoadTeams
===============
*/
static void UI_LoadTeams( void )
{
	char	teamList[4096];
	char	*teamName;
	int		i, len, count;

	count = trap_FS_GetFileList( "", "team", teamList, 4096 );

	if (count) {
		teamName = teamList;
		for ( i = 0; i < count; i++ ) {
			len = strlen( teamName );
			UI_ParseTeamInfo(teamName);
			teamName += len + 1;
		}
	}

}


/*
===============
UI_LoadMovies
===============
*/
static void UI_LoadMovies( void )
{
	char	movielist[4096];
	char	*moviename;
	int		i, len;

	uiInfo.movieCount = trap_FS_GetFileList( "video", "roq", movielist, 4096 );

	if (uiInfo.movieCount) {
		if (uiInfo.movieCount > MAX_MOVIES) {
			uiInfo.movieCount = MAX_MOVIES;
		}
		moviename = movielist;
		for ( i = 0; i < uiInfo.movieCount; i++ ) {
			len = strlen( moviename );
			if (Q_strequal(moviename +  len - 4,".roq")) {
				moviename[len-4] = '\0';
			}
			Q_strupr(moviename);
			uiInfo.movieList[i] = String_Alloc(moviename);
			moviename += len + 1;
		}
	}

}



/*
===============
UI_LoadDemos
===============
*/
static void UI_LoadDemos( void )
{
	char	demolist[4096];
	char demoExt[32];
	char	*demoname;
	int		i, len;

	Com_sprintf(demoExt, sizeof(demoExt), "dm_%d", (int)trap_Cvar_VariableValue("protocol"));

	uiInfo.demoCount = trap_FS_GetFileList( "demos", demoExt, demolist, 4096 );

	Com_sprintf(demoExt, sizeof(demoExt), ".dm_%d", (int)trap_Cvar_VariableValue("protocol"));

	if (uiInfo.demoCount) {
		if (uiInfo.demoCount > MAX_DEMOS) {
			uiInfo.demoCount = MAX_DEMOS;
		}
		demoname = demolist;
		for ( i = 0; i < uiInfo.demoCount; i++ ) {
			len = strlen( demoname );
			if (Q_strequal(demoname +  len - strlen(demoExt), demoExt)) {
				demoname[len-strlen(demoExt)] = '\0';
			}
			Q_strupr(demoname);
			uiInfo.demoList[i] = String_Alloc(demoname);
			demoname += len + 1;
		}
	}

}

static qboolean UI_SetNextMap(int actual, int index, char *name)
{
	int i;
	for (i = actual + 1; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, index + 1, name);
			return qtrue;
		}
	}
	return qfalse;
}


static void UI_StartSkirmish(qboolean next, char *name)
{
	int i, k, g, delay, temp;
	float skill;
	char buff[MAX_STRING_CHARS];

	if (next) {
		int actual;
		int index = trap_Cvar_VariableValue("ui_mapIndex");
		UI_MapCountByGameType(qtrue);
		UI_SelectedMap(index, &actual);
		if (UI_SetNextMap(actual, index, name)) {
		}
		else {
			UI_GameType_HandleKey(0, NULL, K_MOUSE1, qfalse);
			UI_MapCountByGameType(qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, 0, name);
// end changed RD
		}
	}

	g = uiInfo.gameTypes[ui_gameType.integer].gtEnum;
	trap_Cvar_SetValue( "g_gametype", g );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", uiInfo.mapList[ui_currentMap.integer].mapLoadName) );
	skill = trap_Cvar_VariableValue( "g_spSkill" );
	trap_Cvar_Set("ui_scoreMap", uiInfo.mapList[ui_currentMap.integer].mapName);

	k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));

	trap_Cvar_Set("ui_singlePlayerActive", "1");

	// set up sp overrides, will be replaced on postgame
	temp = trap_Cvar_VariableValue( "capturelimit" );
	trap_Cvar_Set("ui_saveCaptureLimit", va("%i", temp));
	temp = trap_Cvar_VariableValue( "fraglimit" );
	trap_Cvar_Set("ui_saveFragLimit", va("%i", temp));
	/* Neon_Knight: Since the SP uses custom time limits, pass it as well. */
	temp = trap_Cvar_VariableValue( "timelimit" );
	trap_Cvar_Set("ui_saveTimeLimit", va("%i", temp));

	UI_SetCapFragLimits(qfalse);

	temp = trap_Cvar_VariableValue( "cg_drawTimer" );
	trap_Cvar_Set("ui_drawTimer", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_doWarmup" );
	trap_Cvar_Set("ui_doWarmup", va("%i", temp));
	/* Neon_Knight: Since the SP uses bot_minplayers, pass it as well. */
	temp = trap_Cvar_VariableValue( "bot_minPlayers" );
	trap_Cvar_Set("ui_minPlayers", va("%i", temp));
	temp = trap_Cvar_VariableValue( "sv_maxClients" );
	trap_Cvar_Set("ui_maxClients", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_warmup" );
	trap_Cvar_Set("ui_Warmup", va("%i", temp));
	temp = trap_Cvar_VariableValue( "sv_pure" );
	trap_Cvar_Set("ui_pure", va("%i", temp));
	/* Neon_Knight: Since the SP uses game options, pass them as well. */
	temp = trap_Cvar_VariableValue( "g_friendlyFire" );
	trap_Cvar_Set("ui_friendlyFire", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_vampire" );
	trap_Cvar_Set("ui_vampire", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_lms_lives" );
	trap_Cvar_Set("ui_lmsLives", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_ddCaptureTime" );
	trap_Cvar_Set("ui_ddCaptureTime", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_ddRespawnDelay" );
	trap_Cvar_Set("ui_ddRespawnDelay", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_obeliskHealth" );
	trap_Cvar_Set("ui_obeliskHealth", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_rockets" );
	trap_Cvar_Set("ui_rockets", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_gravityModifier" );
	trap_Cvar_Set("ui_gravityModifier", va("%i", temp));
	temp = trap_Cvar_VariableValue( "elimination_ctf_oneway" );
	trap_Cvar_Set("ui_eCTFOneWayMode", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_instantgib" );
	trap_Cvar_Set("ui_instaGib", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_harvesterFromBodies" );
	trap_Cvar_Set("ui_harvesterFromBodies", va("%i", temp));

	trap_Cvar_Set("cg_cameraOrbit", "0");
	trap_Cvar_Set("cg_thirdPerson", "0");
	trap_Cvar_Set("cg_drawTimer", "1");
	trap_Cvar_Set("g_doWarmup", "1");
	trap_Cvar_Set("g_warmup", "15");
	trap_Cvar_Set("sv_pure", "0");
	/* Neon_Knight: This interferes. Commenting. */
	/* trap_Cvar_Set("g_friendlyFire", "0"); */
	trap_Cvar_Set("g_redTeam", UI_Cvar_VariableString("ui_teamName"));
	trap_Cvar_Set("g_blueTeam", UI_Cvar_VariableString("ui_opponentName"));

	if (trap_Cvar_VariableValue("ui_recordSPDemo")) {
		Com_sprintf(buff, MAX_STRING_CHARS, "%s_%i", uiInfo.mapList[ui_currentMap.integer].mapLoadName, g);
		trap_Cvar_Set("ui_recordSPDemoName", buff);
	}

	delay = 500;

	if (g == GT_TOURNAMENT) {
		trap_Cvar_Set("sv_maxClients", "2");
		Com_sprintf( buff, sizeof(buff), "addbot %s %f "", %i \n", uiInfo.mapList[ui_currentMap.integer].opponentName, skill, delay);
		trap_Cmd_ExecuteText( EXEC_APPEND, buff );
	}
	else if (!UI_IsATeamGametype(g) && g != GT_TOURNAMENT) { // leilei - parse the opponentname as a list of bots instead like q3_ui's arena parsing
		char		*p;
		char		*bot;
		const char	*botInfo;
		char		bots[MAX_INFO_STRING];
		char		botnames[16][16];
		int			count;
		// Make bot list from opponentName
		trap_Cvar_Set("sv_maxClients", "16");
		trap_Cvar_Set("g_warmup", "0");
		Q_strncpyz( bots, uiInfo.mapList[ui_currentMap.integer].opponentName, sizeof(bots) );
		//        Com_sprintf( buff, sizeof(buff), "wait\n", bot, skill, delay);
		p = &bots[0];
		count = 0;
		while( *p && count < 16 ) {

			//skip spaces
			while( *p == ' ' ) {
				p++;
			}
			if( !p ) {
				break;
			}

			// mark start of bot name
			bot = p;

			// skip until space of null
			while( *p && *p != ' ' ) {
				p++;
			}
			if( *p ) {
				*p++ = 0;
			}

			botInfo = UI_GetBotInfoByName( bot );
			bot = Info_ValueForKey( botInfo, "name" );
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %f "", %i \n", bot, skill, delay);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
			Q_strncpyz( botnames[count], bot, sizeof(botnames[count]) );
			delay += 50;
			//Com_Printf("The bot %s has been queued\n", bot);
			count++;
		}


		Com_sprintf( buff, sizeof(buff), "wait ;\n");
		trap_Cmd_ExecuteText( EXEC_APPEND, buff );
	}
	else {
		temp = uiInfo.mapList[ui_currentMap.integer].teamMembers * 2;
		/* Neon_Knight: Missionpack checks, if != 0, enables this. */
		if(ui_missionpackChecks.integer) {
			trap_Cvar_Set("sv_maxClients", va("%i", temp));
			for (i =0; i < uiInfo.mapList[ui_currentMap.integer].teamMembers; i++) {
				Com_sprintf( buff, sizeof(buff), "addbot %s %f %s %i %s\n", UI_AIFromName(uiInfo.teamList[k].teamMembers[i]), skill, (g == GT_FFA) ? "" : "Blue", delay, uiInfo.teamList[k].teamMembers[i]);
				trap_Cmd_ExecuteText( EXEC_APPEND, buff );
				delay += 500;
			}
			k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
			for (i =0; i < uiInfo.mapList[ui_currentMap.integer].teamMembers-1; i++) {
				Com_sprintf( buff, sizeof(buff), "addbot %s %f %s %i %s\n", UI_AIFromName(uiInfo.teamList[k].teamMembers[i]), skill, (g == GT_FFA) ? "" : "Red", delay, uiInfo.teamList[k].teamMembers[i]);
				trap_Cmd_ExecuteText( EXEC_APPEND, buff );
				delay += 500;
			}
		}
		else {
			trap_Cvar_Set("sv_maxClients", va("%i", temp+2));
			trap_Cvar_Set("bot_minplayers", va("%i", temp));
			trap_Cvar_Set("skill", va("%f", skill));
			Com_sprintf( buff, sizeof(buff), "wait ;\n");
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}
		/* /Neon_Knight */
	}
	if (UI_IsATeamGametype(g)) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; team Red\n" );
	}
}

static void UI_Update(const char *name)
{
	int	val = trap_Cvar_VariableValue(name);

	if (Q_strequal(name, "ui_SetName") ) {
		trap_Cvar_Set( "name", UI_Cvar_VariableString("ui_Name"));
	}
	else if (Q_strequal(name, "ui_setRate") ) {
		float rate = trap_Cvar_VariableValue("rate");
		if (rate >= 5000) {
			trap_Cvar_Set("cl_maxpackets", "30");
			trap_Cvar_Set("cl_packetdup", "1");
		}
		else if (rate >= 4000) {
			trap_Cvar_Set("cl_maxpackets", "15");
			trap_Cvar_Set("cl_packetdup", "2");		// favor less prediction errors when there's packet loss
		}
		else {
			trap_Cvar_Set("cl_maxpackets", "15");
			trap_Cvar_Set("cl_packetdup", "1");		// favor lower bandwidth
		}
	}
	else if (Q_strequal(name, "ui_GetName") ) {
		trap_Cvar_Set( "ui_Name", UI_Cvar_VariableString("name"));
	}
	else if (Q_strequal(name, "r_colorbits") ) {
		switch (val) {
		case 0:
			trap_Cvar_SetValue( "r_depthbits", 0 );
			trap_Cvar_SetValue( "r_stencilbits", 0 );
			break;
		case 16:
			trap_Cvar_SetValue( "r_depthbits", 16 );
			trap_Cvar_SetValue( "r_stencilbits", 0 );
			break;
		case 32:
			trap_Cvar_SetValue( "r_depthbits", 24 );
			break;
		}
	}
	else if (Q_strequal(name, "r_lodbias") ) {
		switch (val) {
		case 0:
			trap_Cvar_SetValue( "r_subdivisions", 4 );
			break;
		case 1:
			trap_Cvar_SetValue( "r_subdivisions", 12 );
			break;
		case 2:
			trap_Cvar_SetValue( "r_subdivisions", 20 );
			break;
		}
	}
	else if (Q_strequal(name, "ui_glCustom") ) {
		switch (val) {
		case 0:	// high quality
			trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 4 );
			trap_Cvar_SetValue( "r_vertexlight", 0 );
			trap_Cvar_SetValue( "r_lodbias", 0 );
			trap_Cvar_SetValue( "r_colorbits", 32 );
			trap_Cvar_SetValue( "r_depthbits", 24 );
			trap_Cvar_SetValue( "r_picmip", 0 );
			trap_Cvar_SetValue( "r_mode", 4 );
			trap_Cvar_SetValue( "r_texturebits", 32 );
			trap_Cvar_SetValue( "r_fastSky", 0 );
			trap_Cvar_SetValue( "r_inGameVideo", 1 );
			trap_Cvar_SetValue( "cg_shadows", 1 );
			trap_Cvar_SetValue( "cg_brassTime", 2500 );
			trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
			break;
		case 1: // normal
			trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 12 );
			trap_Cvar_SetValue( "r_vertexlight", 0 );
			trap_Cvar_SetValue( "r_lodbias", 0 );
			trap_Cvar_SetValue( "r_colorbits", 0 );
			trap_Cvar_SetValue( "r_depthbits", 24 );
			trap_Cvar_SetValue( "r_picmip", 1 );
			trap_Cvar_SetValue( "r_mode", 3 );
			trap_Cvar_SetValue( "r_texturebits", 0 );
			trap_Cvar_SetValue( "r_fastSky", 0 );
			trap_Cvar_SetValue( "r_inGameVideo", 1 );
			trap_Cvar_SetValue( "cg_brassTime", 2500 );
			trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
			trap_Cvar_SetValue( "cg_shadows", 0 );
			break;
		case 2: // fast
			trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 8 );
			trap_Cvar_SetValue( "r_vertexlight", 0 );
			trap_Cvar_SetValue( "r_lodbias", 1 );
			trap_Cvar_SetValue( "r_colorbits", 0 );
			trap_Cvar_SetValue( "r_depthbits", 0 );
			trap_Cvar_SetValue( "r_picmip", 1 );
			trap_Cvar_SetValue( "r_mode", 3 );
			trap_Cvar_SetValue( "r_texturebits", 0 );
			trap_Cvar_SetValue( "cg_shadows", 0 );
			trap_Cvar_SetValue( "r_fastSky", 1 );
			trap_Cvar_SetValue( "r_inGameVideo", 0 );
			trap_Cvar_SetValue( "cg_brassTime", 0 );
			trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
			break;
		case 3: // fastest
			trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 20 );
			trap_Cvar_SetValue( "r_vertexlight", 1 );
			trap_Cvar_SetValue( "r_lodbias", 2 );
			trap_Cvar_SetValue( "r_colorbits", 16 );
			trap_Cvar_SetValue( "r_depthbits", 16 );
			trap_Cvar_SetValue( "r_mode", 3 );
			trap_Cvar_SetValue( "r_picmip", 2 );
			trap_Cvar_SetValue( "r_texturebits", 16 );
			trap_Cvar_SetValue( "cg_shadows", 0 );
			trap_Cvar_SetValue( "cg_brassTime", 0 );
			trap_Cvar_SetValue( "r_fastSky", 1 );
			trap_Cvar_SetValue( "r_inGameVideo", 0 );
			trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
			break;
		}
	}
	else if (Q_strequal(name, "ui_mousePitch") ) {
		if (val == 0) {
			trap_Cvar_SetValue( "m_pitch", 0.022f );
		}
		else {
			trap_Cvar_SetValue( "m_pitch", -0.022f );
		}
// Changed RD
	}
	else if (Q_stricmp(name, "ui_netGametype") == 0) {
		trap_Cvar_Set( "ui_actualnetGameType", va("%d", uiInfo.gameTypes[ui_netGameType.integer].gtEnum));
		trap_Cvar_Set( "ui_currentNetMap", "0");
		UI_MapCountByGameType(qfalse);
		Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, NULL);
	}
	else if (Q_stricmp(name, "ui_netSource") == 0) {
		ui_netSource.integer = (int)trap_Cvar_VariableValue("ui_netSource");
		trap_Cvar_Set( "ui_netSource", va("%d",ui_netSource.integer));
		uiInfo.serverStatusInfo.numLines = 0;
		UI_BuildServerDisplayList(qtrue, qtrue);
		UI_StartServerRefresh(qtrue);
		uiInfo.serverStatus.currentServerPreview = 0;
// end changed RD

	}
}


// Changed RD
qboolean SP_LoadGame(char *load_game, char *loadmap)
{
	return qfalse; // leilei - we do not need this
}
// end changed RD

static void UI_RunMenuScript(char **args)
{
	const char *name, *name2;
	char buff[1024];

	if (String_Parse(args, &name)) {
		if (Q_strequal(name, "StartServer") ) {
			int i, clients, oldclients;
			float skill;
			trap_Cvar_Set("cg_thirdPerson", "0");
			trap_Cvar_Set("cg_cameraOrbit", "0");
			trap_Cvar_Set("ui_singlePlayerActive", "0");
			trap_Cvar_SetValue( "dedicated", Com_Clamp( 0, 2, ui_dedicated.integer ) );
			trap_Cvar_SetValue( "g_gametype", Com_Clamp( 0, 13, uiInfo.gameTypes[ui_netGameType.integer].gtEnum ) );
			trap_Cvar_Set("g_redTeam", UI_Cvar_VariableString("ui_teamName"));
			trap_Cvar_Set("g_blueTeam", UI_Cvar_VariableString("ui_opponentName"));
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", uiInfo.mapList[ui_currentNetMap.integer].mapLoadName ) );
			skill = trap_Cvar_VariableValue( "g_spSkill" );
			// set max clients based on spots
			oldclients = trap_Cvar_VariableValue( "sv_maxClients" );
			clients = 0;
			for (i = 0; i < PLAYERS_PER_TEAM; i++) {
				int bot = trap_Cvar_VariableValue( va("ui_blueteam%i", i+1));
				if (bot >= 0) {
					clients++;
				}
				bot = trap_Cvar_VariableValue( va("ui_redteam%i", i+1));
				if (bot >= 0) {
					clients++;
				}
			}
			if (clients == 0) {
				clients = 8;
			}

			if (oldclients > clients) {
				clients = oldclients;
			}

			trap_Cvar_Set("sv_maxClients", va("%d",clients));

			for (i = 0; i < PLAYERS_PER_TEAM; i++) {
				int bot = trap_Cvar_VariableValue( va("ui_blueteam%i", i+1));
				if (bot > 1) {
					if (UI_IsATeamGametype(ui_actualNetGameType.integer)) {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f %s\n", uiInfo.characterList[bot-2].name, skill, "Blue");
					}
					else {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f \n", UI_GetBotNameByNumber(bot-2), skill);
					}
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
				}
				bot = trap_Cvar_VariableValue( va("ui_redteam%i", i+1));
				if (bot > 1) {
					if (UI_IsATeamGametype(ui_actualNetGameType.integer)) {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f %s\n", uiInfo.characterList[bot-2].name, skill, "Red");
					}
					else {
						Com_sprintf( buff, sizeof(buff), "addbot %s %f \n", UI_GetBotNameByNumber(bot-2), skill);
					}
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
				}
			}
		}
		else if (Q_strequal(name, "updateSPMenu") ) {
			UI_SetCapFragLimits(qtrue);
			UI_MapCountByGameType(qtrue);
			ui_mapIndex.integer = UI_GetIndexFromSelection(ui_currentMap.integer);
			trap_Cvar_Set("ui_mapIndex", va("%d", ui_mapIndex.integer));
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, ui_mapIndex.integer, "skirmish");
			UI_GameType_HandleKey(0, NULL, K_MOUSE1, qfalse);
			UI_GameType_HandleKey(0, NULL, K_MOUSE2, qfalse);
			// Changed RD
		}
		else if (Q_stricmp(name, "updateSPMenu") == 0) {
			// Changed RD
			char name[MAX_NAME_LENGTH];
			name[0] = '\0';
			Q_strncpyz(name, "skirmish", MAX_NAME_LENGTH);
			if (String_Parse(args, &name2)) {
				Q_strncpyz(name, name2, MAX_NAME_LENGTH);
			}
			UI_SetCapFragLimits(qtrue);
			UI_MapCountByGameType(qtrue);
			ui_mapIndex.integer = UI_GetIndexFromSelection(ui_currentMap.integer);
			trap_Cvar_Set("ui_mapIndex", va("%d", ui_mapIndex.integer));
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, ui_mapIndex.integer, name);
			// end changed RD
			UI_GameType_HandleKey(0, NULL, K_MOUSE1, qfalse);
			UI_GameType_HandleKey(0, NULL, K_MOUSE2, qfalse);
			// Changed RD
		}
		else if (Q_stricmp(name, "getTeamMembers") == 0) {
			int team = uiInfo.mapList[ui_currentMap.integer].teamMembers;
			trap_Cvar_Set("ui_TeamMembers", va("%d", team));
			// end changed RD
		}
		else if (Q_stricmp(name, "resetDefaults") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, "exec default.cfg\n");
			trap_Cmd_ExecuteText( EXEC_APPEND, "cvar_restart\n");
			Controls_SetDefaults();
			trap_Cvar_Set("com_introPlayed", "1" );
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
		}
		else if (Q_strequal(name, "getCDKey") ) {
			char out[17];
			trap_GetCDKey(buff, 17);
			trap_Cvar_Set("cdkey1", "");
			trap_Cvar_Set("cdkey2", "");
			trap_Cvar_Set("cdkey3", "");
			trap_Cvar_Set("cdkey4", "");
			if (strlen(buff) == CDKEY_LEN) {
				Q_strncpyz(out, buff, 5);
				trap_Cvar_Set("cdkey1", out);
				Q_strncpyz(out, buff + 4, 5);
				trap_Cvar_Set("cdkey2", out);
				Q_strncpyz(out, buff + 8, 5);
				trap_Cvar_Set("cdkey3", out);
				Q_strncpyz(out, buff + 12, 5);
				trap_Cvar_Set("cdkey4", out);
			}

		}
		else if (Q_strequal(name, "verifyCDKey") ) {
			buff[0] = '\0';
			Q_strcat(buff, 1024, UI_Cvar_VariableString("cdkey1"));
			Q_strcat(buff, 1024, UI_Cvar_VariableString("cdkey2"));
			Q_strcat(buff, 1024, UI_Cvar_VariableString("cdkey3"));
			Q_strcat(buff, 1024, UI_Cvar_VariableString("cdkey4"));
			trap_Cvar_Set("cdkey", buff);
			if (trap_VerifyCDKey(buff, UI_Cvar_VariableString("cdkeychecksum"))) {
				trap_Cvar_Set("ui_cdkeyvalid", "CD Key Appears to be valid.");
				trap_SetCDKey(buff);
			}
			else {
				trap_Cvar_Set("ui_cdkeyvalid", "CD Key does not appear to be valid.");
			}
		}
		else if (Q_strequal(name, "loadArenas") ) {
			UI_LoadArenas();
			UI_MapCountByGameType(qfalse);
			Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, "createserver");
		}
		else if (Q_stricmp(name, "loadArenas") == 0) {
			// Changed RD
			char name[MAX_NAME_LENGTH];
			name[0] = '\0';
			Q_strncpyz(name, "createserver", MAX_NAME_LENGTH);
			UI_LoadArenas();
			UI_MapCountByGameType(qfalse);
			if (String_Parse(args, &name2)) {
				Q_strncpyz(name, name2, MAX_NAME_LENGTH);
			}
			Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, name);
			// end changed RD
		}
		else if (Q_stricmp(name, "saveControls") == 0) {
			Controls_SetConfig(qtrue);
		}
		else if (Q_strequal(name, "loadControls") ) {
			Controls_GetConfig();
		}
		else if (Q_strequal(name, "clearError") ) {
			trap_Cvar_Set("com_errorMessage", "");
		}
		else if (Q_stricmp(name, "loadGameInfo") == 0) {
			UI_ParseGameInfo("gameinfo.txt");
			// Changed RD
			trap_Cvar_Set("ui_currentMap", "0");
			// end changed RD
			UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);
		}
		else if (Q_strequal(name, "resetScores") ) {
			UI_ClearScores();
		}
		else if (Q_stricmp(name, "RefreshServers") == 0) {
			UI_StartServerRefresh(qtrue);
			UI_BuildServerDisplayList(qtrue, qtrue);
		}
		else if (Q_stricmp(name, "RefreshServerStatus") == 0) {
			UI_StartServerRefresh(qtrue);
			UI_BuildServerDisplayList(qtrue, qtrue);
			trap_LAN_GetServerAddressString(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], uiInfo.serverStatusAddress, sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
		}
		else if (Q_stricmp(name, "RefreshFilter") == 0) {
			UI_StartServerRefresh(qfalse);
			UI_BuildServerDisplayList(qtrue, qtrue);
		}
		else if (Q_strequal(name, "RunSPDemo") ) {
			if (uiInfo.demoAvailable) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("demo %s_%i\n", uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum));
			}
		}
		else if (Q_strequal(name, "LoadDemos") ) {
			UI_LoadDemos();
		}
		else if (Q_strequal(name, "LoadMovies") ) {
			UI_LoadMovies();
		}
		else if (Q_strequal(name, "LoadMods") ) {
			UI_LoadMods();
		}
		else if (Q_strequal(name, "playMovie") ) {
			if (uiInfo.previewMovie >= 0) {
				trap_CIN_StopCinematic(uiInfo.previewMovie);
			}
			trap_Cmd_ExecuteText( EXEC_APPEND, va("cinematic %s.roq 2\n", uiInfo.movieList[uiInfo.movieIndex]));
		}
		else if (Q_strequal(name, "RunMod") ) {
			trap_Cvar_Set( "fs_game", uiInfo.modList[uiInfo.modIndex].modName);
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		}
		else if (Q_strequal(name, "RunDemo") ) {
			trap_Cmd_ExecuteText( EXEC_APPEND, va("demo %s\n", uiInfo.demoList[uiInfo.demoIndex]));
		}
		else if (Q_strequal(name, "Quake3") ) {
			trap_Cvar_Set( "fs_game", "");
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		}
		else if (Q_strequal(name, "closeJoin") ) {
			if (uiInfo.serverStatus.refreshActive) {
				UI_StopServerRefresh();
				uiInfo.serverStatus.nextDisplayRefresh = 0;
				uiInfo.nextServerStatusRefresh = 0;
				uiInfo.nextFindPlayerRefresh = 0;
				UI_BuildServerDisplayList(qtrue, qtrue);
			}
			else {
				Menus_CloseByName("joinserver");
				Menus_OpenByName("main");
			}
		}
		else if (Q_strequal(name, "StopRefresh") ) {
			UI_StopServerRefresh();
			uiInfo.serverStatus.nextDisplayRefresh = 0;
			uiInfo.nextServerStatusRefresh = 0;
			uiInfo.nextFindPlayerRefresh = 0;
		}
		else if (Q_strequal(name, "UpdateFilter") ) {
			if (ui_netSource.integer == AS_LOCAL) {
				UI_StartServerRefresh(qtrue);
			}
			UI_BuildServerDisplayList(qtrue, qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_SERVERS, 0, NULL);
			UI_FeederSelection(FEEDER_SERVERS, 0);
		}
		else if (Q_strequal(name, "ServerStatus") ) {
			trap_LAN_GetServerAddressString(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], uiInfo.serverStatusAddress, sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
		}
		else if (Q_strequal(name, "FoundPlayerServerStatus") ) {
			Q_strncpyz(uiInfo.serverStatusAddress, uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer], sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_FINDPLAYER, 0, NULL);
		}
		else if (Q_strequal(name, "FindPlayer") ) {
			UI_BuildFindPlayerList(qtrue);
			// clear the displayed server status info
			uiInfo.serverStatusInfo.numLines = 0;
			Menu_SetFeederSelection(NULL, FEEDER_FINDPLAYER, 0, NULL);
		}
		else if (Q_strequal(name, "JoinServer") ) {
			trap_Cvar_Set("cg_thirdPerson", "0");
			trap_Cvar_Set("cg_cameraOrbit", "0");
			trap_Cvar_Set("ui_singlePlayerActive", "0");
			if (uiInfo.serverStatus.currentServer >= 0 && uiInfo.serverStatus.currentServer < uiInfo.serverStatus.numDisplayServers) {
				trap_LAN_GetServerAddressString(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, 1024);
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", buff ) );
			}
		}
		else if (Q_strequal(name, "FoundPlayerJoinServer") ) {
			trap_Cvar_Set("ui_singlePlayerActive", "0");
			if (uiInfo.currentFoundPlayerServer >= 0 && uiInfo.currentFoundPlayerServer < uiInfo.numFoundPlayerServers) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer] ) );
			}
		}
		else if (Q_strequal(name, "Quit") ) {
			trap_Cvar_Set("ui_singlePlayerActive", "0");
			trap_Cmd_ExecuteText( EXEC_NOW, "quit");
		}
		else if (Q_stricmp(name, "Controls") == 0) {
			// Changed RD
			const char *menus;
			if (String_Parse(args, &menus)) {
				trap_Cvar_Set( "cl_paused", "1" );
				trap_Key_SetCatcher( KEYCATCH_UI );
				Menus_CloseAll();
				Menus_ActivateByName(menus);
				// end changed RD
			}
		}
		else if (Q_stricmp(name, "Leave") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			Menus_CloseAll();
			// Changed RD
			Menus_ActivateByName(uiInfo.uiDC.Assets.mainname);
		}
		else if (Q_stricmp(name, "MaskGameType") == 0) {
			int i, mask;
			for(i=0; i<MAX_GAMETYPES; i++) {
				if (Int_Parse(args, &mask)) {
					uiInfo.maskGameTypes[mask] = 1;
				}
			}
			while (uiInfo.maskGameTypes[uiInfo.gameTypes[ui_gameType.integer].gtEnum]) {
				ui_gameType.integer++;
				if (ui_gameType.integer >= uiInfo.numGameTypes) {
					ui_gameType.integer = 0;
				}
			}
			while (uiInfo.maskGameTypes[uiInfo.gameTypes[ui_netGameType.integer].gtEnum]) {
				ui_netGameType.integer++;
				if (ui_netGameType.integer >= uiInfo.numGameTypes) {
					ui_netGameType.integer = 0;
				}
			}
		}
		else if (Q_stricmp(name, "unMaskGameType") == 0) {
			int i;
			for(i=0; i<MAX_GAMETYPES; i++) {
				uiInfo.maskGameTypes[i] = 0;
			}
			// end changed RD
		}
		else if (Q_stricmp(name, "ServerSort") == 0) {
			int sortColumn;
			if (Int_Parse(args, &sortColumn)) {
				// if same column we're already sorting on then flip the direction
				if (sortColumn == uiInfo.serverStatus.sortKey) {
					uiInfo.serverStatus.sortDir = !uiInfo.serverStatus.sortDir;
				}
				// make sure we sort again
				UI_ServersSort(sortColumn, qtrue);
				Menu_SetFeederSelection(NULL, FEEDER_SERVERS, 0, NULL);
			}
		}
		else if (Q_stricmp(name, "nextSkirmishOld") == 0) {
			UI_StartSkirmish(qtrue, NULL);
		}
		else if (Q_stricmp(name, "nextSkirmish") == 0) {
			// Changed RD
			char name[MAX_NAME_LENGTH];
			name[0] = '\0';
			if (String_Parse(args, &name2)) {
				Q_strncpyz(name, name2, MAX_NAME_LENGTH);
				UI_StartSkirmish(qtrue, name);
			}
			else {
				UI_StartSkirmish(qtrue, "skirmish");
			}
		}
		else if (Q_stricmp(name, "SkirmishStartOld") == 0) {
			UI_StartSkirmish(qfalse, NULL);
		}
		else if (Q_stricmp(name, "SkirmishStart") == 0) {
			UI_StartSkirmish(qfalse, NULL);
			// end changed RD
		}
		else if (Q_stricmp(name, "closeingame") == 0) {
			trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
			trap_Key_ClearStates();
			trap_Cvar_Set( "cl_paused", "0" );
			Menus_CloseAll();
		}
		else if (Q_strequal(name, "voteMap") ) {
			if (ui_currentNetMap.integer >=0 && ui_currentNetMap.integer < uiInfo.mapCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote map %s\n",uiInfo.mapList[ui_currentNetMap.integer].mapLoadName) );
			}
		}
		else if (Q_strequal(name, "voteKick") ) {
			if (uiInfo.playerIndex >= 0 && uiInfo.playerIndex < uiInfo.playerCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote kick %s\n",uiInfo.playerNames[uiInfo.playerIndex]) );
			}
		}
		else if (Q_strequal(name, "voteGame") ) {
			if (ui_netGameType.integer >= 0 && ui_netGameType.integer < uiInfo.numGameTypes) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote g_gametype %i\n",uiInfo.gameTypes[ui_netGameType.integer].gtEnum) );
			}
		}
		else if (Q_strequal(name, "voteLeader") ) {
			if (uiInfo.teamIndex >= 0 && uiInfo.teamIndex < uiInfo.myTeamCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callteamvote leader %s\n",uiInfo.teamNames[uiInfo.teamIndex]) );
			}
		}
		else if (Q_strequal(name, "addBot") ) {
			/* Neon_Knight: Missionpack checks, if != 0, enables this. */
			if (ui_missionpackChecks.integer) {
				if (UI_IsATeamGametype(trap_Cvar_VariableValue("g_gametype"))) {
					trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot %s %i %s\n", UI_GetBotNameByNumber(uiInfo.botIndex), uiInfo.skillIndex+1, (uiInfo.redBlue == 0) ? "Red" : "Blue") );
					}
			} else {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot %s %i %s\n", uiInfo.characterList[uiInfo.botIndex].name, uiInfo.skillIndex+1, (uiInfo.redBlue == 0) ? "Red" : "Blue") );
			}
			// Changed RD
		}
		else if (Q_stricmp(name, "getHostName") == 0) {
			char name[MAX_NAME_LENGTH];
			trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
			name[0] = '\0';
			Q_strncpyz(name, 	Info_ValueForKey(buff, "hostname"), MAX_NAME_LENGTH);
			if (strlen(name) > 0)
				trap_Cvar_Set( "ui_HostName", name );
			else
				trap_Cvar_Set( "ui_HostName", "<empty>" );
			// end changed RD
		}
		else if (Q_stricmp(name, "addFavorite") == 0) {
			if (ui_netSource.integer != AS_FAVORITES) {
				char name[MAX_NAME_LENGTH];
				char addr[MAX_NAME_LENGTH];
				int res;

				trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				name[0] = addr[0] = '\0';
				Q_strncpyz(name, 	Info_ValueForKey(buff, "hostname"), MAX_NAME_LENGTH);
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), MAX_NAME_LENGTH);
				if (strlen(name) > 0 && strlen(addr) > 0) {
					res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
					if (res == 0) {
						// server already in the list
						Com_Printf("Favorite already in list\n");
					}
					else if (res == -1) {
						// list full
						Com_Printf("Favorite list full\n");
					}
					else {
						// successfully added
						Com_Printf("Added favorite server %s\n", addr);
					}
				}
			}
		}
		else if (Q_stricmp(name, "deleteFavorite") == 0) {
			if (ui_netSource.integer == AS_FAVORITES) {
				char addr[MAX_NAME_LENGTH];
				trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				addr[0] = '\0';
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), MAX_NAME_LENGTH);
				if (strlen(addr) > 0) {
					trap_LAN_RemoveServer(AS_FAVORITES, addr);
				}
			}
			// Changed RD
		}
		else if (Q_stricmp(name, "addelFavorite") == 0) {
			if (ui_netSource.integer == AS_FAVORITES) {
				char addr[MAX_NAME_LENGTH];
				trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				addr[0] = '\0';
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), MAX_NAME_LENGTH);
				if (strlen(addr) > 0) {
					trap_LAN_RemoveServer(AS_FAVORITES, addr);
				}
				UI_StartServerRefresh(qtrue);
				UI_BuildServerDisplayList(qtrue, qtrue);
			}
			else {
				char name[MAX_NAME_LENGTH];
				char addr[MAX_NAME_LENGTH];
				int res;

				trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				name[0] = addr[0] = '\0';
				Q_strncpyz(name, 	Info_ValueForKey(buff, "hostname"), MAX_NAME_LENGTH);
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), MAX_NAME_LENGTH);
				if (strlen(name) > 0 && strlen(addr) > 0) {
					res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
					if (res == 0) {
						// server already in the list
						Com_Printf("Favorite already in list\n");
					}
					else if (res == -1) {
						// list full
						Com_Printf("Favorite list full\n");
					}
					else {
						// successfully added
						Com_Printf("Added favorite server %s\n", addr);
					}
				}
			}
			// end changed RD
		}
		else if (Q_stricmp(name, "createFavorite") == 0) {
			if (ui_netSource.integer == AS_FAVORITES) {
				char name[MAX_NAME_LENGTH];
				char addr[MAX_NAME_LENGTH];
				int res;

				name[0] = addr[0] = '\0';
				Q_strncpyz(name, 	UI_Cvar_VariableString("ui_favoriteName"), MAX_NAME_LENGTH);
				Q_strncpyz(addr, 	UI_Cvar_VariableString("ui_favoriteAddress"), MAX_NAME_LENGTH);
				if (strlen(name) > 0 && strlen(addr) > 0) {
					res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
					if (res == 0) {
						// server already in the list
						Com_Printf("Favorite already in list\n");
					}
					else if (res == -1) {
						// list full
						Com_Printf("Favorite list full\n");
					}
					else {
						// successfully added
						Com_Printf("Added favorite server %s\n", addr);
					}
				}
			}
		}
		else if (Q_strequal(name, "orders") ) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer < uiInfo.myTeamCount) {
					Com_sprintf( buff, sizeof( buff ), orders, uiInfo.teamClientNums[selectedPlayer] );
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				}
				else {
					int i;
					for (i = 0; i < uiInfo.myTeamCount; i++) {
						if (uiInfo.playerNumber == uiInfo.teamClientNums[i]) {
							continue;
						}
						Com_sprintf( buff, sizeof( buff ), orders, uiInfo.teamClientNums[i] );
						trap_Cmd_ExecuteText( EXEC_APPEND, buff );
						trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
					}
				}
				trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
				trap_Key_ClearStates();
				trap_Cvar_Set( "cl_paused", "0" );
				Menus_CloseAll();
			}
		}
		else if (Q_strequal(name, "voiceOrdersTeam") ) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer == uiInfo.myTeamCount) {
					trap_Cmd_ExecuteText( EXEC_APPEND, orders );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				}
				trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
				trap_Key_ClearStates();
				trap_Cvar_Set( "cl_paused", "0" );
				Menus_CloseAll();
			}
		}
		else if (Q_strequal(name, "voiceOrders") ) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer < uiInfo.myTeamCount) {
					Com_sprintf( buff, sizeof( buff ), orders, uiInfo.teamClientNums[selectedPlayer] );
					trap_Cmd_ExecuteText( EXEC_APPEND, buff );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				}
				trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
				trap_Key_ClearStates();
				trap_Cvar_Set( "cl_paused", "0" );
				Menus_CloseAll();
			}
		}
		else if (Q_strequal(name, "glCustom") ) {
			trap_Cvar_Set("ui_glCustom", "4");
		}
		else if (Q_strequal(name, "update") ) {
			if (String_Parse(args, &name2)) {
				UI_Update(name2);
			}
		}
		else if (Q_strequal(name, "setPbClStatus") ) {
			int stat;
			if ( Int_Parse( args, &stat ) )
				trap_SetPbClStatus( stat );
		}
		// Changed RD
	}
	else if (Q_stricmp(name, "UpdateModel") == 0) {
		updateModel = qtrue;
	}
	// end changed RD
	else {
		Com_Printf("unknown UI script %s\n", name);
	}


}

static void UI_GetTeamColor(vec4_t *color)
{
}

/*
==================
UI_MapCountByGameType
==================
*/
static int UI_MapCountByGameType(qboolean singlePlayer)
{
	int i, c, game;
	c = 0;
	game = singlePlayer ? uiInfo.gameTypes[ui_gameType.integer].gtEnum : uiInfo.gameTypes[ui_netGameType.integer].gtEnum;
	if (game == GT_SINGLE_PLAYER) {
		game++;
	}
	if (game == GT_TEAM) {
		game = GT_FFA;
	}

	for (i = 0; i < uiInfo.mapCount; i++) {
		uiInfo.mapList[i].active = qfalse;
		if ( uiInfo.mapList[i].typeBits & (1 << game)) {
			if (singlePlayer) {
				if (!(uiInfo.mapList[i].typeBits & (1 << GT_SINGLE_PLAYER))) {
					continue;
				}
			}
			c++;
			uiInfo.mapList[i].active = qtrue;
		}
	}
	return c;
}

qboolean UI_hasSkinForBase(const char *base, const char *team)
{
	char	test[1024];

	Com_sprintf( test, sizeof( test ), "models/players/%s/%s/lower_default.skin", base, team );

	if (trap_FS_FOpenFile(test, NULL, FS_READ)) {
		return qtrue;
	}
	Com_sprintf( test, sizeof( test ), "models/players/characters/%s/%s/lower_default.skin", base, team );

	if (trap_FS_FOpenFile(test, NULL, FS_READ)) {
		return qtrue;
	}
	return qfalse;
}

/*
==================
UI_MapCountByTeam
==================
*/
static int UI_HeadCountByTeam(void)
{
	static int init = 0;
	int i, j, k, c, tIndex;

	c = 0;
	if (!init) {
		for (i = 0; i < uiInfo.characterCount; i++) {
			uiInfo.characterList[i].reference = 0;
			for (j = 0; j < uiInfo.teamCount; j++) {
				if (UI_hasSkinForBase(uiInfo.characterList[i].base, uiInfo.teamList[j].teamName)) {
					uiInfo.characterList[i].reference |= (1<<j);
				}
			}
		}
		init = 1;
	}

	tIndex = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));

	// do names
	for (i = 0; i < uiInfo.characterCount; i++) {
		uiInfo.characterList[i].active = qfalse;
		for(j = 0; j < TEAM_MEMBERS; j++) {
			if (uiInfo.teamList[tIndex].teamMembers[j] != NULL) {
				if (uiInfo.characterList[i].reference&(1<<tIndex)) {// && Q_strequal(uiInfo.teamList[tIndex].teamMembers[j], uiInfo.characterList[i].name)) {
					uiInfo.characterList[i].active = qtrue;
					c++;
					break;
				}
			}
		}
	}

	// and then aliases
	for(j = 0; j < TEAM_MEMBERS; j++) {
		for(k = 0; k < uiInfo.aliasCount; k++) {
			if (uiInfo.aliasList[k].name != NULL) {
				if (Q_strequal(uiInfo.teamList[tIndex].teamMembers[j], uiInfo.aliasList[k].name) ) {
					for (i = 0; i < uiInfo.characterCount; i++) {
						if (uiInfo.characterList[i].headImage != -1 && uiInfo.characterList[i].reference&(1<<tIndex) && Q_strequal(uiInfo.aliasList[k].ai, uiInfo.characterList[i].name) ) {
							if (uiInfo.characterList[i].active == qfalse) {
								uiInfo.characterList[i].active = qtrue;
								c++;
							}
							break;
						}
					}
				}
			}
		}
	}
	return c;
}

/*
==================
UI_InsertServerIntoDisplayList
==================
*/
static void UI_InsertServerIntoDisplayList(int num, int position)
{
	int i;

	if (position < 0 || position > uiInfo.serverStatus.numDisplayServers ) {
		return;
	}
	//
	uiInfo.serverStatus.numDisplayServers++;
	for (i = uiInfo.serverStatus.numDisplayServers; i > position; i--) {
		uiInfo.serverStatus.displayServers[i] = uiInfo.serverStatus.displayServers[i-1];
	}
	uiInfo.serverStatus.displayServers[position] = num;
}

/*
==================
UI_RemoveServerFromDisplayList
==================
*/
static void UI_RemoveServerFromDisplayList(int num)
{
	int i, j;

	for (i = 0; i < uiInfo.serverStatus.numDisplayServers; i++) {
		if (uiInfo.serverStatus.displayServers[i] == num) {
			uiInfo.serverStatus.numDisplayServers--;
			for (j = i; j < uiInfo.serverStatus.numDisplayServers; j++) {
				uiInfo.serverStatus.displayServers[j] = uiInfo.serverStatus.displayServers[j+1];
			}
			return;
		}
	}
}

/*
==================
UI_BinaryServerInsertion
==================
*/
static void UI_BinaryServerInsertion(int num)
{
	int mid, offset, res, len;

	// use binary search to insert server
	len = uiInfo.serverStatus.numDisplayServers;
	mid = len;
	offset = 0;
	res = 0;
	while(mid > 0) {
		mid = len >> 1;
		//
		res = trap_LAN_CompareServers( ui_netSource.integer, uiInfo.serverStatus.sortKey,
		                               uiInfo.serverStatus.sortDir, num, uiInfo.serverStatus.displayServers[offset+mid]);
		// if equal
		if (res == 0) {
			UI_InsertServerIntoDisplayList(num, offset+mid);
			return;
		}
		// if larger
		else if (res == 1) {
			offset += mid;
			len -= mid;
		}
		// if smaller
		else {
			len -= mid;
		}
	}
	if (res == 1) {
		offset++;
	}
	UI_InsertServerIntoDisplayList(num, offset);
}

/*
==================
UI_BuildServerDisplayList
==================
*/
static void UI_BuildServerDisplayList(qboolean force, qboolean doReset)
{
	int i, count, clients, maxClients, ping, game, len, visible;
	char info[MAX_STRING_CHARS];

	if (!(force || uiInfo.uiDC.realTime > uiInfo.serverStatus.nextDisplayRefresh)) {
		return;
	}

	// do motd updates here too
	trap_Cvar_VariableStringBuffer( "cl_motdString", uiInfo.serverStatus.motd, sizeof(uiInfo.serverStatus.motd) );
	len = strlen(uiInfo.serverStatus.motd);
	if (len == 0) {
		strcpy(uiInfo.serverStatus.motd, "Welcome to OpenArena!");
		len = strlen(uiInfo.serverStatus.motd);
	}
	if (len != uiInfo.serverStatus.motdLen) {
		uiInfo.serverStatus.motdLen = len;
		uiInfo.serverStatus.motdWidth = -1;
	}

	if (doReset) {
		// clear number of displayed servers
		uiInfo.serverStatus.numDisplayServers = 0;
		uiInfo.serverStatus.numPlayersOnServers = 0;
		// set list box index to zero
		Menu_SetFeederSelection(NULL, FEEDER_SERVERS, 0, NULL);
		// mark all servers as visible so we store ping updates for them
		trap_LAN_MarkServerVisible(ui_netSource.integer, -1, qtrue);
	}

	// get the server count (comes from the master)
	count = trap_LAN_GetServerCount(ui_netSource.integer);
	if (count == -1 || (ui_netSource.integer == AS_LOCAL && count == 0) ) {
		// still waiting on a response from the master
		uiInfo.serverStatus.numDisplayServers = 0;
		uiInfo.serverStatus.numPlayersOnServers = 0;
		uiInfo.serverStatus.nextDisplayRefresh = uiInfo.uiDC.realTime + 500;
		return;
	}

	visible = qfalse;
	for (i = 0; i < count; i++) {
		// if we already got info for this server
		if (!trap_LAN_ServerIsVisible(ui_netSource.integer, i)) {
			continue;
		}
		visible = qtrue;
		// get the ping for this server
		ping = trap_LAN_GetServerPing(ui_netSource.integer, i);
		if (ping > 0 || ui_netSource.integer == AS_FAVORITES) {

			trap_LAN_GetServerInfo(ui_netSource.integer, i, info, MAX_STRING_CHARS);

			if(trap_Cvar_VariableValue("ui_humansonly"))
				clients = atoi(Info_ValueForKey(info, "g_humanplayers"));
			else
				clients = atoi(Info_ValueForKey(info, "clients"));
			uiInfo.serverStatus.numPlayersOnServers += clients;

			if (ui_browserShowEmpty.integer == 0) {
				if (clients == 0) {
					trap_LAN_MarkServerVisible(ui_netSource.integer, i, qfalse);
					continue;
				}
			}

			if (ui_browserShowFull.integer == 0) {
				maxClients = atoi(Info_ValueForKey(info, "sv_maxclients"));
				if (clients == maxClients) {
					trap_LAN_MarkServerVisible(ui_netSource.integer, i, qfalse);
					continue;
				}
			}

			if (uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum != -1) {
				game = atoi(Info_ValueForKey(info, "gametype"));
				if (game != uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum) {
					trap_LAN_MarkServerVisible(ui_netSource.integer, i, qfalse);
					continue;
				}
			}

			if (ui_serverFilterType.integer > 0) {
				if (Q_stricmp(Info_ValueForKey(info, "game"), serverFilters[ui_serverFilterType.integer].basedir) != 0) {
					trap_LAN_MarkServerVisible(ui_netSource.integer, i, qfalse);
					continue;
				}
			}
			// Changed RD
			else {
				if (Q_stricmp(Info_ValueForKey(info, "game"), UI_Cvar_VariableString("ui_CustomServer")) != 0) {
					trap_LAN_MarkServerVisible(ui_netSource.integer, i, qfalse);
					continue;
				}
			}
			// end changed RD
			// make sure we never add a favorite server twice
			if (ui_netSource.integer == AS_FAVORITES) {
				UI_RemoveServerFromDisplayList(i);
			}
			// insert the server into the list
			UI_BinaryServerInsertion(i);
			// done with this server
			if (ping > 0) {
				trap_LAN_MarkServerVisible(ui_netSource.integer, i, qfalse);
			}
		}
	}

	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime;

	// if there were no servers visible for ping updates
	if (!visible) {
//		UI_StopServerRefresh();
//		uiInfo.serverStatus.nextDisplayRefresh = 0;
	}
}

typedef struct {
	char *name, *altName;
} serverStatusCvar_t;

serverStatusCvar_t serverStatusCvars[] = {
	{"sv_hostname", "Name"},
	{"Address", ""},
	{"gamename", "Game name"},
	{"g_gametype", "Game type"},
	{"mapname", "Map"},
	{"version", ""},
	{"protocol", ""},
	{"timelimit", ""},
	{"fraglimit", ""},
	{"capturelimit", ""},
	{NULL, NULL}
};

/*
==================
UI_SortServerStatusInfo
==================
*/
static void UI_SortServerStatusInfo( serverStatusInfo_t *info )
{
	int i, j, index;
	char *tmp1, *tmp2;

	// FIXME: if "gamename" == "baseq3" or "missionpack" then
	// replace the gametype number by FFA, CTF etc.
	//
	index = 0;
	for (i = 0; serverStatusCvars[i].name; i++) {
		for (j = 0; j < info->numLines; j++) {
			if ( !info->lines[j][1] || info->lines[j][1][0] ) {
				continue;
			}
			if ( Q_strequal(serverStatusCvars[i].name, info->lines[j][0]) ) {
				// swap lines
				tmp1 = info->lines[index][0];
				tmp2 = info->lines[index][3];
				info->lines[index][0] = info->lines[j][0];
				info->lines[index][3] = info->lines[j][3];
				info->lines[j][0] = tmp1;
				info->lines[j][3] = tmp2;
				//
				if ( strlen(serverStatusCvars[i].altName) ) {
					info->lines[index][0] = serverStatusCvars[i].altName;
				}
				index++;
			}
		}
	}
}

/*
==================
UI_GetServerStatusInfo
==================
*/
static int UI_GetServerStatusInfo( const char *serverAddress, serverStatusInfo_t *info )
{
	char *p, *score, *ping, *name;
	int i, len;

	if (!info) {
		trap_LAN_ServerStatus( serverAddress, NULL, 0);
		return qfalse;
	}
	memset(info, 0, sizeof(*info));
	if ( trap_LAN_ServerStatus( serverAddress, info->text, sizeof(info->text)) ) {
		Q_strncpyz(info->address, serverAddress, sizeof(info->address));
		p = info->text;
		info->numLines = 0;
		info->lines[info->numLines][0] = "Address";
		info->lines[info->numLines][1] = "";
		info->lines[info->numLines][2] = "";
		info->lines[info->numLines][3] = info->address;
		info->numLines++;
		// get the cvars
		while (p && *p) {
			p = strchr(p, '\\');
			if (!p) {
				break;
			}
			*p++ = '\0';
			if (*p == '\\') {
				break;
			}
			info->lines[info->numLines][0] = p;
			info->lines[info->numLines][1] = "";
			info->lines[info->numLines][2] = "";
			p = strchr(p, '\\');
			*p++ = '\0';
			info->lines[info->numLines][3] = p;

			info->numLines++;
			if (info->numLines >= MAX_SERVERSTATUS_LINES) {
				break;
			}
		}
		// get the player list
		if (info->numLines < MAX_SERVERSTATUS_LINES-3) {
			// empty line
			info->lines[info->numLines][0] = "";
			info->lines[info->numLines][1] = "";
			info->lines[info->numLines][2] = "";
			info->lines[info->numLines][3] = "";
			info->numLines++;
			// header
			info->lines[info->numLines][0] = "num";
			info->lines[info->numLines][1] = "score";
			info->lines[info->numLines][2] = "ping";
			info->lines[info->numLines][3] = "name";
			info->numLines++;
			// parse players
			i = 0;
			len = 0;
			while (p && *p) {
				if (*p == '\\')
					*p++ = '\0';
				if (!p)
					break;
				score = p;
				p = strchr(p, ' ');
				if (!p)
					break;
				*p++ = '\0';
				ping = p;
				p = strchr(p, ' ');
				if (!p)
					break;
				*p++ = '\0';
				name = p;
				Com_sprintf(&info->pings[len], sizeof(info->pings)-len, "%d", i);
				info->lines[info->numLines][0] = &info->pings[len];
				len += strlen(&info->pings[len]) + 1;
				info->lines[info->numLines][1] = score;
				info->lines[info->numLines][2] = ping;
				info->lines[info->numLines][3] = name;
				info->numLines++;
				if (info->numLines >= MAX_SERVERSTATUS_LINES)
					break;
				p = strchr(p, '\\');
				if (!p)
					break;
				*p++ = '\0';
				//
				i++;
			}
		}
		UI_SortServerStatusInfo( info );
		return qtrue;
	}
	return qfalse;
}

/*
==================
stristr
==================
*/
static char *stristr(char *str, char *charset)
{
	int i;

	while(*str) {
		for (i = 0; charset[i] && str[i]; i++) {
			if (toupper(charset[i]) != toupper(str[i])) break;
		}
		if (!charset[i]) return str;
		str++;
	}
	return NULL;
}

/*
==================
UI_BuildFindPlayerList
==================
*/
static void UI_BuildFindPlayerList(qboolean force)
{
	static int numFound, numTimeOuts;
	int i, j, resend;
	serverStatusInfo_t info;
	char name[MAX_NAME_LENGTH+2];
	char infoString[MAX_STRING_CHARS];

	if (!force) {
		if (!uiInfo.nextFindPlayerRefresh || uiInfo.nextFindPlayerRefresh > uiInfo.uiDC.realTime) {
			return;
		}
	}
	else {
		memset(&uiInfo.pendingServerStatus, 0, sizeof(uiInfo.pendingServerStatus));
		uiInfo.numFoundPlayerServers = 0;
		uiInfo.currentFoundPlayerServer = 0;
		trap_Cvar_VariableStringBuffer( "ui_findPlayer", uiInfo.findPlayerName, sizeof(uiInfo.findPlayerName));
		Q_CleanStr(uiInfo.findPlayerName);
		// should have a string of some length
		if (!strlen(uiInfo.findPlayerName)) {
			uiInfo.nextFindPlayerRefresh = 0;
			return;
		}
		// set resend time
		resend = ui_serverStatusTimeOut.integer / 2 - 10;
		if (resend < 50) {
			resend = 50;
		}
		trap_Cvar_Set("cl_serverStatusResendTime", va("%d", resend));
		// reset all server status requests
		trap_LAN_ServerStatus( NULL, NULL, 0);
		//
		uiInfo.numFoundPlayerServers = 1;
		Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
		            sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
		            "searching %d...", uiInfo.pendingServerStatus.num);
		numFound = 0;
		numTimeOuts++;
	}
	for (i = 0; i < MAX_SERVERSTATUSREQUESTS; i++) {
		// if this pending server is valid
		if (uiInfo.pendingServerStatus.server[i].valid) {
			// try to get the server status for this server
			if (UI_GetServerStatusInfo( uiInfo.pendingServerStatus.server[i].adrstr, &info ) ) {
				//
				numFound++;
				// parse through the server status lines
				for (j = 0; j < info.numLines; j++) {
					// should have ping info
					if ( !info.lines[j][2] || !info.lines[j][2][0] ) {
						continue;
					}
					// clean string first
					Q_strncpyz(name, info.lines[j][3], sizeof(name));
					Q_CleanStr(name);
					// if the player name is a substring
					if (stristr(name, uiInfo.findPlayerName)) {
						// add to found server list if we have space (always leave space for a line with the number found)
						if (uiInfo.numFoundPlayerServers < MAX_FOUNDPLAYER_SERVERS-1) {
							//
							Q_strncpyz(uiInfo.foundPlayerServerAddresses[uiInfo.numFoundPlayerServers-1],
							           uiInfo.pendingServerStatus.server[i].adrstr,
							           sizeof(uiInfo.foundPlayerServerAddresses[0]));
							Q_strncpyz(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
							           uiInfo.pendingServerStatus.server[i].name,
							           sizeof(uiInfo.foundPlayerServerNames[0]));
							uiInfo.numFoundPlayerServers++;
						}
						else {
							// can't add any more so we're done
							uiInfo.pendingServerStatus.num = uiInfo.serverStatus.numDisplayServers;
						}
					}
				}
				Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
				            sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
				            "searching %d/%d...", uiInfo.pendingServerStatus.num, numFound);
				// retrieved the server status so reuse this spot
				uiInfo.pendingServerStatus.server[i].valid = qfalse;
			}
		}
		// if empty pending slot or timed out
		if (!uiInfo.pendingServerStatus.server[i].valid ||
		        uiInfo.pendingServerStatus.server[i].startTime < uiInfo.uiDC.realTime - ui_serverStatusTimeOut.integer) {
			if (uiInfo.pendingServerStatus.server[i].valid) {
				numTimeOuts++;
			}
			// reset server status request for this address
			UI_GetServerStatusInfo( uiInfo.pendingServerStatus.server[i].adrstr, NULL );
			// reuse pending slot
			uiInfo.pendingServerStatus.server[i].valid = qfalse;
			// if we didn't try to get the status of all servers in the main browser yet
			if (uiInfo.pendingServerStatus.num < uiInfo.serverStatus.numDisplayServers) {
				uiInfo.pendingServerStatus.server[i].startTime = uiInfo.uiDC.realTime;
				trap_LAN_GetServerAddressString(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.pendingServerStatus.num],
				                                uiInfo.pendingServerStatus.server[i].adrstr, sizeof(uiInfo.pendingServerStatus.server[i].adrstr));
				trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[uiInfo.pendingServerStatus.num], infoString, sizeof(infoString));
				Q_strncpyz(uiInfo.pendingServerStatus.server[i].name, Info_ValueForKey(infoString, "hostname"), sizeof(uiInfo.pendingServerStatus.server[0].name));
				uiInfo.pendingServerStatus.server[i].valid = qtrue;
				uiInfo.pendingServerStatus.num++;
				Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
				            sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
				            "searching %d/%d...", uiInfo.pendingServerStatus.num, numFound);
			}
		}
	}
	for (i = 0; i < MAX_SERVERSTATUSREQUESTS; i++) {
		if (uiInfo.pendingServerStatus.server[i].valid) {
			break;
		}
	}
	// if still trying to retrieve server status info
	if (i < MAX_SERVERSTATUSREQUESTS) {
		uiInfo.nextFindPlayerRefresh = uiInfo.uiDC.realTime + 25;
	}
	else {
		// add a line that shows the number of servers found
		if (!uiInfo.numFoundPlayerServers) {
			Com_sprintf(uiInfo.foundPlayerServerNames[0], sizeof(uiInfo.foundPlayerServerNames[0]), "no servers found");		}
		else {
			Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1], sizeof(uiInfo.foundPlayerServerAddresses[0]),
			            "%d server%s found with player %s", uiInfo.numFoundPlayerServers-1,
			            uiInfo.numFoundPlayerServers == 2 ? "":"s", uiInfo.findPlayerName);
		}
		uiInfo.nextFindPlayerRefresh = 0;
		// show the server status info for the selected server
		UI_FeederSelection(FEEDER_FINDPLAYER, uiInfo.currentFoundPlayerServer);
	}
}

/*
==================
UI_BuildServerStatus
==================
*/
static void UI_BuildServerStatus(qboolean force)
{

	if (uiInfo.nextFindPlayerRefresh) {
		return;
	}
	if (!force) {
		if (!uiInfo.nextServerStatusRefresh || uiInfo.nextServerStatusRefresh > uiInfo.uiDC.realTime) {
			return;
		}
	}
	else {
		Menu_SetFeederSelection(NULL, FEEDER_SERVERSTATUS, 0, NULL);
		uiInfo.serverStatusInfo.numLines = 0;
		// reset all server status requests
		trap_LAN_ServerStatus( NULL, NULL, 0);
	}
	if (uiInfo.serverStatus.currentServer < 0 || uiInfo.serverStatus.currentServer > uiInfo.serverStatus.numDisplayServers || uiInfo.serverStatus.numDisplayServers == 0) {
		return;
	}
	if (UI_GetServerStatusInfo( uiInfo.serverStatusAddress, &uiInfo.serverStatusInfo ) ) {
		uiInfo.nextServerStatusRefresh = 0;
		UI_GetServerStatusInfo( uiInfo.serverStatusAddress, NULL );
	}
	else {
		uiInfo.nextServerStatusRefresh = uiInfo.uiDC.realTime + 500;
	}
}

/*
==================
UI_FeederCount
==================
*/
static int UI_FeederCount(float feederID)
{
	if (feederID == FEEDER_HEADS) {
		return UI_HeadCountByTeam();
	}
	else if (feederID == FEEDER_Q3HEADS) {
		return uiInfo.q3HeadCount;
	}
	else if (feederID == FEEDER_CINEMATICS) {
		return uiInfo.movieCount;
	}
	else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
		return UI_MapCountByGameType(feederID == FEEDER_MAPS ? qtrue : qfalse);
	}
	else if (feederID == FEEDER_SERVERS) {
		return uiInfo.serverStatus.numDisplayServers;
	}
	else if (feederID == FEEDER_SERVERSTATUS) {
		return uiInfo.serverStatusInfo.numLines;
	}
	else if (feederID == FEEDER_FINDPLAYER) {
		return uiInfo.numFoundPlayerServers;
	}
	else if (feederID == FEEDER_PLAYER_LIST) {
		if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
			uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
			UI_BuildPlayerList();
		}
		return uiInfo.playerCount;
	}
	else if (feederID == FEEDER_TEAM_LIST) {
		if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
			uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
			UI_BuildPlayerList();
		}
		return uiInfo.myTeamCount;
	}
	else if (feederID == FEEDER_MODS) {
		return uiInfo.modCount;
	}
	else if (feederID == FEEDER_DEMOS) {
		return uiInfo.demoCount;
// Changed RD
	}
	else if (feederID == FEEDER_SAVE) {
		return uiInfo.saveCount;
// end changed RD
	}
	return 0;
}

static const char *UI_SelectedMap(int index, int *actual)
{
	int i, c;
	c = 0;
	*actual = 0;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			if (c == index) {
				*actual = i;
				return uiInfo.mapList[i].mapName;
			}
			else {
				c++;
			}
		}
	}
	return "";
}

static const char *UI_SelectedHead(int index, int *actual)
{
	int i, c;
	c = 0;
	*actual = 0;
	for (i = 0; i < uiInfo.characterCount; i++) {
		if (uiInfo.characterList[i].active) {
			if (c == index) {
				*actual = i;
				return uiInfo.characterList[i].name;
			}
			else {
				c++;
			}
		}
	}
	return "";
}

static int UI_GetIndexFromSelection(int actual)
{
	int i, c;
	c = 0;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			if (i == actual) {
				return c;
			}
			c++;
		}
	}
	return 0;
}

static void UI_UpdatePendingPings( void )
{
	trap_LAN_ResetPings(ui_netSource.integer);
	uiInfo.serverStatus.refreshActive = qtrue;
	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;

}

static const char *UI_FeederItemText(float feederID, int index, int column, qhandle_t *handle)
{
	static char info[MAX_STRING_CHARS];
	static char hostname[1024];
	static char clientBuff[32];
	static int lastColumn = -1;
	static int lastTime = 0;
	*handle = -1;
	if (feederID == FEEDER_HEADS) {
		int actual;
		return UI_SelectedHead(index, &actual);
	}
	else if (feederID == FEEDER_Q3HEADS) {
		if (index >= 0 && index < uiInfo.q3HeadCount) {
			return uiInfo.q3HeadNames[index];
		}
	}
	else if (feederID == FEEDER_Q3HEADS_FULL) {
		if (index >= 0 && index < uiInfo.q3HeadCount2) {
			return uiInfo.q3HeadNames2[index];
		}
	}
	else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
		int actual;
		return UI_SelectedMap(index, &actual);
	}
	else if (feederID == FEEDER_SERVERS) {
		if (index >= 0 && index < uiInfo.serverStatus.numDisplayServers) {
			int ping, game;
			if (lastColumn != column || lastTime > uiInfo.uiDC.realTime + 5000) {
				trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);
				lastColumn = column;
				lastTime = uiInfo.uiDC.realTime;
			}
			ping = atoi(Info_ValueForKey(info, "ping"));
			if (ping == -1) {
				// if we ever see a ping that is out of date, do a server refresh
				// UI_UpdatePendingPings();
			}
			switch (column) {
			case SORT_HOST :
				if (ping <= 0) {
					return Info_ValueForKey(info, "addr");
				}
				else {
					if ( ui_netSource.integer == AS_LOCAL ) {
						Com_sprintf( hostname, sizeof(hostname), "%s [%s]",
						             Info_ValueForKey(info, "hostname"),
						             netnames[atoi(Info_ValueForKey(info, "nettype"))] );
						return hostname;
					}
					else {
						Com_sprintf( hostname, sizeof(hostname), "%s", Info_ValueForKey(info, "hostname"));
						return hostname;
					}
				}
			case SORT_MAP :
				return Info_ValueForKey(info, "mapname");
			case SORT_CLIENTS :
				Com_sprintf( clientBuff, sizeof(clientBuff), "%s (%s)", trap_Cvar_VariableValue("ui_humansonly")? Info_ValueForKey(info, "g_humanplayers") : Info_ValueForKey(info, "clients"), Info_ValueForKey(info, "sv_maxclients"));
				return clientBuff;
			case SORT_GAME :
				game = atoi(Info_ValueForKey(info, "gametype"));
				if (game >= 0 && game < numTeamArenaGameTypes) {
					return teamArenaGameTypes[game];
				}
				else {
					return "Unknown";
				}
			case SORT_PING :
				if (ping <= 0) {
					return "...";
				}
				else {
					return Info_ValueForKey(info, "ping");
				}
			}
		}
	}
	else if (feederID == FEEDER_SERVERSTATUS) {
		if ( index >= 0 && index < uiInfo.serverStatusInfo.numLines ) {
			if ( column >= 0 && column < 4 ) {
				return uiInfo.serverStatusInfo.lines[index][column];
			}
		}
	}
	else if (feederID == FEEDER_FINDPLAYER) {
		if ( index >= 0 && index < uiInfo.numFoundPlayerServers ) {
			//return uiInfo.foundPlayerServerAddresses[index];
			return uiInfo.foundPlayerServerNames[index];
		}
	}
	else if (feederID == FEEDER_PLAYER_LIST) {
		if (index >= 0 && index < uiInfo.playerCount) {
			return uiInfo.playerNames[index];
		}
	}
	else if (feederID == FEEDER_TEAM_LIST) {
		if (index >= 0 && index < uiInfo.myTeamCount) {
			return uiInfo.teamNames[index];
		}
	}
	else if (feederID == FEEDER_MODS) {
		if (index >= 0 && index < uiInfo.modCount) {
			if (uiInfo.modList[index].modDescr && *uiInfo.modList[index].modDescr) {
				return uiInfo.modList[index].modDescr;
			}
			else {
				return uiInfo.modList[index].modName;
			}
		}
	}
	else if (feederID == FEEDER_CINEMATICS) {
		if (index >= 0 && index < uiInfo.movieCount) {
			return uiInfo.movieList[index];
		}
	}
	else if (feederID == FEEDER_DEMOS) {
		if (index >= 0 && index < uiInfo.demoCount) {
			return uiInfo.demoList[index];
		}
	}
	return "";
}


static qhandle_t UI_FeederItemImage(float feederID, int index)
{
	if (feederID == FEEDER_HEADS) {
		int actual;
		UI_SelectedHead(index, &actual);
		index = actual;
		if (index >= 0 && index < uiInfo.characterCount) {
			if (uiInfo.characterList[index].headImage == -1) {
				uiInfo.characterList[index].headImage = trap_R_RegisterShaderNoMip(uiInfo.characterList[index].imageName);
			}
			return uiInfo.characterList[index].headImage;
		}
	}
	else if (feederID == FEEDER_Q3HEADS) {
		if (index >= 0 && index < uiInfo.q3HeadCount) {
			return uiInfo.q3HeadIcons[index];
		}
	}
	else if (feederID == FEEDER_Q3HEADS_FULL) {
		if (index >= 0 && index < uiInfo.q3HeadCount2) {
			return uiInfo.q3HeadIcons2[index];
		}
	}
	else if (feederID == FEEDER_ALLMAPS || feederID == FEEDER_MAPS) {
		int actual;
		UI_SelectedMap(index, &actual);
		index = actual;
		if (index >= 0 && index < uiInfo.mapCount) {
			if (uiInfo.mapList[index].levelShot == -1) {
				uiInfo.mapList[index].levelShot = trap_R_RegisterShaderNoMip(uiInfo.mapList[index].imageName);
			}
			return uiInfo.mapList[index].levelShot;
		}
	}
	return 0;
}

static void UI_FeederSelection(float feederID, int index)
{
	static char info[MAX_STRING_CHARS];
	if (feederID == FEEDER_HEADS) {
		int actual;
		UI_SelectedHead(index, &actual);
		index = actual;
		if (index >= 0 && index < uiInfo.characterCount) {
			trap_Cvar_Set( "team_model", va("%s", uiInfo.characterList[index].base));
			trap_Cvar_Set( "team_headmodel", va("*%s", uiInfo.characterList[index].name));
			updateModel = qtrue;
		}
	}
	else if (feederID == FEEDER_Q3HEADS) {
		if (index >= 0 && index < uiInfo.q3HeadCount) {
			trap_Cvar_Set( "model", uiInfo.q3HeadNames[index]);
			trap_Cvar_Set( "headmodel", uiInfo.q3HeadNames[index]);
			updateModel = qtrue;
		}
	}
	else if (feederID == FEEDER_Q3HEADS_FULL) {
		if (index >= 0 && index < uiInfo.q3HeadCount2) {
			trap_Cvar_Set( "model", uiInfo.q3HeadNames2[index]);
			trap_Cvar_Set( "headmodel", uiInfo.q3HeadNames2[index]);
			updateModel = qtrue;
		}
	}
	else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
		int actual, map;
		map = (feederID == FEEDER_ALLMAPS) ? ui_currentNetMap.integer : ui_currentMap.integer;
		if (uiInfo.mapList[map].cinematic >= 0) {
			trap_CIN_StopCinematic(uiInfo.mapList[map].cinematic);
			uiInfo.mapList[map].cinematic = -1;
		}
		UI_SelectedMap(index, &actual);
		trap_Cvar_Set("ui_mapIndex", va("%d", index));
		ui_mapIndex.integer = index;

		if (feederID == FEEDER_MAPS) {
			ui_currentMap.integer = actual;
			trap_Cvar_Set("ui_currentMap", va("%d", actual));
			uiInfo.mapList[ui_currentMap.integer].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[ui_currentMap.integer].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
			UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);
			trap_Cvar_Set("ui_opponentModel", uiInfo.mapList[ui_currentMap.integer].opponentName);
			updateOpponentModel = qtrue;
		}
		else {
			ui_currentNetMap.integer = actual;
			trap_Cvar_Set("ui_currentNetMap", va("%d", actual));
			uiInfo.mapList[ui_currentNetMap.integer].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[ui_currentNetMap.integer].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}

	}
	else if (feederID == FEEDER_SERVERS) {
		const char *mapName = NULL;
		uiInfo.serverStatus.currentServer = index;
		trap_LAN_GetServerInfo(ui_netSource.integer, uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);
		uiInfo.serverStatus.currentServerPreview = trap_R_RegisterShaderNoMip(va("levelshots/%s", Info_ValueForKey(info, "mapname")));
		if (uiInfo.serverStatus.currentServerCinematic >= 0) {
			trap_CIN_StopCinematic(uiInfo.serverStatus.currentServerCinematic);
			uiInfo.serverStatus.currentServerCinematic = -1;
		}
		mapName = Info_ValueForKey(info, "mapname");
		if (mapName && *mapName) {
			uiInfo.serverStatus.currentServerCinematic = trap_CIN_PlayCinematic(va("%s.roq", mapName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}
	}
	else if (feederID == FEEDER_SERVERSTATUS) {
		//
	}
	else if (feederID == FEEDER_FINDPLAYER) {
		uiInfo.currentFoundPlayerServer = index;
		//
		if ( index < uiInfo.numFoundPlayerServers-1) {
			// build a new server status for this server
			Q_strncpyz(uiInfo.serverStatusAddress, uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer], sizeof(uiInfo.serverStatusAddress));
			Menu_SetFeederSelection(NULL, FEEDER_SERVERSTATUS, 0, NULL);
			UI_BuildServerStatus(qtrue);
		}
	}
	else if (feederID == FEEDER_PLAYER_LIST) {
		uiInfo.playerIndex = index;
	}
	else if (feederID == FEEDER_TEAM_LIST) {
		uiInfo.teamIndex = index;
	}
	else if (feederID == FEEDER_MODS) {
		uiInfo.modIndex = index;
	}
	else if (feederID == FEEDER_CINEMATICS) {
		uiInfo.movieIndex = index;
		if (uiInfo.previewMovie >= 0) {
			trap_CIN_StopCinematic(uiInfo.previewMovie);
		}
		uiInfo.previewMovie = -1;
	}
	else if (feederID == FEEDER_DEMOS) {
		uiInfo.demoIndex = index;
	}
}

static qboolean Team_Parse(char **p)
{
	char *token;
	const char *tempStr;
	int i;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	while ( 1 ) {

		token = COM_ParseExt(p, qtrue);

		if (Q_strequal(token, "}") ) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			if (uiInfo.teamCount == MAX_TEAMS) {
				uiInfo.teamCount--;
				Com_Printf("Too many teams, last team replaced!\n");
			}
			// seven tokens per line, team name and icon, and 5 team member names
			if (!String_Parse(p, &uiInfo.teamList[uiInfo.teamCount].teamName) || !String_Parse(p, &tempStr)) {
				return qfalse;
			}


			uiInfo.teamList[uiInfo.teamCount].imageName = tempStr;
			uiInfo.teamList[uiInfo.teamCount].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[uiInfo.teamCount].imageName);
			uiInfo.teamList[uiInfo.teamCount].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[uiInfo.teamCount].imageName));
			uiInfo.teamList[uiInfo.teamCount].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[uiInfo.teamCount].imageName));

			uiInfo.teamList[uiInfo.teamCount].cinematic = -1;

			for (i = 0; i < TEAM_MEMBERS; i++) {
				uiInfo.teamList[uiInfo.teamCount].teamMembers[i] = NULL;
				if (!String_Parse(p, &uiInfo.teamList[uiInfo.teamCount].teamMembers[i])) {
					return qfalse;
				}
			}

			Com_Printf("Loaded team %s with team icon %s.\n", uiInfo.teamList[uiInfo.teamCount].teamName, tempStr);
			uiInfo.teamCount++;

			token = COM_ParseExt(p, qtrue);
			if (token[0] != '}') {
				return qfalse;
			}
		}
	}

	return qfalse;
}

static qboolean Character_Parse(char **p)
{
	char *token;
	const char *tempStr;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}


	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);

		if (Q_strequal(token, "}") ) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			if (uiInfo.characterCount == MAX_HEADS) {
				uiInfo.characterCount--;
				Com_Printf("Too many characters, last character replaced!\n");
			}

			// two tokens per line, character name and sex
			if (!String_Parse(p, &uiInfo.characterList[uiInfo.characterCount].name) || !String_Parse(p, &tempStr)) {
				return qfalse;
			}

			uiInfo.characterList[uiInfo.characterCount].headImage = -1;
			uiInfo.characterList[uiInfo.characterCount].imageName = String_Alloc(va("models/players/heads/%s/icon_default.tga", uiInfo.characterList[uiInfo.characterCount].name));

			if (tempStr && Q_strequal(tempStr, "female")) {
				uiInfo.characterList[uiInfo.characterCount].base = String_Alloc(va("kyonshi"));
			}
			else if (tempStr && Q_strequal(tempStr, "male")) {
				uiInfo.characterList[uiInfo.characterCount].base = String_Alloc(va("sergei"));
			}
			else {
				uiInfo.characterList[uiInfo.characterCount].base = String_Alloc(va("%s",tempStr));
			}

			Com_Printf("Loaded %s character %s.\n", uiInfo.characterList[uiInfo.characterCount].base, uiInfo.characterList[uiInfo.characterCount].name);
			uiInfo.characterCount++;

			token = COM_ParseExt(p, qtrue);
			if (token[0] != '}') {
				return qfalse;
			}
		}
	}

	return qfalse;
}


static qboolean Alias_Parse(char **p)
{
	char *token;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);

		if (Q_strequal(token, "}") ) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			if (uiInfo.aliasCount == MAX_ALIASES) {
				uiInfo.aliasCount--;
				Com_Printf("Too many aliases, last alias replaced!\n");
			}

			// three tokens per line, character name, bot alias, and preferred action a - all purpose, d - defense, o - offense
			if (!String_Parse(p, &uiInfo.aliasList[uiInfo.aliasCount].name) || !String_Parse(p, &uiInfo.aliasList[uiInfo.aliasCount].ai) || !String_Parse(p, &uiInfo.aliasList[uiInfo.aliasCount].action)) {
				return qfalse;
			}

			Com_Printf("Loaded character alias %s using character ai %s.\n", uiInfo.aliasList[uiInfo.aliasCount].name, uiInfo.aliasList[uiInfo.aliasCount].ai);
			uiInfo.aliasCount++;

			token = COM_ParseExt(p, qtrue);
			if (token[0] != '}') {
				return qfalse;
			}
		}
	}

	return qfalse;
}



// mode
// 0 - high level parsing
// 1 - team parsing
// 2 - character parsing
static void UI_ParseTeamInfo(const char *teamFile)
{
	char	*token;
	char *p;
	char *buff = NULL;
	//static int mode = 0; TTimo: unused

	buff = GetMenuBuffer(teamFile);
	if (!buff) {
		return;
	}

	p = buff;

	while ( 1 ) {
		token = COM_ParseExt( &p, qtrue );
		if( !token || token[0] == 0 || token[0] == '}') {
			break;
		}

		if ( Q_strequal( token, "}" ) ) {
			break;
		}

		if (Q_strequal(token, "teams") ) {

			if (Team_Parse(&p)) {
				continue;
			}
			else {
				break;
			}
		}

		if (Q_strequal(token, "characters") ) {
			Character_Parse(&p);
		}

		if (Q_strequal(token, "aliases") ) {
			Alias_Parse(&p);
		}

	}

}


static qboolean GameType_Parse(char **p, qboolean join)
{
	char *token;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	if (join) {
		uiInfo.numJoinGameTypes = 0;
	}
	else {
		uiInfo.numGameTypes = 0;
	}

	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);

		if (Q_strequal(token, "}") ) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			// two tokens per line, character name and sex
			if (join) {
				if (!String_Parse(p, &uiInfo.joinGameTypes[uiInfo.numJoinGameTypes].gameType) || !Int_Parse(p, &uiInfo.joinGameTypes[uiInfo.numJoinGameTypes].gtEnum)) {
					return qfalse;
				}
			}
			else {
				if (!String_Parse(p, &uiInfo.gameTypes[uiInfo.numGameTypes].gameType) || !Int_Parse(p, &uiInfo.gameTypes[uiInfo.numGameTypes].gtEnum)) {
					return qfalse;
				}
			}

			if (join) {
				if (uiInfo.numJoinGameTypes < MAX_GAMETYPES) {
					uiInfo.numJoinGameTypes++;
				}
				else {
					Com_Printf("Too many net game types, last one replace!\n");
				}
			}
			else {
				if (uiInfo.numGameTypes < MAX_GAMETYPES) {
					uiInfo.numGameTypes++;
				}
				else {
					Com_Printf("Too many game types, last one replace!\n");
				}
			}

			token = COM_ParseExt(p, qtrue);
			if (token[0] != '}') {
				return qfalse;
			}
		}
	}
	return qfalse;
}

static qboolean MapList_Parse(char **p)
{
	char *token;

	token = COM_ParseExt(p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	uiInfo.mapCount = 0;

	while ( 1 ) {
		token = COM_ParseExt(p, qtrue);

		if (Q_strequal(token, "}") ) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			if (!String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].mapName) || !String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].mapLoadName)
			        ||!Int_Parse(p, &uiInfo.mapList[uiInfo.mapCount].teamMembers) ) {
				return qfalse;
			}

			if (!String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].opponentName)) {
				return qfalse;
			}

			uiInfo.mapList[uiInfo.mapCount].typeBits = 0;

			while (1) {
				token = COM_ParseExt(p, qtrue);
				if (token[0] >= '0' && token[0] <= '9') {
					uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << (token[0] - 0x030));
					if (!Int_Parse(p, &uiInfo.mapList[uiInfo.mapCount].timeToBeat[token[0] - 0x30])) {
						return qfalse;
					}
				}
				else {
					break;
				}
			}

			//mapList[mapCount].imageName = String_Alloc(va("levelshots/%s", mapList[mapCount].mapLoadName));
			//if (uiInfo.mapCount == 0) {
			// only load the first cinematic, selection loads the others
			//  uiInfo.mapList[uiInfo.mapCount].cinematic = trap_CIN_PlayCinematic(va("%s.roq",uiInfo.mapList[uiInfo.mapCount].mapLoadName), qfalse, qfalse, qtrue, 0, 0, 0, 0);
			//}
			uiInfo.mapList[uiInfo.mapCount].cinematic = -1;
			//uiInfo.mapList[uiInfo.mapCount].levelShot = trap_R_RegisterShaderNoMip(va("levelshots/%s_small", uiInfo.mapList[uiInfo.mapCount].mapLoadName));
			// leilei - We don't have _small levelshots, memory reasons
			//if (!uiInfo.mapList[uiInfo.mapCount].levelShot)
			uiInfo.mapList[uiInfo.mapCount].levelShot = trap_R_RegisterShaderNoMip(va("levelshots/%s", uiInfo.mapList[uiInfo.mapCount].mapLoadName));

			if (uiInfo.mapCount < MAX_MAPS) {
				uiInfo.mapCount++;
			}
			else {
				Com_Printf("Too many maps, last one replaced!\n");
			}
		}
	}
	return qfalse;
}

static void UI_ParseGameInfo(const char *teamFile)
{
	char	*token;
	char *p;
	char *buff = NULL;
	//int mode = 0; TTimo: unused

	buff = GetMenuBuffer(teamFile);
	if (!buff) {
		return;
	}

	p = buff;

	while ( 1 ) {
		token = COM_ParseExt( &p, qtrue );
		if( !token || token[0] == 0 || token[0] == '}') {
			break;
		}

		if ( Q_strequal( token, "}" )) {
			break;
		}

		if (Q_strequal(token, "gametypes") ) {

			if (GameType_Parse(&p, qfalse)) {
				continue;
			}
			else {
				break;
			}
		}

		if (Q_strequal(token, "joingametypes") ) {

			if (GameType_Parse(&p, qtrue)) {
				continue;
			}
			else {
				break;
			}
		}

		if (Q_strequal(token, "maps") ) {
			// start a new menu
			MapList_Parse(&p);
		}

	}
}

static void UI_Pause(qboolean b)
{
	if (b) {
		// pause the game and set the ui keycatcher
		trap_Cvar_Set( "cl_paused", "1" );
		trap_Key_SetCatcher( KEYCATCH_UI );
	}
	else {
		// unpause the game and clear the ui keycatcher
		trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
		trap_Key_ClearStates();
		trap_Cvar_Set( "cl_paused", "0" );
	}
}

#ifndef MISSIONPACK // bk001206
static int UI_OwnerDraw_Width(int ownerDraw)
{
	// bk001205 - LCC missing return value
	return 0;
}
#endif

static int UI_PlayCinematic(const char *name, float x, float y, float w, float h)
{
	return trap_CIN_PlayCinematic(name, x, y, w, h, (CIN_loop | CIN_silent));
}

static void UI_StopCinematic(int handle)
{
	if (handle >= 0) {
		trap_CIN_StopCinematic(handle);
	}
	else {
		handle = abs(handle);
		if (handle == UI_MAPCINEMATIC) {
			if (uiInfo.mapList[ui_currentMap.integer].cinematic >= 0) {
				trap_CIN_StopCinematic(uiInfo.mapList[ui_currentMap.integer].cinematic);
				uiInfo.mapList[ui_currentMap.integer].cinematic = -1;
			}
		}
		else if (handle == UI_NETMAPCINEMATIC) {
			if (uiInfo.serverStatus.currentServerCinematic >= 0) {
				trap_CIN_StopCinematic(uiInfo.serverStatus.currentServerCinematic);
				uiInfo.serverStatus.currentServerCinematic = -1;
			}
		}
		else if (handle == UI_CLANCINEMATIC) {
			int i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
			if (i >= 0 && i < uiInfo.teamCount) {
				if (uiInfo.teamList[i].cinematic >= 0) {
					trap_CIN_StopCinematic(uiInfo.teamList[i].cinematic);
					uiInfo.teamList[i].cinematic = -1;
				}
			}
		}
	}
}

static void UI_DrawCinematic(int handle, float x, float y, float w, float h)
{
	trap_CIN_SetExtents(handle, x, y, w, h);
	trap_CIN_DrawCinematic(handle);
}

static void UI_RunCinematicFrame(int handle)
{
	trap_CIN_RunCinematic(handle);
}



/*
=================
PlayerModel_BuildList
=================
*/
static void UI_BuildQ3Model_List( void )
{
	int		numdirs;
	int		numfiles;
	char	dirlist[2048];
	char	filelist[2048];
	char	skinname[MAX_QPATH];
	char	scratch[256];
	char*	dirptr;
	char*	fileptr;
	int		i;
	int		j, k, dirty;
	int		dirlen;
	int		filelen;

	uiInfo.q3HeadCount = 0;
	uiInfo.q3HeadCount2 = 0;

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players", "/", dirlist, 32768 ); // upped from 2048
	dirptr  = dirlist;
	for (i=0; i<numdirs && uiInfo.q3HeadCount < MAX_PLAYERMODELS; i++,dirptr+=dirlen+1) {
		dirlen = strlen(dirptr);

		if (dirlen && dirptr[dirlen-1]=='/') dirptr[dirlen-1]='\0';

		if ( strequals(dirptr,".") || strequals(dirptr,"..")) {
			continue;
		}

		// iterate all skin files in directory
		numfiles = trap_FS_GetFileList( va("models/players/%s",dirptr), "tga", filelist, 2048 );
		fileptr  = filelist;
		for (j=0; j<numfiles && uiInfo.q3HeadCount < MAX_PLAYERMODELS; j++,fileptr+=filelen+1) {
			filelen = strlen(fileptr);

			COM_StripExtension(fileptr, skinname, sizeof(skinname));

			// look for icon_????
			if (Q_stricmpn(skinname, "icon_", 5) == 0 && !(Q_stricmp(skinname,"icon_blue") == 0 || Q_stricmp(skinname,"icon_red") == 0))
				//if (Q_stricmpn(skinname, "icon_", 5) == 0)	// leilei - show red blue anyway

			{
				if (Q_stricmp(skinname, "icon_default") == 0) {
					Com_sprintf( scratch, sizeof(scratch), "%s", dirptr);
				}
				else {
					Com_sprintf( scratch, sizeof(scratch), "%s/%s",dirptr, skinname + 5);
				}
				dirty = 0;
				for(k=0; k<uiInfo.q3HeadCount; k++) {
					if (!Q_stricmp(scratch, uiInfo.q3HeadNames[uiInfo.q3HeadCount])) {
						dirty = 1;
						break;
					}
				}
				if (!dirty) {
					Com_sprintf( uiInfo.q3HeadNames[uiInfo.q3HeadCount], sizeof(uiInfo.q3HeadNames[uiInfo.q3HeadCount]), "%s", scratch);
					uiInfo.q3HeadIcons[uiInfo.q3HeadCount++] = trap_R_RegisterShaderNoMip(va("models/players/%s/%s",dirptr,skinname));
				}
			}

		}

		// Text Only list

		for (j=0; j<numfiles && uiInfo.q3HeadCount2 < MAX_PLAYERMODELS; j++,fileptr+=filelen+1) {
			filelen = strlen(fileptr);

			COM_StripExtension(fileptr, skinname, sizeof(skinname));

			// look for icon_????
			if (Q_stricmpn(skinname, "icon_", 5) == 0 && !(Q_stricmp(skinname,"icon_blue") == 0 || Q_stricmp(skinname,"icon_red") == 0))
				//if (Q_stricmpn(skinname, "icon_", 5) == 0)	// leilei - show red blue anyway

			{
				if (Q_stricmp(skinname, "icon_default") == 0) {
					Com_sprintf( scratch, sizeof(scratch), "%s", dirptr);
				}
				else {
					Com_sprintf( scratch, sizeof(scratch), "%s/%s",dirptr, skinname + 5);
				}
				dirty = 0;
				for(k=0; k<uiInfo.q3HeadCount2; k++) {
					if (!Q_stricmp(scratch, uiInfo.q3HeadNames2[uiInfo.q3HeadCount2])) {
						dirty = 1;
						break;
					}
				}
				if (!dirty) {
					Com_sprintf( uiInfo.q3HeadNames2[uiInfo.q3HeadCount2], sizeof(uiInfo.q3HeadNames2[uiInfo.q3HeadCount2]), "%s", scratch);
					uiInfo.q3HeadIcons[uiInfo.q3HeadCount2++] = trap_R_RegisterShaderNoMip(va("models/players/%s/%s",dirptr,skinname));
				}
			}

		}
	}

}

int realVidWidth;
int realVidHeight;
int wideAdjustX; // leilei - dirty widescreen hack

float realxscale, realyscale;

/*
=================
UI_Init
=================
*/
void _UI_Init( qboolean inGameLoad, int randomSeed )
{
	const char *menuSet;
	// Changed RD
	char model[MAX_QPATH];
	char team[256];
	char head[256];
	int i;
	// end changed RD

	srand( randomSeed );

	//uiInfo.inGameLoad = inGameLoad;

	UI_RegisterCvars();
	UI_InitMemory();

	// cache redundant calulations
	trap_GetGlconfig( &uiInfo.uiDC.glconfig );

	// for 640x480 virtualized screen
	uiInfo.uiDC.yscale = uiInfo.uiDC.glconfig.vidHeight * (1.0/480.0);
	uiInfo.uiDC.xscale = uiInfo.uiDC.glconfig.vidWidth * (1.0/640.0);

	realxscale = uiInfo.uiDC.xscale;
	realyscale = uiInfo.uiDC.yscale;

	realVidWidth = uiInfo.uiDC.glconfig.vidWidth;
	realVidHeight = uiInfo.uiDC.glconfig.vidHeight;

	// leilei - wide adjust calculation for things!!!


	{
		float resbias;
		float rex, rey;
		int newresx, newresy;

		rex = 640.0f / realVidWidth;
		rey = 480.0f / realVidHeight;

		newresx = 640.0f * (rex);
		newresy = 480.0f * (rey);

		newresx = realVidWidth * rey;
		newresy = realVidHeight * rey;

		resbias  = 0.5 * ( newresx -  ( newresy * (640.0/480.0) ) );

		wideAdjustX = resbias;

	}


	if ( uiInfo.uiDC.glconfig.vidWidth * 480 > uiInfo.uiDC.glconfig.vidHeight * 640 ) {
		// wide screen
		uiInfo.uiDC.bias = 0.5 * ( uiInfo.uiDC.glconfig.vidWidth - ( uiInfo.uiDC.glconfig.vidHeight * (640.0/480.0) ) );
		uiInfo.uiDC.xscale = uiInfo.uiDC.yscale;	 // leilei - widescreen adjust
	}
	else {
		// no wide screen
		uiInfo.uiDC.bias = 0;
	}


	//UI_Load();
	uiInfo.uiDC.registerShaderNoMip = &trap_R_RegisterShaderNoMip;
	uiInfo.uiDC.setColor = &UI_SetColor;
	uiInfo.uiDC.drawHandlePic = &UI_DrawHandlePic;
	uiInfo.uiDC.drawStretchPic = &trap_R_DrawStretchPic;
	uiInfo.uiDC.drawText = &Text_Paint;
	uiInfo.uiDC.textWidth = &Text_Width;
	uiInfo.uiDC.textHeight = &Text_Height;
	uiInfo.uiDC.registerModel = &trap_R_RegisterModel;
	uiInfo.uiDC.modelBounds = &trap_R_ModelBounds;
	uiInfo.uiDC.fillRect = &UI_FillRect;
	uiInfo.uiDC.drawRect = &_UI_DrawRect;
	uiInfo.uiDC.drawSides = &_UI_DrawSides;
	uiInfo.uiDC.drawTopBottom = &_UI_DrawTopBottom;
	uiInfo.uiDC.clearScene = &trap_R_ClearScene;
	uiInfo.uiDC.drawSides = &_UI_DrawSides;
	uiInfo.uiDC.addRefEntityToScene = &trap_R_AddRefEntityToScene;
	uiInfo.uiDC.renderScene = &trap_R_RenderScene;
	uiInfo.uiDC.registerFont = &trap_R_RegisterFont;
	uiInfo.uiDC.ownerDrawItem = &UI_OwnerDraw;
	uiInfo.uiDC.getValue = &UI_GetValue;
	uiInfo.uiDC.ownerDrawVisible = &UI_OwnerDrawVisible;
	uiInfo.uiDC.runScript = &UI_RunMenuScript;
	uiInfo.uiDC.getTeamColor = &UI_GetTeamColor;
	uiInfo.uiDC.setCVar = trap_Cvar_Set;
	uiInfo.uiDC.getCVarString = trap_Cvar_VariableStringBuffer;
	uiInfo.uiDC.getCVarValue = trap_Cvar_VariableValue;
	uiInfo.uiDC.drawTextWithCursor = &Text_PaintWithCursor;
	uiInfo.uiDC.setOverstrikeMode = &trap_Key_SetOverstrikeMode;
	uiInfo.uiDC.getOverstrikeMode = &trap_Key_GetOverstrikeMode;
	uiInfo.uiDC.startLocalSound = &trap_S_StartLocalSound;
	uiInfo.uiDC.ownerDrawHandleKey = &UI_OwnerDrawHandleKey;
	uiInfo.uiDC.feederCount = &UI_FeederCount;
	uiInfo.uiDC.feederItemImage = &UI_FeederItemImage;
	uiInfo.uiDC.feederItemText = &UI_FeederItemText;
	uiInfo.uiDC.feederSelection = &UI_FeederSelection;
	uiInfo.uiDC.setBinding = &trap_Key_SetBinding;
	uiInfo.uiDC.getBindingBuf = &trap_Key_GetBindingBuf;
	uiInfo.uiDC.keynumToStringBuf = &trap_Key_KeynumToStringBuf;
	uiInfo.uiDC.executeText = &trap_Cmd_ExecuteText;
	uiInfo.uiDC.Error = &Com_Error;
	uiInfo.uiDC.Print = &Com_Printf;
	uiInfo.uiDC.Pause = &UI_Pause;
	uiInfo.uiDC.ownerDrawWidth = &UI_OwnerDrawWidth;
	uiInfo.uiDC.registerSound = &trap_S_RegisterSound;
	uiInfo.uiDC.startBackgroundTrack = &trap_S_StartBackgroundTrack;
	uiInfo.uiDC.stopBackgroundTrack = &trap_S_StopBackgroundTrack;
	uiInfo.uiDC.playCinematic = &UI_PlayCinematic;
	uiInfo.uiDC.stopCinematic = &UI_StopCinematic;
	uiInfo.uiDC.drawCinematic = &UI_DrawCinematic;
	uiInfo.uiDC.runCinematicFrame = &UI_RunCinematicFrame;

	Init_Display(&uiInfo.uiDC);

	String_Init();

	uiInfo.uiDC.cursor	= trap_R_RegisterShaderNoMip( "menu/art/3_cursor2" );
	uiInfo.uiDC.whiteShader = trap_R_RegisterShaderNoMip( "white" );

	AssetCache();

	uiInfo.teamCount = 0;
	uiInfo.characterCount = 0;
	uiInfo.aliasCount = 0;

#ifdef PRE_RELEASE_TADEMO
	UI_ParseTeamInfo("demoteaminfo.txt");
	UI_ParseGameInfo("demogameinfo.txt");
#else
	UI_ParseTeamInfo("teaminfo.txt");
	UI_LoadTeams();
	UI_ParseGameInfo("gameinfo.txt");
#endif

	menuSet = UI_Cvar_VariableString("ui_menuFiles");
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/menus.txt";
	}

#if 0
	if (uiInfo.inGameLoad) {
		UI_LoadMenus("ui/ingame.txt", qtrue);
	}
	else {   // bk010222: left this: UI_LoadMenus(menuSet, qtrue);
	}
#else
	UI_LoadMenus(menuSet, qtrue);
	UI_LoadMenus("ui/ingame.txt", qfalse);
#endif

	Menus_CloseAll();

	trap_LAN_LoadCachedServers();
	UI_LoadBestScores(uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum);

	UI_BuildQ3Model_List();
	UI_LoadBots();

	// sets defaults for ui temp cvars
	uiInfo.effectsColor = gamecodetoui[(int)trap_Cvar_VariableValue("color1")-1];
	uiInfo.currentCrosshair = (int)trap_Cvar_VariableValue("cg_drawCrosshair");
	trap_Cvar_Set("ui_mousePitch", (trap_Cvar_VariableValue("m_pitch") >= 0) ? "0" : "1");

	uiInfo.serverStatus.currentServerCinematic = -1;
	uiInfo.previewMovie = -1;

	if (trap_Cvar_VariableValue("ui_TeamArenaFirstRun") == 0) {
		trap_Cvar_Set("s_volume", "0.8");
		trap_Cvar_Set("s_musicvolume", "0.5");
		trap_Cvar_Set("ui_TeamArenaFirstRun", "1");
	}

	trap_Cvar_Register(NULL, "debug_protocol", "", 0 );

	trap_Cvar_Set("ui_actualNetGameType", va("%d", ui_netGameType.integer));

// Changed RD
	trap_Cvar_Set("ui_LowerAnim", va("%d", LEGS_IDLE));
	trap_Cvar_Set("ui_UpperAnim", va("%d", TORSO_STAND));
	trap_Cvar_Set("ui_Weapon", va("%d", WP_MACHINEGUN));
	trap_Cvar_Set("ui_PlayerViewAngleYaw", "165");
	trap_Cvar_Set("ui_PlayerViewAnglePitch", "10");
	trap_Cvar_Set("ui_PlayerMoveAngleYaw", "165");
	trap_Cvar_Set("ui_PlayerMoveAnglePitch", "0");
	strcpy(model, UI_Cvar_VariableString("model"));
	strcpy(head, UI_Cvar_VariableString("headmodel"));
	UI_PlayerInfo_SetModel( &uiInfo.info, model, head, team);
	for(i=0; i<MAX_GAMETYPES; i++) {
		uiInfo.maskGameTypes[i] = 0;
	}
// end changed RD
}


/*
=================
UI_KeyEvent
=================
*/
void _UI_KeyEvent( int key, qboolean down )
{

	if (Menu_Count() > 0) {
		menuDef_t *menu = Menu_GetFocused();
		if (menu) {
			if ((key == K_ESCAPE || key == K_JOY3)  && down && !Menus_AnyFullScreenVisible()) {
				Menus_CloseAll();
			}
			else {
				Menu_HandleKey(menu, key, down );
			}
		}
		else {
			trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
			trap_Key_ClearStates();
			trap_Cvar_Set( "cl_paused", "0" );
		}
	}

	//if ((s > 0) && (s != menu_null_sound)) {
	//  trap_S_StartLocalSound( s, CHAN_LOCAL_SOUND );
	//}
}

/*
=================
UI_MouseEvent
=================
*/
void _UI_MouseEvent( int dx, int dy )
{
	// update mouse screen position
	uiInfo.uiDC.cursorx += dx;
	/*
		if (uiInfo.uiDC.cursorx < 0)
			uiInfo.uiDC.cursorx = 0;
		else if (uiInfo.uiDC.cursorx > SCREEN_WIDTH)
			uiInfo.uiDC.cursorx = SCREEN_WIDTH;

		uiInfo.uiDC.cursory += dy;
		if (uiInfo.uiDC.cursory < 0)
			uiInfo.uiDC.cursory = 0;
		else if (uiInfo.uiDC.cursory > SCREEN_HEIGHT)
			uiInfo.uiDC.cursory = SCREEN_HEIGHT;
	*/

// leilei - try to adjust the limits

	if (uiInfo.uiDC.cursorx < -wideAdjustX)
		uiInfo.uiDC.cursorx = -wideAdjustX;
	else if (uiInfo.uiDC.cursorx > SCREEN_WIDTH + wideAdjustX)
		uiInfo.uiDC.cursorx = SCREEN_WIDTH  + wideAdjustX;

	uiInfo.uiDC.cursory += dy;
	if (uiInfo.uiDC.cursory < 0)
		uiInfo.uiDC.cursory = 0;
	else if (uiInfo.uiDC.cursory > SCREEN_HEIGHT)
		uiInfo.uiDC.cursory = SCREEN_HEIGHT;

	if (Menu_Count() > 0) {
		//menuDef_t *menu = Menu_GetFocused();
		//Menu_HandleMouseMove(menu, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory);
		Display_MouseMove(NULL, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory);
	}

}

void UI_LoadNonIngame( void )
{
	const char *menuSet = UI_Cvar_VariableString("ui_menuFiles");
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/menus.txt";
	}
	UI_LoadMenus(menuSet, qfalse);
	uiInfo.inGameLoad = qfalse;
}

void _UI_SetActiveMenu( uiMenuCommand_t menu )
{
	char buf[256];

	// this should be the ONLY way the menu system is brought up
	// enusure minumum menu data is cached
	if (Menu_Count() > 0) {
		vec3_t v;
		v[0] = v[1] = v[2] = 0;
		switch ( menu ) {
		case UIMENU_NONE:
			trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
			trap_Key_ClearStates();
			trap_Cvar_Set( "cl_paused", "0" );
			Menus_CloseAll();

			return;
		case UIMENU_MAIN:
			trap_Cvar_Set( "sv_killserver", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			//trap_S_StartLocalSound( trap_S_RegisterSound("sound/misc/menu_background.wav", qfalse) , CHAN_LOCAL_SOUND );
			//trap_S_StartBackgroundTrack("sound/misc/menu_background.wav", NULL);
			if (uiInfo.inGameLoad) {
				UI_LoadNonIngame();
			}
			Menus_CloseAll();
			// Changed RD
			Menus_ActivateByName(uiInfo.uiDC.Assets.mainname);
			// end changed RD
			//Menus_ActivateByName("main");
			trap_Cvar_VariableStringBuffer("com_errorMessage", buf, sizeof(buf));
			if (strlen(buf)) {
				if (!ui_singlePlayerActive.integer) {
					// Changed RD
					Menus_ActivateByName(uiInfo.uiDC.Assets.errorname);
					// end changed RD
				}
				else {
					trap_Cvar_Set("com_errorMessage", "");
				}
			}
			// ai script
			if(trap_Cvar_VariableValue("gameover")==1) {
				if(Menus_FindByName(uiInfo.uiDC.Assets.endname))
					Menus_ActivateByName(uiInfo.uiDC.Assets.endname);
				else
					Menus_ActivateByName(uiInfo.uiDC.Assets.mainname);
			}
			// end ai script
			return;
		case UIMENU_TEAM:
			trap_Key_SetCatcher( KEYCATCH_UI );
			// Changed RD
			Menus_ActivateByName(uiInfo.uiDC.Assets.teamname);
			// end changed RD
			return;
		case UIMENU_NEED_CD:
			// no cd check in TA
			//trap_Key_SetCatcher( KEYCATCH_UI );
			//Menus_ActivateByName("needcd");
			//UI_ConfirmMenu( "Insert the CD", NULL, NeedCDAction );
			return;
		case UIMENU_BAD_CD_KEY:
			// no cd check in TA
			//trap_Key_SetCatcher( KEYCATCH_UI );
			//Menus_ActivateByName("badcd");
			//UI_ConfirmMenu( "Bad CD Key", NULL, NeedCDKeyAction );
			return;
		case UIMENU_POSTGAME:
			trap_Cvar_Set( "sv_killserver", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			if (uiInfo.inGameLoad) {
				UI_LoadNonIngame();
			}
			Menus_CloseAll();
			// Changed RD
			Menus_ActivateByName(uiInfo.uiDC.Assets.endname);
			// end changed RD
			//UI_ConfirmMenu( "Bad CD Key", NULL, NeedCDKeyAction );
			return;
		case UIMENU_INGAME:
			trap_Cvar_Set( "cl_paused", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			UI_BuildPlayerList();
			Menus_CloseAll();
			// Changed RD
			Menus_ActivateByName(uiInfo.uiDC.Assets.ingamename);
			// end changed RD
			return;
		}
	}
}

qboolean _UI_IsFullscreen( void )
{
	return Menus_AnyFullScreenVisible();
}



static connstate_t	lastConnState;
static char			lastLoadingText[MAX_INFO_VALUE];

static void UI_ReadableSize ( char *buf, int bufsize, int value )
{
	if (value > 1024*1024*1024 ) { // gigs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d GB",
		             (value % (1024*1024*1024))*100 / (1024*1024*1024) );
	}
	else if (value > 1024*1024 ) {   // megs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d MB",
		             (value % (1024*1024))*100 / (1024*1024) );
	}
	else if (value > 1024 ) {   // kilos
		Com_sprintf( buf, bufsize, "%d KB", value / 1024 );
	}
	else {   // bytes
		Com_sprintf( buf, bufsize, "%d bytes", value );
	}
}

// Assumes time is in msec
static void UI_PrintTime ( char *buf, int bufsize, int time )
{
	time /= 1000;  // change to seconds

	if (time > 3600) { // in the hours range
		Com_sprintf( buf, bufsize, "%d hr %d min", time / 3600, (time % 3600) / 60 );
	}
	else if (time > 60) {   // mins
		Com_sprintf( buf, bufsize, "%d min %d sec", time / 60, time % 60 );
	}
	else  {   // secs
		Com_sprintf( buf, bufsize, "%d sec", time );
	}
}

void Text_PaintCenter(float x, float y, float scale, vec4_t color, const char *text, float adjust)
{
	int len = Text_Width(text, scale, 0);
	Text_Paint(x - len / 2, y, scale, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
}

void Text_PaintCenter_AutoWrapped(float x, float y, float xmax, float ystep, float scale, vec4_t color, const char *str, float adjust)
{
	int width;
	char *s1,*s2,*s3;
	char c_bcp;
	char buf[1024];

	if (!str || str[0]=='\0')
		return;

	Q_strncpyz(buf, str, sizeof(buf));
	s1 = s2 = s3 = buf;

	while (1) {
		do {
			s3++;
		}
		while (*s3!=' ' && *s3!='\0');
		c_bcp = *s3;
		*s3 = '\0';
		width = Text_Width(s1, scale, 0);
		*s3 = c_bcp;
		if (width > xmax) {
			if (s1==s2) {
				// fuck, don't have a clean cut, we'll overflow
				s2 = s3;
			}
			*s2 = '\0';
			Text_PaintCenter(x, y, scale, color, s1, adjust);
			y += ystep;
			if (c_bcp == '\0') {
				// that was the last word
				// we could start a new loop, but that wouldn't be much use
				// even if the word is too long, we would overflow it (see above)
				// so just print it now if needed
				s2++;
				if (*s2 != '\0') // if we are printing an overflowing line we have s2 == s3
					Text_PaintCenter(x, y, scale, color, s2, adjust);
				break;
			}
			s2++;
			s1 = s2;
			s3 = s2;
		}
		else {
			s2 = s3;
			if (c_bcp == '\0') { // we reached the end
				Text_PaintCenter(x, y, scale, color, s1, adjust);
				break;
			}
		}
	}
}

static void UI_DisplayDownloadInfo( const char *downloadName, float centerPoint, float yStart, float scale )
{
	static char dlText[]	= "Downloading:";
	static char etaText[]	= "Estimated time left:";
	static char xferText[]	= "Transfer rate:";

	int downloadSize, downloadCount, downloadTime;
	char dlSizeBuf[64], totalSizeBuf[64], xferRateBuf[64], dlTimeBuf[64];
	int xferRate;
	int leftWidth;
	const char *s;

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	leftWidth = 320;

	UI_SetColor(colorWhite);
	Text_PaintCenter(centerPoint, yStart + 112, scale, colorWhite, dlText, 0);
	Text_PaintCenter(centerPoint, yStart + 192, scale, colorWhite, etaText, 0);
	Text_PaintCenter(centerPoint, yStart + 248, scale, colorWhite, xferText, 0);

	if (downloadSize > 0) {
		s = va( "%s (%d%%)", downloadName,
		        (int)( (float)downloadCount * 100.0f / downloadSize ) );
	}
	else {
		s = downloadName;
	}

	Text_PaintCenter(centerPoint, yStart+136, scale, colorWhite, s, 0);

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );

	if (downloadCount < 4096 || !downloadTime) {
		Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, "estimating", 0);
		Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s of %s copied)", dlSizeBuf, totalSizeBuf), 0);
	}
	else {
		if ((uiInfo.uiDC.realTime - downloadTime) / 1000) {
			xferRate = downloadCount / ((uiInfo.uiDC.realTime - downloadTime) / 1000);
		}
		else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if (downloadSize && xferRate) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs

			// We do it in K (/1024) because we'd overflow around 4MB
			UI_PrintTime ( dlTimeBuf, sizeof dlTimeBuf,
			               (n - (((downloadCount/1024) * n) / (downloadSize/1024))) * 1000);

			Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, dlTimeBuf, 0);
			Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s of %s copied)", dlSizeBuf, totalSizeBuf), 0);
		}
		else {
			Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, "estimating", 0);
			if (downloadSize) {
				Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s of %s copied)", dlSizeBuf, totalSizeBuf), 0);
			}
			else {
				Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s copied)", dlSizeBuf), 0);
			}
		}

		if (xferRate) {
			Text_PaintCenter(leftWidth, yStart+272, scale, colorWhite, va("%s/Sec", xferRateBuf), 0);
		}
	}
}

/*
========================
UI_DrawConnectScreen

This will also be overlaid on the cgame info screen during loading
to prevent it from blinking away too rapidly on local or lan games.
========================
*/
void UI_DrawConnectScreen( qboolean overlay )
{
	char			*s;
	uiClientState_t	cstate;
	char			info[MAX_INFO_VALUE];
	char text[256];
	float centerPoint, yStart, scale;

	menuDef_t *menu = Menus_FindByName("Connect");


	if ( !overlay && menu ) {
		Menu_Paint(menu, qtrue);
	}

	if (!overlay) {
		centerPoint = 320;
		yStart = 130;
		scale = 0.5f;
	}
	else {
		centerPoint = 320;
		yStart = 32;
		scale = 0.6f;
		return;
	}

	// see what information we should display
	trap_GetClientState( &cstate );

	info[0] = '\0';
	if( trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) ) ) {
		Text_PaintCenter(centerPoint, yStart, scale, colorWhite, va( "Loading %s", Info_ValueForKey( info, "mapname" )), 0);
	}

	if (Q_strequal(cstate.servername,"localhost")) {
		Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite, va("Starting up..."), ITEM_TEXTSTYLE_SHADOWEDMORE);
	}
	else {
		Com_sprintf(text, sizeof(text), "Connecting to %s", cstate.servername);
		Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite,text , ITEM_TEXTSTYLE_SHADOWEDMORE);
	}

	// display global MOTD at bottom
	Text_PaintCenter(centerPoint, 600, scale, colorWhite, Info_ValueForKey( cstate.updateInfoString, "motd" ), 0);
	// print any server info (server full, bad version, etc)
	if ( cstate.connState < CA_CONNECTED ) {
		Text_PaintCenter_AutoWrapped(centerPoint, yStart + 176, 630, 20, scale, colorWhite, cstate.messageString, 0);
	}

	if ( lastConnState > cstate.connState ) {
		lastLoadingText[0] = '\0';
	}
	lastConnState = cstate.connState;

	switch ( cstate.connState ) {
	case CA_CONNECTING:
		s = va("Awaiting connection...%i", cstate.connectPacketCount);
		break;
	case CA_CHALLENGING:
		s = va("Awaiting challenge...%i", cstate.connectPacketCount);
		break;
	case CA_CONNECTED: {
		char downloadName[MAX_INFO_VALUE];

		trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );
		if (*downloadName) {
			UI_DisplayDownloadInfo( downloadName, centerPoint, yStart, scale );
			return;
		}
	}
	s = "Awaiting gamestate...";
	break;
	case CA_LOADING:
		return;
	case CA_PRIMED:
		return;
	default:
		return;
	}


	if ( !Q_strequal(cstate.servername,"localhost")) {
		Text_PaintCenter(centerPoint, yStart + 80, scale, colorWhite, s, 0);
	}

	// password required / connection rejected information goes here
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

vmCvar_t	ui_ffa_fraglimit;
vmCvar_t	ui_ffa_timelimit;

vmCvar_t	ui_tourney_fraglimit;
vmCvar_t	ui_tourney_timelimit;

vmCvar_t	ui_team_fraglimit;
vmCvar_t	ui_team_timelimit;
vmCvar_t	ui_team_friendly;

vmCvar_t	ui_ctf_capturelimit;
vmCvar_t	ui_ctf_timelimit;
vmCvar_t	ui_ctf_friendly;

vmCvar_t	ui_1fctf_capturelimit;
vmCvar_t	ui_1fctf_timelimit;
vmCvar_t	ui_1fctf_friendly;

vmCvar_t	ui_overload_capturelimit;
vmCvar_t	ui_overload_timelimit;
vmCvar_t	ui_overload_friendly;

vmCvar_t	ui_harvester_capturelimit;
vmCvar_t	ui_harvester_timelimit;
vmCvar_t	ui_harvester_friendly;

vmCvar_t	ui_elimination_capturelimit;
vmCvar_t	ui_elimination_timelimit;

vmCvar_t	ui_ctf_elimination_capturelimit;
vmCvar_t	ui_ctf_elimination_timelimit;

vmCvar_t	ui_lms_fraglimit;
vmCvar_t	ui_lms_timelimit;

vmCvar_t	ui_dd_capturelimit;
vmCvar_t	ui_dd_timelimit;
vmCvar_t	ui_dd_friendly;

vmCvar_t	ui_dom_capturelimit;
vmCvar_t	ui_dom_timelimit;
vmCvar_t	ui_dom_friendly;

vmCvar_t	ui_pos_scorelimit;
vmCvar_t	ui_pos_timelimit;

vmCvar_t	ui_arenasFile;
vmCvar_t	ui_botsFile;
vmCvar_t	ui_spScores1;
vmCvar_t	ui_spScores2;
vmCvar_t	ui_spScores3;
vmCvar_t	ui_spScores4;
vmCvar_t	ui_spScores5;
vmCvar_t	ui_spAwards;
vmCvar_t	ui_spVideos;
vmCvar_t	ui_spSkill;

vmCvar_t	ui_spSelection;

vmCvar_t	ui_browserMaster;
vmCvar_t	ui_browserGameType;
vmCvar_t	ui_browserSortKey;
vmCvar_t	ui_browserShowFull;
vmCvar_t	ui_browserShowEmpty;

vmCvar_t	ui_brassTime;
vmCvar_t	ui_drawCrosshair;
vmCvar_t	ui_drawCrosshairNames;
vmCvar_t	ui_marks;

vmCvar_t	ui_server1;
vmCvar_t	ui_server2;
vmCvar_t	ui_server3;
vmCvar_t	ui_server4;
vmCvar_t	ui_server5;
vmCvar_t	ui_server6;
vmCvar_t	ui_server7;
vmCvar_t	ui_server8;
vmCvar_t	ui_server9;
vmCvar_t	ui_server10;
vmCvar_t	ui_server11;
vmCvar_t	ui_server12;
vmCvar_t	ui_server13;
vmCvar_t	ui_server14;
vmCvar_t	ui_server15;
vmCvar_t	ui_server16;

vmCvar_t	ui_cdkeychecked;

vmCvar_t	ui_redteam;
vmCvar_t	ui_redteam1;
vmCvar_t	ui_redteam2;
vmCvar_t	ui_redteam3;
vmCvar_t	ui_redteam4;
vmCvar_t	ui_redteam5;
vmCvar_t	ui_blueteam;
vmCvar_t	ui_blueteam1;
vmCvar_t	ui_blueteam2;
vmCvar_t	ui_blueteam3;
vmCvar_t	ui_blueteam4;
vmCvar_t	ui_blueteam5;
vmCvar_t	ui_teamName;
vmCvar_t	ui_dedicated;
vmCvar_t	ui_gameType;
vmCvar_t	ui_netGameType;
vmCvar_t	ui_actualNetGameType;
vmCvar_t	ui_joinGameType;
vmCvar_t	ui_netSource;
vmCvar_t	ui_serverFilterType;
vmCvar_t	ui_opponentName;
vmCvar_t	ui_menuFiles;
vmCvar_t	ui_currentTier;
vmCvar_t	ui_currentMap;
vmCvar_t	ui_currentNetMap;
vmCvar_t	ui_mapIndex;
vmCvar_t	ui_currentOpponent;
vmCvar_t	ui_selectedPlayer;
vmCvar_t	ui_selectedPlayerName;
vmCvar_t	ui_lastServerRefresh_0;
vmCvar_t	ui_lastServerRefresh_1;
vmCvar_t	ui_lastServerRefresh_2;
vmCvar_t	ui_lastServerRefresh_3;
vmCvar_t	ui_singlePlayerActive;
vmCvar_t	ui_scoreAccuracy;
vmCvar_t	ui_scoreImpressives;
vmCvar_t	ui_scoreExcellents;
vmCvar_t	ui_scoreCaptures;
vmCvar_t	ui_scoreDefends;
vmCvar_t	ui_scoreAssists;
vmCvar_t	ui_scoreGauntlets;
vmCvar_t	ui_scoreScore;
vmCvar_t	ui_scorePerfect;
vmCvar_t	ui_scoreTeam;
vmCvar_t	ui_scoreBase;
vmCvar_t	ui_scoreTimeBonus;
vmCvar_t	ui_scoreSkillBonus;
vmCvar_t	ui_scoreShutoutBonus;
vmCvar_t	ui_scoreTime;
vmCvar_t	ui_captureLimit;
vmCvar_t	ui_fragLimit;
vmCvar_t	ui_smallFont;
vmCvar_t	ui_bigFont;
vmCvar_t	ui_findPlayer;
vmCvar_t	ui_Q3Model;
vmCvar_t	ui_hudFiles;
vmCvar_t	ui_recordSPDemo;
vmCvar_t	ui_realCaptureLimit;
vmCvar_t	ui_realWarmUp;
vmCvar_t	ui_serverStatusTimeOut;
// Changed RD
vmCvar_t	ui_LowerAnim;
vmCvar_t	ui_UpperAnim;
vmCvar_t	ui_Weapon;
vmCvar_t	ui_PlayerViewAngleYaw;
vmCvar_t	ui_PlayerViewAnglePitch;
vmCvar_t	ui_PlayerMoveAngleYaw;
vmCvar_t	ui_PlayerMoveAnglePitch;
vmCvar_t	ui_SaveGame;
vmCvar_t	ui_LoadGame;
vmCvar_t	ui_CustomServer;
vmCvar_t	ui_HostName;
vmCvar_t	ui_SpecialGame;
vmCvar_t	ui_TeamMembers;
vmCvar_t	ui_loading;
vmCvar_t	ui_transitionkey;
vmCvar_t	ui_applychanges;
vmCvar_t	Save_Loading;
vmCvar_t	persid;
vmCvar_t	gameover; // ai script
// end changed RD

// leilei
vmCvar_t	ui_introPlayed;

vmCvar_t	ui_colors;

vmCvar_t ui_humansonly;

vmCvar_t ui_missionpackChecks;

// bk001129 - made static to avoid aliasing
static cvarTable_t		cvarTable[] = {
	{ &ui_ffa_fraglimit, "ui_ffa_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_ffa_timelimit, "ui_ffa_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_tourney_fraglimit, "ui_tourney_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_tourney_timelimit, "ui_tourney_timelimit", "15", CVAR_ARCHIVE },

	{ &ui_team_fraglimit, "ui_team_fraglimit", "0", CVAR_ARCHIVE },
	{ &ui_team_timelimit, "ui_team_timelimit", "20", CVAR_ARCHIVE },
	{ &ui_team_friendly, "ui_team_friendly",  "1", CVAR_ARCHIVE },

	{ &ui_ctf_capturelimit, "ui_ctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_timelimit, "ui_ctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_ctf_friendly, "ui_ctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_1fctf_capturelimit, "ui_1fctf_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_1fctf_timelimit, "ui_1fctf_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_1fctf_friendly, "ui_1fctf_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_overload_capturelimit, "ui_overload_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_overload_timelimit, "ui_overload_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_overload_friendly, "ui_overload_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_harvester_capturelimit, "ui_harvester_capturelimit", "20", CVAR_ARCHIVE },
	{ &ui_harvester_timelimit, "ui_harvester_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_harvester_friendly, "ui_harvester_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_elimination_capturelimit, "ui_elimination_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_elimination_timelimit, "ui_elimination_timelimit", "20", CVAR_ARCHIVE },

	{ &ui_ctf_elimination_capturelimit, "ui_ctf_elimination_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_ctf_elimination_timelimit, "ui_ctf_elimination_timelimit", "30", CVAR_ARCHIVE },

	{ &ui_lms_fraglimit, "ui_lms_fraglimit", "20", CVAR_ARCHIVE },
	{ &ui_lms_timelimit, "ui_lms_timelimit", "0", CVAR_ARCHIVE },

	{ &ui_dd_capturelimit, "ui_dd_capturelimit", "8", CVAR_ARCHIVE },
	{ &ui_dd_timelimit, "ui_dd_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_dd_friendly, "ui_dd_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_dom_capturelimit, "ui_dom_capturelimit", "500", CVAR_ARCHIVE },
	{ &ui_dom_timelimit, "ui_dom_timelimit", "30", CVAR_ARCHIVE },
	{ &ui_dom_friendly, "ui_dom_friendly",  "0", CVAR_ARCHIVE },

	{ &ui_pos_scorelimit, "ui_pos_scorelimit", "120", CVAR_ARCHIVE },
	{ &ui_pos_timelimit, "ui_pos_timelimit", "20", CVAR_ARCHIVE },

	{ &ui_arenasFile, "g_arenasFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_spScores1, "g_spScores1", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores2, "g_spScores2", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores3, "g_spScores3", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores4, "g_spScores4", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spScores5, "g_spScores5", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spAwards, "g_spAwards", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spVideos, "g_spVideos", "", CVAR_ARCHIVE | CVAR_ROM },
	{ &ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE },

	{ &ui_spSelection, "ui_spSelection", "", CVAR_ROM },

	{ &ui_browserMaster, "ui_browserMaster", "0", CVAR_ARCHIVE },
	{ &ui_browserGameType, "ui_browserGameType", "0", CVAR_ARCHIVE },
	{ &ui_browserSortKey, "ui_browserSortKey", "4", CVAR_ARCHIVE },
	{ &ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE },
	{ &ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE },

	{ &ui_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE },
	{ &ui_drawCrosshair, "cg_drawCrosshair", "4", CVAR_ARCHIVE },
	{ &ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
	{ &ui_marks, "cg_marks", "1", CVAR_ARCHIVE },

	{ &ui_server1, "server1", "", CVAR_ARCHIVE },
	{ &ui_server2, "server2", "", CVAR_ARCHIVE },
	{ &ui_server3, "server3", "", CVAR_ARCHIVE },
	{ &ui_server4, "server4", "", CVAR_ARCHIVE },
	{ &ui_server5, "server5", "", CVAR_ARCHIVE },
	{ &ui_server6, "server6", "", CVAR_ARCHIVE },
	{ &ui_server7, "server7", "", CVAR_ARCHIVE },
	{ &ui_server8, "server8", "", CVAR_ARCHIVE },
	{ &ui_server9, "server9", "", CVAR_ARCHIVE },
	{ &ui_server10, "server10", "", CVAR_ARCHIVE },
	{ &ui_server11, "server11", "", CVAR_ARCHIVE },
	{ &ui_server12, "server12", "", CVAR_ARCHIVE },
	{ &ui_server13, "server13", "", CVAR_ARCHIVE },
	{ &ui_server14, "server14", "", CVAR_ARCHIVE },
	{ &ui_server15, "server15", "", CVAR_ARCHIVE },
	{ &ui_server16, "server16", "", CVAR_ARCHIVE },
	{ &ui_cdkeychecked, "ui_cdkeychecked", "0", CVAR_ROM },
	{ &ui_new, "ui_new", "0", CVAR_TEMP },
	{ &ui_debug, "ui_debug", "0", CVAR_TEMP },
	{ &ui_leidebug, "ui_leidebug", "0", CVAR_TEMP },
	{ &ui_initialized, "ui_initialized", "0", CVAR_TEMP },
	{ &ui_teamName, "ui_teamName", "Pagans", CVAR_ARCHIVE },
	{ &ui_opponentName, "ui_opponentName", "Stroggs", CVAR_ARCHIVE },
	{ &ui_redteam, "ui_redteam", "Pagans", CVAR_ARCHIVE },
	{ &ui_blueteam, "ui_blueteam", "Stroggs", CVAR_ARCHIVE },
	{ &ui_dedicated, "ui_dedicated", "0", CVAR_ARCHIVE },
	{ &ui_gameType, "ui_gametype", "3", CVAR_ARCHIVE },
	{ &ui_joinGameType, "ui_joinGametype", "0", CVAR_ARCHIVE },
	{ &ui_netGameType, "ui_netGametype", "3", CVAR_ARCHIVE },
	{ &ui_actualNetGameType, "ui_actualNetGametype", "3", CVAR_ARCHIVE },
	{ &ui_redteam1, "ui_redteam1", "0", CVAR_ARCHIVE },
	{ &ui_redteam2, "ui_redteam2", "0", CVAR_ARCHIVE },
	{ &ui_redteam3, "ui_redteam3", "0", CVAR_ARCHIVE },
	{ &ui_redteam4, "ui_redteam4", "0", CVAR_ARCHIVE },
	{ &ui_redteam5, "ui_redteam5", "0", CVAR_ARCHIVE },
	{ &ui_blueteam1, "ui_blueteam1", "0", CVAR_ARCHIVE },
	{ &ui_blueteam2, "ui_blueteam2", "0", CVAR_ARCHIVE },
	{ &ui_blueteam3, "ui_blueteam3", "0", CVAR_ARCHIVE },
	{ &ui_blueteam4, "ui_blueteam4", "0", CVAR_ARCHIVE },
	{ &ui_blueteam5, "ui_blueteam5", "0", CVAR_ARCHIVE },
	{ &ui_netSource, "ui_netSource", "0", CVAR_ARCHIVE },
	{ &ui_menuFiles, "ui_menuFiles", "ui/menus.txt", CVAR_ARCHIVE },
	{ &ui_currentTier, "ui_currentTier", "0", CVAR_ARCHIVE },
	{ &ui_currentMap, "ui_currentMap", "0", CVAR_ARCHIVE },
	{ &ui_currentNetMap, "ui_currentNetMap", "0", CVAR_ARCHIVE },
	{ &ui_mapIndex, "ui_mapIndex", "0", CVAR_ARCHIVE },
	{ &ui_currentOpponent, "ui_currentOpponent", "0", CVAR_ARCHIVE },
	{ &ui_selectedPlayer, "cg_selectedPlayer", "0", CVAR_ARCHIVE},
	{ &ui_selectedPlayerName, "cg_selectedPlayerName", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_0, "ui_lastServerRefresh_0", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_1, "ui_lastServerRefresh_1", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_2, "ui_lastServerRefresh_2", "", CVAR_ARCHIVE},
	{ &ui_lastServerRefresh_3, "ui_lastServerRefresh_3", "", CVAR_ARCHIVE},
	{ &ui_singlePlayerActive, "ui_singlePlayerActive", "0", 0},
	{ &ui_scoreAccuracy, "ui_scoreAccuracy", "0", CVAR_ARCHIVE},
	{ &ui_scoreImpressives, "ui_scoreImpressives", "0", CVAR_ARCHIVE},
	{ &ui_scoreExcellents, "ui_scoreExcellents", "0", CVAR_ARCHIVE},
	{ &ui_scoreCaptures, "ui_scoreCaptures", "0", CVAR_ARCHIVE},
	{ &ui_scoreDefends, "ui_scoreDefends", "0", CVAR_ARCHIVE},
	{ &ui_scoreAssists, "ui_scoreAssists", "0", CVAR_ARCHIVE},
	{ &ui_scoreGauntlets, "ui_scoreGauntlets", "0",CVAR_ARCHIVE},
	{ &ui_scoreScore, "ui_scoreScore", "0", CVAR_ARCHIVE},
	{ &ui_scorePerfect, "ui_scorePerfect", "0", CVAR_ARCHIVE},
	{ &ui_scoreTeam, "ui_scoreTeam", "0 to 0", CVAR_ARCHIVE},
	{ &ui_scoreBase, "ui_scoreBase", "0", CVAR_ARCHIVE},
	{ &ui_scoreTime, "ui_scoreTime", "00:00", CVAR_ARCHIVE},
	{ &ui_scoreTimeBonus, "ui_scoreTimeBonus", "0", CVAR_ARCHIVE},
	{ &ui_scoreSkillBonus, "ui_scoreSkillBonus", "0", CVAR_ARCHIVE},
	{ &ui_scoreShutoutBonus, "ui_scoreShutoutBonus", "0", CVAR_ARCHIVE},
	{ &ui_fragLimit, "ui_fragLimit", "10", 0},
	{ &ui_captureLimit, "ui_captureLimit", "5", 0},
	{ &ui_smallFont, "ui_smallFont", "0.25", CVAR_ARCHIVE},
	{ &ui_bigFont, "ui_bigFont", "0.4", CVAR_ARCHIVE},
	{ &ui_findPlayer, "ui_findPlayer", "Sarge", CVAR_ARCHIVE},
	{ &ui_Q3Model, "ui_q3model", "0", CVAR_ARCHIVE},
	{ &ui_hudFiles, "cg_hudFiles", "ui/hud.txt", CVAR_ARCHIVE},
	{ &ui_recordSPDemo, "ui_recordSPDemo", "0", CVAR_ARCHIVE},
	{ &ui_teamArenaFirstRun, "ui_teamArenaFirstRun", "0", CVAR_ARCHIVE},
	{ &ui_realWarmUp, "g_warmup", "20", CVAR_ARCHIVE},
	{ &ui_realCaptureLimit, "capturelimit", "8", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART},
	{ &ui_serverStatusTimeOut, "ui_serverStatusTimeOut", "7000", CVAR_ARCHIVE},
	{ &ui_humansonly, "ui_humansonly", "0", CVAR_ARCHIVE},
// Changed RD
	{ &ui_LowerAnim, "ui_LowerAnim", "0", CVAR_ARCHIVE},
	{ &ui_UpperAnim, "ui_UpperAnim", "0", CVAR_ARCHIVE},
	{ &ui_Weapon, "ui_Weapon", "0", CVAR_ARCHIVE},
	{ &ui_PlayerViewAngleYaw, "ui_PlayerViewAngleYaw", "0", CVAR_ARCHIVE},
	{ &ui_PlayerViewAnglePitch, "ui_PlayerViewAnglePitch", "0", CVAR_ARCHIVE},
	{ &ui_PlayerMoveAngleYaw, "ui_PlayerMoveAngleYaw", "0", CVAR_ARCHIVE},
	{ &ui_PlayerMoveAnglePitch, "ui_PlayerMoveAnglePitch", "0", CVAR_ARCHIVE},
	{ &ui_SaveGame, "ui_SaveGame", "", CVAR_INIT},
	{ &ui_LoadGame, "ui_LoadGame", "", CVAR_INIT},
	{ &ui_CustomServer, "ui_CustomServer", "", CVAR_INIT},
	{ &ui_HostName, "ui_HostName", "", CVAR_INIT},
	{ &ui_SpecialGame, "ui_SpecialGame", "", CVAR_INIT},
	{ &ui_TeamMembers, "ui_TeamMembers", "0", CVAR_INIT},
	{ &ui_loading, "ui_loading", "0", CVAR_INIT},
	{ &ui_transitionkey, "ui_transitionkey", "0", CVAR_INIT},
	{ &ui_applychanges, "ui_applychanges", "0", CVAR_INIT},
	{ &Save_Loading, "Save_Loading", "0", CVAR_INIT},
	{ &persid, "persid", "0", CVAR_INIT},
	{ &gameover, "gameover", "0", CVAR_INIT}, // ai script
// end changed RD
	{ &ui_introPlayed, "ui_introPlayed", "0", CVAR_INIT },
	{ &ui_missionpackChecks, "ui_missionpackChecks", "1", CVAR_INIT },
// leilei
	{ &ui_colors, "ui_colors", "0x1e3072 0x7286d0 0x1fd1b2 0x606060", CVAR_ARCHIVE},
};

// bk001129 - made static to avoid aliasing
static int		cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);


/*
=================
UI_RegisterCvars
=================
*/
void UI_RegisterCvars( void )
{
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}
}

/*
=================
UI_UpdateCvars
=================
*/
void UI_UpdateCvars( void )
{
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Update( cv->vmCvar );
	}
}


/*
=================
ArenaServers_StopRefresh
=================
*/
static void UI_StopServerRefresh( void )
{
	int count;

	if (!uiInfo.serverStatus.refreshActive) {
		// not currently refreshing
		return;
	}
	uiInfo.serverStatus.refreshActive = qfalse;
	Com_Printf("%d servers listed in browser with %d players.\n",
	           uiInfo.serverStatus.numDisplayServers,
	           uiInfo.serverStatus.numPlayersOnServers);
	count = trap_LAN_GetServerCount(ui_netSource.integer);
	if (count - uiInfo.serverStatus.numDisplayServers > 0) {
		Com_Printf("%d servers not listed due to packet loss or pings higher than %d\n",
		           count - uiInfo.serverStatus.numDisplayServers,
		           (int) trap_Cvar_VariableValue("cl_maxPing"));
	}

}

/*
=================
ArenaServers_MaxPing
=================
*/
#ifndef MISSIONPACK // bk001206
static int ArenaServers_MaxPing( void )
{
	int		maxPing;

	maxPing = (int)trap_Cvar_VariableValue( "cl_maxPing" );
	if( maxPing < 100 ) {
		maxPing = 100;
	}
	return maxPing;
}
#endif

/*
=================
UI_DoServerRefresh
=================
*/
static void UI_DoServerRefresh( void )
{
	qboolean wait = qfalse;

	if (!uiInfo.serverStatus.refreshActive) {
		return;
	}
	if (ui_netSource.integer != AS_FAVORITES) {
		if (ui_netSource.integer == AS_LOCAL) {
			if (!trap_LAN_GetServerCount(ui_netSource.integer)) {
				wait = qtrue;
			}
		}
		else {
			if (trap_LAN_GetServerCount(ui_netSource.integer) < 0) {
				wait = qtrue;
			}
		}
	}

	if (uiInfo.uiDC.realTime < uiInfo.serverStatus.refreshtime) {
		if (wait) {
			return;
		}
	}

	// if still trying to retrieve pings
	if (trap_LAN_UpdateVisiblePings(ui_netSource.integer)) {
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;
	}
	else if (!wait) {
		// get the last servers in the list
		UI_BuildServerDisplayList(qtrue, qfalse);
		// stop the refresh
		UI_StopServerRefresh();
	}
	//
	UI_BuildServerDisplayList(qfalse, qfalse);
}

/*
=================
UI_StartServerRefresh
=================
*/
static void UI_StartServerRefresh(qboolean full)
{
	char	*ptr;

	qtime_t q;
	trap_RealTime(&q);
	trap_Cvar_Set( va("ui_lastServerRefresh_%i", ui_netSource.integer), va("%s-%i, %i at %i:%i", MonthAbbrev[q.tm_mon],q.tm_mday, 1900+q.tm_year,q.tm_hour,q.tm_min));

	if (!full) {
		UI_UpdatePendingPings();
		return;
	}

	uiInfo.serverStatus.refreshActive = qtrue;
	uiInfo.serverStatus.nextDisplayRefresh = uiInfo.uiDC.realTime + 1000;
	// clear number of displayed servers
	uiInfo.serverStatus.numDisplayServers = 0;
	uiInfo.serverStatus.numPlayersOnServers = 0;
	// mark all servers as visible so we store ping updates for them
	trap_LAN_MarkServerVisible(ui_netSource.integer, -1, qtrue);
	// reset all the pings
	trap_LAN_ResetPings(ui_netSource.integer);
	//
	if( ui_netSource.integer == AS_LOCAL ) {
		trap_Cmd_ExecuteText( EXEC_NOW, "localservers\n" );
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;
		return;
	}

	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 5000;
	if( ui_netSource.integer == AS_GLOBAL ) {

		ptr = UI_Cvar_VariableString("debug_protocol");
		if (strlen(ptr)) {
			trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers 0 %s full empty\n", ptr));
		}
		else {
			trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers 0 %d full empty\n", (int)trap_Cvar_VariableValue( "protocol" ) ) );
		}
	}
}

/* Neon_Knight: Useful check in order to have code consistency. */
/*
===================
UI_IsATeamGametype

Checks if the gametype is a team-based game.
(UI_IsATeamGame(check,qtrue))
===================
 */
qboolean UI_IsATeamGametype(int check) {
	return GAMETYPE_IS_A_TEAM_GAME(check);
}
/*
===================
UI_UsesKeyObjectives

Checks if the gametype makes use of gametype-specific objectives.
(Flags, obelisks, control points...)
(UI_IsATeamGame(check,qfalse))
===================
 */
qboolean UI_UsesKeyObjectives(int check) {
	return GAMETYPE_USES_KEY_OBJECTIVES(check);
}
/*
===================
UI_UsesTeamFlags

Checks if the gametype makes use of the red and blue flags.
===================
 */
qboolean UI_UsesTeamFlags(int check) {
	return GAMETYPE_USES_RED_AND_BLUE_FLAG(check);
}
/*
===================
UI_UsesTheWhiteFlag

Checks if the gametype makes use of the neutral flag.
===================
 */
qboolean UI_UsesTheWhiteFlag(int check) {
	return GAMETYPE_USES_WHITE_FLAG(check);
}
/*
===================
UI_IsARoundBasedGametype

Checks if the gametype has a round-based system.
===================
 */
qboolean UI_IsARoundBasedGametype(int check) {
	return GAMETYPE_IS_ROUND_BASED(check);
}
/*
===================
CG_UsesTeamObelisks

Checks if the gametype uses team-colored obelisks.
===================
 */
qboolean UI_UsesTeamObelisks(int check) {
	return GAMETYPE_USES_OBELISKS(check);
}
/* /Neon_Knight */
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
// string allocation/managment

#include "ui_shared.h"

#define SCROLL_TIME_START     500
#define SCROLL_TIME_ADJUST    150
#define SCROLL_TIME_ADJUSTOFFSET 40
#define SCROLL_TIME_FLOOR     20

typedef struct scrollInfo_s {
	int nextScrollTime;
	int nextAdjustTime;
	int adjustValue;
	int scrollKey;
	float xStart;
	float yStart;
	itemDef_t *item;
	qboolean scrollDir;
} scrollInfo_t;

static scrollInfo_t scrollInfo;

static void (*captureFunc) (void *p) = 0;
static void *captureData = NULL;
static itemDef_t *itemCapture = NULL; // item that has the mouse captured ( if any )

displayContextDef_t *DC = NULL;

static qboolean g_waitingForKey = qfalse;
static qboolean g_editingField = qfalse;

// Changed RD
const char *BackgroundTrack = NULL;
// end changed RD


static itemDef_t *g_bindItem = NULL;
static itemDef_t *g_editItem = NULL;

menuDef_t Menus[MAX_MENUS]; // defined menus
int menuCount = 0; // how many

menuDef_t *menuStack[MAX_OPEN_MENUS];
int openMenuCount = 0;

static qboolean debugMode = qfalse;

#define DOUBLE_CLICK_DELAY 300
static int lastListBoxClickTime = 0;

void Item_RunScript(itemDef_t *item, const char *s);
void Item_SetupKeywordHash(void);
void Menu_SetupKeywordHash(void);
int BindingIDFromName(const char *name);
qboolean Item_Bind_HandleKey(itemDef_t *item, int key, qboolean down);
itemDef_t *Menu_SetPrevCursorItem(menuDef_t *menu);
itemDef_t *Menu_SetNextCursorItem(menuDef_t *menu);
static qboolean Menu_OverActiveItem(menuDef_t *menu, float x, float y);
// changed RD
void trap_R_AddLightToScene(const vec3_t org, float intensity, float r, float g, float b);
// end changed RD

#ifdef CGAME
#define MEM_POOL_SIZE  128 * 1024
#else
#define MEM_POOL_SIZE  1024 * 1024
#endif

static char memoryPool[MEM_POOL_SIZE];
static int allocPoint, outOfMemory;

/*
===============
UI_Alloc
===============
 */
void *UI_Alloc(int size) {
	char *p;

	if (allocPoint + size > MEM_POOL_SIZE) {
		outOfMemory = qtrue;
		if (DC->Print) {
			DC->Print("UI_Alloc: Failure. Out of memory!\n");
		}
		//DC->trap_Print(S_COLOR_YELLOW"WARNING: UI Out of Memory!\n");
		return NULL;
	}

	p = &memoryPool[allocPoint];

	allocPoint += (size + 15) & ~15;

	return p;
}

/*
===============
UI_InitMemory
===============
 */
void UI_InitMemory(void) {
	allocPoint = 0;
	outOfMemory = qfalse;
}

qboolean UI_OutOfMemory(void) {
	return outOfMemory;
}





#define HASH_TABLE_SIZE 2048

/*
================
return a hash value for the string
================
 */
static unsigned hashForString(const char *str) {
	int i;
	unsigned hash;
	char letter;

	hash = 0;
	i = 0;
	while (str[i] != '\0') {
		letter = tolower(str[i]);
		hash += (unsigned) (letter)*(i + 119);
		i++;
	}
	hash &= (HASH_TABLE_SIZE - 1);
	return hash;
}

typedef struct stringDef_s {
	struct stringDef_s *next;
	const char *str;
} stringDef_t;

static int strPoolIndex = 0;
static char strPool[STRING_POOL_SIZE];

static int strHandleCount = 0;
static stringDef_t *strHandle[HASH_TABLE_SIZE];

const char *String_Alloc(const char *p) {
	int len;
	unsigned hash;
	stringDef_t *str, *last;
	static const char *staticNULL = "";

	if (p == NULL) {
		return NULL;
	}

	if (*p == 0) {
		return staticNULL;
	}

	hash = hashForString(p);

	str = strHandle[hash];
	while (str) {
		if (strequals(p, str->str)) {
			return str->str;
		}
		str = str->next;
	}

	len = strlen(p);
	if (len + strPoolIndex + 1 < STRING_POOL_SIZE) {
		int ph = strPoolIndex;
		strcpy(&strPool[strPoolIndex], p);
		strPoolIndex += len + 1;

		str = strHandle[hash];
		last = str;
		while (str && str->next) {
			last = str;
			str = str->next;
		}

		str = UI_Alloc(sizeof (stringDef_t));
		if (!str) {
			return NULL;
		}
		str->next = NULL;
		str->str = &strPool[ph];
		if (last) {
			last->next = str;
		} else {
			strHandle[hash] = str;
		}
		return &strPool[ph];
	}
	return NULL;
}

void String_Report(void) {
	float f;
	Com_Printf("Memory/String Pool Info\n");
	Com_Printf("----------------\n");
	f = strPoolIndex;
	f /= STRING_POOL_SIZE;
	f *= 100;
	Com_Printf("String Pool is %.1f%% full, %i bytes out of %i used.\n", f, strPoolIndex, STRING_POOL_SIZE);
	f = allocPoint;
	f /= MEM_POOL_SIZE;
	f *= 100;
	Com_Printf("Memory Pool is %.1f%% full, %i bytes out of %i used.\n", f, allocPoint, MEM_POOL_SIZE);
}

/*
=================
String_Init
=================
 */
void String_Init(void) {
	int i;
	for (i = 0; i < HASH_TABLE_SIZE; i++) {
		strHandle[i] = NULL;
	}
	strHandleCount = 0;
	strPoolIndex = 0;
	menuCount = 0;
	openMenuCount = 0;
	UI_InitMemory();
	Item_SetupKeywordHash();
	Menu_SetupKeywordHash();
	if (DC && DC->getBindingBuf) {
		Controls_GetConfig();
	}
}

/*
=================
PC_SourceError
=================
 */
static void PC_SourceError(int handle, const char *format, ...) __attribute__ ((format(printf, 2, 3)));

static void PC_SourceError(int handle, const char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	// leilei - suppress on non-developer mode because we do a lot of tricks that make errors upset....
	if (DC->getCVarValue("developer"))
	{
	va_start(argptr, format);
	Q_vsnprintf(string, sizeof (string), format, argptr);
	va_end(argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string);
	}
}

/*
=================
LerpColor
=================
 */
void LerpColor(vec4_t a, vec4_t b, vec4_t c, float t) {
	int i;

	// lerp and clamp each component
	for (i = 0; i < 4; i++) {
		c[i] = a[i] + t * (b[i] - a[i]);
		if (c[i] < 0)
			c[i] = 0;
		else if (c[i] > 1.0)
			c[i] = 1.0;
	}
}

/*
=================
Float_Parse
=================
 */
qboolean Float_Parse(char **p, float *f) {
	char *token;
	// Changed RD
	int negative = qfalse;
	token = COM_ParseExt(p, qfalse);
	if (token[0] == '-') {
		token = COM_ParseExt(p, qfalse);
		negative = qtrue;
	}
	if (token && token[0] != 0) {
		if (negative)
			*f = -atof(token);
		else
			// end changed RD
			*f = atof(token);
		return qtrue;
	} else {
		return qfalse;
	}
}

/*
=================
PC_Float_Parse
=================
 */
qboolean PC_Float_Parse(int handle, float *f) {
	pc_token_t token;
	int negative = qfalse;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] == '-') {
		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;
		negative = qtrue;
	}
	if (token.type != TT_NUMBER) {
		PC_SourceError(handle, "expected float but found %s\n", token.string);
		return qfalse;
	}
	if (negative)
		*f = -token.floatvalue;
	else
		*f = token.floatvalue;
	return qtrue;
}

/*
=================
Color_Parse
=================
 */
qboolean Color_Parse(char **p, vec4_t *c) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!Float_Parse(p, &f)) {
			return qfalse;
		}
		(*c)[i] = f;
	}
	return qtrue;
}

/*
=================
PC_Color_Parse
=================
 */
qboolean PC_Color_Parse(int handle, vec4_t *c) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		(*c)[i] = f;
	}
	return qtrue;
}

/*
=================
Int_Parse
=================
 */
qboolean Int_Parse(char **p, int *i) {
	char *token;
	token = COM_ParseExt(p, qfalse);

	// Changed RD
	if (token && token[0] != 0 && token[0] != ';') {
		// end changed RD
		*i = atoi(token);
		return qtrue;
	} else {
		return qfalse;
	}
}

/*
=================
PC_Int_Parse
=================
 */
qboolean PC_Int_Parse(int handle, int *i) {
	pc_token_t token;
	int negative = qfalse;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] == '-') {
		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;
		negative = qtrue;
	}
	if (token.type != TT_NUMBER) {
		PC_SourceError(handle, "expected integer but found %s\n", token.string);
		return qfalse;
	}
	*i = token.intvalue;
	if (negative)
		*i = - *i;
	return qtrue;
}

/*
=================
Rect_Parse
=================
 */
qboolean Rect_Parse(char **p, rectDef_t *r) {
	if (Float_Parse(p, &r->x)) {
		if (Float_Parse(p, &r->y)) {
			if (Float_Parse(p, &r->w)) {
				if (Float_Parse(p, &r->h)) {
					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

/*
=================
PC_Rect_Parse
=================
 */
qboolean PC_Rect_Parse(int handle, rectDef_t *r) {
	if (PC_Float_Parse(handle, &r->x)) {
		if (PC_Float_Parse(handle, &r->y)) {
			if (PC_Float_Parse(handle, &r->w)) {
				if (PC_Float_Parse(handle, &r->h)) {
					return qtrue;
				}
			}
		}
	}
	return qfalse;
}

/*
=================
String_Parse
=================
 */
qboolean String_Parse(char **p, const char **out) {
	char *token;

	token = COM_ParseExt(p, qfalse);
	if (token && token[0] != 0) {
		*(out) = String_Alloc(token);
		return qtrue;
	}
	return qfalse;
}

/*
=================
PC_String_Parse
=================
 */
qboolean PC_String_Parse(int handle, const char **out) {
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;

	*(out) = String_Alloc(token.string);
	return qtrue;
}

/*
=================
PC_Script_Parse
=================
 */
qboolean PC_Script_Parse(int handle, const char **out) {
	// rfactory change
	char script[MAX_SCRIPTSIZE];
	pc_token_t token;

	memset(script, 0, sizeof (script));
	// scripts start with { and have ; separated command lists.. commands are command, arg.. 
	// basically we want everything between the { } as it will be interpreted at run time

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (Q_stricmp(token.string, "{") != 0) {
		return qfalse;
	}

	while (1) {
		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if (Q_stricmp(token.string, "}") == 0) {
			*out = String_Alloc(script);
			return qtrue;
		}

		if (token.string[1] != '\0') {
			Q_strcat(script, MAX_SCRIPTSIZE, va("\"%s\"", token.string));
		} else {
			Q_strcat(script, MAX_SCRIPTSIZE, token.string);
		}
		Q_strcat(script, MAX_SCRIPTSIZE, " ");
	}
	return qfalse; // bk001105 - LCC   missing return value
}

// display, window, menu, item code
// 

/*
==================
Init_Display

Initializes the display with a structure to all the drawing routines
 ==================
 */
void Init_Display(displayContextDef_t *dc) {
	DC = dc;
}



// type and style painting 

void GradientBar_Paint(rectDef_t *rect, vec4_t color) {
	// gradient bar takes two paints
	DC->setColor(color);
	DC->drawHandlePic(rect->x, rect->y, rect->w, rect->h, DC->Assets.gradientBar);
	DC->setColor(NULL);
}

/*
==================
Window_Init

Initializes a window structure ( windowDef_t ) with defaults
 
==================
 */
void Window_Init(Window *w) {
	memset(w, 0, sizeof (windowDef_t));
	w->borderSize = 1;
	w->foreColor[0] = w->foreColor[1] = w->foreColor[2] = w->foreColor[3] = 1.0;
	w->cinematic = -1;
}

void Fade(int *flags, float *f, float clamp, int *nextTime, int offsetTime, qboolean bFlags, float fadeAmount) {
	if (*flags & (WINDOW_FADINGOUT | WINDOW_FADINGIN)) {
		if (DC->realTime > *nextTime) {
			*nextTime = DC->realTime + offsetTime;
			if (*flags & WINDOW_FADINGOUT) {
				*f -= fadeAmount;
				if (bFlags && *f <= 0.0) {
					*flags &= ~(WINDOW_FADINGOUT | WINDOW_VISIBLE);
				}
			} else {
				*f += fadeAmount;
				if (*f >= clamp) {
					*f = clamp;
					if (bFlags) {
						*flags &= ~WINDOW_FADINGIN;
					}
				}
			}
		}
	}
}

void Window_Paint(Window *w, float fadeAmount, float fadeClamp, float fadeCycle) {
	//float bordersize = 0;
	vec4_t color;
	rectDef_t fillRect;

	if ( w == NULL ) {
		return;
	}

	color[0] = color[1] = color[2] = color[3] = 1;
	if (debugMode) {
		DC->drawRect(w->rect.x, w->rect.y, w->rect.w, w->rect.h, 1, color);
	}

	if (w->style == 0 && w->border == 0) {
		return;
	}

	fillRect = w->rect;

	if (w->border != 0) {
		fillRect.x += w->borderSize;
		fillRect.y += w->borderSize;
		fillRect.w -= w->borderSize + 1;
		fillRect.h -= w->borderSize + 1;
	}

	if (w->style == WINDOW_STYLE_FILLED) {
		// box, but possible a shader that needs filled
		if (w->background) {
			Fade(&w->flags, &w->backColor[3], fadeClamp, &w->nextTime, fadeCycle, qtrue, fadeAmount);
			DC->setColor(w->backColor);
			DC->drawHandlePic(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->background);
			DC->setColor(NULL);
		} else {
			DC->fillRect(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->backColor);
		}
	} else if (w->style == WINDOW_STYLE_GRADIENT) {
		GradientBar_Paint(&fillRect, w->backColor);
		// gradient bar
	} else if (w->style == WINDOW_STYLE_SHADER) {
		// Changed RD
		color[0] = color[1] = color[2] = color[3] = 1;
		if (w->flags & WINDOW_FORECOLORSET) {
			Fade(&w->flags, &w->foreColor[3], fadeClamp, &w->nextTime, fadeCycle, qtrue, fadeAmount);
			DC->setColor(w->foreColor);
		} else {
			DC->setColor(color);
		}
		// end changed RD
		DC->drawHandlePic(fillRect.x, fillRect.y, fillRect.w, fillRect.h, w->background);
		DC->setColor(NULL);
	} else if (w->style == WINDOW_STYLE_TEAMCOLOR) {
		if (DC->getTeamColor) {
			DC->getTeamColor(&color);
			DC->fillRect(fillRect.x, fillRect.y, fillRect.w, fillRect.h, color);
		}
	} else if (w->style == WINDOW_STYLE_CINEMATIC) {
		if (w->cinematic == -1) {
			w->cinematic = DC->playCinematic(w->cinematicName, fillRect.x, fillRect.y, fillRect.w, fillRect.h);
			if (w->cinematic == -1) {
				w->cinematic = -2;
			}
		}
		if (w->cinematic >= 0) {
			DC->runCinematicFrame(w->cinematic);
			DC->drawCinematic(w->cinematic, fillRect.x, fillRect.y, fillRect.w, fillRect.h);
		}
	}

	if (w->border == WINDOW_BORDER_FULL) {
		// full
		// HACK HACK HACK
		if (w->style == WINDOW_STYLE_TEAMCOLOR) {
			if (color[0] > 0) {
				// red
				color[0] = 1;
				color[1] = color[2] = .5;

			} else {
				color[2] = 1;
				color[0] = color[1] = .5;
			}
			color[3] = 1;
			DC->drawRect(w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize, color);
		} else {
			DC->drawRect(w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize, w->borderColor);
		}
	} else if (w->border == WINDOW_BORDER_HORZ) {
		// top/bottom
		DC->setColor(w->borderColor);
		DC->drawTopBottom(w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize);
		DC->setColor(NULL);
	} else if (w->border == WINDOW_BORDER_VERT) {
		// left right
		DC->setColor(w->borderColor);
		DC->drawSides(w->rect.x, w->rect.y, w->rect.w, w->rect.h, w->borderSize);
		DC->setColor(NULL);
	} else if (w->border == WINDOW_BORDER_KCGRADIENT) {
		// this is just two gradient bars along each horz edge
		rectDef_t r = w->rect;
		r.h = w->borderSize;
		GradientBar_Paint(&r, w->borderColor);
		// Changed RD
		r.y = w->rect.y + w->rect.h - w->borderSize;
		// end changed RD
		GradientBar_Paint(&r, w->borderColor);
	}

}
//extern vmCvar_t  ui_leidebug;
extern int realVidWidth;
extern int realVidHeight;

// leilei - widescreen adjust
// no KM code was used in the end as that applied to renderer

void Item_SetScreenCoords(itemDef_t *item, float x, float y) {

	float resbias;
	float rex, rey;
	int newresx, newresy;
	float adjustx;
	float adjusty = 0.0f;

	rex = 640.0f / realVidWidth;
	rey = 480.0f / realVidHeight;

	newresx = 640.0f * (rex);
	newresy = 480.0f * (rey);

	newresx = realVidWidth * rey;
	newresy = realVidHeight * rey;

	resbias = 0.5 * (newresx - (newresy * (640.0 / 480.0)));


	if (item == NULL) {
		return;
	}

	if (item->window.border != 0) {
		x += item->window.borderSize;
		y += item->window.borderSize;
	}


	adjustx = 0; // reset to center


	//Com_Printf("%ix%i = real res, %f %f xandyscale, %f bias\nadjustx %f adjusty %f\n%ix%i = newres\n",realVidWidth, realVidHeight, xscale, yscale, resbias, adjustx, adjusty, newresx, newresy);
	{
		if (item->scralign)
			switch (item->scralign) {
				case ALIGN_CENTER:
					break; // already in center of screen
				case ALIGN_STRETCH:
					break; // already in center of screen
				case ALIGN_LETTERBOX:
					break;
				case ALIGN_TOP:
					break;
				case ALIGN_BOTTOM:
					break;
				case ALIGN_LEFT:
					adjustx = -resbias;
					break;
				case ALIGN_RIGHT:
					adjustx = resbias;
					break;
				case ALIGN_TOPLEFT:
					break;
				case ALIGN_TOPRIGHT:
					break;
				case ALIGN_BOTTOMLEFT:
					break;
				case ALIGN_BOTTOMRIGHT:
					break;
				case ALIGN_TOP_STRETCH:
					break;
				case ALIGN_BOTTOM_STRETCH:
					break;

			}
	}

	item->window.rect.x = x + (adjustx * item->scralignfactor) + item->window.rectClient.x;
	item->window.rect.y = y + (adjusty * item->scralignfactor) + item->window.rectClient.y;


	item->window.rect.w = item->window.rectClient.w;
	item->window.rect.h = item->window.rectClient.h;

	// force the text rects to recompute
	item->textRect.w = 0;
	item->textRect.h = 0;
}

// FIXME: consolidate this with nearby stuff

void Item_UpdatePosition(itemDef_t *item) {
	float x, y;
	menuDef_t *menu;

	if (item == NULL || item->parent == NULL) {
		return;
	}

	menu = item->parent;

	x = menu->window.rect.x;
	y = menu->window.rect.y;

	if (menu->window.border != 0) {
		x += menu->window.borderSize;
		y += menu->window.borderSize;
	}

	Item_SetScreenCoords(item, x, y);

}

// menus

void Menu_UpdatePosition(menuDef_t *menu) {
	int i;
	float x, y;

	if (menu == NULL) {
		return;
	}

	x = menu->window.rect.x;
	y = menu->window.rect.y;
	if (menu->window.border != 0) {
		x += menu->window.borderSize;
		y += menu->window.borderSize;
	}

	for (i = 0; i < menu->itemCount; i++) {
		Item_SetScreenCoords(menu->items[i], x, y);
	}
}

void Menu_PostParse(menuDef_t *menu) {
	if (menu == NULL) {
		return;
	}
	if (menu->fullScreen) {
		menu->window.rect.x = 0;
		menu->window.rect.y = 0;
		menu->window.rect.w = 640;
		menu->window.rect.h = 480;
	}
	Menu_UpdatePosition(menu);
}

itemDef_t *Menu_ClearFocus(menuDef_t *menu) {
	int i;
	itemDef_t *ret = NULL;

	if (menu == NULL) {
		return NULL;
	}

	for (i = 0; i < menu->itemCount; i++) {
		if (menu->items[i]->window.flags & WINDOW_HASFOCUS) {
			ret = menu->items[i];
		}
		menu->items[i]->window.flags &= ~WINDOW_HASFOCUS;
		if (menu->items[i]->leaveFocus) {
			Item_RunScript(menu->items[i], menu->items[i]->leaveFocus);
		}
	}

	return ret;
}

qboolean IsVisible(int flags) {
	return (flags & WINDOW_VISIBLE && !(flags & WINDOW_FADINGOUT));
}

qboolean Rect_ContainsPoint(rectDef_t *rect, float x, float y) {
	if (rect) {
		if (x > rect->x && x < rect->x + rect->w && y > rect->y && y < rect->y + rect->h) {
			return qtrue;
		}
	}
	return qfalse;
}

int Menu_ItemsMatchingGroup(menuDef_t *menu, const char *name) {
	int i;
	int count = 0;
	for (i = 0; i < menu->itemCount; i++) {
		if (Q_stricmp(menu->items[i]->window.name, name) == 0 || (menu->items[i]->window.group && Q_stricmp(menu->items[i]->window.group, name) == 0)) {
			count++;
		}
	}
	return count;
}

itemDef_t *Menu_GetMatchingItemByNumber(menuDef_t *menu, int index, const char *name) {
	int i;
	int count = 0;
	for (i = 0; i < menu->itemCount; i++) {
		if (Q_stricmp(menu->items[i]->window.name, name) == 0 || (menu->items[i]->window.group && Q_stricmp(menu->items[i]->window.group, name) == 0)) {
			if (count == index) {
				return menu->items[i];
			}
			count++;
		}
	}
	return NULL;
}

void Script_SetColor(itemDef_t *item, char **args) {
	const char *name;
	int i;
	float f;
	vec4_t *out;
	// expecting type of color to set and 4 args for the color
	if (String_Parse(args, &name)) {
		out = NULL;
		if (Q_stricmp(name, "backcolor") == 0) {
			out = &item->window.backColor;
			item->window.flags |= WINDOW_BACKCOLORSET;
		} else if (Q_stricmp(name, "forecolor") == 0) {
			out = &item->window.foreColor;
			item->window.flags |= WINDOW_FORECOLORSET;
		} else if (Q_stricmp(name, "hexcolor") == 0) {	// leilei - schemes
			out = &item->window.hexColor;
			item->window.flags |= WINDOW_HEXCOLORSET;
		} else if (Q_stricmp(name, "bordercolor") == 0) {
			out = &item->window.borderColor;
		}

		if (out) {
			for (i = 0; i < 4; i++) {
				if (!Float_Parse(args, &f)) {
					return;
				}
				(*out)[i] = f;
			}
		}
	}
}

void Script_SetAsset(itemDef_t *item, char **args) {
	const char *name;
	// expecting name to set asset to
	if (String_Parse(args, &name)) {
		// check for a model 
		if (item->type == ITEM_TYPE_MODEL) {
		}
	}
}

void Script_SetBackground(itemDef_t *item, char **args) {
	const char *name;
	// expecting name to set asset to
	if (String_Parse(args, &name)) {
		item->window.background = DC->registerShaderNoMip(name);
	}
}

itemDef_t *Menu_FindItemByName(menuDef_t *menu, const char *p) {
	int i;
	if (menu == NULL || p == NULL) {
		return NULL;
	}

	for (i = 0; i < menu->itemCount; i++) {
		if (Q_stricmp(p, menu->items[i]->window.name) == 0) {
			return menu->items[i];
		}
	}

	return NULL;
}

void Script_SetTeamColor(itemDef_t *item, char **args) {
	if (DC->getTeamColor) {
		int i;
		vec4_t color;
		DC->getTeamColor(&color);
		for (i = 0; i < 4; i++) {
			item->window.backColor[i] = color[i];
		}
	}
}

void Script_SetItemColor(itemDef_t *item, char **args) {
	const char *itemname;
	const char *name;
	vec4_t color;
	int i;
	vec4_t *out;
	// expecting type of color to set and 4 args for the color
	if (String_Parse(args, &itemname) && String_Parse(args, &name)) {
		itemDef_t *item2;
		int j;
		int count = Menu_ItemsMatchingGroup(item->parent, itemname);

		if (!Color_Parse(args, &color)) {
			return;
		}

		for (j = 0; j < count; j++) {
			item2 = Menu_GetMatchingItemByNumber(item->parent, j, itemname);
			if (item2 != NULL) {
				out = NULL;
				if (Q_stricmp(name, "backcolor") == 0) {
					out = &item2->window.backColor;
				} else if (Q_stricmp(name, "forecolor") == 0) {
					out = &item2->window.foreColor;
					item2->window.flags |= WINDOW_FORECOLORSET;
				} else if (Q_stricmp(name, "hexcolor") == 0) {
					out = &item2->window.hexColor;
					item2->window.flags |= WINDOW_HEXCOLORSET;
				} else if (Q_stricmp(name, "bordercolor") == 0) {
					out = &item2->window.borderColor;
				}

				if (out) {
					for (i = 0; i < 4; i++) {
						(*out)[i] = color[i];
					}
				}
			}
		}
	}
}



// Changed RD

void Menu_ClearItemByName(menuDef_t *menu, const char *p, itemDef_t *item) {
	itemDef_t *item1;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item1 = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item1 != NULL && item1 != item) {
			item1->clickstatus = qfalse;
		}
	}
}

void Menu_DisableItemFocus(menuDef_t *menu, const char *p, qboolean bShow) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (bShow) {
			item->window.flags |= WINDOW_FOCUSDISABLE;
		} else {
			item->window.flags &= ~WINDOW_FOCUSDISABLE;
		}
	}
}

void Menu_ClearItemAlpha(menuDef_t *menu, const char *p) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		item->window.foreColor[3] = 0;
		item->window.borderColor[3] = 0;
		item->fadeColor[3] = 0;
	}
}
// end changed RD

void Menu_ShowItemByName(menuDef_t *menu, const char *p, qboolean bShow) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != NULL) {
			if (bShow) {
				// Changed RD
				if (item->cvarFlags & CVAR_SHOW) {
					if (Item_EnableShowViaCvar(item, CVAR_SHOW)) {
						item->window.flags |= WINDOW_VISIBLE;
					}
				} else {
					item->window.flags |= WINDOW_VISIBLE;
				}
				// end changed RD
				item->window.flags |= WINDOW_VISIBLE;
			} else {
				item->window.flags &= ~WINDOW_VISIBLE;
				// Changed RD
				item->window.flags &= ~WINDOW_HASFOCUS;
				// end changed RD
				// stop cinematics playing in the window
				if (item->window.cinematic >= 0) {
					DC->stopCinematic(item->window.cinematic);
					item->window.cinematic = -1;
				}
			}
		}
	}
}

void Menu_FadeItemByName(menuDef_t *menu, const char *p, qboolean fadeOut) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != NULL) {
			if (fadeOut) {
				item->window.flags |= (WINDOW_FADINGOUT | WINDOW_VISIBLE);
				item->window.flags &= ~WINDOW_FADINGIN;
			} else {
				item->window.flags |= (WINDOW_VISIBLE | WINDOW_FADINGIN);
				item->window.flags &= ~WINDOW_FADINGOUT;
			}
		}
	}
}

menuDef_t *Menus_FindByName(const char *p) {
	int i;
	for (i = 0; i < menuCount; i++) {
		if (Q_stricmp(Menus[i].window.name, p) == 0) {
			return &Menus[i];
		}
	}
	return NULL;
}

void Menus_ShowByName(const char *p) {
	menuDef_t *menu = Menus_FindByName(p);
	if (menu) {
		Menus_Activate(menu);
	}
}

void Menus_OpenByName(const char *p) {
	Menus_ActivateByName(p);
}

static void Menu_RunCloseScript(menuDef_t *menu) {
	if (menu && menu->window.flags & WINDOW_VISIBLE && menu->onClose) {
		itemDef_t item;
		item.parent = menu;
		Item_RunScript(&item, menu->onClose);
	}
}

void Menus_CloseByName(const char *p) {
	menuDef_t *menu = Menus_FindByName(p);
	// Changed RD
	if (Q_stricmp(p, "all") == 0) {
		Menus_CloseAll();
		return;
	}
	// end changed RD
	if (menu != NULL) {
		Menu_RunCloseScript(menu);
		menu->window.flags &= ~(WINDOW_VISIBLE | WINDOW_HASFOCUS);
	}
}

void Menus_CloseAll(void) {
	int i;
	for (i = 0; i < menuCount; i++) {
		Menu_RunCloseScript(&Menus[i]);
		Menus[i].window.flags &= ~(WINDOW_HASFOCUS | WINDOW_VISIBLE);
	}
}


// Changed RD

void Script_Clear(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_ClearItemByName(item->parent, name, item);
	}
}

void Script_FocusDisable(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_DisableItemFocus(item->parent, name, qtrue);
	}
}

void Script_FocusEnable(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_DisableItemFocus(item->parent, name, qfalse);
	}
}

void Script_ClearAlpha(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_ClearItemAlpha(item->parent, name);
	}
}
// end changed RD

void Script_Show(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_ShowItemByName(item->parent, name, qtrue);
	}
}

void Script_Hide(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_ShowItemByName(item->parent, name, qfalse);
	}
}

void Script_FadeIn(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_FadeItemByName(item->parent, name, qfalse);
	}
}

void Script_FadeOut(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menu_FadeItemByName(item->parent, name, qtrue);
	}
}

void Script_Open(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menus_OpenByName(name);
	}
}

void Script_ConditionalOpen(itemDef_t *item, char **args) {
	const char *cvar;
	const char *name1;
	const char *name2;
	float val;

	if (String_Parse(args, &cvar) && String_Parse(args, &name1) && String_Parse(args, &name2)) {
		val = DC->getCVarValue(cvar);
		if (val == 0.f) {
			Menus_OpenByName(name2);
		} else {
			Menus_OpenByName(name1);
		}
	}
}

void Script_Close(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		Menus_CloseByName(name);
	}
}

// Changed RD

void Menu_TransitionItemByName(itemDef_t *callitem, menuDef_t *menu, const char *p, rectDef_t rectFrom, rectDef_t rectTo, int time, float amt, qboolean xflag) {
	//void Menu_TransitionItemByName(menuDef_t *menu, const char *p, rectDef_t rectFrom, rectDef_t rectTo, int time, float amt) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != NULL) {
			item->window.flags |= (WINDOW_INTRANSITION | WINDOW_VISIBLE);
			if (!xflag) {
				item->window.flags |= WINDOW_NOTRANSITION;
			} else {
				item->transitionEnd = callitem->transitionEnd;
			}
			item->window.offsetTime = time;
			memcpy(&item->window.rectClient, &rectFrom, sizeof (rectDef_t));
			memcpy(&item->window.rectEffects, &rectTo, sizeof (rectDef_t));
			item->window.rectEffects2.x = fabs(rectTo.x - rectFrom.x) / amt;
			item->window.rectEffects2.y = fabs(rectTo.y - rectFrom.y) / amt;
			item->window.rectEffects2.w = fabs(rectTo.w - rectFrom.w) / amt;
			item->window.rectEffects2.h = fabs(rectTo.h - rectFrom.h) / amt;
			Item_UpdatePosition(item);
		}
	}
}

void Script_Transition(itemDef_t *item, char **args) {
	const char *name;
	rectDef_t rectFrom, rectTo;
	int time;
	float amt;

	if (String_Parse(args, &name)) {
		if (Rect_Parse(args, &rectFrom) && Rect_Parse(args, &rectTo) && Int_Parse(args, &time) && Float_Parse(args, &amt)) {
			// Changed RD
			Menu_TransitionItemByName(item, item->parent, name, rectFrom, rectTo, time, amt, qfalse);
			// end changed RD
		}
	}
}


// Changed RD

void Script_xTransition(itemDef_t *item, char **args) {
	const char *name;
	rectDef_t rectFrom, rectTo;
	int time;
	float amt;

	if (String_Parse(args, &name)) {
		if (Rect_Parse(args, &rectFrom) && Rect_Parse(args, &rectTo) && Int_Parse(args, &time) && Float_Parse(args, &amt)) {
			Menu_TransitionItemByName(item, item->parent, name, rectFrom, rectTo, time, amt, qtrue);
		}
	}
}
// end changed RD

void Menu_OrbitItemByName(menuDef_t *menu, const char *p, float x, float y, float cx, float cy, int time) {
	itemDef_t *item;
	int i;
	int count = Menu_ItemsMatchingGroup(menu, p);
	for (i = 0; i < count; i++) {
		item = Menu_GetMatchingItemByNumber(menu, i, p);
		if (item != NULL) {
			item->window.flags |= (WINDOW_ORBITING | WINDOW_VISIBLE);
			item->window.offsetTime = time;
			item->window.rectEffects.x = cx;
			item->window.rectEffects.y = cy;
			item->window.rectClient.x = x;
			item->window.rectClient.y = y;
			Item_UpdatePosition(item);
		}
	}
}

void Script_Orbit(itemDef_t *item, char **args) {
	const char *name;
	float cx, cy, x, y;
	int time;

	if (String_Parse(args, &name)) {
		if (Float_Parse(args, &x) && Float_Parse(args, &y) && Float_Parse(args, &cx) && Float_Parse(args, &cy) && Int_Parse(args, &time)) {
			Menu_OrbitItemByName(item->parent, name, x, y, cx, cy, time);
		}
	}
}

void Script_SetFocus(itemDef_t *item, char **args) {
	const char *name;
	itemDef_t *focusItem;

	if (String_Parse(args, &name)) {
		focusItem = Menu_FindItemByName(item->parent, name);
		if (focusItem && !(focusItem->window.flags & WINDOW_DECORATION) && !(focusItem->window.flags & WINDOW_HASFOCUS)) {
			Menu_ClearFocus(item->parent);
			focusItem->window.flags |= WINDOW_HASFOCUS;
			if (focusItem->onFocus) {
				Item_RunScript(focusItem, focusItem->onFocus);
			}
			if (DC->Assets.itemFocusSound) {
				DC->startLocalSound(DC->Assets.itemFocusSound, CHAN_LOCAL_SOUND);
			}
		}
	}
}


// Changed RD

void Script_ClearFocus(itemDef_t *item, char **args) {
	const char *name;
	String_Parse(args, &name);
	Menu_ClearFocus(item->parent);
}
// end changed RD

void Script_SetPlayerModel(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		DC->setCVar("team_model", name);
	}
}

void Script_SetPlayerHead(itemDef_t *item, char **args) {
	const char *name;
	if (String_Parse(args, &name)) {
		DC->setCVar("team_headmodel", name);
	}
}

void Script_SetCvar(itemDef_t *item, char **args) {
	const char *cvar, *val;
	if (String_Parse(args, &cvar) && String_Parse(args, &val)) {
		DC->setCVar(cvar, val);
	}

}

// Changed RD

void Script_ClearCvar(itemDef_t *item, char **args) {
	const char *cvar;
	if (String_Parse(args, &cvar)) {
		DC->setCVar(cvar, "");
	}

}

void Script_SetCvarFloat(itemDef_t *item, char **args) {
	const char *cvar;
	float val;
	if (String_Parse(args, &cvar) && Float_Parse(args, &val)) {
		DC->setCVar(cvar, va("%f ; ", val));
	}

}

void Script_CopyCvar(itemDef_t *item, char **args) {
	const char *cvar, *val;
	char buff[1024];
	if (String_Parse(args, &cvar) && String_Parse(args, &val)) {
		DC->getCVarString(val, buff, sizeof (buff));
		DC->setCVar(cvar, buff);
	}

}
// end changed RD

void Script_Exec(itemDef_t *item, char **args) {
	const char *val;
	if (String_Parse(args, &val)) {
		DC->executeText(EXEC_APPEND, va("%s ; ", val));
	}
}

void Script_Play(itemDef_t *item, char **args) {
	const char *val;
	if (String_Parse(args, &val)) {
		DC->startLocalSound(DC->registerSound(val, qfalse), CHAN_LOCAL_SOUND);
	}
}

void Script_playLooped(itemDef_t *item, char **args) {
	const char *val;
	if (String_Parse(args, &val)) {
		if (!BackgroundTrack) {
			BackgroundTrack = String_Alloc(val);
			DC->stopBackgroundTrack();
			DC->startBackgroundTrack(val, val);
		} else {
			if (Q_stricmp(BackgroundTrack, val)) {
				BackgroundTrack = String_Alloc(val);
				DC->stopBackgroundTrack();
				DC->startBackgroundTrack(val, val);
			}
		}
	}
}


commandDef_t commandList[] ={
	{"fadein", &Script_FadeIn}, // group/name
	{"fadeout", &Script_FadeOut}, // group/name
	{"show", &Script_Show}, // group/name
	{"hide", &Script_Hide}, // group/name
	{"setcolor", &Script_SetColor}, // works on this
	{"open", &Script_Open}, // menu
	{"conditionalopen", &Script_ConditionalOpen}, // menu
	{"close", &Script_Close}, // menu
	{"setasset", &Script_SetAsset}, // works on this
	{"setbackground", &Script_SetBackground}, // works on this
	{"setitemcolor", &Script_SetItemColor}, // group/name
	{"setteamcolor", &Script_SetTeamColor}, // sets this background color to team color
	{"setfocus", &Script_SetFocus}, // sets this background color to team color
	{"setplayermodel", &Script_SetPlayerModel}, // sets this background color to team color
	{"setplayerhead", &Script_SetPlayerHead}, // sets this background color to team color
	{"transition", &Script_Transition}, // group/name
	{"setcvar", &Script_SetCvar}, // group/name
	// Changed RD
	{"xtransition", &Script_xTransition},
	{"setcvarfloat", &Script_SetCvarFloat},
	{"clearcvar", &Script_ClearCvar},
	{"clearclick", &Script_Clear},
	{"disablefocus", &Script_FocusDisable},
	{"enablefocus", &Script_FocusEnable},
	{"copycvar", &Script_CopyCvar},
	{"clearfocus", &Script_ClearFocus},
	{"clearalpha", &Script_ClearAlpha},
	// end changed RD
	{"exec", &Script_Exec}, // group/name
	{"play", &Script_Play}, // group/name
	{"playlooped", &Script_playLooped}, // group/name
	{"orbit", &Script_Orbit} // group/name
};

int scriptCommandCount = sizeof (commandList) / sizeof (commandDef_t);

void Item_RunScript(itemDef_t *item, const char *s) {
	char script[MAX_SCRIPTSIZE], *p;
	int i;
	qboolean bRan;
	memset(script, 0, sizeof (script));
	if (item && s && s[0]) {
		Q_strcat(script, MAX_SCRIPTSIZE, s);
		p = script;
		while (1) {
			const char *command;
			// expect command then arguments, ; ends command, NULL ends script
			if (!String_Parse(&p, &command)) {
				return;
			}

			if (command[0] == ';' && command[1] == '\0') {
				continue;
			}

			bRan = qfalse;
			for (i = 0; i < scriptCommandCount; i++) {
				if (Q_stricmp(command, commandList[i].name) == 0) {
					(commandList[i].handler(item, &p));
					bRan = qtrue;
					break;
				}
			}
			// not in our auto list, pass to handler
			if (!bRan) {
				DC->runScript(&p);
			}
		}
	}
}

qboolean Item_EnableShowViaCvar(itemDef_t *item, int flag) {
	char script[1024], *p;
	memset(script, 0, sizeof (script));
	if (item && item->enableCvar && *item->enableCvar && item->cvarTest && *item->cvarTest) {
		char buff[1024];
		DC->getCVarString(item->cvarTest, buff, sizeof (buff));

		Q_strcat(script, 1024, item->enableCvar);
		p = script;
		while (1) {
			const char *val;
			// expect value then ; or NULL, NULL ends list
			if (!String_Parse(&p, &val)) {
				return (item->cvarFlags & flag) ? qfalse : qtrue;
			}

			if (val[0] == ';' && val[1] == '\0') {
				continue;
			}

			// enable it if any of the values are true
			if (item->cvarFlags & flag) {
				if (Q_strequal(buff, val)) {
					return qtrue;
				}
			} else {
				// disable it if any of the values are true
				if (Q_strequal(buff, val)) {
					return qfalse;
				}
			}

		}
		return (item->cvarFlags & flag) ? qfalse : qtrue;
	}
	return qtrue;
}


// will optionaly set focus to this item 

qboolean Item_SetFocus(itemDef_t *item, float x, float y) {
	int i;
	itemDef_t *oldFocus;
	sfxHandle_t *sfx = &DC->Assets.itemFocusSound;
	qboolean playSound = qfalse;
	menuDef_t *parent; // bk001206: = (menuDef_t*)item->parent;
	// sanity check, non-null, not a decoration and does not already have the focus
	// Changed RD
	if (item == NULL || item->window.flags & WINDOW_DECORATION || item->window.flags & WINDOW_HASFOCUS || !(item->window.flags & WINDOW_VISIBLE) || item->window.flags & WINDOW_FOCUSDISABLE) {
		// end changed RD
		return qfalse;
	}

	// bk001206 - this can be NULL.
	parent = (menuDef_t*) item->parent;

	// items can be enabled and disabled based on cvars
	if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
		return qfalse;
	}

	if (item->cvarFlags & (CVAR_SHOW | CVAR_HIDE) && !Item_EnableShowViaCvar(item, CVAR_SHOW)) {
		return qfalse;
	}

	oldFocus = Menu_ClearFocus(item->parent);

	if (item->type == ITEM_TYPE_TEXT) {
		rectDef_t r;
		r = item->textRect;
		r.y -= r.h;
		if (Rect_ContainsPoint(&r, x, y)) {
			item->window.flags |= WINDOW_HASFOCUS;
			if (item->focusSound) {
				sfx = &item->focusSound;
			}
			playSound = qtrue;
		} else {
			if (oldFocus) {
				oldFocus->window.flags |= WINDOW_HASFOCUS;
				if (oldFocus->onFocus) {
					Item_RunScript(oldFocus, oldFocus->onFocus);
				}
			}
		}
	} else {
		item->window.flags |= WINDOW_HASFOCUS;
		if (item->onFocus) {
			Item_RunScript(item, item->onFocus);
		}
		if (item->focusSound) {
			sfx = &item->focusSound;
		}
		playSound = qtrue;
	}

	if (playSound && sfx) {
		DC->startLocalSound(*sfx, CHAN_LOCAL_SOUND);
	}

	for (i = 0; i < parent->itemCount; i++) {
		if (parent->items[i] == item) {
			parent->cursorItem = i;
			break;
		}
	}

	return qtrue;
}

int Item_ListBox_MaxScroll(itemDef_t *item) {
	listBoxDef_t *listPtr = (listBoxDef_t*) item->typeData;
	int count = DC->feederCount(item->special);
	int max;

	if (item->window.flags & WINDOW_HORIZONTAL) {
		max = count - (item->window.rect.w / listPtr->elementWidth) + 1;
	} else {
		max = count - (item->window.rect.h / listPtr->elementHeight) + 1;
	}
	if (max < 0) {
		return 0;
	}
	return max;
}

int Item_ListBox_ThumbPosition(itemDef_t *item) {
	float max, pos, size;
	listBoxDef_t *listPtr = (listBoxDef_t*) item->typeData;

	max = Item_ListBox_MaxScroll(item);
	if (item->window.flags & WINDOW_HORIZONTAL) {
		size = item->window.rect.w - (SCROLLBAR_SIZE * 2) - 2;
		if (max > 0) {
			pos = (size - SCROLLBAR_SIZE) / (float) max;
		} else {
			pos = 0;
		}
		pos *= listPtr->startPos;
		return item->window.rect.x + 1 + SCROLLBAR_SIZE + pos;
	} else {
		size = item->window.rect.h - (SCROLLBAR_SIZE * 2) - 2;
		if (max > 0) {
			pos = (size - SCROLLBAR_SIZE) / (float) max;
		} else {
			pos = 0;
		}
		pos *= listPtr->startPos;
		return item->window.rect.y + 1 + SCROLLBAR_SIZE + pos;
	}
}

int Item_ListBox_ThumbDrawPosition(itemDef_t *item) {
	int min, max;

	if (itemCapture == item) {
		if (item->window.flags & WINDOW_HORIZONTAL) {
			min = item->window.rect.x + SCROLLBAR_SIZE + 1;
			max = item->window.rect.x + item->window.rect.w - 2 * SCROLLBAR_SIZE - 1;
			if (DC->cursorx >= min + SCROLLBAR_SIZE / 2 && DC->cursorx <= max + SCROLLBAR_SIZE / 2) {
				return DC->cursorx - SCROLLBAR_SIZE / 2;
			} else {
				return Item_ListBox_ThumbPosition(item);
			}
		} else {
			min = item->window.rect.y + SCROLLBAR_SIZE + 1;
			max = item->window.rect.y + item->window.rect.h - 2 * SCROLLBAR_SIZE - 1;
			if (DC->cursory >= min + SCROLLBAR_SIZE / 2 && DC->cursory <= max + SCROLLBAR_SIZE / 2) {
				return DC->cursory - SCROLLBAR_SIZE / 2;
			} else {
				return Item_ListBox_ThumbPosition(item);
			}
		}
	} else {
		return Item_ListBox_ThumbPosition(item);
	}
}

float Item_Slider_ThumbPosition(itemDef_t *item) {
	float value, range, x;
	editFieldDef_t *editDef = item->typeData;

	if (item->text) {
		x = item->textRect.x + item->textRect.w + 8;
	} else {
		x = item->window.rect.x;
	}

	if (!editDef || !item->cvar) {
		return x;
	}

	value = DC->getCVarValue(item->cvar);

	if (value < editDef->minVal) {
		value = editDef->minVal;
	} else if (value > editDef->maxVal) {
		value = editDef->maxVal;
	}

	range = editDef->maxVal - editDef->minVal;
	value -= editDef->minVal;
	value /= range;
	//value /= (editDef->maxVal - editDef->minVal);
	value *= SLIDER_WIDTH;
	x += value;
	// vm fuckage
	//x = x + (((float)value / editDef->maxVal) * SLIDER_WIDTH);
	return x;
}

int Item_Slider_OverSlider(itemDef_t *item, float x, float y) {
	rectDef_t r;

	r.x = Item_Slider_ThumbPosition(item) - (SLIDER_THUMB_WIDTH / 2);
	r.y = item->window.rect.y - 2;
	r.w = SLIDER_THUMB_WIDTH;
	r.h = SLIDER_THUMB_HEIGHT;

	if (Rect_ContainsPoint(&r, x, y)) {
		return WINDOW_LB_THUMB;
	}
	return 0;
}

int Item_ListBox_OverLB(itemDef_t *item, float x, float y) {
	rectDef_t r;
	int thumbstart;

	if (item->window.flags & WINDOW_HORIZONTAL) {
		// check if on left arrow
		r.x = item->window.rect.x;
		r.y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
		r.h = r.w = SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_LEFTARROW;
		}
		// check if on right arrow
		r.x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_RIGHTARROW;
		}
		// check if on thumb
		thumbstart = Item_ListBox_ThumbPosition(item);
		r.x = thumbstart;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_THUMB;
		}
		r.x = item->window.rect.x + SCROLLBAR_SIZE;
		r.w = thumbstart - r.x;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_PGUP;
		}
		r.x = thumbstart + SCROLLBAR_SIZE;
		r.w = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_PGDN;
		}
	} else {
		r.x = item->window.rect.x + item->window.rect.w - SCROLLBAR_SIZE;
		r.y = item->window.rect.y;
		r.h = r.w = SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_LEFTARROW;
		}
		r.y = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_RIGHTARROW;
		}
		thumbstart = Item_ListBox_ThumbPosition(item);
		r.y = thumbstart;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_THUMB;
		}
		r.y = item->window.rect.y + SCROLLBAR_SIZE;
		r.h = thumbstart - r.y;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_PGUP;
		}
		r.y = thumbstart + SCROLLBAR_SIZE;
		r.h = item->window.rect.y + item->window.rect.h - SCROLLBAR_SIZE;
		if (Rect_ContainsPoint(&r, x, y)) {
			return WINDOW_LB_PGDN;
		}
	}
	return 0;
}

void Item_ListBox_MouseEnter(itemDef_t *item, float x, float y) {
	rectDef_t r;
	listBoxDef_t *listPtr = (listBoxDef_t*) item->typeData;
	// Changed RD
	if (item->window.flags & WINDOW_FOCUSDISABLE)
		return;
	// end changed RD   
	item->window.flags &= ~(WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN);
	item->window.flags |= Item_ListBox_OverLB(item, x, y);

	if (item->window.flags & WINDOW_HORIZONTAL) {
		if (!(item->window.flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN))) {
			// check for selection hit as we have exausted buttons and thumb
			if (listPtr->elementStyle == LISTBOX_IMAGE) {
				r.x = item->window.rect.x;
				r.y = item->window.rect.y;
				r.h = item->window.rect.h - DC->Assets.scrollbarsize;
				r.w = item->window.rect.w - listPtr->drawPadding;
				if (Rect_ContainsPoint(&r, x, y)) {
					listPtr->cursorPos = (int) ((x - r.x) / listPtr->elementWidth) + listPtr->startPos;
					if (listPtr->cursorPos >= listPtr->endPos) {
						listPtr->cursorPos = listPtr->endPos;
					}
				}
			} else {
				// text hit.. 
			}
		}
	} else if (!(item->window.flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_THUMB | WINDOW_LB_PGUP | WINDOW_LB_PGDN))) {
		r.x = item->window.rect.x;
		r.y = item->window.rect.y;
		r.w = item->window.rect.w - DC->Assets.scrollbarsize;
		r.h = item->window.rect.h - listPtr->drawPadding;
		if (Rect_ContainsPoint(&r, x, y)) {
			listPtr->cursorPos = (int) ((y - 2 - r.y) / listPtr->elementHeight) + listPtr->startPos;
			if (listPtr->cursorPos > listPtr->endPos) {
				listPtr->cursorPos = listPtr->endPos;
			}
		}
	}
}

void Item_MouseEnter(itemDef_t *item, float x, float y) {
	rectDef_t r;
	if (item) {
		r = item->textRect;
		r.y -= r.h;
		// in the text rect?
		// Changed RD
		if (item->window.flags & WINDOW_FOCUSDISABLE)
			return;
		// end changed RD
		// items can be enabled and disabled based on cvars
		if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
			return;
		}

		if (item->cvarFlags & (CVAR_SHOW | CVAR_HIDE) && !Item_EnableShowViaCvar(item, CVAR_SHOW)) {
			return;
		}

		if (Rect_ContainsPoint(&r, x, y)) {
			if (!(item->window.flags & WINDOW_MOUSEOVERTEXT)) {
				Item_RunScript(item, item->mouseEnterText);
				item->window.flags |= WINDOW_MOUSEOVERTEXT;
			}
			if (!(item->window.flags & WINDOW_MOUSEOVER)) {
				Item_RunScript(item, item->mouseEnter);
				item->window.flags |= WINDOW_MOUSEOVER;
			}

		} else {
			// not in the text rect
			if (item->window.flags & WINDOW_MOUSEOVERTEXT) {
				// if we were
				Item_RunScript(item, item->mouseExitText);
				item->window.flags &= ~WINDOW_MOUSEOVERTEXT;
			}
			if (!(item->window.flags & WINDOW_MOUSEOVER)) {
				Item_RunScript(item, item->mouseEnter);
				item->window.flags |= WINDOW_MOUSEOVER;
			}

			if (item->type == ITEM_TYPE_LISTBOX) {
				Item_ListBox_MouseEnter(item, x, y);
			}
		}
	}
}

void Item_MouseLeave(itemDef_t *item) {
	if (item) {
		if (item->window.flags & WINDOW_MOUSEOVERTEXT) {
			Item_RunScript(item, item->mouseExitText);
			item->window.flags &= ~WINDOW_MOUSEOVERTEXT;
		}
		Item_RunScript(item, item->mouseExit);
		item->window.flags &= ~(WINDOW_LB_RIGHTARROW | WINDOW_LB_LEFTARROW);
	}
}

itemDef_t *Menu_HitTest(menuDef_t *menu, float x, float y) {
	int i;
	for (i = 0; i < menu->itemCount; i++) {
		if (Rect_ContainsPoint(&menu->items[i]->window.rect, x, y)) {
			return menu->items[i];
		}
	}
	return NULL;
}

void Item_SetMouseOver(itemDef_t *item, qboolean focus) {
	if (item) {
		if (focus) {
			item->window.flags |= WINDOW_MOUSEOVER;
		} else {
			item->window.flags &= ~WINDOW_MOUSEOVER;
		}
	}
}

qboolean Item_OwnerDraw_HandleKey(itemDef_t *item, int key) {
	if (item && DC->ownerDrawHandleKey) {
		return DC->ownerDrawHandleKey(item->window.ownerDraw, item->window.ownerDrawFlags, &item->special, key);
	}
	return qfalse;
}

qboolean Item_ListBox_HandleKey(itemDef_t *item, int key, qboolean down, qboolean force) {
	listBoxDef_t *listPtr = (listBoxDef_t*) item->typeData;
	int count = DC->feederCount(item->special);
	int max, viewmax;

	if (force || (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS)) {
		max = Item_ListBox_MaxScroll(item);
		if (item->window.flags & WINDOW_HORIZONTAL) {
			viewmax = (item->window.rect.w / listPtr->elementWidth);
			if (key == K_LEFTARROW || key == K_JOY32) {
				if (!listPtr->notselectable) {
					listPtr->cursorPos--;
					if (listPtr->cursorPos < 0) {
						listPtr->cursorPos = 0;
					}
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos);
				} else {
					listPtr->startPos--;
					if (listPtr->startPos < 0)
						listPtr->startPos = 0;
				}
				return qtrue;
			}
			if (key == K_RIGHTARROW || key == K_JOY30) {
				if (!listPtr->notselectable) {
					listPtr->cursorPos++;
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
					}
					if (listPtr->cursorPos >= count) {
						listPtr->cursorPos = count - 1;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos);
				} else {
					listPtr->startPos++;
					if (listPtr->startPos >= count)
						listPtr->startPos = count - 1;
				}
				return qtrue;
			}
		} else {
			viewmax = (item->window.rect.h / listPtr->elementHeight);
			if (key == K_UPARROW || key == K_JOY29) {
				if (!listPtr->notselectable) {
					listPtr->cursorPos--;
					if (listPtr->cursorPos < 0) {
						listPtr->cursorPos = 0;
					}
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos);
				} else {
					listPtr->startPos--;
					if (listPtr->startPos < 0)
						listPtr->startPos = 0;
				}
				return qtrue;
			}
			if (key == K_DOWNARROW || key == K_JOY31) {
				if (!listPtr->notselectable) {
					listPtr->cursorPos++;
					if (listPtr->cursorPos < listPtr->startPos) {
						listPtr->startPos = listPtr->cursorPos;
					}
					if (listPtr->cursorPos >= count) {
						listPtr->cursorPos = count - 1;
					}
					if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
						listPtr->startPos = listPtr->cursorPos - viewmax + 1;
					}
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos);
				} else {
					listPtr->startPos++;
					if (listPtr->startPos > max)
						listPtr->startPos = max;
				}
				return qtrue;
			}
		}
		// Use mouse wheel in vertical and horizontal menus.
		// If scrolling 3 items would replace over half of the
		// displayed items, only scroll 1 item at a time.
		if ( key == K_MWHEELUP ) {
			int scroll = viewmax < 6 ? 1 : 3;
			listPtr->startPos -= scroll;
			if (listPtr->startPos < 0) {
				listPtr->startPos = 0;
			}
			return qtrue;
		}
		if ( key == K_MWHEELDOWN ) {
			int scroll = viewmax < 6 ? 1 : 3;
			listPtr->startPos += scroll;
			if (listPtr->startPos > max) {
				listPtr->startPos = max;
			}
			return qtrue;
		}
		// mouse hit
		if (key == K_MOUSE1 || key == K_MOUSE2) {
			if (item->window.flags & WINDOW_LB_LEFTARROW) {
				listPtr->startPos--;
				if (listPtr->startPos < 0) {
					listPtr->startPos = 0;
				}
				// Changed RD
				return qfalse;
				// end changed RD
			} else if (item->window.flags & WINDOW_LB_RIGHTARROW) {
				// one down
				listPtr->startPos++;
				if (listPtr->startPos > max) {
					listPtr->startPos = max;
				}
				// Changed RD
				return qfalse;
				// end changed RD
			} else if (item->window.flags & WINDOW_LB_PGUP) {
				// page up
				listPtr->startPos -= viewmax;
				if (listPtr->startPos < 0) {
					listPtr->startPos = 0;
				}
				// Changed RD
				return qfalse;
				// end changed RD
			} else if (item->window.flags & WINDOW_LB_PGDN) {
				// page down
				listPtr->startPos += viewmax;
				if (listPtr->startPos > max) {
					listPtr->startPos = max;
				}
				// Changed RD
				return qfalse;
				// end changed RD
			} else if (item->window.flags & WINDOW_LB_THUMB) {
				// Display_SetCaptureItem(item);
			} else {
				// select an item
				// Changed RD
				if (DC->realTime < lastListBoxClickTime && listPtr->doubleClick && !(item->window.flags & WINDOW_FOCUSDISABLE)) {
					// end changed RD
					Item_RunScript(item, listPtr->doubleClick);
				}
				lastListBoxClickTime = DC->realTime + DOUBLE_CLICK_DELAY;
				if (item->cursorPos != listPtr->cursorPos) {
					item->cursorPos = listPtr->cursorPos;
					DC->feederSelection(item->special, item->cursorPos);
				}
				// Changed RD
				return qfalse;
				// end changed RD
			}
			return qtrue;
		}
		if (key == K_HOME) {
			// home
			listPtr->startPos = 0;
			return qtrue;
		}
		if (key == K_END) {
			// end
			listPtr->startPos = max;
			return qtrue;
		}
		if (key == K_PGUP) {
			// page up
			if (!listPtr->notselectable) {
				listPtr->cursorPos -= viewmax;
				if (listPtr->cursorPos < 0) {
					listPtr->cursorPos = 0;
				}
				if (listPtr->cursorPos < listPtr->startPos) {
					listPtr->startPos = listPtr->cursorPos;
				}
				if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
					listPtr->startPos = listPtr->cursorPos - viewmax + 1;
				}
				item->cursorPos = listPtr->cursorPos;
				DC->feederSelection(item->special, item->cursorPos);
			} else {
				listPtr->startPos -= viewmax;
				if (listPtr->startPos < 0) {
					listPtr->startPos = 0;
				}
			}
			return qtrue;
		}
		if (key == K_PGDN) {
			// page down
			if (!listPtr->notselectable) {
				listPtr->cursorPos += viewmax;
				if (listPtr->cursorPos < listPtr->startPos) {
					listPtr->startPos = listPtr->cursorPos;
				}
				if (listPtr->cursorPos >= count) {
					listPtr->cursorPos = count - 1;
				}
				if (listPtr->cursorPos >= listPtr->startPos + viewmax) {
					listPtr->startPos = listPtr->cursorPos - viewmax + 1;
				}
				item->cursorPos = listPtr->cursorPos;
				DC->feederSelection(item->special, item->cursorPos);
			} else {
				listPtr->startPos += viewmax;
				if (listPtr->startPos > max) {
					listPtr->startPos = max;
				}
			}
			return qtrue;
		}
	}
	return qfalse;
}


// Changed RD

qboolean Item_Checkbox_HandleKey(itemDef_t *item, int key) {

	if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS && item->cvar) {
		if (key == K_MOUSE1 || key == K_ENTER || key == K_JOY1 || key == K_MOUSE2 || key == K_MOUSE3) {
			DC->setCVar(item->cvar, va("%i", !DC->getCVarValue(item->cvar)));
			return qtrue;
		}
	}

	return qfalse;

}

qboolean Item_Combo_HandleKey(itemDef_t *item, int key) {

	Rectangle Box;
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;
	Box.x = item->window.rect.x;
	Box.y = item->window.rect.y;
	Box.w = DC->Assets.combowidth;
	Box.h = DC->Assets.comboheight;
	if (item->alignment == ITEM_ALIGN_RIGHT) {
		Box.x = item->window.rect.x + item->window.rect.w - DC->Assets.combowidth;
	}
	if (comboPtr->combopen) {
		if (Rect_ContainsPoint(&comboPtr->ComboBox, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS && item->cvar) {
			if (key == K_MOUSE1 || key == K_ENTER || key == K_JOY1 || key == K_MOUSE2 || key == K_MOUSE3) {
				if (comboPtr->comboItem != -1) {
					multiDef_t *multiPtr = (multiDef_t*) item->multiData;
					if (!multiPtr->strDef) {
						DC->setCVar(item->cvar, va("%d", (int) multiPtr->cvarValue[comboPtr->comboItem]));
					} else {
						DC->setCVar(item->cvar, multiPtr->cvarStr[comboPtr->comboItem]);
					}
				}
				comboPtr->combopen = qfalse;
				comboPtr->comboItem = -1;
				return qtrue;
			}
		}
	} else if (Rect_ContainsPoint(&Box, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS && item->cvar) {
		if (key == K_MOUSE1 || key == K_ENTER || key == K_JOY1 || key == K_MOUSE2 || key == K_MOUSE3) {
			multiDef_t *multiPtr = (multiDef_t*) item->multiData;
			if (multiPtr) {
				if (multiPtr->count > 0) {
					float h, w, t;
					int i;
					h = w = 0;
					for (i = 0; i < multiPtr->count; i++) {
						t = DC->textWidth(multiPtr->cvarList[i], comboPtr->boxtextscale, 0);
						if (t > w)
							w = t;
						t = DC->textHeight(multiPtr->cvarList[i], comboPtr->boxtextscale, 0);
						if (t > h)
							h = t;
					}
					comboPtr->ComboBox.x = item->window.rect.x + comboPtr->offsetx;
					comboPtr->ComboBox.y = item->window.rect.y + item->window.rect.h + comboPtr->offsety;
					comboPtr->ComboBox.w = w + 20;
					comboPtr->ComboBox.h = (h * multiPtr->count) + (multiPtr->count * BOXTEXTSTEP) + BOXTEXTOFFSET;
					comboPtr->maxheight = h;
					comboPtr->combopen = qtrue;
					return qfalse;
				}
			}
		}
	}

	comboPtr->combopen = qfalse;
	return qfalse;

}
// end changed RD

qboolean Item_YesNo_HandleKey(itemDef_t *item, int key) {

	if (item->cvar) {
		qboolean action = qfalse;
		if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_MOUSE3) {
			if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS) {
				action = qtrue;
			}
		} else if (UI_SelectForKey(key) != 0) {
			action = qtrue;
		}
		if (action) {
			DC->setCVar(item->cvar, va("%i", !DC->getCVarValue(item->cvar)));
			return qtrue;
		}
	}
	return qfalse;
}

int Item_Multi_CountSettings(itemDef_t *item) {
	multiDef_t *multiPtr = (multiDef_t*) item->typeData;
	if (multiPtr == NULL) {
		return 0;
	}
	return multiPtr->count;
}

int Item_Multi_FindCvarByValue(itemDef_t *item) {
	char buff[1024];
	float value = 0;
	int i;
	multiDef_t *multiPtr = (multiDef_t*) item->typeData;
	if (multiPtr) {
		if (multiPtr->strDef) {
			DC->getCVarString(item->cvar, buff, sizeof (buff));
		} else {
			value = DC->getCVarValue(item->cvar);
		}
		for (i = 0; i < multiPtr->count; i++) {
			if (multiPtr->strDef) {
				if (Q_strequal(buff, multiPtr->cvarStr[i])) {
					return i;
				}
			} else {
				if (multiPtr->cvarValue[i] == value) {
					return i;
				}
			}
		}
	}
	return 0;
}

const char *Item_Multi_Setting(itemDef_t *item) {
	char buff[1024];
	float value = 0;
	int i;
	multiDef_t *multiPtr = (multiDef_t*) item->typeData;
	if (multiPtr) {
		if (multiPtr->strDef) {
			DC->getCVarString(item->cvar, buff, sizeof (buff));
		} else {
			value = DC->getCVarValue(item->cvar);
		}
		for (i = 0; i < multiPtr->count; i++) {
			if (multiPtr->strDef) {
				if (Q_strequal(buff, multiPtr->cvarStr[i])) {
					return multiPtr->cvarList[i];
				}
			} else {
				if (multiPtr->cvarValue[i] == value) {
					return multiPtr->cvarList[i];
				}
			}
		}
	}
	return "";
}

qboolean Item_Multi_HandleKey(itemDef_t *item, int key) {
	multiDef_t *multiPtr = (multiDef_t*) item->typeData;
	if (multiPtr) {
		if (item->cvar) {
			int select = 0;
			if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_MOUSE3) {
				if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS) {
					select = (key == K_MOUSE2) ? -1 : 1;
				}
			} else {
				select = UI_SelectForKey(key);
			}
			if (select != 0) {
				int current = Item_Multi_FindCvarByValue(item) + select;
				int max = Item_Multi_CountSettings(item);
				if ( current < 0 ) {
					current = max-1;
				} else if ( current >= max ) {
					current = 0;
				}
				if (multiPtr->strDef) {
					DC->setCVar(item->cvar, multiPtr->cvarStr[current]);
				} else {
					float value = multiPtr->cvarValue[current];
					if (((float) ((int) value)) == value) {
						DC->setCVar(item->cvar, va("%i", (int) value));
					} else {
						DC->setCVar(item->cvar, va("%f", value));
					}
				}
				return qtrue;
			}
		}
	}
	return qfalse;
}

qboolean Item_TextField_HandleKey(itemDef_t *item, int key) {
	char buff[1024];
	int len;
	itemDef_t *newItem = NULL;
	editFieldDef_t *editPtr = (editFieldDef_t*) item->typeData;

	if (item->cvar) {

		memset(buff, 0, sizeof (buff));
		DC->getCVarString(item->cvar, buff, sizeof (buff));
		len = strlen(buff);
		if (editPtr->maxChars && len > editPtr->maxChars) {
			len = editPtr->maxChars;
		}
		if (key & K_CHAR_FLAG) {
			key &= ~K_CHAR_FLAG;


			if (key == 'h' - 'a' + 1) { // ctrl-h is backspace
				if (item->cursorPos > 0) {
					memmove(&buff[item->cursorPos - 1], &buff[item->cursorPos], len + 1 - item->cursorPos);
					item->cursorPos--;
					if (item->cursorPos < editPtr->paintOffset) {
						editPtr->paintOffset--;
					}
				}
				DC->setCVar(item->cvar, buff);
				return qtrue;
			}


			//
			// ignore any non printable chars
			//
			if (key < 32 || !item->cvar) {
				return qtrue;
			}

			if (item->type == ITEM_TYPE_NUMERICFIELD) {
				if (key < '0' || key > '9') {
					return qfalse;
				}
			}

			if (!DC->getOverstrikeMode()) {
				if ((len == MAX_EDITFIELD - 1) || (editPtr->maxChars && len >= editPtr->maxChars)) {
					return qtrue;
				}
				memmove(&buff[item->cursorPos + 1], &buff[item->cursorPos], len + 1 - item->cursorPos);
			} else {
				if (editPtr->maxChars && item->cursorPos >= editPtr->maxChars) {
					return qtrue;
				}
			}

			buff[item->cursorPos] = key;

			DC->setCVar(item->cvar, buff);

			if (item->cursorPos < len + 1) {
				item->cursorPos++;
				if (editPtr->maxPaintChars && item->cursorPos > editPtr->maxPaintChars) {
					editPtr->paintOffset++;
				}
			}

		} else {

			if (key == K_DEL) {
				if (item->cursorPos < len) {
					memmove(buff + item->cursorPos, buff + item->cursorPos + 1, len - item->cursorPos);
					DC->setCVar(item->cvar, buff);
				}
				return qtrue;
			}

			if (key == K_RIGHTARROW || key == K_JOY30) {
				if (editPtr->maxPaintChars && item->cursorPos >= editPtr->maxPaintChars && item->cursorPos < len) {
					item->cursorPos++;
					editPtr->paintOffset++;
					return qtrue;
				}
				if (item->cursorPos < len) {
					item->cursorPos++;
				}
				return qtrue;
			}

			if (key == K_LEFTARROW || key == K_JOY32) {
				if (item->cursorPos > 0) {
					item->cursorPos--;
				}
				if (item->cursorPos < editPtr->paintOffset) {
					editPtr->paintOffset--;
				}
				return qtrue;
			}

			if (key == K_HOME) {// || ( tolower(key) == 'a' && trap_Key_IsDown( K_CTRL ) ) ) {
				item->cursorPos = 0;
				editPtr->paintOffset = 0;
				return qtrue;
			}

			if (key == K_END) {// ( tolower(key) == 'e' && trap_Key_IsDown( K_CTRL ) ) ) {
				item->cursorPos = len;
				if (item->cursorPos > editPtr->maxPaintChars) {
					editPtr->paintOffset = len - editPtr->maxPaintChars;
				}
				return qtrue;
			}

			if (key == K_INS) {
				DC->setOverstrikeMode(!DC->getOverstrikeMode());
				return qtrue;
			}
		}

		if (key == K_TAB || key == K_DOWNARROW || key == K_JOY31) {
			newItem = Menu_SetNextCursorItem(item->parent);
			if (newItem && (newItem->type == ITEM_TYPE_EDITFIELD || newItem->type == ITEM_TYPE_NUMERICFIELD)) {
				g_editItem = newItem;
			}
		}

		if (key == K_UPARROW || key == K_JOY29) {
			newItem = Menu_SetPrevCursorItem(item->parent);
			if (newItem && (newItem->type == ITEM_TYPE_EDITFIELD || newItem->type == ITEM_TYPE_NUMERICFIELD)) {
				g_editItem = newItem;
			}
		}

		if (key == K_ENTER || key == K_JOY1 || key == K_KP_ENTER || key == K_ESCAPE || key == K_JOY4) {
			return qfalse;
		}

		return qtrue;
	}
	return qfalse;

}

static void Scroll_ListBox_AutoFunc(void *p) {
	scrollInfo_t *si = (scrollInfo_t*) p;
	if (DC->realTime > si->nextScrollTime) {
		// need to scroll which is done by simulating a click to the item
		// this is done a bit sideways as the autoscroll "knows" that the item is a listbox
		// so it calls it directly
		Item_ListBox_HandleKey(si->item, si->scrollKey, qtrue, qfalse);
		si->nextScrollTime = DC->realTime + si->adjustValue;
	}

	if (DC->realTime > si->nextAdjustTime) {
		si->nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
		if (si->adjustValue > SCROLL_TIME_FLOOR) {
			si->adjustValue -= SCROLL_TIME_ADJUSTOFFSET;
		}
	}
}

static void Scroll_ListBox_ThumbFunc(void *p) {
	scrollInfo_t *si = (scrollInfo_t*) p;
	rectDef_t r;
	int pos, max;

	listBoxDef_t *listPtr = (listBoxDef_t*) si->item->typeData;
	if (si->item->window.flags & WINDOW_HORIZONTAL) {
		if (DC->cursorx == si->xStart) {
			return;
		}
		r.x = si->item->window.rect.x + DC->Assets.scrollbarsize + 1;
		r.y = si->item->window.rect.y + si->item->window.rect.h - DC->Assets.scrollbarsize - 1;
		r.h = DC->Assets.scrollbarsize;
		r.w = si->item->window.rect.w - (DC->Assets.scrollbarsize * 2) - 2;
		max = Item_ListBox_MaxScroll(si->item);
		//
		pos = (DC->cursorx - r.x - DC->Assets.scrollbarsize / 2) * max / (r.w - DC->Assets.scrollbarsize);
		if (pos < 0) {
			pos = 0;
		} else if (pos > max) {
			pos = max;
		}
		listPtr->startPos = pos;
		si->xStart = DC->cursorx;
	} else if (DC->cursory != si->yStart) {

		r.x = si->item->window.rect.x + si->item->window.rect.w - DC->Assets.scrollbarsize - 1;
		r.y = si->item->window.rect.y + DC->Assets.scrollbarsize + 1;
		r.h = si->item->window.rect.h - (DC->Assets.scrollbarsize * 2) - 2;
		r.w = DC->Assets.scrollbarsize;
		max = Item_ListBox_MaxScroll(si->item);
		//
		pos = (DC->cursory - r.y - DC->Assets.scrollbarsize / 2) * max / (r.h - DC->Assets.scrollbarsize);
		if (pos < 0) {
			pos = 0;
		} else if (pos > max) {
			pos = max;
		}
		listPtr->startPos = pos;
		si->yStart = DC->cursory;
	}

	if (DC->realTime > si->nextScrollTime) {
		// need to scroll which is done by simulating a click to the item
		// this is done a bit sideways as the autoscroll "knows" that the item is a listbox
		// so it calls it directly
		Item_ListBox_HandleKey(si->item, si->scrollKey, qtrue, qfalse);
		si->nextScrollTime = DC->realTime + si->adjustValue;
	}

	if (DC->realTime > si->nextAdjustTime) {
		si->nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
		if (si->adjustValue > SCROLL_TIME_FLOOR) {
			si->adjustValue -= SCROLL_TIME_ADJUSTOFFSET;
		}
	}
}

static void Scroll_Slider_ThumbFunc(void *p) {
	float x, value, cursorx;
	scrollInfo_t *si = (scrollInfo_t*) p;
	editFieldDef_t *editDef = si->item->typeData;

	if (si->item->text) {
		x = si->item->textRect.x + si->item->textRect.w + 8;
	} else {
		x = si->item->window.rect.x;
	}

	cursorx = DC->cursorx;

	if (cursorx < x) {
		cursorx = x;
	} else if (cursorx > x + DC->Assets.sliderwidth) {
		cursorx = x + DC->Assets.sliderwidth;
	}
	value = cursorx - x;
	value /= DC->Assets.sliderwidth;
	value *= (editDef->maxVal - editDef->minVal);
	value += editDef->minVal;
	DC->setCVar(si->item->cvar, va("%f", value));
}

void Item_StartCapture(itemDef_t *item, int key) {
	int flags;
	switch (item->type) {
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:

		case ITEM_TYPE_LISTBOX:
		{
			flags = Item_ListBox_OverLB(item, DC->cursorx, DC->cursory);
			if (flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW)) {
				scrollInfo.nextScrollTime = DC->realTime + SCROLL_TIME_START;
				scrollInfo.nextAdjustTime = DC->realTime + SCROLL_TIME_ADJUST;
				scrollInfo.adjustValue = SCROLL_TIME_START;
				scrollInfo.scrollKey = key;
				scrollInfo.scrollDir = (flags & WINDOW_LB_LEFTARROW) ? qtrue : qfalse;
				scrollInfo.item = item;
				captureData = &scrollInfo;
				captureFunc = &Scroll_ListBox_AutoFunc;
				itemCapture = item;
			} else if (flags & WINDOW_LB_THUMB) {
				scrollInfo.scrollKey = key;
				scrollInfo.item = item;
				scrollInfo.xStart = DC->cursorx;
				scrollInfo.yStart = DC->cursory;
				captureData = &scrollInfo;
				captureFunc = &Scroll_ListBox_ThumbFunc;
				itemCapture = item;
			}
			break;
		}
		case ITEM_TYPE_SLIDER:
		{
			flags = Item_Slider_OverSlider(item, DC->cursorx, DC->cursory);
			if (flags & WINDOW_LB_THUMB) {
				scrollInfo.scrollKey = key;
				scrollInfo.item = item;
				scrollInfo.xStart = DC->cursorx;
				scrollInfo.yStart = DC->cursory;
				captureData = &scrollInfo;
				captureFunc = &Scroll_Slider_ThumbFunc;
				itemCapture = item;
			}
			break;
		}
	}
}

void Item_StopCapture(itemDef_t *item) {

}

qboolean Item_Slider_HandleKey(itemDef_t *item, int key, qboolean down) {
	float x, value, width, work;

	//DC->Print("slider handle key\n");
	if (item->cvar) {
		if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_MOUSE3) {
			editFieldDef_t *editDef = item->typeData;
			if (editDef && Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory) && item->window.flags & WINDOW_HASFOCUS) {
				rectDef_t testRect;
				width = DC->Assets.sliderwidth;
				if (item->text) {
					x = item->textRect.x + item->textRect.w + 8;
				} else {
					x = item->window.rect.x;
				}

				testRect = item->window.rect;
				testRect.x = x;
				value = (float) DC->Assets.sliderthumbwidth / 2;
				testRect.x -= value;
				testRect.w = (DC->Assets.sliderwidth + (float) DC->Assets.sliderthumbwidth / 2);
				if (Rect_ContainsPoint(&testRect, DC->cursorx, DC->cursory)) {
					work = DC->cursorx - x;
					value = work / width;
					value *= (editDef->maxVal - editDef->minVal);
					value += editDef->minVal;
					DC->setCVar(item->cvar, va("%f", value));
					return qtrue;
				}
			}
		} else {
			int select = UI_SelectForKey(key);
			if (select != 0) {
				editFieldDef_t *editDef = item->typeData;
				if (editDef) {
					// 20 is number of steps
					value = DC->getCVarValue(item->cvar) + (((editDef->maxVal - editDef->minVal)/20) * select);
					if (value < editDef->minVal)
						value = editDef->minVal;
					else if (value > editDef->maxVal)
						value = editDef->maxVal;
					DC->setCVar(item->cvar, va("%f", value));
					return qtrue;
				}
			}
		}
	}
	DC->Print("slider handle key exit\n");
	return qfalse;
}

qboolean Item_HandleKey(itemDef_t *item, int key, qboolean down) {

	if (itemCapture) {
		Item_StopCapture(itemCapture);
		itemCapture = NULL;
		captureFunc = 0;
		captureData = NULL;
	} else {
		// bk001206 - parentheses
		if (down && (key == K_MOUSE1 || key == K_MOUSE2 || key == K_MOUSE3)) {
			Item_StartCapture(item, key);
		}
	}

	if (!down) {
		return qfalse;
	}

	switch (item->type) {
		case ITEM_TYPE_BUTTON:
			return qfalse;
			break;
		case ITEM_TYPE_RADIOBUTTON:
			return qfalse;
			break;
		case ITEM_TYPE_CHECKBOX:
			// Changed RD
			return Item_Checkbox_HandleKey(item, key);
			// end changed RD
			break;
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
			//return Item_TextField_HandleKey(item, key);
			return qfalse;
			break;
		case ITEM_TYPE_COMBO:
			// Changed RD
			return Item_Combo_HandleKey(item, key);
			// end changed RD
			break;
		case ITEM_TYPE_LISTBOX:
			return Item_ListBox_HandleKey(item, key, down, qfalse);
			break;
		case ITEM_TYPE_YESNO:
			return Item_YesNo_HandleKey(item, key);
			break;
		case ITEM_TYPE_MULTI:
			return Item_Multi_HandleKey(item, key);
			break;
		case ITEM_TYPE_OWNERDRAW:
			return Item_OwnerDraw_HandleKey(item, key);
			break;
		case ITEM_TYPE_BIND:
			return Item_Bind_HandleKey(item, key, down);
			break;
		case ITEM_TYPE_SLIDER:
			return Item_Slider_HandleKey(item, key, down);
			break;
			//case ITEM_TYPE_IMAGE:
			//  Item_Image_Paint(item);
			//  break;
		default:
			return qfalse;
			break;
	}

	//return qfalse;
}

void Item_Action(itemDef_t *item) {
	if (item) {
		Item_RunScript(item, item->action);
	}
}

// Changed RD

void Item_TransitionEnd(itemDef_t *item) {
	if (item) {
		Item_RunScript(item, item->transitionEnd);
	}
}
// end changed RD

itemDef_t *Menu_SetPrevCursorItem(menuDef_t *menu) {
	qboolean wrapped = qfalse;
	int oldCursor = menu->cursorItem;

	if (menu->cursorItem < 0) {
		menu->cursorItem = menu->itemCount - 1;
		wrapped = qtrue;
	}

	while (menu->cursorItem > -1) {

		menu->cursorItem--;
		if (menu->cursorItem < 0 && !wrapped) {
			wrapped = qtrue;
			menu->cursorItem = menu->itemCount - 1;
		}

		if (Item_SetFocus(menu->items[menu->cursorItem], DC->cursorx, DC->cursory)) {
			Menu_HandleMouseMove(menu, menu->items[menu->cursorItem]->window.rect.x + 1, menu->items[menu->cursorItem]->window.rect.y + 1);
			return menu->items[menu->cursorItem];
		}
	}
	menu->cursorItem = oldCursor;
	return NULL;

}

itemDef_t *Menu_SetNextCursorItem(menuDef_t *menu) {

	qboolean wrapped = qfalse;
	int oldCursor = menu->cursorItem;


	if (menu->cursorItem == -1) {
		menu->cursorItem = 0;
		wrapped = qtrue;
	}

	while (menu->cursorItem < menu->itemCount) {

		menu->cursorItem++;
		if (menu->cursorItem >= menu->itemCount && !wrapped) {
			wrapped = qtrue;
			menu->cursorItem = 0;
		}
		if (Item_SetFocus(menu->items[menu->cursorItem], DC->cursorx, DC->cursory)) {
			Menu_HandleMouseMove(menu, menu->items[menu->cursorItem]->window.rect.x + 1, menu->items[menu->cursorItem]->window.rect.y + 1);
			return menu->items[menu->cursorItem];
		}

	}

	menu->cursorItem = oldCursor;
	return NULL;
}

static void Window_CloseCinematic(windowDef_t *window) {
	if (window->style == WINDOW_STYLE_CINEMATIC && window->cinematic >= 0) {
		DC->stopCinematic(window->cinematic);
		window->cinematic = -1;
	}
}

static void Menu_CloseCinematics(menuDef_t *menu) {
	if (menu) {
		int i;
		Window_CloseCinematic(&menu->window);
		for (i = 0; i < menu->itemCount; i++) {
			Window_CloseCinematic(&menu->items[i]->window);
			if (menu->items[i]->type == ITEM_TYPE_OWNERDRAW) {
				DC->stopCinematic(0 - menu->items[i]->window.ownerDraw);
			}
		}
	}
}

static void Display_CloseCinematics(void) {
	int i;
	for (i = 0; i < menuCount; i++) {
		Menu_CloseCinematics(&Menus[i]);
	}
}

void Menus_Activate(menuDef_t *menu) {
	menu->window.flags |= (WINDOW_HASFOCUS | WINDOW_VISIBLE);
	if (menu->onOpen) {
		itemDef_t item;
		item.parent = menu;
		item.transitionEnd = menu->opentransitionEnd;
		Item_RunScript(&item, menu->onOpen);
	}

	if (menu->soundName && *menu->soundName) {
		//		DC->stopBackgroundTrack();					// you don't want to do this since it will reset s_rawend
		DC->startBackgroundTrack(menu->soundName, menu->soundName);
	}

	Display_CloseCinematics();

}

int Display_VisibleMenuCount(void) {
	int i, count;
	count = 0;
	for (i = 0; i < menuCount; i++) {
		if (Menus[i].window.flags & (WINDOW_FORCED | WINDOW_VISIBLE)) {
			count++;
		}
	}
	return count;
}

void Menus_HandleOOBClick(menuDef_t *menu, int key, qboolean down) {
	if (menu) {
		int i;
		// basically the behaviour we are looking for is if there are windows in the stack.. see if 
		// the cursor is within any of them.. if not close them otherwise activate them and pass the 
		// key on.. force a mouse move to activate focus and script stuff 
		if (down && menu->window.flags & WINDOW_OOB_CLICK) {
			Menu_RunCloseScript(menu);
			menu->window.flags &= ~(WINDOW_HASFOCUS | WINDOW_VISIBLE);
		}

		for (i = 0; i < menuCount; i++) {
			if (Menu_OverActiveItem(&Menus[i], DC->cursorx, DC->cursory)) {
				Menu_RunCloseScript(menu);
				menu->window.flags &= ~(WINDOW_HASFOCUS | WINDOW_VISIBLE);
				Menus_Activate(&Menus[i]);
				Menu_HandleMouseMove(&Menus[i], DC->cursorx, DC->cursory);
				Menu_HandleKey(&Menus[i], key, down);
			}
		}

		if (Display_VisibleMenuCount() == 0) {
			if (DC->Pause) {
				DC->Pause(qfalse);
			}
		}
		Display_CloseCinematics();
	}
}

static rectDef_t *Item_CorrectedTextRect(itemDef_t *item) {
	static rectDef_t rect;
	memset(&rect, 0, sizeof (rectDef_t));
	if (item) {
		rect = item->textRect;
		if (rect.w) {
			rect.y -= rect.h;
		}
	}
	return &rect;
}

// menu item key horizontal action: -1 = previous value, 1 = next value, 0 = no change
int UI_SelectForKey(int key)
{
	switch (key) {
		case K_MOUSE1:
		case K_MOUSE3:
		case K_ENTER:
		case K_KP_ENTER:
		case K_RIGHTARROW:
		case K_KP_RIGHTARROW:
		case K_JOY1:
		case K_JOY2:
		case K_JOY3:
		case K_JOY4:
			return 1; // next
		case K_MOUSE2:
		case K_LEFTARROW:
		case K_KP_LEFTARROW:
			return -1; // previous
	}
	// no change
	return 0;
}

void Menu_HandleKey(menuDef_t *menu, int key, qboolean down) {
	int i;
	itemDef_t *item = NULL;

	// Changed RD
	if (DC->getCVarValue("ui_transitionkey")) {
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->window.flags & WINDOW_INTRANSITION) {
				return;
			}
		}
	}
	// end changed RD
	if (g_waitingForKey && down) {
		Item_Bind_HandleKey(g_bindItem, key, down);
		return;
	}

	if (g_editingField && down) {
		if (!Item_TextField_HandleKey(g_editItem, key)) {
			g_editingField = qfalse;
			g_editItem = NULL;
			return;
		} else if (key == K_MOUSE1 || key == K_MOUSE2 || key == K_MOUSE3) {
			g_editingField = qfalse;
			g_editItem = NULL;
			Display_MouseMove(NULL, DC->cursorx, DC->cursory);
		} else if (key == K_TAB || key == K_UPARROW || key == K_JOY29 || key == K_JOY31 || key == K_DOWNARROW) {
			return;
		}
	}

	if (menu == NULL) {
		return;
	}

	// see if the mouse is within the window bounds and if so is this a mouse click
	if (down && !(menu->window.flags & WINDOW_POPUP) && !Rect_ContainsPoint(&menu->window.rect, DC->cursorx, DC->cursory)) {
		static qboolean inHandleKey = qfalse;
		// bk001206 - parentheses
		if (!inHandleKey && (key == K_MOUSE1 || key == K_MOUSE2 || key == K_MOUSE3)) {
			inHandleKey = qtrue;
			Menus_HandleOOBClick(menu, key, down);
			inHandleKey = qfalse;
			return;
		}
	}

	// get the item with focus
	for (i = 0; i < menu->itemCount; i++) {
		if (menu->items[i]->window.flags & WINDOW_HASFOCUS) {
			item = menu->items[i];
		}
	}

	if (item != NULL) {
		if (Item_HandleKey(item, key, down)) {
			Item_Action(item);
			return;
		}
	}

	if (!down) {
		return;
	}

	// default handling
	switch (key) {

		case K_F11:
			if (DC->getCVarValue("developer")) {
				debugMode ^= 1;
			}
			break;

		case K_F12:
			if (DC->getCVarValue("developer")) {
				DC->executeText(EXEC_APPEND, "screenshot\n");
			}
			break;
		case K_UPARROW:
			Menu_SetPrevCursorItem(menu);
			break;
		case K_JOY29:
			Menu_SetPrevCursorItem(menu);
			break;

		case K_ESCAPE:
			if (!g_waitingForKey && menu->onESC) {
				itemDef_t it;
				it.parent = menu;
				it.transitionEnd = menu->esctransitionEnd;
				Item_RunScript(&it, menu->onESC);
			}
			break;
		case K_JOY4:
			if (!g_waitingForKey && menu->onESC) {
				itemDef_t it;
				it.parent = menu;
				it.transitionEnd = menu->esctransitionEnd;
				Item_RunScript(&it, menu->onESC);
			}
			break;
		case K_TAB:
		case K_DOWNARROW:
			Menu_SetNextCursorItem(menu);
			break;
		case K_JOY31:
			Menu_SetNextCursorItem(menu);
			break;
		case K_MOUSE1:
		case K_MOUSE2:
			if (item) {
				if (item->type == ITEM_TYPE_TEXT) {
					if (Rect_ContainsPoint(Item_CorrectedTextRect(item), DC->cursorx, DC->cursory)) {
						Item_Action(item);
					}
				} else if (item->type == ITEM_TYPE_EDITFIELD || item->type == ITEM_TYPE_NUMERICFIELD) {
					if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory)) {
						item->cursorPos = 0;
						g_editingField = qtrue;
						g_editItem = item;
					}
				} else {
					// Changed RD
					if (item->type != ITEM_TYPE_COMBO) {
						if (Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory)) {
							if (item->type == ITEM_TYPE_LISTBOX) {
								if (!(item->window.flags & (WINDOW_LB_LEFTARROW | WINDOW_LB_RIGHTARROW | WINDOW_LB_PGUP | WINDOW_LB_PGDN | WINDOW_LB_THUMB | WINDOW_FOCUSDISABLE))) {
									Item_Action(item);
								}
							} else {
								Item_Action(item);
							}
						}
					}
					// end changed RD
				}
			}
			break;

			//		case K_JOY1:
			//		case K_JOY2:
			//		case K_JOY3:
			//		case K_JOY4:
		case K_AUX1:
		case K_AUX2:
		case K_AUX3:
		case K_AUX4:
		case K_AUX5:
		case K_AUX6:
		case K_AUX7:
		case K_AUX8:
		case K_AUX9:
		case K_AUX10:
		case K_AUX11:
		case K_AUX12:
		case K_AUX13:
		case K_AUX14:
		case K_AUX15:
		case K_AUX16:
		case K_JOY1:
		case K_KP_ENTER:
		case K_ENTER:
			if (item) {
				if (item->type == ITEM_TYPE_EDITFIELD || item->type == ITEM_TYPE_NUMERICFIELD) {
					item->cursorPos = 0;
					g_editingField = qtrue;
					g_editItem = item;
				} else {
					Item_Action(item);
				}
			}
			break;
	}
}

void ToWindowCoords(float *x, float *y, windowDef_t *window) {
	if (window->border != 0) {
		*x += window->borderSize;
		*y += window->borderSize;
	}
	*x += window->rect.x;
	*y += window->rect.y;
}

void Rect_ToWindowCoords(rectDef_t *rect, windowDef_t *window) {
	ToWindowCoords(&rect->x, &rect->y, window);
}

void Item_SetTextExtents(itemDef_t *item, int *width, int *height, const char *text) {
	const char *textPtr = (text) ? text : item->text;

	if (textPtr == NULL) {
		return;
	}

	*width = item->textRect.w;
	*height = item->textRect.h;

	// keeps us from computing the widths and heights more than once
	if (*width == 0 || (item->type == ITEM_TYPE_OWNERDRAW && item->textalignment == ITEM_ALIGN_CENTER)) {
		int originalWidth = DC->textWidth(item->text, item->textscale, 0);

		if (item->type == ITEM_TYPE_OWNERDRAW && (item->textalignment == ITEM_ALIGN_CENTER || item->textalignment == ITEM_ALIGN_RIGHT)) {
			originalWidth += DC->ownerDrawWidth(item->window.ownerDraw, item->textscale);
		} else if (item->type == ITEM_TYPE_EDITFIELD && item->textalignment == ITEM_ALIGN_CENTER && item->cvar) {
			char buff[256];
			DC->getCVarString(item->cvar, buff, 256);
			originalWidth += DC->textWidth(buff, item->textscale, 0);
		}

		*width = DC->textWidth(textPtr, item->textscale, 0);
		*height = DC->textHeight(textPtr, item->textscale, 0);
		item->textRect.w = *width;
		item->textRect.h = *height;
		item->textRect.x = item->textalignx;
		item->textRect.y = item->textaligny;
		if (item->textalignment == ITEM_ALIGN_RIGHT) {
			item->textRect.x = item->textalignx - originalWidth;
		} else if (item->textalignment == ITEM_ALIGN_CENTER) {
			item->textRect.x = item->textalignx - originalWidth / 2;
		}

		ToWindowCoords(&item->textRect.x, &item->textRect.y, &item->window);
	}
}

void Item_TextColor(itemDef_t *item, vec4_t *newColor) {
	vec4_t lowLight;
	menuDef_t *parent = (menuDef_t*) item->parent;

	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	// Changed RD
	if (item->window.flags & WINDOW_HASFOCUS && !item->nofocuscolor) {
		// end changed RD
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor, lowLight, *newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else if (item->textStyle == ITEM_TEXTSTYLE_BLINK && !((DC->realTime / BLINK_DIVISOR) & 1)) {
		lowLight[0] = 0.8 * item->window.foreColor[0];
		lowLight[1] = 0.8 * item->window.foreColor[1];
		lowLight[2] = 0.8 * item->window.foreColor[2];
		lowLight[3] = 0.8 * item->window.foreColor[3];
		LerpColor(item->window.foreColor, lowLight, *newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else {
		memcpy(newColor, &item->window.foreColor, sizeof (vec4_t));
		// items can be enabled and disabled based on cvars
	}

	if (item->enableCvar && *item->enableCvar && item->cvarTest && *item->cvarTest) {
		if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
			memcpy(newColor, &parent->disableColor, sizeof (vec4_t));
		}
	}
}


// oh boy
// Changed RD
// taken from the Tremulous source code

int Item_Text_AutoWrapped_Lines(itemDef_t *item) {
	char text[ 1024 ];
	const char *p, *textPtr, *newLinePtr;
	char buff[ 1024 ];
	int len, textWidth, newLine;
	int lines = 0;

	textWidth = 0;
	newLinePtr = NULL;

	if (item->text == NULL) {
		if (item->cvar == NULL)
			return 0;
		else {
			DC->getCVarString(item->cvar, text, sizeof ( text));
			textPtr = text;
		}
	} else
		textPtr = item->text;

	if (*textPtr == '\0')
		return 0;

	len = 0;
	buff[ 0 ] = '\0';
	newLine = 0;
	p = textPtr;

	while (p) {
		textWidth = DC->textWidth(buff, item->textscale, 0);

		if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\0') {
			newLine = len;
			newLinePtr = p + 1;
		}

		//TA: forceably split lines that are too long (where normal splitage has failed)
		if (textWidth > item->window.rect.w && newLine == 0 && *p != '\n') {
			newLine = len;
			newLinePtr = p;
		}

		if ((newLine && textWidth > item->window.rect.w) || *p == '\n' || *p == '\0') {
			if (len)
				buff[ newLine ] = '\0';

			if (!(*p == '\n' && !*(p + 1)))
				lines++;

			if (*p == '\0')
				break;

			//
			p = newLinePtr;
			len = 0;
			newLine = 0;

			continue;
		}

		buff[ len++ ] = *p++;
		buff[ len ] = '\0';
	}

	return lines;
}

#define MAX_AUTOWRAP_CACHE  16
#define MAX_AUTOWRAP_LINES  32
#define MAX_AUTOWRAP_TEXT   512

typedef struct {
	//this is used purely for checking for cache hits
	char text[ MAX_AUTOWRAP_TEXT * MAX_AUTOWRAP_LINES ];
	rectDef_t rect;
	int textWidth, textHeight;
	char lines[ MAX_AUTOWRAP_LINES ][ MAX_AUTOWRAP_TEXT ];
	int lineOffsets[ MAX_AUTOWRAP_LINES ][ 2 ];
	int numLines;
} autoWrapCache_t;

static int cacheIndex = 0;
static autoWrapCache_t awc[ MAX_AUTOWRAP_CACHE ];

static int checkCache(const char *text, rectDef_t *rect, int width, int height) {
	int i;

	for (i = 0; i < MAX_AUTOWRAP_CACHE; i++) {
		if (Q_stricmp(text, awc[ i ].text))
			continue;

		if (rect->x != awc[ i ].rect.x ||
				rect->y != awc[ i ].rect.y ||
				rect->w != awc[ i ].rect.w ||
				rect->h != awc[ i ].rect.h)
			continue;

		if (awc[ i ].textWidth != width || awc[ i ].textHeight != height)
			continue;

		//this is a match
		return i;
	}

	//no match - autowrap isn't cached
	return -1;
}

void Item_Text_AutoWrapped_Paint(itemDef_t *item) {
	char text[ 1024 ];
	const char *p, *textPtr, *newLinePtr;
	char buff[ 1024 ];
	char lastCMod[ 2 ] = {0, 0};
	qboolean forwardColor = qfalse;
	int width, height, len, textWidth, newLine, newLineWidth;
	int skipLines, totalLines, lineNum = 0;
	float y, totalY, diffY;
	vec4_t color;
	int cache, i;

	textWidth = 0;
	newLinePtr = NULL;

	if (item->text == NULL) {
		if (item->cvar == NULL)
			return;
		else {
			DC->getCVarString(item->cvar, text, sizeof ( text));
			textPtr = text;
		}
	} else
		textPtr = item->text;

	if (*textPtr == '\0')
		return;

	Item_TextColor(item, &color);
	Item_SetTextExtents(item, &width, &height, textPtr);

	//check if this block is cached
	cache = checkCache(textPtr, &item->window.rect, width, height);
	if (cache >= 0) {
		lineNum = awc[ cache ].numLines;

		for (i = 0; i < lineNum; i++) {
			item->textRect.x = awc[ cache ].lineOffsets[ i ][ 0 ];
			item->textRect.y = awc[ cache ].lineOffsets[ i ][ 1 ];

			DC->drawText(item->textRect.x, item->textRect.y, item->textscale, color,
					awc[ cache ].lines[ i ], 0, 0, item->textStyle);
		}
	} else {
		y = item->textaligny;
		len = 0;
		buff[ 0 ] = '\0';
		newLine = 0;
		newLineWidth = 0;
		p = textPtr;

		totalLines = Item_Text_AutoWrapped_Lines(item);

		totalY = totalLines * (height + 5);
		diffY = totalY - item->window.rect.h;

		if (diffY > 0.0f)
			skipLines = (int) (diffY / ((float) height + 5.0f));
		else
			skipLines = 0;

		//set up a cache entry
		strcpy(awc[ cacheIndex ].text, textPtr);
		awc[ cacheIndex ].rect.x = item->window.rect.x;
		awc[ cacheIndex ].rect.y = item->window.rect.y;
		awc[ cacheIndex ].rect.w = item->window.rect.w;
		awc[ cacheIndex ].rect.h = item->window.rect.h;
		awc[ cacheIndex ].textWidth = width;
		awc[ cacheIndex ].textHeight = height;

		while (p) {
			textWidth = DC->textWidth(buff, item->textscale, 0);

			if (*p == '^') {
				lastCMod[ 0 ] = p[ 0 ];
				lastCMod[ 1 ] = p[ 1 ];
			}

			if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\0') {
				newLine = len;
				newLinePtr = p + 1;
				newLineWidth = textWidth;

				if (*p == '\n') //don't forward colours past deilberate \n's
					lastCMod[ 0 ] = lastCMod[ 1 ] = 0;
				else
					forwardColor = qtrue;
			}

			//TA: forceably split lines that are too long (where normal splitage has failed)
			if (textWidth > item->window.rect.w && newLine == 0 && *p != '\n') {
				newLine = len;
				newLinePtr = p;
				newLineWidth = textWidth;

				forwardColor = qtrue;
			}

			if ((newLine && textWidth > item->window.rect.w) || *p == '\n' || *p == '\0') {
				if (len) {
					if (item->textalignment == ITEM_ALIGN_LEFT)
						item->textRect.x = item->textalignx;
					else if (item->textalignment == ITEM_ALIGN_RIGHT)
						item->textRect.x = item->textalignx - newLineWidth;
					else if (item->textalignment == ITEM_ALIGN_CENTER)
						item->textRect.x = item->textalignx - newLineWidth / 2;

					item->textRect.y = y;
					ToWindowCoords(&item->textRect.x, &item->textRect.y, &item->window);
					//
					buff[ newLine ] = '\0';

					if (!skipLines) {
						DC->drawText(item->textRect.x, item->textRect.y, item->textscale, color, buff, 0, 0, item->textStyle);

						strcpy(awc[ cacheIndex ].lines[ lineNum ], buff);
						awc[ cacheIndex ].lineOffsets[ lineNum ][ 0 ] = item->textRect.x;
						awc[ cacheIndex ].lineOffsets[ lineNum ][ 1 ] = item->textRect.y;

						lineNum++;
					}
				}
				if (*p == '\0')
					break;

				//
				if (!skipLines)
					y += height + 5;

				if (skipLines)
					skipLines--;

				p = newLinePtr;
				len = 0;
				newLine = 0;
				newLineWidth = 0;

				if (forwardColor && lastCMod[ 0 ] != 0) {
					buff[ len++ ] = lastCMod[ 0 ];
					buff[ len++ ] = lastCMod[ 1 ];
					buff[ len ] = '\0';

					forwardColor = qfalse;
				}

				continue;
			}

			buff[ len++ ] = *p++;
			buff[ len ] = '\0';
		}

		//mark the end of the lines list
		awc[ cacheIndex ].numLines = lineNum;

		//increment cacheIndex
		cacheIndex = (cacheIndex + 1) % MAX_AUTOWRAP_CACHE;
	}
}
// end changed RD

void Item_Text_Wrapped_Paint(itemDef_t *item) {
	char text[1024];
	const char *p, *start, *textPtr;
	char buff[1024];
	int width, height;
	float x, y;
	vec4_t color;

	// now paint the text and/or any optional images
	// default to left

	if (item->text == NULL) {
		if (item->cvar == NULL) {
			return;
		} else {
			DC->getCVarString(item->cvar, text, sizeof (text));
			textPtr = text;
		}
	} else {
		textPtr = item->text;
	}
	if (*textPtr == '\0') {
		return;
	}

	Item_TextColor(item, &color);
	Item_SetTextExtents(item, &width, &height, textPtr);

	x = item->textRect.x;
	y = item->textRect.y;
	start = textPtr;
	p = strchr(textPtr, '\r');
	while (p && *p) {
		Q_strncpyz(buff, start, p - start + 1);
		DC->drawText(x, y, item->textscale, color, buff, 0, 0, item->textStyle);
		y += height + 5;
		start += p - start + 1;
		p = strchr(p + 1, '\r');
	}
	DC->drawText(x, y, item->textscale, color, start, 0, 0, item->textStyle);
}

void Item_Text_Paint(itemDef_t *item) {
	char text[1024];
	const char *textPtr;
	int height, width;
	vec4_t color;

	if (item->window.flags & WINDOW_WRAPPED) {
		Item_Text_Wrapped_Paint(item);
		return;
	}
	if (item->window.flags & WINDOW_AUTOWRAPPED) {
		Item_Text_AutoWrapped_Paint(item);
		return;
	}

	if (item->text == NULL) {
		if (item->cvar == NULL) {
			return;
		} else {
			DC->getCVarString(item->cvar, text, sizeof (text));
			textPtr = text;
		}
	} else {
		textPtr = item->text;
	}

	// this needs to go here as it sets extents for cvar types as well
	Item_SetTextExtents(item, &width, &height, textPtr);

	if (*textPtr == '\0') {
		return;
	}


	Item_TextColor(item, &color);

	DC->drawText(item->textRect.x, item->textRect.y, item->textscale, color, textPtr, 0, 0, item->textStyle);
}

// end rd?

void Item_TextField_Paint(itemDef_t *item) {
	char buff[1024];
	vec4_t newColor, lowLight;
	int offset;
	menuDef_t *parent = (menuDef_t*) item->parent;
	editFieldDef_t *editPtr = (editFieldDef_t*) item->typeData;

	Item_Text_Paint(item);

	buff[0] = '\0';

	if (item->cvar) {
		DC->getCVarString(item->cvar, buff, sizeof (buff));
	}

	parent = (menuDef_t*) item->parent;
	// Changed RD
	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	if (item->window.flags & WINDOW_HASFOCUS && !item->nofocuscolor) {
		// end changed RD
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor, lowLight, newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else {
		memcpy(&newColor, &item->window.foreColor, sizeof (vec4_t));
	}

	offset = (item->text && *item->text) ? 8 : 0;
	if (item->window.flags & WINDOW_HASFOCUS && g_editingField) {
		char cursor = DC->getOverstrikeMode() ? '_' : '|';
		DC->drawTextWithCursor(item->textRect.x + item->textRect.w + offset, item->textRect.y, item->textscale, newColor, buff + editPtr->paintOffset, item->cursorPos - editPtr->paintOffset, cursor, editPtr->maxPaintChars, item->textStyle);
	} else {
		DC->drawText(item->textRect.x + item->textRect.w + offset, item->textRect.y, item->textscale, newColor, buff + editPtr->paintOffset, 0, editPtr->maxPaintChars, item->textStyle);
	}

}


// Changed RD

void Item_Checkbox_Paint(itemDef_t *item) {
	vec4_t newColor, lowLight;
	float value, x, y, t;
	menuDef_t *parent = (menuDef_t*) item->parent;

	value = (item->cvar) ? DC->getCVarValue(item->cvar) : 0;

	if (item->window.flags & WINDOW_HASFOCUS && !item->nofocuscolor) {
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor, lowLight, newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else {
		memcpy(&newColor, &item->window.foreColor, sizeof (vec4_t));
	}

	t = (item->window.rect.h - DC->Assets.checkboxheight) / 2.0f;
	y = item->window.rect.y + t;
	x = item->window.rect.x;
	if (item->alignment == ITEM_ALIGN_CENTER) {
		x = x + ((item->window.rect.w / 2.0f)-(DC->Assets.checkboxwidth / 2.0f));
	} else if (item->alignment == ITEM_ALIGN_RIGHT) {
		x = x + item->window.rect.w - DC->Assets.checkboxwidth;
	}

	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	DC->setColor(item->window.foreColor);

	DC->drawHandlePic(x, y, DC->Assets.checkboxwidth, DC->Assets.checkboxheight, (value != 0) ? DC->Assets.checkbox_sel : DC->Assets.checkbox);

	if (item->text) {
		Item_Text_Paint(item);
	}
}

void Item_Combo_Paint(itemDef_t *item) {
	float value = 0.0f;
	float x, y, t;
	char buff[1024];
	const char *textPtr = "";
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;
	menuDef_t *parent = (menuDef_t*) item->parent;

	t = (item->window.rect.h - DC->Assets.comboheight) / 2.0f;
	y = item->window.rect.y + t;
	x = item->window.rect.x + 2;
	if (item->alignment == ITEM_ALIGN_RIGHT) {
		x = item->window.rect.x + item->window.rect.w - DC->Assets.combowidth + 2;
	}

	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	DC->setColor(item->window.foreColor);

	DC->drawHandlePic(x, y, DC->Assets.combowidth, DC->Assets.comboheight, (comboPtr->combopen) ? DC->Assets.combo_sel : DC->Assets.combo);

	if (item->cvar) {
		multiDef_t *multiPtr = (multiDef_t*) item->multiData;
		int i;
		if (multiPtr) {
			if (multiPtr->strDef) {
				DC->getCVarString(item->cvar, buff, sizeof (buff));
			} else {
				value = DC->getCVarValue(item->cvar);
			}
			for (i = 0; i < multiPtr->count; i++) {
				if (multiPtr->strDef) {
					if (Q_stricmp(buff, multiPtr->cvarStr[i]) == 0) {
						textPtr = multiPtr->cvarList[i];
						break;
					}
				} else {
					if (multiPtr->cvarValue[i] == value) {
						textPtr = multiPtr->cvarList[i];
						break;
					}
				}
			}
			t = DC->textWidth(textPtr, item->textscale, 0);
			y = item->window.rect.y + item->textaligny;
			x = item->window.rect.x + item->textalignx;
			if (item->textalignment == ITEM_ALIGN_RIGHT) {
				x = x - t;
			}
			if (item->textalignment == ITEM_ALIGN_CENTER) {
				x = x - (t / 2);
			}
			DC->drawText(x, y, item->textscale, item->window.foreColor, textPtr, 0, 0, item->textStyle);
		}
	}

}

void Item_ComboBox_Paint(itemDef_t *item) {
	float x, y, h, w;
	vec4_t newColor, lowLight;
	int i;
	multiDef_t *multiPtr = (multiDef_t*) item->multiData;
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;
	menuDef_t *parent = (menuDef_t*) item->parent;

	x = comboPtr->ComboBox.x;
	y = comboPtr->ComboBox.y;
	w = comboPtr->ComboBox.w;
	h = comboPtr->ComboBox.h;

	DC->fillRect(x, y, w, h, comboPtr->boxcolor);

	lowLight[0] = 0.8 * parent->focusColor[0];
	lowLight[1] = 0.8 * parent->focusColor[1];
	lowLight[2] = 0.8 * parent->focusColor[2];
	lowLight[3] = 0.8 * parent->focusColor[3];
	LerpColor(parent->focusColor, lowLight, newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));

	if (multiPtr) {
		x += 4;
		y += BOXTEXTOFFSET;
		for (i = 0; i < multiPtr->count; i++) {
			y += comboPtr->maxheight;
			if (i == comboPtr->comboItem) {
				DC->drawText(x, y, comboPtr->boxtextscale, newColor, multiPtr->cvarList[i], 0, 0, item->textStyle);
			} else {
				DC->drawText(x, y, comboPtr->boxtextscale, comboPtr->boxtextcolor, multiPtr->cvarList[i], 0, 0, item->textStyle);
			}
			y += BOXTEXTSTEP;
		}
	}
}
// end changed RD

void Item_YesNo_Paint(itemDef_t *item) {
	vec4_t newColor, lowLight;
	float value;
	menuDef_t *parent = (menuDef_t*) item->parent;

	value = (item->cvar) ? DC->getCVarValue(item->cvar) : 0;
	// Changed RD
	if (item->window.flags & WINDOW_HASFOCUS && !item->nofocuscolor) {
		// end changed RD
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor, lowLight, newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else {
		memcpy(&newColor, &item->window.foreColor, sizeof (vec4_t));
	}

	if (item->text) {
		Item_Text_Paint(item);
		DC->drawText(item->textRect.x + item->textRect.w + 8, item->textRect.y, item->textscale, newColor, (value != 0) ? "Yes" : "No", 0, 0, item->textStyle);
	} else {
		DC->drawText(item->textRect.x, item->textRect.y, item->textscale, newColor, (value != 0) ? "Yes" : "No", 0, 0, item->textStyle);
	}
}

void Item_Multi_Paint(itemDef_t *item) {
	vec4_t newColor, lowLight;
	const char *text = "";
	menuDef_t *parent = (menuDef_t*) item->parent;

	// Changed RD
	if (item->window.flags & WINDOW_HASFOCUS && !item->nofocuscolor) {
		// end changed RD
		lowLight[0] = 0.8 * parent->focusColor[0];
		lowLight[1] = 0.8 * parent->focusColor[1];
		lowLight[2] = 0.8 * parent->focusColor[2];
		lowLight[3] = 0.8 * parent->focusColor[3];
		LerpColor(parent->focusColor, lowLight, newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else {
		memcpy(&newColor, &item->window.foreColor, sizeof (vec4_t));
	}

	text = Item_Multi_Setting(item);

	if (item->text) {
		Item_Text_Paint(item);
		DC->drawText(item->textRect.x + item->textRect.w + 8, item->textRect.y, item->textscale, newColor, text, 0, 0, item->textStyle);
	} else {
		DC->drawText(item->textRect.x, item->textRect.y, item->textscale, newColor, text, 0, 0, item->textStyle);
	}
}

typedef struct {
	char *command;
	int id;
	int defaultbind1;
	int defaultbind2;
	int bind1;
	int bind2;
} bind_t;

static bind_t g_bindings[] ={
	{"+scores", K_TAB, -1, -1, -1},
	{"+button2", K_ENTER, -1, -1, -1},
	{"+speed", K_SHIFT, -1, -1, -1},
	{"+forward", K_UPARROW, -1, -1, -1},
	{"+back", K_DOWNARROW, -1, -1, -1},
	{"+moveleft", ',', -1, -1, -1},
	{"+moveright", '.', -1, -1, -1},
	{"+moveup", K_SPACE, -1, -1, -1},
	{"+movedown", 'c', -1, -1, -1},
	{"+left", K_LEFTARROW, -1, -1, -1},
	{"+right", K_RIGHTARROW, -1, -1, -1},
	{"+strafe", K_ALT, -1, -1, -1},
	{"+lookup", K_PGDN, -1, -1, -1},
	{"+lookdown", K_DEL, -1, -1, -1},
	{"+mlook", '/', -1, -1, -1},
	{"centerview", K_END, -1, -1, -1},
	{"+zoom", -1, -1, -1, -1},
	{"weapon 1", '1', -1, -1, -1},
	{"weapon 2", '2', -1, -1, -1},
	{"weapon 3", '3', -1, -1, -1},
	{"weapon 4", '4', -1, -1, -1},
	{"weapon 5", '5', -1, -1, -1},
	{"weapon 6", '6', -1, -1, -1},
	{"weapon 7", '7', -1, -1, -1},
	{"weapon 8", '8', -1, -1, -1},
	{"weapon 9", '9', -1, -1, -1},
	{"weapon 10", '0', -1, -1, -1},
	{"weapon 11", -1, -1, -1, -1},
	{"weapon 12", -1, -1, -1, -1},
	{"weapon 13", -1, -1, -1, -1},
	{"+attack", K_CTRL, -1, -1, -1},
	{"weapprev", '[', -1, -1, -1},
	{"weapnext", ']', -1, -1, -1},
	{"+button3", K_MOUSE3, -1, -1, -1},
	{"+button4", K_MOUSE4, -1, -1, -1},
	{"prevTeamMember", 'w', -1, -1, -1},
	{"nextTeamMember", 'r', -1, -1, -1},
	{"nextOrder", 't', -1, -1, -1},
	{"confirmOrder", 'y', -1, -1, -1},
	{"denyOrder", 'n', -1, -1, -1},
	{"taskOffense", 'o', -1, -1, -1},
	{"taskDefense", 'd', -1, -1, -1},
	{"taskPatrol", 'p', -1, -1, -1},
	{"taskCamp", 'c', -1, -1, -1},
	{"taskFollow", 'f', -1, -1, -1},
	{"taskRetrieve", 'v', -1, -1, -1},
	{"taskEscort", 'e', -1, -1, -1},
	{"taskOwnFlag", 'i', -1, -1, -1},
	{"taskSuicide", 'k', -1, -1, -1},
	{"tauntKillInsult", K_F1, -1, -1, -1},
	{"tauntPraise", K_F2, -1, -1, -1},
	{"tauntTaunt", K_F3, -1, -1, -1},
	{"tauntDeathInsult", K_F4, -1, -1, -1},
	{"tauntGauntlet", K_F5, -1, -1, -1},
	{"scoresUp", K_KP_PGUP, -1, -1, -1},
	{"scoresDown", K_KP_PGDN, -1, -1, -1},
	// bk001205 - this one below was:  '-1' 
	{"messagemode", -1, -1, -1, -1},
	{"messagemode2", -1, -1, -1, -1},
	{"messagemode3", -1, -1, -1, -1},
	{"messagemode4", -1, -1, -1, -1}
};


static const int g_bindCount = sizeof (g_bindings) / sizeof (bind_t);


/*
=================
Controls_GetKeyAssignment
=================
 */
static void Controls_GetKeyAssignment(char *command, int *twokeys) {
	int count;
	int j;
	char b[256];

	twokeys[0] = twokeys[1] = -1;
	count = 0;

	for (j = 0; j < 256; j++) {
		DC->getBindingBuf(j, b, 256);
		if (*b == 0) {
			continue;
		}
		if (Q_strequal(b, command)) {
			twokeys[count] = j;
			count++;
			if (count == 2) {
				break;
			}
		}
	}
}

/*
=================
Controls_GetConfig
=================
 */
void Controls_GetConfig(void) {
	int i;
	int twokeys[2];

	// iterate each command, get its numeric binding
	for (i = 0; i < g_bindCount; i++) {

		Controls_GetKeyAssignment(g_bindings[i].command, twokeys);

		g_bindings[i].bind1 = twokeys[0];
		g_bindings[i].bind2 = twokeys[1];
	}

	//s_controls.invertmouse.curvalue  = DC->getCVarValue( "m_pitch" ) < 0;
	//s_controls.smoothmouse.curvalue  = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "m_filter" ) );
	//s_controls.alwaysrun.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cl_run" ) );
	//s_controls.autoswitch.curvalue   = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cg_autoswitch" ) );
	//s_controls.sensitivity.curvalue  = UI_ClampCvar( 2, 30, Controls_GetCvarValue( "sensitivity" ) );
	//s_controls.joyenable.curvalue    = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "in_joystick" ) );
	//s_controls.joythreshold.curvalue = UI_ClampCvar( 0.05, 0.75, Controls_GetCvarValue( "joy_threshold" ) );
	//s_controls.freelook.curvalue     = UI_ClampCvar( 0, 1, Controls_GetCvarValue( "cl_freelook" ) );
}

/*
=================
Controls_SetConfig
=================
 */
void Controls_SetConfig(qboolean restart) {
	int i;

	// iterate each command, get its numeric binding
	for (i = 0; i < g_bindCount; i++) {

		if (g_bindings[i].bind1 != -1) {
			DC->setBinding(g_bindings[i].bind1, g_bindings[i].command);

			if (g_bindings[i].bind2 != -1)
				DC->setBinding(g_bindings[i].bind2, g_bindings[i].command);
		}
	}

	//if ( s_controls.invertmouse.curvalue )
	//	DC->setCVar("m_pitch", va("%f),-fabs( DC->getCVarValue( "m_pitch" ) ) );
	//else
	//	trap_Cvar_SetValue( "m_pitch", fabs( trap_Cvar_VariableValue( "m_pitch" ) ) );

	//trap_Cvar_SetValue( "m_filter", s_controls.smoothmouse.curvalue );
	//trap_Cvar_SetValue( "cl_run", s_controls.alwaysrun.curvalue );
	//trap_Cvar_SetValue( "cg_autoswitch", s_controls.autoswitch.curvalue );
	//trap_Cvar_SetValue( "sensitivity", s_controls.sensitivity.curvalue );
	//trap_Cvar_SetValue( "in_joystick", s_controls.joyenable.curvalue );
	//trap_Cvar_SetValue( "joy_threshold", s_controls.joythreshold.curvalue );
	//trap_Cvar_SetValue( "cl_freelook", s_controls.freelook.curvalue );
	DC->executeText(EXEC_APPEND, "in_restart\n");
	//trap_Cmd_ExecuteText( EXEC_APPEND, "in_restart\n" );
}

/*
=================
Controls_SetDefaults
=================
 */
void Controls_SetDefaults(void) {
	int i;

	// iterate each command, set its default binding
	for (i = 0; i < g_bindCount; i++) {
		g_bindings[i].bind1 = g_bindings[i].defaultbind1;
		g_bindings[i].bind2 = g_bindings[i].defaultbind2;
	}

	//s_controls.invertmouse.curvalue  = Controls_GetCvarDefault( "m_pitch" ) < 0;
	//s_controls.smoothmouse.curvalue  = Controls_GetCvarDefault( "m_filter" );
	//s_controls.alwaysrun.curvalue    = Controls_GetCvarDefault( "cl_run" );
	//s_controls.autoswitch.curvalue   = Controls_GetCvarDefault( "cg_autoswitch" );
	//s_controls.sensitivity.curvalue  = Controls_GetCvarDefault( "sensitivity" );
	//s_controls.joyenable.curvalue    = Controls_GetCvarDefault( "in_joystick" );
	//s_controls.joythreshold.curvalue = Controls_GetCvarDefault( "joy_threshold" );
	//s_controls.freelook.curvalue     = Controls_GetCvarDefault( "cl_freelook" );
}

int BindingIDFromName(const char *name) {
	int i;
	for (i = 0; i < g_bindCount; i++) {
		if (Q_strequal(name, g_bindings[i].command)) {
			return i;
		}
	}
	return -1;
}

char g_nameBind1[32];
char g_nameBind2[32];


// Changed RD

void BindingFromName(const char *cvar, int bindtype) {
	int i, b1, b2;

	// iterate each command, set its default binding
	for (i = 0; i < g_bindCount; i++) {
		if (Q_stricmp(cvar, g_bindings[i].command) == 0) {
			b1 = g_bindings[i].bind1;
			if (b1 == -1) {
				break;
			}
			DC->keynumToStringBuf(b1, g_nameBind1, 32);
			Q_strupr(g_nameBind1);
			if (bindtype == BIND_PRIMARY)
				return;

			b2 = g_bindings[i].bind2;
			if (b2 != -1) {
				if (bindtype == BIND_ALTERNATE) {
					DC->keynumToStringBuf(b2, g_nameBind1, 32);
					Q_strupr(g_nameBind1);
				} else {
					DC->keynumToStringBuf(b2, g_nameBind2, 32);
					Q_strupr(g_nameBind2);
					strcat(g_nameBind1, " or ");
					strcat(g_nameBind1, g_nameBind2);
				}
			} else if (bindtype == BIND_ALTERNATE) {
				strcpy(g_nameBind1, "None");
			}
			return;
		}
	}
	strcpy(g_nameBind1, "None");
}
// end changed RD

void Item_Slider_Paint(itemDef_t *item) {
	vec4_t newColor;
	float x, y;
	menuDef_t *parent = (menuDef_t*) item->parent;
	// Changed RD
	/*	if (item->window.flags & WINDOW_HASFOCUS) {
			lowLight[0] = 0.8 * parent->focusColor[0]; 
			lowLight[1] = 0.8 * parent->focusColor[1]; 
			lowLight[2] = 0.8 * parent->focusColor[2]; 
			lowLight[3] = 0.8 * parent->focusColor[3]; 
			LerpColor(parent->focusColor,lowLight,newColor,0.5+0.5*sin(DC->realTime / PULSE_DIVISOR));
		} else { 
			memcpy(&newColor, &item->window.foreColor, sizeof(vec4_t));
		} */

	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	memcpy(&newColor, &item->window.foreColor, sizeof (vec4_t));
	// end changed RD
	y = item->window.rect.y;
	if (item->text) {
		Item_Text_Paint(item);
		x = item->textRect.x + item->textRect.w + 8;
	} else {
		x = item->window.rect.x;
	}
	DC->setColor(newColor);
	DC->drawHandlePic(x, y, DC->Assets.sliderwidth, DC->Assets.sliderheight, DC->Assets.sliderBar);

	x = Item_Slider_ThumbPosition(item);
	// Changed RD
	y = y + (item->window.rect.h / 2) - (DC->Assets.sliderthumbheight / 2);
	if (item->window.flags & WINDOW_HASFOCUS) {
		DC->drawHandlePic(x - (DC->Assets.sliderthumbwidth / 2), y, DC->Assets.sliderthumbwidth, DC->Assets.sliderthumbheight, DC->Assets.sliderThumb_sel);
	} else {
		DC->drawHandlePic(x - (DC->Assets.sliderthumbwidth / 2), y, DC->Assets.sliderthumbwidth, DC->Assets.sliderthumbheight, DC->Assets.sliderThumb);
	}
	// end changed RD
}

void Item_Bind_Paint(itemDef_t *item) {
	vec4_t newColor, lowLight;
	// Changed RD
	float x, y, t;
	int maxChars = 0;
	menuDef_t *parent = (menuDef_t*) item->parent;
	editFieldDef_t *editPtr = (editFieldDef_t*) item->typeData;
	if (editPtr) {
		maxChars = editPtr->maxPaintChars;
	}

	if (item->window.flags & WINDOW_HASFOCUS && !item->nofocuscolor) {
		if (g_bindItem == item) {
			lowLight[0] = 0.8f * 1.0f;
			lowLight[1] = 0.8f * 0.0f;
			lowLight[2] = 0.8f * 0.0f;
			lowLight[3] = 0.8f * 1.0f;
		} else {
			lowLight[0] = 0.8f * parent->focusColor[0];
			lowLight[1] = 0.8f * parent->focusColor[1];
			lowLight[2] = 0.8f * parent->focusColor[2];
			lowLight[3] = 0.8f * parent->focusColor[3];
		}
		LerpColor(parent->focusColor, lowLight, newColor, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
	} else {
		Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
		memcpy(&newColor, &item->window.foreColor, sizeof (vec4_t));
	}

	if (item->text) {
		Item_Text_Paint(item);
		BindingFromName(item->cvar, item->bindtype);
		DC->drawText(item->textRect.x + item->textRect.w + 8, item->textRect.y, item->textscale, newColor, g_nameBind1, 0, maxChars, item->textStyle);
	} else {
		BindingFromName(item->cvar, item->bindtype);
		t = DC->textWidth(g_nameBind1, item->textscale, 0);
		y = item->window.rect.y + item->textaligny;
		x = item->window.rect.x + item->textalignx;
		if (item->textalignment == ITEM_ALIGN_RIGHT) {
			x = x - t;
		}
		if (item->textalignment == ITEM_ALIGN_CENTER) {
			x = x - (t / 2);
		}
		DC->drawText(x, y, item->textscale, newColor, g_nameBind1, 0, maxChars, item->textStyle);
	}
}

qboolean Display_KeyBindPending(void) {
	return g_waitingForKey;
}

qboolean Item_Bind_HandleKey(itemDef_t *item, int key, qboolean down) {
	int id;
	int i;

	if (!g_waitingForKey) {
		if (down && ((key == K_MOUSE1 && Rect_ContainsPoint(&item->window.rect, DC->cursorx, DC->cursory))
				|| key == K_ENTER || key == K_KP_ENTER || key == K_JOY1 || key == K_JOY2 || key == K_JOY3 || key == K_JOY4)) {
			// Changed RD
			if (item->bind2click) {
				if (!item->clickstatus) {
					item->clickstatus = qtrue;
				} else {
					g_waitingForKey = qtrue;
					g_bindItem = item;
					item->clickstatus = qfalse;
				}
			} else {
				g_waitingForKey = qtrue;
				g_bindItem = item;
			}
			// end changed RD
		}
		return qtrue;
	} else {
		if (g_bindItem == NULL) {
			return qtrue;
		}

		if (key & K_CHAR_FLAG) {
			return qtrue;
		}

		switch (key) {
			case K_ESCAPE:
				g_waitingForKey = qfalse;
				return qtrue;

			case K_BACKSPACE:
				id = BindingIDFromName(item->cvar);
				if (id != -1) {
					// Changed RD
					if( g_bindings[id].bind1 != -1 ) {
						DC->setBinding( g_bindings[id].bind1, "" );
						g_bindings[id].bind1 = -1;
					}
					if( g_bindings[id].bind2 != -1 ) {
						DC->setBinding( g_bindings[id].bind2, "" );
						g_bindings[id].bind2 = -1;
					}
/*					if (item->bindtype == BIND_BOTH) {
						g_bindings[id].bind1 = -1;
						g_bindings[id].bind2 = -1;
					} else if (item->bindtype == BIND_PRIMARY) {
						g_bindings[id].bind1 = -1;
					} else {
						g_bindings[id].bind2 = -1;
					} */
					// end changed RD
				}
				Controls_SetConfig(qtrue);
				g_waitingForKey = qfalse;
				g_bindItem = NULL;
				return qtrue;

			case '`':
				return qtrue;
		}
	}

	if (key != -1) {

		for (i = 0; i < g_bindCount; i++) {

			if (g_bindings[i].bind2 == key) {
				g_bindings[i].bind2 = -1;
			}

			if (g_bindings[i].bind1 == key) {
				g_bindings[i].bind1 = g_bindings[i].bind2;
				g_bindings[i].bind2 = -1;
			}
		}
	}


	id = BindingIDFromName(item->cvar);

	// Changed RD
	if (id != -1) {
		if (key == -1) {
			if (g_bindings[id].bind1 != -1 && (item->bindtype == BIND_BOTH || item->bindtype == BIND_PRIMARY)) {
				DC->setBinding(g_bindings[id].bind1, "");
				g_bindings[id].bind1 = -1;
			}
			if (g_bindings[id].bind2 != -1 && (item->bindtype == BIND_BOTH || item->bindtype == BIND_ALTERNATE)) {
				DC->setBinding(g_bindings[id].bind2, "");
				g_bindings[id].bind2 = -1;
			}
		} else if (g_bindings[id].bind1 == -1 && (item->bindtype == BIND_BOTH || item->bindtype == BIND_PRIMARY)) {
			g_bindings[id].bind1 = key;
		} else if (g_bindings[id].bind2 == -1 && (item->bindtype == BIND_ALTERNATE)) {
			g_bindings[id].bind2 = key;
		}
		else if (g_bindings[id].bind1 != key && g_bindings[id].bind2 == -1 && item->bindtype == BIND_BOTH) {
			g_bindings[id].bind2 = key;
		} else {
			if (item->bindtype == BIND_BOTH) {
				DC->setBinding(g_bindings[id].bind1, "");
				DC->setBinding(g_bindings[id].bind2, "");
				g_bindings[id].bind1 = key;
				g_bindings[id].bind2 = -1;
			} else if (item->bindtype == BIND_PRIMARY) {
				DC->setBinding(g_bindings[id].bind1, "");
				g_bindings[id].bind1 = key;
			} else {
				DC->setBinding(g_bindings[id].bind2, "");
				g_bindings[id].bind2 = key;
			}
		}
	}
	// end changed RD

	Controls_SetConfig(qtrue);
	g_waitingForKey = qfalse;

	return qtrue;
}

void AdjustFrom640(float *x, float *y, float *w, float *h) {
	*x = *x * DC->xscale + DC->bias;
	//*x *= DC->xscale;
	*y *= DC->yscale;
	*w *= DC->xscale;
	*h *= DC->yscale;
}

// changed RD

void Item_Model_Paint(itemDef_t *item) {
	float x, y, w, h; //,xx;
	refdef_t refdef;
	qhandle_t hModel;
	refEntity_t ent;
	vec3_t mins, maxs, origin;
	vec3_t angles;
	modelDef_t *modelPtr = (modelDef_t*) item->typeData;
	int backLerpWhole;

	if (modelPtr == NULL) {
		return;
	}

	if (!item->asset)
		return;

	hModel = item->asset;

	// setup the refdef
	memset(&refdef, 0, sizeof ( refdef));
	refdef.rdflags = RDF_NOWORLDMODEL;
	AxisClear(refdef.viewaxis);
	x = item->window.rect.x + 1;
	y = item->window.rect.y + 1;
	w = item->window.rect.w - 2;
	h = item->window.rect.h - 2;

	AdjustFrom640(&x, &y, &w, &h);

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	DC->modelBounds(hModel, mins, maxs);

	origin[2] = -0.5 * (mins[2] + maxs[2]);
	origin[1] = 0.5 * (mins[1] + maxs[1]);

	// calculate distance so the model nearly fills the box
	if (qtrue) {
		float len = 0.5 * (maxs[2] - mins[2]);
		origin[0] = len / 0.268; // len / tan( fov/2 )
		//origin[0] = len / tan(w/2);
	} else {
		origin[0] = item->textscale;
	}

	refdef.fov_x = (modelPtr->fov_x) ? modelPtr->fov_x : w;
	refdef.fov_y = (modelPtr->fov_y) ? modelPtr->fov_y : h;

	DC->clearScene();

	refdef.time = DC->realTime;

	// add the model

	memset(&ent, 0, sizeof (ent));

	// use item storage to track
	if (modelPtr->rotationSpeed) {
		if (DC->realTime > item->window.nextTime) {
			item->window.nextTime = DC->realTime + modelPtr->rotationSpeed;
			modelPtr->angle = (int) (modelPtr->angle + 1) % 360;
		}
	}
	VectorSet(angles, 0, modelPtr->angle, 0);
	AnglesToAxis(angles, ent.axis);

	ent.hModel = hModel;


	if (modelPtr->frameTime) // don't advance on the first frame
		modelPtr->backlerp += (((DC->realTime - modelPtr->frameTime) / 1000.0f) * (float) modelPtr->fps);

	if (modelPtr->backlerp > 1) {
		backLerpWhole = floor(modelPtr->backlerp);

		modelPtr->frame += (backLerpWhole);
		if ((modelPtr->frame - modelPtr->startframe) > modelPtr->numframes)
			modelPtr->frame = modelPtr->startframe + modelPtr->frame % modelPtr->numframes; // todo: ignoring loopframes

		modelPtr->oldframe += (backLerpWhole);
		if ((modelPtr->oldframe - modelPtr->startframe) > modelPtr->numframes)
			modelPtr->oldframe = modelPtr->startframe + modelPtr->oldframe % modelPtr->numframes; // todo: ignoring loopframes

		modelPtr->backlerp = modelPtr->backlerp - backLerpWhole;
	}

	modelPtr->frameTime = DC->realTime;

	ent.frame = modelPtr->frame;
	ent.oldframe = modelPtr->oldframe;
	ent.backlerp = 1.0f - modelPtr->backlerp;

	VectorCopy(origin, ent.origin);
	VectorCopy(origin, ent.lightingOrigin);
	ent.renderfx = RF_LIGHTING_ORIGIN | RF_NOSHADOW;
	VectorCopy(ent.origin, ent.oldorigin);

	// leilei - colorable models
	if (item->window.flags & WINDOW_FORECOLORSET) {
		ent.shaderRGBA[0] = item->window.foreColor[0] * 255;
		ent.shaderRGBA[1] = item->window.foreColor[1] * 255;
		ent.shaderRGBA[2] = item->window.foreColor[2] * 255;

	}
	DC->addRefEntityToScene(&ent);

	// changed RD
	//
	// add an accent light
	//
	origin[0] -= 100; // + = behind, - = in front
	origin[1] += 100; // + = left, - = right
	origin[2] += 100; // + = above, - = below
	trap_R_AddLightToScene(origin, 500, 1.0, 1.0, 1.0);

	origin[0] -= 100;
	origin[1] -= 100;
	origin[2] -= 100;
	trap_R_AddLightToScene(origin, 500, 1.0, 0.0, 0.0);
	// end changed RD

	DC->renderScene(&refdef);
}
// end changed RD

void Item_Image_Paint(itemDef_t *item) {
	if (item == NULL) {
		return;
	}
	DC->drawHandlePic(item->window.rect.x + 1, item->window.rect.y + 1, item->window.rect.w - 2, item->window.rect.h - 2, item->asset);
}

void Item_ListBox_Paint(itemDef_t *item) {
	float x, y, size, thumb;
	int count, i;
	qhandle_t image;
	qhandle_t optionalImage;
	listBoxDef_t *listPtr = (listBoxDef_t*) item->typeData;
	// Changed RD
	menuDef_t *parent = (menuDef_t*) item->parent;

	Fade(&item->window.flags, &item->fadeColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	Fade(&item->window.flags, &item->window.borderColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
	DC->setColor(item->fadeColor);
	// end changed RD

	// the listbox is horizontal or vertical and has a fixed size scroll bar going either direction
	// elements are enumerated from the DC and either text or image handles are acquired from the DC as well
	// textscale is used to size the text, textalignx and textaligny are used to size image elements
	// there is no clipping available so only the last completely visible item is painted
	count = DC->feederCount(item->special);
	// default is vertical if horizontal flag is not here
	if (item->window.flags & WINDOW_HORIZONTAL) {
		// draw scrollbar in bottom of the window
		// bar
		x = item->window.rect.x + 1;
		y = item->window.rect.y + item->window.rect.h - DC->Assets.scrollbarsize - 1;
		DC->drawHandlePic(x, y, DC->Assets.scrollbarsize, DC->Assets.scrollbarsize, DC->Assets.scrollBarArrowLeft);
		x += DC->Assets.scrollbarsize - 1;
		size = item->window.rect.w - (DC->Assets.scrollbarsize * 2);
		DC->drawHandlePic(x, y, size + 1, DC->Assets.scrollbarsize, DC->Assets.scrollBarHorz);
		x += size - 1;
		DC->drawHandlePic(x, y, DC->Assets.scrollbarsize, DC->Assets.scrollbarsize, DC->Assets.scrollBarArrowRight);
		// thumb
		thumb = Item_ListBox_ThumbDrawPosition(item); //Item_ListBox_ThumbPosition(item);
		if (thumb > x - DC->Assets.scrollbarsize - 1) {
			thumb = x - DC->Assets.scrollbarsize - 1;
		}
		DC->drawHandlePic(thumb, y, DC->Assets.scrollbarsize, DC->Assets.scrollbarsize, DC->Assets.scrollBarThumb);
		//
		listPtr->endPos = listPtr->startPos;
		size = item->window.rect.w - 2;
		// items
		// size contains max available space
		if (listPtr->elementStyle == LISTBOX_IMAGE) {
			// fit = 0;
			x = item->window.rect.x + 1;
			y = item->window.rect.y + 1;
			for (i = listPtr->startPos; i < count; i++) {
				// always draw at least one
				// which may overdraw the box if it is too small for the element
				image = DC->feederItemImage(item->special, i);
				if (image) {
					// Changed RD
					DC->setColor(item->fadeColor);
					// end changed RD
					DC->drawHandlePic(x + 1, y + 1, listPtr->elementWidth - 2, listPtr->elementHeight - 2, image);
				}

				if (i == item->cursorPos) {
					DC->drawRect(x, y, listPtr->elementWidth - 1, listPtr->elementHeight - 1, item->window.borderSize, item->window.borderColor);
				}

				size -= listPtr->elementWidth;
				if (size < listPtr->elementWidth) {
					listPtr->drawPadding = size; //listPtr->elementWidth - size;
					break;
				}
				x += listPtr->elementWidth;
				listPtr->endPos++;
				// fit++;
			}
		} else {
			//
		}
	} else {
		// draw scrollbar to right side of the window
		x = item->window.rect.x + item->window.rect.w - DC->Assets.scrollbarsize - 1;
		y = item->window.rect.y + 1;
		DC->drawHandlePic(x, y, DC->Assets.scrollbarsize, DC->Assets.scrollbarsize, DC->Assets.scrollBarArrowUp);
		y += DC->Assets.scrollbarsize - 1;

		listPtr->endPos = listPtr->startPos;
		size = item->window.rect.h - (DC->Assets.scrollbarsize * 2);
		DC->drawHandlePic(x, y, DC->Assets.scrollbarsize, size + 1, DC->Assets.scrollBarVert);
		y += size - 1;
		DC->drawHandlePic(x, y, DC->Assets.scrollbarsize, DC->Assets.scrollbarsize, DC->Assets.scrollBarArrowDown);
		// thumb
		thumb = Item_ListBox_ThumbDrawPosition(item); //Item_ListBox_ThumbPosition(item);
		if (thumb > y - DC->Assets.scrollbarsize - 1) {
			thumb = y - DC->Assets.scrollbarsize - 1;
		}
		DC->drawHandlePic(x, thumb, DC->Assets.scrollbarsize, DC->Assets.scrollbarsize, DC->Assets.scrollBarThumb);

		// adjust size for item painting
		size = item->window.rect.h - 2;
		if (listPtr->elementStyle == LISTBOX_IMAGE) {
			// fit = 0;
			x = item->window.rect.x + 1;
			y = item->window.rect.y + 1;
			for (i = listPtr->startPos; i < count; i++) {
				// always draw at least one
				// which may overdraw the box if it is too small for the element
				image = DC->feederItemImage(item->special, i);
				if (image) {
					// Changed RD
					DC->setColor(item->fadeColor);
					// end changed RD
					DC->drawHandlePic(x + 1, y + 1, listPtr->elementWidth - 2, listPtr->elementHeight - 2, image);
				}

				if (i == item->cursorPos) {
					DC->drawRect(x, y, listPtr->elementWidth - 1, listPtr->elementHeight - 1, item->window.borderSize, item->window.borderColor);
				}

				listPtr->endPos++;
				size -= listPtr->elementWidth;
				if (size < listPtr->elementHeight) {
					listPtr->drawPadding = listPtr->elementHeight - size;
					break;
				}
				y += listPtr->elementHeight;
				// fit++;
			}
		} else {
			x = item->window.rect.x + 1;
			y = item->window.rect.y + 1;
			for (i = listPtr->startPos; i < count; i++) {
				const char *text;
				// always draw at least one
				// which may overdraw the box if it is too small for the element

				if (listPtr->numColumns > 0) {
					int j;
					for (j = 0; j < listPtr->numColumns; j++) {
						text = DC->feederItemText(item->special, i, j, &optionalImage);
						if (optionalImage >= 0) {
							DC->drawHandlePic(x + 4 + listPtr->columnInfo[j].pos, y - 1 + listPtr->elementHeight / 2, listPtr->columnInfo[j].width, listPtr->columnInfo[j].width, optionalImage);
							// Changed RD
						} else if (text && listPtr->columnInfo[j].maxChars > 0) {
							DC->setColor(item->window.foreColor);
							// end changed RD
							DC->drawText(x + 4 + listPtr->columnInfo[j].pos, y + listPtr->elementHeight, item->textscale, item->window.foreColor, text, 0, listPtr->columnInfo[j].maxChars, item->textStyle);
						}
					}
				} else {
					text = DC->feederItemText(item->special, i, 0, &optionalImage);
					if (optionalImage >= 0) {
						//DC->drawHandlePic(x + 4 + listPtr->elementHeight, y, listPtr->columnInfo[j].width, listPtr->columnInfo[j].width, optionalImage);
					} else if (text) {
						// Changed RD
						DC->setColor(item->window.foreColor);
						// end changed RD
						DC->drawText(x + 4, y + listPtr->elementHeight, item->textscale, item->window.foreColor, text, 0, 0, item->textStyle);
					}
				}


				if (i == item->cursorPos) {
					DC->fillRect(x + 2, y + 2, item->window.rect.w - DC->Assets.scrollbarsize - 4, listPtr->elementHeight, item->window.outlineColor);
				}

				size -= listPtr->elementHeight;
				if (size < listPtr->elementHeight) {
					listPtr->drawPadding = listPtr->elementHeight - size;
					break;
				}
				listPtr->endPos++;
				y += listPtr->elementHeight;
				// fit++;
			}
		}
	}
	DC->setColor(NULL);
}

void Item_OwnerDraw_Paint(itemDef_t *item) {
	if (item == NULL) {
		return;
	}

	if (DC->ownerDrawItem) {
		vec4_t color, lowLight;
		menuDef_t *parent = (menuDef_t*) item->parent;
		Fade(&item->window.flags, &item->window.foreColor[3], parent->fadeClamp, &item->window.nextTime, parent->fadeCycle, qtrue, parent->fadeAmount);
		memcpy(&color, &item->window.foreColor, sizeof (color));
		if (item->numColors > 0 && DC->getValue) {
			// if the value is within one of the ranges then set color to that, otherwise leave at default
			int i;
			float f = DC->getValue(item->window.ownerDraw);
			for (i = 0; i < item->numColors; i++) {
				if (f >= item->colorRanges[i].low && f <= item->colorRanges[i].high) {
					memcpy(&color, &item->colorRanges[i].color, sizeof (color));
					break;
				}
			}
		}

		if (item->window.flags & WINDOW_HASFOCUS) {
			lowLight[0] = 0.8 * parent->focusColor[0];
			lowLight[1] = 0.8 * parent->focusColor[1];
			lowLight[2] = 0.8 * parent->focusColor[2];
			lowLight[3] = 0.8 * parent->focusColor[3];
			LerpColor(parent->focusColor, lowLight, color, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
		} else if (item->textStyle == ITEM_TEXTSTYLE_BLINK && !((DC->realTime / BLINK_DIVISOR) & 1)) {
			lowLight[0] = 0.8 * item->window.foreColor[0];
			lowLight[1] = 0.8 * item->window.foreColor[1];
			lowLight[2] = 0.8 * item->window.foreColor[2];
			lowLight[3] = 0.8 * item->window.foreColor[3];
			LerpColor(item->window.foreColor, lowLight, color, 0.5 + 0.5 * sin(DC->realTime / PULSE_DIVISOR));
		}

		if (item->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(item, CVAR_ENABLE)) {
			Com_Memcpy(color, parent->disableColor, sizeof (vec4_t));
		}

		if (item->text) {
			Item_Text_Paint(item);
			if (item->text[0]) {
				// +8 is an offset kludge to properly align owner draw items that have text combined with them
				DC->ownerDrawItem(item->textRect.x + item->textRect.w + 8, item->window.rect.y, item->window.rect.w, item->window.rect.h, 0, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags, item->alignment, item->special, item->textscale, color, item->window.background, item->textStyle);
			} else {
				DC->ownerDrawItem(item->textRect.x + item->textRect.w, item->window.rect.y, item->window.rect.w, item->window.rect.h, 0, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags, item->alignment, item->special, item->textscale, color, item->window.background, item->textStyle);
			}
		} else {
			DC->ownerDrawItem(item->window.rect.x, item->window.rect.y, item->window.rect.w, item->window.rect.h, item->textalignx, item->textaligny, item->window.ownerDraw, item->window.ownerDrawFlags, item->alignment, item->special, item->textscale, color, item->window.background, item->textStyle);
		}
	}
}

void Item_Paint(itemDef_t *item) {
	vec4_t red;
	menuDef_t *parent = (menuDef_t*) item->parent;
	red[0] = red[3] = 1;
	red[1] = red[2] = 0;

	if (item == NULL) {
		return;
	}

	if (item->window.flags & WINDOW_ORBITING) {
		if (DC->realTime > item->window.nextTime) {
			float rx, ry, a, c, s, w, h;

			item->window.nextTime = DC->realTime + item->window.offsetTime;
			// translate
			w = item->window.rectClient.w / 2;
			h = item->window.rectClient.h / 2;
			rx = item->window.rectClient.x + w - item->window.rectEffects.x;
			ry = item->window.rectClient.y + h - item->window.rectEffects.y;
			a = 3 * M_PI / 180;
			c = cos(a);
			s = sin(a);
			item->window.rectClient.x = (rx * c - ry * s) + item->window.rectEffects.x - w;
			item->window.rectClient.y = (rx * s + ry * c) + item->window.rectEffects.y - h;
			Item_UpdatePosition(item);

		}
	}


	if (item->window.flags & WINDOW_INTRANSITION) {
		if (DC->realTime > item->window.nextTime) {
			int done = 0;
			item->window.nextTime = DC->realTime + item->window.offsetTime;
			// transition the x,y
			if (item->window.rectClient.x == item->window.rectEffects.x) {
				done++;
			} else {
				if (item->window.rectClient.x < item->window.rectEffects.x) {
					item->window.rectClient.x += item->window.rectEffects2.x;
					if (item->window.rectClient.x > item->window.rectEffects.x) {
						item->window.rectClient.x = item->window.rectEffects.x;
						done++;
					}
				} else {
					item->window.rectClient.x -= item->window.rectEffects2.x;
					if (item->window.rectClient.x < item->window.rectEffects.x) {
						item->window.rectClient.x = item->window.rectEffects.x;
						done++;
					}
				}
			}
			if (item->window.rectClient.y == item->window.rectEffects.y) {
				done++;
			} else {
				if (item->window.rectClient.y < item->window.rectEffects.y) {
					item->window.rectClient.y += item->window.rectEffects2.y;
					if (item->window.rectClient.y > item->window.rectEffects.y) {
						item->window.rectClient.y = item->window.rectEffects.y;
						done++;
					}
				} else {
					item->window.rectClient.y -= item->window.rectEffects2.y;
					if (item->window.rectClient.y < item->window.rectEffects.y) {
						item->window.rectClient.y = item->window.rectEffects.y;
						done++;
					}
				}
			}
			if (item->window.rectClient.w == item->window.rectEffects.w) {
				done++;
			} else {
				if (item->window.rectClient.w < item->window.rectEffects.w) {
					item->window.rectClient.w += item->window.rectEffects2.w;
					if (item->window.rectClient.w > item->window.rectEffects.w) {
						item->window.rectClient.w = item->window.rectEffects.w;
						done++;
					}
				} else {
					item->window.rectClient.w -= item->window.rectEffects2.w;
					if (item->window.rectClient.w < item->window.rectEffects.w) {
						item->window.rectClient.w = item->window.rectEffects.w;
						done++;
					}
				}
			}
			if (item->window.rectClient.h == item->window.rectEffects.h) {
				done++;
			} else {
				if (item->window.rectClient.h < item->window.rectEffects.h) {
					item->window.rectClient.h += item->window.rectEffects2.h;
					if (item->window.rectClient.h > item->window.rectEffects.h) {
						item->window.rectClient.h = item->window.rectEffects.h;
						done++;
					}
				} else {
					item->window.rectClient.h -= item->window.rectEffects2.h;
					if (item->window.rectClient.h < item->window.rectEffects.h) {
						item->window.rectClient.h = item->window.rectEffects.h;
						done++;
					}
				}
			}

			Item_UpdatePosition(item);

			if (done == 4) {
				item->window.flags &= ~WINDOW_INTRANSITION;
				// Changed RD
				if (!(item->window.flags & WINDOW_NOTRANSITION))
					item->window.flags |= WINDOW_ENDTRANSITION;
				item->window.flags &= ~WINDOW_NOTRANSITION;
				// end changed RD
			}

		}
	}

	if (item->window.ownerDrawFlags && DC->ownerDrawVisible) {
		if (!DC->ownerDrawVisible(item->window.ownerDrawFlags)) {
			item->window.flags &= ~WINDOW_VISIBLE;
		} else {
			item->window.flags |= WINDOW_VISIBLE;
		}
	}

	if (item->cvarFlags & (CVAR_SHOW | CVAR_HIDE)) {
		if (!Item_EnableShowViaCvar(item, CVAR_SHOW)) {
			return;
		}
	}

	if (item->window.flags & WINDOW_TIMEDVISIBLE) {

	}

	if (!(item->window.flags & WINDOW_VISIBLE)) {
		return;
	}

	// paint the rect first.. 
	Window_Paint(&item->window, parent->fadeAmount, parent->fadeClamp, parent->fadeCycle);

	if (debugMode) {
		vec4_t color;
		rectDef_t *r = Item_CorrectedTextRect(item);
		color[1] = color[3] = 1;
		color[0] = color[2] = 0;
		DC->drawRect(r->x, r->y, r->w, r->h, 1, color);
	}

	//DC->drawRect(item->window.rect.x, item->window.rect.y, item->window.rect.w, item->window.rect.h, 1, red);

	switch (item->type) {
		case ITEM_TYPE_OWNERDRAW:
			Item_OwnerDraw_Paint(item);
			break;
		case ITEM_TYPE_TEXT:
		case ITEM_TYPE_BUTTON:
			Item_Text_Paint(item);
			break;
		case ITEM_TYPE_RADIOBUTTON:
			break;
		case ITEM_TYPE_CHECKBOX:
			// Changed RD
			Item_Checkbox_Paint(item);
			// end changed RD
			break;
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
			Item_TextField_Paint(item);
			break;
		case ITEM_TYPE_COMBO:
			// Changed RD
			Item_Combo_Paint(item);
			// end changed RD
			break;
		case ITEM_TYPE_LISTBOX:
			Item_ListBox_Paint(item);
			break;
			//case ITEM_TYPE_IMAGE:
			//  Item_Image_Paint(item);
			//  break;
		case ITEM_TYPE_MODEL:
			Item_Model_Paint(item);
			break;
		case ITEM_TYPE_YESNO:
			Item_YesNo_Paint(item);
			break;
		case ITEM_TYPE_MULTI:
			Item_Multi_Paint(item);
			break;
		case ITEM_TYPE_BIND:
			Item_Bind_Paint(item);
			break;
		case ITEM_TYPE_SLIDER:
			Item_Slider_Paint(item);
			break;
		default:
			break;
	}

}

void Menu_Init(menuDef_t *menu) {
	memset(menu, 0, sizeof (menuDef_t));
	menu->cursorItem = -1;
	menu->fadeAmount = DC->Assets.fadeAmount;
	menu->fadeClamp = DC->Assets.fadeClamp;
	menu->fadeCycle = DC->Assets.fadeCycle;
	Window_Init(&menu->window);
}

itemDef_t *Menu_GetFocusedItem(menuDef_t *menu) {
	int i;
	if (menu) {
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->window.flags & WINDOW_HASFOCUS) {
				return menu->items[i];
			}
		}
	}
	return NULL;
}

menuDef_t *Menu_GetFocused(void) {
	int i;
	for (i = 0; i < menuCount; i++) {
		if (Menus[i].window.flags & WINDOW_HASFOCUS && Menus[i].window.flags & WINDOW_VISIBLE) {
			return &Menus[i];
		}
	}
	return NULL;
}

void Menu_ScrollFeeder(menuDef_t *menu, int feeder, qboolean down) {
	if (menu) {
		int i;
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->special == feeder) {
				Item_ListBox_HandleKey(menu->items[i], (down) ? K_DOWNARROW : K_UPARROW, qtrue, qtrue);
				Item_ListBox_HandleKey(menu->items[i], (down) ? K_JOY31 : K_JOY29, qtrue, qtrue); // leilei - gamepad support

				return;
			}
		}
	}
}

void Menu_SetFeederSelection(menuDef_t *menu, int feeder, int index, const char *name) {
	if (menu == NULL) {
		if (name == NULL) {
			menu = Menu_GetFocused();
		} else {
			menu = Menus_FindByName(name);
		}
	}

	if (menu) {
		int i;
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->special == feeder) {
				if (index == 0) {
					listBoxDef_t *listPtr = (listBoxDef_t*) menu->items[i]->typeData;
					listPtr->cursorPos = 0;
					listPtr->startPos = 0;
				}
				menu->items[i]->cursorPos = index;
				DC->feederSelection(menu->items[i]->special, menu->items[i]->cursorPos);
				return;
			}
		}
	}
}

qboolean Menus_AnyFullScreenVisible(void) {
	int i;
	for (i = 0; i < menuCount; i++) {
		if (Menus[i].window.flags & WINDOW_VISIBLE && Menus[i].fullScreen) {
			return qtrue;
		}
	}
	return qfalse;
}

void RefreshHexColors ( void );
menuDef_t *Menus_ActivateByName(const char *p) {
	int i;
	menuDef_t *m = NULL;
	menuDef_t *focus = Menu_GetFocused();
	for (i = 0; i < menuCount; i++) {
		if (Q_stricmp(Menus[i].window.name, p) == 0) {
			m = &Menus[i];
			Menus_Activate(m);
			if (openMenuCount < MAX_OPEN_MENUS && focus != NULL) {
				menuStack[openMenuCount++] = focus;
			}
		} else {
			Menus[i].window.flags &= ~WINDOW_HASFOCUS;
		}
	}
	Display_CloseCinematics();
	return m;
}

void Item_Init(itemDef_t *item) {
	memset(item, 0, sizeof (itemDef_t));
	item->textscale = 0.55f;
	Window_Init(&item->window);
}

void Menu_HandleMouseMove(menuDef_t *menu, float x, float y) {
	int i, pass;
	qboolean focusSet = qfalse;

	itemDef_t *overItem;
	if (menu == NULL) {
		return;
	}

	if (!(menu->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED))) {
		return;
	}

	// Changed RD
	if (DC->getCVarValue("ui_transitionkey")) {
		for (i = 0; i < menu->itemCount; i++) {
			if (menu->items[i]->window.flags & WINDOW_INTRANSITION) {
				return;
			}
		}
	}
	// end changed RD

	if (itemCapture) {
		//Item_MouseMove(itemCapture, x, y);
		return;
	}

	if (g_waitingForKey || g_editingField) {
		return;
	}

	// FIXME: this is the whole issue of focus vs. mouse over.. 
	// need a better overall solution as i don't like going through everything twice
	for (pass = 0; pass < 2; pass++) {
		for (i = 0; i < menu->itemCount; i++) {
			// turn off focus each item
			// menu->items[i].window.flags &= ~WINDOW_HASFOCUS;

			if (!(menu->items[i]->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED))) {
				continue;
			}

			// items can be enabled and disabled based on cvars
			if (menu->items[i]->cvarFlags & (CVAR_ENABLE | CVAR_DISABLE) && !Item_EnableShowViaCvar(menu->items[i], CVAR_ENABLE)) {
				continue;
			}

			if (menu->items[i]->cvarFlags & (CVAR_SHOW | CVAR_HIDE) && !Item_EnableShowViaCvar(menu->items[i], CVAR_SHOW)) {
				continue;
			}

			if (menu->items[i]->type == ITEM_TYPE_COMBO) {
				comboDef_t *comboPtr = (comboDef_t*) menu->items[i]->comboData;
				if (comboPtr->combopen) {
					if (Rect_ContainsPoint(&comboPtr->ComboBox, x, y)) {
						multiDef_t *multiPtr = (multiDef_t*) menu->items[i]->multiData;
						int j;
						Rectangle Box;
						Box.x = comboPtr->ComboBox.x;
						Box.w = comboPtr->ComboBox.w;
						Box.y = comboPtr->ComboBox.y + BOXTEXTOFFSET;
						for (j = 0; j < multiPtr->count; j++) {
							Box.h = comboPtr->maxheight;
							if (Rect_ContainsPoint(&Box, x, y)) {
								comboPtr->comboItem = j;
								return;
							}
							Box.y += Box.h + BOXTEXTSTEP;
						}
					}
					comboPtr->comboItem = -1;
					return;
				}
			}

			if (Rect_ContainsPoint(&menu->items[i]->window.rect, x, y)) {
				if (pass == 1) {
					overItem = menu->items[i];
					if (overItem->type == ITEM_TYPE_TEXT && overItem->text) {
						if (!Rect_ContainsPoint(Item_CorrectedTextRect(overItem), x, y)) {
							continue;
						}
					}
					// if we are over an item
					if (IsVisible(overItem->window.flags)) {
						// different one
						Item_MouseEnter(overItem, x, y);
						// Item_SetMouseOver(overItem, qtrue);

						// if item is not a decoration see if it can take focus
						if (!focusSet) {
							focusSet = Item_SetFocus(overItem, x, y);
						}
					}
				}
			} else if (menu->items[i]->window.flags & WINDOW_MOUSEOVER) {
				Item_MouseLeave(menu->items[i]);
				Item_SetMouseOver(menu->items[i], qfalse);
			}
		}
	}

}

void Menu_Paint(menuDef_t *menu, qboolean forcePaint) {
	int i;

	if (menu == NULL) {
		return;
	}

	if (!(menu->window.flags & WINDOW_VISIBLE) && !forcePaint) {
		return;
	}

	if (menu->window.ownerDrawFlags && DC->ownerDrawVisible && !DC->ownerDrawVisible(menu->window.ownerDrawFlags)) {
		return;
	}

	if (forcePaint) {
		menu->window.flags |= WINDOW_FORCED;
	}

	// draw the background if necessary
	if (menu->fullScreen) {
		// implies a background shader
		// FIXME: make sure we have a default shader if fullscreen is set with no background
		DC->drawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, menu->window.background);
	} else if (menu->window.background) {
		// this allows a background shader without being full screen
		//UI_DrawHandlePic(menu->window.rect.x, menu->window.rect.y, menu->window.rect.w, menu->window.rect.h, menu->backgroundShader);
	}

	// paint the background and or border
	Window_Paint(&menu->window, menu->fadeAmount, menu->fadeClamp, menu->fadeCycle);

	for (i = 0; i < menu->itemCount; i++) {
		Item_Paint(menu->items[i]);
	}

	// Changed RD
	for (i = 0; i < menu->itemCount; i++) {
		if (menu->items[i]->type == ITEM_TYPE_COMBO) {
			comboDef_t *comboPtr = (comboDef_t*) menu->items[i]->comboData;
			if (comboPtr->combopen) {
				Item_ComboBox_Paint(menu->items[i]);
			}
		}
	}
	// end changed RD

	if (debugMode) {
		vec4_t color;
		color[0] = color[2] = color[3] = 1;
		color[1] = 0;
		DC->drawRect(menu->window.rect.x, menu->window.rect.y, menu->window.rect.w, menu->window.rect.h, 1, color);
	}
}

/*
===============
Item_ValidateTypeData
===============
 */
void Item_ValidateTypeData(itemDef_t *item) {
	if (item->typeData) {
		return;
	}

	if (item->type == ITEM_TYPE_LISTBOX) {
		item->typeData = UI_Alloc(sizeof (listBoxDef_t));
		memset(item->typeData, 0, sizeof (listBoxDef_t));
	} else if (item->type == ITEM_TYPE_EDITFIELD || item->type == ITEM_TYPE_NUMERICFIELD || item->type == ITEM_TYPE_YESNO || item->type == ITEM_TYPE_BIND || item->type == ITEM_TYPE_SLIDER || item->type == ITEM_TYPE_TEXT) {
		item->typeData = UI_Alloc(sizeof (editFieldDef_t));
		memset(item->typeData, 0, sizeof (editFieldDef_t));
		if (item->type == ITEM_TYPE_EDITFIELD) {
			if (!((editFieldDef_t *) item->typeData)->maxPaintChars) {
				((editFieldDef_t *) item->typeData)->maxPaintChars = MAX_EDITFIELD;
			}
		}
	} else if (item->type == ITEM_TYPE_MULTI) {
		item->typeData = UI_Alloc(sizeof (multiDef_t));
	} else if (item->type == ITEM_TYPE_MODEL) {
		item->typeData = UI_Alloc(sizeof (modelDef_t));
		// Changed RD
		memset(item->typeData, 0, sizeof (modelDef_t));
	} else if (item->type == ITEM_TYPE_COMBO && !item->comboData) {
		item->comboData = UI_Alloc(sizeof (comboDef_t));
		memset(item->comboData, 0, sizeof (comboDef_t));
		((comboDef_t *) item->comboData)->combopen = qfalse;
		((comboDef_t *) item->comboData)->comboItem = -1;
		((comboDef_t *) item->comboData)->boxtextscale = 0.28f;
		((comboDef_t *) item->comboData)->offsetx = 8;
		((comboDef_t *) item->comboData)->offsety = 8;
		((comboDef_t *) item->comboData)->boxcolor[3] = 1;
		((comboDef_t *) item->comboData)->boxtextcolor[3] = 1;
		// end changed RD
	}
}

/*
===============
Keyword Hash
===============
 */

#define KEYWORDHASH_SIZE 512

typedef struct keywordHash_s {
	char *keyword;
	qboolean(*func)(itemDef_t *item, int handle);
	struct keywordHash_s *next;
} keywordHash_t;

int KeywordHash_Key(char *keyword) {
	int register hash, i;

	hash = 0;
	for (i = 0; keyword[i] != '\0'; i++) {
		if (keyword[i] >= 'A' && keyword[i] <= 'Z')
			hash += (keyword[i] + ('a' - 'A')) * (119 + i);
		else
			hash += keyword[i] * (119 + i);
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20)) & (KEYWORDHASH_SIZE - 1);
	return hash;
}

void KeywordHash_Add(keywordHash_t *table[], keywordHash_t *key) {
	int hash;

	hash = KeywordHash_Key(key->keyword);
	/*
		if (table[hash]) {
			int collision = qtrue;
		}
	 */
	key->next = table[hash];
	table[hash] = key;
}

keywordHash_t *KeywordHash_Find(keywordHash_t *table[], char *keyword) {
	keywordHash_t *key;
	int hash;

	hash = KeywordHash_Key(keyword);
	for (key = table[hash]; key; key = key->next) {
		if (Q_strequal(key->keyword, keyword))
			return key;
	}
	return NULL;
}

/*
===============
Item Keyword Parse functions
===============
 */

// name <string>

qboolean ItemParse_name(itemDef_t *item, int handle) {
	if (!PC_String_Parse(handle, &item->window.name)) {
		return qfalse;
	}
	return qtrue;
}

// name <string>

qboolean ItemParse_focusSound(itemDef_t *item, int handle) {
	const char *temp;
	if (!PC_String_Parse(handle, &temp)) {
		return qfalse;
	}
	item->focusSound = DC->registerSound(temp, qfalse);
	return qtrue;
}


// text <string>

qboolean ItemParse_text(itemDef_t *item, int handle) {
	if (!PC_String_Parse(handle, &item->text)) {
		return qfalse;
	}
	return qtrue;
}

// group <string>

qboolean ItemParse_group(itemDef_t *item, int handle) {
	if (!PC_String_Parse(handle, &item->window.group)) {
		return qfalse;
	}
	return qtrue;
}

// asset_model <string>

qboolean ItemParse_asset_model(itemDef_t *item, int handle) {
	const char *temp;
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	if (!PC_String_Parse(handle, &temp)) {
		return qfalse;
	}
	item->asset = DC->registerModel(temp);
	//	modelPtr->angle = rand() % 360;
	modelPtr->angle = 0; // leilei - don't do this because it makes menu changing inconsistent

	return qtrue;
}

// asset_shader <string>

qboolean ItemParse_asset_shader(itemDef_t *item, int handle) {
	const char *temp;

	if (!PC_String_Parse(handle, &temp)) {
		return qfalse;
	}
	item->asset = DC->registerShaderNoMip(temp);
	return qtrue;
}

// model_origin <number> <number> <number>

qboolean ItemParse_model_origin(itemDef_t *item, int handle) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	if (PC_Float_Parse(handle, &modelPtr->origin[0])) {
		if (PC_Float_Parse(handle, &modelPtr->origin[1])) {
			if (PC_Float_Parse(handle, &modelPtr->origin[2])) {
				return qtrue;
			}
		}
	}
	return qfalse;
}

// model_fovx <number>

qboolean ItemParse_model_fovx(itemDef_t *item, int handle) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	if (!PC_Float_Parse(handle, &modelPtr->fov_x)) {
		return qfalse;
	}
	return qtrue;
}

// model_fovy <number>

qboolean ItemParse_model_fovy(itemDef_t *item, int handle) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	if (!PC_Float_Parse(handle, &modelPtr->fov_y)) {
		return qfalse;
	}
	return qtrue;
}

// model_rotation <integer>

qboolean ItemParse_model_rotation(itemDef_t *item, int handle) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	if (!PC_Int_Parse(handle, &modelPtr->rotationSpeed)) {
		return qfalse;
	}
	return qtrue;
}

// model_angle <integer>

qboolean ItemParse_model_angle(itemDef_t *item, int handle) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	if (!PC_Int_Parse(handle, &modelPtr->angle)) {
		return qfalse;
	}
	return qtrue;
}

// changed RD
// model_animplay <int(startframe)> <int(numframes)> <int(fps)>

qboolean ItemParse_model_animplay(itemDef_t *item, int handle) {
	modelDef_t *modelPtr;
	Item_ValidateTypeData(item);
	modelPtr = (modelDef_t*) item->typeData;

	modelPtr->animated = 1;

	if (!PC_Int_Parse(handle, &modelPtr->startframe)) return qfalse;
	if (!PC_Int_Parse(handle, &modelPtr->numframes)) return qfalse;
	if (!PC_Int_Parse(handle, &modelPtr->fps)) return qfalse;

	modelPtr->frame = modelPtr->startframe + 1;
	modelPtr->oldframe = modelPtr->startframe;
	modelPtr->backlerp = 0.0f;
	modelPtr->frameTime = DC->realTime;
	return qtrue;
}
// end changed RD

// rect <rectangle>

qboolean ItemParse_rect(itemDef_t *item, int handle) {
	if (!PC_Rect_Parse(handle, &item->window.rectClient)) {
		return qfalse;
	}
	return qtrue;
}

// style <integer>

qboolean ItemParse_style(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->window.style)) {
		return qfalse;
	}
	return qtrue;
}

// decoration

qboolean ItemParse_decoration(itemDef_t *item, int handle) {
	item->window.flags |= WINDOW_DECORATION;
	return qtrue;
}

// notselectable

qboolean ItemParse_notselectable(itemDef_t *item, int handle) {
	listBoxDef_t *listPtr;
	Item_ValidateTypeData(item);
	listPtr = (listBoxDef_t*) item->typeData;
	if (item->type == ITEM_TYPE_LISTBOX && listPtr) {
		listPtr->notselectable = qtrue;
	}
	return qtrue;
}

// manually wrapped

qboolean ItemParse_wrapped(itemDef_t *item, int handle) {
	item->window.flags |= WINDOW_WRAPPED;
	return qtrue;
}

// auto wrapped

qboolean ItemParse_autowrapped(itemDef_t *item, int handle) {
	item->window.flags |= WINDOW_AUTOWRAPPED;
	return qtrue;
}


// horizontalscroll

qboolean ItemParse_horizontalscroll(itemDef_t *item, int handle) {
	item->window.flags |= WINDOW_HORIZONTAL;
	return qtrue;
}

// type <integer>

qboolean ItemParse_type(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->type)) {
		return qfalse;
	}
	Item_ValidateTypeData(item);
	return qtrue;
}

// elementwidth, used for listbox image elements
// uses textalignx for storage

qboolean ItemParse_elementwidth(itemDef_t *item, int handle) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = (listBoxDef_t*) item->typeData;
	if (!PC_Float_Parse(handle, &listPtr->elementWidth)) {
		return qfalse;
	}
	return qtrue;
}

// elementheight, used for listbox image elements
// uses textaligny for storage

qboolean ItemParse_elementheight(itemDef_t *item, int handle) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	listPtr = (listBoxDef_t*) item->typeData;
	if (!PC_Float_Parse(handle, &listPtr->elementHeight)) {
		return qfalse;
	}
	return qtrue;
}

// feeder <float>

qboolean ItemParse_feeder(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->special)) {
		return qfalse;
	}
	return qtrue;
}

// elementtype, used to specify what type of elements a listbox contains
// uses textstyle for storage

qboolean ItemParse_elementtype(itemDef_t *item, int handle) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;
	listPtr = (listBoxDef_t*) item->typeData;
	if (!PC_Int_Parse(handle, &listPtr->elementStyle)) {
		return qfalse;
	}
	return qtrue;
}

// columns sets a number of columns and an x pos and width per.. 

qboolean ItemParse_columns(itemDef_t *item, int handle) {
	int num, i;
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;
	listPtr = (listBoxDef_t*) item->typeData;
	if (PC_Int_Parse(handle, &num)) {
		if (num > MAX_LB_COLUMNS) {
			num = MAX_LB_COLUMNS;
		}
		listPtr->numColumns = num;
		for (i = 0; i < num; i++) {
			int pos, width, maxChars;

			if (PC_Int_Parse(handle, &pos) && PC_Int_Parse(handle, &width) && PC_Int_Parse(handle, &maxChars)) {
				listPtr->columnInfo[i].pos = pos;
				listPtr->columnInfo[i].width = width;
				listPtr->columnInfo[i].maxChars = maxChars;
			} else {
				return qfalse;
			}
		}
	} else {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_border(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->window.border)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_bordersize(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->window.borderSize)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_visible(itemDef_t *item, int handle) {
	int i;

	if (!PC_Int_Parse(handle, &i)) {
		return qfalse;
	}
	if (i) {
		item->window.flags |= WINDOW_VISIBLE;
	}
	return qtrue;
}

qboolean ItemParse_ownerdraw(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->window.ownerDraw)) {
		return qfalse;
	}
	item->type = ITEM_TYPE_OWNERDRAW;
	return qtrue;
}

qboolean ItemParse_align(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->alignment)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_textalign(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->textalignment)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_textalignx(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->textalignx)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_textaligny(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->textaligny)) {
		return qfalse;
	}
	return qtrue;
}

// leilei - adaptation of screen align

qboolean ItemParse_scralign(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->scralign)) {
		item->scralign = ALIGN_CENTER; // leilei - HACK
		return qfalse;
	}
	return qtrue;
}

// leilei - scaling of the adjustment

qboolean ItemParse_scralignfactor(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->scralignfactor)) {
		item->scralign = ALIGN_CENTER; // leilei - HACK
		return qfalse;
	}
	return qtrue;
}


// Changed RD

qboolean ItemParse_boxtextscale(itemDef_t *item, int handle) {
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;
	if (!PC_Float_Parse(handle, &comboPtr->boxtextscale)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_comboalignx(itemDef_t *item, int handle) {
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;
	if (!PC_Float_Parse(handle, &comboPtr->offsetx)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_comboaligny(itemDef_t *item, int handle) {
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;
	if (!PC_Float_Parse(handle, &comboPtr->offsety)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_bindtype(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->bindtype)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_bind2click(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->bind2click)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_nofocuscolor(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->nofocuscolor)) {
		return qfalse;
	}
	return qtrue;
}
// end changed RD

qboolean ItemParse_textscale(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->textscale)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_textstyle(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->textStyle)) {
		return qfalse;
	}
	return qtrue;
}


qboolean ItemParse_bitflag(itemDef_t *item, int handle) {
	if (!PC_Int_Parse(handle, &item->bitflag)) {
		return qfalse;
	}
	return qtrue;
}
// Changed RD

qboolean ItemParse_boxcolor(itemDef_t *item, int handle) {
	int i;
	float f;
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		comboPtr->boxcolor[i] = f;
	}
	return qtrue;
}

qboolean ItemParse_boxtextcolor(itemDef_t *item, int handle) {
	int i;
	float f;
	comboDef_t *comboPtr = (comboDef_t*) item->comboData;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		comboPtr->boxtextcolor[i] = f;
	}
	return qtrue;
}
// end changed RD

qboolean ItemParse_backcolor(itemDef_t *item, int handle) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		item->window.backColor[i] = f;
	}
	return qtrue;
}

qboolean ItemParse_forecolor(itemDef_t *item, int handle) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		item->window.foreColor[i] = f;
		item->window.flags |= WINDOW_FORECOLORSET;
	}
	return qtrue;
}

// leilei - color schemes


// i am lazy
static char *Both_Cvar_VariableString( const char *var_name ) {
	static char	buffer[MAX_STRING_CHARS];

	trap_Cvar_VariableStringBuffer( var_name, buffer, sizeof( buffer ) );

	return buffer;
}

#define MAX_HEXCOLORS 16
vec4_t hexcol[MAX_HEXCOLORS];

void UpdateHexColors ( void ){
	// Parse ui_colors to update our vectors from the hex values in the cvar	
	const char *token;
	int col[MAX_HEXCOLORS];
	int i, j, k;
	menuDef_t *menu;
	
	char *thecolors = Both_Cvar_VariableString("ui_colors");

	for (i=0;i<MAX_HEXCOLORS;i++)
	{
		token = COM_ParseExt( &thecolors, qfalse);
		col[i] = strtol( token, NULL, 0 );
		hexcol[i][0] = (float)((col[i] >> 16) & 0xFF) / 255;
		hexcol[i][1] = (float)((col[i] >> 8 ) & 0xFF) / 255;
		hexcol[i][2] = (float)(col[i] & 0xFF) / 255;
	}


	
}


void ItemRefresh_hexcolor(itemDef_t *item) {
	int i;
	int j;
	float f;
	int hecks[MAX_HEXCOLORS];
	UpdateHexColors();
	for (j = 0; j < 3; j++) { // ignore alpha
		item->window.backColor[j] 	= hexcol[item->window.choshex[0]][j];
		item->window.foreColor[j] 	= hexcol[item->window.choshex[1]][j];
		item->window.outlineColor[j] 	= hexcol[item->window.choshex[2]][j];
		item->window.borderColor[j] 	= hexcol[item->window.choshex[3]][j];
	}
}


qboolean ItemParse_hexcolor(itemDef_t *item, int handle) {
	int i;
	int j;
	float f;
	int hecks[MAX_HEXCOLORS];

	UpdateHexColors();	// TODO: Move to a uiscript command so scheme changes
				// can update everything

	// the way we parse it is take the float vectors as
	// integers that point to which defined hex color value will be
	// used per component:

	// like say ui_colors 202154 FFF 121212
	// hexcolor 0, 1, 1, 2
	//	    |  |  |  |
	//          |  |  |  \------- borderColor becomes RGB 18,18,18
	//          |  |  \--------- textColor becomes RGB 255,255,255
	//          |  \---------- foreColor becomes RGB 255,255,255
	//	    \---------- backColor becomes RGB 32,33,84
	//
	// Alpha components set by previous colors should be unaffected so
	// fades can still work, no sudden borders, etc.
	//
	//

	for (i = 0; i < 3; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		hecks[i] = (int)f;
	}
	
	item->window.choshex[0] = hecks[0];
	item->window.choshex[1] = hecks[1];
	item->window.choshex[2] = hecks[2];
	item->window.choshex[3] = hecks[3];
	item->window.flags |= WINDOW_HEXCOLORSET;

	for (j = 0; j < 3; j++) { // ignore alpha
		item->window.backColor[j] 	= hexcol[hecks[0]][j];
		item->window.foreColor[j] 	= hexcol[hecks[1]][j];
		item->window.outlineColor[j] 	= hexcol[hecks[2]][j];
		item->window.borderColor[j] 	= hexcol[hecks[3]][j];
	}

	return qtrue;
}

void RefreshHexColors ( void ){
	// Parse ui_colors to update our vectors from the hex values in the cvar	
	const char *token;
	int col[MAX_HEXCOLORS];
	int i, j, k;
	menuDef_t *menu;

	// Now go through all of the menu options and check their hex colors and change them all
	for (i = 0; i < Menu_Count(); i++) {
		menu = &Menus[i];
		if (menu != NULL) {
			for (j = 0; j < menu->itemCount; j++) {
				if (menu->items[j]->window.flags & WINDOW_HEXCOLORSET) {
					ItemRefresh_hexcolor(menu->items[j]);
					}
			}
		}
	}

	
}


qboolean ItemParse_bordercolor(itemDef_t *item, int handle) {
	int i;
	float f;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		item->window.borderColor[i] = f;
	}
	return qtrue;
}

qboolean ItemParse_outlinecolor(itemDef_t *item, int handle) {
	if (!PC_Color_Parse(handle, &item->window.outlineColor)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_background(itemDef_t *item, int handle) {
	const char *temp;

	if (!PC_String_Parse(handle, &temp)) {
		return qfalse;
	}
	item->window.background = DC->registerShaderNoMip(temp);
	return qtrue;
}

qboolean ItemParse_cinematic(itemDef_t *item, int handle) {
	if (!PC_String_Parse(handle, &item->window.cinematicName)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_doubleClick(itemDef_t *item, int handle) {
	listBoxDef_t *listPtr;

	Item_ValidateTypeData(item);
	if (!item->typeData) {
		return qfalse;
	}

	listPtr = (listBoxDef_t*) item->typeData;

	if (!PC_Script_Parse(handle, &listPtr->doubleClick)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_onFocus(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->onFocus)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_leaveFocus(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->leaveFocus)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_mouseEnter(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->mouseEnter)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_mouseExit(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->mouseExit)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_mouseEnterText(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->mouseEnterText)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_mouseExitText(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->mouseExitText)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_action(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->action)) {
		return qfalse;
	}
	return qtrue;
}

// Changed RD

qboolean ItemParse_transitionend(itemDef_t *item, int handle) {
	if (!PC_Script_Parse(handle, &item->transitionEnd)) {
		return qfalse;
	}
	return qtrue;
}
// end changed RD

qboolean ItemParse_special(itemDef_t *item, int handle) {
	if (!PC_Float_Parse(handle, &item->special)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_cvarTest(itemDef_t *item, int handle) {
	if (!PC_String_Parse(handle, &item->cvarTest)) {
		return qfalse;
	}
	return qtrue;
}

qboolean ItemParse_cvar(itemDef_t *item, int handle) {
	editFieldDef_t *editPtr;

	Item_ValidateTypeData(item);
	if (!PC_String_Parse(handle, &item->cvar)) {
		return qfalse;
	}
	if (item->typeData) {
		editPtr = (editFieldDef_t*) item->typeData;
		editPtr->minVal = -1;
		editPtr->maxVal = -1;
		editPtr->defVal = -1;
	}
	return qtrue;
}

qboolean ItemParse_maxChars(itemDef_t *item, int handle) {
	editFieldDef_t *editPtr;
	int maxChars;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;

	if (!PC_Int_Parse(handle, &maxChars)) {
		return qfalse;
	}
	editPtr = (editFieldDef_t*) item->typeData;
	editPtr->maxChars = maxChars;
	return qtrue;
}

qboolean ItemParse_maxPaintChars(itemDef_t *item, int handle) {
	editFieldDef_t *editPtr;
	int maxChars;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;

	if (!PC_Int_Parse(handle, &maxChars)) {
		return qfalse;
	}
	editPtr = (editFieldDef_t*) item->typeData;
	editPtr->maxPaintChars = maxChars;
	return qtrue;
}

qboolean ItemParse_cvarFloat(itemDef_t *item, int handle) {
	editFieldDef_t *editPtr;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;
	editPtr = (editFieldDef_t*) item->typeData;
	if (PC_String_Parse(handle, &item->cvar) &&
			PC_Float_Parse(handle, &editPtr->defVal) &&
			PC_Float_Parse(handle, &editPtr->minVal) &&
			PC_Float_Parse(handle, &editPtr->maxVal)) {
		return qtrue;
	}
	return qfalse;
}

qboolean ItemParse_cvarStrList(itemDef_t *item, int handle) {
	pc_token_t token;
	multiDef_t *multiPtr;
	int pass;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;
	multiPtr = (multiDef_t*) item->typeData;
	multiPtr->count = 0;
	multiPtr->strDef = qtrue;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (*token.string != '{') {
		return qfalse;
	}

	pass = 0;
	while (1) {
		if (!trap_PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu item\n");
			return qfalse;
		}

		if (*token.string == '}') {
			return qtrue;
		}

		if (*token.string == ',' || *token.string == ';') {
			continue;
		}

		if (pass == 0) {
			multiPtr->cvarList[multiPtr->count] = String_Alloc(token.string);
			pass = 1;
		} else {
			multiPtr->cvarStr[multiPtr->count] = String_Alloc(token.string);
			pass = 0;
			multiPtr->count++;
			if (multiPtr->count >= MAX_MULTI_CVARS) {
				return qfalse;
			}
		}

	}
	return qfalse; // bk001205 - LCC missing return value
}

qboolean ItemParse_cvarFloatList(itemDef_t *item, int handle) {
	pc_token_t token;
	multiDef_t *multiPtr;

	Item_ValidateTypeData(item);
	if (!item->typeData)
		return qfalse;
	multiPtr = (multiDef_t*) item->typeData;
	multiPtr->count = 0;
	multiPtr->strDef = qfalse;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (*token.string != '{') {
		return qfalse;
	}

	while (1) {
		if (!trap_PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu item\n");
			return qfalse;
		}

		if (*token.string == '}') {
			return qtrue;
		}

		if (*token.string == ',' || *token.string == ';') {
			continue;
		}

		multiPtr->cvarList[multiPtr->count] = String_Alloc(token.string);
		if (!PC_Float_Parse(handle, &multiPtr->cvarValue[multiPtr->count])) {
			return qfalse;
		}

		multiPtr->count++;
		if (multiPtr->count >= MAX_MULTI_CVARS) {
			return qfalse;
		}

	}
	return qfalse; // bk001205 - LCC missing return value
}



// Changed RD

qboolean ItemParse_cvarFloatBoxList(itemDef_t *item, int handle) {
	pc_token_t token;
	multiDef_t *multiPtr;

	Item_ValidateTypeData(item);
	if (!item->comboData)
		return qfalse;
	item->multiData = UI_Alloc(sizeof (multiDef_t));
	if (!item->multiData)
		return qfalse;
	multiPtr = (multiDef_t*) item->multiData;
	multiPtr->count = 0;
	multiPtr->strDef = qfalse;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (*token.string != '{') {
		return qfalse;
	}

	while (1) {
		if (!trap_PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu item\n");
			return qfalse;
		}

		if (*token.string == '}') {
			return qtrue;
		}

		if (*token.string == ',' || *token.string == ';') {
			continue;
		}

		multiPtr->cvarList[multiPtr->count] = String_Alloc(token.string);
		if (!PC_Float_Parse(handle, &multiPtr->cvarValue[multiPtr->count])) {
			return qfalse;
		}

		multiPtr->count++;
		if (multiPtr->count >= MAX_MULTI_CVARS) {
			return qfalse;
		}

	}
	return qfalse; // bk001205 - LCC missing return value
}

qboolean ItemParse_cvarStrBoxList(itemDef_t *item, int handle) {
	pc_token_t token;
	multiDef_t *multiPtr;
	int pass;

	Item_ValidateTypeData(item);
	if (!item->comboData)
		return qfalse;
	item->multiData = UI_Alloc(sizeof (multiDef_t));
	if (!item->multiData)
		return qfalse;
	multiPtr = (multiDef_t*) item->multiData;
	multiPtr->count = 0;
	multiPtr->strDef = qtrue;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (*token.string != '{') {
		return qfalse;
	}

	pass = 0;
	while (1) {
		if (!trap_PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu item\n");
			return qfalse;
		}

		if (*token.string == '}') {
			return qtrue;
		}

		if (*token.string == ',' || *token.string == ';') {
			continue;
		}

		if (pass == 0) {
			multiPtr->cvarList[multiPtr->count] = String_Alloc(token.string);
			pass = 1;
		} else {
			multiPtr->cvarStr[multiPtr->count] = String_Alloc(token.string);
			pass = 0;
			multiPtr->count++;
			if (multiPtr->count >= MAX_MULTI_CVARS) {
				return qfalse;
			}
		}

	}
	return qfalse; // bk001205 - LCC missing return value
}
// end changed RD

qboolean ItemParse_addColorRange(itemDef_t *item, int handle) {
	colorRangeDef_t color;

	if (PC_Float_Parse(handle, &color.low) &&
			PC_Float_Parse(handle, &color.high) &&
			PC_Color_Parse(handle, &color.color)) {
		if (item->numColors < MAX_COLOR_RANGES) {
			memcpy(&item->colorRanges[item->numColors], &color, sizeof (color));
			item->numColors++;
		}
		return qtrue;
	}
	return qfalse;
}

qboolean ItemParse_ownerdrawFlag(itemDef_t *item, int handle) {
	int i;
	if (!PC_Int_Parse(handle, &i)) {
		return qfalse;
	}
	item->window.ownerDrawFlags |= i;
	return qtrue;
}

qboolean ItemParse_enableCvar(itemDef_t *item, int handle) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_ENABLE;
		return qtrue;
	}
	return qfalse;
}

qboolean ItemParse_disableCvar(itemDef_t *item, int handle) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_DISABLE;
		return qtrue;
	}
	return qfalse;
}

qboolean ItemParse_showCvar(itemDef_t *item, int handle) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_SHOW;
		return qtrue;
	}
	return qfalse;
}

qboolean ItemParse_hideCvar(itemDef_t *item, int handle) {
	if (PC_Script_Parse(handle, &item->enableCvar)) {
		item->cvarFlags = CVAR_HIDE;
		return qtrue;
	}
	return qfalse;
}


keywordHash_t itemParseKeywords[] = {
	{"name", ItemParse_name, NULL},
	{"text", ItemParse_text, NULL},
	{"group", ItemParse_group, NULL},
	{"asset_model", ItemParse_asset_model, NULL},
	{"asset_shader", ItemParse_asset_shader, NULL},
	{"model_origin", ItemParse_model_origin, NULL},
	{"model_fovx", ItemParse_model_fovx, NULL},
	{"model_fovy", ItemParse_model_fovy, NULL},
	{"model_rotation", ItemParse_model_rotation, NULL},
	{"model_angle", ItemParse_model_angle, NULL},
	// changed RD
	{"model_animplay", ItemParse_model_animplay, NULL},
	// end changed RD
	{"rect", ItemParse_rect, NULL},
	{"style", ItemParse_style, NULL},
	{"decoration", ItemParse_decoration, NULL},
	{"notselectable", ItemParse_notselectable, NULL},
	{"wrapped", ItemParse_wrapped, NULL},
	{"autowrapped", ItemParse_autowrapped, NULL},
	{"horizontalscroll", ItemParse_horizontalscroll, NULL},
	{"type", ItemParse_type, NULL},
	{"elementwidth", ItemParse_elementwidth, NULL},
	{"elementheight", ItemParse_elementheight, NULL},
	{"feeder", ItemParse_feeder, NULL},
	{"elementtype", ItemParse_elementtype, NULL},
	{"columns", ItemParse_columns, NULL},
	{"border", ItemParse_border, NULL},
	{"bordersize", ItemParse_bordersize, NULL},
	{"visible", ItemParse_visible, NULL},
	{"ownerdraw", ItemParse_ownerdraw, NULL},
	{"align", ItemParse_align, NULL},
	{"textalign", ItemParse_textalign, NULL},
	{"textalignx", ItemParse_textalignx, NULL},
	{"textaligny", ItemParse_textaligny, NULL},
	{"scralign", ItemParse_scralign, NULL}, // leilei - scroll align adaptation
	{"scralignfactor", ItemParse_scralignfactor, NULL}, // leilei - scroll align adaptation factor
	{"textscale", ItemParse_textscale, NULL},
	{"textstyle", ItemParse_textstyle, NULL},
	{"backcolor", ItemParse_backcolor, NULL},
	{"forecolor", ItemParse_forecolor, NULL},
	{"bordercolor", ItemParse_bordercolor, NULL},
	{"outlinecolor", ItemParse_outlinecolor, NULL},
	{"background", ItemParse_background, NULL},
	{"onFocus", ItemParse_onFocus, NULL},
	{"leaveFocus", ItemParse_leaveFocus, NULL},
	{"mouseEnter", ItemParse_mouseEnter, NULL},
	{"mouseExit", ItemParse_mouseExit, NULL},
	{"mouseEnterText", ItemParse_mouseEnterText, NULL},
	{"mouseExitText", ItemParse_mouseExitText, NULL},
	{"action", ItemParse_action, NULL},
	{"special", ItemParse_special, NULL},
	{"cvar", ItemParse_cvar, NULL},
	{"maxChars", ItemParse_maxChars, NULL},
	{"maxPaintChars", ItemParse_maxPaintChars, NULL},
	{"focusSound", ItemParse_focusSound, NULL},
	{"cvarFloat", ItemParse_cvarFloat, NULL},
	{"cvarStrList", ItemParse_cvarStrList, NULL},
	{"cvarFloatList", ItemParse_cvarFloatList, NULL},
	// Changed RD
	{"ontransitionend", ItemParse_transitionend, NULL},
	{"cvarFloatBoxList", ItemParse_cvarFloatBoxList, NULL},
	{"cvarStrBoxList", ItemParse_cvarStrBoxList, NULL},
	{"combobackcolor", ItemParse_boxcolor, NULL},
	{"combotextcolor", ItemParse_boxtextcolor, NULL},
	{"combotextscale", ItemParse_boxtextscale, NULL},
	{"comboalignx", ItemParse_comboalignx, NULL},
	{"comboaligny", ItemParse_comboaligny, NULL},
	{"bindtype", ItemParse_bindtype, NULL},
	{"bind2click", ItemParse_bind2click, NULL},
	{"nofocuscolor", ItemParse_nofocuscolor, NULL},
	// end changed RD
	{"addColorRange", ItemParse_addColorRange, NULL},
	{"ownerdrawFlag", ItemParse_ownerdrawFlag, NULL},
	{"enableCvar", ItemParse_enableCvar, NULL},
	{"cvarTest", ItemParse_cvarTest, NULL},
	{"disableCvar", ItemParse_disableCvar, NULL},
	{"showCvar", ItemParse_showCvar, NULL},
	{"hideCvar", ItemParse_hideCvar, NULL},
	{"cinematic", ItemParse_cinematic, NULL},
	{"doubleclick", ItemParse_doubleClick, NULL},
	{"hexcolor", ItemParse_hexcolor, NULL},
	{"bitflag", ItemParse_bitflag, NULL},
	{NULL, 0, NULL}
};

keywordHash_t *itemParseKeywordHash[KEYWORDHASH_SIZE];

/*
===============
Item_SetupKeywordHash
===============
 */
void Item_SetupKeywordHash(void) {
	int i;

	memset(itemParseKeywordHash, 0, sizeof (itemParseKeywordHash));
	for (i = 0; itemParseKeywords[i].keyword; i++) {
		KeywordHash_Add(itemParseKeywordHash, &itemParseKeywords[i]);
	}
}

/*
===============
Item_Parse
===============
 */
qboolean Item_Parse(int handle, itemDef_t *item) {
	pc_token_t token;
	keywordHash_t *key;


	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (*token.string != '{') {
		return qfalse;
	}

	item->scralign = ALIGN_CENTER; // leilei - HACK
	item->scralignfactor = 1.0f; // leilei - HACK
	while (1) {
		if (!trap_PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu item\n");
			return qfalse;
		}

		if (*token.string == '}') {
			return qtrue;
		}

		key = KeywordHash_Find(itemParseKeywordHash, token.string);
		if (!key) {
			PC_SourceError(handle, "unknown menu item keyword %s", token.string);
			continue;
		}
		if (!key->func(item, handle)) {
			PC_SourceError(handle, "couldn't parse menu item keyword %s", token.string);
			return qfalse;
		}
	}
	return qfalse; // bk001205 - LCC missing return value
}


// Item_InitControls
// init's special control types

void Item_InitControls(itemDef_t *item) {
	if (item == NULL) {
		return;
	}
	if (item->type == ITEM_TYPE_LISTBOX) {
		listBoxDef_t *listPtr = (listBoxDef_t*) item->typeData;
		item->cursorPos = 0;
		// Changed RD
		item->fadeColor[0] = 1;
		item->fadeColor[1] = 1;
		item->fadeColor[2] = 1;
		item->fadeColor[3] = 1;
		// end changed RD

		if (listPtr) {
			listPtr->cursorPos = 0;
			listPtr->startPos = 0;
			listPtr->endPos = 0;
			listPtr->cursorPos = 0;
		}
	}
}

/*
===============
Menu Keyword Parse functions
===============
 */

qboolean MenuParse_font(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_String_Parse(handle, &menu->font)) {
		return qfalse;
	}
	if (!DC->Assets.fontRegistered) {
		DC->registerFont(menu->font, 48, &DC->Assets.textFont);
		DC->Assets.fontRegistered = qtrue;
	}
	return qtrue;
}

qboolean MenuParse_name(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_String_Parse(handle, &menu->window.name)) {
		return qfalse;
	}
	if (Q_strequal(menu->window.name, "main")) {
		// default main as having focus
		//menu->window.flags |= WINDOW_HASFOCUS;
	}
	return qtrue;
}

qboolean MenuParse_fullscreen(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Int_Parse(handle, (int*) &menu->fullScreen)) { // bk001206 - cast qboolean
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_rect(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Rect_Parse(handle, &menu->window.rect)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_style(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Int_Parse(handle, &menu->window.style)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_visible(itemDef_t *item, int handle) {
	int i;
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_Int_Parse(handle, &i)) {
		return qfalse;
	}
	if (i) {
		menu->window.flags |= WINDOW_VISIBLE;
	}
	return qtrue;
}

qboolean MenuParse_onOpen(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Script_Parse(handle, &menu->onOpen)) {
		return qfalse;
	}
	return qtrue;
}


// Changed RD

qboolean MenuParse_onTransition(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Script_Parse(handle, &menu->opentransitionEnd)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_escTransition(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Script_Parse(handle, &menu->esctransitionEnd)) {
		return qfalse;
	}
	return qtrue;
}
// end changed RD

qboolean MenuParse_onClose(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Script_Parse(handle, &menu->onClose)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_onESC(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Script_Parse(handle, &menu->onESC)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_border(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Int_Parse(handle, &menu->window.border)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_borderSize(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Float_Parse(handle, &menu->window.borderSize)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_backcolor(itemDef_t *item, int handle) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*) item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		menu->window.backColor[i] = f;
	}
	return qtrue;
}

qboolean MenuParse_forecolor(itemDef_t *item, int handle) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*) item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		menu->window.foreColor[i] = f;
		menu->window.flags |= WINDOW_FORECOLORSET;
	}
	return qtrue;
}

qboolean MenuParse_bordercolor(itemDef_t *item, int handle) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*) item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		menu->window.borderColor[i] = f;
	}
	return qtrue;
}

qboolean MenuParse_focuscolor(itemDef_t *item, int handle) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*) item;

	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		menu->focusColor[i] = f;
	}
	return qtrue;
}

qboolean MenuParse_disablecolor(itemDef_t *item, int handle) {
	int i;
	float f;
	menuDef_t *menu = (menuDef_t*) item;
	for (i = 0; i < 4; i++) {
		if (!PC_Float_Parse(handle, &f)) {
			return qfalse;
		}
		menu->disableColor[i] = f;
	}
	return qtrue;
}

qboolean MenuParse_outlinecolor(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (!PC_Color_Parse(handle, &menu->window.outlineColor)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_background(itemDef_t *item, int handle) {
	const char *buff;
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_String_Parse(handle, &buff)) {
		return qfalse;
	}
	menu->window.background = DC->registerShaderNoMip(buff);
	return qtrue;
}

qboolean MenuParse_cinematic(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_String_Parse(handle, &menu->window.cinematicName)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_ownerdrawFlag(itemDef_t *item, int handle) {
	int i;
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_Int_Parse(handle, &i)) {
		return qfalse;
	}
	menu->window.ownerDrawFlags |= i;
	return qtrue;
}

qboolean MenuParse_ownerdraw(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_Int_Parse(handle, &menu->window.ownerDraw)) {
		return qfalse;
	}
	return qtrue;
}


// decoration

qboolean MenuParse_popup(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	menu->window.flags |= WINDOW_POPUP;
	return qtrue;
}

qboolean MenuParse_outOfBounds(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	menu->window.flags |= WINDOW_OOB_CLICK;
	return qtrue;
}

qboolean MenuParse_soundLoop(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_String_Parse(handle, &menu->soundName)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_fadeClamp(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_Float_Parse(handle, &menu->fadeClamp)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_fadeAmount(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_Float_Parse(handle, &menu->fadeAmount)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_fadeCycle(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;

	if (!PC_Int_Parse(handle, &menu->fadeCycle)) {
		return qfalse;
	}
	return qtrue;
}

qboolean MenuParse_itemDef(itemDef_t *item, int handle) {
	menuDef_t *menu = (menuDef_t*) item;
	if (menu->itemCount < MAX_MENUITEMS) {
		menu->items[menu->itemCount] = UI_Alloc(sizeof (itemDef_t));
		Item_Init(menu->items[menu->itemCount]);
		if (!Item_Parse(handle, menu->items[menu->itemCount])) {
			return qfalse;
		}
		Item_InitControls(menu->items[menu->itemCount]);
		menu->items[menu->itemCount++]->parent = menu;
	}
	return qtrue;
}

keywordHash_t menuParseKeywords[] = {
	{"font", MenuParse_font, NULL},
	{"name", MenuParse_name, NULL},
	{"fullscreen", MenuParse_fullscreen, NULL},
	{"rect", MenuParse_rect, NULL},
	{"style", MenuParse_style, NULL},
	{"visible", MenuParse_visible, NULL},
	{"onOpen", MenuParse_onOpen, NULL},
	// Changed RD
	{"openTransitionEnd", MenuParse_onTransition, NULL},
	{"escTransitionEnd", MenuParse_escTransition, NULL},
	// changed RD
	{"onClose", MenuParse_onClose, NULL},
	{"onESC", MenuParse_onESC, NULL},
	{"border", MenuParse_border, NULL},
	{"borderSize", MenuParse_borderSize, NULL},
	{"backcolor", MenuParse_backcolor, NULL},
	{"forecolor", MenuParse_forecolor, NULL},
	{"bordercolor", MenuParse_bordercolor, NULL},
	{"focuscolor", MenuParse_focuscolor, NULL},
	{"disablecolor", MenuParse_disablecolor, NULL},
	{"outlinecolor", MenuParse_outlinecolor, NULL},
	{"background", MenuParse_background, NULL},
	{"ownerdraw", MenuParse_ownerdraw, NULL},
	{"ownerdrawFlag", MenuParse_ownerdrawFlag, NULL},
	{"outOfBoundsClick", MenuParse_outOfBounds, NULL},
	{"soundLoop", MenuParse_soundLoop, NULL},
	{"itemDef", MenuParse_itemDef, NULL},
	{"cinematic", MenuParse_cinematic, NULL},
	{"popup", MenuParse_popup, NULL},
	{"fadeClamp", MenuParse_fadeClamp, NULL},
	{"fadeCycle", MenuParse_fadeCycle, NULL},
	{"fadeAmount", MenuParse_fadeAmount, NULL},
	{NULL, 0, NULL}
};

keywordHash_t *menuParseKeywordHash[KEYWORDHASH_SIZE];

/*
===============
Menu_SetupKeywordHash
===============
 */
void Menu_SetupKeywordHash(void) {
	int i;

	memset(menuParseKeywordHash, 0, sizeof (menuParseKeywordHash));
	for (i = 0; menuParseKeywords[i].keyword; i++) {
		KeywordHash_Add(menuParseKeywordHash, &menuParseKeywords[i]);
	}
}

/*
===============
Menu_Parse
===============
 */
qboolean Menu_Parse(int handle, menuDef_t *menu) {
	pc_token_t token;
	keywordHash_t *key;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (*token.string != '{') {
		return qfalse;
	}

	while (1) {

		memset(&token, 0, sizeof (pc_token_t));
		if (!trap_PC_ReadToken(handle, &token)) {
			PC_SourceError(handle, "end of file inside menu\n");
			return qfalse;
		}

		if (*token.string == '}') {
			return qtrue;
		}

		key = KeywordHash_Find(menuParseKeywordHash, token.string);
		if (!key) {
			PC_SourceError(handle, "unknown menu keyword %s", token.string);
			continue;
		}
		if (!key->func((itemDef_t*) menu, handle)) {
			PC_SourceError(handle, "couldn't parse menu keyword %s", token.string);
			return qfalse;
		}
	}
	return qfalse; // bk001205 - LCC missing return value
}

/*
===============
Menu_New
===============
 */
void Menu_New(int handle) {
	menuDef_t *menu = &Menus[menuCount];

	if (menuCount < MAX_MENUS) {
		Menu_Init(menu);
		if (Menu_Parse(handle, menu)) {
			Menu_PostParse(menu);
			menuCount++;
		}
	}
}

int Menu_Count(void) {
	return menuCount;
}

void Menu_PaintAll(void) {
	int i;
	if (captureFunc) {
		captureFunc(captureData);
	}

	for (i = 0; i < Menu_Count(); i++) {
		Menu_Paint(&Menus[i], qfalse);
	}

	if (debugMode) {
		vec4_t v = {1, 1, 1, 1};
		DC->drawText(5, 25, .5, v, va("fps: %f", DC->FPS), 0, 0, 0);
	}
}

// Changed RD

void Menu_PaintEnd(void) {
	int i, j;
	menuDef_t *menu;

	for (i = 0; i < Menu_Count(); i++) {
		menu = &Menus[i];
		if (menu != NULL) {
			for (j = 0; j < menu->itemCount; j++) {
				if (menu->items[j]->window.flags & WINDOW_ENDTRANSITION) {
					menu->items[j]->window.flags &= ~WINDOW_ENDTRANSITION;
					Item_TransitionEnd(menu->items[j]);
					return;
				}
			}
		}
	}
}
// end changed RD

void Menu_Reset(void) {
	menuCount = 0;
}

displayContextDef_t *Display_GetContext(void) {
	return DC;
}

#ifndef MISSIONPACK // bk001206
static float captureX;
static float captureY;
#endif

void *Display_CaptureItem(int x, int y) {
	int i;

	for (i = 0; i < menuCount; i++) {
		// turn off focus each item
		// menu->items[i].window.flags &= ~WINDOW_HASFOCUS;
		if (Rect_ContainsPoint(&Menus[i].window.rect, x, y)) {
			return &Menus[i];
		}
	}
	return NULL;
}


// FIXME: 

qboolean Display_MouseMove(void *p, int x, int y) {
	int i;
	menuDef_t *menu = p;

	if (menu == NULL) {
		menu = Menu_GetFocused();
		if (menu) {
			if (menu->window.flags & WINDOW_POPUP) {
				Menu_HandleMouseMove(menu, x, y);
				return qtrue;
			}
		}
		for (i = 0; i < menuCount; i++) {
			Menu_HandleMouseMove(&Menus[i], x, y);
		}
	} else {
		menu->window.rect.x += x;
		menu->window.rect.y += y;
		Menu_UpdatePosition(menu);
	}
	return qtrue;

}

int Display_CursorType(int x, int y) {
	int i;
	for (i = 0; i < menuCount; i++) {
		rectDef_t r2;
		r2.x = Menus[i].window.rect.x - 3;
		r2.y = Menus[i].window.rect.y - 3;
		r2.w = r2.h = 7;
		if (Rect_ContainsPoint(&r2, x, y)) {
			return CURSOR_SIZER;
		}
	}
	return CURSOR_ARROW;
}

void Display_HandleKey(int key, qboolean down, int x, int y) {
	menuDef_t *menu = Display_CaptureItem(x, y);
	if (menu == NULL) {
		menu = Menu_GetFocused();
	}
	if (menu) {
		Menu_HandleKey(menu, key, down);
	}
}

static void Window_CacheContents(windowDef_t *window) {
	if (window) {
		if (window->cinematicName) {
			int cin = DC->playCinematic(window->cinematicName, 0, 0, 0, 0);
			DC->stopCinematic(cin);
		}
	}
}

static void Item_CacheContents(itemDef_t *item) {
	if (item) {
		Window_CacheContents(&item->window);
	}

}

static void Menu_CacheContents(menuDef_t *menu) {
	if (menu) {
		int i;
		Window_CacheContents(&menu->window);
		for (i = 0; i < menu->itemCount; i++) {
			Item_CacheContents(menu->items[i]);
		}

		if (menu->soundName && *menu->soundName) {
			DC->registerSound(menu->soundName, qfalse);
		}
	}

}

void Display_CacheAll(void) {
	int i;
	for (i = 0; i < menuCount; i++) {
		Menu_CacheContents(&Menus[i]);
	}
}

static qboolean Menu_OverActiveItem(menuDef_t *menu, float x, float y) {
	if (menu && menu->window.flags & (WINDOW_VISIBLE | WINDOW_FORCED)) {
		if (Rect_ContainsPoint(&menu->window.rect, x, y)) {
			int i;
			for (i = 0; i < menu->itemCount; i++) {
				// turn off focus each item
				// menu->items[i].window.flags &= ~WINDOW_HASFOCUS;

				if (menu->items[i]->window.flags & WINDOW_DECORATION || menu->items[i]->window.flags & WINDOW_FOCUSDISABLE) {
					continue;
				}

				if (menu->items[i]->window.flags & WINDOW_DECORATION) {
					continue;
				}

				if (Rect_ContainsPoint(&menu->items[i]->window.rect, x, y)) {
					itemDef_t *overItem = menu->items[i];
					if (overItem->type == ITEM_TYPE_TEXT && overItem->text) {
						if (Rect_ContainsPoint(Item_CorrectedTextRect(overItem), x, y)) {
							return qtrue;
						} else {
							continue;
						}
					} else {
						return qtrue;
					}
				}
			}

		}
	}
	return qfalse;
}


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
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
// Single Player Ladders are defined here
//
//
//
//		I don't have the guts to dig in ui/ to make a fully dynamic customizable
//	scriptable single player progression system so i'll take the lazy way out
//	and do it though extensive cvar testing and setting, with LOTS of archived cvars
//
//	It's a mess but........ *submit a patch*
//
//
//

//	Will be laid out like this:


//	S


#define	LADDER_MAPNAME_DM1				"oa3dm1"					// the map that will be called
#define	LADDER_NAME____DM1				"The First Map"					// the map name that will show
#define	LADDER_DESC____DM1				"If you lose on this map, then you suck!!!"	// description that will show
#define	LADDER_COMMAND_DM1				"startmap oa3dm1; addbot succubus"		// command that is called when executed...
#define	LADDER_FRAGLMT_DM1				"fraglimit 10"					// rules set in this match
#define	LADDER_UNLOCK__DM1				"set sp_cg_01 1; set sp_dm_02 1"		// commands to execute if you win the matcg
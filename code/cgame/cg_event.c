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
// cg_event.c -- handle entity events at snapshot or playerstate transitions

#include "cg_local.h"

// for the voice chats
#ifdef MISSIONPACK // bk001205
#include "../../ui/menudef.h"
#endif
//==========================================================================

/*
===================
CG_PlaceString

Also called by scoreboard drawing
===================
 */
const char *CG_PlaceString(int rank) {
	static char str[64];
	char *s, *t;

	if (rank & RANK_TIED_FLAG) {
		rank &= ~RANK_TIED_FLAG;
		t = "Tied for ";
	} else {
		t = "";
	}

	if (rank == 1) {
		s = S_COLOR_BLUE "1st" S_COLOR_WHITE; // draw in blue
	} else if (rank == 2) {
		s = S_COLOR_RED "2nd" S_COLOR_WHITE; // draw in red
	} else if (rank == 3) {
		s = S_COLOR_YELLOW "3rd" S_COLOR_WHITE; // draw in yellow
	} else if (rank == 11) {
		s = "11th";
	} else if (rank == 12) {
		s = "12th";
	} else if (rank == 13) {
		s = "13th";
	} else if (rank % 10 == 1) {
		s = va("%ist", rank);
	} else if (rank % 10 == 2) {
		s = va("%ind", rank);
	} else if (rank % 10 == 3) {
		s = va("%ird", rank);
	} else {
		s = va("%ith", rank);
	}

	Com_sprintf(str, sizeof ( str), "%s%s", t, s);
	return str;
}

/*
=============
CG_Obituary
=============
 */
static void CG_Obituary(entityState_t *ent) {
	int mod;
	int target, attacker;
	char *message;
	char *message2;
	const char *targetInfo;
	const char *attackerInfo;
	char targetName[32];
	char attackerName[32];
	gender_t gender;
	clientInfo_t *ci;
	int i;
	char *s;
	qhandle_t causeShader;
	fragInfo_t *lastFrag = &cgs.fragMsg[FRAGMSG_MAX - 1];

	target = ent->otherEntityNum;
	attacker = ent->otherEntityNum2;
	mod = ent->eventParm;

	if (target < 0 || target >= MAX_CLIENTS) {
		CG_Error("CG_Obituary: target out of range");
	}
	ci = &cgs.clientinfo[target];

	if (attacker < 0 || attacker >= MAX_CLIENTS) {
		attacker = ENTITYNUM_WORLD;
		attackerInfo = NULL;
	} else {
		attackerInfo = CG_ConfigString(CS_PLAYERS + attacker);
	}

	targetInfo = CG_ConfigString(CS_PLAYERS + target);
	if (!targetInfo) {
		return;
	}
	Q_strncpyz(targetName, Info_ValueForKey(targetInfo, "n"), sizeof (targetName) - 2);
	strcat(targetName, S_COLOR_WHITE);

	message2 = "";

	// check for single client messages

	if (attacker != ENTITYNUM_WORLD) {
		message = NULL;
		causeShader = cgs.media.skullShader;
	} else {
		switch (mod) {
			case MOD_SUICIDE:
				message = "suicides";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_FALLING:
				message = "cratered";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_CRUSH:
				message = "was squished";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_WATER:
				message = "sank like a rock";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_SLIME:
				message = "melted";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_LAVA:
				message = "does a back flip into the lava";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_TARGET_LASER:
				message = "saw the light";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_TRIGGER_HURT:
				message = "was in the wrong place";
				causeShader = cgs.media.skullShader;
				break;
			default:
				message = NULL;
				causeShader = cgs.media.skullShader;
				break;
		}
	}

	if (attacker == target) {
		gender = ci->gender;
		switch (mod) {
			case MOD_KAMIKAZE:
				message = "goes out with a bang";
				causeShader = cgs.media.kamikazeShader;
				break;
			case MOD_GRENADE_SPLASH:
				if (gender == GENDER_FEMALE)
					message = "tripped on her own grenade";
				else if (gender == GENDER_NEUTER)
					message = "tripped on their own grenade";
				else
					message = "tripped on his own grenade";
				causeShader = cgs.media.grenadeShader;
				break;
			case MOD_ROCKET_SPLASH:
				if (gender == GENDER_FEMALE)
					message = "blew herself up";
				else if (gender == GENDER_NEUTER)
					message = "blew themself up";
				else
					message = "blew himself up";
				causeShader = cgs.media.rocketShader;
				break;
			case MOD_PLASMA_SPLASH:
				if (gender == GENDER_FEMALE)
					message = "melted herself";
				else if (gender == GENDER_NEUTER)
					message = "melted themself";
				else
					message = "melted himself";
				causeShader = cgs.media.plasmaShader;
				break;
			case MOD_BFG_SPLASH:
				message = "should have used a smaller gun";
				causeShader = cgs.media.bfgShader;
				break;
			case MOD_PROXIMITY_MINE:
				if (gender == GENDER_FEMALE) {
					message = "found her prox mine";
				} else if (gender == GENDER_NEUTER) {
					message = "found their prox mine";
				} else {
					message = "found his prox mine";
				}
				causeShader = cgs.media.proxlauncherShader;
				break;
			default:
				if (gender == GENDER_FEMALE)
					message = "killed herself";
				else if (gender == GENDER_NEUTER)
					message = "killed themself";
				else
					message = "killed himself";
				causeShader = cgs.media.skullShader;
				break;
		}
	}

	// If a suicide happens while disconnecting
	// then we might not have a targetName
	if (message && strlen(targetName)) {
		if (cg_obituaryOutput.integer == 1 || cg_obituaryOutput.integer == 4) {
			CG_Printf("%s %s.\n", targetName, message);
		}
		if (cg_obituaryOutput.integer >= 2) {
			for (i = 0; i < FRAGMSG_MAX - 1; i++) {
				cgs.fragMsg[i] = cgs.fragMsg[i + 1];
			}
			memset(&cgs.fragMsg[FRAGMSG_MAX - 1], 0, sizeof (fragInfo_t));

			Q_strncpyz(lastFrag->targetName, targetName,
					sizeof (lastFrag->targetName));
			lastFrag->causeShader = causeShader;
			lastFrag->fragTime = cg.time;
			lastFrag->teamFrag = qfalse;
			s = va("%s %s.", targetName, message);
			Q_strncpyz(lastFrag->message, s, sizeof (lastFrag->message));
		}

		return;
	}

	// check for kill messages from the current clientNum
	if (attacker == cg.snap->ps.clientNum) {
		char *s;

		if (!CG_IsATeamGametype(cgs.gametype) && !CG_UsesTeamFlags(cgs.gametype) &&
				!CG_UsesTheWhiteFlag(cgs.gametype) && !CG_IsARoundBasedGametype(cgs.gametype) &&
				cgs.gametype != GT_DOUBLE_D && cgs.gametype != GT_DOMINATION) {
			s = va("You fragged %s\n%s place with %i", targetName,
					CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1),
					cg.snap->ps.persistant[PERS_SCORE]);
		} else {
			if (ent->generic1)
				s = va("You fragged your ^1TEAMMATE^7 %s", targetName);
			else
				s = va("You fragged %s", targetName);
		}
#ifdef MISSIONPACK
		if (!(cg_singlePlayerActive.integer && cg_cameraOrbit.integer)) {
			CG_CenterPrint(s, SCREEN_HEIGHT * 0.30, (int) (BIGCHAR_WIDTH * cg_fragmsgsize.value));
		}
#else
		CG_CenterPrint(s, SCREEN_HEIGHT * 0.30, (int) (BIGCHAR_WIDTH * cg_fragmsgsize.value));
#endif

		// print the text message as well
	}

	// check for double client messages
	if (!attackerInfo) {
		attacker = ENTITYNUM_WORLD;
		strcpy(attackerName, "noname");
	} else {
		Q_strncpyz(attackerName, Info_ValueForKey(attackerInfo, "n"), sizeof (attackerName) - 2);
		strcat(attackerName, S_COLOR_WHITE);
		// check for kill messages about the current clientNum
		if (target == cg.snap->ps.clientNum) {
			Q_strncpyz(cg.killerName, attackerName, sizeof ( cg.killerName));
		}
	}

	if (attacker != ENTITYNUM_WORLD) {
		switch (mod) {
			case MOD_GRAPPLE:
				message = "was caught by";
				causeShader = cgs.media.grapplehookShader;
				break;
			case MOD_GAUNTLET:
				message = "was pummeled by";
				causeShader = cgs.media.gauntletShader;
				break;
			case MOD_MACHINEGUN:
				message = "was machinegunned by";
				causeShader = cgs.media.machinegunShader;
				break;
			case MOD_SHOTGUN:
				message = "was gunned down by";
				causeShader = cgs.media.shotgunShader;
				break;
			case MOD_GRENADE:
				message = "ate";
				message2 = "'s grenade";
				causeShader = cgs.media.grenadeShader;
				break;
			case MOD_GRENADE_SPLASH:
				message = "was shredded by";
				message2 = "'s shrapnel";
				causeShader = cgs.media.grenadeShader;
				break;
			case MOD_ROCKET:
				message = "ate";
				message2 = "'s rocket";
				causeShader = cgs.media.rocketShader;
				break;
			case MOD_ROCKET_SPLASH:
				message = "almost dodged";
				message2 = "'s rocket";
				causeShader = cgs.media.rocketShader;
				break;
			case MOD_PLASMA:
				message = "was melted by";
				message2 = "'s plasmagun";
				causeShader = cgs.media.plasmaShader;
				break;
			case MOD_PLASMA_SPLASH:
				message = "was melted by";
				message2 = "'s plasmagun";
				causeShader = cgs.media.plasmaShader;
				break;
			case MOD_RAILGUN:
				message = "was railed by";
				causeShader = cgs.media.railgunShader;
				break;
			case MOD_LIGHTNING:
				message = "was electrocuted by";
				causeShader = cgs.media.lightninggunShader;
				break;
			case MOD_BFG:
			case MOD_BFG_SPLASH:
				message = "was blasted by";
				message2 = "'s BFG";
				causeShader = cgs.media.bfgShader;
				break;
			case MOD_NAIL:
				message = "was nailed by";
				causeShader = cgs.media.nailgunShader;
				break;
			case MOD_CHAINGUN:
				message = "got lead poisoning from";
				message2 = "'s Chaingun";
				causeShader = cgs.media.chaingunShader;
				break;
			case MOD_PROXIMITY_MINE:
				message = "was too close to";
				message2 = "'s Prox Mine";
				causeShader = cgs.media.proxlauncherShader;
				break;
			case MOD_KAMIKAZE:
				message = "falls to";
				message2 = "'s Kamikaze blast";
				causeShader = cgs.media.kamikazeShader;
				break;
			case MOD_JUICED:
				message = "was juiced by";
				causeShader = cgs.media.proxlauncherShader;
				break;
			case MOD_TELEFRAG:
				message = "tried to invade";
				message2 = "'s personal space";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_LAVA:
				message = "was given a hot bath by";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_SLIME:
				message = "was given a acid bath by";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_FALLING:
				message = "was given a small push by";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_TRIGGER_HURT:
				message = "was helped on the way by";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_CRUSH:
				message = "was crushed in";
				message2 = "'s trap";
				causeShader = cgs.media.skullShader;
				break;
			case MOD_SUICIDE:
				message = "was too eager against";
				causeShader = cgs.media.skullShader;
				break;
			default:
				message = "was killed by";
				causeShader = cgs.media.skullShader;
				break;
		}

		if (message) {
			if (ent->generic1) {
				message = "was killed by ^1TEAMMATE^7";
				message2 = "";
			}

			if (cg_obituaryOutput.integer == 1 || cg_obituaryOutput.integer == 4) {
				CG_Printf("%s %s %s%s\n",
						targetName, message, attackerName, message2);
			}
			if (cg_obituaryOutput.integer >= 2) {
				for (i = 0; i < FRAGMSG_MAX - 1; i++) {
					cgs.fragMsg[i] = cgs.fragMsg[i + 1];
				}
				memset(&cgs.fragMsg[FRAGMSG_MAX - 1], 0, sizeof (fragInfo_t));

				lastFrag->teamFrag = ent->generic1;

				Q_strncpyz(lastFrag->targetName, targetName,
						sizeof (lastFrag->targetName));
				Q_strncpyz(lastFrag->attackerName, attackerName,
						sizeof (lastFrag->attackerName));
				lastFrag->causeShader = causeShader;
				lastFrag->fragTime = cg.time;
				s = va("%s %s %s%s.", targetName, message, attackerName,
						message2);
				Q_strncpyz(lastFrag->message, s, sizeof (lastFrag->message));
			}

			return;
		}
	}

	// we don't know what it was
	if (cg_obituaryOutput.integer == 1 || cg_obituaryOutput.integer == 4) {
		CG_Printf("%s died.\n", targetName);
	}
	if (cg_obituaryOutput.integer >= 2) {
		for (i = 0; i < FRAGMSG_MAX - 1; i++) {
			cgs.fragMsg[i] = cgs.fragMsg[i + 1];
		}
		memset(&cgs.fragMsg[FRAGMSG_MAX - 1], 0, sizeof (fragInfo_t));

		Q_strncpyz(lastFrag->targetName, targetName, 32);
		Q_strncpyz(lastFrag->attackerName, attackerName, 32);
		lastFrag->causeShader = cgs.media.skullShader;
		lastFrag->fragTime = cg.time;
		Q_strncpyz(lastFrag->message, va("%s died.", targetName), 200);
	}
}

//==========================================================================

/*
===============
CG_UseItem
===============
 */
static void CG_UseItem(centity_t *cent) {
	clientInfo_t *ci;
	int itemNum, clientNum;
	gitem_t *item;
	entityState_t *es;

	es = &cent->currentState;

	itemNum = (es->event & ~EV_EVENT_BITS) - EV_USE_ITEM0;
	if (itemNum < 0 || itemNum > HI_NUM_HOLDABLE) {
		itemNum = 0;
	}

	// print a message if the local player
	if (es->number == cg.snap->ps.clientNum) {
		if (!itemNum) {
			CG_CenterPrint("No item to use", SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH);
		} else {
			item = BG_FindItemForHoldable(itemNum);
			CG_CenterPrint(va("Use %s", item->pickup_name), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH);
		}
	}

	switch (itemNum) {
		default:
		case HI_NONE:
			trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.useNothingSound);
			break;

		case HI_TELEPORTER:
			break;

		case HI_MEDKIT:
			clientNum = cent->currentState.clientNum;
			if (clientNum >= 0 && clientNum < MAX_CLIENTS) {
				ci = &cgs.clientinfo[ clientNum ];
				ci->medkitUsageTime = cg.time;
			}
			trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.medkitSound);
			break;

		case HI_KAMIKAZE:
			break;

		case HI_PORTAL:
			break;
		case HI_INVULNERABILITY:
			trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.useInvulnerabilitySound);
			break;
	}

}

/**
 * Function to check if a new is better according to cg_weaponOrder
 * If the order is unspecified (at least one of the weapons are not specified in cg_weaponOrder) the result is false
 * @param currentWeapon The weapon currently equipped
 * @param newWeapon The new weapon that we might want to change to
 * @return true if newWeapon is placed after currentWeapon in cg_weaponOrder. False otherwise
 */
static qboolean CG_WeaponHigher(int currentWeapon, int newWeapon) {
	char *currentScore = NULL;
	char *newScore = NULL;
	char weapon[5];
	Com_sprintf(weapon, 5, "/%i/", currentWeapon);
	currentScore = strstr(cg_weaponOrder.string, weapon);
	Com_sprintf(weapon, 5, "/%i/", newWeapon);
	newScore = strstr(cg_weaponOrder.string, weapon);
	if (!newScore || !currentScore) {
		return qfalse;
	}
	return newScore > currentScore;
}

/*
================
CG_ItemPickup

A new item was picked up this frame
================
 */
static void CG_ItemPickup(int itemNum) {
	cg.itemPickup = itemNum;
	cg.itemPickupTime = cg.time;
	cg.itemPickupBlendTime = cg.time;
	// see if it should be the grabbed weapon
	if (bg_itemlist[itemNum].giType == IT_WEAPON) {
		// select it immediately
		/* always*/
		if (cg_autoswitch.integer == 1 && bg_itemlist[itemNum].giTag != WP_MACHINEGUN) {
			cg.weaponSelectTime = cg.time;
			cg.weaponSelect = bg_itemlist[itemNum].giTag;
		}
		/* if new */
		if (cg_autoswitch.integer == 2 && 0 == (cg.snap->ps.stats[ STAT_WEAPONS ] & (1 << bg_itemlist[itemNum].giTag))) {
			cg.weaponSelectTime = cg.time;
			cg.weaponSelect = bg_itemlist[itemNum].giTag;
		}
		/* if better */
		if (cg_autoswitch.integer == 3 && CG_WeaponHigher(cg.weaponSelect, bg_itemlist[itemNum].giTag)) {
			cg.weaponSelectTime = cg.time;
			cg.weaponSelect = bg_itemlist[itemNum].giTag;
		}
		/* if new and better */
		if (cg_autoswitch.integer == 4 && 0 == (cg.snap->ps.stats[ STAT_WEAPONS ] & (1 << bg_itemlist[itemNum].giTag))
				&& CG_WeaponHigher(cg.weaponSelect, bg_itemlist[itemNum].giTag)) {
			cg.weaponSelectTime = cg.time;
			cg.weaponSelect = bg_itemlist[itemNum].giTag;
		}
		//
	}

}

/*
================
CG_WaterLevel

Returns waterlevel for entity origin
================
 */
int CG_WaterLevel(centity_t *cent) {
	vec3_t point;
	int contents, sample1, sample2, anim, waterlevel;
	int viewheight;

	anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;

	if (anim == LEGS_WALKCR || anim == LEGS_IDLECR) {
		viewheight = CROUCH_VIEWHEIGHT;
	} else {
		if (cg_enableQ.integer)
			viewheight = QUACK_VIEWHEIGHT;
		else
			viewheight = DEFAULT_VIEWHEIGHT;
	}

	//
	// get waterlevel, accounting for ducking
	//
	waterlevel = 0;

	point[0] = cent->lerpOrigin[0];
	point[1] = cent->lerpOrigin[1];
	point[2] = cent->lerpOrigin[2] + MINS_Z + 1;
	contents = CG_PointContents(point, -1);

	if (contents & MASK_WATER) {
		sample2 = viewheight - MINS_Z;
		sample1 = sample2 / 2;
		waterlevel = 1;
		point[2] = cent->lerpOrigin[2] + MINS_Z + sample1;
		contents = CG_PointContents(point, -1);

		if (contents & MASK_WATER) {
			waterlevel = 2;
			point[2] = cent->lerpOrigin[2] + MINS_Z + sample2;
			contents = CG_PointContents(point, -1);

			if (contents & MASK_WATER) {
				waterlevel = 3;
			}
		}
	}

	return waterlevel;
}

/*
================
CG_PainEvent

Also called by playerstate transition
================
 */
void CG_PainEvent(centity_t *cent, int health) {
	char *snd;

	// don't do more than two pain sounds a second
	if (cg.time - cent->pe.painTime < 500) {
		return;
	}

	if (health < 25) {
		snd = "*pain25_1.wav";
	} else if (health < 50) {
		snd = "*pain50_1.wav";
	} else if (health < 75) {
		snd = "*pain75_1.wav";
	} else {
		snd = "*pain100_1.wav";
	}
	// play a gurp sound instead of a normal pain sound
	if (CG_WaterLevel(cent) == 3) {
		if (rand()&1) {
			trap_S_StartSound(NULL, cent->currentState.number, CHAN_VOICE, CG_CustomSound(cent->currentState.number, "sound/player/gurp1.wav"));
		} else {
			trap_S_StartSound(NULL, cent->currentState.number, CHAN_VOICE, CG_CustomSound(cent->currentState.number, "sound/player/gurp2.wav"));
		}
	} else {
		trap_S_StartSound(NULL, cent->currentState.number, CHAN_VOICE, CG_CustomSound(cent->currentState.number, snd));
	}
	// save pain time for programitic twitch animation
	cent->pe.painTime = cg.time;
	cent->pe.painDirection ^= 1;
}



/*
==============
CG_EntityEvent

An entity has an event value
also called by CG_CheckPlayerstateEvents
==============
 */
#define DEBUGNAME(x) if(cg_debugEvents.integer){CG_Printf(x"\n");}

void CG_EntityEvent(centity_t *cent, vec3_t position) {
	entityState_t *es;
	int event;
	vec3_t dir;
	const char *s;
	int clientNum;
	clientInfo_t *ci;

	es = &cent->currentState;
	event = es->event & ~EV_EVENT_BITS;

	if (cg_debugEvents.integer) {
		CG_Printf("ent:%3i  event:%3i ", es->number, event);
	}

	if (!event) {
		DEBUGNAME("ZEROEVENT");
		return;
	}

	clientNum = es->clientNum;
	if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	switch (event) {
			//
			// movement generated events
			//
		case EV_FOOTSTEP:
			DEBUGNAME("EV_FOOTSTEP");
			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY,
						cgs.media.footsteps[ ci->footsteps ][rand()&3]);
			}
			break;
		case EV_FOOTSTEP_METAL:
			DEBUGNAME("EV_FOOTSTEP_METAL");
			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY,
						cgs.media.footsteps[ FOOTSTEP_METAL ][rand()&3]);
			}
			break;
		case EV_FOOTSPLASH:
			DEBUGNAME("EV_FOOTSPLASH");
			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY,
						cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3]);
			}
			break;
		case EV_FOOTWADE:
			DEBUGNAME("EV_FOOTWADE");
			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY,
						cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3]);
			}
			break;
		case EV_SWIM:
			DEBUGNAME("EV_SWIM");
			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY,
						cgs.media.footsteps[ FOOTSTEP_SPLASH ][rand()&3]);
			}
			break;


		case EV_FALL_SHORT:
			DEBUGNAME("EV_FALL_SHORT");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.landSound);
			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -8;
				cg.landTime = cg.time;
			}
			break;
		case EV_FALL_MEDIUM:
			DEBUGNAME("EV_FALL_MEDIUM");
			// use normal pain sound
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*pain100_1.wav"));
			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -16;
				cg.landTime = cg.time;
			}
			break;
		case EV_FALL_FAR:
			DEBUGNAME("EV_FALL_FAR");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*fall1.wav"));
			cent->pe.painTime = cg.time; // don't play a pain sound right after this
			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -24;
				cg.landTime = cg.time;
			}
			break;

		case EV_STEP_4:
		case EV_STEP_8:
		case EV_STEP_12:
		case EV_STEP_16: // smooth out step up transitions
			DEBUGNAME("EV_STEP");
		{
			float oldStep;
			int delta;
			int step;

			if (clientNum != cg.predictedPlayerState.clientNum) {
				break;
			}
			// if we are interpolating, we don't need to smooth steps
			if (cg.demoPlayback || (cg.snap->ps.pm_flags & PMF_FOLLOW) ||
					cg_nopredict.integer || cg_synchronousClients.integer) {
				break;
			}
			// check for stepping up before a previous step is completed
			delta = cg.time - cg.stepTime;
			if (delta < STEP_TIME) {
				oldStep = cg.stepChange * (STEP_TIME - delta) / STEP_TIME;
			} else {
				oldStep = 0;
			}

			// add this amount
			step = 4 * (event - EV_STEP_4 + 1);
			cg.stepChange = oldStep + step;
			if (cg.stepChange > MAX_STEP_CHANGE) {
				cg.stepChange = MAX_STEP_CHANGE;
			}
			cg.stepTime = cg.time;
			break;
		}

		case EV_JUMP_PAD:
			DEBUGNAME("EV_JUMP_PAD");
			//		CG_Printf( "EV_JUMP_PAD w/effect #%i\n", es->eventParm );
		{
			vec3_t up = {0, 0, 1};
			CG_SmokePuff(cent->lerpOrigin, up,
					32,
					1, 1, 1, 0.33f,
					1000,
					cg.time, 0,
					LEF_PUFF_DONT_SCALE,
					cgs.media.smokePuffShader);
		}

			// boing sound at origin, jump sound on player
			trap_S_StartSound(cent->lerpOrigin, -1, CHAN_VOICE, cgs.media.jumpPadSound);
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*jump1.wav"));
			break;

		case EV_JUMP:
			DEBUGNAME("EV_JUMP");
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*jump1.wav"));
			break;
		case EV_TAUNT:
			DEBUGNAME("EV_TAUNT");
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*taunt.wav"));
			break;
#ifdef MISSIONPACK
		case EV_TAUNT_YES:
			DEBUGNAME("EV_TAUNT_YES");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_YES);
			break;
		case EV_TAUNT_NO:
			DEBUGNAME("EV_TAUNT_NO");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_NO);
			break;
		case EV_TAUNT_FOLLOWME:
			DEBUGNAME("EV_TAUNT_FOLLOWME");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_FOLLOWME);
			break;
		case EV_TAUNT_GETFLAG:
			DEBUGNAME("EV_TAUNT_GETFLAG");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_ONGETFLAG);
			break;
		case EV_TAUNT_GUARDBASE:
			DEBUGNAME("EV_TAUNT_GUARDBASE");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_ONDEFENSE);
			break;
		case EV_TAUNT_PATROL:
			DEBUGNAME("EV_TAUNT_PATROL");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_ONPATROL);
			break;
#endif
		case EV_WATER_TOUCH:
			DEBUGNAME("EV_WATER_TOUCH");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.watrInSound);
			break;
		case EV_WATER_LEAVE:
			DEBUGNAME("EV_WATER_LEAVE");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.watrOutSound);
			break;
		case EV_WATER_UNDER:
			DEBUGNAME("EV_WATER_UNDER");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.watrUnSound);
			break;
		case EV_WATER_CLEAR:
			DEBUGNAME("EV_WATER_CLEAR");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*gasp.wav"));
			break;

		case EV_ITEM_PICKUP:
			DEBUGNAME("EV_ITEM_PICKUP");
		{
			gitem_t *item;
			int index;

			index = es->eventParm; // player predicted

			if (index < 1 || index >= bg_numItems) {
				break;
			}
			item = &bg_itemlist[ index ];

			// powerups and team items will have a separate global sound, this one
			// will be played at prediction time
			if (item->giType == IT_POWERUP || item->giType == IT_TEAM) {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.n_healthSound);
			} else if (item->giType == IT_PERSISTANT_POWERUP) {
				switch (item->giTag) {
					case PW_SCOUT:
						trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.scoutSound);
						break;
					case PW_GUARD:
						trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.guardSound);
						break;
					case PW_DOUBLER:
						trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.doublerSound);
						break;
					case PW_AMMOREGEN:
						trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.ammoregenSound);
						break;
				}
			} else {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, trap_S_RegisterSound(item->pickup_sound, qfalse));
			}

			// show icon and name on status bar
			if (es->number == cg.snap->ps.clientNum) {
				CG_ItemPickup(index);
			}
		}
			break;

		case EV_GLOBAL_ITEM_PICKUP:
			DEBUGNAME("EV_GLOBAL_ITEM_PICKUP");
		{
			gitem_t *item;
			int index;

			index = es->eventParm; // player predicted

			if (index < 1 || index >= bg_numItems) {
				break;
			}
			item = &bg_itemlist[ index ];
			// powerup pickups are global
			if (item->pickup_sound) {
				trap_S_StartSound(NULL, cg.snap->ps.clientNum, CHAN_AUTO, trap_S_RegisterSound(item->pickup_sound, qfalse));
			}

			// show icon and name on status bar
			if (es->number == cg.snap->ps.clientNum) {
				CG_ItemPickup(index);
			}
		}
			break;

			//
			// weapon events
			//
		case EV_NOAMMO:
			DEBUGNAME("EV_NOAMMO");
			//		trap_S_StartSound (NULL, es->number, CHAN_AUTO, cgs.media.noAmmoSound );
			if (es->number == cg.snap->ps.clientNum) {
				CG_OutOfAmmoChange();
			}
			break;
		case EV_CHANGE_WEAPON:
			DEBUGNAME("EV_CHANGE_WEAPON");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.selectSound);
			break;
		case EV_FIRE_WEAPON:
			DEBUGNAME("EV_FIRE_WEAPON");
			CG_FireWeapon(cent);
			break;

		case EV_USE_ITEM0:
			DEBUGNAME("EV_USE_ITEM0");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM1:
			DEBUGNAME("EV_USE_ITEM1");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM2:
			DEBUGNAME("EV_USE_ITEM2");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM3:
			DEBUGNAME("EV_USE_ITEM3");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM4:
			DEBUGNAME("EV_USE_ITEM4");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM5:
			DEBUGNAME("EV_USE_ITEM5");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM6:
			DEBUGNAME("EV_USE_ITEM6");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM7:
			DEBUGNAME("EV_USE_ITEM7");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM8:
			DEBUGNAME("EV_USE_ITEM8");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM9:
			DEBUGNAME("EV_USE_ITEM9");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM10:
			DEBUGNAME("EV_USE_ITEM10");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM11:
			DEBUGNAME("EV_USE_ITEM11");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM12:
			DEBUGNAME("EV_USE_ITEM12");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM13:
			DEBUGNAME("EV_USE_ITEM13");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM14:
			DEBUGNAME("EV_USE_ITEM14");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM15:
			DEBUGNAME("EV_USE_ITEM15");
			CG_UseItem(cent);
			break;

			//=================================================================

			//
			// other events
			//
		case EV_PLAYER_TELEPORT_IN:
			DEBUGNAME("EV_PLAYER_TELEPORT_IN");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.teleInSound);
			if (cg_leiEnhancement.integer)
				trap_R_LFX_ParticleEffect(18, cent->lerpOrigin, cent->currentState.angles);
			else
				CG_SpawnEffect(position);
			break;

		case EV_PLAYER_TELEPORT_OUT:
			DEBUGNAME("EV_PLAYER_TELEPORT_OUT");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.teleOutSound);
			if (cg_leiEnhancement.integer)
				trap_R_LFX_ParticleEffect(18, cent->lerpOrigin, cent->currentState.angles);
			else
				CG_SpawnEffect(position);
			break;

		case EV_ITEM_POP:
			DEBUGNAME("EV_ITEM_POP");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.respawnSound);
			break;
		case EV_ITEM_RESPAWN:
			DEBUGNAME("EV_ITEM_RESPAWN");
			cent->miscTime = cg.time; // scale up from this
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.respawnSound);

			if (cg_leiEnhancement.integer)
				trap_R_LFX_ParticleEffect(41, cent->lerpOrigin, cent->currentState.angles);
			break;

		case EV_GRENADE_BOUNCE:
			DEBUGNAME("EV_GRENADE_BOUNCE");
			if (rand() & 1) {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.hgrenb1aSound);
			} else {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.hgrenb2aSound);
			}
			break;


		case EV_PROXIMITY_MINE_STICK:
			DEBUGNAME("EV_PROXIMITY_MINE_STICK");
			if (es->eventParm & SURF_FLESH) {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.wstbimplSound);
			} else if (es->eventParm & SURF_METALSTEPS) {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.wstbimpmSound);
			} else {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.wstbimpdSound);
			}
			break;

		case EV_PROXIMITY_MINE_TRIGGER:
			DEBUGNAME("EV_PROXIMITY_MINE_TRIGGER");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.wstbactvSound);
			break;
		case EV_KAMIKAZE:
			DEBUGNAME("EV_KAMIKAZE");
			CG_KamikazeEffect(cent->lerpOrigin);
			break;
		case EV_OBELISKEXPLODE:
			DEBUGNAME("EV_OBELISKEXPLODE");
			if (cg_leiEnhancement.integer)
				trap_R_LFX_ParticleEffect(17, cent->lerpOrigin, cent->currentState.angles);
			else
				CG_ObeliskExplode(cent->lerpOrigin, es->eventParm);
			break;
		case EV_OBELISKPAIN:
			DEBUGNAME("EV_OBELISKPAIN");
			CG_ObeliskPain(cent->lerpOrigin);
			break;
		case EV_INVUL_IMPACT:
			DEBUGNAME("EV_INVUL_IMPACT");
			if (cg_leiEnhancement.integer)
				trap_R_LFX_ParticleEffect(42, cent->lerpOrigin, cent->currentState.angles);
			else
				CG_InvulnerabilityImpact(cent->lerpOrigin, cent->currentState.angles);
			break;
		case EV_JUICED:
			DEBUGNAME("EV_JUICED");
			CG_InvulnerabilityJuiced(cent->lerpOrigin);
			break;
		case EV_LIGHTNINGBOLT:
			DEBUGNAME("EV_LIGHTNINGBOLT");
			CG_LightningBoltBeam(es->origin2, es->pos.trBase);
			break;
		case EV_SCOREPLUM:
			DEBUGNAME("EV_SCOREPLUM");
			CG_ScorePlum(cent->currentState.otherEntityNum, cent->lerpOrigin, cent->currentState.time);
			break;

			//
			// missile impacts
			//
		case EV_MISSILE_HIT:
			DEBUGNAME("EV_MISSILE_HIT");
			ByteToDir(es->eventParm, dir);
			CG_MissileHitPlayer(es->weapon, position, dir, es->otherEntityNum);
			break;

		case EV_MISSILE_MISS:
			DEBUGNAME("EV_MISSILE_MISS");
			ByteToDir(es->eventParm, dir);
			CG_MissileHitWall(es->weapon, 0, position, dir, IMPACTSOUND_DEFAULT);
			break;

		case EV_MISSILE_MISS_METAL:
			DEBUGNAME("EV_MISSILE_MISS_METAL");
			ByteToDir(es->eventParm, dir);
			CG_MissileHitWall(es->weapon, 0, position, dir, IMPACTSOUND_METAL);
			break;

		case EV_RAILTRAIL:
			DEBUGNAME("EV_RAILTRAIL");
			cent->currentState.weapon = WP_RAILGUN;
			//unlagged - attack prediction #2
			// if the client is us, unlagged is on server-side, and we've got it client-side
			if (es->clientNum == cg.predictedPlayerState.clientNum &&
					cgs.delagHitscan && (cg_delag.integer & 1 || cg_delag.integer & 16)) {
				// do nothing, because it was already predicted
				//Com_Printf("Ignoring rail trail event\n");
			} else {
				if (es->clientNum == cg.snap->ps.clientNum && !cg.renderingThirdPerson) {
					if (cg_drawGun.integer == 2)
						VectorMA(es->origin2, 8, cg.refdef.viewaxis[1], es->origin2);
					else if (cg_drawGun.integer == 3)
						VectorMA(es->origin2, 4, cg.refdef.viewaxis[1], es->origin2);
				}


				// draw a rail trail, because it wasn't predicted
				CG_RailTrail(ci, es->origin2, es->pos.trBase);

				// if the end was on a nomark surface, don't make an explosion
				if (es->eventParm != 255) {
					ByteToDir(es->eventParm, dir);
					CG_MissileHitWall(es->weapon, es->clientNum, position, dir, IMPACTSOUND_DEFAULT);
				}
				//Com_Printf("Non-predicted rail trail\n");
			}
			//unlagged - attack prediction #2
			break;

		case EV_BULLET_HIT_WALL:
			DEBUGNAME("EV_BULLET_HIT_WALL");
			//unlagged - attack prediction #2
			// if the client is us, unlagged is on server-side, and we've got it client-side
			if (es->clientNum == cg.predictedPlayerState.clientNum &&
					cgs.delagHitscan && (cg_delag.integer & 1 || cg_delag.integer & 2)) {
				// do nothing, because it was already predicted
				//Com_Printf("Ignoring bullet event\n");
			} else {
				// do the bullet, because it wasn't predicted
				ByteToDir(es->eventParm, dir);
				CG_Bullet(es->pos.trBase, es->otherEntityNum, dir, qfalse, ENTITYNUM_WORLD);
				//Com_Printf("Non-predicted bullet\n");
			}
			//unlagged - attack prediction #2
			break;

		case EV_BULLET_HIT_FLESH:
			DEBUGNAME("EV_BULLET_HIT_FLESH");
			//unlagged - attack prediction #2
			// if the client is us, unlagged is on server-side, and we've got it client-side
			if (es->clientNum == cg.predictedPlayerState.clientNum &&
					cgs.delagHitscan && (cg_delag.integer & 1 || cg_delag.integer & 2)) {
				// do nothing, because it was already predicted
				//Com_Printf("Ignoring bullet event\n");
			} else {
				// do the bullet, because it wasn't predicted
				CG_Bullet(es->pos.trBase, es->otherEntityNum, dir, qtrue, es->eventParm);
				//Com_Printf("Non-predicted bullet\n");
			}
			//unlagged - attack prediction #2
			break;

		case EV_SHOTGUN:
			DEBUGNAME("EV_SHOTGUN");
			//unlagged - attack prediction #2
			// if the client is us, unlagged is on server-side, and we've got it client-side
			if (es->otherEntityNum == cg.predictedPlayerState.clientNum &&
					cgs.delagHitscan && (cg_delag.integer & 1 || cg_delag.integer & 4)) {
				// do nothing, because it was already predicted
				//Com_Printf("Ignoring shotgun event\n");
			} else {
				// do the shotgun pattern, because it wasn't predicted
				CG_ShotgunFire(es);
				//Com_Printf("Non-predicted shotgun pattern\n");
			}
			//unlagged - attack prediction #2
			break;

		case EV_GENERAL_SOUND:
			DEBUGNAME("EV_GENERAL_SOUND");
			if (cgs.gameSounds[ es->eventParm ]) {
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, cgs.gameSounds[ es->eventParm ]);
			} else {
				s = CG_ConfigString(CS_SOUNDS + es->eventParm);
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, s));
			}
			break;

		case EV_GLOBAL_SOUND: // play from the player's head so it never diminishes
			DEBUGNAME("EV_GLOBAL_SOUND");
			if (cgs.gameSounds[ es->eventParm ]) {
				trap_S_StartSound(NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.gameSounds[ es->eventParm ]);
			} else {
				s = CG_ConfigString(CS_SOUNDS + es->eventParm);
				trap_S_StartSound(NULL, cg.snap->ps.clientNum, CHAN_AUTO, CG_CustomSound(es->number, s));
			}
			break;

		case EV_GLOBAL_TEAM_SOUND: // play from the player's head so it never diminishes
		{
			DEBUGNAME("EV_GLOBAL_TEAM_SOUND");
			switch (es->eventParm) {
				case GTS_RED_CAPTURE: // CTF: red team captured the blue flag, 1FCTF: red team captured the neutral flag
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED)
						CG_AddBufferedSound(cgs.media.captureYourTeamSound);
					else
						CG_AddBufferedSound(cgs.media.captureOpponentSound);
					break;
				case GTS_BLUE_CAPTURE: // CTF: blue team captured the red flag, 1FCTF: blue team captured the neutral flag
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
						CG_AddBufferedSound(cgs.media.captureYourTeamSound);
					else
						CG_AddBufferedSound(cgs.media.captureOpponentSound);
					break;
				case GTS_RED_RETURN: // CTF: blue flag returned, 1FCTF: never used
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED)
						CG_AddBufferedSound(cgs.media.returnYourTeamSound);
					else
						CG_AddBufferedSound(cgs.media.returnOpponentSound);
					//
					CG_AddBufferedSound(cgs.media.blueFlagReturnedSound);
					break;
				case GTS_BLUE_RETURN: // CTF red flag returned, 1FCTF: neutral flag returned
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE)
						CG_AddBufferedSound(cgs.media.returnYourTeamSound);
					else
						CG_AddBufferedSound(cgs.media.returnOpponentSound);
					//
					CG_AddBufferedSound(cgs.media.redFlagReturnedSound);
					break;

				case GTS_RED_TAKEN: // CTF: red team took blue flag, 1FCTF: blue team took the neutral flag
					// if this player picked up the flag then a sound is played in CG_CheckLocalSounds
					if (cg.snap->ps.powerups[PW_BLUEFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG]) {
					} else {
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
							if (cgs.gametype == GT_1FCTF)
								CG_AddBufferedSound(cgs.media.yourTeamTookTheFlagSound);
							else
								CG_AddBufferedSound(cgs.media.enemyTookYourFlagSound);
						} else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
							if (cgs.gametype == GT_1FCTF)
								CG_AddBufferedSound(cgs.media.enemyTookTheFlagSound);
							else
								CG_AddBufferedSound(cgs.media.yourTeamTookEnemyFlagSound);
						}
					}
					break;
				case GTS_BLUE_TAKEN: // CTF: blue team took the red flag, 1FCTF red team took the neutral flag
					// if this player picked up the flag then a sound is played in CG_CheckLocalSounds
					if (cg.snap->ps.powerups[PW_REDFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG]) {
					} else {
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
							if (cgs.gametype == GT_1FCTF)
								CG_AddBufferedSound(cgs.media.yourTeamTookTheFlagSound);
							else
								CG_AddBufferedSound(cgs.media.enemyTookYourFlagSound);
						} else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
							if (cgs.gametype == GT_1FCTF)
								CG_AddBufferedSound(cgs.media.enemyTookTheFlagSound);
							else
								CG_AddBufferedSound(cgs.media.yourTeamTookEnemyFlagSound);
						}
					}
					break;
				case GTS_REDOBELISK_ATTACKED: // Overload: red obelisk is being attacked
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
						CG_AddBufferedSound(cgs.media.yourBaseIsUnderAttackSound);
					}
					break;
				case GTS_BLUEOBELISK_ATTACKED: // Overload: blue obelisk is being attacked
					if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
						CG_AddBufferedSound(cgs.media.yourBaseIsUnderAttackSound);
					}
					break;

				case GTS_REDTEAM_SCORED:
					CG_AddBufferedSound(cgs.media.redScoredSound);
					break;
				case GTS_BLUETEAM_SCORED:
					CG_AddBufferedSound(cgs.media.blueScoredSound);
					break;
				case GTS_REDTEAM_TOOK_LEAD:
					CG_AddBufferedSound(cgs.media.redLeadsSound);
					break;
				case GTS_BLUETEAM_TOOK_LEAD:
					CG_AddBufferedSound(cgs.media.blueLeadsSound);
					break;
				case GTS_TEAMS_ARE_TIED:
					CG_AddBufferedSound(cgs.media.teamsTiedSound);
					break;
				case GTS_KAMIKAZE:
					trap_S_StartLocalSound(cgs.media.kamikazeFarSound, CHAN_ANNOUNCER);
					break;
				default:
					break;
			}
			break;
		}

		case EV_PAIN:
			// local player sounds are triggered in CG_CheckLocalSounds,
			// so ignore events on the player
			DEBUGNAME("EV_PAIN");
			if (cent->currentState.number != cg.snap->ps.clientNum) {
				CG_PainEvent(cent, es->eventParm);
			}
			break;

		case EV_DEATH1:
		case EV_DEATH2:
		case EV_DEATH3:
			DEBUGNAME("EV_DEATHx");

			if (CG_WaterLevel(cent) == 3) {
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*drown.wav"));
			} else {
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, va("*death%i.wav", event - EV_DEATH1 + 1)));
			}

			break;


		case EV_OBITUARY:
			DEBUGNAME("EV_OBITUARY");
			CG_Obituary(es);
			break;

			//
			// powerup events
			//
		case EV_POWERUP_QUAD:
			DEBUGNAME("EV_POWERUP_QUAD");
			if (es->number == cg.snap->ps.clientNum) {
				cg.powerupActive = PW_QUAD;
				cg.powerupTime = cg.time;
			}
			trap_S_StartSound(NULL, es->number, CHAN_ITEM, cgs.media.quadSound);
			break;
		case EV_POWERUP_BATTLESUIT:
			DEBUGNAME("EV_POWERUP_BATTLESUIT");
			if (es->number == cg.snap->ps.clientNum) {
				cg.powerupActive = PW_BATTLESUIT;
				cg.powerupTime = cg.time;
			}
			trap_S_StartSound(NULL, es->number, CHAN_ITEM, cgs.media.protectSound);
			break;
		case EV_POWERUP_REGEN:
			DEBUGNAME("EV_POWERUP_REGEN");
			if (es->number == cg.snap->ps.clientNum) {
				cg.powerupActive = PW_REGEN;
				cg.powerupTime = cg.time;
			}
			trap_S_StartSound(NULL, es->number, CHAN_ITEM, cgs.media.regenSound);
			break;

		case EV_GIB_PLAYER:
			DEBUGNAME("EV_GIB_PLAYER");
			// don't play gib sound when using the kamikaze because it interferes
			// with the kamikaze sound, downside is that the gib sound will also
			// not be played when someone is gibbed while just carrying the kamikaze
			if (!(es->eFlags & EF_KAMIKAZE)) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.gibSound);
			}
			CG_GibPlayer(cent->lerpOrigin);
			// TODO: New function option for a more directional gib effect
			if (cg_leiEnhancement.integer)
				trap_R_LFX_ParticleEffect(16, cent->lerpOrigin, cent->currentState.angles);

			break;

		case EV_STOPLOOPINGSOUND:
			DEBUGNAME("EV_STOPLOOPINGSOUND");
			trap_S_StopLoopingSound(es->number);
			es->loopSound = 0;
			break;

		case EV_DEBUG_LINE:
			DEBUGNAME("EV_DEBUG_LINE");
			CG_Beam(cent);
			break;

		default:
			DEBUGNAME("UNKNOWN");
			CG_Error("Unknown event: %i", event);
			break;
	}

}

/*
==============
CG_CheckEvents

==============
 */
void CG_CheckEvents(centity_t *cent) {
	// check for event-only entities
	if (cent->currentState.eType > ET_EVENTS) {
		if (cent->previousEvent) {
			return; // already fired
		}
		// if this is a player event set the entity number of the client entity number
		if (cent->currentState.eFlags & EF_PLAYER_EVENT) {
			cent->currentState.number = cent->currentState.otherEntityNum;
		}

		cent->previousEvent = 1;

		cent->currentState.event = cent->currentState.eType - ET_EVENTS;
	} else {
		// check for events riding with another entity
		if (cent->currentState.event == cent->previousEvent) {
			return;
		}
		cent->previousEvent = cent->currentState.event;
		if ((cent->currentState.event & ~EV_EVENT_BITS) == 0) {
			return;
		}
	}

	// calculate the position at exactly the frame time
	BG_EvaluateTrajectory(&cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin);
	CG_SetEntitySoundPosition(cent);

	CG_EntityEvent(cent, cent->lerpOrigin);
}


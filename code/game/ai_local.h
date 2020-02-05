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

/*****************************************************************************
 * name:		ai_main.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

//#define DEBUG
#define CTF

#define MAX_ITEMS					256
//bot flags
#define BFL_STRAFERIGHT				1	//strafe to the right
#define BFL_ATTACKED				2	//bot has attacked last ai frame
#define BFL_ATTACKJUMPED			4	//bot jumped during attack last frame
#define BFL_AIMATENEMY				8	//bot aimed at the enemy this frame
#define BFL_AVOIDRIGHT				16	//avoid obstacles by going to the right
#define BFL_IDEALVIEWSET			32	//bot has ideal view angles set
#define BFL_FIGHTSUICIDAL			64	//bot is in a suicidal fight
//long term goal types
#define LTG_TEAMHELP				1	//help a team mate
#define LTG_TEAMACCOMPANY			2	//accompany a team mate
#define LTG_DEFENDKEYAREA			3	//defend a key area
#define LTG_GETFLAG					4	//get the enemy flag
#define LTG_RUSHBASE				5	//rush to the base
#define LTG_RETURNFLAG				6	//return the flag
#define LTG_CAMP					7	//camp somewhere
#define LTG_CAMPORDER				8	//ordered to camp somewhere
#define LTG_PATROL					9	//patrol
#define LTG_GETITEM					10	//get an item
#define LTG_KILL					11	//kill someone
#define LTG_HARVEST					12	//harvest skulls
#define LTG_ATTACKENEMYBASE			13	//attack the enemy base
#define LTG_MAKELOVE_UNDER			14
#define LTG_MAKELOVE_ONTOP			15
//Long term DD goals
#define LTG_POINTA				16	//Take/Defend point A
#define LTG_POINTB				17	//Take/Defend point B
//Long term DD goals
#define LTG_DOMROAM                             18      //Go for a non taken point.
#define LTG_DOMHOLD                             19      //Pick a point and hold it.
//some goal dedication times
#define TEAM_HELP_TIME				60	//1 minute teamplay help time
#define TEAM_ACCOMPANY_TIME			600	//10 minutes teamplay accompany time
#define TEAM_DEFENDKEYAREA_TIME		600	//10 minutes ctf defend base time
#define TEAM_CAMP_TIME				600	//10 minutes camping time
#define TEAM_PATROL_TIME			600	//10 minutes patrolling time
#define TEAM_LEAD_TIME				600	//10 minutes taking the lead
#define TEAM_GETITEM_TIME			60	//1 minute
#define	TEAM_KILL_SOMEONE			180	//3 minute to kill someone
#define TEAM_ATTACKENEMYBASE_TIME	600	//10 minutes
#define TEAM_HARVEST_TIME			120	//2 minutes
#define CTF_GETFLAG_TIME			600	//10 minutes ctf get flag time
#define CTF_RUSHBASE_TIME			120	//2 minutes ctf rush base time
#define CTF_RETURNFLAG_TIME			180	//3 minutes to return the flag
#define CTF_ROAM_TIME				60	//1 minute ctf roam time
//Time for Double Domination tasks
#define DD_POINTA				600
#define DD_POINTB				600
//patrol flags
#define PATROL_LOOP					1
#define PATROL_REVERSE				2
#define PATROL_BACK					4
//teamplay task preference
#define TEAMTP_DEFENDER				1
#define TEAMTP_ATTACKER				2
//CTF strategy
#define CTFS_AGRESSIVE				1
//copied from the aas file header
#define PRESENCE_NONE				1
#define PRESENCE_NORMAL				2
#define PRESENCE_CROUCH				4
//
#define MAX_PROXMINES				64

//check points
typedef struct bot_waypoint_s
{
	int			inuse;
	char		name[32];
	bot_goal_t	goal;
	struct		bot_waypoint_s *next, *prev;
} bot_waypoint_t;

#define MAX_ACTIVATESTACK		8
#define MAX_ACTIVATEAREAS		32

typedef struct bot_activategoal_s
{
	int inuse;
	bot_goal_t goal;						//goal to activate (buttons etc.)
	float time;								//time to activate something
	float start_time;						//time starting to activate something
	float justused_time;					//time the goal was used
	int shoot;								//true if bot has to shoot to activate
	int weapon;								//weapon to be used for activation
	vec3_t target;							//target to shoot at to activate something
	vec3_t origin;							//origin of the blocking entity to activate
	int areas[MAX_ACTIVATEAREAS];			//routing areas disabled by blocking entity
	int numareas;							//number of disabled routing areas
	int areasdisabled;						//true if the areas are disabled for the routing
	struct bot_activategoal_s *next;		//next activate goal on stack
} bot_activategoal_t;

//bot state
typedef struct bot_state_s
{
	int inuse;										//true if this state is used by a bot client
	int botthink_residual;							//residual for the bot thinks
	int client;										//client number of the bot
	int entitynum;									//entity number of the bot
	playerState_t cur_ps;							//current player state
	int last_eFlags;								//last ps flags
	usercmd_t lastucmd;								//usercmd from last frame
	int entityeventTime[1024];						//last entity event time
	//
	bot_settings_t settings;						//several bot settings
	int (*ainode)(struct bot_state_s *bs);			//current AI node
	float thinktime;								//time the bot thinks this frame
	vec3_t origin;									//origin of the bot
	vec3_t velocity;								//velocity of the bot
	int presencetype;								//presence type of the bot
	vec3_t eye;										//eye coordinates of the bot
	int areanum;									//the number of the area the bot is in
	int inventory[MAX_ITEMS];						//string with items amounts the bot has
	int tfl;										//the travel flags the bot uses
	int flags;										//several flags
	int respawn_wait;								//wait until respawned
	int lasthealth;									//health value previous frame
	int lastkilledplayer;							//last killed player
	int lastkilledby;								//player that last killed this bot
	int botdeathtype;								//the death type of the bot
	int enemydeathtype;								//the death type of the enemy
	int botsuicide;									//true when the bot suicides
	int enemysuicide;								//true when the enemy of the bot suicides
	int setupcount;									//true when the bot has just been setup
	int map_restart;									//true when the map is being restarted
	int entergamechat;								//true when the bot used an enter game chat
	int num_deaths;									//number of time this bot died
	int num_kills;									//number of kills of this bot
	int revenge_enemy;								//the revenge enemy
	int revenge_kills;								//number of kills the enemy made
	int lastframe_health;							//health value the last frame
	int lasthitcount;								//number of hits last frame
	int chatto;										//chat to all or team
	float walker;									//walker charactertic
	float ltime;									//local bot time
	float entergame_time;							//time the bot entered the game
	float ltg_time;									//long term goal time
	float nbg_time;									//nearby goal time
	float respawn_time;								//time the bot takes to respawn
	float respawnchat_time;							//time the bot started a chat during respawn
	float chase_time;								//time the bot will chase the enemy
	float enemyvisible_time;						//time the enemy was last visible
	float check_time;								//time to check for nearby items
	float stand_time;								//time the bot is standing still
	float lastchat_time;							//time the bot last selected a chat
	float kamikaze_time;							//time to check for kamikaze usage
	float invulnerability_time;						//time to check for invulnerability usage
	float standfindenemy_time;						//time to find enemy while standing
	float attackstrafe_time;						//time the bot is strafing in one dir
	float attackcrouch_time;						//time the bot will stop crouching
	float attackchase_time;							//time the bot chases during actual attack
	float attackjump_time;							//time the bot jumped during attack
	float enemysight_time;							//time before reacting to enemy
	float enemydeath_time;							//time the enemy died
	float enemyposition_time;						//time the position and velocity of the enemy were stored
	float defendaway_time;							//time away while defending
	float defendaway_range;							//max travel time away from defend area
	float rushbaseaway_time;						//time away from rushing to the base
	float attackaway_time;							//time away from attacking the enemy base
	float harvestaway_time;							//time away from harvesting
	float ctfroam_time;								//time the bot is roaming in ctf
	float killedenemy_time;							//time the bot killed the enemy
	float arrive_time;								//time arrived (at companion)
	float lastair_time;								//last time the bot had air
	float teleport_time;							//last time the bot teleported
	float camp_time;								//last time camped
	float camp_range;								//camp range
	float weaponchange_time;						//time the bot started changing weapons
	float firethrottlewait_time;					//amount of time to wait
	float firethrottleshoot_time;					//amount of time to shoot
	float notblocked_time;							//last time the bot was not blocked
	float blockedbyavoidspot_time;					//time blocked by an avoid spot
	float predictobstacles_time;					//last time the bot predicted obstacles
	int predictobstacles_goalareanum;				//last goal areanum the bot predicted obstacles for
	vec3_t aimtarget;
	vec3_t enemyvelocity;							//enemy velocity 0.5 secs ago during battle
	vec3_t enemyorigin;								//enemy origin 0.5 secs ago during battle
	//
	int kamikazebody;								//kamikaze body
	int proxmines[MAX_PROXMINES];
	int numproxmines;
	//
	int character;									//the bot character
	int ms;											//move state of the bot
	int gs;											//goal state of the bot
	int cs;											//chat state of the bot
	int ws;											//weapon state of the bot
	//
	int enemy;										//enemy entity number
	int lastenemyareanum;							//last reachability area the enemy was in
	vec3_t lastenemyorigin;							//last origin of the enemy in the reachability area
	int weaponnum;									//current weapon number
	vec3_t viewangles;								//current view angles
	vec3_t ideal_viewangles;						//ideal view angles
	vec3_t viewanglespeed;
	//
	int ltgtype;									//long term goal type
	// team goals
	int teammate;									//team mate involved in this team goal
	int decisionmaker;								//player who decided to go for this goal
	int ordered;									//true if ordered to do something
	float order_time;								//time ordered to do something
	int owndecision_time;							//time the bot made it's own decision
	bot_goal_t teamgoal;							//the team goal
	bot_goal_t altroutegoal;						//alternative route goal
	float reachedaltroutegoal_time;					//time the bot reached the alt route goal
	float teammessage_time;							//time to message team mates what the bot is doing
	float teamgoal_time;							//time to stop helping team mate
	float teammatevisible_time;						//last time the team mate was NOT visible
	int teamtaskpreference;							//team task preference
	// last ordered team goal
	int lastgoal_decisionmaker;
	int lastgoal_ltgtype;
	int lastgoal_teammate;
	bot_goal_t lastgoal_teamgoal;
	// for leading team mates
	int lead_teammate;								//team mate the bot is leading
	bot_goal_t lead_teamgoal;						//team goal while leading
	float lead_time;								//time leading someone
	float leadvisible_time;							//last time the team mate was visible
	float leadmessage_time;							//last time a messaged was sent to the team mate
	float leadbackup_time;							//time backing up towards team mate
	//
	char teamleader[MAX_NETNAME];					//netname of the team leader
	float askteamleader_time;						//time asked for team leader
	float becometeamleader_time;					//time the bot will become the team leader
	float teamgiveorders_time;						//time to give team orders
	float lastflagcapture_time;						//last time a flag was captured
	int numteammates;								//number of team mates
	int redflagstatus;								//0 = at base, 1 = not at base
	int blueflagstatus;								//0 = at base, 1 = not at base
	int neutralflagstatus;							//0 = at base, 1 = our team has flag, 2 = enemy team has flag, 3 = enemy team dropped the flag
	int flagstatuschanged;							//flag status changed
	int forceorders;								//true if forced to give orders
	int flagcarrier;								//team mate carrying the enemy flag
	int ctfstrategy;								//ctf strategy
	char subteam[32];								//sub team name
	float formation_dist;							//formation team mate intervening space
	char formation_teammate[16];					//netname of the team mate the bot uses for relative positioning
	float formation_angle;							//angle relative to the formation team mate
	vec3_t formation_dir;							//the direction the formation is moving in
	vec3_t formation_origin;						//origin the bot uses for relative positioning
	bot_goal_t formation_goal;						//formation goal

	bot_activategoal_t *activatestack;				//first activate goal on the stack
	bot_activategoal_t activategoalheap[MAX_ACTIVATESTACK];	//activate goal heap

	bot_waypoint_t *checkpoints;					//check points
	bot_waypoint_t *patrolpoints;					//patrol points
	bot_waypoint_t *curpatrolpoint;					//current patrol point the bot is going for
	int patrolflags;								//patrol flags
} bot_state_t;

//resets the whole bot state
void BotResetState(bot_state_t *bs);
//returns the number of bots in the game
int NumBots(void);
//returns info about the entity
void BotEntityInfo(int entnum, aas_entityinfo_t *info);

extern float floattime;
#define FloatTime() floattime

// from the game source
void	QDECL BotAI_Print(int type, const char *fmt, ...) __attribute__((format(printf,2,3)));
void	QDECL QDECL BotAI_BotInitialChat( bot_state_t *bs, char *type, ... );
void	BotAI_Trace(bsp_trace_t *bsptrace, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask);
int		BotAI_GetClientState( int clientNum, playerState_t *state );
int		BotAI_GetEntityState( int entityNum, entityState_t *state );
int		BotAI_GetSnapshotEntity( int clientNum, int sequence, entityState_t *state );
int		BotTeamLeader(bot_state_t *bs);

/*****************************************************************************
 * name:		ai_dmq3.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

//setup the deathmatch AI
void BotSetupDeathmatchAI(void);
//shutdown the deathmatch AI
void BotShutdownDeathmatchAI(void);
//let the bot live within it's deathmatch AI net
void BotDeathmatchAI(bot_state_t *bs, float thinktime);
//free waypoints
void BotFreeWaypoints(bot_waypoint_t *wp);
//choose a weapon
void BotChooseWeapon(bot_state_t *bs);
//setup movement stuff
void BotSetupForMovement(bot_state_t *bs);
//update the inventory
void BotUpdateInventory(bot_state_t *bs);
//update the inventory during battle
void BotUpdateBattleInventory(bot_state_t *bs, int enemy);
//use holdable items during battle
void BotBattleUseItems(bot_state_t *bs);
//return true if the bot is dead
qboolean BotIsDead(bot_state_t *bs);
//returns true if the bot is in observer mode
qboolean BotIsObserver(bot_state_t *bs);
//returns true if the bot is in the intermission
qboolean BotIntermission(bot_state_t *bs);
//returns true if the bot is in lava or slime
qboolean BotInLavaOrSlime(bot_state_t *bs);
//returns true if the entity is dead
qboolean EntityIsDead(aas_entityinfo_t *entinfo);
//returns true if the entity is invisible
qboolean EntityIsInvisible(aas_entityinfo_t *entinfo);
//returns true if the entity is shooting
qboolean EntityIsShooting(aas_entityinfo_t *entinfo);
//returns true if this entity has the kamikaze
qboolean EntityHasKamikaze(aas_entityinfo_t *entinfo);
// set a user info key/value pair
void BotSetUserInfo(bot_state_t *bs, char *key, char *value);
// set the team status (offense, defense etc.)
void BotSetTeamStatus(bot_state_t *bs);
//returns the name of the client
char *ClientName(int client, char *name, int size);
//returns an simplyfied client name
char *EasyClientName(int client, char *name, int size);
//returns the skin used by the client
char *ClientSkin(int client, char *skin, int size);
// returns the appropriate synonym context for the current game type and situation
int BotSynonymContext(bot_state_t *bs);
// set last ordered task
int BotSetLastOrderedTask(bot_state_t *bs);
// selection of goals for teamplay
void BotTeamGoals(bot_state_t *bs, int retreat);
//returns the aggression of the bot in the range [0, 100]
float BotAggression(bot_state_t *bs);
//returns how bad the bot feels
float BotFeelingBad(bot_state_t *bs);
//returns true if the bot wants to retreat
int BotWantsToRetreat(bot_state_t *bs);
//returns true if the bot wants to chase
int BotWantsToChase(bot_state_t *bs);
//returns true if the bot wants to help
int BotWantsToHelp(bot_state_t *bs);
//returns true if the bot can and wants to rocketjump
int BotCanAndWantsToRocketJump(bot_state_t *bs);
// returns true if the bot has a persistant powerup and a weapon
int BotHasPersistantPowerupAndWeapon(bot_state_t *bs);
//returns true if the bot wants to and goes camping
int BotWantsToCamp(bot_state_t *bs);
//the bot will perform attack movements
bot_moveresult_t BotAttackMove(bot_state_t *bs, int tfl);
//returns true if the bot and the entity are in the same team
int BotSameTeam(bot_state_t *bs, int entnum);
//returns true if teamplay is on
int TeamPlayIsOn(void);
// returns the client number of the team mate flag carrier (-1 if none)
int BotTeamFlagCarrier(bot_state_t *bs);
//returns visible team mate flag carrier if available
int BotTeamFlagCarrierVisible(bot_state_t *bs);
//returns visible enemy flag carrier if available
int BotEnemyFlagCarrierVisible(bot_state_t *bs);
//get the number of visible teammates and enemies
void BotVisibleTeamMatesAndEnemies(bot_state_t *bs, int *teammates, int *enemies, float range);
//returns true if within the field of vision for the given angles
qboolean InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles);
//returns true and sets the .enemy field when an enemy is found
int BotFindEnemy(bot_state_t *bs, int curenemy);
//returns a roam goal
void BotRoamGoal(bot_state_t *bs, vec3_t goal);
//returns entity visibility in the range [0, 1]
float BotEntityVisible(int viewer, vec3_t eye, vec3_t viewangles, float fov, int ent);
//the bot will aim at the current enemy
void BotAimAtEnemy(bot_state_t *bs);
//check if the bot should attack
void BotCheckAttack(bot_state_t *bs);
//AI when the bot is blocked
void BotAIBlocked(bot_state_t *bs, bot_moveresult_t *moveresult, int activate);
//AI to predict obstacles
int BotAIPredictObstacles(bot_state_t *bs, bot_goal_t *goal);
//enable or disable the areas the blocking entity is in
void BotEnableActivateGoalAreas(bot_activategoal_t *activategoal, int enable);
//pop an activate goal from the stack
int BotPopFromActivateGoalStack(bot_state_t *bs);
//clear the activate goal stack
void BotClearActivateGoalStack(bot_state_t *bs);
//returns the team the bot is in
int BotTeam(bot_state_t *bs);
//retuns the opposite team of the bot
int BotOppositeTeam(bot_state_t *bs);
//returns the flag the bot is carrying (CTFFLAG_?)
int BotCarriesObjective(bot_state_t *bs);
//remember the last ordered task
void BotRememberLastOrderedTask(bot_state_t *bs);
//set ctf goals (defend base, get enemy flag) during seek
void BotCTFSeekGoals(bot_state_t *bs);
//set ctf goals (defend base, get enemy flag) during retreat
void BotCTFRetreatGoals(bot_state_t *bs);
//
void Bot1FCTFSeekGoals(bot_state_t *bs);
void Bot1FCTFRetreatGoals(bot_state_t *bs);
void BotObeliskSeekGoals(bot_state_t *bs);
void BotObeliskRetreatGoals(bot_state_t *bs);
void BotGoHarvest(bot_state_t *bs);
void BotHarvesterSeekGoals(bot_state_t *bs);
void BotHarvesterRetreatGoals(bot_state_t *bs);
int BotTeamCubeCarrierVisible(bot_state_t *bs);
int BotEnemyCubeCarrierVisible(bot_state_t *bs);
//get a random alternate route goal towards the given base
int BotGetAlternateRouteGoal(bot_state_t *bs, int base);
//returns either the alternate route goal or the given goal
bot_goal_t *BotAlternateRoute(bot_state_t *bs, bot_goal_t *goal);
//create a new waypoint
bot_waypoint_t *BotCreateWayPoint(char *name, vec3_t origin, int areanum);
//find a waypoint with the given name
bot_waypoint_t *BotFindWayPoint(bot_waypoint_t *waypoints, char *name);
//strstr but case insensitive
char *stristr(char *str, char *charset);
//returns the number of the client with the given name
int ClientFromName(char *name);
int ClientOnSameTeamFromName(bot_state_t *bs, char *name);
//
int BotPointAreaNum(vec3_t origin);
//
void BotMapScripts(bot_state_t *bs);

//ctf flags
#define CTF_FLAG_NONE		0
#define CTF_FLAG_RED		1
#define CTF_FLAG_BLUE		2
//CTF skins
#define CTF_SKIN_REDTEAM	"red"
#define CTF_SKIN_BLUETEAM	"blue"

extern int gametype;		//game type
extern int maxclients;		//maximum number of clients

extern vmCvar_t bot_grapple;
extern vmCvar_t bot_rocketjump;
extern vmCvar_t bot_fastchat;
extern vmCvar_t bot_nochat;
extern vmCvar_t bot_testrchat;
extern vmCvar_t bot_challenge;

extern bot_goal_t ctf_redflag;
extern bot_goal_t ctf_blueflag;
extern bot_goal_t ctf_neutralflag;
extern bot_goal_t redobelisk;
extern bot_goal_t blueobelisk;
extern bot_goal_t neutralobelisk;

/*****************************************************************************
 * name:		ai_chat.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

// time the selected chat takes to type in
#define BOT_CHAT_TIME 2.0		//(float) trap_BotChatLength(bs->cs) * 30 / cpm;

//
int BotChat_EnterGame(bot_state_t *bs);
//
int BotChat_ExitGame(bot_state_t *bs);
//
int BotChat_StartLevel(bot_state_t *bs);
//
int BotChat_EndLevel(bot_state_t *bs);
//
int BotChat_HitTalking(bot_state_t *bs);
//
int BotChat_HitNoDeath(bot_state_t *bs);
//
int BotChat_HitNoKill(bot_state_t *bs);
//
int BotChat_Death(bot_state_t *bs);
//
int BotChat_Kill(bot_state_t *bs);
//
int BotChat_EnemySuicide(bot_state_t *bs);
//
int BotChat_Random(bot_state_t *bs);
// returns true if the bot can chat at the current position
int BotValidChatPosition(bot_state_t *bs);
// test the initial bot chats
void BotChatTest(bot_state_t *bs);

/*****************************************************************************
 * name:		ai_cmd.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

extern int notleader[MAX_CLIENTS];

int BotMatchMessage(bot_state_t *bs, char *message);
void BotPrintTeamGoal(bot_state_t *bs);

/*****************************************************************************
 * name:		ai_dmnet.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

#define MAX_NODESWITCHES	50

void AIEnter_Intermission(bot_state_t *bs, char *s);
void AIEnter_Observer(bot_state_t *bs, char *s);
void AIEnter_Respawn(bot_state_t *bs, char *s);
void AIEnter_Stand(bot_state_t *bs, char *s);
void AIEnter_Seek_ActivateEntity(bot_state_t *bs, char *s);
void AIEnter_Seek_NBG(bot_state_t *bs, char *s);
void AIEnter_Seek_LTG(bot_state_t *bs, char *s);
void AIEnter_Seek_Camp(bot_state_t *bs, char *s);
void AIEnter_Battle_Fight(bot_state_t *bs, char *s);
void AIEnter_Battle_Chase(bot_state_t *bs, char *s);
void AIEnter_Battle_Retreat(bot_state_t *bs, char *s);
void AIEnter_Battle_NBG(bot_state_t *bs, char *s);
int AINode_Intermission(bot_state_t *bs);
int AINode_Observer(bot_state_t *bs);
int AINode_Respawn(bot_state_t *bs);
int AINode_Stand(bot_state_t *bs);
int AINode_Seek_ActivateEntity(bot_state_t *bs);
int AINode_Seek_NBG(bot_state_t *bs);
int AINode_Seek_LTG(bot_state_t *bs);
int AINode_Battle_Fight(bot_state_t *bs);
int AINode_Battle_Chase(bot_state_t *bs);
int AINode_Battle_Retreat(bot_state_t *bs);
int AINode_Battle_NBG(bot_state_t *bs);

void BotResetNodeSwitches(void);
void BotDumpNodeSwitches(bot_state_t *bs);

/*****************************************************************************
 * name:		ai_team.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_chat.c $
 *
 *****************************************************************************/

void BotTeamAI(bot_state_t *bs);
int BotGetTeamMateTaskPreference(bot_state_t *bs, int teammate);
void BotSetTeamMateTaskPreference(bot_state_t *bs, int teammate, int preference);
void BotVoiceChat(bot_state_t *bs, int toclient, char *voicechat);
void BotVoiceChatOnly(bot_state_t *bs, int toclient, char *voicechat);

/*****************************************************************************
 * name:		ai_vcmd.h
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /source/code/botai/ai_vcmd.c $
 *
 *****************************************************************************/

int BotVoiceChatCommand(bot_state_t *bs, int mode, char *voicechat);
void BotVoiceChat_Defend(bot_state_t *bs, int client, int mode);
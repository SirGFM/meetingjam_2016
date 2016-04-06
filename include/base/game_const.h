/**
 * @file include/base/game_const.h
 *
 * Defines all constants
 */
#ifndef __GAME_CONST_H__
#define __GAME_CONST_H__

/* TODO Set attributes for the game */

/* == Game's basic configurations =========================================== */

/** Game's "organization" */
#define ORG         "br.com.gamux"
/** Game's title */
#define TITLE       "Cow Escape"
/** Config file name */
#define CONF        "config.sav"
/** Virtual window's width */
#define V_WIDTH     64
/** Virtual window's height */
#define V_HEIGHT    64
/** Whether the user can resize the game window */
#define CAN_RESIZE  1
/** Initial background color in 0xAARRGGBB format */
#define BG_COLOR    0xFF363F0D
/** Spriteset used by the FPS counter. It's ignored, if NULL */
#define FPS_SSET    pGfx->pSset8x8
/** First ASCII tile in FPS_SSET */
#define FPS_INIT    0
/** FPS counter position. It must be enabled through a 'gfm_showFPSCounter',
 * which is called by default on debug mode */
#define FPS_X       0
#define FPS_Y       0

/* == Config file IDs ======================================================= */

#define CONF_ID_INIT        "init"
#define CONF_ID_FLAGS       "flags"
#define CONF_ID_RES         "res"
#define CONF_ID_WIDTH       "width"
#define CONF_ID_HEIGHT      "height"
#define CONF_ID_FPS         "fps"
#define CONF_ID_AUDIOQ      "audio"
#define CONF_ID_LAST_FLAGS  "lFlags"
#define CONF_ID_LAST_RES    "lRes"
#define CONF_ID_LAST_WIDTH  "lWidth"
#define CONF_ID_LAST_HEIGHT "lHeight"
#define CONF_ID_LAST_FPS    "lFps"
#define CONF_ID_LAST_AUDIOQ "lAudio"

/* == Default configuration ================================================= */

#define CONF_FLAGS  (CFG_VSYNC)
#define CONF_RES    0
#define CONF_WIDTH  640
#define CONF_HEIGHT 480
#define CONF_FPS    60
#define CONF_AUDIOQ gfmAudio_defQuality

/* == ... =================================================================== */

/** Texture's transparent color */
#define KEY_COLOR       0xFF00FF
/** Maximum number of times the quadtree will subdivide */
#define QT_MAX_DEPTH    6
/** Number of nodes required for the quadtree to subdivide */
#define QT_MAX_NODES    10

#define TEXATLAS "atlas.bmp"
#define KEY_COLOR 0xFF00FF

#define COW_X 4
#define COW_Y 4
#define COW_W 12
#define COW_H 9
#define COW_OX -2
#define COW_OY -5
#define GRAV 500
/*#define MAP_W 160*/
/*#define MAP_H 64*/
#define COW_VX 24
#define COW_JUMPVY -150
#define COW_EAT_FRAME 27
#define COW_EAT_W 8
#define COW_EAT_H 8
#define COW_EAT_OX 2
#define COW_EAT_OY 0

#define FLOOR_X 0
#define FLOOR_Y 48
#define FLOOR_W 64
#define FLOOR_H 16
#define FLOOR_OX 0
#define FLOOR_OY 0
#define FLOOR_FRAME 24

#define PART_CACHE 256
#define PART_TTL 5000

#define CLOUD_W 32
#define CLOUD_H 16
#define CLOUD_FRAME 40
#define CLOUD_NEXTTIME 750
#define CLOUD_NEXTTIMEMOD 125
#define CLOUD_NEXTTIMEMUL 8
#define CLOUD_VX 24
#define CLOUD_VXMOD 24

#define BUL_DX 4
#define BUL_DY -1
#define BUL_W 4
#define BUL_H 4
#define BUL_OX -2
#define BUL_OY -2
#define BUL_FRAME 324
#define BUL_VX 32
#define BUL_COOLDOWN 66

#define CAM_DEAD_X0 24
#define CAM_DEAD_X1 16

#define UI_GRASS_BAR_Y 53
#define UI_GRASS_BAR_FRAME0 64
#define UI_HEART_X0 40
#define UI_HEART_Y 53
#define UI_HEART_FRAME0 288
#define UI_NUM_HEARTS 3

#define GRASS_FRAME0 192
#define GRASS_NUMFRAMES 3
#define GRASS_MAX 4
#define LASER_TIME 4000

#define STAR_FRAME0 226
#define STAR_NUM_FRAME 5
#define STAR_NUM_MOD 5
#define STAR_NUM_MUL 2
#define STAR_COOLDOWN 1000
#define STAR_COOLDOWN_MOD 50
#define STAR_COOLDOWN_MUL 10

#define MOON_Y 3
#define MOON_X0 4
#define MOON_X1 52
#define MOON_FRAME 356

#endif /* __GAME_CONST_H__ */


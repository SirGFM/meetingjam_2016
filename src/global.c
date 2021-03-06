/**
 * @file src/global.c
 *
 * Declare all global variables
 */
#include <base/game_const.h>
#include <base/game_ctx.h>
#include <base/global.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmQuadtree.h>

#include <jam/alien.h>
#include <jam/particle.h>

/** Store data related to game */
gameCtx *pGame = 0;

/** Store all handles to texture and spritesets' pointers */
gfxCtx *pGfx = 0;

/** Store all handles to songs and sound effects */
audioCtx *pAudio = 0;

/** Store all actions do-able ingame */
buttonCtx *pButton = 0;

/** Store all data modifiably on the option menu, as well as anything that may
 * be saved on the config file */
configCtx *pConfig = 0;

/** Store game-related variables that should be globally accessible */
globalCtx *pGlobal = 0;

/**
 * Initialize all pointers
 *
 * @param  [ in]pMem A buffer of at least SIZEOF_GAME_MEM bytes
 */
void global_init(void *pMem) {
    /* Retrieve all pointers from the memory */
    pGame = (gameCtx*)OFFSET_MEM(pMem, GAME_OFFSET);
    pGfx = (gfxCtx*)OFFSET_MEM(pMem, GFX_OFFSET);
    pAudio = (audioCtx*)OFFSET_MEM(pMem, AUDIO_OFFSET);
    pButton = (buttonCtx*)OFFSET_MEM(pMem, BUTTON_OFFSET);
    pConfig = (configCtx*)OFFSET_MEM(pMem, CONFIG_OFFSET);
    pGlobal = (globalCtx*)OFFSET_MEM(pMem, GLOBAL_OFFSET);

    /* Set any pointers within those structs that were already alloc'ed */
    pConfig->pLast = (configCtx*)OFFSET_MEM(pMem, LASTCONFIG_OFFSET);
}

/**
 * Initialize all variables in pGlobal
 *
 * @return GFraMe return value
 */
gfmRV global_initUserVar() {
    /** GFraMe return value */
    gfmRV rv;

    /* Initialize the quadtree, so it can be later used for collision */
    rv = gfmQuadtree_getNew(&(pGlobal->pQt));
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmObject_getNew(&(pGlobal->pEatHitbox));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getNew(&(pGlobal->pFloor));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getNew(&(pGlobal->pCow));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_getNew(&(pGlobal->pParticles));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_getNew(&(pGlobal->pBullets));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_getNew(&(pGlobal->pGrass));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmText_getNew(&(pGlobal->pText));
    ASSERT(rv == GFMRV_OK, rv);

    rv = particle_initGroup(pGlobal->pParticles, T_CLOUD, 4/*w*/, 4/*h*/,
            PART_TTL);
    ASSERT(rv == GFMRV_OK, rv);
    rv = particle_initGroup(pGlobal->pBullets, T_CLOUD, 4/*w*/, 4/*h*/,
            PART_TTL);
    ASSERT(rv == GFMRV_OK, rv);
    rv = particle_initGroup(pGlobal->pGrass, T_GRASS, 4/*w*/, 4/*h*/,
            -1/*ttl*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Release all variables in pGlobal
 */
void global_freeUserVar() {
    if (pGlobal->pQt) {
        gfmQuadtree_free(&(pGlobal->pQt));
    }
    if (pGlobal->pEatHitbox) {
        gfmObject_free(&(pGlobal->pEatHitbox));
    }
    if (pGlobal->pFloor) {
        gfmSprite_free(&(pGlobal->pFloor));
    }
    if (pGlobal->pCow) {
        gfmSprite_free(&(pGlobal->pCow));
    }
    if (pGlobal->pGrass) {
        gfmGroup_free(&(pGlobal->pGrass));
    }
    if (pGlobal->pBullets) {
        gfmGroup_free(&(pGlobal->pBullets));
    }
    if (pGlobal->pParticles) {
        gfmGroup_free(&(pGlobal->pParticles));
    }
    if (pGlobal->pText) {
        gfmText_free(&(pGlobal->pText));
    }
    gfmGenArr_clean(pGlobal->pAliens, alien_free);
}


/**
 * @file src/assets.c
 *
 * Handles loading assets and creating the required spritesets
 */
#include <base/game_ctx.h>
#include <base/game_const.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gframe.h>

/**
 * Load all assets
 *
 * @return GFraMe return value
 */
gfmRV assets_load() {
    /** Return value */
    gfmRV rv;

    /* Macros for loading stuff... */
#define GEN_SPRITESET(W, H, TEX) \
    rv = gfm_createSpritesetCached(&(pGfx->pSset##W##x##H), pGame->pCtx, TEX, \
            W, H); \
    ASSERT(rv == GFMRV_OK, rv);
#define LOAD_SFX(var, name) \
    rv = gfm_loadAudio(&(pAudio->var), pGame->pCtx, name, sizeof(name) - 1); \
    ASSERT(rv == GFMRV_OK, rv)

    rv = gfm_loadTextureStatic(&(pGfx->texHandle), pGame->pCtx, TEXATLAS,
            KEY_COLOR);
    ASSERT(rv == GFMRV_OK, rv);
    GEN_SPRITESET(8, 8, pGfx->texHandle);
    GEN_SPRITESET(8, 16, pGfx->texHandle);
    GEN_SPRITESET(16, 16, pGfx->texHandle);
    GEN_SPRITESET(32, 8, pGfx->texHandle);
    GEN_SPRITESET(32, 16, pGfx->texHandle);
    GEN_SPRITESET(64, 16, pGfx->texHandle);

    LOAD_SFX(song, "song/song.mml");
    LOAD_SFX(jump, "sfx/Jump.wav");
    LOAD_SFX(fall, "sfx/Fall.wav");
    LOAD_SFX(cowHit, "sfx/CowHit.wav");
    LOAD_SFX(alienHit, "sfx/AlienHit.wav");
    LOAD_SFX(menu, "sfx/Menu.wav");
    LOAD_SFX(enterGame, "sfx/EnterGame.wav");
    LOAD_SFX(cowStep, "sfx/CowStep.wav");
    LOAD_SFX(alienStep, "sfx/AlienStep.wav");
    LOAD_SFX(eat, "sfx/Eat.wav");
    LOAD_SFX(laser, "sfx/Laser.wav");

    rv = GFMRV_OK;
__ret:
    return rv;
}


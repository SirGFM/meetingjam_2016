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

    rv = GFMRV_OK;
__ret:
    return rv;
}


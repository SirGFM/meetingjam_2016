/**
 * @file src/alien.c
 */
#include <base/game_ctx.h>
#include <base/game_const.h>
#include <base/collision.h>

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmSprite.h>

#include <jam/alien.h>
#include <jam/type.h>

#include <stdlib.h>
#include <string.h>

struct stAlien {
    gfmSprite *pSelf;
    gfmObject *pView;
};

gfmRV alien_getNew(alien **ppCtx) {
    alien *pCtx;
    gfmRV rv;

    pCtx = (alien*)malloc(sizeof(alien));
    ASSERT(pCtx, GFMRV_ALLOC_FAILED);
    memset(pCtx, 0x0, sizeof(alien));

    rv = gfmSprite_getNew(&(pCtx->pSelf));
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_getNew(&(pCtx->pView));
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

void alien_free(alien **ppCtx) {
    if (!ppCtx || !*ppCtx) {
        return;
    }

    gfmSprite_free(&((*ppCtx)->pSelf));
    gfmObject_free(&((*ppCtx)->pView));
    free(*ppCtx);
    *ppCtx = 0;
}

gfmRV alien_init(alien *pAlien, gfmParser *pParser);
gfmRV alien_update(alien *pAlien);
gfmRV alien_postUpdate(alien *pAlien);
gfmRV alien_draw(alien *pAlien);


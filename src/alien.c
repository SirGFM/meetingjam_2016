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

enum alienAnim {
    ALIEN_STAND,
    ALIEN_RUN,
    ALIEN_HIT
};
int pAlienAnimData[] = {
            /*len|fps|loop|data */
/*ALIEN_STAND*/16, 12,  1 ,64,64,64,64,65,64,66,64,65,64,66,64,64,64,67,68,
/*ALIEN_RUN  */ 4, 4 ,  1 ,69,70,69,71,
/*ALIEN_HIT  */ 2, 4 ,  1 ,72,73
};
int alienAnimDataLen = sizeof(pAlienAnimData) / sizeof(int);

struct stAlien {
    gfmSprite *pSelf;
    gfmObject *pView;
    int anim;
    int timer;
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

gfmRV alien_init(gfmParser *pParser) {
    alien *pAlien;
    gfmRV rv;
    int x, y;

    gfmGenArr_getNextRef(alien, pGlobal->pAliens, 1, pAlien, alien_getNew);

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    y -= 16;
    rv = gfmSprite_init(pAlien->pSelf, x, y, 4/*w*/, 12/*h*/, pGfx->pSset8x16,
            -2/*ox*/, -2/*oy*/, pAlien, T_ALIEN);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimations(pAlien->pSelf, pAlienAnimData,
            alienAnimDataLen);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pAlien->pSelf, ALIEN_STAND);
    ASSERT(rv == GFMRV_OK, rv);
    pAlien->anim = ALIEN_STAND;

    rv = gfmObject_init(pAlien->pView, x, y, 24, 10, pAlien, T_ALIENV);
    ASSERT(rv == GFMRV_OK, rv);

    gfmGenArr_push(pGlobal->pAliens);
    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _alien_update(alien *pAlien) {
    gfmRV rv;
    int flipped, x, y;

    rv = gfmSprite_update(pAlien->pSelf, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getPosition(&x, &y, pAlien->pSelf);
    ASSERT(rv == GFMRV_OK, rv);

    /* Update view */
    rv = gfmSprite_getDirection(&flipped, pAlien->pSelf);
    ASSERT(rv == GFMRV_OK, rv);
    if (flipped) {
        rv = gfmObject_setPosition(pAlien->pView, x + 4, y - 2);
    }
    else {
        rv = gfmObject_setPosition(pAlien->pView, x - 24, y - 2);
    }
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmObject_update(pAlien->pView, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmQuadtree_collideSprite(pGlobal->pQt, pAlien->pSelf);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }
    rv = gfmQuadtree_collideObject(pGlobal->pQt, pAlien->pView);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}
gfmRV alien_update() {
    gfmRV rv;

    gfmGenArr_callAllRV(pGlobal->pAliens, _alien_update);

    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _alien_postUpdate(alien *pAlien) {
    gfmRV rv;

    /* TODO Update AI */
    /* TODO Update animation */

    rv = GFMRV_OK;
__ret:
    return rv;
}
gfmRV alien_postUpdate() {
    gfmRV rv;

    gfmGenArr_callAllRV(pGlobal->pAliens, _alien_postUpdate);

    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _alien_draw(alien *pAlien) {
    return gfmSprite_draw(pAlien->pSelf, pGame->pCtx);
}
gfmRV alien_draw() {
    gfmRV rv;

    gfmGenArr_callAllRV(pGlobal->pAliens, _alien_draw);

    rv = GFMRV_OK;
__ret:
    return rv;
}


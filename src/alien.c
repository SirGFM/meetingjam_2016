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
/*ALIEN_RUN  */ 4, 8 ,  1 ,69,70,71,70,
/*ALIEN_HIT  */ 2, 8 ,  1 ,72,73
};
int alienAnimDataLen = sizeof(pAlienAnimData) / sizeof(int);

struct stAlien {
    gfmSprite *pSelf;
    gfmObject *pView;
    int anim;
    int timer;
    /** Relative position; 0 = away, 1 = right, -1 = left */
    int cowRelativePos;
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

    *ppCtx = pCtx;
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
    y -= 12;
    rv = gfmSprite_init(pAlien->pSelf, x, y, 2/*w*/, 10/*h*/, pGfx->pSset8x16,
            -3/*ox*/, -4/*oy*/, pAlien, T_ALIEN);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimations(pAlien->pSelf, pAlienAnimData,
            alienAnimDataLen);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pAlien->pSelf, ALIEN_STAND);
    ASSERT(rv == GFMRV_OK, rv);
    pAlien->anim = ALIEN_STAND;

    rv = gfmObject_init(pAlien->pView, x, y, 24, 10, pAlien, T_ALIENV);
    ASSERT(rv == GFMRV_OK, rv);

    pAlien->anim = -1;
    pAlien->timer = 0;
    pAlien->cowRelativePos = 0;

    gfmGenArr_push(pGlobal->pAliens);
    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV _alien_update(alien *pAlien) {
    gfmRV rv;
    int flipped, x, y;

    if (pAlien->timer > 0) {
        pAlien->timer -= pGame->elapsed;
    }

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

    /* Update AI */
    if (pAlien->anim != ALIEN_HIT && pAlien->timer <= 0) {
        int vx;

        if (pAlien->cowRelativePos == 1) {
            rv = gfmSprite_setDirection(pAlien->pSelf, 1/*flipped*/);
            ASSERT(rv == GFMRV_OK, rv);

            vx = ALIEN_VX;
        }
        else if (pAlien->cowRelativePos == -1) {
            rv = gfmSprite_setDirection(pAlien->pSelf, 0/*flipped*/);
            ASSERT(rv == GFMRV_OK, rv);

            vx = -ALIEN_VX;
        }
        else {
            vx = 0;
            switch(rand() % 10) {
                case 0:
                    vx = ALIEN_VX;
                case 1:
                case 2:
                case 3:
                case 4: {
                    rv = gfmSprite_setDirection(pAlien->pSelf, 1/*flipped*/);
                    ASSERT(rv == GFMRV_OK, rv);
                } break;
                case 5:
                    vx = -ALIEN_VX;
                case 6:
                case 7:
                case 8:
                case 9: {
                    rv = gfmSprite_setDirection(pAlien->pSelf, 0/*flipped*/);
                    ASSERT(rv == GFMRV_OK, rv);
                } break;
            }
        }
        pAlien->cowRelativePos = 0;

        rv = gfmSprite_setHorizontalVelocity(pAlien->pSelf, vx);
        ASSERT(rv == GFMRV_OK, rv);

        if (vx != 0 && pAlien->anim != ALIEN_RUN) {
            pAlien->anim = ALIEN_RUN;
            rv = gfmSprite_playAnimation(pAlien->pSelf, ALIEN_RUN);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if(vx == 0 && pAlien->anim != ALIEN_STAND) {
            pAlien->anim = ALIEN_STAND;
            rv = gfmSprite_playAnimation(pAlien->pSelf, ALIEN_STAND);
            ASSERT(rv == GFMRV_OK, rv);
        }

        pAlien->timer += ALIEN_COOLDOWN;
    }
    else {
        pAlien->timer -= pGame->elapsed;
    }

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

void alien_pursueDir(alien *pAlien, int goLeft) {
    if (pAlien->cowRelativePos != 0) {
        return;
    }

    if (goLeft) {
        pAlien->cowRelativePos = -1;
    }
    else {
        pAlien->cowRelativePos = 1;
    }
}

gfmRV alien_hit(alien *pAlien) {
    gfmRV rv;

    if (pAlien->anim == ALIEN_HIT) {
        return GFMRV_OK;
    }

    pAlien->anim = ALIEN_HIT;
    rv = gfmSprite_playAnimation(pAlien->pSelf, ALIEN_HIT);
    ASSERT(rv == GFMRV_OK, rv);

    pGlobal->alienCount.cur--;

    rv = gfmSprite_setVerticalAcceleration(pAlien->pSelf, GRAV);

    rv = GFMRV_OK;
__ret:
    return rv;
}


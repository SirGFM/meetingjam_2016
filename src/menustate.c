/**
 * @file src/menustate.c
 */
#include <base/game_ctx.h>
#include <base/game_const.h>
#include <base/collision.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <jam/type.h>

enum cowAnim {
    COW_STAND_0,
    COW_STAND_1,
    COW_STAND_2,
    COW_STAND_3,
    COW_RUN,
    COW_JUMP,
    COW_FALL,
    COW_EAT,
    COW_HIT,
};
int pCowAnimData[] = {
           /*len|fps|loop|data */
/*COW_STAND_0*/1, 0 ,  0 ,16,
/*COW_STAND_1*/4, 12,  1 ,16,18,19,18,
/*COW_STAND_2*/4, 12,  1 ,20,18,19,18,
/*COW_STAND_3*/4, 12,  1 ,21,18,19,18,
/*COW_RUN    */2, 4 ,  1 ,22,24,
/*COW_JUMP   */1, 0 ,  0 ,22,
/*COW_FALL   */1, 0 ,  0 ,24,
/*COW_EAT    */3, 1 ,  0 ,25,26,25,
/*COW_HIT    */1, 0 ,  0 ,27
};
int cowAnimDataLen = sizeof(pCowAnimData) / sizeof(int);

gfmRV menu_init() {
    gfmRV rv;

    /* COW */
    rv = gfmSprite_init(pGlobal->pCow, COW_X, COW_Y, COW_W, COW_H,
            pGfx->pSset16x16, COW_OX, COW_OY, 0, T_COW);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimations(pGlobal->pCow, pCowAnimData, cowAnimDataLen);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pGlobal->pCow, COW_STAND_1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setVerticalAcceleration(pGlobal->pCow, GRAV);
    ASSERT(rv == GFMRV_OK, rv);

    /*FLOOR*/
    rv = gfmSprite_init(pGlobal->pFloor, FLOOR_X, FLOOR_Y, MAP_W, MAP_H,
            pGfx->pSset64x16, FLOOR_OX, FLOOR_OY, 0, T_FLOOR);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFixed(pGlobal->pFloor);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pGlobal->pFloor, FLOOR_FRAME);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_clean() {
    return GFMRV_OK;
}

gfmRV menu_update() {
    gfmRV rv;

    rv = gfmSprite_update(pGlobal->pCow, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmQuadtree_initRoot(pGlobal->pQt, -8, -8, MAP_W, MAP_H, QT_MAX_DEPTH,
            QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateSprite(pGlobal->pQt, pGlobal->pFloor);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_collideSprite(pGlobal->pQt, pGlobal->pCow);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_draw() {
    gfmRV rv;

    rv = gfmSprite_draw(pGlobal->pFloor, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_draw(pGlobal->pCow, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}



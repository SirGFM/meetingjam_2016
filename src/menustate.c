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
    COW_STAND_0 = 0,
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
/*COW_STAND_0*/ 1, 0 ,  0 ,16,
/*COW_STAND_1*/ 4, 12,  1 ,16,18,19,18,
/*COW_STAND_2*/12, 12,  1 ,16,18,19,18,16,18,19,18,20,18,19,18,
/*COW_STAND_3*/12, 12,  1 ,16,18,19,18,16,18,19,18,21,18,19,18,
/*COW_RUN    */ 2, 4 ,  1 ,22,24,
/*COW_JUMP   */ 1, 0 ,  0 ,29,
/*COW_FALL   */ 1, 0 ,  0 ,30,
/*COW_EAT    */ 3, 2 ,  0 ,26,27,26,
/*COW_HIT    */ 1, 0 ,  0 ,28
};
int cowAnimDataLen = sizeof(pCowAnimData) / sizeof(int);

gfmRV menu_init() {
    gfmRV rv;

    rv = gfmCamera_setWorldDimensions(pGame->pCam, MAP_W, MAP_H);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pGame->pCam, CAM_DEAD_X0, 0/*y*/, CAM_DEAD_X1,
            MAP_H);
    ASSERT(rv == GFMRV_OK, rv);

    /* COW */
    rv = gfmObject_init(pGlobal->pEatHitbox, 0/*x*/, 0/*y*/, COW_EAT_W,
            COW_EAT_H, 0, T_EAT);
    ASSERT(rv == GFMRV_OK, rv);
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

    /*PARTICLES*/
    rv = gfmGroup_setDefType(pGlobal->pParticles, T_CLOUD);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGlobal->pParticles, pGfx->pSset8x8);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGlobal->pParticles, 4/*w*/, 4/*h*/, 0/*ox*/,
            0/*oy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefVelocity(pGlobal->pParticles, 0/*vx*/, 0/*vy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAcceleration(pGlobal->pParticles, 0/*ax*/, 0/*ay*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGlobal->pParticles, 0/*doDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGlobal->pParticles, PART_TTL);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pGlobal->pParticles, PART_CACHE, PART_CACHE);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDrawOrder(pGlobal->pParticles, gfmDrawOrder_newestFirst);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setCollisionQuality(pGlobal->pParticles,
            gfmCollisionQuality_visibleOnly);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_clean() {
    return GFMRV_OK;
}

gfmRV menu_update() {
    double vx, vy;
    gfmRV rv;
    gfmCollision dir;
    int cx, cy;

    /* == UPDATE ================== */

    /*PARTICLES*/
    if (pGlobal->cloudTime <= 0) {
        gfmSprite *pCloud;
        int y;

        pCloud = 0;
        rv = gfmGroup_recycle(&pCloud, pGlobal->pParticles);
        ASSERT(rv == GFMRV_OK, rv);

        y = 13 + (rand() % 8);
        rv = gfmSprite_init(pCloud, pGame->camX + V_WIDTH, y, CLOUD_W, CLOUD_H,
            pGfx->pSset32x16, 0/*ox*/, 0/*oy*/, 0, T_CLOUD);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setFrame(pCloud, CLOUD_FRAME);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setVelocity(pCloud, -(CLOUD_VX + (rand() % CLOUD_VXMOD)),
                0/*vy*/);
        ASSERT(rv == GFMRV_OK, rv);

        pGlobal->cloudTime += CLOUD_NEXTTIME;
        pGlobal->cloudTime += (rand() % CLOUD_NEXTTIMEMUL) * CLOUD_NEXTTIMEMOD;
    }
    pGlobal->cloudTime -= pGame->elapsed;

    if (pGlobal->laserTime > 0  && pGlobal->cooldown <= 0 &&
                (pButton->act.state & gfmInput_pressed)) {
        gfmSprite *pBullet;
        int flipped, vx, x, y;

        pBullet = 0;
        rv = gfmGroup_recycle(&pBullet, pGlobal->pParticles);
        ASSERT(rv == GFMRV_OK, rv);

        rv = gfmSprite_getDirection(&flipped, pGlobal->pCow);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_getCenter(&x, &y, pGlobal->pCow);
        ASSERT(rv == GFMRV_OK, rv);
        if (!flipped) {
            x += BUL_DX;
            y += BUL_DY;
            vx = BUL_VX;
        }
        else {
            x -= BUL_DX;
            y -= BUL_DY;
            vx = -BUL_VX;
        }

        rv = gfmSprite_init(pBullet, x, y, BUL_W, BUL_H, pGfx->pSset8x8, BUL_OX,
                BUL_OY, 0, T_BULLET);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setFrame(pBullet, BUL_FRAME);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setVelocity(pBullet, vx, 0/*vy*/);
        ASSERT(rv == GFMRV_OK, rv);

        pGlobal->cooldown += BUL_COOLDOWN;
    }
    if (pGlobal->cooldown > 0) {
        pGlobal->cooldown -= pGame->elapsed;
    }
    rv = gfmGroup_update(pGlobal->pParticles, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /*COW*/
    rv = gfmSprite_update(pGlobal->pCow, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    if (pButton->left.state & gfmInput_pressed) {
        rv = gfmSprite_setHorizontalVelocity(pGlobal->pCow, -COW_VX);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setDirection(pGlobal->pCow, 1/*flipped*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (pButton->right.state & gfmInput_pressed) {
        rv = gfmSprite_setHorizontalVelocity(pGlobal->pCow, COW_VX);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setDirection(pGlobal->pCow, 0/*flipped*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else {
        rv = gfmSprite_setHorizontalVelocity(pGlobal->pCow, 0);
        ASSERT(rv == GFMRV_OK, rv);
    }

    /* == COLLISION =============== */

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
    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pParticles);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    /* Hit box to eat grass */
    if (gfmSprite_didAnimationJustChangeFrame(pGlobal->pCow) == GFMRV_TRUE) {
        int frame;

        rv = gfmSprite_getFrame(&frame, pGlobal->pCow);
        ASSERT(rv == GFMRV_OK, rv);
        if (frame == COW_EAT_FRAME) {
            rv = gfmSprite_getCenter(&cx, &cy, pGlobal->pCow);
            ASSERT(rv == GFMRV_OK, rv);

            rv = gfmObject_setPosition(pGlobal->pEatHitbox, cx + COW_EAT_OX,
                    cy + COW_EAT_OY);
            ASSERT(rv == GFMRV_OK, rv);
            rv = gfmQuadtree_collideObject(pGlobal->pQt, pGlobal->pEatHitbox);
            ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
            if (rv == GFMRV_QUADTREE_OVERLAPED) {
                rv = collision_run();
                ASSERT(rv == GFMRV_OK, rv);
            }
        }
    }

    if (pGlobal->cowAnim == COW_EAT &&
            gfmSprite_didAnimationFinish(pGlobal->pCow) == GFMRV_TRUE) {
        pGlobal->cowAnim = -1;
    }

    /* == POST ========================= */

    /*COW*/
    rv = gfmSprite_getCollision(&dir, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    if ((dir & gfmCollision_down) &&
            (pButton->jump.state & gfmInput_justPressed) ==
            gfmInput_justPressed) {
        rv = gfmSprite_setVerticalVelocity(pGlobal->pCow, COW_JUMPVY);
        ASSERT(rv == GFMRV_OK, rv);
    }
    rv = gfmSprite_getVelocity(&vx, &vy, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    if (vy < 0) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_JUMP);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_JUMP;
    }
    else if (vy > 0) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_FALL);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_FALL;
    }
    /* TODO HIT */
    else if (pGlobal->cowAnim == COW_EAT || !(dir & gfmCollision_down)) {
        /* Filter any other animation */
    }
    else if (vx != 0) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_RUN);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_RUN;
    }
    else if (pGlobal->laserTime <= 0 &&
            (pButton->act.state & gfmInput_justPressed) == gfmInput_justPressed) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_EAT);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_EAT;
    }
    else if (pGlobal->laserTime > 0) {
        /* Force non-animated stand if shooting */
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_STAND_0);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_STAND_0;
    }
    else {
        int anim;

        if (pGlobal->cowAnim != COW_STAND_1 &&
                pGlobal->cowAnim != COW_STAND_2 &&
                pGlobal->cowAnim != COW_STAND_3) {
            anim = COW_STAND_1 + (rand() % 3);
            rv = gfmSprite_playAnimation(pGlobal->pCow, anim);
            ASSERT(rv == GFMRV_OK, rv);

            pGlobal->cowAnim = anim;
        }
    }

    /*CAMERA*/
    rv = gfmSprite_getCenter(&cx, &cy, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_centerAtPoint(pGame->pCam, cx, cy);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_draw() {
    gfmRV rv;
    int frame;

    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset64x16, 0, FLOOR_Y, FLOOR_FRAME, 0);
    ASSERT(rv == GFMRV_OK, rv);

    if (pGlobal->laserTime > 0) {
        rv = gfmSprite_getFrame(&frame, pGlobal->pCow);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setFrame(pGlobal->pCow, frame | 1);
        ASSERT(rv == GFMRV_OK, rv);
    }
    rv = gfmSprite_draw(pGlobal->pCow, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    if (pGlobal->laserTime > 0) {
        rv = gfmSprite_setFrame(pGlobal->pCow, frame | 1);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = gfmGroup_draw(pGlobal->pParticles, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}


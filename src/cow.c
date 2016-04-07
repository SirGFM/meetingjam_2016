/**
 * @file src/cow.c
 */
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmSprite.h>

#include <jam/cow.h>
#include <jam/particle.h>
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
/*COW_EAT    */ 3, 3 ,  0 ,26,27,26,
/*COW_HIT    */13, 8 ,  0 ,28,31,28,31,28,31,28,31,28,31,28,31,28
};
int cowAnimDataLen = sizeof(pCowAnimData) / sizeof(int);

gfmRV cow_init(gfmParser *pParser) {
    gfmRV rv;
    int x, y;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    y -= 16;

    rv = gfmObject_init(pGlobal->pEatHitbox, 0/*x*/, 0/*y*/, COW_EAT_W,
            COW_EAT_H, 0, T_EAT);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pGlobal->pCow, x, y, COW_W, COW_H,
            pGfx->pSset16x16, COW_OX, COW_OY, 0, T_COW);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_addAnimations(pGlobal->pCow, pCowAnimData, cowAnimDataLen);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_playAnimation(pGlobal->pCow, COW_STAND_1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setVerticalAcceleration(pGlobal->pCow, GRAV);
    ASSERT(rv == GFMRV_OK, rv);

    pGlobal->cowHitstun = 0;

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV cow_update() {
    gfmRV rv;
    gfmCollision dir;
    int cx, cy;

    /* Start shooting bullets */
    if (pGlobal->grassCounter >= GRASS_MAX &&
            (pButton->act.state & gfmInput_justPressed) ==
            gfmInput_justPressed) {
        int frame;

        rv = gfmSprite_getFrame(&frame, pGlobal->pCow);
        ASSERT(rv == GFMRV_OK, rv);

        if (frame != 26 && frame != 27) {
            pGlobal->grassCounter = 0;
            pGlobal->laserTime = LASER_TIME;
        }
    }
    if (pGlobal->laserTime > 0) {
        pGlobal->laserTime -= pGame->elapsed;
        if (pGlobal->laserTime > 0) {
            pGlobal->grassCounter = GRASS_MAX * pGlobal->laserTime / LASER_TIME;
            pGlobal->grassCounter++;
        }
        else {
            pGlobal->grassCounter = 0;
        }
    }
    if (pGlobal->cowHitstun > 0) {
        pGlobal->cowHitstun  -= pGame->elapsed;
    }


    /* Shoot and update bullets */
    if (pGlobal->laserTime > 0  && pGlobal->cooldown <= 0 &&
                (pButton->act.state & gfmInput_pressed)) {
        int flipped, vx, x, y;

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
            y += BUL_DY;
            vx = -BUL_VX;
        }

        rv = particle_recycle(pGlobal->pBullets, T_BULLET, x, y, BUL_W, BUL_H,
                BUL_OX, BUL_OY, vx);
        ASSERT(rv == GFMRV_OK, rv);

        pGlobal->cooldown += BUL_COOLDOWN;
    }
    if (pGlobal->cooldown > 0) {
        pGlobal->cooldown -= pGame->elapsed;
    }

    /* Movement */
    rv = gfmSprite_getCollision(&dir, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    /* JUMP */
    if ((dir & gfmCollision_down) &&
            (pButton->jump.state & gfmInput_justPressed) ==
            gfmInput_justPressed) {
        rv = gfmSprite_setVerticalVelocity(pGlobal->pCow, COW_JUMPVY);
        ASSERT(rv == GFMRV_OK, rv);
    }
    /* WALK */
    if (pGlobal->cowAnim == COW_EAT) {
        rv = gfmSprite_setHorizontalVelocity(pGlobal->pCow, 0);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (pButton->left.state & gfmInput_pressed) {
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

    /* Update and collide */
    rv = gfmSprite_update(pGlobal->pCow, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmQuadtree_collideSprite(pGlobal->pQt, pGlobal->pCow);
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
            int flipped;

            rv = gfmSprite_getCenter(&cx, &cy, pGlobal->pCow);
            ASSERT(rv == GFMRV_OK, rv);

            rv = gfmSprite_getDirection(&flipped, pGlobal->pCow);
            ASSERT(rv == GFMRV_OK, rv);
            if (flipped) {
                cx -= 5*COW_EAT_OX;
            }
            else {
                cx += COW_EAT_OX;
            }

            rv = gfmObject_setPosition(pGlobal->pEatHitbox, cx,
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
    if (gfmSprite_didAnimationFinish(pGlobal->pCow) == GFMRV_TRUE) {
        if (pGlobal->cowAnim == COW_EAT || pGlobal->cowAnim == COW_HIT) {
            pGlobal->cowAnim = -1;
        }
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV cow_postUpdate() {
    double vx, vy;
    gfmRV rv;
    gfmCollision dir;

    rv = gfmSprite_getCollision(&dir, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_getVelocity(&vx, &vy, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);

    if (pGlobal->cowHitstun > 0) {
        /* Filter any other animation */
    }
    else if (vy < 0) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_JUMP);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_JUMP;
    }
    else if (vy > 0) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_FALL);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_FALL;
    }
    else if (pGlobal->cowAnim == COW_EAT || !(dir & gfmCollision_down)) {
        /* Filter any other animation */
    }
    else if (pGlobal->grassCounter < GRASS_MAX && pGlobal->laserTime <= 0 &&
            (pButton->act.state & gfmInput_justPressed) ==
            gfmInput_justPressed) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_EAT);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_EAT;
    }
    else if (vx != 0) {
        rv = gfmSprite_playAnimation(pGlobal->pCow, COW_RUN);
        ASSERT(rv == GFMRV_OK, rv);
        pGlobal->cowAnim = COW_RUN;
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

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV cow_draw() {
    gfmRV rv;
    int frame;

    if (pGlobal->laserTime > 0 && pGlobal->cowAnim != COW_HIT) {
        rv = gfmSprite_getFrame(&frame, pGlobal->pCow);
        ASSERT(rv == GFMRV_OK, rv);
        rv = gfmSprite_setFrame(pGlobal->pCow, frame | 1);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = gfmSprite_draw(pGlobal->pCow, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    if (pGlobal->laserTime > 0 && pGlobal->cowAnim != COW_HIT) {
        rv = gfmSprite_setFrame(pGlobal->pCow, frame);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV cow_hit() {
    gfmRV rv;

    if (pGlobal->cowHitstun > 0) {
        return GFMRV_OK;
    }

    pGlobal->hearts--;
    pGlobal->cowHitstun += COW_HITSTUN;
    pGlobal->cowAnim = COW_HIT;
    rv = gfmSprite_playAnimation(pGlobal->pCow, COW_HIT);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}


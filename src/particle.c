/**
 * @file src/particle.c
 */
#include <base/game_const.h>
#include <base/game_ctx.h>
#include <base/collision.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>

#include <jam/type.h>

#include <stdlib.h>

gfmRV particle_initGroup(gfmGroup *pGroup, mjType type, int w, int h, int ttl) {
    gfmRV rv;

    rv = gfmGroup_setDefType(pGroup, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGroup, pGfx->pSset8x8);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGroup, w, h, 0/*ox*/, 0/*oy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefVelocity(pGroup, 0/*vx*/, 0/*vy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAcceleration(pGroup, 0/*ax*/, 0/*ay*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGroup, 0/*doDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGroup, ttl);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pGroup, PART_CACHE, PART_CACHE);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDrawOrder(pGroup, gfmDrawOrder_oldestFirst);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setCollisionQuality(pGroup, gfmCollisionQuality_visibleOnly);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV particle_recycle(gfmGroup *pGroup, mjType type, int x, int y, int w,
        int h, int ox, int oy, int vx) {
    gfmSprite *pSpr;
    gfmSpriteset *pSset;
    gfmRV rv;
    int frame;

    if (type == T_CLOUD) {
        pSset = pGfx->pSset32x16;
    }
    else {
        pSset = pGfx->pSset8x8;
    }

    if (type == T_CLOUD) {
        frame = CLOUD_FRAME;
    }
    else if (type == T_STAR) {
        frame = STAR_FRAME0 + (rand() % STAR_NUM_FRAME);
    }
    else if (type == T_BULLET) {
        frame = BUL_FRAME;
    }
    else if (type == T_GRASS) {
        frame = GRASS_FRAME0;
    }
    else {
        /* Won't happen, but avoid warning */
        frame = 0;
    }

    pSpr = 0;
    rv = gfmGroup_recycle(&pSpr, pGroup);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_init(pSpr, x, y, w, h, pSset, ox, oy, 0, type);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pSpr, frame);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFixed(pSpr);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setVelocity(pSpr, vx, 0/*vy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setAcceleration(pSpr, 0/*ax*/, 0/*ay*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV particle_spawnScene() {
    gfmRV rv;

    if (pGlobal->starTime <= 0) {
        int i;

        i = (rand() % STAR_NUM_MOD + 1) * STAR_NUM_MUL;
        while (i > 0) {
            int x, y;

            x = (((pGame->camX / 8) - 1) * 2 + (rand() % 10)) * 8;
            y = 0 + (rand() % 4) * 8;

            rv = particle_recycle(pGlobal->pParticles, T_STAR, x, y, 8/*w*/,
                    8/*h*/, 0/*ox*/, 0/*oy*/, 0/*vx*/);
            ASSERT(rv == GFMRV_OK, rv);

            i--;
        }

        pGlobal->starTime += STAR_COOLDOWN;
        pGlobal->starTime += (rand() % STAR_COOLDOWN_MOD) * STAR_COOLDOWN_MUL;
    }
    if (pGlobal->starTime > 0) {
        pGlobal->starTime -= pGame->elapsed;
    }

    if (pGlobal->cloudTime <= 0) {
        int vx, y;

        y = 13 + (rand() % 8);
        vx = -(CLOUD_VX + (rand() % CLOUD_VXMOD));
        rv = particle_recycle(pGlobal->pParticles, T_CLOUD,
                pGame->camX + V_WIDTH, y, CLOUD_W, CLOUD_H, 0/*ox*/, 0/*oy*/,
                vx);
        ASSERT(rv == GFMRV_OK, rv);

        pGlobal->cloudTime += CLOUD_NEXTTIME;
        pGlobal->cloudTime += (rand() % CLOUD_NEXTTIMEMUL) * CLOUD_NEXTTIMEMOD;
    }
    pGlobal->cloudTime -= pGame->elapsed;

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV particle_update(gfmGroup *pGroup, int doCollide) {
    gfmRV rv;

    rv = gfmGroup_update(pGroup, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    if (doCollide) {
        rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGroup);
        ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
        if (rv == GFMRV_QUADTREE_OVERLAPED) {
            rv = collision_run();
            ASSERT(rv == GFMRV_OK, rv);
        }
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


/**
 * Handle collisions
 *
 * @file src/collision.c
 */
#include <base/collision.h>
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTypes.h>

#include <jam/alien.h>
#include <jam/cow.h>
#include <jam/type.h>

#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
#include <stdlib.h>
#  include <signal.h>
#endif

static gfmRV __floor_cow() {
    gfmRV rv;
    gfmCollision dir;

    rv = gfmSprite_getCurrentCollision(&dir, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    if (dir & gfmCollision_down) {
        rv = gfmSprite_setVerticalVelocity(pGlobal->pCow, 0);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Retrieve a object's main subtype
 *
 * @param  [out]ppObj The object's pointer
 * @param  [out]pType The object's type
 * @param  [ in]pObj  The gfmObject being collided/overlaped
 * @param             GFraMe return value
 */
static inline gfmRV collision_getSubtype(void **ppObj, int *pType, gfmObject *pObj) {
    /** GFraMe return value */
    gfmRV rv;

    /** Get the object's child, if any (it's usually a sprite, but tilemaps, for
     * examples, doesn't  have a child even though they have a type (which is
     * OK) */
    rv = gfmObject_getChild(ppObj, pType, pObj);
    ASSERT(rv == GFMRV_OK, rv);

    if (*pType == gfmType_sprite) {
        /** The object's child, a sprite */
        gfmSprite *pSpr;
        void *pTmp;

        /** Retrieve the child of the sprite */
        pSpr = *((gfmSprite**)ppObj);
        rv = gfmSprite_getChild(&pTmp, pType, pSpr);
        ASSERT(rv == GFMRV_OK, rv);

        if (pTmp) {
            *ppObj = pTmp;
        }
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

/**
 * Continue the currently executing collision and handle the interaction of
 * objects
 *
 * @return GFraMe return value
 */
gfmRV collision_run() {
    /** GFraMe return value */
    gfmRV rv;

    /* Continue colliding until the quadtree finishes */
    rv = GFMRV_QUADTREE_OVERLAPED;
    while (rv != GFMRV_QUADTREE_DONE) {
        /** Overlaping gfmObjects */
        gfmObject *pObj1, *pObj2;
        /** gfmObjects children (if any) */
        void *pChild1, *pChild2;
        int type1, type2;
        int orType, isCase1;

        /* Retrieve the two overlaping objects and their types */
        rv = gfmQuadtree_getOverlaping(&pObj1, &pObj2, pGlobal->pQt);
        ASSERT(rv == GFMRV_OK, rv);
        rv = collision_getSubtype(&pChild1, &type1, pObj1);
        ASSERT(rv == GFMRV_OK, rv);
        rv = collision_getSubtype(&pChild2, &type2, pObj2);
        ASSERT(rv == GFMRV_OK, rv);

        /* If types have at most 16 bits, one could easily OR them together to
         * use a nice case to filter collision/handle */
        orType = type1 | (type2 << 16);

        /* Handle the collision */
        rv = GFMRV_OK;
        isCase1 = 0;
#define COL_TYPES(T1, T2) \
    case T1 | (T2 << 16): \
        isCase1 = 1; \
    case T2 | (T1 << 16):
#define IGN_TYPES(T1, T2) \
    case T1 | (T2 << 16): \
    case T2 | (T1 << 16):
        switch (orType) {
            /* e.g.: Handle collision between A and B the same as A and C */
            COL_TYPES(T_COW, T_FLOOR) {
                rv = gfmObject_collide(pObj1, pObj2);
                if (rv == GFMRV_TRUE) {
                    rv = __floor_cow();
                }
            } break;
            COL_TYPES(T_GRASS, T_EAT) {
                int frame;
                gfmSprite *pGrass;

                if (isCase1) {
                    pGrass = (gfmSprite*)pChild1;
                }
                else {
                    pGrass = (gfmSprite*)pChild2;
                }

                rv = gfmSprite_getFrame(&frame, pGrass);
                ASSERT(rv == GFMRV_OK, rv);
                if (frame == GRASS_FRAME0 + GRASS_NUMFRAMES - 1) {
                    pGlobal->grassCounter++;
                    pGlobal->grassCount.cur--;
                }
                if (frame != GRASS_FRAME0 + GRASS_NUMFRAMES) {
                    frame++;
                    rv = gfmSprite_setFrame(pGrass, frame);
                    ASSERT(rv == GFMRV_OK, rv);
                    rv = gfm_playAudio(0, pGame->pCtx, pAudio->eat, 0.6);
                    ASSERT(rv == GFMRV_OK, rv);
                }
                rv = GFMRV_OK;
            } break;
            COL_TYPES(T_ALIEN, T_COW) {
                rv = cow_hit();
            } break;
            COL_TYPES(T_ALIEN, T_BULLET) {
                alien *pAlien;
                gfmSprite *pBullet;

                if (isCase1) {
                    pAlien = (alien*)pChild1;
                    rv = gfmObject_getChild((void**)&pBullet, &type2, pObj2);
                    ASSERT(rv == GFMRV_OK, rv);
                }
                else {
                    pAlien = (alien*)pChild2;
                    rv = gfmObject_getChild((void**)&pBullet, &type1, pObj1);
                    ASSERT(rv == GFMRV_OK, rv);
                }

                rv = alien_hit(pAlien);
                ASSERT(rv == GFMRV_OK, rv);

                /* Explode bullet */
                rv = gfmSprite_playAnimation(pBullet, 0);
                ASSERT(rv == GFMRV_OK, rv);
                /* Move it slightly upward */
                rv = gfmSprite_setVerticalVelocity(pBullet, -16);
                ASSERT(rv == GFMRV_OK, rv);
            } break;
            COL_TYPES(T_ALIENV, T_COW) {
                gfmObject *pOther, *pSelf;
                alien *pAlien;
                int x1, x2;

                if (isCase1) {
                    pAlien = (alien*)pChild1;
                    pSelf = pObj1;
                    pOther = pObj2;
                }
                else {
                    pAlien = (alien*)pChild2;
                    pSelf = pObj2;
                    pOther = pObj1;
                }

                rv = gfmObject_getHorizontalPosition(&x1, pSelf);
                ASSERT(rv == GFMRV_OK, rv);
                rv = gfmObject_getHorizontalPosition(&x2, pOther);
                ASSERT(rv == GFMRV_OK, rv);
                if (x2 < x1) {
                    /* Make alien run to the left */
                    alien_pursueDir(pAlien, 1/*goLeft*/);
                }
                else {
                    /* Make alien run to the right */
                    alien_pursueDir(pAlien, 0/*goLeft*/);
                }
            } break;
            IGN_TYPES(T_CLOUD, T_COW)
            IGN_TYPES(T_CLOUD, T_BULLET)
            IGN_TYPES(T_CLOUD, T_STAR)
            IGN_TYPES(T_CLOUD, T_ALIEN)
            IGN_TYPES(T_CLOUD, T_ALIENV)
            IGN_TYPES(T_BULLET, T_COW)
            IGN_TYPES(T_BULLET, T_FLOOR)
            IGN_TYPES(T_BULLET, T_EAT)
            IGN_TYPES(T_BULLET, T_STAR)
            IGN_TYPES(T_BULLET, T_ALIENV)
            IGN_TYPES(T_EAT, T_FLOOR)
            IGN_TYPES(T_EAT, T_COW)
            IGN_TYPES(T_EAT, T_ALIEN)
            IGN_TYPES(T_EAT, T_ALIENV)
            IGN_TYPES(T_GRASS, T_FLOOR)
            IGN_TYPES(T_GRASS, T_COW)
            IGN_TYPES(T_GRASS, T_BULLET)
            IGN_TYPES(T_GRASS, T_ALIEN)
            IGN_TYPES(T_GRASS, T_ALIENV)
            IGN_TYPES(T_STAR, T_ALIEN)
            IGN_TYPES(T_STAR, T_COW)
            IGN_TYPES(T_STAR, T_ALIENV)
            IGN_TYPES(T_ALIEN, T_ALIENV)
            IGN_TYPES(T_ALIEN, T_FLOOR)
            IGN_TYPES(T_ALIENV, T_FLOOR)
            case T_CLOUD | (T_CLOUD << 16):
            case T_BULLET | (T_BULLET << 16):
            case T_GRASS | (T_GRASS << 16):
            case T_STAR | (T_STAR << 16):
            case T_ALIEN | (T_ALIEN << 16):
            case T_ALIENV | (T_ALIENV << 16):
                { /* Ignore collisiong */ } break;
            /* On Linux, a SIGINT is raised any time a unhandled collision
             * happens. When debugging, GDB will stop here and allow the user to
             * check which types weren't handled */
            default: {
#  if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
                /* Unfiltered collision, do something about it */
                raise(SIGINT);
                rv = GFMRV_INTERNAL_ERROR;
#  endif
            }
        } /* switch (orType) */
        ASSERT(rv == GFMRV_OK, rv);

        /** Update the quadtree (so any other collision is detected) */
        rv = gfmQuadtree_continue(pGlobal->pQt);
        ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE,
                rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


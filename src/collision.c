/**
 * Handle collisions
 *
 * @file src/collision.c
 */
#include <base/collision.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTypes.h>

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
        int orType;

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
        switch (orType) {
            /* e.g.: Handle collision between A and B the same as A and C */
            case T_COW | (T_FLOOR << 16):
            case T_FLOOR | (T_COW << 16): {
                rv = gfmObject_collide(pObj1, pObj2);
                if (rv == GFMRV_TRUE) {
                    rv = __floor_cow();
                }
            } break;
            case T_CLOUD | (T_COW << 16):
            case T_CLOUD | (T_CLOUD << 16):
            case T_CLOUD | (T_BULLET << 16):
            case T_COW | (T_CLOUD << 16):
            case T_BULLET | (T_CLOUD << 16):
            case T_BULLET | (T_COW << 16):
            case T_COW | (T_BULLET << 16):
            case T_BULLET | (T_FLOOR << 16):
            case T_FLOOR | (T_BULLET << 16):
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


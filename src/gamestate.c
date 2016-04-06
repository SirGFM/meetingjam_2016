/**
 * @file src/gamestate.c
 */
#include <base/game_ctx.h>
#include <base/game_const.h>
#include <base/collision.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <jam/cow.h>
#include <jam/gamestate.h>
#include <jam/particle.h>
#include <jam/type.h>

#include <string.h>

int MAP_W = 1000;
int MAP_H = 64;

static gfmRV init_grass(gfmParser *pParser) {
    gfmRV rv;
    int x, y;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    y -= 8;

    rv = particle_recycle(pGlobal->pGrass, T_GRASS, x, y, 8/*w*/, 8/*h*/,
            0/*ox*/, 0/*oy*/, 0/*vx*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV game_init() {
    gfmRV rv;
    gfmParser *pParser;

    if (!pGlobal->pFile) {
        pGlobal->pFile = "maps/map_easy.gfm";
    }

    pParser = 0;
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_init(pParser, pGame->pCtx, pGlobal->pFile,
            strlen(pGlobal->pFile));
    ASSERT(rv == GFMRV_OK, rv);

    pGlobal->hearts = UI_NUM_HEARTS;

    rv = gfmCamera_setWorldDimensions(pGame->pCam, MAP_W, MAP_H);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pGame->pCam, CAM_DEAD_X0, 0/*y*/, CAM_DEAD_X1,
            MAP_H);
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
    rv = particle_initGroup(pGlobal->pParticles, T_CLOUD, 4/*w*/, 4/*h*/,
            PART_TTL);
    ASSERT(rv == GFMRV_OK, rv);

    rv = particle_initGroup(pGlobal->pGrass, T_GRASS, 4/*w*/, 4/*h*/,
            -1/*ttl*/);
    ASSERT(rv == GFMRV_OK, rv);

    while (1) {
        char *pType;
        gfmParserType type;

        rv = gfmParser_parseNext(pParser);
        if (rv == GFMRV_PARSER_FINISHED) {
            break;
        }

        rv = gfmParser_getType(&type, pParser);
        ASSERT(rv == GFMRV_OK, rv);

        pType = 0;
        rv = gfmParser_getIngameType(&pType, pParser);
        ASSERT(rv == GFMRV_OK, rv);

        if (!strcmp(pType, "cow")) {
            rv = cow_init(pParser);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if (!strcmp(pType, "grass")) {
            rv = init_grass(pParser);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV game_clean() {
    return GFMRV_OK;
}

gfmRV game_update() {
    gfmRV rv;
    int cx, cy;

    /* == UPDATE ================== */

    rv = gfmQuadtree_initRoot(pGlobal->pQt, -8, -8, MAP_W, MAP_H, QT_MAX_DEPTH,
            QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateSprite(pGlobal->pQt, pGlobal->pFloor);
    ASSERT(rv == GFMRV_OK, rv);

    rv = particle_spawnScene();
    ASSERT(rv == GFMRV_OK, rv);

    rv = cow_update();
    ASSERT(rv == GFMRV_OK, rv);

    rv = particle_update(pGlobal->pParticles);
    ASSERT(rv == GFMRV_OK, rv);
    rv = particle_update(pGlobal->pGrass);
    ASSERT(rv == GFMRV_OK, rv);

    /* == POST ========================= */

    /*COW*/
    rv = cow_postUpdate();
    ASSERT(rv == GFMRV_OK, rv);

    /*CAMERA*/
    rv = gfmSprite_getCenter(&cx, &cy, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_centerAtPoint(pGame->pCam, cx, cy);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV game_draw() {
    gfmRV rv;
    int frame, i, x;

    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset64x16, 0/*x*/, FLOOR_Y,
            FLOOR_FRAME, 0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);

    x = (MOON_X0) * (pGame->camX / (float)(MAP_W)) + 
            (MOON_X1) * (1.0f - pGame->camX / (float)MAP_W);
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, MOON_Y, MOON_FRAME,
            0/*flip*/);
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
    rv = cow_draw();
    ASSERT(rv == GFMRV_OK, rv);

    /*UI*/
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset32x8, 0/*x*/, UI_GRASS_BAR_Y,
            UI_GRASS_BAR_FRAME0 + pGlobal->grassCounter, 0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);
    i = 0;
    x = UI_HEART_X0;
    while (i < pGlobal->hearts) {
        rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, UI_HEART_Y,
                UI_HEART_FRAME0, 0/*flip*/);
        ASSERT(rv == GFMRV_OK, rv);
        x += 8;
        i++;
    }
    while (i < UI_NUM_HEARTS) {
        rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, UI_HEART_Y,
                UI_HEART_FRAME0 + 1, 0/*flip*/);
        ASSERT(rv == GFMRV_OK, rv);
        x += 8;
        i++;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


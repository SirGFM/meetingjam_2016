/**
 * @file src/menustate.c
 */
#include <base/game_ctx.h>
#include <base/game_const.h>
#include <base/collision.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <jam/cow.h>
#include <jam/type.h>

#include <string.h>

int MAP_W = 1000;
int MAP_H = 64;

static gfmRV init_grass_group() {
    gfmRV rv;

    rv = gfmGroup_setDefType(pGlobal->pGrass, T_CLOUD);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefSpriteset(pGlobal->pGrass, pGfx->pSset8x8);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefDimensions(pGlobal->pGrass, 4/*w*/, 4/*h*/, 0/*ox*/,
            0/*oy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefVelocity(pGlobal->pGrass, 0/*vx*/, 0/*vy*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDefAcceleration(pGlobal->pGrass, 0/*ax*/, 0/*ay*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnLeave(pGlobal->pGrass, 0/*doDie*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDeathOnTime(pGlobal->pGrass, -1);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_preCache(pGlobal->pGrass, PART_CACHE, PART_CACHE);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setDrawOrder(pGlobal->pGrass, gfmDrawOrder_newestFirst);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_setCollisionQuality(pGlobal->pGrass,
            gfmCollisionQuality_visibleOnly);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

static gfmRV init_grass(gfmParser *pParser) {
    gfmRV rv;
    gfmSprite *pGrass;
    int x, y;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, rv);
    y -= 8;

    pGrass = 0;
    rv = gfmGroup_recycle(&pGrass, pGlobal->pGrass);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_init(pGrass, x, y, 8, 8,
        pGfx->pSset8x8, 0/*ox*/, 0/*oy*/, 0, T_GRASS);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFrame(pGrass, GRASS_FRAME0);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setFixed(pGrass);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setVelocity(pGrass, 0, 0);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmSprite_setAcceleration(pGrass, 0, 0);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_init() {
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

    rv = init_grass_group();
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

gfmRV menu_clean() {
    return GFMRV_OK;
}

gfmRV menu_update() {
    gfmRV rv;
    int cx, cy;

    /* == UPDATE ================== */

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
    }

    /*PARTICLES*/
    if (pGlobal->starTime <= 0) {
        int i;

        i = (rand() % 4 + 1) * 3;
        while (i > 0) {
            gfmSprite *pStar;
            int frame, x, y;

            pStar = 0;
            rv = gfmGroup_recycle(&pStar, pGlobal->pParticles);
            ASSERT(rv == GFMRV_OK, rv);

            x = 0 + (rand() % 8) * 8;
            y = 0 + (rand() % 4) * 8;
            frame = 226 + (rand() % 5);
            rv = gfmSprite_init(pStar, pGame->camX + x, y, 8, 8, pGfx->pSset8x8, 0,
                    0, 0, T_CLOUD);
            ASSERT(rv == GFMRV_OK, rv);
            rv = gfmSprite_setFrame(pStar, frame);
            ASSERT(rv == GFMRV_OK, rv);
            rv = gfmSprite_setVelocity(pStar, 0, 0);
            ASSERT(rv == GFMRV_OK, rv);
            i--;
        }

        pGlobal->starTime += 2500 - 250;
        pGlobal->starTime += (rand() % 50) * 10;
    }
    if (pGlobal->starTime > 0) {
        pGlobal->starTime -= pGame->elapsed;
    }

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

    /*GRASS*/
    rv = gfmGroup_update(pGlobal->pGrass, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    /* == COLLISION =============== */

    rv = gfmQuadtree_initRoot(pGlobal->pQt, -8, -8, MAP_W, MAP_H, QT_MAX_DEPTH,
            QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateSprite(pGlobal->pQt, pGlobal->pFloor);
    ASSERT(rv == GFMRV_OK, rv);

    /*COW*/
    rv = cow_update();
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pParticles);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = gfmQuadtree_collideGroup(pGlobal->pQt, pGlobal->pGrass);
    ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, rv);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        rv = collision_run();
        ASSERT(rv == GFMRV_OK, rv);
    }


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

gfmRV menu_draw() {
    gfmRV rv;
    int frame, i, x;

    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset64x16, 0/*x*/, FLOOR_Y,
            FLOOR_FRAME, 0/*flip*/);
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


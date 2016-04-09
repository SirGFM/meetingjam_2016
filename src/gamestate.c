/**
 * @file src/gamestate.c
 */
#include <base/game_ctx.h>
#include <base/game_const.h>
#include <base/collision.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmText.h>

#include <jam/alien.h>
#include <jam/cow.h>
#include <jam/gamestate.h>
#include <jam/particle.h>
#include <jam/type.h>

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

    gfmGenArr_reset(pGlobal->pAliens);

    pParser = 0;
    rv = gfmParser_getNew(&pParser);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmParser_initStatic(pParser, pGame->pCtx, "maps/map.gfm");
    ASSERT(rv == GFMRV_OK, rv);

    pGlobal->hearts = UI_NUM_HEARTS;

    rv = gfmCamera_setWorldDimensions(pGame->pCam, MAP_W, MAP_H);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmCamera_setDeadzone(pGame->pCam, 28, 0/*y*/, 4,
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
    rv = gfmGroup_killAll(pGlobal->pGrass);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_killAll(pGlobal->pBullets);
    ASSERT(rv == GFMRV_OK, rv);

    pGlobal->grassCounter = 0;
    pGlobal->grassCount.total = 0;
    pGlobal->alienCount.total = 0;

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
        else if (!strcmp(pType, "grass") ||
                (!strcmp(pType, "grass-easy") &&
                    pGlobal->menuState <= MENU_EASY) ||
                (!strcmp(pType, "grass-normal") &&
                     pGlobal->menuState <= MENU_NORMAL) ||
                (!strcmp(pType, "grass-hard") &&
                     pGlobal->menuState <= MENU_HARD)) {
            rv = init_grass(pParser);
            ASSERT(rv == GFMRV_OK, rv);
            pGlobal->grassCount.total++;
        }
        else if (!strcmp(pType, "alien") ||
                (!strcmp(pType, "alien-easy") &&
                    pGlobal->menuState >= MENU_EASY) ||
                (!strcmp(pType, "alien-normal") &&
                     pGlobal->menuState >= MENU_NORMAL) ||
                (!strcmp(pType, "alien-hard") &&
                     pGlobal->menuState >= MENU_HARD)) {
            rv = alien_init(pParser);
            ASSERT(rv == GFMRV_OK, rv);
            pGlobal->alienCount.total++;
        }
    }

    pGlobal->grassCount.cur = pGlobal->grassCount.total;
    pGlobal->alienCount.cur = pGlobal->alienCount.total;
    pGlobal->winState = WIN_NOT_SET;

    pGlobal->resetCount = 0;

    rv = gfmText_init(pGlobal->pText, 0/*x*/, 16/*y*/, 1 + V_WIDTH/8/*width*/,
            1/*maxLines*/, 300/*delay*/, 0/*bind to camera*/, pGfx->pSset8x8,
            0/*first tile*/);
    ASSERT(rv == GFMRV_OK, rv);

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

    rv = gfmQuadtree_initRoot(pGlobal->pQt, -8, -8, MAP_W, MAP_H, QT_MAX_DEPTH,
            QT_MAX_NODES);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmQuadtree_populateSprite(pGlobal->pQt, pGlobal->pFloor);
    ASSERT(rv == GFMRV_OK, rv);

    /* == UPDATE ================== */

    rv = cow_update();
    ASSERT(rv == GFMRV_OK, rv);
    rv = alien_update();
    ASSERT(rv == GFMRV_OK, rv);

    rv = particle_update(pGlobal->pParticles, 0/*doCollide*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = particle_update(pGlobal->pBullets, 1/*doCollide*/);
    ASSERT(rv == GFMRV_OK, rv);
    rv = particle_update(pGlobal->pGrass, 1/*doCollide*/);
    ASSERT(rv == GFMRV_OK, rv);

    /* == POST ========================= */

    rv = cow_postUpdate();
    ASSERT(rv == GFMRV_OK, rv);
    rv = alien_postUpdate();
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmSprite_getCenter(&cx, &cy, pGlobal->pCow);
    ASSERT(rv == GFMRV_OK, rv);
    gfmCamera_centerAtPoint(pGame->pCam, cx, cy);

    if (pGlobal->winState != WIN_NOT_SET) {
        rv = gfmText_update(pGlobal->pText, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);

        if (gfmText_didFinish(pGlobal->pText) == GFMRV_TRUE) {
            if (pGlobal->resetCount >= RESET_COUNT) {
                pGame->nextState = ST_MENUSTATE;
            }
            else if (pGlobal->winState == WIN_STATE) {
                rv = gfmText_setTextStatic(pGlobal->pText, " COW WIN ",
                        1/*doCopy*/);
                ASSERT(rv == GFMRV_OK, rv);
            }
            else if (pGlobal->winState == LOSE_STATE) {
                rv = gfmText_setTextStatic(pGlobal->pText, "ALIEN W. ",
                        1/*doCopy*/);
                ASSERT(rv == GFMRV_OK, rv);
            }
            pGlobal->resetCount++;
        }
    }
    else if (pGlobal->alienCount.cur == 0) {
        pGlobal->winState = WIN_STATE;

        rv = gfmText_setTextStatic(pGlobal->pText, " COW WIN ", 1/*doCopy*/);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else if (pGlobal->hearts == 0 || (pGlobal->grassCount.cur == 0 &&
            pGlobal->grassCounter < GRASS_MAX && pGlobal->laserTime <= 0)) {
        pGlobal->winState = LOSE_STATE;

        rv = gfmText_setTextStatic(pGlobal->pText, "ALIEN W. ", 1/*doCopy*/);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = particle_spawnScene();
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV game_draw() {
    float alpha;
    gfmRV rv;
    int i, x, y;

    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset64x16, 0/*x*/, FLOOR_Y,
            FLOOR_FRAME, 0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);

    alpha = pGame->camX / (float)(MAP_W);
    x = (MOON_X0) * alpha + (MOON_X1) * (1.0f - alpha);
    y = f_MOON_Y(x);
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, y, MOON_FRAME,
            0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);


    rv = cow_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = alien_draw();
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_draw(pGlobal->pBullets, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_draw(pGlobal->pParticles, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfmGroup_draw(pGlobal->pGrass, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

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

    x = 0;
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, 0/*y*/, '/'-'!', 0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);
    x += 8;
    rv = gfm_drawNumber(pGame->pCtx, pGfx->pSset8x8, x, 0/*y*/,
            pGlobal->alienCount.cur, 2/*res*/,  0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);
    x = V_WIDTH - 3*8;
    rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, 1/*y*/, 256/*tile*/,
            0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);
    x += 8;
    rv = gfm_drawNumber(pGame->pCtx, pGfx->pSset8x8, x, 0/*y*/,
            pGlobal->grassCount.cur, 2/*res*/,  0/*firstTile*/);
    ASSERT(rv == GFMRV_OK, rv);

    if (pGlobal->winState != WIN_NOT_SET) {
        rv = gfmText_draw(pGlobal->pText, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}


/**
 * @file src/menustate.c
 */
#include <base/game_const.h>
#include <base/game_ctx.h>

#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmText.h>

gfmRV menu_init() {
    gfmRV rv;

    rv = gfmText_init(pGlobal->pText, 4/*x*/, 4/*y*/, V_WIDTH/8/*width*/,
            7/*maxLines*/, 100/*delay*/, 0/*bind to camera*/, pGfx->pSset8x8,
            0/*first tile*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = gfmText_setTextStatic(pGlobal->pText,
            "THE COW\n"
            "STRIKES\n"
            " BACK\n"
            "\n"
            " EASY \n"
            " NORMAL\n"
            " HARD", 1/*doCopy*/);
    ASSERT(rv == GFMRV_OK, rv);

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_update() {
    gfmRV rv;

    if (gfmText_didFinish(pGlobal->pText) != GFMRV_TRUE) {
        if ((pButton->act.state & gfmInput_justPressed) ==
                gfmInput_justPressed ||
                (pButton->enter.state & gfmInput_justPressed) ==
                gfmInput_justPressed) {
            rv = gfmText_forceFinish(pGlobal->pText);
            ASSERT(rv == GFMRV_OK, rv);
        }
        rv = gfmText_update(pGlobal->pText, pGame->pCtx);
        ASSERT(rv == GFMRV_OK, rv);
    }
    else {
        if ((pButton->down.state & gfmInput_justPressed) ==
                gfmInput_justPressed){
            pGlobal->menuState++;
            pGlobal->menuState &= MENU_MAX;
            if (pGlobal->menuState == MENU_MAX) {
                pGlobal->menuState = 0;
            }
            rv = gfm_playAudio(0, pGame->pCtx, pAudio->menu, 0.6);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if ((pButton->jump.state & gfmInput_justPressed) ==
                gfmInput_justPressed){
            pGlobal->menuState--;
            pGlobal->menuState &= MENU_MAX;
            if (pGlobal->menuState == MENU_MAX) {
                pGlobal->menuState--;
            }
            rv = gfm_playAudio(0, pGame->pCtx, pAudio->menu, 0.6);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else if ((pButton->act.state & gfmInput_justPressed) ==
                gfmInput_justPressed ||
                (pButton->enter.state & gfmInput_justPressed) ==
                gfmInput_justPressed){
            pGame->nextState = ST_GAMESTATE;
            rv = gfm_playAudio(0, pGame->pCtx, pAudio->enterGame, 0.6);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

gfmRV menu_draw() {
    gfmRV rv;

    rv = gfmText_draw(pGlobal->pText, pGame->pCtx);
    ASSERT(rv == GFMRV_OK, rv);

    if (gfmText_didFinish(pGlobal->pText) == GFMRV_TRUE) {
        int x, y;

        x = 4;
        y = 4 + 4 * 8 + pGlobal->menuState * 8;
        rv = gfm_drawTile(pGame->pCtx, pGfx->pSset8x8, x, y, '>' - '!',
                0/*flip*/);
        ASSERT(rv == GFMRV_OK, rv);
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}

void menu_clean() {
}


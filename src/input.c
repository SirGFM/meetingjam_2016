/**
 * @file src/input.c
 *
 * Updates the game input and runs commands whenever an action is detected
 * (e.g., switch to/from fullscreen on btFullscreen)
 */
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmInput.h>

#include <base/game_ctx.h>
#include <base/input.h>

/**
 * Update all buttons' states
 *
 * @return GFraMe return value
 */
gfmRV input_updateButtons() {
    /** List of buttons, used to easily iterate through all of pButton's
     * members */
    button *pButtonList;
    /** Return value */
    gfmRV rv;
    /** Index of the current button being iterated */
    int i;

    /* Iterate through all buttons and retrieve their state */
    i = 0;
    pButtonList = (button*)pButton;
    while (i < (sizeof(buttonCtx) / sizeof(button))) {
        rv = gfm_getKeyState(&(pButtonList[i].state),
                &(pButtonList[i].numPressed), pGame->pCtx,
                pButtonList[i].handle);
        ASSERT(rv == GFMRV_OK, rv);

        i++;
    }

    /* Switch to/from fullscreen if the key was just pressed */
    if ((pButton->fullscreen.state & gfmInput_justReleased) ==
            gfmInput_justReleased) {
        if (pConfig->flags & CFG_FULLSCREEN) {
            rv = gfm_setWindowed(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        else {
            rv = gfm_setFullscreen(pGame->pCtx);
            ASSERT(rv == GFMRV_OK, rv);
        }
        pConfig->flags ^= CFG_FULLSCREEN;
    }
#if defined(DEBUG)
    /* Switch quadtree visibility */
    if ((pButton->qt.state & gfmInput_justReleased) == gfmInput_justReleased) {
        if (pGame->flags & DBG_RENDERQT) {
            pGame->flags &= ~DBG_RENDERQT;
        }
        else {
            pGame->flags |= DBG_RENDERQT;
        }
    }
    /* Start recording a GIF */
    if ((pButton->gif.state & gfmInput_justReleased) == gfmInput_justReleased) {
        rv = gfm_didExportGif(pGame->pCtx);
        if (rv == GFMRV_TRUE || rv == GFMRV_GIF_OPERATION_NOT_ACTIVE) {
            rv = gfm_recordGif(pGame->pCtx, 10000 /* ms */, "anim.gif", 8, 0);
            ASSERT(rv == GFMRV_OK, rv);
        }
    }
    /* Update the 'manual stepper' */
    rv = input_updateDebugButtons();
    ASSERT(rv == GFMRV_OK, rv);
#endif

    rv = GFMRV_OK;
__ret:
    return rv;
}

#if defined(DEBUG)
/**
 * Update only the debug buttons
 *
 * @return GFraMe return value
 */
gfmRV input_updateDebugButtons() {
    /** Return value */
    gfmRV rv;

    /* Retrieve the keys state */
    rv = gfm_getKeyState(&(pButton->dbgPause.state),
            &(pButton->dbgPause.numPressed), pGame->pCtx,
            pButton->dbgPause.handle);
    ASSERT(rv == GFMRV_OK, rv);
    rv = gfm_getKeyState(&(pButton->dbgStep.state),
            &(pButton->dbgStep.numPressed), pGame->pCtx,
            pButton->dbgStep.handle);
    ASSERT(rv == GFMRV_OK, rv);

    /* Check if the game was (un)paused */
    if ((pButton->dbgPause.state & gfmInput_justReleased) == gfmInput_justReleased) {
        if (pGame->flags & GAME_RUN) {
            pGame->flags &= ~GAME_RUN;
        }
        else {
            pGame->flags |= GAME_RUN;
        }
    }
    /* Check if a new frame should be issued */
    if ((pButton->dbgStep.state & gfmInput_justReleased) == gfmInput_justReleased) {
        pGame->flags |= GAME_STEP;
        pGame->flags &= ~GAME_RUN;
    }

    rv = GFMRV_OK;
__ret:
    return rv;
}
#endif

/**
 * Initialize and bind all buttons
 *
 * @return GFraMe return value
 */
gfmRV input_init() {
    /** Return value */
    gfmRV rv;

    /* Initialize all buttons */
#define ADD_KEY(bt) \
    rv = gfm_addVirtualKey(&(pButton->bt.handle), pGame->pCtx); \
    ASSERT(rv == GFMRV_OK, rv)

    ADD_KEY(fullscreen);
#if defined(DEBUG)
    ADD_KEY(gif);
    ADD_KEY(qt);
    ADD_KEY(dbgPause);
    ADD_KEY(dbgStep);
#endif
    ADD_KEY(enter);
    ADD_KEY(down);
    ADD_KEY(left);
    ADD_KEY(right);
    ADD_KEY(jump);
    ADD_KEY(act);

#undef ADD_KEY

    /* Bind all keys */
#define BIND_KEY(bt, key) \
    rv = gfm_bindInput(pGame->pCtx, pButton->bt.handle, key); \
    ASSERT(rv == GFMRV_OK, rv)
#define BIND_GAMEPAD_BT(bt, key, port) \
    rv = gfm_bindGamepadInput(pGame->pCtx, pButton->bt.handle, key, port); \
    ASSERT(rv == GFMRV_OK, rv)

    BIND_KEY(fullscreen, gfmKey_f12);
#if defined(DEBUG)
    BIND_KEY(gif, gfmKey_f10);
    BIND_KEY(qt, gfmKey_f11);
    BIND_KEY(dbgPause, gfmKey_f5);
    BIND_KEY(dbgStep, gfmKey_f6);
#endif
    BIND_KEY(enter, gfmKey_return);
    BIND_KEY(down, gfmKey_down);
    BIND_KEY(left, gfmKey_left);
    BIND_KEY(left, gfmKey_a);
    BIND_GAMEPAD_BT(left, gfmController_left, 0/*port*/);
    BIND_GAMEPAD_BT(left, gfmController_laxis_left, 0/*port*/);
    BIND_KEY(right, gfmKey_right);
    BIND_KEY(right, gfmKey_d);
    BIND_GAMEPAD_BT(right, gfmController_right, 0/*port*/);
    BIND_GAMEPAD_BT(right, gfmController_laxis_right, 0/*port*/);
    BIND_KEY(jump, gfmKey_up);
    BIND_KEY(jump, gfmKey_w);
    BIND_KEY(jump, gfmKey_j);
    BIND_KEY(jump, gfmKey_x);
    BIND_GAMEPAD_BT(jump, gfmController_a, 0/*port*/);
    BIND_KEY(act, gfmKey_space);
    BIND_KEY(act, gfmKey_k);
    BIND_KEY(act, gfmKey_c);
    BIND_GAMEPAD_BT(act, gfmController_b, 0/*port*/);

#undef BIND_KEY

    rv = GFMRV_OK;
__ret:
    return rv;
}


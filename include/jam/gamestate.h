/**
 * @file include/jam/gamestate.h
 */
#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <GFraMe/gfmError.h>

gfmRV game_init();
gfmRV game_clean();
gfmRV game_update();
gfmRV game_draw();

#endif /* __GAMESTATE_H__ */


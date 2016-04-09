/**
 * @file include/jam/menustate.h
 */
#ifndef __MENUSTATE_H__
#define __MENUSTATE_H__

#include <GFraMe/gfmError.h>

gfmRV menu_init();
gfmRV menu_update();
gfmRV menu_draw();
void menu_clean();

#endif /* __MENUSTATE_H__ */


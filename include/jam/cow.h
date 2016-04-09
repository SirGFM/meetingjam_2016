/**
 * @file include/cow.h
 */
#ifndef __COW_H__
#define __COW_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

gfmRV cow_init(gfmParser *pParser);
gfmRV cow_update();
gfmRV cow_postUpdate();
gfmRV cow_draw();
gfmRV cow_hit();

#endif /* __COW_H__ */


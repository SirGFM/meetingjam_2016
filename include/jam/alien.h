/**
 * @file include/jam/alien.h
 */
#ifndef __ALIEN_STRUCT__
#define __ALIEN_STRUCT__
typedef struct stAlien alien;
#endif /* __ALIEN_STRUCT__ */

#ifndef __ALIEN_H__
#define __ALIEN_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>


gfmRV alien_getNew(alien **ppCtx);
void alien_free(alien **ppCtx);
gfmRV alien_init(gfmParser *pParser);
gfmRV alien_update();
gfmRV alien_postUpdate();
gfmRV alien_draw();

#endif /* __ALIEN_H__ */


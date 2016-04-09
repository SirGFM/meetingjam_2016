/**
 * @file include/jam/particle.h
 */
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGroup.h>

#include <jam/type.h>

gfmRV particle_initGroup(gfmGroup *pGroup, mjType type, int w, int h, int ttl);
gfmRV particle_recycle(gfmGroup *pGroup, mjType type, int x, int y, int w,
        int h, int ox, int oy, int vx);
gfmRV particle_spawnScene();
gfmRV particle_update(gfmGroup *pGroup, int doCollide);

#endif /* __PARTICLE_H__ */


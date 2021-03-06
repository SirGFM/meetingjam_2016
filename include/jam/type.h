/**
 * @file include/jam/type.h
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#include <GFraMe/gfmTypes.h>

/**
 * Insteresting feature about types and the quadtree:
 *
 * If a type 'T' is greater or equal to gfmType_max, it will be displayed with
 * the same color as the type '(T % gfmType_max) + 1'.
 *
 * So, it's possible to have to different enemies be displayed with the same
 * color, even though they are handled differently by the code during collisiong
 */

/* Mask that returns only the entity, ignoring its base type */
#define T_MASK 0x0000ffe0
/* Number of bits for the types */
#define T_NBITS 5

enum enType {
    T_PART    = gfmType_reserved_5,
    T_CLOUD   = (1 << T_NBITS) | T_PART,
    T_STAR    = (2 << T_NBITS) | T_PART,
    /* Base type for collideables that doesn't hurt nor move (e.g., floor) */
    T_FLOOR   = gfmType_reserved_4,
    /* Base type for hitbox objects (i.e., bullets, slashes, etc) */
    T_HITBOX = gfmType_reserved_7,
    T_BULLET = (1 << T_NBITS) | T_HITBOX,
    T_EAT    = (2 << T_NBITS) | T_HITBOX,
    T_ALIENV = (3 << T_NBITS) | T_HITBOX,
    /* Base type for players and enemies */
    T_MOB    = gfmType_reserved_6,
    /* Base type for both players */
    T_COW    = (1 << T_NBITS) | T_MOB,
    T_ALIEN  = (2 << T_NBITS) | T_MOB,
    T_GRASS  = (3 << T_NBITS) | T_MOB,
};
typedef enum enType mjType;

#endif /* __TYPE_H__ */


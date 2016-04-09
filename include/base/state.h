/**
 * @file include/base/state.h
 *
 * List all the possibles states
 */
#ifndef __STATES_H__
#define __STATES_H__

enum enState {
    ST_NONE = 0,
    ST_MENUSTATE,
    ST_GAMESTATE,
    ST_CREDITSTATE,
    ST_MAX,
};
typedef enum enState state;

#endif /* __STATES_H__ */


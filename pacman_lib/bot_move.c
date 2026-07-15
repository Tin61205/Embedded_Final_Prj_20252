//--------------------------------------------------------------
// bot_move.c
// Ghost direction choice: personalities and scatter.
//--------------------------------------------------------------
#include "bot.h"
#include "pacman.h"
#include "ghost.h"
#include "player.h"
#include "skin.h"

#include <stdlib.h>

/* Path helpers live in bot.c (also declared in bot.h when available). */
uint32_t bot_calc_move(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t akt_dir);
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);


uint32_t bot_calc_move_random(uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t direction = 0;
    uint32_t loop_count = 0;

    // take a random direction, but not go back
    do {
        direction = rand() % (4); // [0...3]
        if ((direction == 0) && (akt_dir != MOVE_DOWN)) {
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) && bot_is_walkable(xp, yp - 1, 1)) {
                ret_wert = MOVE_UP;
            }
        } else if ((direction == 1) && (akt_dir != MOVE_LEFT)) {
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) && bot_is_walkable(xp + 1, yp, 1)) {
                ret_wert = MOVE_RIGHT;
            }
        } else if ((direction == 2) && (akt_dir != MOVE_UP)) {
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) && bot_is_walkable(xp, yp + 1, 1)) {
                ret_wert = MOVE_DOWN;
            }
        } else if ((direction == 3) && (akt_dir != MOVE_RIGHT)) {
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) && bot_is_walkable(xp - 1, yp, 1)) {
                ret_wert = MOVE_LEFT;
            }
        }
        loop_count++;
        if (loop_count > 100) {
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) && bot_is_walkable(xp, yp - 1, 1)) return MOVE_UP;
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) && bot_is_walkable(xp + 1, yp, 1)) return MOVE_RIGHT;
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) && bot_is_walkable(xp, yp + 1, 1)) return MOVE_DOWN;
            if (((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) && bot_is_walkable(xp - 1, yp, 1)) return MOVE_LEFT;
            break;
        }
    } while (ret_wert == MOVE_STOP);

    return (ret_wert);
}

//--------------------------------------------------------------
// bot strategy : blinky
//                 move to the room with the shortest distance
//                 to the player
//                 (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_calc_move_blinky(uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t txp, typ;

    bot_get_nearest_player(xp, yp, &txp, &typ);

    // calc the new move
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);

    return (ret_wert);
}

//--------------------------------------------------------------
// bot strategy : pinky
//                 move to the room with the shortest distance
//                 to the 4th room in front of the player's direction
//                 (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_calc_move_pinky(uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    int32_t txp, typ; // signed int !!
    uint32_t p_dir = MOVE_STOP;
    Player_t *target = bot_get_nearest_player_ptr(xp, yp);

    txp = target->xp;
    typ = target->yp;

    if (target->move == MOVE_STOP) {
        if (target->skin == PLAYER_SKIN_UP1) p_dir = MOVE_UP;
        if (target->skin == PLAYER_SKIN_RIGHT1) p_dir = MOVE_RIGHT;
        if (target->skin == PLAYER_SKIN_DOWN1) p_dir = MOVE_DOWN;
        if (target->skin == PLAYER_SKIN_LEFT1) p_dir = MOVE_LEFT;
    }

    if ((target->move == MOVE_UP) || (p_dir == MOVE_UP)) {
        typ -= 4;
        if (typ < 0) typ = 0;
    } else if ((target->move == MOVE_RIGHT) || (p_dir == MOVE_RIGHT)) {
        txp += 4;
        if (txp >= ROOM_CNT_X) txp = ROOM_CNT_X - 1;
    } else if ((target->move == MOVE_DOWN) || (p_dir == MOVE_DOWN)) {
        typ += 4;
        if (typ >= ROOM_CNT_Y) typ = ROOM_CNT_Y - 1;
    } else {
        txp -= 4;
        if (txp < 0) txp = 0;
    }

    // calc the new move
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);

    return (ret_wert);
}

//--------------------------------------------------------------
// bot strategy : inky
//                 move to the room with the shortest distance
//                 to the position "x"
//                 where "x" is calculated as :
//                 
//                 (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_calc_move_inky(uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    int32_t txp, typ; // signed int !!
    int32_t dxp, dyp; // signed int !!
    uint32_t bxp, byp;
    uint32_t p_dir = MOVE_STOP;
    Player_t *target = bot_get_nearest_player_ptr(xp, yp);

    txp = target->xp;
    typ = target->yp;

    if (target->move == MOVE_STOP) {
        if (target->skin == PLAYER_SKIN_UP1) p_dir = MOVE_UP;
        if (target->skin == PLAYER_SKIN_RIGHT1) p_dir = MOVE_RIGHT;
        if (target->skin == PLAYER_SKIN_DOWN1) p_dir = MOVE_DOWN;
        if (target->skin == PLAYER_SKIN_LEFT1) p_dir = MOVE_LEFT;
    }

    if ((target->move == MOVE_UP) || (p_dir == MOVE_UP)) {
        typ -= 2;
    } else if ((target->move == MOVE_RIGHT) || (p_dir == MOVE_RIGHT)) {
        txp += 2;
    } else if ((target->move == MOVE_DOWN) || (p_dir == MOVE_DOWN)) {
        typ += 2;
    } else {
        txp -= 2;
    }

    /* Tricky AI: use slot-0 as partner ghost (fallback to self if inactive). */
    if ((Game.ghost_active_mask & MOVE_GHOST0) != 0 && Ghosts[0].status == GHOST_STATUS_ALIVE) {
        bxp = Ghosts[0].xp;
        byp = Ghosts[0].yp;
    } else {
        bxp = xp;
        byp = yp;
    }

    // calculate the offset from blinky to the target
    dxp = txp - bxp;
    dyp = typ - byp;

    // move the target at the distance from the offset
    txp += dxp;
    typ += dyp;

    // check overflow
    if (txp < 0) txp = 0;
    if (txp >= ROOM_CNT_X) txp = ROOM_CNT_X - 1;
    if (typ < 0) typ = 0;
    if (typ >= ROOM_CNT_Y) typ = ROOM_CNT_Y - 1;

    // calc the new move
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);

    return (ret_wert);
}

//--------------------------------------------------------------
// bot strategy : clyde
//                 if the distance to the player is large enough
//                 set the player as target and chase him
//                 else set the scatter point as target
//
//                 (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_calc_move_clyde(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t txp, typ;
    uint32_t d_clyde = INIT_DISTANCE;

    bot_get_nearest_player(xp, yp, &txp, &typ);

    d_clyde = bot_calc_distance(txp, typ, xp, yp);

    if (d_clyde > 64) {
        // player is far away (more than 8 Rooms) -> chase him directly
        ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);
    } else {
        // player is nearby -> shy flee to home corner
        ret_wert = bot_calc_move_scatter(ghost, xp, yp, akt_dir);
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// bot strategy : scatter
//                 move to the room with the shortest distance
//                 to the scatter point
//                 (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_calc_move_scatter(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t txp, typ;

    ghosts_get_scatter(ghost, &txp, &typ);
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);

    return (ret_wert);
}


//--------------------------------------------------------------
// bot strategy : lazy
// chase (greedy) only when player is within 6 cells radius
// otherwise wander randomly
//--------------------------------------------------------------
uint32_t bot_calc_move_lazy(uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t txp;
    uint32_t typ;
    uint32_t dist;

    bot_get_nearest_player(xp, yp, &txp, &typ);
    dist = bot_calc_distance(txp, typ, xp, yp);

    if (dist <= 36) {
        return bot_calc_move_blinky(xp, yp, akt_dir);
    }
    return bot_calc_move_random(xp, yp, akt_dir);
}

//--------------------------------------------------------------
// resolve ghost personality to movement
//--------------------------------------------------------------
uint32_t bot_calc_move_by_strategy(uint32_t ghost, uint32_t strategy, uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    if (strategy == GHOST_TYPE_RANDOM || strategy == GHOST_TYPE_DRUNK) {
        return bot_calc_move_random(xp, yp, akt_dir);
    }
    if (strategy == GHOST_TYPE_LAZY) {
        return bot_calc_move_lazy(xp, yp, akt_dir);
    }
    if (strategy == GHOST_TYPE_CHASE) {
        return bot_calc_move_blinky(xp, yp, akt_dir);
    }
    if (strategy == GHOST_TYPE_AMBUSH) {
        return bot_calc_move_pinky(xp, yp, akt_dir);
    }
    if (strategy == GHOST_TYPE_TRICKY) {
        return bot_calc_move_inky(xp, yp, akt_dir);
    }
    return bot_calc_move_clyde(ghost, xp, yp, akt_dir);
}

//--------------------------------------------------------------
// human-readable ghost personality label
//--------------------------------------------------------------
const char* bot_strategy_name(uint32_t strategy) {
    if (strategy == GHOST_TYPE_CHASE) return "Chase";
    if (strategy == GHOST_TYPE_AMBUSH) return "Ambush";
    if (strategy == GHOST_TYPE_TRICKY) return "Tricky";
    if (strategy == GHOST_TYPE_SHY) return "Shy";
    if (strategy == GHOST_TYPE_DRUNK) return "Drunk";
    if (strategy == GHOST_TYPE_LAZY) return "Lazy";
    return "Random";
}


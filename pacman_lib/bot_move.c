//--------------------------------------------------------------
// bot_move.c
// Ghost direction choice: personalities, scatter, home (dead).
//--------------------------------------------------------------
#include "bot.h"
#include "pacman.h"
#include "ghost.h"
#include "player.h"
#include "skin.h"

#include <stdlib.h>
#include <string.h>

/* Path helpers live in bot.c (also declared in bot.h when available). */
uint32_t bot_calc_move(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t akt_dir);
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

extern uint32_t bot_avoid_portal;


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

static void bot_ghost_home_target(uint32_t ghost_id, uint32_t *tx, uint32_t *ty) {
    if (Game.play_type == GAME_PLAY_CUSTOM && ghost_id == GHOST_HUMAN) {
        *tx = 14;
        *ty = 14;
        return;
    }
    if (ghost_id < GHOST_MAX) {
        ghosts_get_home(ghost_id, tx, ty);
    } else {
        *tx = GHOST_HOUSE_EXIT_X;
        *ty = GHOST_HOUSE_EXIT_Y;
    }
}

static uint32_t bot_bfs_can_traverse(uint32_t cx, uint32_t cy,uint32_t nx, uint32_t ny,uint32_t dir) {
    uint32_t out_door;
    uint32_t in_door;

    switch (dir) {
        case MOVE_UP:
            out_door = ROOM_DOOR_U;
            in_door = ROOM_DOOR_D;
            break;
        case MOVE_RIGHT:
            out_door = ROOM_DOOR_R;
            in_door = ROOM_DOOR_L;
            break;
        case MOVE_DOWN:
            out_door = ROOM_DOOR_D;
            in_door = ROOM_DOOR_U;
            break;
        case MOVE_LEFT:
            out_door = ROOM_DOOR_L;
            in_door = ROOM_DOOR_R;
            break;
        default:
            return 0;
    }

    if (((Maze.Room[cx][cy].door & out_door) == 0) &&
        ((Maze.Room[nx][ny].door & in_door) == 0)) {
        if (Maze.Room[cx][cy].typ == ROOM_TYP_PATH &&
            Maze.Room[nx][ny].typ == ROOM_TYP_PATH) {
            return bot_is_walkable(nx, ny, 1);
        }
        return 0;
    }

    return bot_is_walkable(nx, ny, 1);
}

static uint32_t bot_calc_move_bfs_home(uint32_t x1, uint32_t y1,uint32_t x2, uint32_t y2) {
    static uint8_t visited[ROOM_CNT_X][ROOM_CNT_Y];
    static uint8_t first_move_arr[ROOM_CNT_X][ROOM_CNT_Y];
    static uint8_t q_x[ROOM_CNT_X * ROOM_CNT_Y];
    static uint8_t q_y[ROOM_CNT_X * ROOM_CNT_Y];
    uint32_t q_head;
    uint32_t q_tail;
    uint32_t cx;
    uint32_t cy;
    uint32_t nx;
    uint32_t ny;
    uint32_t fm;

    if (x1 == x2 && y1 == y2) {
        return MOVE_STOP;
    }
    if (x1 >= ROOM_CNT_X || y1 >= ROOM_CNT_Y) {
        return MOVE_STOP;
    }
    if (x2 >= ROOM_CNT_X || y2 >= ROOM_CNT_Y) {
        return MOVE_STOP;
    }

    memset(visited, 0, sizeof(visited));
    q_head = 0;
    q_tail = 0;
    visited[x1][y1] = 1;
    first_move_arr[x1][y1] = (uint8_t)MOVE_STOP;
    q_x[q_tail] = (uint8_t)x1;
    q_y[q_tail] = (uint8_t)y1;
    q_tail++;

    while (q_head < q_tail) {
        cx = q_x[q_head];
        cy = q_y[q_head];
        fm = first_move_arr[cx][cy];
        q_head++;

        if (cx == x2 && cy == y2) {
            return fm;
        }

        if (cy > 0) {
            nx = cx;
            ny = cy - 1;
            if (visited[nx][ny] == 0 && bot_bfs_can_traverse(cx, cy, nx, ny, MOVE_UP) != 0) {
                visited[nx][ny] = 1;
                first_move_arr[nx][ny] = (fm == MOVE_STOP) ? (uint8_t)MOVE_UP : (uint8_t)fm;
                q_x[q_tail] = (uint8_t)nx;
                q_y[q_tail] = (uint8_t)ny;
                q_tail++;
            }
        }
        if (cx < ROOM_CNT_X - 1) {
            nx = cx + 1;
            ny = cy;
            if (visited[nx][ny] == 0 && bot_bfs_can_traverse(cx, cy, nx, ny, MOVE_RIGHT) != 0) {
                visited[nx][ny] = 1;
                first_move_arr[nx][ny] = (fm == MOVE_STOP) ? (uint8_t)MOVE_RIGHT : (uint8_t)fm;
                q_x[q_tail] = (uint8_t)nx;
                q_y[q_tail] = (uint8_t)ny;
                q_tail++;
            }
        }
        if (cy < ROOM_CNT_Y - 1) {
            nx = cx;
            ny = cy + 1;
            if (visited[nx][ny] == 0 && bot_bfs_can_traverse(cx, cy, nx, ny, MOVE_DOWN) != 0) {
                visited[nx][ny] = 1;
                first_move_arr[nx][ny] = (fm == MOVE_STOP) ? (uint8_t)MOVE_DOWN : (uint8_t)fm;
                q_x[q_tail] = (uint8_t)nx;
                q_y[q_tail] = (uint8_t)ny;
                q_tail++;
            }
        }
        if (cx > 0) {
            nx = cx - 1;
            ny = cy;
            if (visited[nx][ny] == 0 && bot_bfs_can_traverse(cx, cy, nx, ny, MOVE_LEFT) != 0) {
                visited[nx][ny] = 1;
                first_move_arr[nx][ny] = (fm == MOVE_STOP) ? (uint8_t)MOVE_LEFT : (uint8_t)fm;
                q_x[q_tail] = (uint8_t)nx;
                q_y[q_tail] = (uint8_t)ny;
                q_tail++;
            }
        }
    }

    return MOVE_STOP;
}

//--------------------------------------------------------------
// bot strategy : home (dead ghost)
// BFS shortest path; avoid portal loops while returning home.
//--------------------------------------------------------------
uint32_t bot_calc_move_home(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t txp;
    uint32_t typ;

    bot_ghost_home_target(ghost, &txp, &typ);

    bot_avoid_portal = 1;
    ret_wert = bot_calc_move_bfs_home(xp, yp, txp, typ);
    bot_avoid_portal = 0;

    if (ret_wert != MOVE_STOP) {
        return ret_wert;
    }

    bot_avoid_portal = 1;
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);
    bot_avoid_portal = 0;

    return ret_wert;
}

uint32_t bot_calc_move_dead(uint32_t ghost_id, uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    Room_t *room = &Maze.Room[xp][yp];
    uint32_t move;

    if (room->special == ROOM_SPEC_GATE) {
        if ((room->door & (ROOM_BGATE_U | ROOM_PGATE_U | ROOM_IGATE_U | ROOM_CGATE_U)) != 0) {
            return MOVE_UP;
        }
        if ((room->door & (ROOM_BGATE_R | ROOM_PGATE_R | ROOM_IGATE_R | ROOM_CGATE_R)) != 0) {
            return MOVE_RIGHT;
        }
        if ((room->door & (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D)) != 0) {
            return MOVE_DOWN;
        }
        if ((room->door & (ROOM_BGATE_L | ROOM_PGATE_L | ROOM_IGATE_L | ROOM_CGATE_L)) != 0) {
            return MOVE_LEFT;
        }
    }

    move = bot_calc_move_home(ghost_id, xp, yp, MOVE_STOP);
    if (move != MOVE_STOP) {
        return move;
    }

    move = bot_calc_only_exit(xp, yp);
    if (move != MOVE_STOP) {
        return move;
    }

    return MOVE_STOP;
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


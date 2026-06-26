//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "bot.h"

#include <stdlib.h>

#include <stdio.h>

#include "stm32_ub_buzzer.h"
#include "gui.h"
#include "pacman.h"
#include "player.h"
#include "blinky.h"
#include "pinky.h"
#include "inky.h"
#include "clyde.h"

// Global variable definitions (declared extern in header)
Player_t Player;
Player_t Player2;
Ghost_t Blinky;
Ghost_t Pinky;
Ghost_t Inky;
Ghost_t Clyde;

uint32_t UB_SQRT(uint32_t wert);
uint32_t bot_calc_move(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t akt_dir);
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

//--------------------------------------------------------------
// bot strategy : random
// move randomly to the next room
// (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_is_2p_coop(void) {
    return (Game.play_type == GAME_PLAY_CUSTOM &&
            Game.custom.player_count == CUSTOM_PLAYER_2 &&
            Game.custom.two_player_mode == CUSTOM_2P_COOP &&
            Game.player2_active != 0) ? 1 : 0;
}

uint32_t bot_is_2p_vs_ghost(void) {
    return (Game.play_type == GAME_PLAY_CUSTOM &&
            Game.custom.player_count == CUSTOM_PLAYER_2 &&
            Game.custom.two_player_mode == CUSTOM_2P_VS_GHOST) ? 1 : 0;
}

uint32_t bot_custom_ai_ghost_count(void) {
    if (bot_is_2p_vs_ghost()) {
        return (Game.custom.ghost_count > 0) ? (Game.custom.ghost_count - 1) : 0;
    }
    return Game.custom.ghost_count;
}

uint32_t bot_is_player_controlled_ghost(uint32_t ghost_id) {
    return (bot_is_2p_vs_ghost() && ghost_id == GHOST_BLINKY) ? 1 : 0;
}

uint32_t bot_calc_move_player_ghost(uint32_t xp, uint32_t yp, uint32_t akt_dir, uint32_t joy) {
    if (joy == GUI_JOY_UP && (akt_dir != MOVE_DOWN) &&
        ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0)) {
        return MOVE_UP;
    }
    if (joy == GUI_JOY_RIGHT && (akt_dir != MOVE_LEFT) &&
        ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0)) {
        return MOVE_RIGHT;
    }
    if (joy == GUI_JOY_DOWN && (akt_dir != MOVE_UP) &&
        ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0)) {
        return MOVE_DOWN;
    }
    if (joy == GUI_JOY_LEFT && (akt_dir != MOVE_RIGHT) &&
        ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0)) {
        return MOVE_LEFT;
    }

    if (akt_dir == MOVE_UP && ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0)) {
        return MOVE_UP;
    }
    if (akt_dir == MOVE_RIGHT && ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0)) {
        return MOVE_RIGHT;
    }
    if (akt_dir == MOVE_DOWN && ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0)) {
        return MOVE_DOWN;
    }
    if (akt_dir == MOVE_LEFT && ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0)) {
        return MOVE_LEFT;
    }

    return MOVE_STOP;
}

void bot_apply_player_ghost_input(Ghost_t *ghost, uint32_t joy) {
    uint32_t xp;
    uint32_t yp;

    if (ghost == 0 || ghost->status != GHOST_STATUS_ALIVE || joy == GUI_JOY_NONE) {
        return;
    }

    xp = ghost->xp;
    yp = ghost->yp;

    if ((ghost->move == MOVE_LEFT || ghost->move == MOVE_RIGHT) &&
        (ghost->port == PORT_DONE) && (ghost->delta_y == 0)) {
        if (ABS(ghost->delta_x) <= PLAYER_TURN_ALIGN) {
            if (joy == GUI_JOY_UP && (ghost->move != MOVE_DOWN)) {
                if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) {
                    ghost->delta_x = 0;
                    ghost->move = MOVE_UP;
                    ghost->next_move = MOVE_UP;
                    return;
                }
            }
            if (joy == GUI_JOY_DOWN && (ghost->move != MOVE_UP)) {
                if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) {
                    ghost->delta_x = 0;
                    ghost->move = MOVE_DOWN;
                    ghost->next_move = MOVE_DOWN;
                    return;
                }
            }
        }
    }

    if ((ghost->move == MOVE_UP || ghost->move == MOVE_DOWN) &&
        (ghost->port == PORT_DONE) && (ghost->delta_x == 0)) {
        if (ABS(ghost->delta_y) <= PLAYER_TURN_ALIGN) {
            if (joy == GUI_JOY_LEFT && (ghost->move != MOVE_RIGHT)) {
                if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) {
                    ghost->delta_y = 0;
                    ghost->move = MOVE_LEFT;
                    ghost->next_move = MOVE_LEFT;
                    return;
                }
            }
            if (joy == GUI_JOY_RIGHT && (ghost->move != MOVE_LEFT)) {
                if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) {
                    ghost->delta_y = 0;
                    ghost->move = MOVE_RIGHT;
                    ghost->next_move = MOVE_RIGHT;
                    return;
                }
            }
        }
    }

    if (joy == GUI_JOY_UP) {
        if (ghost->move == MOVE_UP) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_DOWN) {
            ghost->move = MOVE_UP;
            ghost->next_move = MOVE_UP;
            return;
        }
        if ((ghost->delta_x == 0) && (ghost->delta_y == 0)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) {
                ghost->move = MOVE_UP;
                ghost->next_move = MOVE_UP;
            }
        }
    } else if (joy == GUI_JOY_RIGHT) {
        if (ghost->move == MOVE_RIGHT) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_LEFT) {
            ghost->move = MOVE_RIGHT;
            ghost->next_move = MOVE_RIGHT;
            return;
        }
        if ((ghost->delta_x == 0) && (ghost->delta_y == 0)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) {
                ghost->move = MOVE_RIGHT;
                ghost->next_move = MOVE_RIGHT;
            }
        }
    } else if (joy == GUI_JOY_DOWN) {
        if (ghost->move == MOVE_DOWN) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_UP) {
            ghost->move = MOVE_DOWN;
            ghost->next_move = MOVE_DOWN;
            return;
        }
        if ((ghost->delta_x == 0) && (ghost->delta_y == 0)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) {
                ghost->move = MOVE_DOWN;
                ghost->next_move = MOVE_DOWN;
            }
        }
    } else if (joy == GUI_JOY_LEFT) {
        if (ghost->move == MOVE_LEFT) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_RIGHT) {
            ghost->move = MOVE_LEFT;
            ghost->next_move = MOVE_LEFT;
            return;
        }
        if ((ghost->delta_x == 0) && (ghost->delta_y == 0)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) {
                ghost->move = MOVE_LEFT;
                ghost->next_move = MOVE_LEFT;
            }
        }
    }
}

void bot_get_nearest_player(uint32_t xp, uint32_t yp, uint32_t *txp, uint32_t *typ) {
    Player_t *target = bot_get_nearest_player_ptr(xp, yp);
    *txp = target->xp;
    *typ = target->yp;
}

Player_t* bot_get_nearest_player_ptr(uint32_t xp, uint32_t yp) {
    uint32_t p1_alive = (Player.status == PLAYER_STATUS_ALIVE) ? 1 : 0;
    uint32_t p2_alive = (Game.player2_active != 0 && Player2.status == PLAYER_STATUS_ALIVE) ? 1 : 0;

    if (p1_alive != 0 && p2_alive != 0) {
        uint32_t d1 = bot_calc_distance(xp, yp, Player.xp, Player.yp);
        uint32_t d2 = bot_calc_distance(xp, yp, Player2.xp, Player2.yp);
        if (d2 < d1) {
            return &Player2;
        }
        return &Player;
    }
    if (p2_alive != 0) {
        return &Player2;
    }
    if (p1_alive != 0) {
        return &Player;
    }
    return &Player;
}

static void bot_find_safe_respawn(uint32_t start_x, uint32_t start_y, uint32_t *respawn_x, uint32_t *respawn_y) {
    uint32_t best_x = start_x;
    uint32_t best_y = start_y;
    int32_t max_min_dist = -1;
    uint32_t x, y;

    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            if (Maze.Room[x][y].typ == ROOM_TYP_PATH && Maze.Room[x][y].special == ROOM_SPEC_NONE) {
                int32_t min_dist = 999999;
                
                if (Blinky.status == GHOST_STATUS_ALIVE) {
                    int32_t dist = ABS((int32_t)x - (int32_t)Blinky.xp) + ABS((int32_t)y - (int32_t)Blinky.yp);
                    if (dist < min_dist) min_dist = dist;
                }
                if (Pinky.status == GHOST_STATUS_ALIVE) {
                    int32_t dist = ABS((int32_t)x - (int32_t)Pinky.xp) + ABS((int32_t)y - (int32_t)Pinky.yp);
                    if (dist < min_dist) min_dist = dist;
                }
                if (Inky.status == GHOST_STATUS_ALIVE) {
                    int32_t dist = ABS((int32_t)x - (int32_t)Inky.xp) + ABS((int32_t)y - (int32_t)Inky.yp);
                    if (dist < min_dist) min_dist = dist;
                }
                if (Clyde.status == GHOST_STATUS_ALIVE) {
                    int32_t dist = ABS((int32_t)x - (int32_t)Clyde.xp) + ABS((int32_t)y - (int32_t)Clyde.yp);
                    if (dist < min_dist) min_dist = dist;
                }
                
                if (min_dist != 999999 && min_dist > max_min_dist) {
                    max_min_dist = min_dist;
                    best_x = x;
                    best_y = y;
                }
            }
        }
    }
    *respawn_x = best_x;
    *respawn_y = best_y;
}

void bot_kill_pacman(Player_t *p, uint32_t start_x, uint32_t start_y) {
    if (p->status != PLAYER_STATUS_ALIVE) {
        return;
    }

    // Play death sound (blocking tone sequence)
    UB_Buzzer_Play_Die();

    if (p->lives > 0) {
        p->lives--;
    }
    if (p->lives == 0) {
        p->status = PLAYER_STATUS_DEAD;
    } else {
        uint32_t rx = start_x;
        uint32_t ry = start_y;
        bot_find_safe_respawn(start_x, start_y, &rx, &ry);
        p->xp = rx;
        p->yp = ry;
        p->delta_x = 0;
        p->delta_y = 0;
        p->move = MOVE_STOP;
        p->skin = PLAYER_SKIN_LEFT1;
        p->skin_cnt = 0;
        p->port = PORT_DONE;
        p->status = PLAYER_STATUS_ALIVE;
    }
    bot_release_ghosts_on_pacman_death();
    GUI.refresh_value = GUI_REFRESH_VALUE;
}

void bot_team_kill_pacman(void) {
    bot_kill_pacman(&Player, PLAYER_START_X, PLAYER_START_Y);
}

uint32_t bot_coop_is_game_over(void) {
    if (bot_is_2p_coop()) {
        return (Player.lives == 0 && Player2.lives == 0) ? 1 : 0;
    }
    if (bot_is_2p_vs_ghost()) {
        return (Player.lives == 0) ? 1 : 0;
    }
    return 0;
}

void bot_team_win_pacman(void) {
    Player.status = PLAYER_STATUS_WIN;
    if (bot_is_2p_coop()) {
        Player2.status = PLAYER_STATUS_WIN;
    }
    GUI.refresh_value = GUI_REFRESH_VALUE;
}

void bot_ghost_hit_pacman(uint32_t gxp, uint32_t gyp, Ghost_t *ghost) {
    if (Game.collision == BOOL_FALSE || ghost->status != GHOST_STATUS_ALIVE) {
        return;
    }

    if (Player.status == PLAYER_STATUS_ALIVE && Player.xp == gxp && Player.yp == gyp) {
        if (Game.frightened == BOOL_FALSE) {
            bot_kill_pacman(&Player, PLAYER_START_X, PLAYER_START_Y);
        } else {
            ghost->status = GHOST_STATUS_DEAD;
            Player.score += Game.frightened_points;
            Game.frightened_points += Game.frightened_points;
        }
        GUI.refresh_value = GUI_REFRESH_VALUE;
        return;
    }

    if (bot_is_2p_coop() && Player2.status == PLAYER_STATUS_ALIVE && Player2.xp == gxp && Player2.yp == gyp) {
        if (Game.frightened == BOOL_FALSE) {
            bot_kill_pacman(&Player2, PLAYER2_START_X, PLAYER2_START_Y);
        } else {
            ghost->status = GHOST_STATUS_DEAD;
            Player.score += Game.frightened_points;
            Game.frightened_points += Game.frightened_points;
        }
        GUI.refresh_value = GUI_REFRESH_VALUE;
    }
}

uint32_t bot_ghost_get_body_color(uint32_t ghost_id, const Ghost_t *ghost, uint16_t *color) {
    if (color == 0 || ghost == 0) {
        return 0;
    }

    if (bot_is_player_controlled_ghost(ghost_id) != 0) {
        *color = GHOST_P2_COLOR;
        return 1;
    }

    if (ghost->strategy == GHOST_STRATEGY_BLINKY) {
        *color = GHOST_COLOR_CHASE;
    } else if (ghost->strategy == GHOST_STRATEGY_PINKY) {
        *color = GHOST_COLOR_AMBUSH;
    } else if (ghost->strategy == GHOST_STRATEGY_INKY) {
        *color = GHOST_COLOR_TRICKY;
    } else if (ghost->strategy == GHOST_STRATEGY_CLYDE) {
        *color = GHOST_COLOR_SHY;
    } else if (ghost->strategy == GHOST_STRATEGY_DRUNK) {
        *color = GHOST_COLOR_DRUNK;
    } else if (ghost->strategy == GHOST_STRATEGY_LAZY) {
        *color = GHOST_COLOR_LAZY;
    } else {
        *color = GHOST_COLOR_RANDOM;
    }
    return 1;
}

uint32_t bot_player_can_turn(uint32_t xp, uint32_t yp, uint32_t dir) {
    if (dir == MOVE_UP) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) && bot_is_walkable(xp, yp - 1, 0)) ? 1 : 0;
    }
    if (dir == MOVE_RIGHT) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) && bot_is_walkable(xp + 1, yp, 0)) ? 1 : 0;
    }
    if (dir == MOVE_DOWN) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) && bot_is_walkable(xp, yp + 1, 0)) ? 1 : 0;
    }
    if (dir == MOVE_LEFT) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) && bot_is_walkable(xp - 1, yp, 0)) ? 1 : 0;
    }
    return 0;
}

void bot_ghost_validate_position(Ghost_t *ghost) {
    if (!bot_is_walkable(ghost->xp, ghost->yp, 1)) {
        ghost->move = MOVE_STOP;
        ghost->next_move = MOVE_STOP;
        ghost->delta_x = 0;
        ghost->delta_y = 0;
    }
}

void bot_ghost_unstick(Ghost_t *ghost) {
    if (ghost->status != GHOST_STATUS_ALIVE || ghost->move != MOVE_STOP) {
        return;
    }
    ghost->next_move = bot_calc_move_random(ghost->xp, ghost->yp, MOVE_STOP);
    ghost->move = ghost->next_move;
}

void bot_release_ghosts_on_pacman_death(void) {
    if ((Game.ghost_active_mask & MOVE_BLINKY) != 0 && Blinky.status == GHOST_STATUS_ALIVE) {
        Blinky.new_mode = 0;
        Blinky.dot_cnt = BLINKY_DOT_CNT_MAX;
        bot_ghost_unstick(&Blinky);
    }
    if ((Game.ghost_active_mask & MOVE_PINKY) != 0 && Pinky.status == GHOST_STATUS_ALIVE) {
        Pinky.new_mode = 0;
        Pinky.dot_cnt = PINKY_DOT_CNT_MAX;
        bot_ghost_unstick(&Pinky);
    }
    if ((Game.ghost_active_mask & MOVE_INKY) != 0 && Inky.status == GHOST_STATUS_ALIVE) {
        Inky.new_mode = 0;
        Inky.dot_cnt = INKY_DOT_CNT_MAX;
        bot_ghost_unstick(&Inky);
    }
    if ((Game.ghost_active_mask & MOVE_CLYDE) != 0 && Clyde.status == GHOST_STATUS_ALIVE) {
        Clyde.new_mode = 0;
        Clyde.dot_cnt = CLYDE_DOT_CNT_MAX;
        bot_ghost_unstick(&Clyde);
    }
}

uint32_t bot_is_walkable(uint32_t x, uint32_t y, uint32_t for_ghost) {
    Room_t *room;

    if (x >= ROOM_CNT_X || y >= ROOM_CNT_Y) {
        return 0;
    }

    room = &Maze.Room[x][y];
    if (room->typ == ROOM_TYP_PATH) {
        return 1;
    }
    if (room->special == ROOM_SPEC_PORTAL) {
        return 1;
    }
    if (for_ghost != 0) {
        if (room->special == ROOM_SPEC_GATE) {
            return 1;
        }
        if ((room->typ == ROOM_TYP_WALL) && ((room->door & 0x0F) != 0)) {
            return 1;
        }
    }
    return 0;
}

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

    // spot Blinky
    bxp = Blinky.xp;
    byp = Blinky.yp;

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
        // player is nearby -> move to scatter point
        ret_wert = bot_calc_move_scatter(ghost, xp, yp, akt_dir);
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// bot strategy : home
//                 move to the room with the shortest distance
//                 to the home position
//                 (but don't go backwards to avoid toggling)
//--------------------------------------------------------------
uint32_t bot_calc_move_home(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t txp, typ;

    // spot the home position as target
    if (ghost == GHOST_BLINKY) {
        txp = BLINKY_HOME_X;
        typ = BLINKY_HOME_Y;
    } else if (ghost == GHOST_PINKY) {
        txp = PINKY_HOME_X;
        typ = PINKY_HOME_Y;
    } else if (ghost == GHOST_INKY) {
        txp = INKY_HOME_X;
        typ = INKY_HOME_Y;
    } else {
        txp = CLYDE_HOME_X;
        typ = CLYDE_HOME_Y;
    }

    // calc the new move
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);

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

    // spot the scatter point as target
    if (ghost == GHOST_BLINKY) {
        txp = BLINKY_SCATTER_X;
        typ = BLINKY_SCATTER_Y;
    } else if (ghost == GHOST_PINKY) {
        txp = PINKY_SCATTER_X;
        typ = PINKY_SCATTER_Y;
    } else if (ghost == GHOST_INKY) {
        txp = INKY_SCATTER_X;
        typ = INKY_SCATTER_Y;
    } else {
        txp = CLYDE_SCATTER_X;
        typ = CLYDE_SCATTER_Y;
    }

    // calc the new move
    ret_wert = bot_calc_move(xp, yp, txp, typ, akt_dir);

    return (ret_wert);
}

//--------------------------------------------------------------
// search the room around the actual position
// with the shortest distance to a target position
// but dont go back
// x1,y1 = actual position
// x2,y2 = target position
//--------------------------------------------------------------
uint32_t bot_calc_move(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t xn, yn;
    uint32_t d_up = INIT_DISTANCE, d_right = INIT_DISTANCE, d_down = INIT_DISTANCE, d_left = INIT_DISTANCE;
    uint32_t d_min;

    // calc for all possible directions the distance
    // but dont go back
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_U) != 0) && (akt_dir != MOVE_DOWN) && bot_is_walkable(x1, y1 - 1, 1)) {
        xn = x1;
        yn = y1 - 1;
        d_up = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_R) != 0) && (akt_dir != MOVE_LEFT) && bot_is_walkable(x1 + 1, y1, 1)) {
        xn = x1 + 1;
        yn = y1;
        d_right = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_D) != 0) && (akt_dir != MOVE_UP) && bot_is_walkable(x1, y1 + 1, 1)) {
        xn = x1;
        yn = y1 + 1;
        d_down = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_L) != 0) && (akt_dir != MOVE_RIGHT) && bot_is_walkable(x1 - 1, y1, 1)) {
        xn = x1 - 1;
        yn = y1;
        d_left = bot_calc_distance(x2, y2, xn, yn);
    }

    // search the shortest distance
    // in this order (up,left,down,right)
    d_min = INIT_DISTANCE;
    if (d_up < d_min) {
        d_min = d_up;
        ret_wert = MOVE_UP;
    }
    if (d_left < d_min) {
        d_min = d_left;
        ret_wert = MOVE_LEFT;
    }
    if (d_down < d_min) {
        d_min = d_down;
        ret_wert = MOVE_DOWN;
    }
    if (d_right < d_min) {
        d_min = d_right;
        ret_wert = MOVE_RIGHT;
    }

    // Fallback: if no valid direction is found (e.g. dead end or corner case)
    // select any available door (even if it is backwards)
    if (ret_wert == MOVE_STOP) {
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_U) != 0) && bot_is_walkable(x1, y1 - 1, 1)) {
            d_up = bot_calc_distance(x2, y2, x1, y1 - 1);
        }
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_R) != 0) && bot_is_walkable(x1 + 1, y1, 1)) {
            d_right = bot_calc_distance(x2, y2, x1 + 1, y1);
        }
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_D) != 0) && bot_is_walkable(x1, y1 + 1, 1)) {
            d_down = bot_calc_distance(x2, y2, x1, y1 + 1);
        }
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_L) != 0) && bot_is_walkable(x1 - 1, y1, 1)) {
            d_left = bot_calc_distance(x2, y2, x1 - 1, y1);
        }

        d_min = INIT_DISTANCE;
        if (d_up < d_min) {
            d_min = d_up;
            ret_wert = MOVE_UP;
        }
        if (d_left < d_min) {
            d_min = d_left;
            ret_wert = MOVE_LEFT;
        }
        if (d_down < d_min) {
            d_min = d_down;
            ret_wert = MOVE_DOWN;
        }
        if (d_right < d_min) {
            d_min = d_right;
            ret_wert = MOVE_RIGHT;
        }
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// calc the distane between two points
// c = SQRT(a*a + b*b)
//--------------------------------------------------------------
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    int32_t dx = (int32_t)x1 - (int32_t)x2;
    int32_t dy = (int32_t)y1 - (int32_t)y2;
    return (uint32_t)(dx * dx + dy * dy);
}

//--------------------------------------------------------------
// integer SQRT
//--------------------------------------------------------------
uint32_t UB_SQRT(uint32_t wert) {
    uint32_t ret_wert = 0;
    uint32_t square = 1;
    uint32_t delta = 3;

    while (square <= wert) {
        square += delta;
        delta += 2;
    }

    ret_wert = (delta / 2) - 1;

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
    if (strategy == GHOST_STRATEGY_RANDOM || strategy == GHOST_STRATEGY_DRUNK) {
        return bot_calc_move_random(xp, yp, akt_dir);
    }
    if (strategy == GHOST_STRATEGY_LAZY) {
        return bot_calc_move_lazy(xp, yp, akt_dir);
    }
    if (strategy == GHOST_STRATEGY_BLINKY) {
        return bot_calc_move_blinky(xp, yp, akt_dir);
    }
    if (strategy == GHOST_STRATEGY_PINKY) {
        return bot_calc_move_pinky(xp, yp, akt_dir);
    }
    if (strategy == GHOST_STRATEGY_INKY) {
        return bot_calc_move_inky(xp, yp, akt_dir);
    }
    return bot_calc_move_clyde(ghost, xp, yp, akt_dir);
}

//--------------------------------------------------------------
// human-readable ghost personality label
//--------------------------------------------------------------
const char* bot_strategy_name(uint32_t strategy) {
    if (strategy == GHOST_STRATEGY_BLINKY) return "Chase";
    if (strategy == GHOST_STRATEGY_PINKY) return "Ambush";
    if (strategy == GHOST_STRATEGY_INKY) return "Tricky";
    if (strategy == GHOST_STRATEGY_CLYDE) return "Shy";
    if (strategy == GHOST_STRATEGY_DRUNK) return "Drunk";
    if (strategy == GHOST_STRATEGY_LAZY) return "Lazy";
    return "Random";
}

//--------------------------------------------------------------
// find random path cell far enough from pacman start
//--------------------------------------------------------------
static uint32_t bot_find_spawn_pos(uint32_t *xp, uint32_t *yp) {
    uint32_t tries = 0;
    uint32_t px = Player.xp;
    uint32_t py = Player.yp;

    while (tries < 200) {
        uint32_t cx = (uint32_t)(rand() % (ROOM_CNT_X - 2)) + 1;
        uint32_t cy = (uint32_t)(rand() % (ROOM_CNT_Y - 2)) + 1;

        if (Maze.Room[cx][cy].typ == ROOM_TYP_PATH) {
            uint32_t dist = bot_calc_distance(px, py, cx, cy);
            if (bot_is_2p_coop() && Player2.status == PLAYER_STATUS_ALIVE) {
                uint32_t dist2 = bot_calc_distance(Player2.xp, Player2.yp, cx, cy);
                if (dist2 < dist) {
                    dist = dist2;
                }
            }
            if (dist >= GHOST_SPAWN_MIN_DIST) {
                *xp = cx;
                *yp = cy;
                return 1;
            }
        }
        tries++;
    }
    return 0;
}

//--------------------------------------------------------------
// apply custom ghost setup: count, personality, speed, spawn
//--------------------------------------------------------------
void bot_apply_custom_ghosts(uint32_t ghost_count, uint32_t strategies[4], uint32_t speed_ms) {
    Ghost_t *ghosts[4];
    uint32_t i;
    uint32_t used_x[4];
    uint32_t used_y[4];
    uint32_t used_cnt = 0;

    ghosts[0] = &Blinky;
    ghosts[1] = &Pinky;
    ghosts[2] = &Inky;
    ghosts[3] = &Clyde;

    for (i = 0; i < 4; i++) {
        if (bot_is_2p_vs_ghost()) {
            if (i == 0) {
                ghosts[i]->strategy = GHOST_STRATEGY_RANDOM;
            } else {
                ghosts[i]->strategy = strategies[i - 1];
            }
        } else {
            ghosts[i]->strategy = strategies[i];
        }
        ghosts[i]->akt_speed_ms = speed_ms;
        ghosts[i]->status = GHOST_STATUS_DEAD;
        ghosts[i]->move = MOVE_STOP;
        ghosts[i]->next_move = MOVE_STOP;
        ghosts[i]->delta_x = 0;
        ghosts[i]->delta_y = 0;
        ghosts[i]->skin_cnt = 0;
        ghosts[i]->port = PORT_DONE;
        ghosts[i]->dot_cnt = 0;
        ghosts[i]->new_mode = 0;
        
        if (i == 0) ghosts[i]->frightened_buf = 30;
        else if (i == 1) ghosts[i]->frightened_buf = 30;
        else if (i == 2) ghosts[i]->frightened_buf = 30;
        else ghosts[i]->frightened_buf = 30;
    }

    for (i = 0; i < ghost_count; i++) {
        uint32_t sx = 14;
        uint32_t sy = 11;
        uint32_t found = 0;
        uint32_t try_idx;
        uint32_t u;

        for (try_idx = 0; try_idx < 50; try_idx++) {
            if (bot_find_spawn_pos(&sx, &sy) == 0) {
                break;
            }
            found = 1;
            for (u = 0; u < used_cnt; u++) {
                if (used_x[u] == sx && used_y[u] == sy) {
                    found = 0;
                    break;
                }
            }
            if (found != 0) {
                break;
            }
        }

        if (found == 0) {
            if (i == 0) { sx = 14; sy = 11; }
            else if (i == 1) { sx = 14; sy = 14; }
            else if (i == 2) { sx = 13; sy = 14; }
            else { sx = 15; sy = 14; }
        }

        used_x[used_cnt] = sx;
        used_y[used_cnt] = sy;
        used_cnt++;

        ghosts[i]->status = GHOST_STATUS_ALIVE;
        ghosts[i]->xp = sx;
        ghosts[i]->yp = sy;
        ghosts[i]->skin = GHOST_SKIN_LEFT1;
        ghosts[i]->move = MOVE_LEFT;
        ghosts[i]->next_move = MOVE_LEFT;
        if (i == 0) {
            ghosts[i]->dot_cnt = BLINKY_DOT_CNT_MAX;
        } else if (i == 1) {
            ghosts[i]->dot_cnt = PINKY_DOT_CNT_MAX;
        } else if (i == 2) {
            ghosts[i]->dot_cnt = INKY_DOT_CNT_MAX;
        } else {
            ghosts[i]->dot_cnt = CLYDE_DOT_CNT_MAX;
        }
    }
}
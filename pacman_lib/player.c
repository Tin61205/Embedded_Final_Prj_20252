//--------------------------------------------------------------
// File     : player.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
//--------------------------------------------------------------
// File     : player.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "player.h"

#include <stdio.h>

extern Level_t Level[];

static void player_entity_init(Player_t *p, uint32_t start_x, uint32_t start_y, uint32_t mode, uint32_t owns_meta);
static void player_entity_move(Player_t *p, void (*check_fn)(Player_t *));
static void player_entity_change_skin(Player_t *p, uint32_t direction);
static void player_entity_check_event(Player_t *p);
static void player_entity_change_direction(Player_t *p, uint32_t joy);
static void player_entity_handle_ghost_hit(Player_t *p, Ghost_t *ghost);
static void player_check_entity_collision(Player_t *p);

void player_init(uint32_t mode) {
    player_entity_init(&Player, PLAYER_START_X, PLAYER_START_Y, mode, 1);
}

void player2_init(uint32_t mode) {
    player_entity_init(&Player2, PLAYER2_START_X, PLAYER2_START_Y, mode, 2);
}

static void player_entity_init(Player_t *p, uint32_t start_x, uint32_t start_y, uint32_t mode, uint32_t owns_meta) {
    p->status = PLAYER_STATUS_ALIVE;
    p->xp = start_x;
    p->yp = start_y;
    p->skin = PLAYER_SKIN_LEFT1;
    p->skin_cnt = 0;
    p->delta_x = 0;
    p->delta_y = 0;
    p->move = MOVE_LEFT;
    p->port = PORT_DONE;
    p->frightened_buf = PLAYER_FRIGHTENED_BUF;

    if (owns_meta == 1 && mode == GAME_OVER) {
        p->level = 1;
        p->score = 0;
        p->lives = PLAYER_START_LIVES;
    }
    if (owns_meta == 2 && mode == GAME_OVER) {
        p->lives = PLAYER_START_LIVES;
    }
    if (owns_meta == 1) {
        if (p->level <= GAME_MAX_LEVEL) {
            p->akt_speed_ms = Level[p->level - 1].player_speed;
        } else {
            p->akt_speed_ms = Level[GAME_MAX_LEVEL - 1].player_speed;
        }
        if ((mode == GAME_PLAYER_WIN) || (mode == GAME_OVER)) {
            p->point_dots = 0;
        }
    } else {
        p->akt_speed_ms = Player.akt_speed_ms;
    }
}

void player_move(void) {
    player_entity_move(&Player, player_entity_check_event);
}

void player2_move(void) {
    player_entity_move(&Player2, player_entity_check_event);
}

static void player_entity_move(Player_t *p, void (*check_fn)(Player_t *)) {
    if (p->status != PLAYER_STATUS_ALIVE) {
        return;
    }

    if (p->move == MOVE_UP) {
        p->delta_y--;
        player_entity_change_skin(p, MOVE_UP);
        if (p->delta_y == 0) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_U) == 0) {
                p->move = MOVE_STOP;
                p->skin = PLAYER_SKIN_UP1;
                return;
            }
        }
        if (ABS(p->delta_y) >= ROOM_HEIGHT) {
            p->delta_y = 0;
            if (p->port == PORT_DONE) {
                if (!bot_is_walkable(p->xp, p->yp - 1, 0)) {
                    p->move = MOVE_STOP;
                    p->skin = PLAYER_SKIN_UP1;
                } else {
                    p->yp--;
                    if (Maze.Room[p->xp][p->yp - 1].special == ROOM_SPEC_PORTAL) {
                        p->port = PORT_UP0;
                    } else if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_U) == 0) {
                        p->move = MOVE_STOP;
                        p->skin = PLAYER_SKIN_UP1;
                    }
                }
            } else if (p->port == PORT_UP0) {
                p->yp = ROOM_CNT_Y - 2;
                p->port = PORT_UP1;
            } else {
                p->yp--;
                p->port = PORT_DONE;
            }
            check_fn(p);
        }
    } else if (p->move == MOVE_RIGHT) {
        p->delta_x++;
        player_entity_change_skin(p, MOVE_RIGHT);
        if (p->delta_x == 0) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_R) == 0) {
                p->move = MOVE_STOP;
                p->skin = PLAYER_SKIN_RIGHT1;
                return;
            }
        }
        if (ABS(p->delta_x) >= ROOM_WIDTH) {
            p->delta_x = 0;
            if (p->port == PORT_DONE) {
                if (!bot_is_walkable(p->xp + 1, p->yp, 0)) {
                    p->move = MOVE_STOP;
                    p->skin = PLAYER_SKIN_RIGHT1;
                } else {
                    p->xp++;
                    if (Maze.Room[p->xp + 1][p->yp].special == ROOM_SPEC_PORTAL) {
                        p->port = PORT_RIGHT0;
                    } else if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_R) == 0) {
                        p->move = MOVE_STOP;
                        p->skin = PLAYER_SKIN_RIGHT1;
                    }
                }
            } else if (p->port == PORT_RIGHT0) {
                p->xp = 1;
                p->port = PORT_RIGHT1;
            } else {
                p->port = PORT_DONE;
                p->xp++;
            }
            check_fn(p);
        }
    } else if (p->move == MOVE_DOWN) {
        p->delta_y++;
        player_entity_change_skin(p, MOVE_DOWN);
        if (p->delta_y == 0) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_D) == 0) {
                p->move = MOVE_STOP;
                p->skin = PLAYER_SKIN_DOWN1;
                return;
            }
        }
        if (ABS(p->delta_y) >= ROOM_HEIGHT) {
            p->delta_y = 0;
            if (p->port == PORT_DONE) {
                if (!bot_is_walkable(p->xp, p->yp + 1, 0)) {
                    p->move = MOVE_STOP;
                    p->skin = PLAYER_SKIN_DOWN1;
                } else {
                    p->yp++;
                    if (Maze.Room[p->xp][p->yp + 1].special == ROOM_SPEC_PORTAL) {
                        p->port = PORT_DOWN0;
                    } else if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_D) == 0) {
                        p->move = MOVE_STOP;
                        p->skin = PLAYER_SKIN_DOWN1;
                    }
                }
            } else if (p->port == PORT_DOWN0) {
                p->yp = 1;
                p->port = PORT_DOWN1;
            } else {
                p->port = PORT_DONE;
                p->yp++;
            }
            check_fn(p);
        }
    } else if (p->move == MOVE_LEFT) {
        p->delta_x--;
        player_entity_change_skin(p, MOVE_LEFT);
        if (p->delta_x == 0) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_L) == 0) {
                p->move = MOVE_STOP;
                p->skin = PLAYER_SKIN_LEFT1;
                return;
            }
        }
        if (ABS(p->delta_x) >= ROOM_WIDTH) {
            p->delta_x = 0;
            if (p->port == PORT_DONE) {
                if (!bot_is_walkable(p->xp - 1, p->yp, 0)) {
                    p->move = MOVE_STOP;
                    p->skin = PLAYER_SKIN_LEFT1;
                } else {
                    p->xp--;
                    if (Maze.Room[p->xp - 1][p->yp].special == ROOM_SPEC_PORTAL) {
                        p->port = PORT_LEFT0;
                    } else if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_L) == 0) {
                        p->move = MOVE_STOP;
                        p->skin = PLAYER_SKIN_LEFT1;
                    }
                }
            } else if (p->port == PORT_LEFT0) {
                p->xp = ROOM_CNT_X - 2;
                p->port = PORT_LEFT1;
            } else {
                p->port = PORT_DONE;
                p->xp--;
            }
            check_fn(p);
        }
    }
}

static void player_entity_change_skin(Player_t *p, uint32_t direction) {
    p->skin_cnt++;
    if (p->skin_cnt > 7) p->skin_cnt = 0;

    if (direction == MOVE_UP) {
        if (p->skin_cnt < 2) p->skin = PLAYER_SKIN_FULL;
        else if (p->skin_cnt < 4) p->skin = PLAYER_SKIN_UP1;
        else if (p->skin_cnt < 6) p->skin = PLAYER_SKIN_UP2;
        else p->skin = PLAYER_SKIN_UP1;
    } else if (direction == MOVE_RIGHT) {
        if (p->skin_cnt < 2) p->skin = PLAYER_SKIN_FULL;
        else if (p->skin_cnt < 4) p->skin = PLAYER_SKIN_RIGHT1;
        else if (p->skin_cnt < 6) p->skin = PLAYER_SKIN_RIGHT2;
        else p->skin = PLAYER_SKIN_RIGHT1;
    } else if (direction == MOVE_DOWN) {
        if (p->skin_cnt < 2) p->skin = PLAYER_SKIN_FULL;
        else if (p->skin_cnt < 4) p->skin = PLAYER_SKIN_DOWN1;
        else if (p->skin_cnt < 6) p->skin = PLAYER_SKIN_DOWN2;
        else p->skin = PLAYER_SKIN_DOWN1;
    } else if (direction == MOVE_LEFT) {
        if (p->skin_cnt < 2) p->skin = PLAYER_SKIN_FULL;
        else if (p->skin_cnt < 4) p->skin = PLAYER_SKIN_LEFT1;
        else if (p->skin_cnt < 6) p->skin = PLAYER_SKIN_LEFT2;
        else p->skin = PLAYER_SKIN_LEFT1;
    }
}

static void player_entity_handle_ghost_hit(Player_t *p, Ghost_t *ghost) {
    if (ghost->status != GHOST_STATUS_ALIVE) {
        return;
    }
    if (Game.frightened == BOOL_FALSE) {
        if (p == &Player2) {
            bot_kill_pacman(&Player2, PLAYER2_START_X, PLAYER2_START_Y);
        } else {
            bot_kill_pacman(&Player, PLAYER_START_X, PLAYER_START_Y);
        }
    } else {
        ghost->status = GHOST_STATUS_DEAD;
        Player.score += Game.frightened_points;
        Game.frightened_points += Game.frightened_points;
        GUI.refresh_value = GUI_REFRESH_VALUE;
    }
}

static void player_entity_check_event(Player_t *p) {
    uint32_t xp = p->xp;
    uint32_t yp = p->yp;

    if (Maze.Room[xp][yp].points != ROOM_POINTS_NONE) {
        if (Maze.Room[xp][yp].points == ROOM_POINTS_NORMAL) {
            Player.score += GAME_POINTS_NORMAL;
            Player.point_dots++;
        } else {
            Player.score += GAME_POINTS_ENERGY;
            Player.point_dots++;
            Game.frightened = BOOL_TRUE;
            Game.frightened_timer = GAME_FRIGHTENED_TIME;
            if (Blinky.status == GHOST_STATUS_ALIVE) Blinky.new_mode = 1;
            if (Pinky.status == GHOST_STATUS_ALIVE) Pinky.new_mode = 1;
            if (Inky.status == GHOST_STATUS_ALIVE) Inky.new_mode = 1;
            if (Clyde.status == GHOST_STATUS_ALIVE) Clyde.new_mode = 1;
        }
        Maze.Room[xp][yp].points = ROOM_POINTS_NONE;
        Maze.Room[xp][yp].skin = ROOM_SKIN_POINTS_NONE;
        GUI.refresh_value = GUI_REFRESH_VALUE;
        Blinky.dot_cnt++;
        Pinky.dot_cnt++;
        Inky.dot_cnt++;
        Clyde.dot_cnt++;
    }

    if (Player.point_dots >= Maze.point_dots) {
        bot_team_win_pacman();
    }

    if (Game.collision == BOOL_TRUE) {
        if (Blinky.status == GHOST_STATUS_ALIVE && Blinky.xp == xp && Blinky.yp == yp) {
            player_entity_handle_ghost_hit(p, &Blinky);
        }
        if (Pinky.status == GHOST_STATUS_ALIVE && Pinky.xp == xp && Pinky.yp == yp) {
            player_entity_handle_ghost_hit(p, &Pinky);
        }
        if (Inky.status == GHOST_STATUS_ALIVE && Inky.xp == xp && Inky.yp == yp) {
            player_entity_handle_ghost_hit(p, &Inky);
        }
        if (Clyde.status == GHOST_STATUS_ALIVE && Clyde.xp == xp && Clyde.yp == yp) {
            player_entity_handle_ghost_hit(p, &Clyde);
        }
    }
}

void player_change_direction(uint32_t joy) {
    player_entity_change_direction(&Player, joy);
}

void player2_change_direction(uint32_t joy) {
    player_entity_change_direction(&Player2, joy);
}

static void player_entity_change_direction(Player_t *p, uint32_t joy) {
    if (p->status != PLAYER_STATUS_ALIVE) {
        return;
    }

    if ((p->move == MOVE_LEFT || p->move == MOVE_RIGHT) && (p->port == PORT_DONE) && (p->delta_y == 0)) {
        if (ABS(p->delta_x) <= PLAYER_TURN_ALIGN) {
            if (joy == GUI_JOY_UP && (p->move != MOVE_DOWN)) {
                if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_U) != 0) {
                    p->delta_x = 0;
                    p->move = MOVE_UP;
                    return;
                }
            }
            if (joy == GUI_JOY_DOWN && (p->move != MOVE_UP)) {
                if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_D) != 0) {
                    p->delta_x = 0;
                    p->move = MOVE_DOWN;
                    return;
                }
            }
        }
    }

    if ((p->move == MOVE_UP || p->move == MOVE_DOWN) && (p->port == PORT_DONE) && (p->delta_x == 0)) {
        if (ABS(p->delta_y) <= PLAYER_TURN_ALIGN) {
            if (joy == GUI_JOY_LEFT && (p->move != MOVE_RIGHT)) {
                if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_L) != 0) {
                    p->delta_y = 0;
                    p->move = MOVE_LEFT;
                    return;
                }
            }
            if (joy == GUI_JOY_RIGHT && (p->move != MOVE_LEFT)) {
                if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_R) != 0) {
                    p->delta_y = 0;
                    p->move = MOVE_RIGHT;
                    return;
                }
            }
        }
    }

    if (joy == GUI_JOY_UP) {
        if (p->move == MOVE_UP) return;
        if (p->port != PORT_DONE) return;
        if (p->move == MOVE_DOWN) {
            p->move = MOVE_UP;
            return;
        }
        if ((p->delta_x == 0) && (p->delta_y == 0)) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_U) != 0) {
                p->move = MOVE_UP;
            }
        }
    } else if (joy == GUI_JOY_RIGHT) {
        if (p->move == MOVE_RIGHT) return;
        if (p->port != PORT_DONE) return;
        if (p->move == MOVE_LEFT) {
            p->move = MOVE_RIGHT;
            return;
        }
        if ((p->delta_x == 0) && (p->delta_y == 0)) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_R) != 0) {
                p->move = MOVE_RIGHT;
            }
        }
    } else if (joy == GUI_JOY_DOWN) {
        if (p->move == MOVE_DOWN) return;
        if (p->port != PORT_DONE) return;
        if (p->move == MOVE_UP) {
            p->move = MOVE_DOWN;
            return;
        }
        if ((p->delta_x == 0) && (p->delta_y == 0)) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_D) != 0) {
                p->move = MOVE_DOWN;
            }
        }
    } else if (joy == GUI_JOY_LEFT) {
        if (p->move == MOVE_LEFT) return;
        if (p->port != PORT_DONE) return;
        if (p->move == MOVE_RIGHT) {
            p->move = MOVE_LEFT;
            return;
        }
        if ((p->delta_x == 0) && (p->delta_y == 0)) {
            if ((Maze.Room[p->xp][p->yp].door & ROOM_DOOR_L) != 0) {
                p->move = MOVE_LEFT;
            }
        }
    }
}

void player_check_collisions(void) {
    if (Game.collision == BOOL_FALSE) {
        return;
    }

    if (Player.status == PLAYER_STATUS_ALIVE) {
        player_check_entity_collision(&Player);
    }

    if (Game.player2_active != 0 && Player2.status == PLAYER_STATUS_ALIVE) {
        player_check_entity_collision(&Player2);
    }
}

static void player_check_entity_collision(Player_t *p) {
    Ghost_t *ghosts[4] = { &Blinky, &Pinky, &Inky, &Clyde };
    uint32_t active_ghost_mask[4] = { MOVE_BLINKY, MOVE_PINKY, MOVE_INKY, MOVE_CLYDE };
    int i;

    for (i = 0; i < 4; i++) {
        Ghost_t *g = ghosts[i];
        if ((Game.ghost_active_mask & active_ghost_mask[i]) != 0 && g->status == GHOST_STATUS_ALIVE) {
            int32_t px = ((int32_t)p->xp * ROOM_WIDTH) + p->delta_x;
            int32_t py = ((int32_t)p->yp * ROOM_HEIGHT) + p->delta_y;
            int32_t gx = ((int32_t)g->xp * ROOM_WIDTH) + g->delta_x;
            int32_t gy = ((int32_t)g->yp * ROOM_HEIGHT) + g->delta_y;

            if (ABS(px - gx) < 6 && ABS(py - gy) < 6) {
                player_entity_handle_ghost_hit(p, g);
            }
        }
    }
}
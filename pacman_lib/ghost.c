//--------------------------------------------------------------
// Unified ghost entity: move / init for Ghosts[0..GHOST_MAX-1]
//--------------------------------------------------------------
#include "ghost.h"
#include "pacman.h"
#include "gui.h"
#include "humanghost.h"

static void ghost_change_skin(Ghost_t *g, uint32_t direction);
static void ghost_check_event(Ghost_t *g, uint32_t id);
static void ghost_calc_next_move(Ghost_t *g, uint32_t id);

static const uint8_t GhostStartX[GHOST_MAX] = {
    GHOST_START_X0, GHOST_START_X1, GHOST_START_X2, GHOST_START_X3
};
static const uint8_t GhostStartY[GHOST_MAX] = {
    GHOST_START_Y0, GHOST_START_Y1, GHOST_START_Y2, GHOST_START_Y3
};
static const uint8_t GhostHomeX[GHOST_MAX] = {
    GHOST_HOME_X0, GHOST_HOME_X1, GHOST_HOME_X2, GHOST_HOME_X3
};
static const uint8_t GhostHomeY[GHOST_MAX] = {
    GHOST_HOME_Y0, GHOST_HOME_Y1, GHOST_HOME_Y2, GHOST_HOME_Y3
};
static const uint8_t GhostScatterX[GHOST_MAX] = {
    GHOST_SCATTER_X0, GHOST_SCATTER_X1, GHOST_SCATTER_X2, GHOST_SCATTER_X3
};
static const uint8_t GhostScatterY[GHOST_MAX] = {
    GHOST_SCATTER_Y0, GHOST_SCATTER_Y1, GHOST_SCATTER_Y2, GHOST_SCATTER_Y3
};
/* Slot 0 starts outside house; 1..3 start inside with pixel offset. */
static const int8_t GhostHomeDeltaX[GHOST_MAX] = { 0, GHOST_HOME_X_DIFF, GHOST_HOME_X_DIFF, GHOST_HOME_X_DIFF };
static const int8_t GhostHomeDeltaY[GHOST_MAX] = { 0, GHOST_HOME_Y_DIFF, GHOST_HOME_Y_DIFF, GHOST_HOME_Y_DIFF };
static const uint8_t GhostInitDir[GHOST_MAX] = { MOVE_LEFT, MOVE_UP, MOVE_RIGHT, MOVE_LEFT };
static const uint8_t GhostInitSkin[GHOST_MAX] = {
    GHOST_SKIN_LEFT1, GHOST_SKIN_UP1, GHOST_SKIN_RIGHT1, GHOST_SKIN_LEFT1
};
static const uint8_t GhostDefaultType[GHOST_MAX] = {
    GHOST_TYPE_CHASE, GHOST_TYPE_AMBUSH, GHOST_TYPE_TRICKY, GHOST_TYPE_SHY
};

uint32_t ghost_move_mask(uint32_t id) {
    if (id >= GHOST_MAX) {
        return 0;
    }
    return (1u << (id + 1)); /* MOVE_GHOST0 = 0x02 */
}

uint32_t ghost_dot_cnt_max(uint32_t id) {
    (void)id;
    if (id == GHOST_HUMAN) {
        return HUMAN_GHOST_DOT_CNT_MAX;
    }
    return GHOST_DOT_CNT_MAX_DEFAULT;
}

void ghosts_get_home(uint32_t id, uint32_t *hx, uint32_t *hy) {
    if (id < GHOST_MAX) {
        *hx = GhostHomeX[id];
        *hy = GhostHomeY[id];
    } else {
        *hx = GHOST_HOUSE_EXIT_X;
        *hy = GHOST_HOUSE_EXIT_Y;
    }
}

void ghosts_get_scatter(uint32_t id, uint32_t *sx, uint32_t *sy) {
    if (id < GHOST_MAX) {
        *sx = GhostScatterX[id];
        *sy = GhostScatterY[id];
    } else {
        *sx = 0;
        *sy = 0;
    }
}

uint32_t ghost_id_from_ptr(const Ghost_t *ghost) {
    uint32_t i;

    if (ghost == 0) {
        return GHOST_MAX;
    }
    if (ghost == &HumanGhost) {
        return GHOST_HUMAN;
    }
    for (i = 0; i < GHOST_MAX; i++) {
        if (ghost == &Ghosts[i]) {
            return i;
        }
    }
    return 0;
}

//--------------------------------------------------------------
// Reset all AI ghost slots (position/state). Personality/speed
// are applied afterwards by campaign/custom config.
//--------------------------------------------------------------
void ghosts_init(uint32_t mode) {
    uint32_t i;

    for (i = 0; i < GHOST_MAX; i++) {
        Ghost_t *g = &Ghosts[i];

        if (mode == GAME_OVER) {
            g->strategy = GhostDefaultType[i];
        }
        g->akt_speed_ms = GHOST_SPEED_NORMAL_MS;
        g->status = GHOST_STATUS_ALIVE;
        g->xp = GhostStartX[i];
        g->yp = GhostStartY[i];
        g->skin = GhostInitSkin[i];
        g->skin_cnt = 0;
        g->delta_x = GhostHomeDeltaX[i];
        g->delta_y = GhostHomeDeltaY[i];
        g->move = GhostInitDir[i];
        g->next_move = GhostInitDir[i];
        g->port = PORT_DONE;
        g->dot_cnt = (mode == GAME_OVER) ? 0 : ghost_dot_cnt_max(i);
        g->frightened_buf = GHOST_FRIGHTENED_BUF_DEFAULT;
        g->new_mode = 0;
    }
}

void ghost_move(uint32_t id) {
    Ghost_t *g;

    if (id >= GHOST_MAX) {
        return;
    }
    g = &Ghosts[id];

    if (g->status == GHOST_STATUS_DEAD) {
        bot_ghost_try_revive(g, id);
    }
    if (g->status == GHOST_STATUS_ALIVE && g->dot_cnt < ghost_dot_cnt_max(id)) {
        return;
    }

    if (g->move == MOVE_STOP) {
        bot_ghost_unstick(g, id);
    }
    if (g->move == MOVE_STOP) {
        return;
    }

    if (g->move == MOVE_UP) {
        g->delta_y--;
        ghost_change_skin(g, MOVE_UP);
        if (ABS(g->delta_y) >= ROOM_HEIGHT) {
            g->delta_y = 0;
            if (g->port == PORT_DONE) {
                g->yp--;
                if (Maze.Room[g->xp][g->yp - 1].special == ROOM_SPEC_PORTAL) {
                    g->port = PORT_UP0;
                }
            } else if (g->port == PORT_UP0) {
                g->yp = ROOM_CNT_Y - 2;
                g->port = PORT_UP1;
            } else {
                g->port = PORT_DONE;
                g->yp--;
            }
            ghost_check_event(g, id);
            g->move = g->next_move;
            ghost_calc_next_move(g, id);
        }
    } else if (g->move == MOVE_RIGHT) {
        g->delta_x++;
        ghost_change_skin(g, MOVE_RIGHT);
        if (ABS(g->delta_x) >= ROOM_WIDTH) {
            g->delta_x = 0;
            if (g->port == PORT_DONE) {
                g->xp++;
                if (Maze.Room[g->xp + 1][g->yp].special == ROOM_SPEC_PORTAL) {
                    g->port = PORT_RIGHT0;
                }
            } else if (g->port == PORT_RIGHT0) {
                g->xp = 1;
                g->port = PORT_RIGHT1;
            } else {
                g->port = PORT_DONE;
                g->xp++;
            }
            ghost_check_event(g, id);
            g->move = g->next_move;
            ghost_calc_next_move(g, id);
        }
    } else if (g->move == MOVE_DOWN) {
        g->delta_y++;
        ghost_change_skin(g, MOVE_DOWN);
        if (ABS(g->delta_y) >= ROOM_HEIGHT) {
            g->delta_y = 0;
            if (g->port == PORT_DONE) {
                g->yp++;
                if (Maze.Room[g->xp][g->yp + 1].special == ROOM_SPEC_PORTAL) {
                    g->port = PORT_DOWN0;
                }
            } else if (g->port == PORT_DOWN0) {
                g->yp = 1;
                g->port = PORT_DOWN1;
            } else {
                g->port = PORT_DONE;
                g->yp++;
            }
            ghost_check_event(g, id);
            g->move = g->next_move;
            ghost_calc_next_move(g, id);
        }
    } else if (g->move == MOVE_LEFT) {
        g->delta_x--;
        ghost_change_skin(g, MOVE_LEFT);
        if (ABS(g->delta_x) >= ROOM_WIDTH) {
            g->delta_x = 0;
            if (g->port == PORT_DONE) {
                g->xp--;
                if (Maze.Room[g->xp - 1][g->yp].special == ROOM_SPEC_PORTAL) {
                    g->port = PORT_LEFT0;
                }
            } else if (g->port == PORT_LEFT0) {
                g->xp = ROOM_CNT_X - 2;
                g->port = PORT_LEFT1;
            } else {
                g->port = PORT_DONE;
                g->xp--;
            }
            ghost_check_event(g, id);
            g->move = g->next_move;
            ghost_calc_next_move(g, id);
        }
    } else {
        g->next_move = MOVE_STOP;
    }
}

static void ghost_change_skin(Ghost_t *g, uint32_t direction) {
    g->skin_cnt++;
    if (g->skin_cnt > 7) {
        g->skin_cnt = 0;
    }

    if (g->status == GHOST_STATUS_DEAD) {
        g->skin = GHOST_SKIN_DEAD;
        return;
    }

    if (direction == MOVE_UP) {
        g->skin = (g->skin_cnt < 4) ? GHOST_SKIN_UP2 : GHOST_SKIN_UP1;
    } else if (direction == MOVE_RIGHT) {
        g->skin = (g->skin_cnt < 4) ? GHOST_SKIN_RIGHT2 : GHOST_SKIN_RIGHT1;
    } else if (direction == MOVE_DOWN) {
        g->skin = (g->skin_cnt < 4) ? GHOST_SKIN_DOWN2 : GHOST_SKIN_DOWN1;
    } else if (direction == MOVE_LEFT) {
        g->skin = (g->skin_cnt < 4) ? GHOST_SKIN_LEFT2 : GHOST_SKIN_LEFT1;
    }

    if (Game.frightened == BOOL_TRUE) {
        if (Game.frightened_timer > GAME_FRIGHTENED_BLINK) {
            g->skin = GHOST_SKIN_FRIGHTEN1;
        } else {
            g->skin = (g->skin_cnt < 4) ? GHOST_SKIN_FRIGHTEN2 : GHOST_SKIN_FRIGHTEN1;
        }
    }
}

static void ghost_check_event(Ghost_t *g, uint32_t id) {
    bot_ghost_validate_position(g);
    bot_ghost_hit_pacman(g->xp, g->yp, g);
    bot_ghost_try_revive(g, id);
}

static void ghost_calc_next_move(Ghost_t *g, uint32_t id) {
    uint32_t door_cnt = 0;
    uint32_t xp, yp;

    xp = g->xp;
    yp = g->yp;

    if (g->move == MOVE_STOP) {
        g->next_move = MOVE_STOP;
        return;
    }
    if (g->move == MOVE_UP) yp--;
    if (g->move == MOVE_RIGHT) xp++;
    if (g->move == MOVE_DOWN) yp++;
    if (g->move == MOVE_LEFT) xp--;

    if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) door_cnt++;

    if ((g->status == GHOST_STATUS_DEAD) && (Maze.Room[xp][yp].special == ROOM_SPEC_GATE)) {
        door_cnt = 2;
    }

    if (door_cnt > 1 && g->new_mode == 1) {
        g->new_mode = 0;
        if (g->move == MOVE_UP) g->next_move = MOVE_DOWN;
        if (g->move == MOVE_RIGHT) g->next_move = MOVE_LEFT;
        if (g->move == MOVE_DOWN) g->next_move = MOVE_UP;
        if (g->move == MOVE_LEFT) g->next_move = MOVE_RIGHT;
        return;
    }

    if (g->status == GHOST_STATUS_DEAD) {
        g->next_move = bot_calc_move_dead(id, xp, yp, g->move);
        return;
    }

    if (door_cnt == 0) {
        g->next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        g->next_move = bot_calc_only_exit(xp, yp);
    } else {
        if (Game.frightened == BOOL_TRUE) {
            g->next_move = bot_calc_move_random(xp, yp, g->move);
            return;
        }
        g->next_move = bot_calc_move_by_strategy(id, g->strategy, xp, yp, g->move);
    }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
// Player-controlled ghost (2P Vs Ghost mode)
//--------------------------------------------------------------
#include "humanghost.h"
#include "pacman.h"

void humanghost_check_event(void);
void humanghost_change_skin(uint32_t direction);
void humanghost_calc_next_move(void);

static void humanghost_advance_cell(void) {
    humanghost_check_event();
    if (bot_is_human_ghost_active() != 0 && HumanGhost.status == GHOST_STATUS_ALIVE) {
        uint32_t dir = bot_calc_move_player_ghost(
            HumanGhost.xp, HumanGhost.yp, HumanGhost.move, Game.player2_joy);
        HumanGhost.move = dir;
        HumanGhost.next_move = dir;
    } else {
        HumanGhost.move = HumanGhost.next_move;
    }
    humanghost_calc_next_move();
}

/* Stop if current direction has no walkable exit from cell center. */
static uint32_t humanghost_dir_blocked_at_center(uint32_t dir) {
    return (bot_ghost_can_turn(HumanGhost.xp, HumanGhost.yp, dir) == 0) ? 1 : 0;
}

static void humanghost_stop_here(void) {
    HumanGhost.move = MOVE_STOP;
    HumanGhost.next_move = MOVE_STOP;
    HumanGhost.delta_x = 0;
    HumanGhost.delta_y = 0;
}

static uint32_t last_moving_dir = MOVE_LEFT;

void humanghost_move(void) {
    if (HumanGhost.move != MOVE_STOP) {
        last_moving_dir = HumanGhost.move;
    }
    humanghost_change_skin(last_moving_dir);

    if (HumanGhost.move == MOVE_STOP) {
        bot_ghost_unstick(&HumanGhost, GHOST_HUMAN);
    }
    if (HumanGhost.move == MOVE_STOP) {
        return;
    }

    /* At rest on a cell: do not start moving into a wall (spawn reverse bug). */
    if (HumanGhost.port == PORT_DONE &&
        HumanGhost.delta_x == 0 && HumanGhost.delta_y == 0 &&
        humanghost_dir_blocked_at_center(HumanGhost.move) != 0) {
        humanghost_stop_here();
        bot_ghost_unstick(&HumanGhost, GHOST_HUMAN);
        return;
    }

    if (HumanGhost.move == MOVE_UP) {
        HumanGhost.delta_y--;
        /* Returning to cell center: stop if up is a wall (e.g. reverse into wall). */
        if (HumanGhost.delta_y == 0 && HumanGhost.port == PORT_DONE) {
            if (humanghost_dir_blocked_at_center(MOVE_UP) != 0) {
                humanghost_stop_here();
                return;
            }
        }
        if (ABS(HumanGhost.delta_y) >= ROOM_HEIGHT) {
            HumanGhost.delta_y = 0;
            if (HumanGhost.port == PORT_DONE) {
                if (!bot_is_walkable(HumanGhost.xp, HumanGhost.yp - 1, 1)) {
                    humanghost_stop_here();
                    return;
                }
                HumanGhost.yp--;
                if (Maze.Room[HumanGhost.xp][HumanGhost.yp - 1].special == ROOM_SPEC_PORTAL) {
                    HumanGhost.port = PORT_UP0;
                }
            } else if (HumanGhost.port == PORT_UP0) {
                HumanGhost.yp = ROOM_CNT_Y - 2;
                HumanGhost.port = PORT_UP1;
            } else {
                HumanGhost.port = PORT_DONE;
                HumanGhost.yp--;
            }
            humanghost_advance_cell();
        }
    } else if (HumanGhost.move == MOVE_RIGHT) {
        HumanGhost.delta_x++;
        if (HumanGhost.delta_x == 0 && HumanGhost.port == PORT_DONE) {
            if (humanghost_dir_blocked_at_center(MOVE_RIGHT) != 0) {
                humanghost_stop_here();
                return;
            }
        }
        if (ABS(HumanGhost.delta_x) >= ROOM_WIDTH) {
            HumanGhost.delta_x = 0;
            if (HumanGhost.port == PORT_DONE) {
                if (!bot_is_walkable(HumanGhost.xp + 1, HumanGhost.yp, 1)) {
                    humanghost_stop_here();
                    return;
                }
                HumanGhost.xp++;
                if (Maze.Room[HumanGhost.xp + 1][HumanGhost.yp].special == ROOM_SPEC_PORTAL) {
                    HumanGhost.port = PORT_RIGHT0;
                }
            } else if (HumanGhost.port == PORT_RIGHT0) {
                HumanGhost.xp = 1;
                HumanGhost.port = PORT_RIGHT1;
            } else {
                HumanGhost.port = PORT_DONE;
                HumanGhost.xp++;
            }
            humanghost_advance_cell();
        }
    } else if (HumanGhost.move == MOVE_DOWN) {
        HumanGhost.delta_y++;
        if (HumanGhost.delta_y == 0 && HumanGhost.port == PORT_DONE) {
            if (humanghost_dir_blocked_at_center(MOVE_DOWN) != 0) {
                humanghost_stop_here();
                return;
            }
        }
        if (ABS(HumanGhost.delta_y) >= ROOM_HEIGHT) {
            HumanGhost.delta_y = 0;
            if (HumanGhost.port == PORT_DONE) {
                if (!bot_is_walkable(HumanGhost.xp, HumanGhost.yp + 1, 1)) {
                    humanghost_stop_here();
                    return;
                }
                HumanGhost.yp++;
                if (Maze.Room[HumanGhost.xp][HumanGhost.yp + 1].special == ROOM_SPEC_PORTAL) {
                    HumanGhost.port = PORT_DOWN0;
                }
            } else if (HumanGhost.port == PORT_DOWN0) {
                HumanGhost.yp = 1;
                HumanGhost.port = PORT_DOWN1;
            } else {
                HumanGhost.port = PORT_DONE;
                HumanGhost.yp++;
            }
            humanghost_advance_cell();
        }
    } else if (HumanGhost.move == MOVE_LEFT) {
        HumanGhost.delta_x--;
        if (HumanGhost.delta_x == 0 && HumanGhost.port == PORT_DONE) {
            if (humanghost_dir_blocked_at_center(MOVE_LEFT) != 0) {
                humanghost_stop_here();
                return;
            }
        }
        if (ABS(HumanGhost.delta_x) >= ROOM_WIDTH) {
            HumanGhost.delta_x = 0;
            if (HumanGhost.port == PORT_DONE) {
                if (!bot_is_walkable(HumanGhost.xp - 1, HumanGhost.yp, 1)) {
                    humanghost_stop_here();
                    return;
                }
                HumanGhost.xp--;
                if (Maze.Room[HumanGhost.xp - 1][HumanGhost.yp].special == ROOM_SPEC_PORTAL) {
                    HumanGhost.port = PORT_LEFT0;
                }
            } else if (HumanGhost.port == PORT_LEFT0) {
                HumanGhost.xp = ROOM_CNT_X - 2;
                HumanGhost.port = PORT_LEFT1;
            } else {
                HumanGhost.port = PORT_DONE;
                HumanGhost.xp--;
            }
            humanghost_advance_cell();
        }
    } else {
        HumanGhost.next_move = MOVE_STOP;
    }
}

void humanghost_change_skin(uint32_t direction) {
    HumanGhost.skin_cnt++;
    if (HumanGhost.skin_cnt > 7) {
        HumanGhost.skin_cnt = 0;
    }

    if (HumanGhost.status == GHOST_STATUS_DEAD) {
        HumanGhost.skin = GHOST_SKIN_DEAD;
        return;
    }

    if (direction == MOVE_UP) {
        HumanGhost.skin = (HumanGhost.skin_cnt < 4) ? GHOST_SKIN_UP2 : GHOST_SKIN_UP1;
    } else if (direction == MOVE_RIGHT) {
        HumanGhost.skin = (HumanGhost.skin_cnt < 4) ? GHOST_SKIN_RIGHT2 : GHOST_SKIN_RIGHT1;
    } else if (direction == MOVE_DOWN) {
        HumanGhost.skin = (HumanGhost.skin_cnt < 4) ? GHOST_SKIN_DOWN2 : GHOST_SKIN_DOWN1;
    } else if (direction == MOVE_LEFT) {
        HumanGhost.skin = (HumanGhost.skin_cnt < 4) ? GHOST_SKIN_LEFT2 : GHOST_SKIN_LEFT1;
    }

    if (Game.frightened == BOOL_TRUE) {
        if (Game.frightened_timer > GAME_FRIGHTENED_BLINK) {
            HumanGhost.skin = GHOST_SKIN_FRIGHTEN1;
        } else {
            HumanGhost.skin = (HumanGhost.skin_cnt < 4) ? GHOST_SKIN_FRIGHTEN2 : GHOST_SKIN_FRIGHTEN1;
        }
    }
}

void humanghost_check_event(void) {
    bot_ghost_validate_position(&HumanGhost);
    bot_ghost_hit_pacman(HumanGhost.xp, HumanGhost.yp, &HumanGhost);
    bot_ghost_try_revive(&HumanGhost, GHOST_HUMAN);
}

void humanghost_calc_next_move(void) {
    uint32_t door_cnt = 0;
    uint32_t xp = HumanGhost.xp;
    uint32_t yp = HumanGhost.yp;

    if (HumanGhost.move == MOVE_STOP) {
        HumanGhost.next_move = MOVE_STOP;
        return;
    }

    if (HumanGhost.move == MOVE_UP) yp--;
    if (HumanGhost.move == MOVE_RIGHT) xp++;
    if (HumanGhost.move == MOVE_DOWN) yp++;
    if (HumanGhost.move == MOVE_LEFT) xp--;

    if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) door_cnt++;

    if ((HumanGhost.status == GHOST_STATUS_DEAD) && (Maze.Room[xp][yp].special == ROOM_SPEC_GATE)) {
        // when dead and on a gate ignore the door count
        door_cnt = 2;
    }

    if (door_cnt > 1 && HumanGhost.new_mode == 1) {
        HumanGhost.new_mode = 0;
        if (HumanGhost.move == MOVE_UP) HumanGhost.next_move = MOVE_DOWN;
        if (HumanGhost.move == MOVE_RIGHT) HumanGhost.next_move = MOVE_LEFT;
        if (HumanGhost.move == MOVE_DOWN) HumanGhost.next_move = MOVE_UP;
        if (HumanGhost.move == MOVE_LEFT) HumanGhost.next_move = MOVE_RIGHT;
        return;
    }

    if (HumanGhost.status == GHOST_STATUS_DEAD) {
        HumanGhost.next_move = bot_calc_move_dead(GHOST_HUMAN, xp, yp, HumanGhost.move);
        return;
    }

    if (door_cnt == 0) {
        HumanGhost.next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        HumanGhost.next_move = bot_calc_only_exit(xp, yp);
    } else {
        HumanGhost.next_move = bot_calc_move_player_ghost(xp, yp, HumanGhost.move, Game.player2_joy);
    }
}

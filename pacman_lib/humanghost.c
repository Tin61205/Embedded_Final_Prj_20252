//--------------------------------------------------------------
//--------------------------------------------------------------
// Player-controlled ghost (2P Vs Ghost mode)
//--------------------------------------------------------------
#include "humanghost.h"
#include "pacman.h"

void humanghost_check_event(void);
void humanghost_change_skin(uint32_t direction);
void humanghost_calc_next_move(void);

void humanghost_move(void) {
    if (HumanGhost.move == MOVE_STOP && HumanGhost.status == GHOST_STATUS_ALIVE) {
        bot_ghost_unstick(&HumanGhost);
    }
    if (HumanGhost.move == MOVE_STOP) {
        return;
    }

    if (HumanGhost.move == MOVE_UP) {
        HumanGhost.delta_y--;
        humanghost_change_skin(MOVE_UP);
        if (ABS(HumanGhost.delta_y) >= ROOM_HEIGHT) {
            HumanGhost.delta_y = 0;
            if (HumanGhost.port == PORT_DONE) {
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
            humanghost_check_event();
            HumanGhost.move = HumanGhost.next_move;
            humanghost_calc_next_move();
        }
    } else if (HumanGhost.move == MOVE_RIGHT) {
        HumanGhost.delta_x++;
        humanghost_change_skin(MOVE_RIGHT);
        if (ABS(HumanGhost.delta_x) >= ROOM_WIDTH) {
            HumanGhost.delta_x = 0;
            if (HumanGhost.port == PORT_DONE) {
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
            humanghost_check_event();
            HumanGhost.move = HumanGhost.next_move;
            humanghost_calc_next_move();
        }
    } else if (HumanGhost.move == MOVE_DOWN) {
        HumanGhost.delta_y++;
        humanghost_change_skin(MOVE_DOWN);
        if (ABS(HumanGhost.delta_y) >= ROOM_HEIGHT) {
            HumanGhost.delta_y = 0;
            if (HumanGhost.port == PORT_DONE) {
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
            humanghost_check_event();
            HumanGhost.move = HumanGhost.next_move;
            humanghost_calc_next_move();
        }
    } else if (HumanGhost.move == MOVE_LEFT) {
        HumanGhost.delta_x--;
        humanghost_change_skin(MOVE_LEFT);
        if (ABS(HumanGhost.delta_x) >= ROOM_WIDTH) {
            HumanGhost.delta_x = 0;
            if (HumanGhost.port == PORT_DONE) {
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
            humanghost_check_event();
            HumanGhost.move = HumanGhost.next_move;
            humanghost_calc_next_move();
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

    if (door_cnt == 0) {
        HumanGhost.next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        HumanGhost.next_move = bot_calc_only_exit(xp, yp);
    } else {
        if (HumanGhost.new_mode == 1) {
            HumanGhost.new_mode = 0;
            if (HumanGhost.move == MOVE_UP) HumanGhost.next_move = MOVE_DOWN;
            if (HumanGhost.move == MOVE_RIGHT) HumanGhost.next_move = MOVE_LEFT;
            if (HumanGhost.move == MOVE_DOWN) HumanGhost.next_move = MOVE_UP;
            if (HumanGhost.move == MOVE_LEFT) HumanGhost.next_move = MOVE_RIGHT;
            return;
        }

        if (HumanGhost.status == GHOST_STATUS_DEAD) {
            HumanGhost.next_move = bot_calc_move_home(GHOST_HUMAN, xp, yp, HumanGhost.move);
            return;
        }

        HumanGhost.next_move = bot_calc_move_player_ghost(xp, yp, HumanGhost.move, Game.player2_joy);
    }
}

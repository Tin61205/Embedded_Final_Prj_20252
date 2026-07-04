//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "clyde.h"


extern Level_t Level[];

//--------------------------------------------------------------
// internal functions
//--------------------------------------------------------------
void clyde_check_event(void);
void clyde_change_skin(uint32_t direction);
void clyde_calc_next_move(void);

//--------------------------------------------------------------
// init start position
// and set first and second move
//--------------------------------------------------------------
void clyde_init(uint32_t mode) {
    if (mode == GAME_OVER) {
        Clyde.strategy = GHOST_STRATEGY_CLYDE;
    }
    if (Player.level <= GAME_MAX_LEVEL) {
        Clyde.akt_speed_ms = Level[Player.level - 1].clyde_speed;
    } else {
        Clyde.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].clyde_speed;
    }
    Clyde.status = GHOST_STATUS_ALIVE;
    Clyde.xp = CLYDE_START_X;
    Clyde.yp = CLYDE_START_Y;
    Clyde.skin = GHOST_SKIN_LEFT1;
    Clyde.skin_cnt = 0;
    Clyde.delta_x = GHOST_HOME_X_DIFF;
    Clyde.delta_y = GHOST_HOME_Y_DIFF;
    Clyde.move = MOVE_LEFT;
    Clyde.next_move = MOVE_LEFT;
    Clyde.port = PORT_DONE;
    Clyde.dot_cnt = (mode == GAME_OVER) ? 0 : CLYDE_DOT_CNT_MAX;
    Clyde.frightened_buf = CLYDE_FRIGHTENED_BUF;
    Clyde.new_mode = 0;
}

//--------------------------------------------------------------
// move bot one pixel
// if bot enters a new room : look for the next movement
//-------------------------------------------------------------- 
void clyde_move(void) {
    if (Clyde.status == GHOST_STATUS_DEAD) {
        bot_ghost_try_revive(&Clyde, GHOST_CLYDE);
    }
    if (Clyde.status == GHOST_STATUS_ALIVE && Clyde.dot_cnt < CLYDE_DOT_CNT_MAX) return;

    if (Clyde.move == MOVE_STOP) {
        bot_ghost_unstick(&Clyde, GHOST_CLYDE);
    }
    if (Clyde.move == MOVE_STOP) return;

    if (Clyde.move == MOVE_UP) {
        // move one pixel
        Clyde.delta_y--;
        // change skin
        clyde_change_skin(MOVE_UP);
        // check if enter new room
        if (ABS(Clyde.delta_y) >= ROOM_HEIGHT) {
            Clyde.delta_y = 0;
            if (Clyde.port == PORT_DONE) {
                // move to new room
                Clyde.yp--;
                // check if portal
                if (Maze.Room[Clyde.xp][Clyde.yp - 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Clyde.port = PORT_UP0;
                }
            } else if (Clyde.port == PORT_UP0) {
                // port to new room
                Clyde.yp = ROOM_CNT_Y - 2;
                // stop portal
                Clyde.port = PORT_UP1;
            } else {
                // deaktivate portal
                Clyde.port = PORT_DONE;
                // move to new room
                Clyde.yp--;
            }
            // check of new event in this room
            clyde_check_event();
            // set new direction;
            Clyde.move = Clyde.next_move;
            // calc next move
            clyde_calc_next_move();
        }
    } else if (Clyde.move == MOVE_RIGHT) {
        // move one pixel
        Clyde.delta_x++;
        // change skin
        clyde_change_skin(MOVE_RIGHT);
        // check if enter new room
        if (ABS(Clyde.delta_x) >= ROOM_WIDTH) {
            Clyde.delta_x = 0;
            if (Clyde.port == PORT_DONE) {
                // move to new room
                Clyde.xp++;
                // check if portal
                if (Maze.Room[Clyde.xp + 1][Clyde.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Clyde.port = PORT_RIGHT0;
                }
            } else if (Clyde.port == PORT_RIGHT0) {
                // port to new room
                Clyde.xp = 1;
                // stop portal
                Clyde.port = PORT_RIGHT1;
            } else {
                // deaktivate portal
                Clyde.port = PORT_DONE;
                // move to new room
                Clyde.xp++;
            }
            // check of new event in this room
            clyde_check_event();
            // set new direction;
            Clyde.move = Clyde.next_move;
            // calc next move
            clyde_calc_next_move();
        }
    } else if (Clyde.move == MOVE_DOWN) {
        // move one pixel
        Clyde.delta_y++;
        // change skin
        clyde_change_skin(MOVE_DOWN);
        // check if enter new room
        if (ABS(Clyde.delta_y) >= ROOM_HEIGHT) {
            Clyde.delta_y = 0;
            if (Clyde.port == PORT_DONE) {
                // move to new room
                Clyde.yp++;
                // check if portal
                if (Maze.Room[Clyde.xp][Clyde.yp + 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Clyde.port = PORT_DOWN0;
                }
            } else if (Clyde.port == PORT_DOWN0) {
                // port to new room
                Clyde.yp = 1;
                // stop portal
                Clyde.port = PORT_DOWN1;
            } else {
                // deaktivate portal
                Clyde.port = PORT_DONE;
                // move to new room
                Clyde.yp++;
            }
            // check of new event in this room
            clyde_check_event();
            // set new direction;
            Clyde.move = Clyde.next_move;
            // calc next move
            clyde_calc_next_move();
        }
    } else if (Clyde.move == MOVE_LEFT) {
        // move one pixel
        Clyde.delta_x--;
        // change skin
        clyde_change_skin(MOVE_LEFT);
        // check if enter new room
        if (ABS(Clyde.delta_x) >= ROOM_WIDTH) {
            Clyde.delta_x = 0;
            if (Clyde.port == PORT_DONE) {
                // move to new room
                Clyde.xp--;
                // check if portal
                if (Maze.Room[Clyde.xp - 1][Clyde.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Clyde.port = PORT_LEFT0;
                }
            } else if (Clyde.port == PORT_LEFT0) {
                // port to new room
                Clyde.xp = ROOM_CNT_X - 2;
                // stop portal
                Clyde.port = PORT_LEFT1;
            } else {
                // deaktivate portal
                Clyde.port = PORT_DONE;
                // move to new room
                Clyde.xp--;
            }
            // check of new event in this room
            clyde_check_event();
            // set new direction;
            Clyde.move = Clyde.next_move;
            // calc next move
            clyde_calc_next_move();
        }
    } else {
        // no movement (if this happens...error in maze design)
        Clyde.next_move = MOVE_STOP;
    }
}

//--------------------------------------------------------------
// change skin from bot for animation
//--------------------------------------------------------------
void clyde_change_skin(uint32_t direction) {
    Clyde.skin_cnt++;
    if (Clyde.skin_cnt > 7) Clyde.skin_cnt = 0;

    if (Clyde.status == GHOST_STATUS_DEAD) {
        Clyde.skin = GHOST_SKIN_DEAD;
        return;
    }

    if (direction == MOVE_UP) {
        if (Clyde.skin_cnt < 4) {
            Clyde.skin = GHOST_SKIN_UP2;
        } else {
            Clyde.skin = GHOST_SKIN_UP1;
        }
    } else if (direction == MOVE_RIGHT) {
        if (Clyde.skin_cnt < 4) {
            Clyde.skin = GHOST_SKIN_RIGHT2;
        } else {
            Clyde.skin = GHOST_SKIN_RIGHT1;
        }
    } else if (direction == MOVE_DOWN) {
        if (Clyde.skin_cnt < 4) {
            Clyde.skin = GHOST_SKIN_DOWN2;
        } else {
            Clyde.skin = GHOST_SKIN_DOWN1;
        }
    } else if (direction == MOVE_LEFT) {
        if (Clyde.skin_cnt < 4) {
            Clyde.skin = GHOST_SKIN_LEFT2;
        } else {
            Clyde.skin = GHOST_SKIN_LEFT1;
        }
    }

    if (Game.frightened == BOOL_TRUE) {
        if (Game.frightened_timer > GAME_FRIGHTENED_BLINK) {
            Clyde.skin = GHOST_SKIN_FRIGHTEN1;
        } else {
            if (Clyde.skin_cnt < 4) {
                Clyde.skin = GHOST_SKIN_FRIGHTEN2;
            } else {
                Clyde.skin = GHOST_SKIN_FRIGHTEN1;
            }
        }
    }
}

//--------------------------------------------------------------
// check of event in the new room
//--------------------------------------------------------------
void clyde_check_event(void) {
    uint32_t xp, yp;

    bot_ghost_validate_position(&Clyde);
    xp = Clyde.xp;
    yp = Clyde.yp;

    // check if contact
    bot_ghost_hit_pacman(xp, yp, &Clyde);

    bot_ghost_try_revive(&Clyde, GHOST_CLYDE);
}

//--------------------------------------------------------------
// calc next move from bot
//--------------------------------------------------------------
void clyde_calc_next_move(void) {
    uint32_t door_cnt = 0;
    uint32_t xp, yp;

    // actual position
    xp = Clyde.xp;
    yp = Clyde.yp;

    if (Clyde.move == MOVE_STOP) {
        // (if this happens...error in maze design)
        Clyde.next_move = MOVE_STOP;
        return;
    }
    // calc the next room the bot enters
    if (Clyde.move == MOVE_UP) yp--;
    if (Clyde.move == MOVE_RIGHT) xp++;
    if (Clyde.move == MOVE_DOWN) yp++;
    if (Clyde.move == MOVE_LEFT) xp--;

    // count the doors in this room
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) door_cnt++;

    if ((Clyde.status == GHOST_STATUS_DEAD) && (Maze.Room[xp][yp].special == ROOM_SPEC_GATE)) {
        // when dead and on a gate ignore the door count
        door_cnt = 2;
    }

    if (door_cnt > 1 && Clyde.new_mode == 1) {
        Clyde.new_mode = 0;
        if (Clyde.move == MOVE_UP) Clyde.next_move = MOVE_DOWN;
        if (Clyde.move == MOVE_RIGHT) Clyde.next_move = MOVE_LEFT;
        if (Clyde.move == MOVE_DOWN) Clyde.next_move = MOVE_UP;
        if (Clyde.move == MOVE_LEFT) Clyde.next_move = MOVE_RIGHT;
        return;
    }

    if (Clyde.status == GHOST_STATUS_DEAD) {
        Clyde.next_move = bot_calc_move_dead(GHOST_CLYDE, xp, yp, Clyde.move);
        return;
    }

    // choose a way
    if (door_cnt == 0) {
        Clyde.next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        Clyde.next_move = bot_calc_only_exit(xp, yp);
    } else {
        if (Game.frightened == BOOL_TRUE) {
            // frightened (make a random move)
            Clyde.next_move = bot_calc_move_random(xp, yp, Clyde.move);
            return;
        }

        Clyde.next_move = bot_calc_move_by_strategy(GHOST_CLYDE, Clyde.strategy, xp, yp, Clyde.move);
    }
}
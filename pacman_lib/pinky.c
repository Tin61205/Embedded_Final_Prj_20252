#include "pinky.h"


extern Level_t Level[];

void pinky_check_event(void);
void pinky_change_skin(uint32_t direction);
void pinky_calc_next_move(void);

//--------------------------------------------------------------
// init start position
// and set first and second move
//--------------------------------------------------------------
void pinky_init(uint32_t mode) {
    if (mode == GAME_OVER) {
        Pinky.strategy = GHOST_STRATEGY_PINKY;
    }
    if (Player.level <= GAME_MAX_LEVEL) {
        Pinky.akt_speed_ms = Level[Player.level - 1].pinky_speed;
    } else {
        Pinky.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].pinky_speed;
    }
    Pinky.status = GHOST_STATUS_ALIVE;
    Pinky.xp = PINKY_START_X;
    Pinky.yp = PINKY_START_Y;
    Pinky.skin = GHOST_SKIN_UP1;
    Pinky.skin_cnt = 0;
    Pinky.delta_x = GHOST_HOME_X_DIFF;
    Pinky.delta_y = GHOST_HOME_Y_DIFF;
    Pinky.move = MOVE_UP;
    Pinky.next_move = MOVE_UP;
    Pinky.port = PORT_DONE;
    Pinky.dot_cnt = 0;
    Pinky.frightened_buf = PINKY_FRIGHTENED_BUF;
    Pinky.new_mode = 0;
}

//--------------------------------------------------------------
// move bot one pixel
// if bot enters a new room : look for the next movement
//-------------------------------------------------------------- 
void pinky_move(void) {
    if (Pinky.dot_cnt < PINKY_DOT_CNT_MAX) return;

    if (Pinky.move == MOVE_UP) {
        // move one pixel
        Pinky.delta_y--;
        // change skin
        pinky_change_skin(MOVE_UP);
        // check if enter new room
        if (ABS(Pinky.delta_y) >= ROOM_HEIGHT) {
            Pinky.delta_y = 0;
            if (Pinky.port == PORT_DONE) {
                // move to new room
                Pinky.yp--;
                // check if portal
                if (Maze.Room[Pinky.xp][Pinky.yp - 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Pinky.port = PORT_UP0;
                }
            } else if (Pinky.port == PORT_UP0) {
                // port to new room
                Pinky.yp = ROOM_CNT_Y - 2;
                // stop portal
                Pinky.port = PORT_UP1;
            } else {
                // deaktivate portal
                Pinky.port = PORT_DONE;
                // move to new room
                Pinky.yp--;
            }
            // check of new event in this room
            pinky_check_event();
            // set new direction;
            Pinky.move = Pinky.next_move;
            // calc next move
            pinky_calc_next_move();
        }
    } else if (Pinky.move == MOVE_RIGHT) {
        // move one pixel
        Pinky.delta_x++;
        // change skin
        pinky_change_skin(MOVE_RIGHT);
        // check if enter new room
        if (ABS(Pinky.delta_x) >= ROOM_WIDTH) {
            Pinky.delta_x = 0;
            if (Pinky.port == PORT_DONE) {
                // move to new room
                Pinky.xp++;
                // check if portal
                if (Maze.Room[Pinky.xp + 1][Pinky.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Pinky.port = PORT_RIGHT0;
                }
            } else if (Pinky.port == PORT_RIGHT0) {
                // port to new room
                Pinky.xp = 1;
                // stop portal
                Pinky.port = PORT_RIGHT1;
            } else {
                // deaktivate portal
                Pinky.port = PORT_DONE;
                // move to new room
                Pinky.xp++;
            }
            // check of new event in this room
            pinky_check_event();
            // set new direction;
            Pinky.move = Pinky.next_move;
            // calc next move
            pinky_calc_next_move();
        }
    } else if (Pinky.move == MOVE_DOWN) {
        // move one pixel
        Pinky.delta_y++;
        // change skin
        pinky_change_skin(MOVE_DOWN);
        // check if enter new room
        if (ABS(Pinky.delta_y) >= ROOM_HEIGHT) {
            Pinky.delta_y = 0;
            if (Pinky.port == PORT_DONE) {
                // move to new room
                Pinky.yp++;
                // check if portal
                if (Maze.Room[Pinky.xp][Pinky.yp + 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Pinky.port = PORT_DOWN0;
                }
            } else if (Pinky.port == PORT_DOWN0) {
                // port to new room
                Pinky.yp = 1;
                // stop portal
                Pinky.port = PORT_DOWN1;
            } else {
                // deaktivate portal
                Pinky.port = PORT_DONE;
                // move to new room
                Pinky.yp++;
            }
            // check of new event in this room
            pinky_check_event();
            // set new direction;
            Pinky.move = Pinky.next_move;
            // calc next move
            pinky_calc_next_move();
        }
    } else if (Pinky.move == MOVE_LEFT) {
        // move one pixel
        Pinky.delta_x--;
        // change skin
        pinky_change_skin(MOVE_LEFT);
        // check if enter new room
        if (ABS(Pinky.delta_x) >= ROOM_WIDTH) {
            Pinky.delta_x = 0;
            if (Pinky.port == PORT_DONE) {
                // move to new room
                Pinky.xp--;
                // check if portal
                if (Maze.Room[Pinky.xp - 1][Pinky.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Pinky.port = PORT_LEFT0;
                }
            } else if (Pinky.port == PORT_LEFT0) {
                // port to new room
                Pinky.xp = ROOM_CNT_X - 2;
                // stop portal
                Pinky.port = PORT_LEFT1;
            } else {
                // deaktivate portal
                Pinky.port = PORT_DONE;
                // move to new room
                Pinky.xp--;
            }
            // check of new event in this room
            pinky_check_event();
            // set new direction;
            Pinky.move = Pinky.next_move;
            // calc next move
            pinky_calc_next_move();
        }
    } else {
        // no movement (if this happens...error in maze design)
        Pinky.next_move = MOVE_STOP;
    }
}

//--------------------------------------------------------------
// change skin from bot for animation
//--------------------------------------------------------------
void pinky_change_skin(uint32_t direction) {
    Pinky.skin_cnt++;
    if (Pinky.skin_cnt > 7) Pinky.skin_cnt = 0;

    if (Pinky.status == GHOST_STATUS_DEAD) {
        Pinky.skin = GHOST_SKIN_DEAD;
        return;
    }

    if (direction == MOVE_UP) {
        if (Pinky.skin_cnt < 4) {
            Pinky.skin = GHOST_SKIN_UP2;
        } else {
            Pinky.skin = GHOST_SKIN_UP1;
        }
    } else if (direction == MOVE_RIGHT) {
        if (Pinky.skin_cnt < 4) {
            Pinky.skin = GHOST_SKIN_RIGHT2;
        } else {
            Pinky.skin = GHOST_SKIN_RIGHT1;
        }
    } else if (direction == MOVE_DOWN) {
        if (Pinky.skin_cnt < 4) {
            Pinky.skin = GHOST_SKIN_DOWN2;
        } else {
            Pinky.skin = GHOST_SKIN_DOWN1;
        }
    } else if (direction == MOVE_LEFT) {
        if (Pinky.skin_cnt < 4) {
            Pinky.skin = GHOST_SKIN_LEFT2;
        } else {
            Pinky.skin = GHOST_SKIN_LEFT1;
        }
    }

    if (Game.frightened == BOOL_TRUE) {
        if (Game.frightened_timer > GAME_FRIGHTENED_BLINK) {
            Pinky.skin = GHOST_SKIN_FRIGHTEN1;
        } else {
            if (Pinky.skin_cnt < 4) {
                Pinky.skin = GHOST_SKIN_FRIGHTEN2;
            } else {
                Pinky.skin = GHOST_SKIN_FRIGHTEN1;
            }
        }
    }
}

//--------------------------------------------------------------
// check of event in the new room
//--------------------------------------------------------------
void pinky_check_event(void) {
    uint32_t xp, yp;
    uint32_t pxp, pyp;

    xp = Pinky.xp;
    yp = Pinky.yp;

    // check if contact
    if ((Game.collision == BOOL_TRUE) && (Pinky.status == GHOST_STATUS_ALIVE)) {
        pxp = Player.xp;
        pyp = Player.yp;
        if ((xp == pxp) && (yp == pyp)) {
            if (Game.frightened == BOOL_FALSE) {
                Player.status = PLAYER_STATUS_DEAD;
            } else {
                Pinky.status = GHOST_STATUS_DEAD;
            }
            GUI.refresh_value = GUI_REFRESH_VALUE;
        }
    }

    // check if home position after dead
    if (Pinky.status == GHOST_STATUS_DEAD) {
        if ((xp == PINKY_HOME_X) && (yp == PINKY_HOME_Y)) {
            Pinky.status = GHOST_STATUS_ALIVE;
            Pinky.skin = GHOST_SKIN_UP1;
            Pinky.delta_x = GHOST_HOME_X_DIFF;
            Pinky.delta_y = GHOST_HOME_Y_DIFF;
            Pinky.move = MOVE_UP;
            Pinky.next_move = MOVE_UP;
            Pinky.dot_cnt = 0;
        }
    }
}

//--------------------------------------------------------------
// calc next move from bot
//--------------------------------------------------------------
void pinky_calc_next_move(void) {
    uint32_t door_cnt = 0;
    uint32_t xp, yp;

    // actual position
    xp = Pinky.xp;
    yp = Pinky.yp;

    if (Pinky.move == MOVE_STOP) {
        // (if this happens...error in maze design)
        Pinky.next_move = MOVE_STOP;
        return;
    }
    // calc the next room the bot enters
    if (Pinky.move == MOVE_UP) yp--;
    if (Pinky.move == MOVE_RIGHT) xp++;
    if (Pinky.move == MOVE_DOWN) yp++;
    if (Pinky.move == MOVE_LEFT) xp--;

    // count the doors in this room
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) door_cnt++;

    if ((Pinky.status == GHOST_STATUS_DEAD) && (Maze.Room[xp][yp].special == ROOM_SPEC_GATE)) {
        // when dead and on a gate ignore the door count
        door_cnt = 2;
    }

    // choose a way
    if (door_cnt == 0) {
        // (if this happens...error in maze design)
        Pinky.next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        // take the only possible way
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) Pinky.next_move = MOVE_UP;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) Pinky.next_move = MOVE_RIGHT;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) Pinky.next_move = MOVE_DOWN;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) Pinky.next_move = MOVE_LEFT;
    } else {
        // more than one possible way
        if (Pinky.new_mode == 1) {
            // mode has changed
            Pinky.new_mode = 0;
            // revers direction
            if (Pinky.move == MOVE_UP) Pinky.next_move = MOVE_DOWN;
            if (Pinky.move == MOVE_RIGHT) Pinky.next_move = MOVE_LEFT;
            if (Pinky.move == MOVE_DOWN) Pinky.next_move = MOVE_UP;
            if (Pinky.move == MOVE_LEFT) Pinky.next_move = MOVE_RIGHT;
            return;
        }

        if (Pinky.status == GHOST_STATUS_DEAD) {
            // dead (return to home)
            if (Maze.Room[xp][yp].special == ROOM_SPEC_GATE) {
                // found entry gate
                if ((Maze.Room[xp][yp].door & ROOM_PGATE_U) != 0) Pinky.next_move = MOVE_UP;
                if ((Maze.Room[xp][yp].door & ROOM_PGATE_R) != 0) Pinky.next_move = MOVE_RIGHT;
                if ((Maze.Room[xp][yp].door & ROOM_PGATE_D) != 0) Pinky.next_move = MOVE_DOWN;
                if ((Maze.Room[xp][yp].door & ROOM_PGATE_L) != 0) Pinky.next_move = MOVE_LEFT;
            } else {
                Pinky.next_move = bot_calc_move_home(GHOST_PINKY, xp, yp, Pinky.move);
            }
            return;
        }

        if (Game.frightened == BOOL_TRUE) {
            // frightened (make a random move)
            Pinky.next_move = bot_calc_move_random(xp, yp, Pinky.move);
            return;
        }

        if (Game.mode == GAME_MODE_CHASE) {
            // chase
            if (Pinky.strategy == GHOST_STRATEGY_RANDOM) {
                Pinky.next_move = bot_calc_move_random(xp, yp, Pinky.move);
            } else if (Pinky.strategy == GHOST_STRATEGY_BLINKY) {
                Pinky.next_move = bot_calc_move_blinky(xp, yp, Pinky.move);
            } else if (Pinky.strategy == GHOST_STRATEGY_PINKY) {
                Pinky.next_move = bot_calc_move_pinky(xp, yp, Pinky.move);
            } else if (Pinky.strategy == GHOST_STRATEGY_INKY) {
                Pinky.next_move = bot_calc_move_inky(xp, yp, Pinky.move);
            } else {
                Pinky.next_move = bot_calc_move_clyde(GHOST_PINKY, xp, yp, Pinky.move);
            }
        } else {
            // scatter
            Pinky.next_move = bot_calc_move_scatter(GHOST_PINKY, xp, yp, Pinky.move);
        }
    }
}
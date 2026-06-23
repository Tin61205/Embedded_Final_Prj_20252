//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "inky.h"


extern Level_t Level[];

//--------------------------------------------------------------
// interne Funktionen
//--------------------------------------------------------------
void inky_check_event(void);
void inky_change_skin(uint32_t direction);
void inky_calc_next_move(void);

//--------------------------------------------------------------
// init start position
// and set first and second move
//--------------------------------------------------------------
void inky_init(uint32_t mode) {
    if (mode == GAME_OVER) {
        Inky.strategy = GHOST_STRATEGY_INKY;
    }
    if (Player.level <= GAME_MAX_LEVEL) {
        Inky.akt_speed_ms = Level[Player.level - 1].inky_speed;
    } else {
        Inky.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].inky_speed;
    }
    Inky.status = GHOST_STATUS_ALIVE;
    Inky.xp = INKY_START_X;
    Inky.yp = INKY_START_Y;
    Inky.skin = GHOST_SKIN_RIGHT1;
    Inky.skin_cnt = 0;
    Inky.delta_x = GHOST_HOME_X_DIFF;
    Inky.delta_y = GHOST_HOME_Y_DIFF;
    Inky.move = MOVE_RIGHT;
    Inky.next_move = MOVE_RIGHT;
    Inky.port = PORT_DONE;
    Inky.dot_cnt = 0;
    Inky.frightened_buf = INKY_FRIGHTENED_BUF;
    Inky.new_mode = 0;
}

//--------------------------------------------------------------
// move bot one pixel
// if bot enters a new room : look for the next movement
//-------------------------------------------------------------- 
void inky_move(void) {
    if (Inky.dot_cnt < INKY_DOT_CNT_MAX) return;

    if (Inky.move == MOVE_UP) {
        // move one pixel
        Inky.delta_y--;
        // change skin
        inky_change_skin(MOVE_UP);
        // check if enter new room
        if (ABS(Inky.delta_y) >= ROOM_HEIGHT) {
            Inky.delta_y = 0;
            if (Inky.port == PORT_DONE) {
                // move to new room
                Inky.yp--;
                // check if portal
                if (Maze.Room[Inky.xp][Inky.yp - 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Inky.port = PORT_UP0;
                }
            } else if (Inky.port == PORT_UP0) {
                // port to new room
                Inky.yp = ROOM_CNT_Y - 2;
                // stop portal
                Inky.port = PORT_UP1;
            } else {
                // deaktivate portal
                Inky.port = PORT_DONE;
                // move to new room
                Inky.yp--;
            }
            // check of new event in this room
            inky_check_event();
            // set new direction;
            Inky.move = Inky.next_move;
            // calc next move
            inky_calc_next_move();
        }
    } else if (Inky.move == MOVE_RIGHT) {
        // move one pixel
        Inky.delta_x++;
        // change skin
        inky_change_skin(MOVE_RIGHT);
        // check if enter new room
        if (ABS(Inky.delta_x) >= ROOM_WIDTH) {
            Inky.delta_x = 0;
            if (Inky.port == PORT_DONE) {
                // move to new room
                Inky.xp++;
                // check if portal
                if (Maze.Room[Inky.xp + 1][Inky.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Inky.port = PORT_RIGHT0;
                }
            } else if (Inky.port == PORT_RIGHT0) {
                // port to new room
                Inky.xp = 1;
                // stop portal
                Inky.port = PORT_RIGHT1;
            } else {
                // deaktivate portal
                Inky.port = PORT_DONE;
                // move to new room
                Inky.xp++;
            }
            // check of new event in this room
            inky_check_event();
            // set new direction;
            Inky.move = Inky.next_move;
            // calc next move
            inky_calc_next_move();
        }
    } else if (Inky.move == MOVE_DOWN) {
        // move one pixel
        Inky.delta_y++;
        // change skin
        inky_change_skin(MOVE_DOWN);
        // check if enter new room
        if (ABS(Inky.delta_y) >= ROOM_HEIGHT) {
            Inky.delta_y = 0;
            if (Inky.port == PORT_DONE) {
                // move to new room
                Inky.yp++;
                // check if portal
                if (Maze.Room[Inky.xp][Inky.yp + 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Inky.port = PORT_DOWN0;
                }
            } else if (Inky.port == PORT_DOWN0) {
                // port to new room
                Inky.yp = 1;
                // stop portal
                Inky.port = PORT_DOWN1;
            } else {
                // deaktivate portal
                Inky.port = PORT_DONE;
                // move to new room
                Inky.yp++;
            }
            // check of new event in this room
            inky_check_event();
            // set new direction;
            Inky.move = Inky.next_move;
            // calc next move
            inky_calc_next_move();
        }
    } else if (Inky.move == MOVE_LEFT) {
        // move one pixel
        Inky.delta_x--;
        // change skin
        inky_change_skin(MOVE_LEFT);
        // check if enter new room
        if (ABS(Inky.delta_x) >= ROOM_WIDTH) {
            Inky.delta_x = 0;
            if (Inky.port == PORT_DONE) {
                // move to new room
                Inky.xp--;
                // check if portal
                if (Maze.Room[Inky.xp - 1][Inky.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Inky.port = PORT_LEFT0;
                }
            } else if (Inky.port == PORT_LEFT0) {
                // port to new room
                Inky.xp = ROOM_CNT_X - 2;
                // stop portal
                Inky.port = PORT_LEFT1;
            } else {
                // deaktivate portal
                Inky.port = PORT_DONE;
                // move to new room
                Inky.xp--;
            }
            // check of new event in this room
            inky_check_event();
            // set new direction;
            Inky.move = Inky.next_move;
            // calc next move
            inky_calc_next_move();
        }
    } else {
        // no movement (if this happens...error in maze design)
        Inky.next_move = MOVE_STOP;
    }
}

//--------------------------------------------------------------
// change skin from bot for animation
//--------------------------------------------------------------
void inky_change_skin(uint32_t direction) {
    Inky.skin_cnt++;
    if (Inky.skin_cnt > 7) Inky.skin_cnt = 0;

    if (Inky.status == GHOST_STATUS_DEAD) {
        Inky.skin = GHOST_SKIN_DEAD;
        return;
    }

    if (direction == MOVE_UP) {
        if (Inky.skin_cnt < 4) {
            Inky.skin = GHOST_SKIN_UP2;
        } else {
            Inky.skin = GHOST_SKIN_UP1;
        }
    } else if (direction == MOVE_RIGHT) {
        if (Inky.skin_cnt < 4) {
            Inky.skin = GHOST_SKIN_RIGHT2;
        } else {
            Inky.skin = GHOST_SKIN_RIGHT1;
        }
    } else if (direction == MOVE_DOWN) {
        if (Inky.skin_cnt < 4) {
            Inky.skin = GHOST_SKIN_DOWN2;
        } else {
            Inky.skin = GHOST_SKIN_DOWN1;
        }
    } else if (direction == MOVE_LEFT) {
        if (Inky.skin_cnt < 4) {
            Inky.skin = GHOST_SKIN_LEFT2;
        } else {
            Inky.skin = GHOST_SKIN_LEFT1;
        }
    }

    if (Game.frightened == BOOL_TRUE) {
        if (Game.frightened_timer > GAME_FRIGHTENED_BLINK) {
            Inky.skin = GHOST_SKIN_FRIGHTEN1;
        } else {
            if (Inky.skin_cnt < 4) {
                Inky.skin = GHOST_SKIN_FRIGHTEN2;
            } else {
                Inky.skin = GHOST_SKIN_FRIGHTEN1;
            }
        }
    }
}

//--------------------------------------------------------------
// check of event in the new room
//--------------------------------------------------------------
void inky_check_event(void) {
    uint32_t xp, yp;
    uint32_t pxp, pyp;

    xp = Inky.xp;
    yp = Inky.yp;

    // check if contact
    if ((Game.collision == BOOL_TRUE) && (Inky.status == GHOST_STATUS_ALIVE)) {
        pxp = Player.xp;
        pyp = Player.yp;
        if ((xp == pxp) && (yp == pyp)) {
            if (Game.frightened == BOOL_FALSE) {
                Player.status = PLAYER_STATUS_DEAD;
            } else {
                Inky.status = GHOST_STATUS_DEAD;
            }
            GUI.refresh_value = GUI_REFRESH_VALUE;
        }
    }

    // check if home position after dead
    if (Inky.status == GHOST_STATUS_DEAD) {
        if ((xp == INKY_HOME_X) && (yp == INKY_HOME_Y)) {
            Inky.status = GHOST_STATUS_ALIVE;
            Inky.skin = GHOST_SKIN_RIGHT1;
            Inky.delta_x = GHOST_HOME_X_DIFF;
            Inky.delta_y = GHOST_HOME_Y_DIFF;
            Inky.move = MOVE_RIGHT;
            Inky.next_move = MOVE_RIGHT;
            Inky.dot_cnt = 0;
        }
    }
}

//--------------------------------------------------------------
// calc next move from bot
//--------------------------------------------------------------
void inky_calc_next_move(void) {
    uint32_t door_cnt = 0;
    uint32_t xp, yp;

    // actual position
    xp = Inky.xp;
    yp = Inky.yp;

    if (Inky.move == MOVE_STOP) {
        // (if this happens...error in maze design)
        Inky.next_move = MOVE_STOP;
        return;
    }
    // calc the next room the bot enters
    if (Inky.move == MOVE_UP) yp--;
    if (Inky.move == MOVE_RIGHT) xp++;
    if (Inky.move == MOVE_DOWN) yp++;
    if (Inky.move == MOVE_LEFT) xp--;

    // count the doors in this room
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) door_cnt++;

    if ((Inky.status == GHOST_STATUS_DEAD) && (Maze.Room[xp][yp].special == ROOM_SPEC_GATE)) {
        // when dead and on a gate ignore the door count
        door_cnt = 2;
    }

    // choose a way
    if (door_cnt == 0) {
        // (if this happens...error in maze design)
        Inky.next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        // take the only possible way
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) Inky.next_move = MOVE_UP;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) Inky.next_move = MOVE_RIGHT;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) Inky.next_move = MOVE_DOWN;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) Inky.next_move = MOVE_LEFT;
    } else {
        // more than one possible way
        if (Inky.new_mode == 1) {
            // mode has changed
            Inky.new_mode = 0;
            // revers direction
            if (Inky.move == MOVE_UP) Inky.next_move = MOVE_DOWN;
            if (Inky.move == MOVE_RIGHT) Inky.next_move = MOVE_LEFT;
            if (Inky.move == MOVE_DOWN) Inky.next_move = MOVE_UP;
            if (Inky.move == MOVE_LEFT) Inky.next_move = MOVE_RIGHT;
            return;
        }

        if (Inky.status == GHOST_STATUS_DEAD) {
            // dead (return to home)
            if (Maze.Room[xp][yp].special == ROOM_SPEC_GATE) {
                // found entry gate
                if ((Maze.Room[xp][yp].door & ROOM_IGATE_U) != 0) Inky.next_move = MOVE_UP;
                if ((Maze.Room[xp][yp].door & ROOM_IGATE_R) != 0) Inky.next_move = MOVE_RIGHT;
                if ((Maze.Room[xp][yp].door & ROOM_IGATE_D) != 0) Inky.next_move = MOVE_DOWN;
                if ((Maze.Room[xp][yp].door & ROOM_IGATE_L) != 0) Inky.next_move = MOVE_LEFT;
            } else {
                Inky.next_move = bot_calc_move_home(GHOST_INKY, xp, yp, Inky.move);
            }
            return;
        }

        if (Game.frightened == BOOL_TRUE) {
            // frightened (make a random move)
            Inky.next_move = bot_calc_move_random(xp, yp, Inky.move);
            return;
        }

        if (Game.mode == GAME_MODE_CHASE) {
            // chase
            if (Inky.strategy == GHOST_STRATEGY_RANDOM) {
                Inky.next_move = bot_calc_move_random(xp, yp, Inky.move);
            } else if (Inky.strategy == GHOST_STRATEGY_BLINKY) {
                Inky.next_move = bot_calc_move_blinky(xp, yp, Inky.move);
            } else if (Inky.strategy == GHOST_STRATEGY_PINKY) {
                Inky.next_move = bot_calc_move_pinky(xp, yp, Inky.move);
            } else if (Inky.strategy == GHOST_STRATEGY_INKY) {
                Inky.next_move = bot_calc_move_inky(xp, yp, Inky.move);
            } else {
                Inky.next_move = bot_calc_move_clyde(GHOST_INKY, xp, yp, Inky.move);
            }
        } else {
            // scatter
            Inky.next_move = bot_calc_move_scatter(GHOST_INKY, xp, yp, Inky.move);
        }
    }
}
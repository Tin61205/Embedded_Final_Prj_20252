//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "blinky.h"


extern Level_t Level[];

void blinky_check_event(void);
void blinky_change_skin(uint32_t direction);
void blinky_calc_next_move(void);

//--------------------------------------------------------------
// init start position
// and set first and second move
//--------------------------------------------------------------
void blinky_init(uint32_t mode) {
    if (mode == GAME_OVER) {
        Blinky.strategy = GHOST_STRATEGY_BLINKY;
    }
    if (Player.level <= GAME_MAX_LEVEL) {
        Blinky.akt_speed_ms = Level[Player.level - 1].blinky_speed;
    } else {
        Blinky.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].blinky_speed;
    }
    Blinky.status = GHOST_STATUS_ALIVE;
    Blinky.xp = BLINKY_START_X;
    Blinky.yp = BLINKY_START_Y;
    Blinky.skin = GHOST_SKIN_LEFT1;
    Blinky.skin_cnt = 0;
    Blinky.delta_x = 0;
    Blinky.delta_y = 0;
    Blinky.move = MOVE_LEFT;
    Blinky.next_move = MOVE_LEFT;
    Blinky.port = PORT_DONE;
    Blinky.dot_cnt = 0;
    Blinky.frightened_buf = BLINKY_FRIGHTENED_BUF;
    Blinky.new_mode = 0;
}

//--------------------------------------------------------------
// move bot one pixel
// if bot enters a new room : look for the next movement
//-------------------------------------------------------------- 
void blinky_move(void) {
    if (Blinky.dot_cnt < BLINKY_DOT_CNT_MAX) return;

    if (Blinky.move == MOVE_UP) {
        // move one pixel
        Blinky.delta_y--;
        // change skin
        blinky_change_skin(MOVE_UP);
        // check if enter new room
        if (ABS(Blinky.delta_y) >= ROOM_HEIGHT) {
            Blinky.delta_y = 0;
            if (Blinky.port == PORT_DONE) {
                // move to new room
                Blinky.yp--;
                // check if portal
                if (Maze.Room[Blinky.xp][Blinky.yp - 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Blinky.port = PORT_UP0;
                }
            } else if (Blinky.port == PORT_UP0) {
                // port to new room
                Blinky.yp = ROOM_CNT_Y - 2;
                // stop portal
                Blinky.port = PORT_UP1;
            } else {
                // deaktivate portal
                Blinky.port = PORT_DONE;
                // move to new room
                Blinky.yp--;
            }
            // check of new event in this room
            blinky_check_event();
            // set new direction;
            Blinky.move = Blinky.next_move;
            // calc next move
            blinky_calc_next_move();
        }
    } else if (Blinky.move == MOVE_RIGHT) {
        // move one pixel
        Blinky.delta_x++;
        // change skin
        blinky_change_skin(MOVE_RIGHT);
        // check if enter new room
        if (ABS(Blinky.delta_x) >= ROOM_WIDTH) {
            Blinky.delta_x = 0;
            if (Blinky.port == PORT_DONE) {
                // move to new room
                Blinky.xp++;
                // check if portal
                if (Maze.Room[Blinky.xp + 1][Blinky.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Blinky.port = PORT_RIGHT0;
                }
            } else if (Blinky.port == PORT_RIGHT0) {
                // port to new room
                Blinky.xp = 1;
                // stop portal
                Blinky.port = PORT_RIGHT1;
            } else {
                // deaktivate portal
                Blinky.port = PORT_DONE;
                // move to new room
                Blinky.xp++;
            }
            // check of new event in this room
            blinky_check_event();
            // set new direction;
            Blinky.move = Blinky.next_move;
            // calc next move
            blinky_calc_next_move();
        }
    } else if (Blinky.move == MOVE_DOWN) {
        // move one pixel
        Blinky.delta_y++;
        // change skin
        blinky_change_skin(MOVE_DOWN);
        // check if enter new room
        if (ABS(Blinky.delta_y) >= ROOM_HEIGHT) {
            Blinky.delta_y = 0;
            if (Blinky.port == PORT_DONE) {
                // move to new room
                Blinky.yp++;
                // check if portal
                if (Maze.Room[Blinky.xp][Blinky.yp + 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Blinky.port = PORT_DOWN0;
                }
            } else if (Blinky.port == PORT_DOWN0) {
                // port to new room
                Blinky.yp = 1;
                // stop portal
                Blinky.port = PORT_DOWN1;
            } else {
                // deaktivate portal
                Blinky.port = PORT_DONE;
                // move to new room
                Blinky.yp++;
            }
            // check of new event in this room
            blinky_check_event();
            // set new direction;
            Blinky.move = Blinky.next_move;
            // calc next move
            blinky_calc_next_move();
        }
    } else if (Blinky.move == MOVE_LEFT) {
        // move one pixel
        Blinky.delta_x--;
        // change skin
        blinky_change_skin(MOVE_LEFT);
        // check if enter new room
        if (ABS(Blinky.delta_x) >= ROOM_WIDTH) {
            Blinky.delta_x = 0;
            if (Blinky.port == PORT_DONE) {
                // move to new room
                Blinky.xp--;
                // check if portal
                if (Maze.Room[Blinky.xp - 1][Blinky.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Blinky.port = PORT_LEFT0;
                }
            } else if (Blinky.port == PORT_LEFT0) {
                // port to new room
                Blinky.xp = ROOM_CNT_X - 2;
                // stop portal
                Blinky.port = PORT_LEFT1;
            } else {
                // deaktivate portal
                Blinky.port = PORT_DONE;
                // move to new room
                Blinky.xp--;
            }
            // check of new event in this room
            blinky_check_event();
            // set new direction;
            Blinky.move = Blinky.next_move;
            // calc next move
            blinky_calc_next_move();
        }
    } else {
        // no movement (if this happens...error in maze design)
        Blinky.next_move = MOVE_STOP;
    }
}

//--------------------------------------------------------------
// change skin from bot for animation
//--------------------------------------------------------------
void blinky_change_skin(uint32_t direction) {
    Blinky.skin_cnt++;
    if (Blinky.skin_cnt > 7) Blinky.skin_cnt = 0;

    if (Blinky.status == GHOST_STATUS_DEAD) {
        Blinky.skin = GHOST_SKIN_DEAD;
        return;
    }

    if (direction == MOVE_UP) {
        if (Blinky.skin_cnt < 4) {
            Blinky.skin = GHOST_SKIN_UP2;
        } else {
            Blinky.skin = GHOST_SKIN_UP1;
        }
    } else if (direction == MOVE_RIGHT) {
        if (Blinky.skin_cnt < 4) {
            Blinky.skin = GHOST_SKIN_RIGHT2;
        } else {
            Blinky.skin = GHOST_SKIN_RIGHT1;
        }
    } else if (direction == MOVE_DOWN) {
        if (Blinky.skin_cnt < 4) {
            Blinky.skin = GHOST_SKIN_DOWN2;
        } else {
            Blinky.skin = GHOST_SKIN_DOWN1;
        }
    } else if (direction == MOVE_LEFT) {
        if (Blinky.skin_cnt < 4) {
            Blinky.skin = GHOST_SKIN_LEFT2;
        } else {
            Blinky.skin = GHOST_SKIN_LEFT1;
        }
    }

    if (Game.frightened == BOOL_TRUE) {
        if (Game.frightened_timer > GAME_FRIGHTENED_BLINK) {
            Blinky.skin = GHOST_SKIN_FRIGHTEN1;
        } else {
            if (Blinky.skin_cnt < 4) {
                Blinky.skin = GHOST_SKIN_FRIGHTEN2;
            } else {
                Blinky.skin = GHOST_SKIN_FRIGHTEN1;
            }
        }
    }
}

//--------------------------------------------------------------
// check of event in the new room
//--------------------------------------------------------------
void blinky_check_event(void) {
    uint32_t xp, yp;
    uint32_t pxp, pyp;

    xp = Blinky.xp;
    yp = Blinky.yp;

    // check if contact
    if ((Game.collision == BOOL_TRUE) && (Blinky.status == GHOST_STATUS_ALIVE)) {
        pxp = Player.xp;
        pyp = Player.yp;
        if ((xp == pxp) && (yp == pyp)) {
            if (Game.frightened == BOOL_FALSE) {
                Player.status = PLAYER_STATUS_DEAD;
            } else {
                Blinky.status = GHOST_STATUS_DEAD;
            }
            GUI.refresh_value = GUI_REFRESH_VALUE;
        }
    }

    // check if home position after dead
    if (Blinky.status == GHOST_STATUS_DEAD) {
        if ((xp == BLINKY_HOME_X) && (yp == BLINKY_HOME_Y)) {
            Blinky.status = GHOST_STATUS_ALIVE;
            Blinky.skin = GHOST_SKIN_UP1;
            Blinky.move = MOVE_UP;
            Blinky.next_move = MOVE_UP;
            Blinky.dot_cnt = 0;
        }
    }
}

//--------------------------------------------------------------
// calc next move from bot
//--------------------------------------------------------------
void blinky_calc_next_move(void) {
    uint32_t door_cnt = 0;
    uint32_t xp, yp;

    // actual position
    xp = Blinky.xp;
    yp = Blinky.yp;

    if (Blinky.move == MOVE_STOP) {
        // (if this happens...error in maze design)
        Blinky.next_move = MOVE_STOP;
        return;
    }
    // calc the next room the bot enters
    if (Blinky.move == MOVE_UP) yp--;
    if (Blinky.move == MOVE_RIGHT) xp++;
    if (Blinky.move == MOVE_DOWN) yp++;
    if (Blinky.move == MOVE_LEFT) xp--;

    // count the doors in this room
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) door_cnt++;
    if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) door_cnt++;

    if ((Blinky.status == GHOST_STATUS_DEAD) && (Maze.Room[xp][yp].special == ROOM_SPEC_GATE)) {
        // when dead and on a gate ignore the door count
        door_cnt = 2;
    }

    // choose a way
    if (door_cnt == 0) {
        // (if this happens...error in maze design)
        Blinky.next_move = MOVE_STOP;
    } else if (door_cnt == 1) {
        // take the only possible way
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) Blinky.next_move = MOVE_UP;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) Blinky.next_move = MOVE_RIGHT;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) Blinky.next_move = MOVE_DOWN;
        if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) Blinky.next_move = MOVE_LEFT;
    } else {
        // more than one possible way
        if (Blinky.new_mode == 1) {
            // mode has changed
            Blinky.new_mode = 0;
            // revers direction
            if (Blinky.move == MOVE_UP) Blinky.next_move = MOVE_DOWN;
            if (Blinky.move == MOVE_RIGHT) Blinky.next_move = MOVE_LEFT;
            if (Blinky.move == MOVE_DOWN) Blinky.next_move = MOVE_UP;
            if (Blinky.move == MOVE_LEFT) Blinky.next_move = MOVE_RIGHT;
            return;
        }

        if (Blinky.status == GHOST_STATUS_DEAD) {
            // dead (return to home)
            if (Maze.Room[xp][yp].special == ROOM_SPEC_GATE) {
                // found entry gate
                if ((Maze.Room[xp][yp].door & ROOM_BGATE_U) != 0) Blinky.next_move = MOVE_UP;
                if ((Maze.Room[xp][yp].door & ROOM_BGATE_R) != 0) Blinky.next_move = MOVE_RIGHT;
                if ((Maze.Room[xp][yp].door & ROOM_BGATE_D) != 0) Blinky.next_move = MOVE_DOWN;
                if ((Maze.Room[xp][yp].door & ROOM_BGATE_L) != 0) Blinky.next_move = MOVE_LEFT;
            } else {
                Blinky.next_move = bot_calc_move_home(GHOST_BLINKY, xp, yp, Blinky.move);
            }
            return;
        }

        if (Game.frightened == BOOL_TRUE) {
            // frightened (make a random move)
            Blinky.next_move = bot_calc_move_random(xp, yp, Blinky.move);
            return;
        }

        if (Game.mode == GAME_MODE_CHASE) {
            // chase
            if (Blinky.strategy == GHOST_STRATEGY_RANDOM) {
                Blinky.next_move = bot_calc_move_random(xp, yp, Blinky.move);
            } else if (Blinky.strategy == GHOST_STRATEGY_BLINKY) {
                Blinky.next_move = bot_calc_move_blinky(xp, yp, Blinky.move);
            } else if (Blinky.strategy == GHOST_STRATEGY_PINKY) {
                Blinky.next_move = bot_calc_move_pinky(xp, yp, Blinky.move);
            } else if (Blinky.strategy == GHOST_STRATEGY_INKY) {
                Blinky.next_move = bot_calc_move_inky(xp, yp, Blinky.move);
            } else {
                Blinky.next_move = bot_calc_move_clyde(GHOST_BLINKY, xp, yp, Blinky.move);
            }
        } else {
            // scatter
            Blinky.next_move = bot_calc_move_scatter(GHOST_BLINKY, xp, yp, Blinky.move);
        }
    }
}
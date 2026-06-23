//--------------------------------------------------------------
// File     : player.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "player.h"

#include <stdio.h>


extern Level_t Level[];

//--------------------------------------------------------------
// internal function
//--------------------------------------------------------------
void player_check_event(void);
void player_change_skin(uint32_t direction);

//--------------------------------------------------------------
// init start position 
//--------------------------------------------------------------
void player_init(uint32_t mode) {
    Player.status = PLAYER_STATUS_ALIVE;
    Player.xp = PLAYER_START_X;
    Player.yp = PLAYER_START_Y;
    Player.skin = PLAYER_SKIN_LEFT1;
    Player.skin_cnt = 0;
    Player.delta_x = 0;
    Player.delta_y = 0;
    Player.move = MOVE_LEFT;
    Player.port = PORT_DONE;
    if (mode == GAME_OVER) {
        Player.level = 1;
        Player.score = 0;
        Player.lives = PLAYER_START_LIVES;

    }
    if (Player.level <= GAME_MAX_LEVEL) {
        Player.akt_speed_ms = Level[Player.level - 1].player_speed;
    } else {
        Player.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].player_speed;
    }
    if ((mode == GAME_PLAYER_WIN) || (mode == GAME_OVER)) {
        Player.point_dots = 0;
    }
    Player.frightened_buf = PLAYER_FRIGHTENED_BUF;
}

//--------------------------------------------------------------
// moves player one pixel
//--------------------------------------------------------------
void player_move(void) {

    if (Player.move == MOVE_UP) {
        // move one pixel
        Player.delta_y--;
        // change skin
        player_change_skin(MOVE_UP);
        // overflow check
        if (Player.delta_y == 0) {
            if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_U) == 0) {
                // stop player not to move in a wall
                Player.move = MOVE_STOP;
                Player.skin = PLAYER_SKIN_UP1;
                return;
            }
        }
        // check if enter new room
        if (ABS(Player.delta_y) >= ROOM_HEIGHT) {
            // enter new room
            Player.delta_y = 0;
            if (Player.port == PORT_DONE) {
                // move to new room
                Player.yp--;
                // check if portal or door is closed
                if (Maze.Room[Player.xp][Player.yp - 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Player.port = PORT_UP0;
                } else if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_U) == 0) {
                    // stop player not to move in a wall
                    Player.move = MOVE_STOP;
                    Player.skin = PLAYER_SKIN_UP1;
                }
            } else if (Player.port == PORT_UP0) {
                // port to new room
                Player.yp = ROOM_CNT_Y - 2;
                // stop portal
                Player.port = PORT_UP1;
            } else {
                // move to new room
                Player.yp--;
                // deaktivate portal
                Player.port = PORT_DONE;
            }
            // check of new event in this room
            player_check_event();
        }
    } else if (Player.move == MOVE_RIGHT) {
        // move one pixel
        Player.delta_x++;
        // change skin
        player_change_skin(MOVE_RIGHT);
        // overflow check
        if (Player.delta_x == 0) {
            if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_R) == 0) {
                // stop player not to move in a wall
                Player.move = MOVE_STOP;
                Player.skin = PLAYER_SKIN_RIGHT1;
                return;
            }
        }
        // check if enter new room
        if (ABS(Player.delta_x) >= ROOM_WIDTH) {
            // enter new room
            Player.delta_x = 0;
            if (Player.port == PORT_DONE) {
                // move to new room
                Player.xp++;
                // check if portal or door is closed
                if (Maze.Room[Player.xp + 1][Player.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Player.port = PORT_RIGHT0;
                } else if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_R) == 0) {
                    // stop player not to move in a wall
                    Player.move = MOVE_STOP;
                    Player.skin = PLAYER_SKIN_RIGHT1;
                }
            } else if (Player.port == PORT_RIGHT0) {
                // port to new room
                Player.xp = 1;
                // stop portal
                Player.port = PORT_RIGHT1;
            } else {
                // deaktivate portal
                Player.port = PORT_DONE;
                // move to new room
                Player.xp++;
            }
            // check of new event in this room
            player_check_event();
        }
    } else if (Player.move == MOVE_DOWN) {
        // move one pixel
        Player.delta_y++;
        // change skin
        player_change_skin(MOVE_DOWN);
        // overflow check
        if (Player.delta_y == 0) {
            if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_D) == 0) {
                // stop player not to move in a wall
                Player.move = MOVE_STOP;
                Player.skin = PLAYER_SKIN_DOWN1;
                return;
            }
        }
        // check if enter new room
        if (ABS(Player.delta_y) >= ROOM_HEIGHT) {
            // enter new room
            Player.delta_y = 0;
            if (Player.port == PORT_DONE) {
                // move to new room
                Player.yp++;
                // check if portal or door is closed
                if (Maze.Room[Player.xp][Player.yp + 1].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Player.port = PORT_DOWN0;
                } else if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_D) == 0) {
                    // stop player not to move in a wall
                    Player.move = MOVE_STOP;
                    Player.skin = PLAYER_SKIN_DOWN1;
                }
            } else if (Player.port == PORT_DOWN0) {
                // port to new room
                Player.yp = 1;
                // stop portal
                Player.port = PORT_DOWN1;
            } else {
                // deaktivate portal
                Player.port = PORT_DONE;
                // move to new room
                Player.yp++;
            }
            // check of new event in this room
            player_check_event();
        }
    } else if (Player.move == MOVE_LEFT) {
        // move one pixel
        Player.delta_x--;
        // change skin
        player_change_skin(MOVE_LEFT);
        // overflow check
        if (Player.delta_x == 0) {
            if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_L) == 0) {
                // stop player not to move in a wall
                Player.move = MOVE_STOP;
                Player.skin = PLAYER_SKIN_LEFT1;
                return;
            }
        }
        // check if enter new room
        if (ABS(Player.delta_x) >= ROOM_WIDTH) {
            // enter new room
            Player.delta_x = 0;
            if (Player.port == PORT_DONE) {
                // move to new room
                Player.xp--;
                // check if portal or door is closed
                if (Maze.Room[Player.xp - 1][Player.yp].special == ROOM_SPEC_PORTAL) {
                    // start portal
                    Player.port = PORT_LEFT0;
                } else if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_L) == 0) {
                    // stop player not to move in a wall
                    Player.move = MOVE_STOP;
                    Player.skin = PLAYER_SKIN_LEFT1;
                }
            } else if (Player.port == PORT_LEFT0) {
                // port to new room
                Player.xp = ROOM_CNT_X - 2;
                // stop portal
                Player.port = PORT_LEFT1;
            } else {
                // deaktivate portal
                Player.port = PORT_DONE;
                // move to new room
                Player.xp--;
            }
            // check of new event in this room
            player_check_event();
        }
    } else {
        // no movement (wait on a wall)
    }
}

//--------------------------------------------------------------
// change skin from player for animation
//--------------------------------------------------------------
void player_change_skin(uint32_t direction) {
    Player.skin_cnt++;
    if (Player.skin_cnt > 7) Player.skin_cnt = 0;

    if (direction == MOVE_UP) {
        if (Player.skin_cnt < 2) {
            Player.skin = PLAYER_SKIN_FULL;
        } else if (Player.skin_cnt < 4) {
            Player.skin = PLAYER_SKIN_UP1;
        } else if (Player.skin_cnt < 6) {
            Player.skin = PLAYER_SKIN_UP2;
        } else {
            Player.skin = PLAYER_SKIN_UP1;
        }
    } else if (direction == MOVE_RIGHT) {
        if (Player.skin_cnt < 2) {
            Player.skin = PLAYER_SKIN_FULL;
        } else if (Player.skin_cnt < 4) {
            Player.skin = PLAYER_SKIN_RIGHT1;
        } else if (Player.skin_cnt < 6) {
            Player.skin = PLAYER_SKIN_RIGHT2;
        } else {
            Player.skin = PLAYER_SKIN_RIGHT1;
        }
    } else if (direction == MOVE_DOWN) {
        if (Player.skin_cnt < 2) {
            Player.skin = PLAYER_SKIN_FULL;
        } else if (Player.skin_cnt < 4) {
            Player.skin = PLAYER_SKIN_DOWN1;
        } else if (Player.skin_cnt < 6) {
            Player.skin = PLAYER_SKIN_DOWN2;
        } else {
            Player.skin = PLAYER_SKIN_DOWN1;
        }
    } else if (direction == MOVE_LEFT) {
        if (Player.skin_cnt < 2) {
            Player.skin = PLAYER_SKIN_FULL;
        } else if (Player.skin_cnt < 4) {
            Player.skin = PLAYER_SKIN_LEFT1;
        } else if (Player.skin_cnt < 6) {
            Player.skin = PLAYER_SKIN_LEFT2;
        } else {
            Player.skin = PLAYER_SKIN_LEFT1;
        }
    }
}

//--------------------------------------------------------------
// check of event in the new room
//--------------------------------------------------------------
void player_check_event(void) {
    uint32_t xp, yp;
    uint32_t gxp, gyp;

    xp = Player.xp;
    yp = Player.yp;

    // check of points
    if (Maze.Room[xp][yp].points != ROOM_POINTS_NONE) {
        // we got points
        if (Maze.Room[xp][yp].points == ROOM_POINTS_NORMAL ) {
            // normal
            Player.score += GAME_POINTS_NORMAL;
            Player.point_dots++;
        } else {
            // energyzier
            Player.score += GAME_POINTS_ENERGY;
            Player.point_dots++;
            // set "frightened"
            Game.frightened = BOOL_TRUE;
            Game.frightened_timer = GAME_FRIGHTENED_TIME;
            // set marker of all alive bots
            if (Blinky.status == GHOST_STATUS_ALIVE) Blinky.new_mode = 1;
            if (Pinky.status == GHOST_STATUS_ALIVE) Pinky.new_mode = 1;
            if (Inky.status == GHOST_STATUS_ALIVE) Inky.new_mode = 1;
            if (Clyde.status == GHOST_STATUS_ALIVE) Clyde.new_mode = 1;
        }
        // set room points to none
        Maze.Room[xp][yp].points = ROOM_POINTS_NONE;
        Maze.Room[xp][yp].skin = ROOM_SKIN_POINTS_NONE;
        GUI.refresh_value = GUI_REFRESH_VALUE;
        // increment dot counter
        Blinky.dot_cnt++;
        Pinky.dot_cnt++;
        Inky.dot_cnt++;
        Clyde.dot_cnt++;
    }

    // check if player wins
    if (Player.point_dots >= Maze.point_dots) {
        Player.status = PLAYER_STATUS_WIN;
        GUI.refresh_value = GUI_REFRESH_VALUE;
    }

    // check if contact
    if (Game.collision == BOOL_TRUE) {
        if (Blinky.status == GHOST_STATUS_ALIVE) {
            gxp = Blinky.xp;
            gyp = Blinky.yp;
            if ((xp == gxp) && (yp == gyp)) {
                if (Game.frightened == BOOL_FALSE) {
                    Player.status = PLAYER_STATUS_DEAD;
                } else {
                    Blinky.status = GHOST_STATUS_DEAD;
                    // add points
                    Player.score += Game.frightened_points;
                    Game.frightened_points += Game.frightened_points;
                }
                GUI.refresh_value = GUI_REFRESH_VALUE;
            }
        }

        if (Pinky.status == GHOST_STATUS_ALIVE) {
            gxp = Pinky.xp;
            gyp = Pinky.yp;
            if ((xp == gxp) && (yp == gyp)) {
                if (Game.frightened == BOOL_FALSE) {
                    Player.status = PLAYER_STATUS_DEAD;
                } else {
                    Pinky.status = GHOST_STATUS_DEAD;
                    // add points
                    Player.score += Game.frightened_points;
                    Game.frightened_points += Game.frightened_points;
                }
                GUI.refresh_value = GUI_REFRESH_VALUE;
            }
        }

        if (Inky.status == GHOST_STATUS_ALIVE) {
            gxp = Inky.xp;
            gyp = Inky.yp;
            if ((xp == gxp) && (yp == gyp)) {
                if (Game.frightened == BOOL_FALSE) {
                    Player.status = PLAYER_STATUS_DEAD;
                } else {
                    Inky.status = GHOST_STATUS_DEAD;
                    // add points
                    Player.score += Game.frightened_points;
                    Game.frightened_points += Game.frightened_points;
                }
                GUI.refresh_value = GUI_REFRESH_VALUE;
            }
        }

        if (Clyde.status == GHOST_STATUS_ALIVE) {
            gxp = Clyde.xp;
            gyp = Clyde.yp;
            if ((xp == gxp) && (yp == gyp)) {
                if (Game.frightened == BOOL_FALSE) {
                    Player.status = PLAYER_STATUS_DEAD;
                } else {
                    Clyde.status = GHOST_STATUS_DEAD;
                    // add points
                    Player.score += Game.frightened_points;
                    Game.frightened_points += Game.frightened_points;
                }
                GUI.refresh_value = GUI_REFRESH_VALUE;
            }
        }
    }
}

//--------------------------------------------------------------
// change direction of player
// in case of input
//--------------------------------------------------------------
void player_change_direction(uint32_t joy) {
    if (joy == GUI_JOY_UP) {
        if (Player.move == MOVE_UP) return;

        if (Player.port != PORT_DONE) {
            // dont change direction while porting
            return;
        } else if (Player.move == MOVE_DOWN) {
            // change direction immediately
            Player.move = MOVE_UP;
            return;
        } else {
            // turn only in center of a room
            if ((Player.delta_x == 0) && (Player.delta_y == 0)) {
                if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_U) != 0) {
                    Player.move = MOVE_UP;
                    return;
                }
            }
        }
    } else if (joy == GUI_JOY_RIGHT) {
        if (Player.move == MOVE_RIGHT) return;

        if (Player.port != PORT_DONE) {
            // dont change direction while porting
            return;
        } else if (Player.move == MOVE_LEFT) {
            // change direction immediately
            Player.move = MOVE_RIGHT;
            return;
        } else {
            // turn only in center of a room
            if ((Player.delta_x == 0) && (Player.delta_y == 0)) {
                if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_R) != 0) {
                    Player.move = MOVE_RIGHT;
                    return;
                }
            }
        }
    } else if (joy == GUI_JOY_DOWN) {
        if (Player.move == MOVE_DOWN) return;

        if (Player.port != PORT_DONE) {
            // dont change direction while porting
            return;
        } else if (Player.move == MOVE_UP) {
            // change direction immediately
            Player.move = MOVE_DOWN;
            return;
        } else {
            // turn only in center of a room
            if ((Player.delta_x == 0) && (Player.delta_y == 0)) {
                if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_D) != 0) {
                    Player.move = MOVE_DOWN;
                    return;
                }
            }
        }
    } else if (joy == GUI_JOY_LEFT) {
        if (Player.move == MOVE_LEFT) return;

        if (Player.port != PORT_DONE) {
            // dont change direction while porting
            return;
        } else if (Player.move == MOVE_RIGHT) {
            // change direction immediately
            Player.move = MOVE_LEFT;
            return;
        } else {
            // turn only in center of a room
            if ((Player.delta_x == 0) && (Player.delta_y == 0)) {
                if ((Maze.Room[Player.xp][Player.yp].door & ROOM_DOOR_L) != 0) {
                    Player.move = MOVE_LEFT;
                    return;
                }
            }
        }
    }
}
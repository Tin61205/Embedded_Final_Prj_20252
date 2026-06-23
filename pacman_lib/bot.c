//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "bot.h"

#include <stdlib.h>

#include <stdio.h>

#include "gui.h"

// Global variable definitions (declared extern in header)
Player_t Player;
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
uint32_t bot_calc_move_random(uint32_t xp, uint32_t yp, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t direction = 0;

    // take a random direction, but not go back
    do {
        direction = rand() % (4); // [0...3]
        if ((direction == 0) && (akt_dir != MOVE_DOWN)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) ret_wert = MOVE_UP;
        } else if ((direction == 1) && (akt_dir != MOVE_LEFT)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) ret_wert = MOVE_RIGHT;
        } else if ((direction == 2) && (akt_dir != MOVE_UP)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) ret_wert = MOVE_DOWN;
        } else if ((direction == 3) && (akt_dir != MOVE_RIGHT)) {
            if ((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) ret_wert = MOVE_LEFT;
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

    // spot the player as target
    txp = Player.xp;
    typ = Player.yp;

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

    // spot the player
    txp = Player.xp;
    typ = Player.yp;

    // check the player orientation
    if (Player.move == MOVE_STOP) {
        // player dont move (check the skin direction
        if (Player.skin == PLAYER_SKIN_UP1) p_dir = MOVE_UP;
        if (Player.skin == PLAYER_SKIN_RIGHT1) p_dir = MOVE_RIGHT;
        if (Player.skin == PLAYER_SKIN_DOWN1) p_dir = MOVE_DOWN;
        if (Player.skin == PLAYER_SKIN_LEFT1) p_dir = MOVE_LEFT;
    }

    // set the target (4 Rooms in Front of the Player position)
    if ((Player.move == MOVE_UP) || (p_dir == MOVE_UP)) {
        typ -= 4;
        if (typ < 0) typ = 0;
    } else if ((Player.move == MOVE_RIGHT) || (p_dir == MOVE_RIGHT)) {
        txp += 4;
        if (txp >= ROOM_CNT_X) txp = ROOM_CNT_X - 1;
    } else if ((Player.move == MOVE_DOWN) || (p_dir == MOVE_DOWN)) {
        typ += 4;
        if (typ >= ROOM_CNT_Y) txp = ROOM_CNT_Y - 1;
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

    // spot the player
    txp = Player.xp;
    typ = Player.yp;

    // check the player orientation
    if (Player.move == MOVE_STOP) {
        // player dont move (check the skin direction
        if (Player.skin == PLAYER_SKIN_UP1) p_dir = MOVE_UP;
        if (Player.skin == PLAYER_SKIN_RIGHT1) p_dir = MOVE_RIGHT;
        if (Player.skin == PLAYER_SKIN_DOWN1) p_dir = MOVE_DOWN;
        if (Player.skin == PLAYER_SKIN_LEFT1) p_dir = MOVE_LEFT;
    }

    // set the target (2 Rooms in Front of the Player position)
    if ((Player.move == MOVE_UP) || (p_dir == MOVE_UP)) {
        typ -= 2;
    } else if ((Player.move == MOVE_RIGHT) || (p_dir == MOVE_RIGHT)) {
        txp += 2;
    } else if ((Player.move == MOVE_DOWN) || (p_dir == MOVE_DOWN)) {
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
    if (typ >= ROOM_CNT_Y) txp = ROOM_CNT_Y - 1;

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

    // spot the player
    txp = Player.xp;
    typ = Player.yp;

    // calculate the distance to the player
    d_clyde = bot_calc_distance(txp, typ, xp, yp);

    if (d_clyde > 800) {
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
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_U) != 0) && (akt_dir != MOVE_DOWN)) {
        xn = x1;
        yn = y1 - 1;
        d_up = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_R) != 0) && (akt_dir != MOVE_LEFT)) {
        xn = x1 + 1;
        yn = y1;
        d_right = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_D) != 0) && (akt_dir != MOVE_UP)) {
        xn = x1;
        yn = y1 + 1;
        d_down = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_L) != 0) && (akt_dir != MOVE_RIGHT)) {
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

    return (ret_wert);
}

//--------------------------------------------------------------
// calc the distane between two points
// c = SQRT(a*a + b*b)
//--------------------------------------------------------------
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    uint32_t ret_wert = 0;
    int32_t dx, dy; // signed int !!

    dx = (x1 - x2);
    dx = ABS(dx);

    dy = (y1 - y2);
    dy = ABS(dy);

    // to increase resolution
    dx = dx * SQRT_FAKTOR;
    dy = dy * SQRT_FAKTOR;

    if ((dx == 0) || (dy == 0)) {
        if ((dx == 0) && (dy == 0)) {
            ret_wert = 0;
            return (ret_wert);
        }
        if (dx == 0) {
            ret_wert = dy;
            return (ret_wert);
        }
        if (dy == 0) {
            ret_wert = dx;
            return (ret_wert);
        }
    }

    //-----------------------
    // c = SQRT(a*a + b*b)
    //-----------------------
    dx = dx * dx;
    dy = dy * dy;

    ret_wert = UB_SQRT(dx + dy);

    return (ret_wert);
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
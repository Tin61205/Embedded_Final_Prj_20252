//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "maze_generate.h"

void maze_generate_setonewallskin_outside(uint32_t x, uint32_t y, uint32_t r);
void maze_generate_setonewallskin_inside(uint32_t x, uint32_t y);

//--------------------------------------------------------------
// search all rooms with a general mark
// and set the room data
//--------------------------------------------------------------
void maze_generate_searchandset(uint32_t mark, Room_t room) {
    uint32_t x, y;

    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            if (mark == SEARCH_ROOMS_ALL) {
                Maze.Room[x][y].typ = room.typ;
                Maze.Room[x][y].special = room.special;
                Maze.Room[x][y].door = room.door;
                Maze.Room[x][y].skin = room.skin;
                Maze.Room[x][y].points = room.points;
                Maze.Room[x][y].deb_err = room.deb_err;
            } else if (mark == SEARCH_DOORS_YES) {
                if (Maze.Room[x][y].door != ROOM_DOOR_NONE) {
                    Maze.Room[x][y].typ = room.typ;
                    Maze.Room[x][y].skin = room.skin;
                }
            } else if (mark == SEARCH_SKIN_PATH) {
                if ((Maze.Room[x][y].typ == ROOM_TYP_PATH) || (Maze.Room[x][y].special == ROOM_SPEC_PORTAL)) {
                    Maze.Room[x][y].skin = ROOM_SKIN_POINTS_NONE;
                    if (Maze.Room[x][y].points == ROOM_POINTS_NORMAL) Maze.Room[x][y].skin = ROOM_SKIN_POINTS_NORMAL;
                    // if (Maze.Room[x][y].points == ROOM_POINTS_ENERGY) Maze.Room[x][y].skin = ROOM_SKIN_POINTS_NORMAL;
                    if(Maze.Room[x][y].points==ROOM_POINTS_ENERGY)  Maze.Room[x][y].skin=ROOM_SKIN_POINTS_ENERGY;
                }
            } else if (mark == SEARCH_SKIN_UNDEF) {
                if (Maze.Room[x][y].skin == ROOM_SKIN_WALL_UNDEF) {
                    Maze.Room[x][y].skin = room.skin;
                }
            }
        }
    }
}

//--------------------------------------------------------------
// dig a path horizontal and set automaticly the doors and points
// len>=2
// 0=ok
//--------------------------------------------------------------
uint32_t maze_generate_digpath_h(uint32_t x, uint32_t y, uint32_t l, uint32_t p) {
    uint32_t ret_wert = 0;
    uint32_t n;

    if (x >= ROOM_CNT_X) return (1);
    if (y >= ROOM_CNT_Y) return (2);
    if (l < 2) return (3);
    if ((x + l) > ROOM_CNT_X) return (4);

    for (n = 0; n < l; n++) {
        if (n == 0) {
            Maze.Room[x + n][y].door |= ROOM_DOOR_R;
        } else if (n == l - 1) {
            Maze.Room[x + n][y].door |= ROOM_DOOR_L;
        } else {
            Maze.Room[x + n][y].door |= ROOM_DOOR_R;
            Maze.Room[x + n][y].door |= ROOM_DOOR_L;
        }
        if (p != ROOM_POINTS_NONE) Maze.Room[x + n][y].points = p;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// dao 1 duong thang dung va tu dong dat ten cac cua va diem+
// len>=2
// 0=ok
//--------------------------------------------------------------
uint32_t maze_generate_digpath_v(uint32_t x, uint32_t y, uint32_t l, uint32_t p) {
    uint32_t ret_wert = 0;
    uint32_t n;

    if (x >= ROOM_CNT_X) return (1);
    if (y >= ROOM_CNT_Y) return (2);
    if (l < 2) return (3);
    if ((y + l) > ROOM_CNT_Y) return (4);

    for (n = 0; n < l; n++) {
        if (n == 0) {
            Maze.Room[x][y + n].door |= ROOM_DOOR_D;
        } else if (n == l - 1) {
            Maze.Room[x][y + n].door |= ROOM_DOOR_U;
        } else {
            Maze.Room[x][y + n].door |= ROOM_DOOR_D;
            Maze.Room[x][y + n].door |= ROOM_DOOR_U;
        }
        if (p != ROOM_POINTS_NONE) Maze.Room[x][y + n].points = p;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// set two rooms to special "portal" and add automaticly the doors
// 0=ok
//--------------------------------------------------------------
uint32_t maze_generate_setportals(uint32_t x, uint32_t y, Room_t room) {
    uint32_t ret_wert = 0;

    if (x == 0) {
        if ((y == 0) || (y >= ROOM_CNT_Y - 1)) return (1);
        // set portal-A
        Maze.Room[0][y].typ = room.typ;
        Maze.Room[0][y].special = room.special;
        Maze.Room[0][y].door |= ROOM_DOOR_L;
        // set portal-B
        Maze.Room[ROOM_CNT_X - 1][y].typ = room.typ;
        Maze.Room[ROOM_CNT_X - 1][y].special = room.special;
        Maze.Room[ROOM_CNT_X - 1][y].door |= ROOM_DOOR_R;
    } else if (x == (ROOM_CNT_X - 1)) {
        if ((y == 0) || (y >= ROOM_CNT_Y - 1)) return (2);
        // set portal-A
        Maze.Room[ROOM_CNT_X - 1][y].typ = room.typ;
        Maze.Room[ROOM_CNT_X - 1][y].special = room.special;
        Maze.Room[ROOM_CNT_X - 1][y].door |= ROOM_DOOR_R;
        // set portal-B
        Maze.Room[0][y].typ = room.typ;
        Maze.Room[0][y].typ = room.typ;
        Maze.Room[0][y].door |= ROOM_DOOR_L;
    } else if (y == 0) {
        if ((x == 0) || (x >= ROOM_CNT_X - 1)) return (3);
        // set portal-A
        Maze.Room[x][0].typ = room.typ;
        Maze.Room[x][0].special = room.special;
        Maze.Room[x][0].door |= ROOM_DOOR_U;
        // set portal-B
        Maze.Room[x][ROOM_CNT_Y - 1].typ = room.typ;
        Maze.Room[x][ROOM_CNT_Y - 1].special = room.special;
        Maze.Room[x][ROOM_CNT_Y - 1].door |= ROOM_DOOR_D;
    } else if (y == (ROOM_CNT_Y - 1)) {
        if ((x == 0) || (x >= ROOM_CNT_X - 1)) return (4);
        // set portal-A
        Maze.Room[x][ROOM_CNT_Y - 1].typ = room.typ;
        Maze.Room[x][ROOM_CNT_Y - 1].special = room.special;
        Maze.Room[x][ROOM_CNT_Y - 1].door |= ROOM_DOOR_D;
        // set portal-B
        Maze.Room[x][0].typ = room.typ;
        Maze.Room[x][0].special = room.special;
        Maze.Room[x][0].door |= ROOM_DOOR_U;
    } else {
        return (5);
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// set one room to special "gate" and add automaticly the doors
// 0=ok
//--------------------------------------------------------------
uint32_t maze_generate_setgate(uint32_t x, uint32_t y, Room_t room, uint32_t door) {
    uint32_t ret_wert = 0;

    if (door == ROOM_DOOR_U) {
        Maze.Room[x][y].special = room.special;
        // set Gate
        Maze.Room[x][y].door |= room.door;
        // open inner door to release the bots
        Maze.Room[x][y - 1].door |= ROOM_DOOR_D;
        Maze.Room[x][y - 1].typ = ROOM_TYP_WALL;
    } else if (door == ROOM_DOOR_R) {
        Maze.Room[x][y].special = room.special;
        // set Gate
        Maze.Room[x][y].door |= room.door;
        // open inner door to release the bots
        Maze.Room[x + 1][y].door |= ROOM_DOOR_L;
        Maze.Room[x + 1][y].typ = ROOM_TYP_WALL;
    } else if (door == ROOM_DOOR_D) {
        Maze.Room[x][y].special = room.special;
        // set Gate
        Maze.Room[x][y].door |= room.door;
        // open inner door to release the bots
        Maze.Room[x][y + 1].door |= ROOM_DOOR_U;
        Maze.Room[x][y + 1].typ = ROOM_TYP_WALL;
    } else if (door == ROOM_DOOR_L) {
        Maze.Room[x][y].special = room.special;
        // set Gate
        Maze.Room[x][y].door |= room.door;
        // open inner door to release the bots
        Maze.Room[x - 1][y].door |= ROOM_DOOR_R;
        Maze.Room[x - 1][y].typ = ROOM_TYP_WALL;
    } else {
        ret_wert = 1;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// set automaticly the skin of the walls outside the maze
//--------------------------------------------------------------
void maze_generate_setwallskin_outside(void) {
    uint32_t x, y;

    for (x = 0; x < ROOM_CNT_X; x++) {
        if ((Maze.Room[x][0].typ == ROOM_TYP_WALL) && (Maze.Room[x][0].skin == ROOM_SKIN_WALL_STD)) {
            maze_generate_setonewallskin_outside(x, 0, 0);
        }
        if ((Maze.Room[x][ROOM_CNT_Y - 1].typ == ROOM_TYP_WALL) && (Maze.Room[x][ROOM_CNT_Y - 1].skin == ROOM_SKIN_WALL_STD)) {
            maze_generate_setonewallskin_outside(x, ROOM_CNT_Y - 1, 1);
        }
    }

    for (y = 0; y < ROOM_CNT_Y; y++) {
        if ((Maze.Room[0][y].typ == ROOM_TYP_WALL) && (Maze.Room[0][y].skin == ROOM_SKIN_WALL_STD)) {
            maze_generate_setonewallskin_outside(0, y, 2);
        }
        if ((Maze.Room[ROOM_CNT_X - 1][y].typ == ROOM_TYP_WALL) && (Maze.Room[ROOM_CNT_X - 1][y].skin == ROOM_SKIN_WALL_STD)) {
            maze_generate_setonewallskin_outside(ROOM_CNT_X - 1, y, 3);
        }
    }
}

//--------------------------------------------------------------
// set automaticly the skin of one wall outside the maze
// if not possible set to "UNDEF"
//--------------------------------------------------------------
void maze_generate_setonewallskin_outside(uint32_t x, uint32_t y, uint32_t r) {
    // default set to "BLANK"
    Maze.Room[x][y].skin = ROOM_SKIN_WALL_UNDEF;

    // try to find the right skin
    if (r == 0) {
        // top
        if (Maze.Room[x][y + 1].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_O_TOP;
        }
    } else if (r == 1) {
        // bottom
        if (Maze.Room[x][y - 1].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_O_BOTTOM;
        }
    } else if (r == 2) {
        // left
        if (Maze.Room[x + 1][y].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_O_LEFT;
        }
    } else {
        // right
        if (Maze.Room[x - 1][y].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_O_RIGHT;
        }
    }
}

//--------------------------------------------------------------
// set automaticly the skin of the walls inside the maze
//--------------------------------------------------------------
void maze_generate_setwallskin_inside(void) {
    uint32_t x, y;

    for (y = 1; y < ROOM_CNT_Y - 1; y++) {
        for (x = 1; x < ROOM_CNT_X - 1; x++) {
            if ((Maze.Room[x][y].typ == ROOM_TYP_WALL) && (Maze.Room[x][y].skin == ROOM_SKIN_WALL_STD)) {
                maze_generate_setonewallskin_inside(x, y);
            }
        }
    }
}

//--------------------------------------------------------------
// set automaticly the skin of one wall inside the maze
// if not possible set to "UNDEF"
//--------------------------------------------------------------
void maze_generate_setonewallskin_inside(uint32_t x, uint32_t y) {
    // default set to "BLANK"
    Maze.Room[x][y].skin = ROOM_SKIN_WALL_UNDEF;

    // try to find the right skin
    if (Maze.Room[x][y - 1].typ == ROOM_TYP_PATH) {
        if (Maze.Room[x - 1][y].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_TOPLEFT1;
        } else if (Maze.Room[x + 1][y].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_TOPRIGHT1;
        } else {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_TOP;
        }
    } else if (Maze.Room[x][y + 1].typ == ROOM_TYP_PATH) {
        if (Maze.Room[x - 1][y].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_BOTTOMLEFT1;
        } else if (Maze.Room[x + 1][y].typ == ROOM_TYP_PATH) {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_BOTTOMRIGHT1;
        } else {
            Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_BOTTOM;
        }
    } else if (Maze.Room[x - 1][y].typ == ROOM_TYP_PATH) {
        Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_LEFT;
    } else if (Maze.Room[x + 1][y].typ == ROOM_TYP_PATH) {
        Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_RIGHT;
    } else if (Maze.Room[x - 1][y - 1].typ == ROOM_TYP_PATH) {
        Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_BOTTOMRIGHT1;
    } else if (Maze.Room[x - 1][y + 1].typ == ROOM_TYP_PATH) {
        Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_TOPRIGHT1;
    } else if (Maze.Room[x + 1][y - 1].typ == ROOM_TYP_PATH) {
        Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_BOTTOMLEFT1;
    } else if (Maze.Room[x + 1][y + 1].typ == ROOM_TYP_PATH) {
        Maze.Room[x][y].skin = ROOM_SKIN_WALL_I_TOPLEFT1;
    }
}

//--------------------------------------------------------------
// set manually the skin horizontal
// len>=1
// 0=ok
//--------------------------------------------------------------
uint32_t maze_generate_setskin_h(uint32_t x, uint32_t y, uint32_t l, uint32_t s) {
    uint32_t ret_wert = 0;
    uint32_t n;

    if (x >= ROOM_CNT_X) return (1);
    if (y >= ROOM_CNT_Y) return (2);
    if (l < 1) return (3);
    if ((x + l) > ROOM_CNT_X) return (4);

    for (n = 0; n < l; n++) {
        Maze.Room[x + n][y].skin = s;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// set manually the skin vertikal
// len>=1
// 0=ok
//--------------------------------------------------------------
uint32_t maze_generate_setskin_v(uint32_t x, uint32_t y, uint32_t l, uint32_t s) {
    uint32_t ret_wert = 0;
    uint32_t n;

    if (x >= ROOM_CNT_X) return (1);
    if (y >= ROOM_CNT_Y) return (2);
    if (l < 1) return (3);
    if ((y + l) > ROOM_CNT_Y) return (4);

    for (n = 0; n < l; n++) {
        Maze.Room[x][y + n].skin = s;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// check the maze
// 0=ok
// >0 = last error nr
//--------------------------------------------------------------
uint32_t maze_generate_check(void) {
    uint32_t ret_wert = 0;
    uint32_t x, y;

    //------------------------------
    // 1. set all rooms to ok
    //------------------------------
    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            Maze.Room[x][y].deb_err = ROOM_DEB_OK;
        }
    }

    //------------------------------
    // 2. check all outline rooms to be no "PATH"
    //------------------------------
    for (x = 0; x < ROOM_CNT_X; x++) {
        if (Maze.Room[x][0].typ == ROOM_TYP_PATH) {
            ret_wert = 1;
            Maze.Room[x][0].deb_err |= ROOM_DEB_BORDER;
        }
        if (Maze.Room[x][ROOM_CNT_Y - 1].typ == ROOM_TYP_PATH) {
            ret_wert = 2;
            Maze.Room[x][ROOM_CNT_Y - 1].deb_err |= ROOM_DEB_BORDER;
        }
    }
    for (y = 0; y < ROOM_CNT_Y; y++) {
        if (Maze.Room[0][y].typ == ROOM_TYP_PATH) {
            ret_wert = 3;
            Maze.Room[0][y].deb_err |= ROOM_DEB_BORDER;
        }
        if (Maze.Room[ROOM_CNT_X - 1][y].typ == ROOM_TYP_PATH) {
            ret_wert = 4;
            Maze.Room[ROOM_CNT_X - 1][y].deb_err |= ROOM_DEB_BORDER;
        }
    }

    //------------------------------
    // 3. check all "path" has at least one door
    //------------------------------
    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            if (Maze.Room[x][y].typ == ROOM_TYP_PATH) {
                if (Maze.Room[x][y].door == ROOM_DOOR_NONE) {
                    Maze.Room[x][y].deb_err |= ROOM_DEB_PATH;
                    ret_wert = 5;
                }
            }
        }
    }

    //------------------------------
    // 4. check all doors has a neighbour
    //------------------------------
    for (y = 1; y < ROOM_CNT_Y - 1; y++) {
        for (x = 1; x < ROOM_CNT_X - 1; x++) {
            if (Maze.Room[x][y].typ == ROOM_TYP_PATH) {
                if ((Maze.Room[x][y].door & ROOM_DOOR_U) != 0) {
                    if ((Maze.Room[x][y - 1].door & ROOM_DOOR_D) == 0) {
                        Maze.Room[x][y].deb_err |= ROOM_DEB_DOOR;
                        ret_wert = 6;
                    }
                }
                if ((Maze.Room[x][y].door & ROOM_DOOR_D) != 0) {
                    if ((Maze.Room[x][y + 1].door & ROOM_DOOR_U) == 0) {
                        Maze.Room[x][y].deb_err |= ROOM_DEB_DOOR;
                        ret_wert = 7;
                    }
                }
                if ((Maze.Room[x][y].door & ROOM_DOOR_L) != 0) {
                    if ((Maze.Room[x - 1][y].door & ROOM_DOOR_R) == 0) {
                        Maze.Room[x][y].deb_err |= ROOM_DEB_DOOR;
                        ret_wert = 8;
                    }
                }
                if ((Maze.Room[x][y].door & ROOM_DOOR_R) != 0) {
                    if ((Maze.Room[x + 1][y].door & ROOM_DOOR_L) == 0) {
                        Maze.Room[x][y].deb_err |= ROOM_DEB_DOOR;
                        ret_wert = 9;
                    }
                }
            }
        }
    }

    //------------------------------
    // 5. check all "walls" has a skin
    //------------------------------
    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            if (Maze.Room[x][y].typ == ROOM_TYP_WALL) {
                if (Maze.Room[x][y].skin == ROOM_SKIN_WALL_UNDEF) {
                    Maze.Room[x][y].deb_err |= ROOM_DEB_SKIN;
                    ret_wert = 10;
                }
            }
        }
    }

    //------------------------------
    // 6. check that points >0
    //------------------------------ 
    if (maze_generate_count_dots() == 0) {
        ret_wert = 11;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// count the rooms with points
//--------------------------------------------------------------
uint32_t maze_generate_count_dots(void) {
    uint32_t ret_wert = 0;
    uint32_t x, y;

    ret_wert = 0;
    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            if (Maze.Room[x][y].points != ROOM_POINTS_NONE) {
                ret_wert++;
            }
        }
    }

    return (ret_wert);
}
#include "maze.h"
#include "random.h"
#include "stm32_ub_font.h"

#include "maze_generate.h"

// Global variable definition (declared extern in header)
Maze_t Maze;

void maze_make_rooms(void);
void maze_set_skin(void);
void maze_count_dots(void);

//--------------------------------------------------------------
// generate the maze
//--------------------------------------------------------------
void maze_build(void) {
    // generate all rooms for the maze
    maze_make_rooms();
    // set skin for all rooms
    maze_set_skin();
    // count all dots in the maze (room with points)
    maze_count_dots();
}

//--------------------------------------------------------------
// generate the complete maze (with standard skin)
//--------------------------------------------------------------
void maze_make_rooms(void) {
    Room_t room;

    //------------------------------
    // 1. init all rooms as "WALL"
    //    with no doors,no points
    //    and standard "wall" skin
    //------------------------------
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    //------------------------------
    // 2a. dig pathes through the walls
    // horizontal and set doors and points 
    //------------------------------
    maze_generate_digpath_h(1, 1, 12, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(15, 1, 12, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 5, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 8, 6, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(9, 8, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(15, 8, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(21, 8, 6, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(9, 11, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_h(0, 14, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_h(18, 14, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_h(9, 17, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_h(1, 20, 12, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(15, 20, 12, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 23, 3, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(6, 23, 16, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(24, 23, 3, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 26, 6, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(9, 26, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(15, 26, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(21, 26, 6, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 29, 26, ROOM_POINTS_NORMAL);

    //------------------------------
    // 2b. dig pathes through the walls
    // vertikal and set doors and points
    //------------------------------
    maze_generate_digpath_v(1, 1, 8, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(6, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(12, 1, 5, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(15, 1, 5, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(21, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 1, 8, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(9, 5, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(18, 5, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(12, 8, 4, ROOM_POINTS_NONE);
    maze_generate_digpath_v(15, 8, 4, ROOM_POINTS_NONE);
    maze_generate_digpath_v(9, 11, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_v(18, 11, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_v(1, 20, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(12, 20, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(15, 20, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 20, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(3, 23, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(9, 23, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(18, 23, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(24, 23, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(1, 26, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(12, 26, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(15, 26, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 26, 4, ROOM_POINTS_NORMAL);

    //------------------------------
    // 3. set all rooms with doors to "PATH"
    //    and set standard "path" skin
    //------------------------------
    room.typ = ROOM_TYP_PATH;
    room.skin = ROOM_SKIN_PATH_STD;
    maze_generate_searchandset(SEARCH_DOORS_YES, room);

    //------------------------------
    // 5. set special "PORTAL"
    //------------------------------
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    maze_generate_setportals(0, 14, room);

    //------------------------------
    // 6. set special "GATE"
    //------------------------------
    room.special = ROOM_SPEC_GATE;
    room.door = (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D);
    maze_generate_setgate(14, 11, room, ROOM_DOOR_D); // gate for all (down)
    room.special = ROOM_SPEC_GATE;
    room.door = (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D);
    maze_generate_setgate(14, 12, room, ROOM_DOOR_D); // gate for all (down)
    room.special = ROOM_SPEC_GATE;
    room.door = (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D);
    maze_generate_setgate(14, 13, room, ROOM_DOOR_D); // gate for all (down)
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_CGATE_R; // gate for clyde (right)
    maze_generate_setgate(14, 14, room, ROOM_DOOR_R);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_IGATE_L; // gate for inky (left)
    maze_generate_setgate(14, 14, room, ROOM_DOOR_L);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_CGATE_R; // gate for clyde (right)
    maze_generate_setgate(15, 14, room, ROOM_DOOR_R);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_IGATE_L; // gate for inky (left)
    maze_generate_setgate(13, 14, room, ROOM_DOOR_L);

    //------------------------------
    // 8. clear manually two Points at the startfield
    //------------------------------
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;

    //------------------------------
    // 9. choose 4 random rooms for the energy points
    //------------------------------
    int count_energy = 0;
    while(count_energy < 4){
        random_init();
        uint32_t x = get_randrange(1, 26);
        uint32_t y = get_randrange(1, 26);
        // char buf[20];
        // sprintf(buf, "x: %d, y: %d", x, y);
        // UB_Font_DrawString(0, 0, buf, &Arial_7x10, 0x0000, 0xFFFF);
        if (Maze.Room[x][y].points == ROOM_POINTS_NORMAL) {
            Maze.Room[x][y].points = ROOM_POINTS_ENERGY;
            count_energy++;
        }
    }



}

//--------------------------------------------------------------
// set the Skin for the complete maze
//--------------------------------------------------------------
void maze_set_skin(void) {
    Room_t room;

    //------------------------------
    // 1. set all "PATH" Skins
    //------------------------------
    maze_generate_searchandset(SEARCH_SKIN_PATH, room);

    //------------------------------
    // 2. set automaticly all possible
    //    outside wall skins
    //------------------------------
    maze_generate_setwallskin_outside();

    //------------------------------
    // 3. set manually
    //    outside wall skins
    //------------------------------
    maze_generate_setskin_h(0, 0, 1, ROOM_SKIN_WALL_O_TOPLEFT1);
    maze_generate_setskin_h(27, 0, 1, ROOM_SKIN_WALL_O_TOPRIGHT1);
    maze_generate_setskin_h(13, 0, 1, ROOM_SKIN_WALL_O_HORTOP1);
    maze_generate_setskin_h(14, 0, 1, ROOM_SKIN_WALL_O_HORTOP2);
    maze_generate_setskin_h(0, 9, 1, ROOM_SKIN_WALL_O_BOTTOMLEFT1);
    maze_generate_setskin_h(27, 9, 1, ROOM_SKIN_WALL_O_BOTTOMRIGHT1);
    maze_generate_setskin_h(1, 9, 4, ROOM_SKIN_WALL_O_BOTTOM);
    maze_generate_setskin_h(23, 9, 4, ROOM_SKIN_WALL_O_BOTTOM);
    maze_generate_setskin_h(5, 9, 1, ROOM_SKIN_WALL_O_TOPRIGHT2);
    maze_generate_setskin_h(22, 9, 1, ROOM_SKIN_WALL_O_TOPLEFT2);
    maze_generate_setskin_v(5, 10, 3, ROOM_SKIN_WALL_O_LEFT);
    maze_generate_setskin_v(22, 10, 3, ROOM_SKIN_WALL_O_RIGHT);
    maze_generate_setskin_h(5, 13, 1, ROOM_SKIN_WALL_O_BOTTOMRIGHT2);
    maze_generate_setskin_h(22, 13, 1, ROOM_SKIN_WALL_O_BOTTOMLEFT2);
    maze_generate_setskin_h(0, 13, 5, ROOM_SKIN_WALL_O_TOP);
    maze_generate_setskin_h(23, 13, 5, ROOM_SKIN_WALL_O_TOP);
    maze_generate_setskin_h(0, 15, 5, ROOM_SKIN_WALL_O_BOTTOM);
    maze_generate_setskin_h(23, 15, 5, ROOM_SKIN_WALL_O_BOTTOM);
    maze_generate_setskin_h(5, 15, 1, ROOM_SKIN_WALL_O_TOPRIGHT2);
    maze_generate_setskin_h(22, 15, 1, ROOM_SKIN_WALL_O_TOPLEFT2);
    maze_generate_setskin_v(5, 16, 3, ROOM_SKIN_WALL_O_LEFT);
    maze_generate_setskin_v(22, 16, 3, ROOM_SKIN_WALL_O_RIGHT);
    maze_generate_setskin_h(5, 19, 1, ROOM_SKIN_WALL_O_BOTTOMRIGHT2);
    maze_generate_setskin_h(22, 19, 1, ROOM_SKIN_WALL_O_BOTTOMLEFT2);
    maze_generate_setskin_h(1, 19, 4, ROOM_SKIN_WALL_O_TOP);
    maze_generate_setskin_h(23, 19, 4, ROOM_SKIN_WALL_O_TOP);
    maze_generate_setskin_h(0, 19, 1, ROOM_SKIN_WALL_O_TOPLEFT1);
    maze_generate_setskin_h(27, 19, 1, ROOM_SKIN_WALL_O_TOPRIGHT1);
    maze_generate_setskin_h(0, 24, 1, ROOM_SKIN_WALL_O_VERLEFT1);
    maze_generate_setskin_h(0, 25, 1, ROOM_SKIN_WALL_O_VERLEFT2);
    maze_generate_setskin_h(27, 24, 1, ROOM_SKIN_WALL_O_VERRIGHT1);
    maze_generate_setskin_h(27, 25, 1, ROOM_SKIN_WALL_O_VERRIGHT2);
    maze_generate_setskin_h(0, 30, 1, ROOM_SKIN_WALL_O_BOTTOMLEFT1);
    maze_generate_setskin_h(27, 30, 1, ROOM_SKIN_WALL_O_BOTTOMRIGHT1);

    //------------------------------
    // 5. set manually
    //    walls of home base
    //------------------------------
    maze_generate_setskin_h(10, 12, 1, ROOM_SKIN_WALL_I_TOPLEFT2);
    maze_generate_setskin_h(17, 12, 1, ROOM_SKIN_WALL_I_TOPRIGHT2);
    maze_generate_setskin_h(11, 12, 2, ROOM_SKIN_WALL_O_BOTTOM);
    maze_generate_setskin_h(13, 12, 2, ROOM_SKIN_HOME_DOOR);
    maze_generate_setskin_h(15, 12, 2, ROOM_SKIN_WALL_O_BOTTOM);
    maze_generate_setskin_v(10, 13, 3, ROOM_SKIN_WALL_O_RIGHT);
    maze_generate_setskin_v(17, 13, 3, ROOM_SKIN_WALL_O_LEFT);
    maze_generate_setskin_h(10, 16, 1, ROOM_SKIN_WALL_I_BOTTOMLEFT2);
    maze_generate_setskin_h(17, 16, 1, ROOM_SKIN_WALL_I_BOTTOMRIGHT2);
    maze_generate_setskin_h(11, 16, 6, ROOM_SKIN_WALL_O_TOP);

    //------------------------------
    // 6. set automaticly all possible
    //    inside wall skins
    //------------------------------
    maze_generate_setwallskin_inside();

    //------------------------------
    // 7. set all "UNDEF" Walls to "Black"
    //------------------------------
    room.skin = ROOM_SKIN_WALL_BLACK;
    maze_generate_searchandset(SEARCH_SKIN_UNDEF, room);

}

//--------------------------------------------------------------
// count all dots in the maze
//--------------------------------------------------------------
void maze_count_dots(void) {
    Maze.point_dots = maze_generate_count_dots();
}
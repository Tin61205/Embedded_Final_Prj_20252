//--------------------------------------------------------------
// File     : maze_txtmap.c
// Purpose  : Load maps from const uint8_t arrays into the Maze
//            structure.  Handles wall/path setup, doors, skin,
//            and automatic placement of ghost home base + portals.
//--------------------------------------------------------------

#include "maze_txtmap.h"
#include "maze_generate.h"
#include "random.h"
#include <string.h>

//--------------------------------------------------------------
// Table of all txt-imported map data pointers
//--------------------------------------------------------------
const uint8_t * const maze_txtmap_table[MAZE_TXTMAP_COUNT] = {
    maze_txtmap_sample1,
};

//--------------------------------------------------------------
// Display names for the menu
//--------------------------------------------------------------
const char * const maze_txtmap_names[MAZE_TXTMAP_COUNT] = {
    "Sample1",
};

//--------------------------------------------------------------
// Find a suitable position for the ghost home base
// Searches for a large enough open area to place the 8x6
// home base structure (cols 10..17, rows 11..16 in classic).
// For txt maps, we find the largest contiguous path region
// and place the home base there, OR we just use the
// center of the map and carve out space.
//
// Simpler approach: place home base at fixed position
// (center of grid) and FORCE those cells to be correct.
//--------------------------------------------------------------
static void txtmap_setup_home_base(void) {
    Room_t room;
    uint32_t x, y;

    // Home base occupies columns 10..17, rows 12..16 (same as classic)
    // The entry gate is at row 11 (above the home box)
    // We need to ensure the area around home base is walkable

    // 1. Clear the home base interior to PATH (no food)
    for (y = 13; y <= 15; y++) {
        for (x = 11; x <= 16; x++) {
            Maze.Room[x][y].typ = ROOM_TYP_PATH;
            Maze.Room[x][y].special = ROOM_SPEC_NONE;
            Maze.Room[x][y].door = ROOM_DOOR_NONE;
            Maze.Room[x][y].points = ROOM_POINTS_NONE;
        }
    }

    // 2. Set home base walls (box outline)
    // Top wall (row 12, cols 10..17)
    for (x = 10; x <= 17; x++) {
        Maze.Room[x][12].typ = ROOM_TYP_WALL;
        Maze.Room[x][12].special = ROOM_SPEC_NONE;
        Maze.Room[x][12].door = ROOM_DOOR_NONE;
        Maze.Room[x][12].skin = ROOM_SKIN_WALL_STD;
        Maze.Room[x][12].points = ROOM_POINTS_NONE;
    }
    // Bottom wall (row 16, cols 10..17)
    for (x = 10; x <= 17; x++) {
        Maze.Room[x][16].typ = ROOM_TYP_WALL;
        Maze.Room[x][16].special = ROOM_SPEC_NONE;
        Maze.Room[x][16].door = ROOM_DOOR_NONE;
        Maze.Room[x][16].skin = ROOM_SKIN_WALL_STD;
        Maze.Room[x][16].points = ROOM_POINTS_NONE;
    }
    // Left wall (col 10, rows 12..16)
    for (y = 12; y <= 16; y++) {
        Maze.Room[10][y].typ = ROOM_TYP_WALL;
        Maze.Room[10][y].special = ROOM_SPEC_NONE;
        Maze.Room[10][y].door = ROOM_DOOR_NONE;
        Maze.Room[10][y].skin = ROOM_SKIN_WALL_STD;
        Maze.Room[10][y].points = ROOM_POINTS_NONE;
    }
    // Right wall (col 17, rows 12..16)
    for (y = 12; y <= 16; y++) {
        Maze.Room[17][y].typ = ROOM_TYP_WALL;
        Maze.Room[17][y].special = ROOM_SPEC_NONE;
        Maze.Room[17][y].door = ROOM_DOOR_NONE;
        Maze.Room[17][y].skin = ROOM_SKIN_WALL_STD;
        Maze.Room[17][y].points = ROOM_POINTS_NONE;
    }

    // 3. Ensure a path corridor exists above home base for entry (row 11)
    // Make row 11, cols 9..18 into PATH (no food) to guarantee access
    for (x = 9; x <= 18; x++) {
        if (Maze.Room[x][11].typ == ROOM_TYP_WALL) {
            Maze.Room[x][11].typ = ROOM_TYP_PATH;
            Maze.Room[x][11].special = ROOM_SPEC_NONE;
            Maze.Room[x][11].door = ROOM_DOOR_NONE;
            Maze.Room[x][11].points = ROOM_POINTS_NONE;
        }
    }

    // 4. Ensure a path corridor exists below home base (row 17)
    for (x = 9; x <= 18; x++) {
        if (Maze.Room[x][17].typ == ROOM_TYP_WALL) {
            Maze.Room[x][17].typ = ROOM_TYP_PATH;
            Maze.Room[x][17].special = ROOM_SPEC_NONE;
            Maze.Room[x][17].door = ROOM_DOOR_NONE;
            Maze.Room[x][17].points = ROOM_POINTS_NONE;
        }
    }

    // 5. Ensure vertical corridors on sides of home base connect
    // Left side (col 9, rows 11..17)
    for (y = 11; y <= 17; y++) {
        if (Maze.Room[9][y].typ == ROOM_TYP_WALL) {
            Maze.Room[9][y].typ = ROOM_TYP_PATH;
            Maze.Room[9][y].special = ROOM_SPEC_NONE;
            Maze.Room[9][y].door = ROOM_DOOR_NONE;
            Maze.Room[9][y].points = ROOM_POINTS_NONE;
        }
    }
    // Right side (col 18, rows 11..17)
    for (y = 11; y <= 17; y++) {
        if (Maze.Room[18][y].typ == ROOM_TYP_WALL) {
            Maze.Room[18][y].typ = ROOM_TYP_PATH;
            Maze.Room[18][y].special = ROOM_SPEC_NONE;
            Maze.Room[18][y].door = ROOM_DOOR_NONE;
            Maze.Room[18][y].points = ROOM_POINTS_NONE;
        }
    }

    // 6. Set gates (same structure as classic map)
    room.special = ROOM_SPEC_GATE;
    room.door = (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D);
    maze_generate_setgate(14, 11, room, ROOM_DOOR_D);
    room.special = ROOM_SPEC_GATE;
    room.door = (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D);
    maze_generate_setgate(14, 12, room, ROOM_DOOR_D);
    room.special = ROOM_SPEC_GATE;
    room.door = (ROOM_BGATE_D | ROOM_PGATE_D | ROOM_IGATE_D | ROOM_CGATE_D);
    maze_generate_setgate(14, 13, room, ROOM_DOOR_D);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_CGATE_R;
    maze_generate_setgate(14, 14, room, ROOM_DOOR_R);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_IGATE_L;
    maze_generate_setgate(14, 14, room, ROOM_DOOR_L);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_CGATE_R;
    maze_generate_setgate(15, 14, room, ROOM_DOOR_R);
    room.special = ROOM_SPEC_GATE;
    room.door = ROOM_IGATE_L;
    maze_generate_setgate(13, 14, room, ROOM_DOOR_L);
}

//--------------------------------------------------------------
// Setup portals (tunnel) at the horizontal edges of row 14
//--------------------------------------------------------------
static void txtmap_setup_portals(void) {
    Room_t room;

    // Ensure path on row 14 extends to borders for portal access
    // Left side: cols 1..9
    {
        uint32_t x;
        for (x = 1; x <= 9; x++) {
            if (Maze.Room[x][14].typ == ROOM_TYP_WALL) {
                Maze.Room[x][14].typ = ROOM_TYP_PATH;
                Maze.Room[x][14].special = ROOM_SPEC_NONE;
                Maze.Room[x][14].door = ROOM_DOOR_NONE;
                Maze.Room[x][14].points = ROOM_POINTS_NONE;
            }
        }
    }
    // Right side: cols 18..26
    {
        uint32_t x;
        for (x = 18; x <= 26; x++) {
            if (Maze.Room[x][14].typ == ROOM_TYP_WALL) {
                Maze.Room[x][14].typ = ROOM_TYP_PATH;
                Maze.Room[x][14].special = ROOM_SPEC_NONE;
                Maze.Room[x][14].door = ROOM_DOOR_NONE;
                Maze.Room[x][14].points = ROOM_POINTS_NONE;
            }
        }
    }

    // Set portals at column 0 and column 27 on row 14
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_setportals(0, 14, room);
}

//--------------------------------------------------------------
// Ensure player start area (around y=23, x=13..14) is walkable
//--------------------------------------------------------------
static void txtmap_setup_player_start(void) {
    uint32_t x, y;

    // Player starts at (14, 23) / Player2 at (13, 23)
    // Ensure at least a small region around the start is PATH
    for (y = 22; y <= 24; y++) {
        for (x = 12; x <= 15; x++) {
            if (x >= 0 && x < ROOM_CNT_X && y >= 0 && y < ROOM_CNT_Y) {
                if (Maze.Room[x][y].typ == ROOM_TYP_WALL) {
                    Maze.Room[x][y].typ = ROOM_TYP_PATH;
                    Maze.Room[x][y].special = ROOM_SPEC_NONE;
                    Maze.Room[x][y].door = ROOM_DOOR_NONE;
                    Maze.Room[x][y].points = ROOM_POINTS_NONE;
                }
            }
        }
    }

    // Connect player start area to home base area
    // Vertical corridor at x=14 from y=17 to y=23
    for (y = 17; y <= 23; y++) {
        if (Maze.Room[14][y].typ == ROOM_TYP_WALL) {
            Maze.Room[14][y].typ = ROOM_TYP_PATH;
            Maze.Room[14][y].special = ROOM_SPEC_NONE;
            Maze.Room[14][y].door = ROOM_DOOR_NONE;
            Maze.Room[14][y].points = ROOM_POINTS_NONE;
        }
    }

    // Clear food at player start positions
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;
}

//--------------------------------------------------------------
// Main function: loads a txt-imported map into the Maze struct
//--------------------------------------------------------------
void maze_make_rooms_txtmap(uint32_t txt_map_index) {
    const uint8_t *data;
    uint32_t x, y;
    Room_t room;

    if (txt_map_index >= MAZE_TXTMAP_COUNT) {
        txt_map_index = 0;
    }

    data = maze_txtmap_table[txt_map_index];

    // 1. Initialize all rooms as WALL
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    // 2. Apply the map data: set PATH cells and food
    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            uint8_t cell = data[y * ROOM_CNT_X + x];

            if (cell == TXTMAP_CELL_WALL) {
                // Already wall from step 1
                continue;
            }

            // It's a path cell
            Maze.Room[x][y].typ = ROOM_TYP_PATH;
            Maze.Room[x][y].special = ROOM_SPEC_NONE;
            Maze.Room[x][y].door = ROOM_DOOR_NONE;

            if (cell == TXTMAP_CELL_PATH_FOOD) {
                Maze.Room[x][y].points = ROOM_POINTS_NORMAL;
            } else if (cell == TXTMAP_CELL_PATH_ENERGY) {
                Maze.Room[x][y].points = ROOM_POINTS_ENERGY;
            } else {
                // TXTMAP_CELL_PATH_EMPTY or any other value
                Maze.Room[x][y].points = ROOM_POINTS_NONE;
            }
        }
    }

    // 3. Setup the ghost home base (forced at the standard location)
    txtmap_setup_home_base();

    // 4. Setup portals (tunnel)
    txtmap_setup_portals();

    // 5. Ensure player start position is walkable
    txtmap_setup_player_start();

    // NOTE: maze_build_map() will call sync_path_doors, strip_orphan_wall_doors,
    //       set_skin, and count_dots after this function returns.
}

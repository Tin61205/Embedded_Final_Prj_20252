#include "maze.h"
#include <string.h>
#include "random.h"
#include "stm32_ub_font.h"
#include "stm32_ub_lcd_ili9341.h"
#include "stm32_ub_graphic2d.h"

#include "maze_generate.h"
#include "skin.h"

// Global variable definition (declared extern in header)
Maze_t Maze;
uint32_t Maze_selected_map = MAZE_MAP_CLASSIC;

void maze_make_rooms_classic(void);
void maze_make_rooms_open(void);
void maze_make_rooms_map3(void);
void maze_make_rooms_map4(void);
void maze_make_rooms_map5(void);
void maze_make_rooms_map6(void);
static void maze_make_home_connectors(void);
void maze_set_skin(void);
void maze_count_dots(void);
void maze_place_energy_dots(void);

//--------------------------------------------------------------
// generate the maze
//--------------------------------------------------------------
void maze_build(void) {
    maze_build_map(Maze_selected_map);
}

void maze_build_map(uint32_t map_id) {
    memset(&Maze, 0, sizeof(Maze_t));
    Maze_selected_map = map_id;
    if (map_id == MAZE_MAP_OPEN) {
        maze_make_rooms_open();
    } else if (map_id == MAZE_MAP_THREE) {
        maze_make_rooms_map3();
    } else if (map_id == MAZE_MAP_FOUR) {
        maze_make_rooms_map4();
    } else if (map_id == MAZE_MAP_FIVE) {
        maze_make_rooms_map5();
    } else if (map_id == MAZE_MAP_SIX) {
        maze_make_rooms_map6();
    } else {
        maze_make_rooms_classic();
    }
    maze_generate_sync_path_doors();
    maze_generate_strip_orphan_wall_doors();
    maze_set_skin();
    maze_count_dots();
}

//--------------------------------------------------------------
// generate the complete maze (with standard skin)
//--------------------------------------------------------------
void maze_make_rooms_classic(void) {
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

    maze_place_energy_dots();
}

//--------------------------------------------------------------
// ghost-house links without long 1-cell vertical grooves
//--------------------------------------------------------------
static void maze_make_home_connectors(void) {
    maze_generate_digpath_h(9, 11, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_v(9, 11, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_v(18, 11, 10, ROOM_POINTS_NONE);
    maze_generate_digpath_v(12, 1, 5, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(15, 1, 5, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(12, 8, 4, ROOM_POINTS_NONE);
    maze_generate_digpath_v(15, 8, 4, ROOM_POINTS_NONE);
    maze_generate_digpath_v(12, 20, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(15, 20, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 14, 9, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(18, 14, 9, ROOM_POINTS_NORMAL);
}

//--------------------------------------------------------------
// simplified open map layout
//--------------------------------------------------------------
void maze_make_rooms_open(void) {
    Room_t room;

    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    // 1. Dig border paths
    maze_generate_digpath_h(1, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 29, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(1, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 1, 29, ROOM_POINTS_NORMAL);

    // 2. Dig inner horizontal paths (avoiding home base region)
    maze_generate_digpath_h(1, 5, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 9, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 20, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(12, 23, 4, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 25, 26, ROOM_POINTS_NORMAL);

    // 3. Dig vertical paths connecting the horizontal paths
    maze_generate_digpath_v(6, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(21, 1, 29, ROOM_POINTS_NORMAL);

    // 4. Ghost-house connectors (classic style, no long grooves)
    maze_make_home_connectors();

    // 5. Set all dug rooms (having doors) to PATH
    room.typ = ROOM_TYP_PATH;
    room.skin = ROOM_SKIN_PATH_STD;
    maze_generate_searchandset(SEARCH_DOORS_YES, room);

    // 6. Set special PORTALs
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    maze_generate_setportals(0, 14, room);

    // 7. Set special GATEx and inner doors for Home Base (same as Classic)
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

    // 8. Clear points at player start position
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;

    // 9. Place energy dots
    maze_place_energy_dots();
}

void maze_place_energy_dots(void) {
    int count_energy = 0;

    while (count_energy < 4) {
        random_init();
        uint32_t x = get_randrange(1, 26);
        uint32_t y = get_randrange(1, 26);

        if (Maze.Room[x][y].points == ROOM_POINTS_NORMAL) {
            Maze.Room[x][y].points = ROOM_POINTS_ENERGY;
            count_energy++;
        }
    }
}

static void maze_draw_preview_cell(uint32_t dest_x, uint32_t dest_y, uint32_t cell_px, uint32_t skin_id) {
    uint32_t sx = Room_Skin[skin_id].xp;
    uint32_t sy = Room_Skin[skin_id].yp;
    uint32_t px;
    uint32_t py;
    uint32_t ws = Skin1.width;

    if (cell_px == ROOM_WIDTH) {
        Image2LCD_t koord;
        koord.dest_xp = dest_x;
        koord.dest_yp = dest_y;
        koord.w = ROOM_WIDTH;
        koord.h = ROOM_HEIGHT;
        koord.source_xp = sx;
        koord.source_yp = sy;
        UB_Graphic2D_DrawImageRect(koord);
        return;
    }

    for (py = 0; py < cell_px; py++) {
        uint32_t y_start = (py * ROOM_HEIGHT) / cell_px;
        uint32_t y_end = ((py + 1) * ROOM_HEIGHT) / cell_px;
        if (y_end <= y_start) y_end = y_start + 1;

        UB_LCD_SetCursor2Draw(dest_x, dest_y + py);

        for (px = 0; px < cell_px; px++) {
            uint32_t x_start = (px * ROOM_WIDTH) / cell_px;
            uint32_t x_end = ((px + 1) * ROOM_WIDTH) / cell_px;
            if (x_end <= x_start) x_end = x_start + 1;

            uint16_t final_color = 0;
            uint32_t found_wall = 0;
            uint32_t scan_y, scan_x;

            for (scan_y = y_start; scan_y < y_end; scan_y++) {
                uint32_t src_y = sy + scan_y;
                for (scan_x = x_start; scan_x < x_end; scan_x++) {
                    uint32_t src_x = sx + scan_x;
                    uint16_t c = Skin1.table[(src_y * ws) + src_x];
                    if (c != 0) { // Khác màu đen nền
                        final_color = c;
                        found_wall = 1;
                        break;
                    }
                }
                if (found_wall) break;
            }

            if (!found_wall) {
                uint32_t mid_y = sy + (y_start + y_end) / 2;
                uint32_t mid_x = sx + (x_start + x_end) / 2;
                final_color = Skin1.table[(mid_y * ws) + mid_x];
            }

            UB_LCD_DrawPixel(final_color);
        }
    }
}

void maze_draw_preview(uint32_t map_id, uint32_t dest_x, uint32_t dest_y, uint32_t cell_px) {
    uint32_t x;
    uint32_t y;
    uint32_t pw = ROOM_CNT_X * cell_px;
    uint32_t ph = ROOM_CNT_Y * cell_px;

    maze_build_map(map_id);
    UB_Graphic2D_DrawFullRectDMA(dest_x, dest_y, pw, ph, RGB_COL_BLACK);

    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            maze_draw_preview_cell(
                dest_x + (x * cell_px),
                dest_y + (y * cell_px),
                cell_px,
                Maze.Room[x][y].skin);
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
    //    outside wall skins (only for Classic Map)
    //------------------------------
    if (Maze_selected_map == MAZE_MAP_CLASSIC) {
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
    }

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

void maze_make_rooms_map3(void) {
    Room_t room;

    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    // 1. Dig border paths
    maze_generate_digpath_h(1, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 29, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(1, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 1, 29, ROOM_POINTS_NORMAL);

    // 2. Spiral walls / paths inside
    // Left Spiral
    maze_generate_digpath_h(3, 5, 8, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(3, 5, 20, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(3, 25, 8, ROOM_POINTS_NORMAL);
    // Right Spiral (Symmetric)
    maze_generate_digpath_h(17, 5, 8, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(24, 5, 20, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(17, 25, 8, ROOM_POINTS_NORMAL);

    // 3. Connectors
    maze_generate_digpath_h(3, 9, 22, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(3, 21, 22, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(12, 23, 4, ROOM_POINTS_NORMAL);

    maze_make_home_connectors();

    // 4. Set PATH rooms
    room.typ = ROOM_TYP_PATH;
    room.skin = ROOM_SKIN_PATH_STD;
    maze_generate_searchandset(SEARCH_DOORS_YES, room);

    // 5. Portals
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    maze_generate_setportals(0, 14, room);

    // 6. Home Base Gate & Doors (same as classic)
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

    // 7. Clear start positions
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;

    maze_place_energy_dots();
}

void maze_make_rooms_map4(void) {
    Room_t room;

    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    // 1. Dig border paths
    maze_generate_digpath_h(1, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 29, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(1, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 1, 29, ROOM_POINTS_NORMAL);

    // 2. Blocks main corridors
    maze_generate_digpath_h(1, 7, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 23, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(7, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(20, 1, 29, ROOM_POINTS_NORMAL);

    maze_make_home_connectors();

    // 3. Set PATH rooms
    room.typ = ROOM_TYP_PATH;
    room.skin = ROOM_SKIN_PATH_STD;
    maze_generate_searchandset(SEARCH_DOORS_YES, room);

    // 4. Portals
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    maze_generate_setportals(0, 14, room);

    // 5. Home Base Gate & Doors (same as classic)
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

    // 6. Clear start positions
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;

    maze_place_energy_dots();
}

void maze_make_rooms_map5(void) {
    Room_t room;

    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    // 1. Dig border paths
    maze_generate_digpath_h(1, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 29, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(1, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 1, 29, ROOM_POINTS_NORMAL);

    // 2. Dig vertical columns
    maze_generate_digpath_v(4, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(8, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(19, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(23, 1, 29, ROOM_POINTS_NORMAL);

    // 3. Dig horizontal cross sections
    maze_generate_digpath_h(1, 8, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 22, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(12, 23, 4, ROOM_POINTS_NORMAL);

    maze_make_home_connectors();

    // 4. Set PATH rooms
    room.typ = ROOM_TYP_PATH;
    room.skin = ROOM_SKIN_PATH_STD;
    maze_generate_searchandset(SEARCH_DOORS_YES, room);

    // 5. Portals
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    maze_generate_setportals(0, 14, room);

    // 6. Home Base Gate & Doors (same as classic)
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

    // 7. Clear start positions
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;

    maze_place_energy_dots();
}

void maze_make_rooms_map6(void) {
    Room_t room;

    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_NONE;
    room.door = ROOM_DOOR_NONE;
    room.skin = ROOM_SKIN_WALL_STD;
    room.points = ROOM_POINTS_NONE;
    room.deb_err = ROOM_DEB_OK;
    maze_generate_searchandset(SEARCH_ROOMS_ALL, room);

    // 1. Dig border paths
    maze_generate_digpath_h(1, 1, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(1, 29, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(1, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(26, 1, 29, ROOM_POINTS_NORMAL);

    // 2. Dig upper half
    maze_generate_digpath_h(1, 6, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(6, 1, 29, ROOM_POINTS_NORMAL);
    maze_generate_digpath_v(21, 1, 29, ROOM_POINTS_NORMAL);
    
    // 3. Dig lower half
    maze_generate_digpath_h(1, 24, 26, ROOM_POINTS_NORMAL);
    maze_generate_digpath_h(12, 23, 4, ROOM_POINTS_NORMAL);

    maze_make_home_connectors();

    // 4. Set PATH rooms
    room.typ = ROOM_TYP_PATH;
    room.skin = ROOM_SKIN_PATH_STD;
    maze_generate_searchandset(SEARCH_DOORS_YES, room);

    // 5. Portals
    room.typ = ROOM_TYP_WALL;
    room.special = ROOM_SPEC_PORTAL;
    maze_generate_setportals(0, 14, room);

    // 6. Home Base Gate & Doors (same as classic)
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

    // 7. Clear start positions
    Maze.Room[13][23].points = ROOM_POINTS_NONE;
    Maze.Room[14][23].points = ROOM_POINTS_NONE;

    maze_place_energy_dots();
}
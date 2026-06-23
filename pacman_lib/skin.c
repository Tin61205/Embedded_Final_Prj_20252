//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "skin.h"

// Global variable definitions (declared extern in header)
Skin_t Room_Skin[ROOM_SKIN_CNT];
Skin_t Player_Skin[PLAYER_SKIN_CNT];
Skin_t Blinky_Skin[GHOST_SKIN_CNT];
Skin_t Pinky_Skin[GHOST_SKIN_CNT];
Skin_t Inky_Skin[GHOST_SKIN_CNT];
Skin_t Clyde_Skin[GHOST_SKIN_CNT];
Skin_t Button_Skin[BUTTON_SKIN_CNT];

//--------------------------------------------------------------
// internal function
//--------------------------------------------------------------
void skin_init_maze(void);
void skin_init_player(void);
void skin_init_ghosts(void);
void skin_init_gui(void);

//--------------------------------------------------------------
// init all skins
//--------------------------------------------------------------
void skin_init(void) {
    skin_init_maze();
    skin_init_player();
    skin_init_ghosts();
    skin_init_gui();
}

//--------------------------------------------------------------
// init skin off all rooms from the maze
//--------------------------------------------------------------
void skin_init_maze(void) {
    uint32_t n;

    for (n = 0; n < ROOM_SKIN_CNT; n++) {
        Room_Skin[n].xp = 0;
        Room_Skin[n].yp = 0;
    }

    Room_Skin[ROOM_SKIN_WALL_STD].xp = 2;
    Room_Skin[ROOM_SKIN_WALL_STD].yp = 2;

    Room_Skin[ROOM_SKIN_PATH_STD].xp = 10;
    Room_Skin[ROOM_SKIN_PATH_STD].yp = 2;

    Room_Skin[ROOM_SKIN_WALL_UNDEF].xp = 18;
    Room_Skin[ROOM_SKIN_WALL_UNDEF].yp = 2;

    Room_Skin[ROOM_SKIN_WALL_BLACK].xp = 26;
    Room_Skin[ROOM_SKIN_WALL_BLACK].yp = 2;

    Room_Skin[ROOM_SKIN_WALL_O_TOP].xp = 18;
    Room_Skin[ROOM_SKIN_WALL_O_TOP].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_O_RIGHT].xp = 26;
    Room_Skin[ROOM_SKIN_WALL_O_RIGHT].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_O_BOTTOM].xp = 26;
    Room_Skin[ROOM_SKIN_WALL_O_BOTTOM].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_O_LEFT].xp = 18;
    Room_Skin[ROOM_SKIN_WALL_O_LEFT].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_O_TOPRIGHT1].xp = 10;
    Room_Skin[ROOM_SKIN_WALL_O_TOPRIGHT1].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMRIGHT1].xp = 10;
    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMRIGHT1].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMLEFT1].xp = 2;
    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMLEFT1].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_O_TOPLEFT1].xp = 2;
    Room_Skin[ROOM_SKIN_WALL_O_TOPLEFT1].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_O_TOPRIGHT2].xp = 42;
    Room_Skin[ROOM_SKIN_WALL_O_TOPRIGHT2].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMRIGHT2].xp = 42;
    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMRIGHT2].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMLEFT2].xp = 34;
    Room_Skin[ROOM_SKIN_WALL_O_BOTTOMLEFT2].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_O_TOPLEFT2].xp = 34;
    Room_Skin[ROOM_SKIN_WALL_O_TOPLEFT2].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_O_HORTOP1].xp = 34;
    Room_Skin[ROOM_SKIN_WALL_O_HORTOP1].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_O_HORTOP2].xp = 42;
    Room_Skin[ROOM_SKIN_WALL_O_HORTOP2].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_O_VERLEFT1].xp = 50;
    Room_Skin[ROOM_SKIN_WALL_O_VERLEFT1].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_O_VERLEFT2].xp = 50;
    Room_Skin[ROOM_SKIN_WALL_O_VERLEFT2].yp = 34;

    Room_Skin[ROOM_SKIN_WALL_O_VERRIGHT1].xp = 58;
    Room_Skin[ROOM_SKIN_WALL_O_VERRIGHT1].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_O_VERRIGHT2].xp = 58;
    Room_Skin[ROOM_SKIN_WALL_O_VERRIGHT2].yp = 34;

    Room_Skin[ROOM_SKIN_WALL_I_TOP].xp = 26;
    Room_Skin[ROOM_SKIN_WALL_I_TOP].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_I_RIGHT].xp = 18;
    Room_Skin[ROOM_SKIN_WALL_I_RIGHT].yp = 34;

    Room_Skin[ROOM_SKIN_WALL_I_BOTTOM].xp = 18;
    Room_Skin[ROOM_SKIN_WALL_I_BOTTOM].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_I_LEFT].xp = 26;
    Room_Skin[ROOM_SKIN_WALL_I_LEFT].yp = 34;

    Room_Skin[ROOM_SKIN_WALL_I_TOPRIGHT1].xp = 10;
    Room_Skin[ROOM_SKIN_WALL_I_TOPRIGHT1].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMRIGHT1].xp = 10;
    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMRIGHT1].yp = 34;

    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMLEFT1].xp = 2;
    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMLEFT1].yp = 34;

    Room_Skin[ROOM_SKIN_WALL_I_TOPLEFT1].xp = 2;
    Room_Skin[ROOM_SKIN_WALL_I_TOPLEFT1].yp = 26;

    Room_Skin[ROOM_SKIN_WALL_I_TOPRIGHT2].xp = 58;
    Room_Skin[ROOM_SKIN_WALL_I_TOPRIGHT2].yp = 10;

    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMRIGHT2].xp = 58;
    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMRIGHT2].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMLEFT2].xp = 50;
    Room_Skin[ROOM_SKIN_WALL_I_BOTTOMLEFT2].yp = 18;

    Room_Skin[ROOM_SKIN_WALL_I_TOPLEFT2].xp = 50;
    Room_Skin[ROOM_SKIN_WALL_I_TOPLEFT2].yp = 10;

    Room_Skin[ROOM_SKIN_HOME_DOOR].xp = 66;
    Room_Skin[ROOM_SKIN_HOME_DOOR].yp = 10;

    Room_Skin[ROOM_SKIN_POINTS_NONE].xp = 2;
    Room_Skin[ROOM_SKIN_POINTS_NONE].yp = 42;

    Room_Skin[ROOM_SKIN_POINTS_NORMAL].xp = 10;
    Room_Skin[ROOM_SKIN_POINTS_NORMAL].yp = 42;

    Room_Skin[ROOM_SKIN_POINTS_ENERGY].xp = 18;
    Room_Skin[ROOM_SKIN_POINTS_ENERGY].yp = 42;
}

//--------------------------------------------------------------
// init skin from the player
//--------------------------------------------------------------
void skin_init_player(void) {
    uint32_t n;

    for (n = 0; n < PLAYER_SKIN_CNT; n++) {
        Player_Skin[n].xp = 0;
        Player_Skin[n].yp = 0;
    }

    Player_Skin[PLAYER_SKIN_FULL].xp = 2;
    Player_Skin[PLAYER_SKIN_FULL].yp = 54;

    Player_Skin[PLAYER_SKIN_UP1].xp = 2 + (1 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_UP1].yp = 54;

    Player_Skin[PLAYER_SKIN_UP2].xp = 2 + (2 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_UP2].yp = 54;

    Player_Skin[PLAYER_SKIN_RIGHT1].xp = 2 + (3 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_RIGHT1].yp = 54;

    Player_Skin[PLAYER_SKIN_RIGHT2].xp = 2 + (4 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_RIGHT2].yp = 54;

    Player_Skin[PLAYER_SKIN_DOWN1].xp = 2 + (5 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_DOWN1].yp = 54;

    Player_Skin[PLAYER_SKIN_DOWN2].xp = 2 + (6 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_DOWN2].yp = 54;

    Player_Skin[PLAYER_SKIN_LEFT1].xp = 2 + (7 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_LEFT1].yp = 54;

    Player_Skin[PLAYER_SKIN_LEFT2].xp = 2 + (8 * BOTS_WIDTH);
    Player_Skin[PLAYER_SKIN_LEFT2].yp = 54;
}

//--------------------------------------------------------------
// init skin from all ghosts
//--------------------------------------------------------------
void skin_init_ghosts(void) {
    uint32_t n;

    for (n = 0; n < GHOST_SKIN_CNT; n++) {
        Blinky_Skin[n].xp = 0;
        Blinky_Skin[n].yp = 0;
        Pinky_Skin[n].xp = 0;
        Pinky_Skin[n].yp = 0;
        Inky_Skin[n].xp = 0;
        Inky_Skin[n].yp = 0;
        Clyde_Skin[n].xp = 0;
        Clyde_Skin[n].yp = 0;
    }

    //------------------------------------------
    Blinky_Skin[GHOST_SKIN_RIGHT1].xp = 2;
    Blinky_Skin[GHOST_SKIN_RIGHT1].yp = 68;

    Blinky_Skin[GHOST_SKIN_RIGHT2].xp = 2 + (1 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_RIGHT2].yp = 68;

    Blinky_Skin[GHOST_SKIN_LEFT1].xp = 2 + (2 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_LEFT1].yp = 68;

    Blinky_Skin[GHOST_SKIN_LEFT2].xp = 2 + (3 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_LEFT2].yp = 68;

    Blinky_Skin[GHOST_SKIN_UP1].xp = 2 + (4 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_UP1].yp = 68;

    Blinky_Skin[GHOST_SKIN_UP2].xp = 2 + (5 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_UP2].yp = 68;

    Blinky_Skin[GHOST_SKIN_DOWN1].xp = 2 + (6 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_DOWN1].yp = 68;

    Blinky_Skin[GHOST_SKIN_DOWN2].xp = 2 + (7 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_DOWN2].yp = 68;

    Blinky_Skin[GHOST_SKIN_FRIGHTEN1].xp = 3 + (8 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_FRIGHTEN1].yp = 69;

    Blinky_Skin[GHOST_SKIN_FRIGHTEN2].xp = 3 + (8 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_FRIGHTEN2].yp = 83;

    Blinky_Skin[GHOST_SKIN_DEAD].xp = 3 + (8 * BOTS_WIDTH);
    Blinky_Skin[GHOST_SKIN_DEAD].yp = 98;

    //------------------------------------------
    Pinky_Skin[GHOST_SKIN_RIGHT1].xp = 2;
    Pinky_Skin[GHOST_SKIN_RIGHT1].yp = 82;

    Pinky_Skin[GHOST_SKIN_RIGHT2].xp = 2 + (1 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_RIGHT2].yp = 82;

    Pinky_Skin[GHOST_SKIN_LEFT1].xp = 2 + (2 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_LEFT1].yp = 82;

    Pinky_Skin[GHOST_SKIN_LEFT2].xp = 2 + (3 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_LEFT2].yp = 82;

    Pinky_Skin[GHOST_SKIN_UP1].xp = 2 + (4 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_UP1].yp = 82;

    Pinky_Skin[GHOST_SKIN_UP2].xp = 2 + (5 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_UP2].yp = 82;

    Pinky_Skin[GHOST_SKIN_DOWN1].xp = 2 + (6 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_DOWN1].yp = 82;

    Pinky_Skin[GHOST_SKIN_DOWN2].xp = 2 + (7 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_DOWN2].yp = 82;

    Pinky_Skin[GHOST_SKIN_FRIGHTEN1].xp = 3 + (8 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_FRIGHTEN1].yp = 69;
    
    Pinky_Skin[GHOST_SKIN_FRIGHTEN2].xp = 3 + (8 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_FRIGHTEN2].yp = 83;

    Pinky_Skin[GHOST_SKIN_DEAD].xp = 3 + (8 * BOTS_WIDTH);
    Pinky_Skin[GHOST_SKIN_DEAD].yp = 98;

    //------------------------------------------
    Inky_Skin[GHOST_SKIN_RIGHT1].xp = 2;
    Inky_Skin[GHOST_SKIN_RIGHT1].yp = 96;

    Inky_Skin[GHOST_SKIN_RIGHT2].xp = 2 + (1 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_RIGHT2].yp = 96;

    Inky_Skin[GHOST_SKIN_LEFT1].xp = 2 + (2 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_LEFT1].yp = 96;

    Inky_Skin[GHOST_SKIN_LEFT2].xp = 2 + (3 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_LEFT2].yp = 96;

    Inky_Skin[GHOST_SKIN_UP1].xp = 2 + (4 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_UP1].yp = 96;

    Inky_Skin[GHOST_SKIN_UP2].xp = 2 + (5 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_UP2].yp = 96;

    Inky_Skin[GHOST_SKIN_DOWN1].xp = 2 + (6 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_DOWN1].yp = 96;

    Inky_Skin[GHOST_SKIN_DOWN2].xp = 2 + (7 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_DOWN2].yp = 96;

    Inky_Skin[GHOST_SKIN_FRIGHTEN1].xp = 3 + (8 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_FRIGHTEN1].yp = 69;

    Inky_Skin[GHOST_SKIN_FRIGHTEN2].xp = 3 + (8 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_FRIGHTEN2].yp = 83;

    Inky_Skin[GHOST_SKIN_DEAD].xp = 3 + (8 * BOTS_WIDTH);
    Inky_Skin[GHOST_SKIN_DEAD].yp = 98;

    //------------------------------------------
    Clyde_Skin[GHOST_SKIN_RIGHT1].xp = 2;
    Clyde_Skin[GHOST_SKIN_RIGHT1].yp = 110;

    Clyde_Skin[GHOST_SKIN_RIGHT2].xp = 2 + (1 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_RIGHT2].yp = 110;

    Clyde_Skin[GHOST_SKIN_LEFT1].xp = 2 + (2 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_LEFT1].yp = 110;

    Clyde_Skin[GHOST_SKIN_LEFT2].xp = 2 + (3 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_LEFT2].yp = 110;

    Clyde_Skin[GHOST_SKIN_UP1].xp = 2 + (4 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_UP1].yp = 110;

    Clyde_Skin[GHOST_SKIN_UP2].xp = 2 + (5 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_UP2].yp = 110;

    Clyde_Skin[GHOST_SKIN_DOWN1].xp = 2 + (6 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_DOWN1].yp = 110;

    Clyde_Skin[GHOST_SKIN_DOWN2].xp = 2 + (7 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_DOWN2].yp = 110;
    
    Clyde_Skin[GHOST_SKIN_FRIGHTEN1].xp = 3 + (8 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_FRIGHTEN1].yp = 69;

    Clyde_Skin[GHOST_SKIN_FRIGHTEN2].xp = 3 + (8 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_FRIGHTEN2].yp = 83;

    Clyde_Skin[GHOST_SKIN_DEAD].xp = 3 + (8 * BOTS_WIDTH);
    Clyde_Skin[GHOST_SKIN_DEAD].yp = 98;

}

//--------------------------------------------------------------
// init skin from gui (buttons)
//--------------------------------------------------------------
void skin_init_gui(void) {
    uint32_t n;

    for (n = 0; n < BUTTON_SKIN_CNT; n++) {
        Button_Skin[n].xp = 0;
        Button_Skin[n].yp = 0;
    }

    Button_Skin[BUTTON_SKIN1].xp = 83;
    Button_Skin[BUTTON_SKIN1].yp = 2;

    Button_Skin[BUTTON_SKIN2].xp = 83;
    Button_Skin[BUTTON_SKIN2].yp = 26;
}
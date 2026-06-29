//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "gui.h"

#include <stdio.h>
#include <string.h>
#include "stm32_ub_buzzer.h"

// Global variable definition (declared extern in header)
GUI_t GUI;

//--------------------------------------------------------------
// internal function
//--------------------------------------------------------------
void gui_draw_player(void);
void gui_clear_player(void);
void gui_draw_player2(void);
void gui_clear_player2(void);
void gui_draw_blinky(void);
void gui_clear_blinky(void);
static void gui_draw_ghost_sprite(Image2LCD_t koord, Skin_t *skin_table, uint32_t skin_idx,
                                uint32_t ghost_id, Ghost_t *ghost);
void gui_draw_pinky(void);
void gui_clear_pinky(void);
void gui_clear_inky(void);
void gui_draw_inky(void);
void gui_clear_clyde(void);
void gui_draw_clyde(void);
void gui_clear_humanghost(void);
void gui_draw_humanghost(void);

//--------------------------------------------------------------
// clear screen
//--------------------------------------------------------------
void gui_clear_screen(void) {
    UB_LCD_SetLayer_2();
    UB_LCD_SetTransparency(255);
    UB_LCD_FillLayer(BACKGROUND_COL);
}

//--------------------------------------------------------------
// clear player and all ghosts
//--------------------------------------------------------------
void gui_clear_bots(void) {
    gui_clear_player();
    if (Game.player2_active != 0) gui_clear_player2();
    if ((Game.ghost_active_mask & MOVE_BLINKY) != 0) gui_clear_blinky();
    if ((Game.ghost_active_mask & MOVE_PINKY) != 0) gui_clear_pinky();
    if ((Game.ghost_active_mask & MOVE_INKY) != 0) gui_clear_inky();
    if ((Game.ghost_active_mask & MOVE_CLYDE) != 0) gui_clear_clyde();
    if ((Game.ghost_active_mask & MOVE_HUMAN_GHOST) != 0) gui_clear_humanghost();
}

//--------------------------------------------------------------
// draw player and all ghosts
//--------------------------------------------------------------
void gui_draw_bots(void) {
    gui_draw_player();
    if (Game.player2_active != 0) gui_draw_player2();
    if ((Game.ghost_active_mask & MOVE_BLINKY) != 0) gui_draw_blinky();
    if ((Game.ghost_active_mask & MOVE_PINKY) != 0) gui_draw_pinky();
    if ((Game.ghost_active_mask & MOVE_INKY) != 0) gui_draw_inky();
    if ((Game.ghost_active_mask & MOVE_CLYDE) != 0) gui_draw_clyde();
    if ((Game.ghost_active_mask & MOVE_HUMAN_GHOST) != 0) gui_draw_humanghost();
}

//--------------------------------------------------------------
// draw maze in gameplay mode
//--------------------------------------------------------------
void gui_draw_maze(void) {
    uint32_t x, y, s;
    Image2LCD_t koord;

    gui_clear_screen();

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

//--------------------------------------------------------------
// draw maze in error mode
// color all rooms with error in red
//--------------------------------------------------------------
void gui_draw_errmaze(void) {
    uint32_t x, y;
    uint32_t xp, yp;

    gui_clear_screen();

    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            if (Maze.Room[x][y].deb_err == ROOM_DEB_OK) {
                if (Maze.Room[x][y].typ == ROOM_TYP_WALL) {
                    UB_Graphic2D_DrawFullRectDMA(xp, yp, ROOM_WIDTH, ROOM_HEIGHT, WALL_COL);
                } else {
                    UB_Graphic2D_DrawFullRectDMA(xp, yp, ROOM_WIDTH, ROOM_HEIGHT, ROOM_COL);
                }
            } else {
                UB_Graphic2D_DrawFullRectDMA(xp, yp, ROOM_WIDTH, ROOM_HEIGHT, ERR_COL);
            }
        }
    }
}

//--------------------------------------------------------------
// draw player
//--------------------------------------------------------------
void gui_draw_player(void) {
    extern uint32_t Player_Dying_Timer_ms;
    extern uint32_t Player_Invuln_Timer_ms;
    Image2LCD_t koord;
    uint32_t x, y, s;

    // --- HIỆU ỨNG NHẤP NHÁY ĐỎ KHI CHẾT ---
    if (Player.status == PLAYER_STATUS_DYING) {
        if ((Player_Dying_Timer_ms / 100) % 2 == 0) {
            x = Player.xp;
            y = Player.yp;
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Player.delta_x + BOTS_DIFF_X;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Player.delta_y + BOTS_DIFF_Y;
            if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
            if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
            koord.w = BOTS_WIDTH;
            koord.h = BOTS_HEIGHT;
            s = Player.skin;
            koord.source_xp = Player_Skin[s].xp;
            koord.source_yp = Player_Skin[s].yp;
            UB_Graphic2D_DrawImageRectRecolor(koord, RGB_COL_RED);
        }
        return;
    }

    if (Player.status != PLAYER_STATUS_ALIVE) {
        return;
    }

    // --- HIỆU ỨNG NHẤP NHÁY BẤT TỬ KHI HỒI SINH ---
    if (Player_Invuln_Timer_ms > 0) {
        if ((Player_Invuln_Timer_ms / 50) % 2 == 0) {
            return; // Bỏ qua không vẽ frame này
        }
    }

    x = Player.xp;
    y = Player.yp;

    if (Player.port != PORT_DONE) {
        // dont draw while porting
        // its easier not to draw than calc the port animation
    } else {
        koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Player.delta_x + BOTS_DIFF_X;
        koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Player.delta_y + BOTS_DIFF_Y;
        if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
        if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
        koord.w = BOTS_WIDTH;
        koord.h = BOTS_HEIGHT;
        s = Player.skin;
        koord.source_xp = Player_Skin[s].xp;
        koord.source_yp = Player_Skin[s].yp;
        UB_Graphic2D_DrawImageRect(koord);
    }
}

//--------------------------------------------------------------
// clear player (repaint all rooms around the player pos)
//--------------------------------------------------------------
void gui_clear_player(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax; // signed int !!
    int16_t ymin, ymax; // signed int !!

    xp = Player.xp;
    yp = Player.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (Player.move == MOVE_UP) ymax = yp + 2;
    if (Player.move == MOVE_RIGHT) xmin = xp - 2;
    if (Player.move == MOVE_DOWN) ymin = yp - 2;
    if (Player.move == MOVE_LEFT) xmax = xp + 2;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

void gui_draw_player2(void) {
    extern uint32_t Player2_Dying_Timer_ms;
    extern uint32_t Player2_Invuln_Timer_ms;
    Image2LCD_t koord;
    uint32_t x, y, s;

    // --- HIỆU ỨNG NHẤP NHÁY ĐỎ KHI CHẾT ---
    if (Player2.status == PLAYER_STATUS_DYING) {
        if ((Player2_Dying_Timer_ms / 100) % 2 == 0) {
            x = Player2.xp;
            y = Player2.yp;
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Player2.delta_x + BOTS_DIFF_X;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Player2.delta_y + BOTS_DIFF_Y;
            if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
            if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
            koord.w = BOTS_WIDTH;
            koord.h = BOTS_HEIGHT;
            s = Player2.skin;
            koord.source_xp = Player_Skin[s].xp;
            koord.source_yp = Player_Skin[s].yp;
            UB_Graphic2D_DrawImageRectRecolor(koord, RGB_COL_RED);
        }
        return;
    }

    if (Player2.status != PLAYER_STATUS_ALIVE) {
        return;
    }

    // --- HIỆU ỨNG NHẤP NHÁY BẤT TỬ KHI HỒI SINH ---
    if (Player2_Invuln_Timer_ms > 0) {
        if ((Player2_Invuln_Timer_ms / 50) % 2 == 0) {
            return; // Bỏ qua không vẽ frame này
        }
    }

    x = Player2.xp;
    y = Player2.yp;

    if (Player2.port != PORT_DONE) {
        return;
    }
    koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Player2.delta_x + BOTS_DIFF_X;
    koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Player2.delta_y + BOTS_DIFF_Y;
    if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
    if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
    koord.w = BOTS_WIDTH;
    koord.h = BOTS_HEIGHT;
    s = Player2.skin;
    koord.source_xp = Player_Skin[s].xp;
    koord.source_yp = Player_Skin[s].yp;
    UB_Graphic2D_DrawImageRectRecolor(koord, PLAYER2_BODY_COLOR);
}

void gui_clear_player2(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax;
    int16_t ymin, ymax;

    xp = Player2.xp;
    yp = Player2.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (Player2.move == MOVE_UP) ymax = yp + 2;
    if (Player2.move == MOVE_RIGHT) xmin = xp - 2;
    if (Player2.move == MOVE_DOWN) ymin = yp - 2;
    if (Player2.move == MOVE_LEFT) xmax = xp + 2;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

static void gui_draw_ghost_sprite(Image2LCD_t koord, Skin_t *skin_table, uint32_t skin_idx,
                                uint32_t ghost_id, Ghost_t *ghost) {
    uint16_t body_color;

    koord.source_xp = skin_table[skin_idx].xp;
    koord.source_yp = skin_table[skin_idx].yp;
    if ((skin_idx < GHOST_SKIN_FRIGHTEN1) &&
        (bot_ghost_get_body_color(ghost_id, ghost, &body_color) != 0)) {
        UB_Graphic2D_DrawImageRectRecolor(koord, body_color);
    } else {
        UB_Graphic2D_DrawImageRect(koord);
    }
}

//--------------------------------------------------------------
// draw bot : Blinky
//--------------------------------------------------------------
void gui_draw_blinky(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;

    x = Blinky.xp;
    y = Blinky.yp;

    if (Blinky.port != PORT_DONE) {
        // dont draw while porting
        // its easier not to draw than calc the port animation
    } else {
        koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Blinky.delta_x + BOTS_DIFF_X;
        koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Blinky.delta_y + BOTS_DIFF_Y;
        if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
        if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
        koord.w = BOTS_WIDTH;
        koord.h = BOTS_HEIGHT;
        s = Blinky.skin;
        gui_draw_ghost_sprite(koord, Blinky_Skin, s, GHOST_BLINKY, &Blinky);
    }
}

//--------------------------------------------------------------
// clear bot : Blinky (repaint all rooms around the bot pos)
//--------------------------------------------------------------
void gui_clear_blinky(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax; // signed int !!
    int16_t ymin, ymax; // signed int !!

    xp = Blinky.xp;
    yp = Blinky.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

//--------------------------------------------------------------
// draw bot : Pinky
//--------------------------------------------------------------
void gui_draw_pinky(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;

    x = Pinky.xp;
    y = Pinky.yp;

    if (Pinky.port != PORT_DONE) {
        // dont draw while porting
        // its easier not to draw than calc the port animation
    } else {
        koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Pinky.delta_x + BOTS_DIFF_X;
        koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Pinky.delta_y + BOTS_DIFF_Y;
        if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
        if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
        koord.w = BOTS_WIDTH;
        koord.h = BOTS_HEIGHT;
        s = Pinky.skin;
        gui_draw_ghost_sprite(koord, Pinky_Skin, s, GHOST_PINKY, &Pinky);
    }
}

//--------------------------------------------------------------
// clear bot : Pinky (repaint all rooms around the bot pos)
//--------------------------------------------------------------
void gui_clear_pinky(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax; // signed int !!
    int16_t ymin, ymax; // signed int !!

    xp = Pinky.xp;
    yp = Pinky.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

//--------------------------------------------------------------
// draw bot : Inky
//--------------------------------------------------------------
void gui_draw_inky(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;

    x = Inky.xp;
    y = Inky.yp;

    if (Inky.port != PORT_DONE) {
        // dont draw while porting
        // its easier not to draw than calc the port animation
    } else {
        koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Inky.delta_x + BOTS_DIFF_X;
        koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Inky.delta_y + BOTS_DIFF_Y;
        if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
        if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
        koord.w = BOTS_WIDTH;
        koord.h = BOTS_HEIGHT;
        s = Inky.skin;
        gui_draw_ghost_sprite(koord, Inky_Skin, s, GHOST_INKY, &Inky);
    }
}

//--------------------------------------------------------------
// clear bot : Inky (repaint all rooms around the bot pos)
//--------------------------------------------------------------
void gui_clear_inky(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax; // signed int !!
    int16_t ymin, ymax; // signed int !!

    xp = Inky.xp;
    yp = Inky.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

//--------------------------------------------------------------
// draw bot : Clyde
//--------------------------------------------------------------
void gui_draw_clyde(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;

    x = Clyde.xp;
    y = Clyde.yp;

    if (Clyde.port != PORT_DONE) {
        // don't draw while porting
        // its easier not to draw than calc the port animation
    } else {
        koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + Clyde.delta_x + BOTS_DIFF_X;
        koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + Clyde.delta_y + BOTS_DIFF_Y;
        if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
        if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
        koord.w = BOTS_WIDTH;
        koord.h = BOTS_HEIGHT;
        s = Clyde.skin;
        gui_draw_ghost_sprite(koord, Clyde_Skin, s, GHOST_CLYDE, &Clyde);
    }
}

//--------------------------------------------------------------
// clear bot : Clyde (repaint all rooms around the bot pos)
//--------------------------------------------------------------
void gui_clear_clyde(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax; // signed int !!
    int16_t ymin, ymax; // signed int !!

    xp = Clyde.xp;
    yp = Clyde.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

//--------------------------------------------------------------
// draw bot : HumanGhost (player 2 controlled)
//--------------------------------------------------------------
void gui_draw_humanghost(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;

    x = HumanGhost.xp;
    y = HumanGhost.yp;

    if (HumanGhost.port != PORT_DONE) {
    } else {
        koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX + HumanGhost.delta_x + BOTS_DIFF_X;
        koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY + HumanGhost.delta_y + BOTS_DIFF_Y;
        if (koord.dest_xp < GUI_MAZE_STARTX) koord.dest_xp = GUI_MAZE_STARTX;
        if (koord.dest_yp < GUI_MAZE_STARTY) koord.dest_yp = GUI_MAZE_STARTY;
        koord.w = BOTS_WIDTH;
        koord.h = BOTS_HEIGHT;
        s = HumanGhost.skin;
        gui_draw_ghost_sprite(koord, Blinky_Skin, s, GHOST_HUMAN, &HumanGhost);
    }
}

//--------------------------------------------------------------
// clear bot : HumanGhost
//--------------------------------------------------------------
void gui_clear_humanghost(void) {
    Image2LCD_t koord;
    uint32_t x, y, s;
    uint32_t xp, yp;
    int16_t xmin, xmax;
    int16_t ymin, ymax;

    xp = HumanGhost.xp;
    yp = HumanGhost.yp;

    xmin = xp - 1;
    xmax = xp + 1;
    ymin = yp - 1;
    ymax = yp + 1;

    if (xmin < 0) xmin = 0;
    if (xmax >= ROOM_CNT_X) xmax = ROOM_CNT_X - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= ROOM_CNT_Y) ymax = ROOM_CNT_Y - 1;

    koord.w = ROOM_WIDTH;
    koord.h = ROOM_HEIGHT;

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            koord.dest_xp = (x * ROOM_WIDTH) + GUI_MAZE_STARTX;
            koord.dest_yp = (y * ROOM_HEIGHT) + GUI_MAZE_STARTY;
            s = Maze.Room[x][y].skin;
            koord.source_xp = Room_Skin[s].xp;
            koord.source_yp = Room_Skin[s].yp;
            UB_Graphic2D_DrawImageRect(koord);
        }
    }
}

//--------------------------------------------------------------
// draw gui (buttons, text ...)
//--------------------------------------------------------------
void gui_draw_gui(uint32_t joy) {
    char buf[20];

    if (GUI.refresh_value > 0) {
        GUI.refresh_value--;

        if (Game.player2_active != 0) {
            if (Game.play_type == GAME_PLAY_CAMPAIGN) {
                sprintf(buf, "L: %d", (int)(Player.level));
                UB_Font_DrawString(92, 260, buf, & Arial_7x10, FONT_COL3, BACKGROUND_COL);
            }

            sprintf(buf, "S: %d", (int)(Player.score));
            UB_Font_DrawString(92, 275, buf, & Arial_7x10, FONT_COL3, BACKGROUND_COL);

            sprintf(buf, "P1:%d P2:%d", (int)(Player.lives), (int)(Player2.lives));
            UB_Font_DrawString(90, 290, buf, & Arial_7x10, FONT_COL3, BACKGROUND_COL);
        } else {
            if (Game.play_type == GAME_PLAY_CAMPAIGN) {
                sprintf(buf, "level : %d", (int)(Player.level));
                UB_Font_DrawString(10, 260, buf, & Arial_7x10, FONT_COL3, BACKGROUND_COL);
            }

            sprintf(buf, "score : %d", (int)(Player.score));
            UB_Font_DrawString(10, 275, buf, & Arial_7x10, FONT_COL3, BACKGROUND_COL);

            sprintf(buf, "lives : %d", (int)(Player.lives));
            UB_Font_DrawString(10, 290, buf, & Arial_7x10, FONT_COL3, BACKGROUND_COL);
        }

        if (Player.status == PLAYER_STATUS_WIN) {
            if (Game.play_type == GAME_PLAY_CUSTOM) {
                UB_Font_DrawString(10, 305, "you win!", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            } else {
                UB_Font_DrawString(10, 305, "level complete", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            }
        } else if (Game.player2_active != 0 && bot_coop_is_game_over() != 0) {
            UB_Font_DrawString(10, 305, "GAME OVER", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
        } else if (Player.status == PLAYER_STATUS_DEAD) {
            if (Player.lives >= 1) {
                UB_Font_DrawString(10, 305, "P1 respawn", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            } else if (Game.player2_active == 0) {
                if (Player.lives >= 2) {
                    UB_Font_DrawString(10, 305, "player lose", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
                } else {
                    UB_Font_DrawString(10, 305, "GAME OVER", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
                }
            }
        } else if (Game.player2_active != 0 && Player2.status == PLAYER_STATUS_DEAD && Player2.lives == 0) {
            UB_Font_DrawString(10, 305, "P2 out", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
        }
    }

    // In thông tin debug joystick lên màn hình
    {
        extern uint16_t debug_joy1_x, debug_joy1_y, debug_joy2_x, debug_joy2_y;
        extern int32_t joy1_center_x, joy1_center_y, joy2_center_x, joy2_center_y;
        char dbg_buf[32];
        
        // Joystick 1: Giá trị thô
        sprintf(dbg_buf, "J1:%4u %4u", debug_joy1_x, debug_joy1_y);
        UB_Font_DrawString(10, 310, dbg_buf, &Arial_7x10, FONT_COL2, BACKGROUND_COL);
        
        // Joystick 2: Giá trị thô
        sprintf(dbg_buf, "J2:%4u %4u", debug_joy2_x, debug_joy2_y);
        UB_Font_DrawString(120, 310, dbg_buf, &Arial_7x10, FONT_COL2, BACKGROUND_COL);
    }

    if (GUI.refresh_buttons > 0) {
        GUI.refresh_buttons--;
        gui_draw_buttons(joy);
    }
}

//--------------------------------------------------------------
// draw all buttons
//--------------------------------------------------------------
void gui_draw_buttons(uint32_t joy) {
    Image2LCD_t koord;
    uint32_t su, sd, sr, sl;

    if (Game.player2_active != 0 || bot_is_2p_vs_ghost() != 0) {
        // --- PLAYER 1 D-PAD (LEFT SIDE - COMPACT) ---
        su = BUTTON_SKIN1;
        sd = BUTTON_SKIN1;
        sr = BUTTON_SKIN1;
        sl = BUTTON_SKIN1;

        if (joy == GUI_JOY_UP) su = BUTTON_SKIN2;
        if (joy == GUI_JOY_LEFT) sl = BUTTON_SKIN2;
        if (joy == GUI_JOY_DOWN) sd = BUTTON_SKIN2;
        if (joy == GUI_JOY_RIGHT) sr = BUTTON_SKIN2;

        // Button up
        koord.dest_xp = 40;
        koord.dest_yp = 255;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[su].xp;
        koord.source_yp = Button_Skin[su].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 3);

        // Button down
        koord.dest_xp = 40;
        koord.dest_yp = 280;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sd].xp;
        koord.source_yp = Button_Skin[sd].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 2);

        // Button right
        koord.dest_xp = 62; // Compacted from 70
        koord.dest_yp = 270;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sr].xp;
        koord.source_yp = Button_Skin[sr].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 0);

        // Button left
        koord.dest_xp = 18; // Compacted from 10
        koord.dest_yp = 270;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sl].xp;
        koord.source_yp = Button_Skin[sl].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 1);

        // --- PLAYER 2 D-PAD (RIGHT SIDE - COMPACT) ---
        su = BUTTON_SKIN1;
        sd = BUTTON_SKIN1;
        sr = BUTTON_SKIN1;
        sl = BUTTON_SKIN1;

        uint32_t joy2 = Game.player2_joy;
        if (joy2 == GUI_JOY_UP) su = BUTTON_SKIN2;
        if (joy2 == GUI_JOY_LEFT) sl = BUTTON_SKIN2;
        if (joy2 == GUI_JOY_DOWN) sd = BUTTON_SKIN2;
        if (joy2 == GUI_JOY_RIGHT) sr = BUTTON_SKIN2;

        // Button up
        koord.dest_xp = 178; // Shifted from 170
        koord.dest_yp = GUI_BTN_UP_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[su].xp;
        koord.source_yp = Button_Skin[su].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 3);

        // Button down
        koord.dest_xp = 178; // Shifted from 170
        koord.dest_yp = GUI_BTN_DOWN_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sd].xp;
        koord.source_yp = Button_Skin[sd].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 2);

        // Button right
        koord.dest_xp = 200; // Shifted from 192
        koord.dest_yp = GUI_BTN_RIGHT_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sr].xp;
        koord.source_yp = Button_Skin[sr].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 0);

        // Button left
        koord.dest_xp = 156; // Shifted from 148
        koord.dest_yp = GUI_BTN_LEFT_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sl].xp;
        koord.source_yp = Button_Skin[sl].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 1);
    } else {
        // --- SINGLE D-PAD (RIGHT SIDE - COMPACT) ---
        su = BUTTON_SKIN1;
        sd = BUTTON_SKIN1;
        sr = BUTTON_SKIN1;
        sl = BUTTON_SKIN1;

        if (joy == GUI_JOY_UP) su = BUTTON_SKIN2;
        if (joy == GUI_JOY_LEFT) sl = BUTTON_SKIN2;
        if (joy == GUI_JOY_DOWN) sd = BUTTON_SKIN2;
        if (joy == GUI_JOY_RIGHT) sr = BUTTON_SKIN2;

        // Button up
        koord.dest_xp = 178; // Shifted from 170
        koord.dest_yp = GUI_BTN_UP_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[su].xp;
        koord.source_yp = Button_Skin[su].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 3);

        // Button down
        koord.dest_xp = 178; // Shifted from 170
        koord.dest_yp = GUI_BTN_DOWN_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sd].xp;
        koord.source_yp = Button_Skin[sd].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 2);

        // Button right
        koord.dest_xp = 200; // Shifted from 192
        koord.dest_yp = GUI_BTN_RIGHT_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sr].xp;
        koord.source_yp = Button_Skin[sr].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 0);

        // Button left
        koord.dest_xp = 156; // Shifted from 148
        koord.dest_yp = GUI_BTN_LEFT_Y;
        koord.w = BUTTON_WIDTH;
        koord.h = BUTTON_HEIGHT;
        koord.source_xp = Button_Skin[sl].xp;
        koord.source_yp = Button_Skin[sl].yp;
        UB_Graphic2D_DrawImageRectRotate(koord, 1);
    }
}

//--------------------------------------------------------------
// check 4 buttons on the touch
//--------------------------------------------------------------
uint32_t gui_check_touch(void) {
    return GUI_JOY_NONE;
}

//--------------------------------------------------------------
// check 4 external buttons
// if button aktiv = GND -> "BTN_RELEASED"
// if button aktiv = VCC -> "BTN_PRESSED"
//--------------------------------------------------------------
uint32_t gui_check_button(void) {
    uint32_t ret_wert = GUI_JOY_NONE;
    static uint32_t old_button = 999;

    if (UB_Button_Read(BTN_UP) == BTN_RELEASED) {
        ret_wert = GUI_JOY_UP;
    }
    if (UB_Button_Read(BTN_RIGHT) == BTN_RELEASED) {
        ret_wert = GUI_JOY_RIGHT;
    }
    if (UB_Button_Read(BTN_DOWN) == BTN_RELEASED) {
        ret_wert = GUI_JOY_DOWN;
    }
    if (UB_Button_Read(BTN_LEFT) == BTN_RELEASED) {
        ret_wert = GUI_JOY_LEFT;
    }

    if (old_button != ret_wert) {
        old_button = ret_wert;
        GUI.refresh_buttons = GUI_REFRESH_VALUE;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// check analog joystick 1 (VRx/VRy via ADC)
//--------------------------------------------------------------
uint32_t gui_check_joystick1(void) {
    uint32_t ret_wert = GUI_JOY_NONE;
    static uint32_t old_button1 = 999;

#if JOYSTICK_USE_ADC == 1
    ret_wert = UB_Joystick1_ReadDirection();
#endif

    if (old_button1 != ret_wert) {
        old_button1 = ret_wert;
        GUI.refresh_buttons = GUI_REFRESH_VALUE;
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// check analog joystick 2 (VRx/VRy via ADC)
//--------------------------------------------------------------
uint32_t gui_check_joystick2(void) {
    uint32_t ret_wert = GUI_JOY_NONE;
    static uint32_t old_button2 = 999;

#if JOYSTICK_USE_ADC == 1
    ret_wert = UB_Joystick2_ReadDirection();
#endif

    if (old_button2 != ret_wert) {
        old_button2 = ret_wert;
        GUI.refresh_buttons = GUI_REFRESH_VALUE;
    }

    return (ret_wert);
}
//--------------------------------------------------------------
// debug UART
//--------------------------------------------------------------
void gui_debug_uart(char * ptr) {
    #if USE_GUI_UART_DEBUG == 1
    UB_Uart_SendString(COM1, ptr, CRLF); // for DEBUG
    #endif
}

//--------------------------------------------------------------
// Scale drawing and result screens implementation
//--------------------------------------------------------------
void gui_draw_char_scale(uint16_t x, uint16_t y, uint8_t ascii, UB_Font *font, uint16_t vg, uint16_t bg, uint8_t scale) {
    uint16_t xn, yn, start_maske, maske;
    const uint16_t *wert;

    ascii -= 32;
    wert = &font->table[ascii * font->height];

    start_maske = 0x80;
    if (font->width > 8) start_maske = 0x8000;

    for (yn = 0; yn < font->height; yn++) {
        maske = start_maske;
        for (xn = 0; xn < font->width; xn++) {
            uint16_t color = ((wert[yn] & maske) == 0x00) ? bg : vg;
            if (color != bg) {
                UB_Graphic2D_DrawFullRectDMA(x + xn * scale, y + yn * scale, scale, scale, color);
            } else {
                UB_Graphic2D_DrawFullRectDMA(x + xn * scale, y + yn * scale, scale, scale, bg);
            }
            maske = (maske >> 1);
        }
    }
}

void gui_draw_string_scale(uint16_t x, uint16_t y, char *ptr, UB_Font *font, uint16_t vg, uint16_t bg, uint8_t scale) {
    uint16_t pos = x;
    while (*ptr != 0) {
        gui_draw_char_scale(pos, y, *ptr, font, vg, bg, scale);
        pos += font->width * scale;
        ptr++;
    }
}

void gui_show_countdown_text(const char *text, uint8_t scale) {
    uint16_t text_w = (uint16_t)(strlen(text) * 7 * scale);
    uint16_t text_h = (uint16_t)(10 * scale);
    uint16_t x = (240 - text_w) / 2;
    uint16_t y = GUI_MAZE_STARTY + (ROOM_CNT_Y * ROOM_HEIGHT) / 2 - text_h / 2;
    uint16_t clear_w = 160;
    uint16_t clear_h = 44;
    uint16_t clear_x = (240 - clear_w) / 2;
    uint16_t clear_y = y + text_h / 2 - clear_h / 2;

    UB_Graphic2D_DrawFullRectDMA(clear_x, clear_y, clear_w, clear_h, BACKGROUND_COL);
    gui_draw_string_scale(x, y, (char *)text, &Arial_7x10, FONT_COL2, BACKGROUND_COL, scale);
}

void gui_wait_interaction(void) {
    UB_Systick_Pause_ms(500); // Prevent accidental double clicks
    
    // Clear previous center button click state
    UB_Button_OnClick(BTN_CENTER);
    
    while (1) {
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Systick_Pause_ms(200); // Debounce before menu reads buttons
            UB_Button_OnClick(BTN_CENTER); // Consume press so menu does not auto-select
            break;
        }
        UB_Systick_Pause_ms(30);
    }
}



static void gui_pause_prepare_frame(void) {
    if (LCD_CurrentLayer == 0) {
        UB_LCD_Copy_Layer2_to_Layer1();
    } else {
        UB_LCD_Copy_Layer1_to_Layer2();
    }
}

void gui_resume_from_pause(uint32_t joy) {
    gui_draw_maze();
    gui_draw_bots();
    GUI.refresh_value = GUI_REFRESH_VALUE;
    GUI.refresh_buttons = GUI_REFRESH_VALUE;
    gui_draw_gui(joy);
    UB_LCD_Copy_Layer2_to_Layer1();
    UB_LCD_Refresh();
}

static void gui_draw_pause_overlay(uint32_t sel) {
    uint16_t title_w = strlen("PAUSED") * 7 * 2;

    UB_Graphic2D_DrawFullRectDMA(GUI_PAUSE_BOX_X, GUI_PAUSE_BOX_Y, GUI_PAUSE_BOX_W, GUI_PAUSE_BOX_H, RGB_COL_BLACK);
    UB_Graphic2D_DrawRectDMA(GUI_PAUSE_BOX_X, GUI_PAUSE_BOX_Y, GUI_PAUSE_BOX_W, GUI_PAUSE_BOX_H, RGB_COL_YELLOW);
    UB_Graphic2D_DrawRectDMA(GUI_PAUSE_BOX_X + 3, GUI_PAUSE_BOX_Y + 3, GUI_PAUSE_BOX_W - 6, GUI_PAUSE_BOX_H - 6, RGB_COL_BLUE);
    gui_draw_string_scale((240 - title_w) / 2, 95, "PAUSED", &Arial_7x10, RGB_COL_YELLOW, RGB_COL_BLACK, 2);

    UB_Graphic2D_DrawRectDMA(GUI_PAUSE_CONTINUE_X, GUI_PAUSE_BTN_Y, GUI_PAUSE_BTN_W, GUI_PAUSE_BTN_H,
                             (sel == 0) ? RGB_COL_GREEN : RGB_COL_BLUE);
    UB_Font_DrawString(GUI_PAUSE_CONTINUE_X + 8, GUI_PAUSE_BTN_Y + 9, "Continue", &Arial_7x10,
                       RGB_COL_WHITE, RGB_COL_BLACK);

    UB_Graphic2D_DrawRectDMA(GUI_PAUSE_EXIT_X, GUI_PAUSE_BTN_Y, GUI_PAUSE_BTN_W, GUI_PAUSE_BTN_H,
                             (sel == 1) ? RGB_COL_RED : RGB_COL_BLUE);
    UB_Font_DrawString(GUI_PAUSE_EXIT_X + 22, GUI_PAUSE_BTN_Y + 9, "Exit", &Arial_7x10,
                       RGB_COL_WHITE, RGB_COL_BLACK);

    UB_Font_DrawString(35, 245, "UP/DOWN + CENTER", &Arial_7x10, FONT_COL2, RGB_COL_BLACK);
    UB_Font_DrawString(55, 260, "or tap buttons", &Arial_7x10, FONT_COL3, RGB_COL_BLACK);
}

uint32_t gui_run_pause_menu(void) {
    uint32_t sel = 0;

    UB_Game_Timers_Paused = 1;
    gui_pause_prepare_frame();
    gui_draw_pause_overlay(sel);
    UB_LCD_Refresh();

    while (1) {
        if (UB_Button_OnClick(BTN_UP) || UB_Button_OnClick(BTN_DOWN)) {
            sel = 1 - sel;
            gui_pause_prepare_frame();
            gui_draw_pause_overlay(sel);
            UB_LCD_Refresh();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Game_Timers_Paused = 0;
            if (sel == 0) {
                gui_resume_from_pause(GUI_JOY_NONE);
                return GUI_PAUSE_CONTINUE;
            }
            return GUI_PAUSE_EXIT;
        }

        UB_Systick_Pause_ms(30);
    }
}

void gui_show_win_screen(uint32_t score) {
    char buf[32];
    
    UB_Graphic2D_ClearSreenDMA(RGB_COL_BLACK);
    
    // Double border decoration
    UB_Graphic2D_DrawRectDMA(10, 10, 220, 300, RGB_COL_GREEN);
    UB_Graphic2D_DrawRectDMA(13, 13, 214, 294, RGB_COL_YELLOW);
    
    // YOU WIN! text
    gui_draw_string_scale(36, 60, "YOU WIN!", &Arial_7x10, RGB_COL_GREEN, RGB_COL_BLACK, 3);
    
    // Draw a big Pacman character at center (120, 140), r=20
    UB_Graphic2D_DrawFullCircleDMA(120, 140, 20, RGB_COL_YELLOW);
    UB_Graphic2D_DrawFullCircleDMA(125, 130, 3, RGB_COL_BLACK); // eye
    // Pacman dots
    UB_Graphic2D_DrawFullCircleDMA(160, 140, 4, RGB_COL_YELLOW);
    UB_Graphic2D_DrawFullCircleDMA(185, 140, 4, RGB_COL_YELLOW);
    
    // Score
    sprintf(buf, "SCORE: %u", (unsigned int)score);
    uint16_t score_w = strlen(buf) * 7 * 2;
    gui_draw_string_scale((240 - score_w) / 2, 200, buf, &Arial_7x10, RGB_COL_WHITE, RGB_COL_BLACK, 2);
    
    if (Game.play_type == GAME_PLAY_CAMPAIGN) {
        sprintf(buf, "LEVEL: %u", (unsigned int)Player.level);
        uint16_t lvl_w = strlen(buf) * 7 * 2;
        gui_draw_string_scale((240 - lvl_w) / 2, 230, buf, &Arial_7x10, RGB_COL_CYAN, RGB_COL_BLACK, 2);
    }
    
    // Interaction instructions
    UB_Font_DrawString((240 - strlen("PRESS CENTER BTN TO CONTINUE") * 7) / 2, 280, "PRESS CENTER BTN TO CONTINUE", &Arial_7x10, RGB_COL_YELLOW, RGB_COL_BLACK);
    
    UB_LCD_Refresh();
    
    // Play win theme (blocking melody)
    UB_Buzzer_Play_Win();
    
    gui_wait_interaction();
}

void gui_show_lost_screen(uint32_t score) {
    char buf[32];
    
    UB_Graphic2D_ClearSreenDMA(RGB_COL_BLACK);
    
    // Double border decoration
    UB_Graphic2D_DrawRectDMA(10, 10, 220, 300, RGB_COL_RED);
    UB_Graphic2D_DrawRectDMA(13, 13, 214, 294, RGB_COL_GREY);
    
    // YOU LOST text
    gui_draw_string_scale(36, 60, "YOU LOST", &Arial_7x10, RGB_COL_RED, RGB_COL_BLACK, 3);
    
    // Draw a Blinky ghost at center
    UB_Graphic2D_DrawFullCircleDMA(120, 135, 16, RGB_COL_RED);
    UB_Graphic2D_DrawFullRectDMA(104, 135, 32, 16, RGB_COL_RED);
    UB_Graphic2D_DrawFullCircleDMA(113, 132, 4, RGB_COL_WHITE); // left eye
    UB_Graphic2D_DrawFullCircleDMA(127, 132, 4, RGB_COL_WHITE); // right eye
    UB_Graphic2D_DrawFullCircleDMA(114, 132, 2, RGB_COL_BLUE);  // pupils
    UB_Graphic2D_DrawFullCircleDMA(128, 132, 2, RGB_COL_BLUE);
    
    // Score
    sprintf(buf, "SCORE: %u", (unsigned int)score);
    uint16_t score_w = strlen(buf) * 7 * 2;
    gui_draw_string_scale((240 - score_w) / 2, 200, buf, &Arial_7x10, RGB_COL_WHITE, RGB_COL_BLACK, 2);
    
    if (Game.play_type == GAME_PLAY_CAMPAIGN) {
        sprintf(buf, "LEVEL: %u", (unsigned int)Player.level);
        uint16_t lvl_w = strlen(buf) * 7 * 2;
        gui_draw_string_scale((240 - lvl_w) / 2, 230, buf, &Arial_7x10, RGB_COL_GREY, RGB_COL_BLACK, 2);
    }
    
    // Interaction instructions
    UB_Font_DrawString((240 - strlen("PRESS CENTER BTN TO CONTINUE") * 7) / 2, 280, "PRESS CENTER BTN TO CONTINUE", &Arial_7x10, RGB_COL_RED, RGB_COL_BLACK);
    
    UB_LCD_Refresh();
    
    // Play lost theme (blocking melody)
    UB_Buzzer_Play_Lost();
    
    gui_wait_interaction();
}

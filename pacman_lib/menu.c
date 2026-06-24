//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "menu.h"

#include <stdlib.h>
#include <stdio.h>

#include "bot.h"
#include "maze.h"

//--------------------------------------------------------------
// wizard step ids
//--------------------------------------------------------------
#define WIZ_MODE 0
#define WIZ_MAP 1
#define WIZ_SPEED 2
#define WIZ_2P_TYPE 3
#define WIZ_GHOST_COUNT 4
#define WIZ_GHOST_SETUP 5

#define WIZ_STEP_COUNT 6

//--------------------------------------------------------------
// intern helpers
//--------------------------------------------------------------
static uint32_t menu_touch_pressed(uint16_t *xp, uint16_t *yp);
static uint32_t menu_touch_in_rect(uint16_t tx, uint16_t ty, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
static void menu_touch_wait_release(void);
static void menu_draw_nav_buttons(uint32_t show_back, const char *next_label);
static void menu_draw_main(uint32_t sel);
static uint32_t menu_run_main(void);
static void menu_custom_defaults(void);
static void menu_draw_wizard(uint32_t step, uint32_t ghost_idx);
static uint32_t menu_run_custom_wizard(void);
static uint32_t menu_cycle_value(uint32_t val, uint32_t min_v, uint32_t max_v, int32_t dir);

//--------------------------------------------------------------
// entry: main menu then optional custom wizard
//--------------------------------------------------------------
uint32_t menu_start(void) {
    uint32_t result;
    uint32_t srand_init = 0;

    GUI.refresh_value = GUI_REFRESH_VALUE;
    GUI.refresh_buttons = GUI_REFRESH_VALUE;

    menu_custom_defaults();

    do {
        srand_init++;
        result = menu_run_main();
    } while (result == MENU_RESULT_NONE);

    if (result == MENU_RESULT_CUSTOM) {
        while (menu_run_custom_wizard() == 0) {
            srand_init++;
            result = menu_run_main();
            if (result != MENU_RESULT_CUSTOM) {
                Game.play_type = GAME_PLAY_CAMPAIGN;
                srand(srand_init);
                return result;
            }
        }
        Game.play_type = GAME_PLAY_CUSTOM;
    } else {
        Game.play_type = GAME_PLAY_CAMPAIGN;
        Game.debug_mode = 0;
    }

    srand(srand_init);
    return result;
}

static void menu_custom_defaults(void) {
    uint32_t i;

    Game.custom.player_count = CUSTOM_PLAYER_1;
    Game.custom.map_id = MAZE_MAP_CLASSIC;
    Game.custom.ghost_speed_idx = CUSTOM_SPEED_NORMAL;
    Game.custom.two_player_mode = CUSTOM_2P_COOP;
    Game.custom.ghost_count = 1;
    for (i = 0; i < CUSTOM_MAX_GHOSTS; i++) {
        Game.custom.ghost_strategies[i] = GHOST_STRATEGY_BLINKY;
    }
}

static uint32_t menu_touch_pressed(uint16_t *xp, uint16_t *yp) {
    UB_Touch_Read();
    *xp = Touch_Data.xp;
    *yp = Touch_Data.yp;
    return (Touch_Data.status == TOUCH_PRESSED) ? 1 : 0;
}

static uint32_t menu_touch_in_rect(uint16_t tx, uint16_t ty, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (tx >= x && tx < (x + w) && ty >= y && ty < (y + h)) {
        return 1;
    }
    return 0;
}

static void menu_touch_wait_release(void) {
    uint16_t xp;
    uint16_t yp;

    while (menu_touch_pressed(&xp, &yp) != 0) {
        UB_Systick_Pause_ms(20);
    }
    UB_Systick_Pause_ms(120);
}

static void menu_draw_nav_buttons(uint32_t show_back, const char *next_label) {
    if (show_back != 0) {
        UB_Font_DrawString(MENU_BTN_BACK_X, MENU_BTN_Y, "Back", &Arial_7x10, MENUE_COL_OFF, BACKGROUND_COL);
    } else {
        UB_Font_DrawString(MENU_BTN_BACK_X, MENU_BTN_Y, "     ", &Arial_7x10, MENUE_COL_OFF, BACKGROUND_COL);
    }
    UB_Font_DrawString(MENU_BTN_NEXT_X, MENU_BTN_Y, next_label, &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
}

static uint32_t menu_cycle_value(uint32_t val, uint32_t min_v, uint32_t max_v, int32_t dir) {
    if (dir > 0) {
        if (val < max_v) {
            return val + 1;
        }
        return min_v;
    }
    if (val > min_v) {
        return val - 1;
    }
    return max_v;
}

static void menu_draw_main(uint32_t sel) {
    gui_clear_screen();
    UB_Font_DrawString(50, 5, "   GAME PACMAN", &Arial_7x10, FONT_COL, BACKGROUND_COL);
    UB_Font_DrawString(20, 18, " Project CKI VN K68 Embedded", &Arial_7x10, FONT_COL, BACKGROUND_COL);
    UB_Font_DrawString(MENUE_STARTX1, MENUE_STARTY, "Campaign", &Arial_7x10, (sel == 0) ? MENUE_COL_ON : MENUE_COL_OFF, BACKGROUND_COL);
    UB_Font_DrawString(MENUE_STARTX1, MENUE_STARTY + MENUE_DELTA1, "Custom", &Arial_7x10, (sel == 1) ? MENUE_COL_ON : MENUE_COL_OFF, BACKGROUND_COL);
    UB_Font_DrawString(MENUE_STARTX1, 120, "Tap to select", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
}

static uint32_t menu_run_main(void) {
    uint32_t sel = 0;
    uint16_t tx;
    uint16_t ty;

    menu_draw_main(sel);

    while (1) {
        if (menu_touch_pressed(&tx, &ty) != 0) {
            if (menu_touch_in_rect(tx, ty, MENUE_STARTX1, MENUE_STARTY, 200, 18)) {
                menu_touch_wait_release();
                return MENU_RESULT_CAMPAIGN;
            }
            if (menu_touch_in_rect(tx, ty, MENUE_STARTX1, MENUE_STARTY + MENUE_DELTA1, 200, 18)) {
                menu_touch_wait_release();
                return MENU_RESULT_CUSTOM;
            }
        }

        if (UB_Button_OnClick(BTN_UP) || UB_Button_OnClick(BTN_DOWN)) {
            sel = 1 - sel;
            menu_draw_main(sel);
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            if (sel == 0) {
                return MENU_RESULT_CAMPAIGN;
            }
            return MENU_RESULT_CUSTOM;
        }

        UB_Systick_Pause_ms(30);
    }
}

static const char* menu_map_name(uint32_t map_id) {
    if (map_id == MAZE_MAP_OPEN) return "Arena";
    if (map_id == MAZE_MAP_THREE) return "Spiral";
    if (map_id == MAZE_MAP_FOUR) return "Blocks";
    if (map_id == MAZE_MAP_FIVE) return "Columns";
    if (map_id == MAZE_MAP_SIX) return "Divide";
    return "Classic";
}

static const char* menu_speed_name(uint32_t idx) {
    if (idx == CUSTOM_SPEED_SLOW) return "Slow";
    if (idx == CUSTOM_SPEED_FAST) return "Fast";
    return "Normal";
}

static void menu_draw_wizard(uint32_t sel_line, uint32_t unused) {
    char buf[32];
    uint32_t color;

    gui_clear_screen();
    UB_Font_DrawString(70, 5, "CUSTOM SETUP", &Arial_7x10, FONT_COL, BACKGROUND_COL);

    // 1. Players count
    color = (sel_line == 0) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 30, "Players:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%uP", (unsigned int)Game.custom.player_count);
    UB_Font_DrawString(75, 30, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 2. 2P Mode
    if (Game.custom.player_count == CUSTOM_PLAYER_2) {
        color = (sel_line == 1) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 52, "2P Mode:", &Arial_7x10, color, BACKGROUND_COL);
        sprintf(buf, "%s", (Game.custom.two_player_mode == CUSTOM_2P_COOP) ? "Co-op" : "VS");
        UB_Font_DrawString(75, 52, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    } else {
        UB_Font_DrawString(10, 52, "2P Mode: N/A", &Arial_7x10, FONT_COL3, BACKGROUND_COL);
    }

    // 3. Map
    color = (sel_line == 2) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 74, "Map:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_map_name(Game.custom.map_id));
    UB_Font_DrawString(75, 74, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 4. Ghost Speed
    color = (sel_line == 3) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 96, "Speed:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_speed_name(Game.custom.ghost_speed_idx));
    UB_Font_DrawString(75, 96, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 5. Ghost Count
    color = (sel_line == 4) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 118, "Ghosts:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%u", (unsigned int)Game.custom.ghost_count);
    UB_Font_DrawString(75, 118, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 6. Blinky strategy (G1)
    color = (sel_line == 5) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 140, "G1 Str:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[0]));
    UB_Font_DrawString(75, 140, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 7. Pinky strategy (G2)
    if (Game.custom.ghost_count >= 2) {
        color = (sel_line == 6) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 162, "G2 Str:", &Arial_7x10, color, BACKGROUND_COL);
        sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[1]));
        UB_Font_DrawString(75, 162, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    }

    // 8. Inky strategy (G3)
    if (Game.custom.ghost_count >= 3) {
        color = (sel_line == 7) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 184, "G3 Str:", &Arial_7x10, color, BACKGROUND_COL);
        sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[2]));
        UB_Font_DrawString(75, 184, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    }

    // 9. Clyde strategy (G4)
    if (Game.custom.ghost_count == 4) {
        color = (sel_line == 8) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 206, "G4 Str:", &Arial_7x10, color, BACKGROUND_COL);
        sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[3]));
        UB_Font_DrawString(75, 206, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    }

    // Draw Preview Map
    UB_Font_DrawString(120, 15, "Map Preview", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
    maze_draw_preview(Game.custom.map_id, 120, 30, 4);

    // Draw Buttons
    // Back Button (Blue border, white text)
    UB_Graphic2D_DrawRectDMA(10, 275, 70, 25, RGB_COL_BLUE);
    UB_Font_DrawString(25, 282, "Back", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);

    // Start Button (Red border, white text)
    UB_Graphic2D_DrawRectDMA(160, 275, 70, 25, RGB_COL_RED);
    UB_Font_DrawString(175, 282, "Start", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
}

static uint32_t menu_handle_value_tap(uint32_t unused1, uint32_t unused2, uint16_t tx, uint16_t ty) {
    if (tx > 115) {
        return 0;
    }

    // Players
    if (ty >= 26 && ty <= 44) {
        Game.custom.player_count = (Game.custom.player_count == CUSTOM_PLAYER_1) ? CUSTOM_PLAYER_2 : CUSTOM_PLAYER_1;
        return 1;
    }
    // 2P Mode
    if (ty >= 48 && ty <= 66) {
        if (Game.custom.player_count == CUSTOM_PLAYER_2) {
            Game.custom.two_player_mode = (Game.custom.two_player_mode == CUSTOM_2P_COOP) ? CUSTOM_2P_VS_GHOST : CUSTOM_2P_COOP;
            return 1;
        }
        return 0;
    }
    // Map
    if (ty >= 70 && ty <= 88) {
        Game.custom.map_id = menu_cycle_value(Game.custom.map_id, 0, MAZE_MAP_COUNT - 1, 1);
        return 1;
    }
    // Speed
    if (ty >= 92 && ty <= 110) {
        Game.custom.ghost_speed_idx = menu_cycle_value(Game.custom.ghost_speed_idx, CUSTOM_SPEED_SLOW, CUSTOM_SPEED_FAST, 1);
        return 1;
    }
    // Ghosts count
    if (ty >= 114 && ty <= 132) {
        Game.custom.ghost_count = menu_cycle_value(Game.custom.ghost_count, 1, CUSTOM_MAX_GHOSTS, 1);
        return 1;
    }
    // G1
    if (ty >= 136 && ty <= 154) {
        Game.custom.ghost_strategies[0] = menu_cycle_value(Game.custom.ghost_strategies[0], 0, GHOST_STRATEGY_COUNT - 1, 1);
        return 1;
    }
    // G2
    if (ty >= 158 && ty <= 176 && Game.custom.ghost_count >= 2) {
        Game.custom.ghost_strategies[1] = menu_cycle_value(Game.custom.ghost_strategies[1], 0, GHOST_STRATEGY_COUNT - 1, 1);
        return 1;
    }
    // G3
    if (ty >= 180 && ty <= 198 && Game.custom.ghost_count >= 3) {
        Game.custom.ghost_strategies[2] = menu_cycle_value(Game.custom.ghost_strategies[2], 0, GHOST_STRATEGY_COUNT - 1, 1);
        return 1;
    }
    // G4
    if (ty >= 202 && ty <= 220 && Game.custom.ghost_count == 4) {
        Game.custom.ghost_strategies[3] = menu_cycle_value(Game.custom.ghost_strategies[3], 0, GHOST_STRATEGY_COUNT - 1, 1);
        return 1;
    }

    return 0;
}

static uint32_t menu_run_custom_wizard(void) {
    uint32_t sel_line = 0;
    uint16_t tx;
    uint16_t ty;
    uint32_t max_lines;

    menu_draw_wizard(sel_line, 0);

    while (1) {
        max_lines = 5 + Game.custom.ghost_count;

        if (menu_touch_pressed(&tx, &ty) != 0) {
            // Check buttons first
            // Back Button
            if (menu_touch_in_rect(tx, ty, 10, 275, 70, 25)) {
                menu_touch_wait_release();
                return 0; // Go back to main menu
            }
            // Start Button
            if (menu_touch_in_rect(tx, ty, 160, 275, 70, 25)) {
                menu_touch_wait_release();
                return 1; // Start game
            }

            // Check value tap
            if (menu_handle_value_tap(0, 0, tx, ty) != 0) {
                // Update sel_line to match touch Y coordinate
                if (ty >= 26 && ty <= 44) sel_line = 0;
                else if (ty >= 48 && ty <= 66 && Game.custom.player_count == CUSTOM_PLAYER_2) sel_line = 1;
                else if (ty >= 70 && ty <= 88) sel_line = 2;
                else if (ty >= 92 && ty <= 110) sel_line = 3;
                else if (ty >= 114 && ty <= 132) sel_line = 4;
                else if (ty >= 136 && ty <= 154) sel_line = 5;
                else if (ty >= 158 && ty <= 176 && Game.custom.ghost_count >= 2) sel_line = 6;
                else if (ty >= 180 && ty <= 198 && Game.custom.ghost_count >= 3) sel_line = 7;
                else if (ty >= 202 && ty <= 220 && Game.custom.ghost_count == 4) sel_line = 8;

                menu_touch_wait_release();
                menu_draw_wizard(sel_line, 0);
            }
        }

        // Hard buttons controls
        if (UB_Button_OnClick(BTN_UP)) {
            if (sel_line > 0) {
                sel_line--;
                if (sel_line == 1 && Game.custom.player_count == CUSTOM_PLAYER_1) {
                    sel_line = 0;
                }
            } else {
                sel_line = max_lines - 1;
            }
            menu_draw_wizard(sel_line, 0);
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN)) {
            sel_line++;
            if (sel_line == 1 && Game.custom.player_count == CUSTOM_PLAYER_1) {
                sel_line = 2;
            }
            if (sel_line >= max_lines) {
                sel_line = 0;
            }
            menu_draw_wizard(sel_line, 0);
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_LEFT) || UB_Button_OnClick(BTN_RIGHT)) {
            // Simulate touch on selected line to cycle value
            uint16_t simulated_y = 35;
            if (sel_line == 0) simulated_y = 35;
            else if (sel_line == 1) simulated_y = 55;
            else if (sel_line == 2) simulated_y = 75;
            else if (sel_line == 3) simulated_y = 95;
            else if (sel_line == 4) simulated_y = 115;
            else if (sel_line == 5) simulated_y = 145;
            else if (sel_line == 6) simulated_y = 165;
            else if (sel_line == 7) simulated_y = 185;
            else if (sel_line == 8) simulated_y = 205;

            if (menu_handle_value_tap(0, 0, 10, simulated_y)) {
                menu_draw_wizard(sel_line, 0);
            }
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            return 1; // Start game
        }

        UB_Systick_Pause_ms(30);
    }
}
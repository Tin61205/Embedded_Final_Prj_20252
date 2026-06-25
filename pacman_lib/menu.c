//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "menu.h"

#include <stdlib.h>
#include <stdio.h>

#include "bot.h"
#include "maze.h"
#include "maze_txtmap.h"
#include "skin.h"
#include "stm32_ub_graphic2d.h"


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

#define MENU_CAMPAIGN_PREVIEW_CELL  5
#define MENU_CAMPAIGN_PREVIEW_X     ((240U - (ROOM_CNT_X * MENU_CAMPAIGN_PREVIEW_CELL)) / 2U)
#define MENU_CAMPAIGN_PREVIEW_Y     118

#define MENU_CUSTOM_PREVIEW_CELL    2
#define MENU_CUSTOM_PREVIEW_X       ((240U - (ROOM_CNT_X * MENU_CUSTOM_PREVIEW_CELL)) / 2U)
#define MENU_CUSTOM_PREVIEW_Y       212

//--------------------------------------------------------------
// intern helpers
//--------------------------------------------------------------
// static void menu_draw_nav_buttons(uint32_t show_back, const char *next_label);
static void menu_draw_main(uint32_t sel);
static uint32_t menu_run_main(void);
static void menu_custom_defaults(void);
static void menu_draw_wizard(uint32_t step, uint32_t ghost_idx);
static uint32_t menu_run_custom_wizard(void);
static void menu_draw_custom_map_select(void);
static uint32_t menu_run_custom_map_select(void);
static uint32_t menu_cycle_value(uint32_t val, uint32_t min_v, uint32_t max_v, int32_t dir);
static void menu_draw_campaign_wizard(uint32_t sel_line);
static uint32_t menu_handle_campaign_value_tap(uint16_t tx, uint16_t ty);
static uint32_t menu_run_campaign_wizard(void);

//--------------------------------------------------------------
// entry: main menu then optional custom wizard
//--------------------------------------------------------------
uint32_t menu_start(void) {
    uint32_t result;
    uint32_t srand_init = 0;

    GUI.refresh_value = GUI_REFRESH_VALUE;
    GUI.refresh_buttons = GUI_REFRESH_VALUE;

    skin_init();
    menu_custom_defaults();
    Game.campaign_map_id = MAZE_MAP_CLASSIC;
    Game.campaign_difficulty = 1;

    while (1) {
        srand_init++;
        result = menu_run_main();

        if (result == MENU_RESULT_CAMPAIGN) {
            if (menu_run_campaign_wizard() != 0) {
                Game.play_type = GAME_PLAY_CAMPAIGN;
                break;
            }
        } else if (result == MENU_RESULT_CUSTOM) {
            uint32_t custom_step = 1;
            uint32_t custom_done = 0;
            while (!custom_done) {
                if (custom_step == 1) {
                    if (menu_run_custom_map_select() != 0) {
                        custom_step = 2;
                    } else {
                        custom_done = 1; // Back to main menu
                    }
                } else if (custom_step == 2) {
                    uint32_t wizard_res = menu_run_custom_wizard();
                    if (wizard_res == 1) {
                        Game.play_type = GAME_PLAY_CUSTOM;
                        custom_done = 2; // Start game
                    } else {
                        custom_step = 1; // Go back to map select
                    }
                }
            }
            if (custom_done == 2) {
                break;
            }
        }
    }

    srand(srand_init);
    return result;
}

static uint32_t menu_is_vs_ghost_2p(void) {
    return (Game.custom.player_count == CUSTOM_PLAYER_2 &&
            Game.custom.two_player_mode == CUSTOM_2P_VS_GHOST) ? 1 : 0;
}

static uint32_t menu_ai_ghost_count(void) {
    if (menu_is_vs_ghost_2p()) {
        return (Game.custom.ghost_count > 0) ? (Game.custom.ghost_count - 1) : 0;
    }
    return Game.custom.ghost_count;
}

static uint32_t menu_wizard_max_lines(void) {
    return 4 + Game.custom.ghost_count;
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



/*
static void menu_draw_nav_buttons(uint32_t show_back, const char *next_label) {
    if (show_back != 0) {
        UB_Font_DrawString(MENU_BTN_BACK_X, MENU_BTN_Y, "Back", &Arial_7x10, MENUE_COL_OFF, BACKGROUND_COL);
    } else {
        UB_Font_DrawString(MENU_BTN_BACK_X, MENU_BTN_Y, "     ", &Arial_7x10, MENUE_COL_OFF, BACKGROUND_COL);
    }
    UB_Font_DrawString(MENU_BTN_NEXT_X, MENU_BTN_Y, next_label, &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
}
*/

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
    // 1. Draw the background image
    UB_Graphic2D_DrawImageFull(&background_menu, 0, 0);
    
    // 2. Draw game title header with transparent/black background container
    UB_Graphic2D_DrawFullRectDMA(10, 10, 220, 32, RGB_COL_BLACK);
    UB_Graphic2D_DrawRectDMA(10, 10, 220, 32, RGB_COL_YELLOW);
    UB_Font_DrawString(40, 21, "   PACMAN GAME K68", &Arial_7x10, RGB_COL_YELLOW, RGB_COL_BLACK);
    
    // 3. Draw Campaign Button (Centered, X: 40, Y: 120, W: 160, H: 30)
    if (sel == 0) {
        UB_Graphic2D_DrawFullRectDMA(40, 120, 160, 30, RGB_COL_RED);
        UB_Graphic2D_DrawRectDMA(40, 120, 160, 30, RGB_COL_WHITE);
        UB_Font_DrawString(92, 130, "Campaign", &Arial_7x10, RGB_COL_WHITE, RGB_COL_RED);
    } else {
        UB_Graphic2D_DrawFullRectDMA(40, 120, 160, 30, 0x39E7); // Dark Grey
        UB_Graphic2D_DrawRectDMA(40, 120, 160, 30, RGB_COL_BLACK);
        UB_Font_DrawString(92, 130, "Campaign", &Arial_7x10, RGB_COL_WHITE, 0x39E7);
    }
    
    // 4. Draw Custom Button (Centered, X: 40, Y: 170, W: 160, H: 30)
    if (sel == 1) {
        UB_Graphic2D_DrawFullRectDMA(40, 170, 160, 30, RGB_COL_RED);
        UB_Graphic2D_DrawRectDMA(40, 170, 160, 30, RGB_COL_WHITE);
        UB_Font_DrawString(99, 180, "Custom", &Arial_7x10, RGB_COL_WHITE, RGB_COL_RED);
    } else {
        UB_Graphic2D_DrawFullRectDMA(40, 170, 160, 30, 0x39E7); // Dark Grey
        UB_Graphic2D_DrawRectDMA(40, 170, 160, 30, RGB_COL_BLACK);
        UB_Font_DrawString(99, 180, "Custom", &Arial_7x10, RGB_COL_WHITE, 0x39E7);
    }
    
    // 5. Draw prompt message container at the bottom
}

static uint32_t menu_run_main(void) {
    uint32_t sel = 0;

    menu_draw_main(sel);

    while (1) {
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
    if (map_id >= MAZE_MAP_TXT_BASE) {
        uint32_t idx = map_id - MAZE_MAP_TXT_BASE;
        if (idx < MAZE_TXTMAP_COUNT) {
            return maze_txtmap_names[idx];
        }
        return "Unknown";
    }
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
        sprintf(buf, "%s", (Game.custom.two_player_mode == CUSTOM_2P_COOP) ? "Co-op" : "Vs Ghost");
        UB_Font_DrawString(75, 52, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    } else {
        UB_Font_DrawString(10, 52, "2P Mode: N/A", &Arial_7x10, FONT_COL3, BACKGROUND_COL);
    }

    // 3. Ghost Speed
    color = (sel_line == 2) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 74, "Speed:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_speed_name(Game.custom.ghost_speed_idx));
    UB_Font_DrawString(75, 74, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 4. Ghost Count
    color = (sel_line == 3) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 96, "Ghosts:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%u", (unsigned int)Game.custom.ghost_count);
    UB_Font_DrawString(75, 96, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    if (menu_is_vs_ghost_2p() && Game.custom.ghost_count >= 1) {
        color = (sel_line == 4) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 118, "G1 Ctrl:", &Arial_7x10, color, BACKGROUND_COL);
        UB_Font_DrawString(75, 118, "Player 2", &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    } else if (Game.custom.ghost_count >= 1) {
        color = (sel_line == 4) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 118, "G1 Str:", &Arial_7x10, color, BACKGROUND_COL);
        sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[0]));
        UB_Font_DrawString(75, 118, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    }

    if (menu_is_vs_ghost_2p()) {
        if (menu_ai_ghost_count() >= 1) {
            color = (sel_line == 5) ? MENUE_COL_ON : MENUE_COL_OFF;
            UB_Font_DrawString(10, 140, "AI1 Str:", &Arial_7x10, color, BACKGROUND_COL);
            sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[0]));
            UB_Font_DrawString(75, 140, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
        if (menu_ai_ghost_count() >= 2) {
            color = (sel_line == 6) ? MENUE_COL_ON : MENUE_COL_OFF;
            UB_Font_DrawString(10, 162, "AI2 Str:", &Arial_7x10, color, BACKGROUND_COL);
            sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[1]));
            UB_Font_DrawString(75, 162, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
        if (menu_ai_ghost_count() >= 3) {
            color = (sel_line == 7) ? MENUE_COL_ON : MENUE_COL_OFF;
            UB_Font_DrawString(10, 184, "AI3 Str:", &Arial_7x10, color, BACKGROUND_COL);
            sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[2]));
            UB_Font_DrawString(75, 184, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
    } else {
        if (Game.custom.ghost_count >= 2) {
            color = (sel_line == 5) ? MENUE_COL_ON : MENUE_COL_OFF;
            UB_Font_DrawString(10, 140, "G2 Str:", &Arial_7x10, color, BACKGROUND_COL);
            sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[1]));
            UB_Font_DrawString(75, 140, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
        if (Game.custom.ghost_count >= 3) {
            color = (sel_line == 6) ? MENUE_COL_ON : MENUE_COL_OFF;
            UB_Font_DrawString(10, 162, "G3 Str:", &Arial_7x10, color, BACKGROUND_COL);
            sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[2]));
            UB_Font_DrawString(75, 162, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
        if (Game.custom.ghost_count == 4) {
            color = (sel_line == 7) ? MENUE_COL_ON : MENUE_COL_OFF;
            UB_Font_DrawString(10, 184, "G4 Str:", &Arial_7x10, color, BACKGROUND_COL);
            sprintf(buf, "%s", bot_strategy_name(Game.custom.ghost_strategies[3]));
            UB_Font_DrawString(75, 184, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
    }

    sprintf(buf, "Map: %s", menu_map_name(Game.custom.map_id));
    UB_Font_DrawString(10, 215, buf, &Arial_7x10, FONT_COL2, BACKGROUND_COL);

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
    // Speed
    if (ty >= 70 && ty <= 88) {
        Game.custom.ghost_speed_idx = menu_cycle_value(Game.custom.ghost_speed_idx, CUSTOM_SPEED_SLOW, CUSTOM_SPEED_FAST, 1);
        return 1;
    }
    // Ghosts count
    if (ty >= 92 && ty <= 110) {
        Game.custom.ghost_count = menu_cycle_value(Game.custom.ghost_count, 1, CUSTOM_MAX_GHOSTS, 1);
        return 1;
    }
    if (menu_is_vs_ghost_2p()) {
        if (ty >= 114 && ty <= 132 && menu_ai_ghost_count() >= 1) {
            Game.custom.ghost_strategies[0] = menu_cycle_value(Game.custom.ghost_strategies[0], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
        if (ty >= 136 && ty <= 154 && menu_ai_ghost_count() >= 2) {
            Game.custom.ghost_strategies[1] = menu_cycle_value(Game.custom.ghost_strategies[1], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
        if (ty >= 158 && ty <= 176 && menu_ai_ghost_count() >= 3) {
            Game.custom.ghost_strategies[2] = menu_cycle_value(Game.custom.ghost_strategies[2], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
    } else {
        if (ty >= 114 && ty <= 132 && Game.custom.ghost_count >= 1) {
            Game.custom.ghost_strategies[0] = menu_cycle_value(Game.custom.ghost_strategies[0], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
        if (ty >= 136 && ty <= 154 && Game.custom.ghost_count >= 2) {
            Game.custom.ghost_strategies[1] = menu_cycle_value(Game.custom.ghost_strategies[1], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
        if (ty >= 158 && ty <= 176 && Game.custom.ghost_count >= 3) {
            Game.custom.ghost_strategies[2] = menu_cycle_value(Game.custom.ghost_strategies[2], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
        if (ty >= 180 && ty <= 198 && Game.custom.ghost_count == 4) {
            Game.custom.ghost_strategies[3] = menu_cycle_value(Game.custom.ghost_strategies[3], 0, GHOST_STRATEGY_COUNT - 1, 1);
            return 1;
        }
    }

    return 0;
}

static uint32_t menu_run_custom_wizard(void) {
    uint32_t sel_line = 0;
    uint32_t max_lines;

    menu_draw_wizard(sel_line, 0);

    while (1) {
        max_lines = menu_wizard_max_lines();

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

            if (menu_handle_value_tap(0, 0, 10, simulated_y)) {
                menu_draw_wizard(sel_line, 0);
            }
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            return 0; // Go back to map select
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            return 1; // Start game
        }

        UB_Systick_Pause_ms(30);
    }
}

static void menu_draw_custom_map_select(void) {
    char buf[64];
    gui_clear_screen();

    // Title
    UB_Font_DrawString(75, 5, "SELECT MAP", &Arial_7x10, FONT_COL, BACKGROUND_COL);

    // Map Name centered under header
    sprintf(buf, "Map: %s", menu_map_name(Game.custom.map_id));
    UB_Font_DrawString(60, 25, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // Direction indicators
    UB_Font_DrawString(40, 25, "<", &Arial_7x10, MENUE_COL_OFF, BACKGROUND_COL);
    UB_Font_DrawString(190, 25, ">", &Arial_7x10, MENUE_COL_OFF, BACKGROUND_COL);

    // Draw large preview (cell size = 6)
    // 28 * 6 = 168 px, 31 * 6 = 186 px. Centered: (240 - 168) / 2 = 36.
    maze_draw_preview(Game.custom.map_id, 36, 45, 6);

    // Draw buttons
    // Back Button (Blue border, white text)
    UB_Graphic2D_DrawRectDMA(10, 275, 70, 25, RGB_COL_BLUE);
    UB_Font_DrawString(25, 282, "Back", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);

    // Next Button (Red border, white text)
    UB_Graphic2D_DrawRectDMA(160, 275, 70, 25, RGB_COL_RED);
    UB_Font_DrawString(180, 282, "Next", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
}

static uint32_t menu_run_custom_map_select(void) {
    menu_draw_custom_map_select();

    while (1) {
        // Hard buttons controls
        if (UB_Button_OnClick(BTN_UP) || UB_Button_OnClick(BTN_LEFT)) {
            Game.custom.map_id = menu_cycle_value(Game.custom.map_id, 0, MAZE_MAP_COUNT - 1, -1);
            menu_draw_custom_map_select();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN) || UB_Button_OnClick(BTN_RIGHT)) {
            Game.custom.map_id = menu_cycle_value(Game.custom.map_id, 0, MAZE_MAP_COUNT - 1, 1);
            menu_draw_custom_map_select();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            return 0; // Go back to main menu
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            return 1; // Go to step 2
        }

        UB_Systick_Pause_ms(30);
    }
}

//--------------------------------------------------------------
// Campaign setup wizard functions
//--------------------------------------------------------------
static void menu_draw_campaign_wizard(uint32_t sel_line) {
    char buf[32];
    uint32_t color;

    gui_clear_screen();
    UB_Font_DrawString(60, 5, "CAMPAIGN SETUP", &Arial_7x10, FONT_COL, BACKGROUND_COL);

    // 1. Map
    color = (sel_line == 0) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 50, "Map:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_map_name(Game.campaign_map_id));
    UB_Font_DrawString(75, 50, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 2. Difficulty
    color = (sel_line == 1) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 80, "Difficulty:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%u", (unsigned int)Game.campaign_difficulty);
    UB_Font_DrawString(90, 80, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    UB_Font_DrawString(10, 105, "Map Preview:", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
    maze_draw_preview(Game.campaign_map_id, MENU_CAMPAIGN_PREVIEW_X, MENU_CAMPAIGN_PREVIEW_Y, MENU_CAMPAIGN_PREVIEW_CELL);

    // Draw Buttons
    // Back Button (Blue border, white text)
    UB_Graphic2D_DrawRectDMA(10, 275, 70, 25, RGB_COL_BLUE);
    UB_Font_DrawString(25, 282, "Back", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);

    // Start Button (Red border, white text)
    UB_Graphic2D_DrawRectDMA(160, 275, 70, 25, RGB_COL_RED);
    UB_Font_DrawString(175, 282, "Start", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
}

static uint32_t menu_handle_campaign_value_tap(uint16_t tx, uint16_t ty) {
    if (tx > 115) {
        return 0;
    }
    // Map
    if (ty >= 46 && ty <= 64) {
        Game.campaign_map_id = menu_cycle_value(Game.campaign_map_id, 0, MAZE_MAP_COUNT - 1, 1);
        return 1;
    }
    // Difficulty
    if (ty >= 76 && ty <= 94) {
        Game.campaign_difficulty = menu_cycle_value(Game.campaign_difficulty, 1, 10, 1);
        return 1;
    }
    return 0;
}

static uint32_t menu_run_campaign_wizard(void) {
    uint32_t sel_line = 0;

    menu_draw_campaign_wizard(sel_line);

    while (1) {
        // Hard buttons
        if (UB_Button_OnClick(BTN_UP) || UB_Button_OnClick(BTN_DOWN)) {
            sel_line = 1 - sel_line;
            menu_draw_campaign_wizard(sel_line);
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_LEFT)) {
            if (sel_line == 0) {
                Game.campaign_map_id = menu_cycle_value(Game.campaign_map_id, 0, MAZE_MAP_COUNT - 1, -1);
            } else {
                Game.campaign_difficulty = menu_cycle_value(Game.campaign_difficulty, 1, 10, -1);
            }
            menu_draw_campaign_wizard(sel_line);
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_RIGHT)) {
            if (sel_line == 0) {
                Game.campaign_map_id = menu_cycle_value(Game.campaign_map_id, 0, MAZE_MAP_COUNT - 1, 1);
            } else {
                Game.campaign_difficulty = menu_cycle_value(Game.campaign_difficulty, 1, 10, 1);
            }
            menu_draw_campaign_wizard(sel_line);
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            return 0; // Back to main menu
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            return 1; // Start
        }
        UB_Systick_Pause_ms(30);
    }
}
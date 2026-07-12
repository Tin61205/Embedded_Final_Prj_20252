//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "menu.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bot.h"
#include "maze.h"
#include "skin.h"
#include "stm32_ub_graphic2d.h"
#include "stm32_ub_buzzer.h"


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
static void menu_draw_custom_players_speed(uint32_t sel_line);
static uint32_t menu_run_custom_players_speed(void);
static void menu_draw_custom_ghosts(uint32_t sel_line);
static uint32_t menu_run_custom_ghosts(void);
static void menu_draw_custom_map_select(void);
static uint32_t menu_run_custom_map_select(void);
static uint32_t menu_cycle_value(uint32_t val, uint32_t min_v, uint32_t max_v, int32_t dir);
static uint32_t menu_cycle_ghost_strategy(uint32_t strategy, int32_t dir);
static void menu_draw_campaign_wizard(uint32_t sel_line);
static uint32_t menu_run_campaign_wizard(void);
static void menu_draw_highscore_view(void);
static void menu_run_highscore_view(void);

//--------------------------------------------------------------
// entry: main menu then optional custom wizard
//--------------------------------------------------------------
uint32_t menu_start(void) {
    uint32_t result;
    uint32_t srand_init = 0;

    UB_Buzzer_Stop();
    GUI.refresh_value = GUI_REFRESH_VALUE;
    GUI.refresh_buttons = GUI_REFRESH_VALUE;

    skin_init();
    menu_custom_defaults();
    Game.campaign_map_id = MAZE_MAP_CLASSIC;
    Game.campaign_difficulty = 1;
    Game.campaign_coop = 0;

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
                    if (menu_run_custom_players_speed() != 0) {
                        custom_step = 3;
                    } else {
                        custom_step = 1; // Go back to map select
                    }
                } else if (custom_step == 3) {
                    if (menu_run_custom_ghosts() != 0) {
                        Game.play_type = GAME_PLAY_CUSTOM;
                        custom_done = 2; // Start game
                    } else {
                        custom_step = 2; // Go back to players & speed
                    }
                }
            }
            if (custom_done == 2) {
                break;
            }
        } else if (result == MENU_RESULT_HIGHSCORE) {
            menu_run_highscore_view();
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

static uint32_t menu_players_speed_max_lines(void) {
    return 4;
}

static uint32_t menu_ghosts_max_lines(void) {
    if (menu_is_vs_ghost_2p() && Game.custom.ghost_count >= 1) {
        return 2 + menu_ai_ghost_count();
    }
    return 1 + Game.custom.ghost_count;
}

static uint32_t menu_players_speed_skip_line(uint32_t line) {
    return (line == 1 && Game.custom.player_count == CUSTOM_PLAYER_1) ? 1 : 0;
}

static uint32_t menu_ghosts_skip_line(uint32_t line) {
    return (menu_is_vs_ghost_2p() && line == 1 && Game.custom.ghost_count >= 1) ? 1 : 0;
}

static void menu_draw_footer_buttons(const char *next_label, uint16_t next_x) {
    UB_Graphic2D_DrawRectDMA(10, 275, 70, 25, RGB_COL_BLUE);
    UB_Font_DrawString(25, 282, "Back", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
    UB_Graphic2D_DrawRectDMA(160, 275, 70, 25, RGB_COL_RED);
    UB_Font_DrawString(next_x, 282, (char *)next_label, &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
}

static void menu_custom_defaults(void) {
    uint32_t i;

    Game.custom.player_count = CUSTOM_PLAYER_1;
    Game.custom.map_id = MAZE_MAP_CLASSIC;
    Game.custom.player_speed_idx = CUSTOM_SPEED_NORMAL;
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

static void menu_present_frame(void) {
    UB_LCD_SetLayer_2();
    UB_LCD_SetTransparency(255);
    UB_LCD_Copy_Layer2_to_Layer1();
    UB_LCD_Refresh();
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

static uint32_t menu_cycle_ghost_strategy(uint32_t strategy, int32_t dir) {
    if (strategy < GHOST_STRATEGY_CUSTOM_MIN || strategy > GHOST_STRATEGY_CUSTOM_MAX) {
        strategy = GHOST_STRATEGY_DRUNK;
    }
    return menu_cycle_value(strategy, GHOST_STRATEGY_CUSTOM_MIN, GHOST_STRATEGY_CUSTOM_MAX, dir);
}

static void menu_draw_main(uint32_t sel) {
    UB_LCD_SetLayer_2();
    UB_LCD_SetTransparency(255);

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
    
    // 5. Draw High Score Button (Centered, X: 40, Y: 220, W: 160, H: 30)
    if (sel == 2) {
        UB_Graphic2D_DrawFullRectDMA(40, 220, 160, 30, RGB_COL_RED);
        UB_Graphic2D_DrawRectDMA(40, 220, 160, 30, RGB_COL_WHITE);
        UB_Font_DrawString(85, 230, "High Score", &Arial_7x10, RGB_COL_WHITE, RGB_COL_RED);
    } else {
        UB_Graphic2D_DrawFullRectDMA(40, 220, 160, 30, 0x39E7); // Dark Grey
        UB_Graphic2D_DrawRectDMA(40, 220, 160, 30, RGB_COL_BLACK);
        UB_Font_DrawString(85, 230, "High Score", &Arial_7x10, RGB_COL_WHITE, 0x39E7);
    }

    menu_present_frame();
}

static uint32_t menu_run_main(void) {
    uint32_t sel = (Game.play_type == GAME_PLAY_CUSTOM) ? 1 : 0;

    menu_draw_main(sel);
    gui_drain_center_button();

    while (1) {
        if (UB_Button_OnClick(BTN_UP)) {
            if (sel == 0) {
                sel = 2;
            } else {
                sel--;
            }
            menu_draw_main(sel);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN)) {
            if (sel == 2) {
                sel = 0;
            } else {
                sel++;
            }
            menu_draw_main(sel);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Buzzer_Play_MenuClick();
            if (sel == 0) {
                return MENU_RESULT_CAMPAIGN;
            } else if (sel == 1) {
                return MENU_RESULT_CUSTOM;
            } else if (sel == 2) {
                return MENU_RESULT_HIGHSCORE;
            }
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

static uint32_t menu_adjust_players_speed(uint32_t sel_line, int32_t dir) {
    if (sel_line == 0) {
        Game.custom.player_count = (Game.custom.player_count == CUSTOM_PLAYER_1) ? CUSTOM_PLAYER_2 : CUSTOM_PLAYER_1;
        return 1;
    }
    if (sel_line == 1) {
        if (Game.custom.player_count == CUSTOM_PLAYER_2) {
            Game.custom.two_player_mode = (Game.custom.two_player_mode == CUSTOM_2P_COOP) ? CUSTOM_2P_VS_GHOST : CUSTOM_2P_COOP;
            return 1;
        }
        return 0;
    }
    if (sel_line == 2) {
        Game.custom.player_speed_idx = menu_cycle_value(Game.custom.player_speed_idx, CUSTOM_SPEED_SLOW, CUSTOM_SPEED_FAST, dir);
        return 1;
    }
    if (sel_line == 3) {
        Game.custom.ghost_speed_idx = menu_cycle_value(Game.custom.ghost_speed_idx, CUSTOM_SPEED_SLOW, CUSTOM_SPEED_FAST, dir);
        return 1;
    }
    return 0;
}

static uint32_t menu_adjust_ghosts(uint32_t sel_line, int32_t dir) {
    uint32_t ai_idx;

    if (sel_line == 0) {
        Game.custom.ghost_count = menu_cycle_value(Game.custom.ghost_count, 1, CUSTOM_MAX_GHOSTS, dir);
        return 1;
    }
    if (menu_is_vs_ghost_2p()) {
        if (sel_line == 1 && Game.custom.ghost_count >= 1) {
            return 0;
        }
        ai_idx = sel_line - 2;
        if (ai_idx < menu_ai_ghost_count()) {
            Game.custom.ghost_strategies[ai_idx] = menu_cycle_ghost_strategy(
                Game.custom.ghost_strategies[ai_idx], dir);
            return 1;
        }
        return 0;
    }
    if (sel_line > 0 && sel_line <= Game.custom.ghost_count) {
        Game.custom.ghost_strategies[sel_line - 1] = menu_cycle_ghost_strategy(
            Game.custom.ghost_strategies[sel_line - 1], dir);
        return 1;
    }
    return 0;
}

static uint32_t menu_step_line_up(uint32_t sel_line, uint32_t max_lines, uint32_t (*skip_line)(uint32_t)) {
    if (sel_line > 0) {
        sel_line--;
        while (sel_line > 0 && skip_line(sel_line) != 0) {
            sel_line--;
        }
        if (skip_line(sel_line) != 0) {
            sel_line = 0;
        }
    } else {
        sel_line = max_lines - 1;
        while (sel_line > 0 && skip_line(sel_line) != 0) {
            sel_line--;
        }
    }
    return sel_line;
}

static uint32_t menu_step_line_down(uint32_t sel_line, uint32_t max_lines, uint32_t (*skip_line)(uint32_t)) {
    sel_line++;
    if (sel_line >= max_lines) {
        sel_line = 0;
    }
    while (skip_line(sel_line) != 0) {
        sel_line++;
        if (sel_line >= max_lines) {
            sel_line = 0;
            break;
        }
    }
    return sel_line;
}

static void menu_draw_custom_players_speed(uint32_t sel_line) {
    char buf[32];
    uint32_t color;

    gui_clear_screen();
    UB_Font_DrawString(55, 5, "PLAYERS & SPEED", &Arial_7x10, FONT_COL, BACKGROUND_COL);
    UB_Font_DrawString(200, 5, "1/2", &Arial_7x10, FONT_COL2, BACKGROUND_COL);

    color = (sel_line == 0) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 40, "Players:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%uP", (unsigned int)Game.custom.player_count);
    UB_Font_DrawString(85, 40, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    if (Game.custom.player_count == CUSTOM_PLAYER_2) {
        color = (sel_line == 1) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, 68, "2P Mode:", &Arial_7x10, color, BACKGROUND_COL);
        sprintf(buf, "%s", (Game.custom.two_player_mode == CUSTOM_2P_COOP) ? "Co-op" : "Vs Ghost");
        UB_Font_DrawString(85, 68, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    } else {
        UB_Font_DrawString(10, 68, "2P Mode: N/A", &Arial_7x10, FONT_COL3, BACKGROUND_COL);
    }

    color = (sel_line == 2) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 96, "Pacman Spd:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_speed_name(Game.custom.player_speed_idx));
    UB_Font_DrawString(95, 96, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    color = (sel_line == 3) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 124, "Ghost Spd:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_speed_name(Game.custom.ghost_speed_idx));
    UB_Font_DrawString(95, 124, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    sprintf(buf, "Map: %s", menu_map_name(Game.custom.map_id));
    UB_Font_DrawString(10, 220, buf, &Arial_7x10, FONT_COL2, BACKGROUND_COL);

    menu_draw_footer_buttons("Next", 180);
    menu_present_frame();
}

static uint32_t menu_run_custom_players_speed(void) {
    uint32_t sel_line = 0;

    menu_draw_custom_players_speed(sel_line);

    while (1) {
        if (UB_Button_OnClick(BTN_UP)) {
            sel_line = menu_step_line_up(sel_line, menu_players_speed_max_lines(), menu_players_speed_skip_line);
            menu_draw_custom_players_speed(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN)) {
            sel_line = menu_step_line_down(sel_line, menu_players_speed_max_lines(), menu_players_speed_skip_line);
            menu_draw_custom_players_speed(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        {
            bool left_click = UB_Button_OnClick(BTN_LEFT);
            bool right_click = UB_Button_OnClick(BTN_RIGHT);

            if (left_click || right_click) {
                int32_t dir = right_click ? 1 : -1;

                if (menu_adjust_players_speed(sel_line, dir) != 0) {
                    menu_draw_custom_players_speed(sel_line);
                    UB_Buzzer_Play_MenuClick();
                }
                UB_Systick_Pause_ms(150);
            }
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            UB_Buzzer_Play_MenuClick();
            return 0;
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Buzzer_Play_MenuClick();
            return 1;
        }

        UB_Systick_Pause_ms(30);
    }
}

static void menu_draw_custom_ghosts(uint32_t sel_line) {
    char buf[32];
    uint32_t color;
    uint32_t y = 40;
    uint32_t line = 0;
    uint32_t i;

    gui_clear_screen();
    UB_Font_DrawString(65, 5, "GHOSTS SETUP", &Arial_7x10, FONT_COL, BACKGROUND_COL);
    UB_Font_DrawString(200, 5, "2/2", &Arial_7x10, FONT_COL2, BACKGROUND_COL);

    color = (sel_line == line) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, y, "Ghosts:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%u", (unsigned int)Game.custom.ghost_count);
    UB_Font_DrawString(85, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
    line++;
    y += 28;

    if (menu_is_vs_ghost_2p() && Game.custom.ghost_count >= 1) {
        color = (sel_line == line) ? MENUE_COL_ON : MENUE_COL_OFF;
        UB_Font_DrawString(10, y, "Human G:", &Arial_7x10, color, BACKGROUND_COL);
        UB_Font_DrawString(85, y, "P2 Green", &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        line++;
        y += 28;
        for (i = 0; i < menu_ai_ghost_count(); i++) {
            color = (sel_line == line) ? MENUE_COL_ON : MENUE_COL_OFF;
            sprintf(buf, "AI%u Str:", (unsigned int)(i + 1));
            UB_Font_DrawString(10, y, buf, &Arial_7x10, color, BACKGROUND_COL);
            UB_Font_DrawString(85, y, (char *)bot_strategy_name(Game.custom.ghost_strategies[i]), &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
            line++;
            y += 28;
        }
    } else {
        for (i = 0; i < Game.custom.ghost_count; i++) {
            color = (sel_line == line) ? MENUE_COL_ON : MENUE_COL_OFF;
            sprintf(buf, "G%u Str:", (unsigned int)(i + 1));
            UB_Font_DrawString(10, y, buf, &Arial_7x10, color, BACKGROUND_COL);
            UB_Font_DrawString(85, y, (char *)bot_strategy_name(Game.custom.ghost_strategies[i]), &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
            line++;
            y += 28;
        }
    }

    sprintf(buf, "Map: %s", menu_map_name(Game.custom.map_id));
    UB_Font_DrawString(10, 220, buf, &Arial_7x10, FONT_COL2, BACKGROUND_COL);

    menu_draw_footer_buttons("Start", 175);
    menu_present_frame();
}

static uint32_t menu_run_custom_ghosts(void) {
    uint32_t sel_line = 0;

    menu_draw_custom_ghosts(sel_line);

    while (1) {
        if (UB_Button_OnClick(BTN_UP)) {
            sel_line = menu_step_line_up(sel_line, menu_ghosts_max_lines(), menu_ghosts_skip_line);
            menu_draw_custom_ghosts(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN)) {
            sel_line = menu_step_line_down(sel_line, menu_ghosts_max_lines(), menu_ghosts_skip_line);
            menu_draw_custom_ghosts(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        {
            bool left_click = UB_Button_OnClick(BTN_LEFT);
            bool right_click = UB_Button_OnClick(BTN_RIGHT);

            if (left_click || right_click) {
                int32_t dir = right_click ? 1 : -1;

                if (menu_adjust_ghosts(sel_line, dir) != 0) {
                    if (sel_line >= menu_ghosts_max_lines()) {
                        sel_line = menu_ghosts_max_lines() - 1;
                    }
                    menu_draw_custom_ghosts(sel_line);
                    UB_Buzzer_Play_MenuClick();
                }
                UB_Systick_Pause_ms(150);
            }
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            UB_Buzzer_Play_MenuClick();
            return 0;
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Buzzer_Play_MenuClick();
            return 1;
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
    menu_present_frame();
}

static uint32_t menu_run_custom_map_select(void) {
    menu_draw_custom_map_select();

    while (1) {
        // Hard buttons controls
        if (UB_Button_OnClick(BTN_UP) || UB_Button_OnClick(BTN_LEFT)) {
            Game.custom.map_id = menu_cycle_value(Game.custom.map_id, 0, MAZE_MAP_COUNT - 1, -1);
            menu_draw_custom_map_select();
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN) || UB_Button_OnClick(BTN_RIGHT)) {
            Game.custom.map_id = menu_cycle_value(Game.custom.map_id, 0, MAZE_MAP_COUNT - 1, 1);
            menu_draw_custom_map_select();
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            UB_Buzzer_Play_MenuClick();
            return 0; // Go back to main menu
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Buzzer_Play_MenuClick();
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
    UB_Font_DrawString(10, 30, "Map:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", menu_map_name(Game.campaign_map_id));
    UB_Font_DrawString(75, 30, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 2. Difficulty
    color = (sel_line == 1) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 55, "Difficulty:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%u", (unsigned int)Game.campaign_difficulty);
    UB_Font_DrawString(90, 55, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    // 3. Play Mode (Solo / Co-op)
    color = (sel_line == 2) ? MENUE_COL_ON : MENUE_COL_OFF;
    UB_Font_DrawString(10, 80, "Mode:", &Arial_7x10, color, BACKGROUND_COL);
    sprintf(buf, "%s", Game.campaign_coop ? "Pacman Co-op" : "Solo");
    UB_Font_DrawString(75, 80, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);

    UB_Font_DrawString(10, 105, "Map Preview:", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
    maze_draw_preview(Game.campaign_map_id, MENU_CAMPAIGN_PREVIEW_X, MENU_CAMPAIGN_PREVIEW_Y, MENU_CAMPAIGN_PREVIEW_CELL);

    // Draw Buttons
    // Back Button (Blue border, white text)
    UB_Graphic2D_DrawRectDMA(10, 275, 70, 25, RGB_COL_BLUE);
    UB_Font_DrawString(25, 282, "Back", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);

    // Start Button (Red border, white text)
    UB_Graphic2D_DrawRectDMA(160, 275, 70, 25, RGB_COL_RED);
    UB_Font_DrawString(175, 282, "Start", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
    menu_present_frame();
}

static uint32_t menu_run_campaign_wizard(void) {
    uint32_t sel_line = 0;

    menu_draw_campaign_wizard(sel_line);
    gui_drain_center_button();

    while (1) {
        // Hard buttons
        if (UB_Button_OnClick(BTN_UP)) {
            sel_line = menu_cycle_value(sel_line, 0, 2, -1);
            menu_draw_campaign_wizard(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_DOWN)) {
            sel_line = menu_cycle_value(sel_line, 0, 2, 1);
            menu_draw_campaign_wizard(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_LEFT)) {
            if (sel_line == 0) {
                Game.campaign_map_id = menu_cycle_value(Game.campaign_map_id, 0, MAZE_MAP_COUNT - 1, -1);
            } else if (sel_line == 1) {
                Game.campaign_difficulty = menu_cycle_value(Game.campaign_difficulty, 1, 10, -1);
            } else {
                Game.campaign_coop = Game.campaign_coop ? 0 : 1;
            }
            menu_draw_campaign_wizard(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_RIGHT)) {
            if (sel_line == 0) {
                Game.campaign_map_id = menu_cycle_value(Game.campaign_map_id, 0, MAZE_MAP_COUNT - 1, 1);
            } else if (sel_line == 1) {
                Game.campaign_difficulty = menu_cycle_value(Game.campaign_difficulty, 1, 10, 1);
            } else {
                Game.campaign_coop = Game.campaign_coop ? 0 : 1;
            }
            menu_draw_campaign_wizard(sel_line);
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_BACK)) {
            UB_Buzzer_Play_MenuClick();
            return 0; // Back to main menu
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            UB_Buzzer_Play_MenuClick();
            return 1; // Start
        }
        UB_Systick_Pause_ms(30);
    }
}

static void menu_draw_highscore_view(void) {
    uint32_t idx;
    char buf[64];
    
    gui_clear_screen();
    
    // Draw Title
    UB_Font_DrawString(81, 15, "HIGH SCORES", &Arial_7x10, RGB_COL_YELLOW, BACKGROUND_COL);
    
    // Draw divider line
    UB_Graphic2D_DrawFullRectDMA(10, 35, 220, 2, RGB_COL_BLUE);
    
    // Draw scores for each map
    for (idx = 0; idx < MAZE_MAP_COUNT; idx++) {
        const char *name = menu_map_name(idx);
        uint32_t score = Game.campaign_high_scores[idx];
        sprintf(buf, "%s: %u", name, (unsigned int)score);
        
        uint16_t len = strlen(buf) * 7;
        uint16_t x = (240 - len) / 2;
        uint16_t y = 55 + idx * 25;
        UB_Font_DrawString(x, y, buf, &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
    }
    
    // Draw prompt instruction at the bottom
    UB_Font_DrawString(29, 275, "[Back] / [Center] to Return", &Arial_7x10, RGB_COL_WHITE, BACKGROUND_COL);
    menu_present_frame();
}

static void menu_run_highscore_view(void) {
    menu_draw_highscore_view();
    UB_Systick_Pause_ms(200); // Debounce hard button click
    
    while (1) {
        if (UB_Button_OnClick(BTN_BACK) || UB_Button_OnClick(BTN_CENTER)) {
            UB_Buzzer_Play_MenuClick();
            UB_Systick_Pause_ms(150);
            return;
        }
        UB_Systick_Pause_ms(30);
    }
}
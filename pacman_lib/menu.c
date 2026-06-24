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
    if (map_id == MAZE_MAP_OPEN) {
        return "Arena";
    }
    return "Classic";
}

static const char* menu_speed_name(uint32_t idx) {
    if (idx == CUSTOM_SPEED_SLOW) return "Slow";
    if (idx == CUSTOM_SPEED_FAST) return "Fast";
    return "Normal";
}

static void menu_draw_wizard(uint32_t step, uint32_t ghost_idx) {
    char buf[32];
    uint32_t y = MENUE_STARTY;

    gui_clear_screen();
    UB_Font_DrawString(70, 5, "CUSTOM SETUP", &Arial_7x10, FONT_COL, BACKGROUND_COL);

    if (step == WIZ_MODE) {
        UB_Font_DrawString(MENUE_STARTX1, y, "Mode:", &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
        y += MENUE_DELTA1;
        sprintf(buf, "  %u Player(s)", (unsigned int)Game.custom.player_count);
        UB_Font_DrawString(MENUE_STARTX1, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        y += MENUE_DELTA1;
        UB_Font_DrawString(MENUE_STARTX1, y, "< tap value >", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
        menu_draw_nav_buttons(0, "Next");
        return;
    }

    if (step == WIZ_MAP) {
        UB_Font_DrawString(MENUE_STARTX1, y, "Map:", &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
        y += MENUE_DELTA1;
        sprintf(buf, "  %s", menu_map_name(Game.custom.map_id));
        UB_Font_DrawString(MENUE_STARTX1, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        UB_Font_DrawString(150, y - MENUE_DELTA1, "Preview", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
        maze_draw_preview(Game.custom.map_id, 150, 55, 3);
        menu_draw_nav_buttons(1, "Next");
        return;
    }

    if (step == WIZ_SPEED) {
        UB_Font_DrawString(MENUE_STARTX1, y, "Ghost Speed:", &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
        y += MENUE_DELTA1;
        sprintf(buf, "  %s", menu_speed_name(Game.custom.ghost_speed_idx));
        UB_Font_DrawString(MENUE_STARTX1, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        menu_draw_nav_buttons(1, "Next");
        return;
    }

    if (step == WIZ_2P_TYPE) {
        UB_Font_DrawString(MENUE_STARTX1, y, "2P Mode:", &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
        y += MENUE_DELTA1;
        if (Game.custom.two_player_mode == CUSTOM_2P_COOP) {
            UB_Font_DrawString(MENUE_STARTX1, y, "  Pacman Co-op", &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        } else {
            UB_Font_DrawString(MENUE_STARTX1, y, "  Pacman vs Ghost", &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        }
        menu_draw_nav_buttons(1, "Next");
        return;
    }

    if (step == WIZ_GHOST_COUNT) {
        UB_Font_DrawString(MENUE_STARTX1, y, "Ghost Count:", &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
        y += MENUE_DELTA1;
        sprintf(buf, "  %u", (unsigned int)Game.custom.ghost_count);
        UB_Font_DrawString(MENUE_STARTX1, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        y += MENUE_DELTA1 + 4;
        UB_Font_DrawString(MENUE_STARTX1, y, "(excl. player)", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
        menu_draw_nav_buttons(1, "Next");
        return;
    }

    if (step == WIZ_GHOST_SETUP) {
        UB_Font_DrawString(MENUE_STARTX1, y, "Ghost Setup:", &Arial_7x10, MENUE_COL_ON, BACKGROUND_COL);
        y += MENUE_DELTA1;
        sprintf(buf, "  Ghost %u", (unsigned int)(ghost_idx + 1));
        UB_Font_DrawString(MENUE_STARTX1, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        y += MENUE_DELTA1;
        sprintf(buf, "  %s", bot_strategy_name(Game.custom.ghost_strategies[ghost_idx]));
        UB_Font_DrawString(MENUE_STARTX1, y, buf, &Arial_7x10, MENUE_COL_VALUE, BACKGROUND_COL);
        y += MENUE_DELTA1;
        UB_Font_DrawString(MENUE_STARTX1, y, "< tap value >", &Arial_7x10, FONT_COL2, BACKGROUND_COL);
        if (ghost_idx + 1 >= Game.custom.ghost_count) {
            menu_draw_nav_buttons(1, "Start");
        } else {
            menu_draw_nav_buttons(1, "Next");
        }
    }
}

static uint32_t menu_handle_value_tap(uint32_t step, uint32_t ghost_idx, uint16_t tx, uint16_t ty) {
    uint32_t y = MENUE_STARTY + MENUE_DELTA1;

    if (menu_touch_in_rect(tx, ty, MENUE_STARTX1, y, 220, 18) == 0) {
        return 0;
    }

    if (step == WIZ_MODE) {
        Game.custom.player_count = (Game.custom.player_count == CUSTOM_PLAYER_1) ? CUSTOM_PLAYER_2 : CUSTOM_PLAYER_1;
        return 1;
    }
    if (step == WIZ_MAP) {
        Game.custom.map_id = menu_cycle_value(Game.custom.map_id, 0, MAZE_MAP_COUNT - 1, 1);
        return 1;
    }
    if (step == WIZ_SPEED) {
        Game.custom.ghost_speed_idx = menu_cycle_value(Game.custom.ghost_speed_idx, CUSTOM_SPEED_SLOW, CUSTOM_SPEED_FAST, 1);
        return 1;
    }
    if (step == WIZ_2P_TYPE) {
        Game.custom.two_player_mode = (Game.custom.two_player_mode == CUSTOM_2P_COOP) ? CUSTOM_2P_VS_GHOST : CUSTOM_2P_COOP;
        return 1;
    }
    if (step == WIZ_GHOST_COUNT) {
        Game.custom.ghost_count = menu_cycle_value(Game.custom.ghost_count, 1, CUSTOM_MAX_GHOSTS, 1);
        return 1;
    }
    if (step == WIZ_GHOST_SETUP) {
        Game.custom.ghost_strategies[ghost_idx] = menu_cycle_value(
            Game.custom.ghost_strategies[ghost_idx], 0, GHOST_STRATEGY_COUNT - 1, 1);
        return 1;
    }
    return 0;
}

static uint32_t menu_next_step(uint32_t step) {
    if (step == WIZ_MODE) return WIZ_MAP;
    if (step == WIZ_MAP) return WIZ_SPEED;
    if (step == WIZ_SPEED) {
        if (Game.custom.player_count == CUSTOM_PLAYER_2) {
            return WIZ_2P_TYPE;
        }
        return WIZ_GHOST_COUNT;
    }
    if (step == WIZ_2P_TYPE) return WIZ_GHOST_COUNT;
    if (step == WIZ_GHOST_COUNT) return WIZ_GHOST_SETUP;
    return WIZ_GHOST_SETUP;
}

static uint32_t menu_prev_step(uint32_t step) {
    if (step == WIZ_MAP) return WIZ_MODE;
    if (step == WIZ_SPEED) return WIZ_MAP;
    if (step == WIZ_2P_TYPE) return WIZ_SPEED;
    if (step == WIZ_GHOST_COUNT) {
        if (Game.custom.player_count == CUSTOM_PLAYER_2) {
            return WIZ_2P_TYPE;
        }
        return WIZ_SPEED;
    }
    return WIZ_GHOST_COUNT;
}

static uint32_t menu_run_custom_wizard(void) {
    uint32_t step = WIZ_MODE;
    uint32_t ghost_idx = 0;
    uint16_t tx;
    uint16_t ty;
    uint32_t show_back = 0;

    menu_draw_wizard(step, ghost_idx);

    while (1) {
        show_back = (step != WIZ_MODE) ? 1 : 0;

        if (menu_touch_pressed(&tx, &ty) != 0) {
            if (menu_handle_value_tap(step, ghost_idx, tx, ty) != 0) {
                menu_touch_wait_release();
                menu_draw_wizard(step, ghost_idx);
            } else if (show_back != 0 && menu_touch_in_rect(tx, ty, MENU_BTN_BACK_X, MENU_BTN_Y, MENU_BTN_W, MENU_BTN_H)) {
                menu_touch_wait_release();
                if (step == WIZ_GHOST_SETUP && ghost_idx > 0) {
                    ghost_idx--;
                } else if (step == WIZ_MODE) {
                    return 0;
                } else {
                    step = menu_prev_step(step);
                }
                menu_draw_wizard(step, ghost_idx);
            } else if (menu_touch_in_rect(tx, ty, MENU_BTN_NEXT_X, MENU_BTN_Y, 70, MENU_BTN_H)) {
                menu_touch_wait_release();
                if (step == WIZ_GHOST_SETUP) {
                    if (ghost_idx + 1 < Game.custom.ghost_count) {
                        ghost_idx++;
                    } else {
                        return 1;
                    }
                } else {
                    step = menu_next_step(step);
                }
                menu_draw_wizard(step, ghost_idx);
            }
        }

        if (UB_Button_OnClick(BTN_LEFT)) {
            if (menu_handle_value_tap(step, ghost_idx, MENUE_STARTX1, MENUE_STARTY + MENUE_DELTA1 + 1)) {
                menu_draw_wizard(step, ghost_idx);
            }
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_RIGHT)) {
            if (menu_handle_value_tap(step, ghost_idx, MENUE_STARTX1, MENUE_STARTY + MENUE_DELTA1 + 1)) {
                menu_draw_wizard(step, ghost_idx);
            }
            UB_Systick_Pause_ms(150);
        }
        if (UB_Button_OnClick(BTN_CENTER)) {
            if (step == WIZ_GHOST_SETUP && ghost_idx + 1 >= Game.custom.ghost_count) {
                return 1;
            }
            if (step == WIZ_GHOST_SETUP && ghost_idx + 1 < Game.custom.ghost_count) {
                ghost_idx++;
            } else {
                step = menu_next_step(step);
            }
            menu_draw_wizard(step, ghost_idx);
            UB_Systick_Pause_ms(150);
        }

        UB_Systick_Pause_ms(30);
    }
}
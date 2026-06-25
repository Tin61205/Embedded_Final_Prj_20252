//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "pacman.h"

#include <stdio.h>

// Global variable definition (declared extern in header)
Game_t Game;

//--------------------------------------------------------------
// level settings
// speed in ms [Player, Blinky, Pinky, Inky, Clyde]
//--------------------------------------------------------------
Level_t Level[] = {
    {
        30,
        40,
        50,
        60,
        70
    },
    {
        26,
        35,
        44,
        52,
        60
    },
    {
        22,
        30,
        38,
        44,
        50
    },
    {
        18,
        25,
        32,
        36,
        40
    },
    {
        14,
        20,
        26,
        28,
        30
    },
    {
        10,
        18,
        24,
        26,
        28
    },
    {
        10,
        16,
        22,
        24,
        26
    },
    {
        10,
        14,
        20,
        22,
        24
    },
    {
        10,
        12,
        18,
        20,
        22
    },
    {
        10,
        10,
        16,
        18,
        20
    },
};

CampaignDifficulty_t CampaignDifficultyScenario[10] = {
    // 1: 1 Clyde (Drunk), 80ms
    { 1, MOVE_CLYDE, 80, { GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK } },
    // 2: 1 Blinky (Lazy), 70ms
    { 1, MOVE_BLINKY, 70, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK } },
    // 3: 2 Ghost (1 Drunk, 1 Lazy), 65ms
    { 2, MOVE_BLINKY | MOVE_CLYDE, 65, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK } },
    // 4: 2 Ghost (1 Lazy, 1 Shy Clyde), 60ms
    { 2, MOVE_BLINKY | MOVE_CLYDE, 60, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_CLYDE } },
    // 5: 3 Ghost (1 Chase, 1 Drunk, 1 Lazy), 55ms
    { 3, MOVE_BLINKY | MOVE_PINKY | MOVE_CLYDE, 55, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_LAZY } },
    // 6: 3 Ghost (1 Chase, 1 Lazy, 1 Tricky Inky), 50ms
    { 3, MOVE_BLINKY | MOVE_INKY | MOVE_CLYDE, 50, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_INKY, GHOST_STRATEGY_LAZY } },
    // 7: 4 Ghost (2 Lazy, 1 Tricky, 1 Shy), 45ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 45, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_INKY, GHOST_STRATEGY_CLYDE } },
    // 8: 4 Ghost (1 Chase, 1 Ambush, 1 Lazy, 1 Shy), 40ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 40, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_PINKY, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_CLYDE } },
    // 9: 4 Ghost (Chase, Ambush, Tricky, Shy) - Mức gốc, 32ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 32, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_PINKY, GHOST_STRATEGY_INKY, GHOST_STRATEGY_CLYDE } },
    // 10: 4 Ghost (Chase, Ambush, Tricky, Shy) - Ác mộng, 24ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 24, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_PINKY, GHOST_STRATEGY_INKY, GHOST_STRATEGY_CLYDE } }
};

void pacman_apply_campaign_difficulty(void) {
    uint32_t diff = Game.campaign_difficulty;
    if (diff < 1) diff = 1;
    if (diff > 10) diff = 10;
    
    CampaignDifficulty_t config = CampaignDifficultyScenario[diff - 1];
    
    Game.numberOfBots = config.ghost_count;
    Game.ghost_active_mask = config.active_mask;
    
    Blinky.akt_speed_ms = config.ghost_speed;
    Pinky.akt_speed_ms = config.ghost_speed;
    Inky.akt_speed_ms = config.ghost_speed;
    Clyde.akt_speed_ms = config.ghost_speed;
    
    Blinky.strategy = config.strategies[0];
    Pinky.strategy = config.strategies[1];
    Inky.strategy = config.strategies[2];
    Clyde.strategy = config.strategies[3];
    
    Player.akt_speed_ms = 30; // Standard speed for player
    Player.level = diff;
}

extern USB_HID_HOST_STATUS_t usb_status; // Trạng thái Keyboard
uint32_t pacman_hw_init(void);
uint32_t pacman_play(void);
void pacman_dec_mode_timer(void);
void pacman_init(uint32_t mode);
void pacman_set_level(void);

//--------------------------------------------------------------
// init and start endless pacman gameplay
//--------------------------------------------------------------
void pacman_start(void) {
    uint32_t check;
    char buf[32];

    Game.numberOfBots = 4;
    Game.play_type = GAME_PLAY_CAMPAIGN;
    Game.ghost_active_mask = MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE;
    Game.player2_joy = GUI_JOY_NONE;
    Game.player2_active = 0;

    pacman_init(GAME_OVER);
    check = pacman_hw_init();

    if (check != 0) {
        UB_LCD_FillLayer(BACKGROUND_COL);
        UB_Font_DrawString(10, 280, "Touch ERR", & Arial_7x10, FONT_COL, BACKGROUND_COL);
        while (1);
    }

    // Khởi tạo Player và ghost
    player_init(GAME_OVER);
    blinky_init(GAME_OVER);
    pinky_init(GAME_OVER);
    inky_init(GAME_OVER);
    clyde_init(GAME_OVER);

    menu_start();

    skin_init();

    maze_build_map((Game.play_type == GAME_PLAY_CUSTOM) ? Game.custom.map_id : Game.campaign_map_id);
    if (Game.play_type == GAME_PLAY_CUSTOM) {
        pacman_apply_custom_config(GAME_OVER);
    } else {
        pacman_apply_campaign_difficulty();
    }
    check = maze_generate_check();

    // Wait for PA0 (BTN_CENTER) press to start
    {
        //UB_Font_DrawString(0, 0, "Press PA0 to start", &Arial_7x10, 0x07FF, BACKGROUND_COL);
        // Wait until PA0 is pressed (pin goes LOW)
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET);
        // Wait until PA0 is released
        while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != Bit_RESET);
        UB_Font_DrawString(0, 0, "                  ", &Arial_7x10, 0x07FF, BACKGROUND_COL);
    }

    if (check == 0) {
        // Maze OK
        while (1) {
            gui_draw_maze();
            gui_draw_bots();
            GUI.refresh_value = GUI_REFRESH_VALUE;
            GUI.refresh_buttons = GUI_REFRESH_VALUE;
            gui_draw_gui(GUI_JOY_NONE);

            UB_Font_DrawString(10, 305, "GET READY", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            UB_Systick_Pause_ms(1000);
            UB_Font_DrawString(10, 305, "    1    ", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            UB_Systick_Pause_ms(1000);
            UB_Font_DrawString(10, 305, "    2    ", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            UB_Systick_Pause_ms(1000);
            UB_Font_DrawString(10, 305, "    3    ", & Arial_7x10, FONT_COL2, BACKGROUND_COL);
            UB_Systick_Pause_ms(1000);
            UB_Font_DrawString(10, 305, "    GO   ", & Arial_7x10, FONT_COL2, BACKGROUND_COL);

            check = pacman_play();

            if (check == GAME_PLAYER_WIN) {
                gui_show_win_screen(Player.score);
                if (Game.campaign_difficulty < 10) {
                    Game.campaign_difficulty++;
                }
            } else {
                if (Game.player2_active != 0) {
                    if (bot_is_2p_coop()) {
                        if (Player.lives == 0 && Player2.lives == 0) {
                            check = GAME_OVER;
                        }
                    } else {
                        if (Player.lives == 0) {
                            check = GAME_OVER;
                        }
                    }
                } else {
                    Player.lives--;
                    if (Player.lives == 0) {
                        check = GAME_OVER;
                    }
                }

                if (check == GAME_OVER) {
                    gui_show_lost_screen(Player.score);
                } else {
                    UB_Systick_Pause_ms(2000); // Delay for respawn feedback
                }
            }

            pacman_init(check);
            player_init(check);
            if (Game.play_type == GAME_PLAY_CUSTOM) {
                pacman_apply_custom_config(check);
            } else {
                Game.player2_active = 0;
                blinky_init(check);
                pinky_init(check);
                inky_init(check);
                clyde_init(check);
                pacman_apply_campaign_difficulty();
            }

            if (check == GAME_OVER) {
                menu_start();
                if (Game.play_type == GAME_PLAY_CUSTOM) {
                    pacman_apply_custom_config(GAME_OVER);
                } else {
                    blinky_init(GAME_OVER);
                    pinky_init(GAME_OVER);
                    inky_init(GAME_OVER);
                    clyde_init(GAME_OVER);
                    pacman_apply_campaign_difficulty();
                }
            }
            if ((check == GAME_PLAYER_WIN) || (check == GAME_OVER)) {
                maze_build_map((Game.play_type == GAME_PLAY_CUSTOM) ? Game.custom.map_id : Game.campaign_map_id);
            }
        }
    } else {
        // Maze error
        gui_draw_errmaze();
        UB_Font_DrawString(10, 280, "MAZE ERR", & Arial_7x10, FONT_COL, BACKGROUND_COL);
        sprintf(buf, "last error nr = %d", (int)(check));
        UB_Font_DrawString(10, 290, buf, & Arial_7x10, FONT_COL, BACKGROUND_COL);
    }
}

uint32_t pacman_hw_init(void) {
    uint32_t ret_wert = 0;

    if (UB_Touch_Init() != SUCCESS) {
        ret_wert = 1;
    }

    UB_Systick_Init();
    UB_USB_HID_HOST_Init();
    UB_Uart_Init();
    UB_Button_Init();

    UB_LCD_Init();
    UB_LCD_LayerInit_Fullscreen();
    gui_clear_screen();

    return ret_wert;
}

//--------------------------------------------------------------
// init values
//--------------------------------------------------------------
void pacman_init(uint32_t mode) {
    if (mode == GAME_OVER) {
        Game.collision = BOOL_TRUE;
        Game.controller = GAME_CONTROL_4BUTTON;
    }
    Game.mode = GAME_MODE_SCATTER;
    Game.mode_timer = GAME_SCATTER_TIME;
    Game.frightened = BOOL_FALSE;
    Game.frightened_timer = GAME_FRIGHTENED_TIME;
    Game.frightened_points = GAME_FRIGHTENED_START_POINTS;
}

//--------------------------------------------------------------
// init Level
//--------------------------------------------------------------
void pacman_set_level(void) {
    if (Player.level <= GAME_MAX_LEVEL) {
        Player.akt_speed_ms = Level[Player.level - 1].player_speed;
        Blinky.akt_speed_ms = Level[Player.level - 1].blinky_speed;
        Pinky.akt_speed_ms = Level[Player.level - 1].pinky_speed;
        Inky.akt_speed_ms = Level[Player.level - 1].inky_speed;
        Clyde.akt_speed_ms = Level[Player.level - 1].clyde_speed;
    } else {
        Player.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].player_speed;
        Blinky.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].blinky_speed;
        Pinky.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].pinky_speed;
        Inky.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].inky_speed;
        Clyde.akt_speed_ms = Level[GAME_MAX_LEVEL - 1].clyde_speed;
    }
}

void pacman_apply_custom_config(uint32_t mode) {
    uint32_t speed_ms = GHOST_SPEED_NORMAL_MS;
    uint32_t i;

    if (Game.custom.ghost_speed_idx == CUSTOM_SPEED_SLOW) {
        speed_ms = GHOST_SPEED_SLOW_MS;
    } else if (Game.custom.ghost_speed_idx == CUSTOM_SPEED_FAST) {
        speed_ms = GHOST_SPEED_FAST_MS;
    }

    if (mode == GAME_OVER) {
        Player.level = 1;
        Player.lives = PLAYER_START_LIVES;
        Player2.lives = PLAYER_START_LIVES;
        Player.score = 0;
    }
    Player.akt_speed_ms = Level[0].player_speed;

    Game.ghost_active_mask = 0;
    for (i = 0; i < Game.custom.ghost_count; i++) {
        if (i == 0) Game.ghost_active_mask |= MOVE_BLINKY;
        if (i == 1) Game.ghost_active_mask |= MOVE_PINKY;
        if (i == 2) Game.ghost_active_mask |= MOVE_INKY;
        if (i == 3) Game.ghost_active_mask |= MOVE_CLYDE;
    }

    bot_apply_custom_ghosts(Game.custom.ghost_count, Game.custom.ghost_strategies, speed_ms);

    Game.player2_active = 0;
    if (Game.custom.player_count == CUSTOM_PLAYER_2 &&
        Game.custom.two_player_mode == CUSTOM_2P_COOP) {
        Game.player2_active = 1;
        player2_init(mode);
    }
}

//--------------------------------------------------------------
// play one round pacman (until pacman die or level complete)
//--------------------------------------------------------------
uint32_t pacman_play(void) {
    uint32_t ret_wert = GAME_RUN;
    uint32_t joy = GUI_JOY_NONE;
    uint32_t movement = 0;
    int32_t pl_speed;

    // copy screen to background
    UB_LCD_Copy_Layer2_to_Layer1();

    do {
        //----------------------------------------
        // Touch/Button Timer
        //----------------------------------------
        if (Gui_Touch_Timer_ms == 0) {
            Gui_Touch_Timer_ms = GUI_TOUCH_INTERVALL_MS;

            joy = gui_check_button();
            if (joy == GUI_JOY_NONE) {
                joy = gui_check_touch();
            }
            Game.player2_joy = GUI_JOY_NONE;
            if (Game.play_type == GAME_PLAY_CUSTOM && Game.custom.player_count == CUSTOM_PLAYER_2) {
                uint32_t kb_joy = gui_check_keyboard();
                if (Game.custom.two_player_mode == CUSTOM_2P_COOP) {
                    Game.player2_joy = kb_joy;
                } else if (Game.custom.two_player_mode == CUSTOM_2P_VS_GHOST) {
                    Game.player2_joy = kb_joy;
                }
            }
        }

        movement = MOVE_NOBODY;

        //----------------------------------------
        // Player Timer
        //----------------------------------------
        if (Player_Systick_Timer_ms == 0) {
            if (Game.frightened == BOOL_FALSE) {
                Player_Systick_Timer_ms = Player.akt_speed_ms;
            } else {
                pl_speed = Player.akt_speed_ms - Player.frightened_buf;
                if (pl_speed < PLAYER_MAX_SPEED) pl_speed = PLAYER_MAX_SPEED;
                Player_Systick_Timer_ms = pl_speed;
            }
            movement |= MOVE_PLAYER;
        }

        if (Game.player2_active != 0 && Player2_Systick_Timer_ms == 0) {
            if (Game.frightened == BOOL_FALSE) {
                Player2_Systick_Timer_ms = Player2.akt_speed_ms;
            } else {
                pl_speed = Player2.akt_speed_ms - Player2.frightened_buf;
                if (pl_speed < PLAYER_MAX_SPEED) pl_speed = PLAYER_MAX_SPEED;
                Player2_Systick_Timer_ms = pl_speed;
            }
            movement |= MOVE_PLAYER2;
        }

        //----------------------------------------
        // Blinky Timer
        //----------------------------------------
        if (Blinky_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_BLINKY) != 0) {
            if (Blinky.status == GHOST_STATUS_ALIVE) {
                if (Game.frightened == BOOL_FALSE) {
                    Blinky_Systic_Timer_ms = Blinky.akt_speed_ms;
                } else {
                    Blinky_Systic_Timer_ms = Blinky.akt_speed_ms + Blinky.frightened_buf;
                }
            } else {
                Blinky_Systic_Timer_ms = GHOST_DEAD_DELAY_MS;
            }
            movement |= MOVE_BLINKY;
        }

        //----------------------------------------
        // Pinky Timer
        //----------------------------------------
        if (Pinky_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_PINKY) != 0) {
            if (Pinky.status == GHOST_STATUS_ALIVE) {
                if (Game.frightened == BOOL_FALSE) {
                    Pinky_Systic_Timer_ms = Pinky.akt_speed_ms;
                } else {
                    Pinky_Systic_Timer_ms = Pinky.akt_speed_ms + Pinky.frightened_buf;
                }
            } else {
                Pinky_Systic_Timer_ms = GHOST_DEAD_DELAY_MS;
            }
            movement |= MOVE_PINKY;
        }

        //----------------------------------------
        // Inky Timer
        //----------------------------------------
        if (Inky_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_INKY) != 0) {
            if (Inky.status == GHOST_STATUS_ALIVE) {
                if (Game.frightened == BOOL_FALSE) {
                    Inky_Systic_Timer_ms = Inky.akt_speed_ms;
                } else {
                    Inky_Systic_Timer_ms = Inky.akt_speed_ms + Inky.frightened_buf;
                }
            } else {
                Inky_Systic_Timer_ms = GHOST_DEAD_DELAY_MS;
            }
            movement |= MOVE_INKY;
        }

        //----------------------------------------
        // Clyde Timer
        //----------------------------------------
        if (Clyde_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_CLYDE) != 0) {
            if (Clyde.status == GHOST_STATUS_ALIVE) {
                if (Game.frightened == BOOL_FALSE) {
                    Clyde_Systic_Timer_ms = Clyde.akt_speed_ms;
                } else {
                    Clyde_Systic_Timer_ms = Clyde.akt_speed_ms + Clyde.frightened_buf;
                }
            } else {
                Clyde_Systic_Timer_ms = GHOST_DEAD_DELAY_MS;
            }
            movement |= MOVE_CLYDE;
        }

        //----------------------------------------
        // Mode Timer
        //----------------------------------------
        if (Mode_Systic_Timer_ms == 0) {
            Mode_Systic_Timer_ms = GAME_MODE_TIMER;
            pacman_dec_mode_timer();
        }

        //----------------------------------------
        // 1. clear all Bots
        //----------------------------------------
        if (movement != MOVE_NOBODY) {
            gui_clear_bots();
        }

        //----------------------------------------
        // 2a. Player movement
        //----------------------------------------
        if ((movement & MOVE_PLAYER) != 0) {
            player_move();
            player_change_direction(joy);
        }

        if ((movement & MOVE_PLAYER2) != 0 && Game.player2_active != 0) {
            player2_move();
            player2_change_direction(Game.player2_joy);
        }

        if ((movement & MOVE_BLINKY) != 0 && (Game.ghost_active_mask & MOVE_BLINKY) != 0) {
            blinky_move();
        }

        if ((movement & MOVE_PINKY) != 0 && (Game.ghost_active_mask & MOVE_PINKY) != 0) {
            pinky_move();
        }

        if ((movement & MOVE_INKY) != 0 && (Game.ghost_active_mask & MOVE_INKY) != 0) {
            inky_move();
        }

        if ((movement & MOVE_CLYDE) != 0 && (Game.ghost_active_mask & MOVE_CLYDE) != 0) {
            clyde_move();
        }

        //----------------------------------------
        // 3. redraw all Bots + 4. LCD refresh
        //----------------------------------------
        if (movement != MOVE_NOBODY) {
            player_check_collisions();
            gui_draw_bots();
            gui_draw_gui(joy);
            UB_LCD_Refresh();
        }

        //----------------------------------------
        // check if game over
        //----------------------------------------
        if (Game.player2_active != 0) {
            if (bot_coop_is_game_over() != 0) {
                ret_wert = GAME_PLAYER_LOSE;
            }
        } else if (Player.status == PLAYER_STATUS_DEAD) {
            ret_wert = GAME_PLAYER_LOSE;
        }
        if (Player.status == PLAYER_STATUS_WIN ||
            (Game.player2_active != 0 && Player2.status == PLAYER_STATUS_WIN)) {
            ret_wert = GAME_PLAYER_WIN;
        }
    }
    while (ret_wert == GAME_RUN);

    return ret_wert;
}

//--------------------------------------------------------------
// decrement mode timer and change mode if necessary
//--------------------------------------------------------------
void pacman_dec_mode_timer(void) {
    if (Game.frightened == BOOL_FALSE) {
        Game.mode_timer--;
        if (Game.mode_timer == 0) {
            GUI.refresh_value = GUI_REFRESH_VALUE;
            if (Game.mode == GAME_MODE_SCATTER) {
                Game.mode = GAME_MODE_CHASE;
                Game.mode_timer = GAME_CHASE_TIME;
            } else {
                Game.mode = GAME_MODE_SCATTER;
                Game.mode_timer = GAME_SCATTER_TIME;
            }
            if (Blinky.status == GHOST_STATUS_ALIVE) Blinky.new_mode = 1;
            if (Pinky.status == GHOST_STATUS_ALIVE) Pinky.new_mode = 1;
            if (Inky.status == GHOST_STATUS_ALIVE) Inky.new_mode = 1;
            if (Clyde.status == GHOST_STATUS_ALIVE) Clyde.new_mode = 1;
        }
    } else {
        Game.frightened_timer--;
        if (Game.frightened_timer == 0) {
            Game.frightened = BOOL_FALSE;
            Game.frightened_points = GAME_FRIGHTENED_START_POINTS;
        }
    }
}
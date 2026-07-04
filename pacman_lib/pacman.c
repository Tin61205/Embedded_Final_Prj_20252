//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "pacman.h"
#include <stdio.h>
#include "stm32_ub_buzzer.h"
#include "humanghost.h"

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
    // Ghost delay: 34ms (lv1, slowest) → 24ms (lv10, fastest)
    // 1: 4 Ghost (all Lazy), 34ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 34, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_LAZY } },
    // 2: 4 Ghost (all Drunk), 33ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 33, { GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK } },
    // 3: 3 Ghost (Lazy + 2 Drunk), 32ms
    { 3, MOVE_BLINKY | MOVE_PINKY | MOVE_CLYDE, 32, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK } },
    // 4: 4 Ghost (Lazy + Drunk mix), 31ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 31, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_DRUNK } },
    // 5: 3 Ghost (1 Chase, 1 Drunk, 1 Lazy), 30ms
    { 3, MOVE_BLINKY | MOVE_PINKY | MOVE_CLYDE, 30, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_LAZY } },
    // 6: 3 Ghost (1 Chase, 1 Lazy, 1 Tricky Inky), 29ms
    { 3, MOVE_BLINKY | MOVE_INKY | MOVE_CLYDE, 29, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_DRUNK, GHOST_STRATEGY_INKY, GHOST_STRATEGY_LAZY } },
    // 7: 4 Ghost (2 Lazy, 1 Tricky, 1 Shy), 28ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 28, { GHOST_STRATEGY_LAZY, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_INKY, GHOST_STRATEGY_CLYDE } },
    // 8: 4 Ghost (1 Chase, 1 Ambush, 1 Lazy, 1 Shy), 27ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 27, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_PINKY, GHOST_STRATEGY_LAZY, GHOST_STRATEGY_CLYDE } },
    // 9: 4 Ghost (Chase, Ambush, Tricky, Shy), 26ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 26, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_PINKY, GHOST_STRATEGY_INKY, GHOST_STRATEGY_CLYDE } },
    // 10: 4 Ghost (Chase, Ambush, Tricky, Shy) - Ác mộng, 24ms
    { 4, MOVE_BLINKY | MOVE_PINKY | MOVE_INKY | MOVE_CLYDE, 24, { GHOST_STRATEGY_BLINKY, GHOST_STRATEGY_PINKY, GHOST_STRATEGY_INKY, GHOST_STRATEGY_CLYDE } }
};

/* Pacman delay (ms/tick): cố định 28ms cho mọi cấp độ Campaign. */
static const uint32_t CampaignPlayerSpeedMs[10] = {
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28
};

void pacman_apply_campaign_difficulty(uint32_t mode) {
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
    
    Player.akt_speed_ms = CampaignPlayerSpeedMs[diff - 1];
    Player.level = diff;

    Game.player2_active = 0;
    if (Game.campaign_coop != 0) {
        Game.player2_active = 1;
        player2_init(mode);
    }
}

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
    Game.campaign_coop = 0;

    {
        uint32_t i;
        for (i = 0; i < MAZE_MAP_COUNT; i++) {
            Game.campaign_high_scores[i] = 0;
        }
    }

    pacman_init(GAME_OVER);
    pacman_hw_init();

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
        pacman_apply_campaign_difficulty(GAME_OVER);
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

            {
                uint32_t played_ms;

                gui_show_countdown_text("GET READY", 2);
                played_ms = UB_Buzzer_Play_Countdown(BUZZER_COUNTDOWN_READY);
                if (played_ms < 1000U) {
                    UB_Systick_Pause_ms(1000U - played_ms);
                }

                gui_show_countdown_text("3", 3);
                played_ms = UB_Buzzer_Play_Countdown(BUZZER_COUNTDOWN_3);
                if (played_ms < 1000U) {
                    UB_Systick_Pause_ms(1000U - played_ms);
                }

                gui_show_countdown_text("2", 3);
                played_ms = UB_Buzzer_Play_Countdown(BUZZER_COUNTDOWN_2);
                if (played_ms < 1000U) {
                    UB_Systick_Pause_ms(1000U - played_ms);
                }

                gui_show_countdown_text("1", 3);
                played_ms = UB_Buzzer_Play_Countdown(BUZZER_COUNTDOWN_1);
                if (played_ms < 1000U) {
                    UB_Systick_Pause_ms(1000U - played_ms);
                }

                gui_show_countdown_text("GO!", 3);
                played_ms = UB_Buzzer_Play_Countdown(BUZZER_COUNTDOWN_GO);
                if (played_ms < 1500U) {
                    UB_Systick_Pause_ms(1500U - played_ms);
                }
            }

            // Redraw clean maze/bots/gui to remove countdown text overlay
            gui_draw_maze();
            gui_draw_bots();
            gui_draw_gui(GUI_JOY_NONE);

            check = pacman_play();

            if (check == GAME_EXIT) {
                menu_start();
                pacman_init(GAME_OVER);
                player_init(GAME_OVER);
                if (Game.play_type == GAME_PLAY_CUSTOM) {
                    pacman_apply_custom_config(GAME_OVER);
                } else {
                    blinky_init(GAME_OVER);
                    pinky_init(GAME_OVER);
                    inky_init(GAME_OVER);
                    clyde_init(GAME_OVER);
                    pacman_apply_campaign_difficulty(GAME_OVER);
                }
                maze_build_map((Game.play_type == GAME_PLAY_CUSTOM) ? Game.custom.map_id : Game.campaign_map_id);
                continue;
            }

            if (check == GAME_PLAYER_WIN) {
                if (Game.play_type == GAME_PLAY_CAMPAIGN) {
                    if (Player.score > Game.campaign_high_scores[Game.campaign_map_id]) {
                        Game.campaign_high_scores[Game.campaign_map_id] = Player.score;
                    }
                    if (Game.campaign_difficulty < 10) {
                        Game.campaign_difficulty++;
                    }
                }
                gui_show_win_screen(Player.score);
                if (Game.play_type != GAME_PLAY_CAMPAIGN) {
                    Player.score = 0;
                }
                if (Game.play_type == GAME_PLAY_CUSTOM) {
                    check = GAME_OVER;
                }
            } else {
                if (Game.player2_active != 0 && bot_is_2p_coop()) {
                    if (Player.lives == 0 && Player2.lives == 0) {
                        check = GAME_OVER;
                    }
                } else {
                    if (Player.lives == 0) {
                        check = GAME_OVER;
                    }
                }

                if (check == GAME_OVER) {
                    if (Game.play_type == GAME_PLAY_CAMPAIGN) {
                        if (Player.score > Game.campaign_high_scores[Game.campaign_map_id]) {
                            Game.campaign_high_scores[Game.campaign_map_id] = Player.score;
                        }
                    }
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
                blinky_init(check);
                pinky_init(check);
                inky_init(check);
                clyde_init(check);
                pacman_apply_campaign_difficulty(check);
            }

            UB_Systick_Reset_Player_Timers();

            Game.frightened_points = (GAME_FRIGHTENED_START_POINTS * (Player.level + 1)) / 2;

            if (check == GAME_OVER) {
                menu_start();
                if (Game.play_type == GAME_PLAY_CUSTOM) {
                    pacman_apply_custom_config(GAME_OVER);
                } else {
                    blinky_init(GAME_OVER);
                    pinky_init(GAME_OVER);
                    inky_init(GAME_OVER);
                    clyde_init(GAME_OVER);
                    pacman_apply_campaign_difficulty(GAME_OVER);
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

    UB_Systick_Init();
    UB_Buzzer_Init();
    UB_Uart_Init();
    UB_Button_Init();
#if JOYSTICK_USE_ADC == 1
    UB_Joystick_Init();
#endif

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
    if (Game.play_type == GAME_PLAY_CUSTOM) {
        Game.mode = GAME_MODE_CHASE;
        Game.mode_timer = GAME_CHASE_TIME;
    } else {
        Game.mode = GAME_MODE_SCATTER;
        Game.mode_timer = GAME_SCATTER_TIME;
    }
    Game.frightened = BOOL_FALSE;
    Game.frightened_timer = GAME_FRIGHTENED_TIME;
    Game.frightened_points = (GAME_FRIGHTENED_START_POINTS * (Player.level + 1)) / 2;
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
    uint32_t player_speed_ms = PLAYER_SPEED_NORMAL_MS;
    uint32_t i;

    if (Game.custom.ghost_speed_idx == CUSTOM_SPEED_SLOW) {
        speed_ms = GHOST_SPEED_SLOW_MS;
    } else if (Game.custom.ghost_speed_idx == CUSTOM_SPEED_FAST) {
        speed_ms = GHOST_SPEED_FAST_MS;
    }

    if (Game.custom.player_speed_idx == CUSTOM_SPEED_SLOW) {
        player_speed_ms = PLAYER_SPEED_SLOW_MS;
    } else if (Game.custom.player_speed_idx == CUSTOM_SPEED_FAST) {
        player_speed_ms = PLAYER_SPEED_FAST_MS;
    }

    if (mode == GAME_OVER) {
        Player.level = 1;
        Player.lives = PLAYER_START_LIVES;
        Player2.lives = PLAYER_START_LIVES;
        Player.score = 0;
    }
    Player.akt_speed_ms = player_speed_ms;

    Game.ghost_active_mask = 0;
    {
        uint32_t ai_count = Game.custom.ghost_count;
        if (bot_is_2p_vs_ghost() != 0) {
            ai_count = bot_custom_ai_ghost_count();
            Game.ghost_active_mask |= MOVE_HUMAN_GHOST;
        }
        for (i = 0; i < ai_count; i++) {
            if (i == 0) Game.ghost_active_mask |= MOVE_BLINKY;
            if (i == 1) Game.ghost_active_mask |= MOVE_PINKY;
            if (i == 2) Game.ghost_active_mask |= MOVE_INKY;
            if (i == 3) Game.ghost_active_mask |= MOVE_CLYDE;
        }
    }

    bot_apply_custom_ghosts(Game.custom.ghost_count, Game.custom.ghost_strategies, speed_ms);

    Game.mode = GAME_MODE_CHASE;
    Game.mode_timer = GAME_CHASE_TIME;

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
    extern uint32_t Player_Dying_Timer_ms;
    extern uint32_t Player_Invuln_Timer_ms;
    extern uint32_t Player2_Dying_Timer_ms;
    extern uint32_t Player2_Invuln_Timer_ms;

    uint32_t ret_wert = GAME_RUN;
    uint32_t joy = GUI_JOY_NONE;
    uint32_t movement = 0;
    int32_t pl_speed;

    // copy screen to background
    UB_LCD_Copy_Layer2_to_Layer1();

    do {
        // --- XỬ LÝ RESPAWN NON-BLOCKING CHO PLAYER 1 ---
        if (Player.status == PLAYER_STATUS_DYING && Player_Dying_Timer_ms == 0) {
            if (Player.lives == 0) {
                Player.status = PLAYER_STATUS_DEAD;
            } else {
                uint32_t rx = Player.respawn_x;
                uint32_t ry = Player.respawn_y;
                bot_find_safe_respawn(Player.respawn_x, Player.respawn_y, &rx, &ry);
                Player.xp = rx;
                Player.yp = ry;
                Player.delta_x = 0;
                Player.delta_y = 0;
                Player.move = MOVE_STOP;
                Player.skin = PLAYER_SKIN_LEFT1;
                Player.skin_cnt = 0;
                Player.port = PORT_DONE;
                Player.status = PLAYER_STATUS_ALIVE;
                Player_Invuln_Timer_ms = 1500; // Bất tử trong 1.5 giây

                // Vẽ lại toàn bộ mê cung và đồng bộ 2 layer LCD để xóa hoàn toàn sprite cũ ở vị trí chết
                gui_draw_maze();
                gui_draw_bots();
                UB_LCD_Copy_Layer2_to_Layer1();
                UB_LCD_Refresh();
            }
            bot_release_ghosts_on_pacman_death();
            GUI.refresh_value = GUI_REFRESH_VALUE;
        }

        // --- XỬ LÝ RESPAWN NON-BLOCKING CHO PLAYER 2 ---
        if (Game.player2_active != 0 && Player2.status == PLAYER_STATUS_DYING && Player2_Dying_Timer_ms == 0) {
            if (Player2.lives == 0) {
                Player2.status = PLAYER_STATUS_DEAD;
            } else {
                uint32_t rx = Player2.respawn_x;
                uint32_t ry = Player2.respawn_y;
                bot_find_safe_respawn(Player2.respawn_x, Player2.respawn_y, &rx, &ry);
                Player2.xp = rx;
                Player2.yp = ry;
                Player2.delta_x = 0;
                Player2.delta_y = 0;
                Player2.move = MOVE_STOP;
                Player2.skin = PLAYER_SKIN_LEFT1;
                Player2.skin_cnt = 0;
                Player2.port = PORT_DONE;
                Player2.status = PLAYER_STATUS_ALIVE;
                Player2_Invuln_Timer_ms = 1500; // Bất tử trong 1.5 giây

                // Vẽ lại toàn bộ mê cung và đồng bộ 2 layer LCD để xóa hoàn toàn sprite cũ ở vị trí chết
                gui_draw_maze();
                gui_draw_bots();
                UB_LCD_Copy_Layer2_to_Layer1();
                UB_LCD_Refresh();
            }
            bot_release_ghosts_on_pacman_death();
            GUI.refresh_value = GUI_REFRESH_VALUE;
        }

        //----------------------------------------
        // Touch/Button Timer
        //----------------------------------------
        if (Gui_Touch_Timer_ms == 0) {
            Gui_Touch_Timer_ms = GUI_TOUCH_INTERVALL_MS;

            // Player 1 controls via Joystick 1 in gameplay
            joy = gui_check_joystick1();
            if (joy == GUI_JOY_NONE) {
                joy = gui_check_touch();
            }

            // Player 2 controls via Joystick 2 in gameplay
            if (Game.player2_active != 0 || bot_is_2p_vs_ghost() != 0) {
                Game.player2_joy = gui_check_joystick2();
            } else {
                Game.player2_joy = GUI_JOY_NONE;
            }
        }

        if (UB_Button_OnClick(BTN_BACK)) {
            if (gui_run_pause_menu() == GUI_PAUSE_EXIT) {
                ret_wert = GAME_EXIT;
            }
        }

        movement = MOVE_NOBODY;

        {
            uint32_t allow_ghost_move = bot_should_allow_ghost_move();

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
        if (allow_ghost_move != 0 && Blinky_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_BLINKY) != 0) {
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
        if (allow_ghost_move != 0 && Pinky_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_PINKY) != 0) {
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
        if (allow_ghost_move != 0 && Inky_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_INKY) != 0) {
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
        if (allow_ghost_move != 0 && Clyde_Systic_Timer_ms == 0 && (Game.ghost_active_mask & MOVE_CLYDE) != 0) {
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

        if (allow_ghost_move != 0 && HumanGhost_Systic_Timer_ms == 0 &&
            (Game.ghost_active_mask & MOVE_HUMAN_GHOST) != 0) {
            if (HumanGhost.status == GHOST_STATUS_ALIVE) {
                if (Game.frightened == BOOL_FALSE) {
                    HumanGhost_Systic_Timer_ms = HumanGhost.akt_speed_ms;
                } else {
                    HumanGhost_Systic_Timer_ms = HumanGhost.akt_speed_ms + HumanGhost.frightened_buf;
                }
            } else {
                HumanGhost_Systic_Timer_ms = GHOST_DEAD_DELAY_MS;
            }
            movement |= MOVE_HUMAN_GHOST;
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

        if (allow_ghost_move != 0 && (movement & MOVE_BLINKY) != 0 && (Game.ghost_active_mask & MOVE_BLINKY) != 0) {
            blinky_move();
        }

        if (allow_ghost_move != 0 && (movement & MOVE_PINKY) != 0 && (Game.ghost_active_mask & MOVE_PINKY) != 0) {
            pinky_move();
        }

        if (allow_ghost_move != 0 && (movement & MOVE_INKY) != 0 && (Game.ghost_active_mask & MOVE_INKY) != 0) {
            inky_move();
        }

        if (allow_ghost_move != 0 && (movement & MOVE_CLYDE) != 0 && (Game.ghost_active_mask & MOVE_CLYDE) != 0) {
            clyde_move();
        }

        if (allow_ghost_move != 0 && (movement & MOVE_HUMAN_GHOST) != 0 &&
            (Game.ghost_active_mask & MOVE_HUMAN_GHOST) != 0) {
            if (HumanGhost.status == GHOST_STATUS_ALIVE) {
                bot_apply_player_ghost_input(&HumanGhost, Game.player2_joy);
            }
            humanghost_move();
        }
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
            if (bot_is_human_ghost_active() != 0 && HumanGhost.status == GHOST_STATUS_ALIVE) {
                HumanGhost.new_mode = 1;
            }
        }
    } else {
        Game.frightened_timer--;
        if (Game.frightened_timer == 0) {
            Game.frightened = BOOL_FALSE;
            Game.frightened_points = (GAME_FRIGHTENED_START_POINTS * (Player.level + 1)) / 2;
        }
    }
}
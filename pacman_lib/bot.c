//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "bot.h"

#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#include "stm32_ub_buzzer.h"
#include "gui.h"
#include "pacman.h"
#include "player.h"
#include "ghost.h"
#include "humanghost.h"

// Global variable definitions (declared extern in header)
Player_t Player;
Player_t Player2;
Ghost_t Ghosts[GHOST_MAX];
Ghost_t HumanGhost;

static uint32_t Ghost_Spawn_X[4];
static uint32_t Ghost_Spawn_Y[4];
static uint32_t HumanGhost_Spawn_X;
static uint32_t HumanGhost_Spawn_Y;
/* Set by bot_move when a dead ghost returns home (avoid portal loops). */
uint32_t bot_avoid_portal = 0;

static uint32_t bot_ghost_skin_for_dir(uint32_t dir);
static uint32_t bot_ghost_dot_cnt_max(uint32_t ghost_id);

uint32_t UB_SQRT(uint32_t wert);
uint32_t bot_calc_move(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t akt_dir);
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

//--------------------------------------------------------------
// Mode / multiplayer helpers
//--------------------------------------------------------------
uint32_t bot_is_2p_coop(void) {
    if (Game.play_type == GAME_PLAY_CUSTOM) {
        return (Game.custom.player_count == CUSTOM_PLAYER_2 &&
                Game.custom.two_player_mode == CUSTOM_2P_COOP &&
                Game.player2_active != 0) ? 1 : 0;
    } else if (Game.play_type == GAME_PLAY_CAMPAIGN) {
        return (Game.campaign_coop != 0 && Game.player2_active != 0) ? 1 : 0;
    }
    return 0;
}

uint32_t bot_is_2p_vs_ghost(void) {
    return (Game.play_type == GAME_PLAY_CUSTOM &&
            Game.custom.player_count == CUSTOM_PLAYER_2 &&
            Game.custom.two_player_mode == CUSTOM_2P_VS_GHOST) ? 1 : 0;
}

uint32_t bot_custom_ai_ghost_count(void) {
    if (bot_is_2p_vs_ghost()) {
        return (Game.custom.ghost_count > 0) ? (Game.custom.ghost_count - 1) : 0;
    }
    return Game.custom.ghost_count;
}

uint32_t bot_is_player_controlled_ghost(uint32_t ghost_id) {
    return (bot_is_2p_vs_ghost() && ghost_id == GHOST_HUMAN) ? 1 : 0;
}

uint32_t bot_is_human_ghost_active(void) {
    return bot_is_2p_vs_ghost() ? 1 : 0;
}

uint32_t bot_ghost_can_turn(uint32_t xp, uint32_t yp, uint32_t dir) {
    if (dir == MOVE_UP) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) && bot_is_walkable(xp, yp - 1, 1)) ? 1 : 0;
    }
    if (dir == MOVE_RIGHT) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) && bot_is_walkable(xp + 1, yp, 1)) ? 1 : 0;
    }
    if (dir == MOVE_DOWN) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) && bot_is_walkable(xp, yp + 1, 1)) ? 1 : 0;
    }
    if (dir == MOVE_LEFT) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) && bot_is_walkable(xp - 1, yp, 1)) ? 1 : 0;
    }
    return 0;
}

uint32_t bot_should_allow_ghost_move(void) {
    if (bot_is_2p_coop()) {
        return (Player.status == PLAYER_STATUS_ALIVE ||
                Player2.status == PLAYER_STATUS_ALIVE) ? 1 : 0;
    }
    if (bot_is_2p_vs_ghost()) {
        return (Player.lives > 0 || Player.status == PLAYER_STATUS_ALIVE ||
                Player.status == PLAYER_STATUS_DYING) ? 1 : 0;
    }
    return (Player.status == PLAYER_STATUS_ALIVE ||
            Player.status == PLAYER_STATUS_DYING) ? 1 : 0;
}

uint32_t bot_calc_move_player_ghost(uint32_t xp, uint32_t yp, uint32_t akt_dir, uint32_t joy) {
    if (joy == GUI_JOY_UP && (akt_dir != MOVE_DOWN) && bot_ghost_can_turn(xp, yp, MOVE_UP)) {
        return MOVE_UP;
    }
    if (joy == GUI_JOY_RIGHT && (akt_dir != MOVE_LEFT) && bot_ghost_can_turn(xp, yp, MOVE_RIGHT)) {
        return MOVE_RIGHT;
    }
    if (joy == GUI_JOY_DOWN && (akt_dir != MOVE_UP) && bot_ghost_can_turn(xp, yp, MOVE_DOWN)) {
        return MOVE_DOWN;
    }
    if (joy == GUI_JOY_LEFT && (akt_dir != MOVE_RIGHT) && bot_ghost_can_turn(xp, yp, MOVE_LEFT)) {
        return MOVE_LEFT;
    }

    if (akt_dir == MOVE_UP && bot_ghost_can_turn(xp, yp, MOVE_UP)) {
        return MOVE_UP;
    }
    if (akt_dir == MOVE_RIGHT && bot_ghost_can_turn(xp, yp, MOVE_RIGHT)) {
        return MOVE_RIGHT;
    }
    if (akt_dir == MOVE_DOWN && bot_ghost_can_turn(xp, yp, MOVE_DOWN)) {
        return MOVE_DOWN;
    }
    if (akt_dir == MOVE_LEFT && bot_ghost_can_turn(xp, yp, MOVE_LEFT)) {
        return MOVE_LEFT;
    }

    return MOVE_STOP;
}

void bot_apply_player_ghost_input(Ghost_t *ghost, uint32_t joy) {
    uint32_t xp;
    uint32_t yp;
    uint32_t aligned;

    if (ghost == 0 || ghost->status != GHOST_STATUS_ALIVE || joy == GUI_JOY_NONE) {
        return;
    }

    xp = ghost->xp;
    yp = ghost->yp;
    aligned = (ABS(ghost->delta_x) <= PLAYER_TURN_ALIGN) && (ABS(ghost->delta_y) <= PLAYER_TURN_ALIGN);

    if (ghost->move == MOVE_STOP && ghost->port == PORT_DONE &&
        ghost->delta_x == 0 && ghost->delta_y == 0) {
        if (joy == GUI_JOY_UP && bot_ghost_can_turn(xp, yp, MOVE_UP) != 0) {
            ghost->move = MOVE_UP;
            ghost->next_move = MOVE_UP;
            return;
        }
        if (joy == GUI_JOY_RIGHT && bot_ghost_can_turn(xp, yp, MOVE_RIGHT) != 0) {
            ghost->move = MOVE_RIGHT;
            ghost->next_move = MOVE_RIGHT;
            return;
        }
        if (joy == GUI_JOY_DOWN && bot_ghost_can_turn(xp, yp, MOVE_DOWN) != 0) {
            ghost->move = MOVE_DOWN;
            ghost->next_move = MOVE_DOWN;
            return;
        }
        if (joy == GUI_JOY_LEFT && bot_ghost_can_turn(xp, yp, MOVE_LEFT) != 0) {
            ghost->move = MOVE_LEFT;
            ghost->next_move = MOVE_LEFT;
            return;
        }
    }

    if ((ghost->move == MOVE_LEFT || ghost->move == MOVE_RIGHT) &&
        (ghost->port == PORT_DONE) && (ghost->delta_y == 0)) {
        if (ABS(ghost->delta_x) <= PLAYER_TURN_ALIGN) {
            if (joy == GUI_JOY_UP && (ghost->move != MOVE_DOWN)) {
                if (bot_ghost_can_turn(xp, yp, MOVE_UP) != 0) {
                    ghost->delta_x = 0;
                    ghost->move = MOVE_UP;
                    ghost->next_move = MOVE_UP;
                    return;
                }
            }
            if (joy == GUI_JOY_DOWN && (ghost->move != MOVE_UP)) {
                if (bot_ghost_can_turn(xp, yp, MOVE_DOWN) != 0) {
                    ghost->delta_x = 0;
                    ghost->move = MOVE_DOWN;
                    ghost->next_move = MOVE_DOWN;
                    return;
                }
            }
        }
        if (ghost->move == MOVE_RIGHT && ghost->delta_x >= (int32_t)(ROOM_WIDTH - PLAYER_TURN_ALIGN)) {
            if (joy == GUI_JOY_UP) {
                if (bot_ghost_can_turn(xp + 1, yp, MOVE_UP) != 0) {
                    ghost->xp = xp + 1;
                    ghost->delta_x = 0;
                    ghost->move = MOVE_UP;
                    ghost->next_move = MOVE_UP;
                    return;
                }
            }
            if (joy == GUI_JOY_DOWN) {
                if (bot_ghost_can_turn(xp + 1, yp, MOVE_DOWN) != 0) {
                    ghost->xp = xp + 1;
                    ghost->delta_x = 0;
                    ghost->move = MOVE_DOWN;
                    ghost->next_move = MOVE_DOWN;
                    return;
                }
            }
        }
        if (ghost->move == MOVE_LEFT && ghost->delta_x <= -(int32_t)(ROOM_WIDTH - PLAYER_TURN_ALIGN)) {
            if (joy == GUI_JOY_UP) {
                if (bot_ghost_can_turn(xp - 1, yp, MOVE_UP) != 0) {
                    ghost->xp = xp - 1;
                    ghost->delta_x = 0;
                    ghost->move = MOVE_UP;
                    ghost->next_move = MOVE_UP;
                    return;
                }
            }
            if (joy == GUI_JOY_DOWN) {
                if (bot_ghost_can_turn(xp - 1, yp, MOVE_DOWN) != 0) {
                    ghost->xp = xp - 1;
                    ghost->delta_x = 0;
                    ghost->move = MOVE_DOWN;
                    ghost->next_move = MOVE_DOWN;
                    return;
                }
            }
        }
    }

    if ((ghost->move == MOVE_UP || ghost->move == MOVE_DOWN) &&
        (ghost->port == PORT_DONE) && (ghost->delta_x == 0)) {
        if (ABS(ghost->delta_y) <= PLAYER_TURN_ALIGN) {
            if (joy == GUI_JOY_LEFT && (ghost->move != MOVE_RIGHT)) {
                if (bot_ghost_can_turn(xp, yp, MOVE_LEFT) != 0) {
                    ghost->delta_y = 0;
                    ghost->move = MOVE_LEFT;
                    ghost->next_move = MOVE_LEFT;
                    return;
                }
            }
            if (joy == GUI_JOY_RIGHT && (ghost->move != MOVE_LEFT)) {
                if (bot_ghost_can_turn(xp, yp, MOVE_RIGHT) != 0) {
                    ghost->delta_y = 0;
                    ghost->move = MOVE_RIGHT;
                    ghost->next_move = MOVE_RIGHT;
                    return;
                }
            }
        }
        if (ghost->move == MOVE_DOWN && ghost->delta_y >= (int32_t)(ROOM_HEIGHT - PLAYER_TURN_ALIGN)) {
            if (joy == GUI_JOY_LEFT) {
                if (bot_ghost_can_turn(xp, yp + 1, MOVE_LEFT) != 0) {
                    ghost->yp = yp + 1;
                    ghost->delta_y = 0;
                    ghost->move = MOVE_LEFT;
                    ghost->next_move = MOVE_LEFT;
                    return;
                }
            }
            if (joy == GUI_JOY_RIGHT) {
                if (bot_ghost_can_turn(xp, yp + 1, MOVE_RIGHT) != 0) {
                    ghost->yp = yp + 1;
                    ghost->delta_y = 0;
                    ghost->move = MOVE_RIGHT;
                    ghost->next_move = MOVE_RIGHT;
                    return;
                }
            }
        }
        if (ghost->move == MOVE_UP && ghost->delta_y <= -(int32_t)(ROOM_HEIGHT - PLAYER_TURN_ALIGN)) {
            if (joy == GUI_JOY_LEFT) {
                if (bot_ghost_can_turn(xp, yp - 1, MOVE_LEFT) != 0) {
                    ghost->yp = yp - 1;
                    ghost->delta_y = 0;
                    ghost->move = MOVE_LEFT;
                    ghost->next_move = MOVE_LEFT;
                    return;
                }
            }
            if (joy == GUI_JOY_RIGHT) {
                if (bot_ghost_can_turn(xp, yp - 1, MOVE_RIGHT) != 0) {
                    ghost->yp = yp - 1;
                    ghost->delta_y = 0;
                    ghost->move = MOVE_RIGHT;
                    ghost->next_move = MOVE_RIGHT;
                    return;
                }
            }
        }
    }

    if (joy == GUI_JOY_UP) {
        if (ghost->move == MOVE_UP) return;
        if (ghost->port != PORT_DONE) return;
        /* Reverse is free only after leaving cell center; at rest require open door
         * so spawn reverse into a wall cannot trap the human ghost. */
        if (ghost->move == MOVE_DOWN) {
            if (ghost->delta_y != 0 || bot_ghost_can_turn(xp, yp, MOVE_UP) != 0) {
                ghost->move = MOVE_UP;
                ghost->next_move = MOVE_UP;
            }
            return;
        }
        if (aligned != 0) {
            if (bot_ghost_can_turn(xp, yp, MOVE_UP) != 0) {
                ghost->delta_x = 0;
                ghost->delta_y = 0;
                ghost->move = MOVE_UP;
                ghost->next_move = MOVE_UP;
            }
        }
    } else if (joy == GUI_JOY_RIGHT) {
        if (ghost->move == MOVE_RIGHT) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_LEFT) {
            if (ghost->delta_x != 0 || bot_ghost_can_turn(xp, yp, MOVE_RIGHT) != 0) {
                ghost->move = MOVE_RIGHT;
                ghost->next_move = MOVE_RIGHT;
            }
            return;
        }
        if (aligned != 0) {
            if (bot_ghost_can_turn(xp, yp, MOVE_RIGHT) != 0) {
                ghost->delta_x = 0;
                ghost->delta_y = 0;
                ghost->move = MOVE_RIGHT;
                ghost->next_move = MOVE_RIGHT;
            }
        }
    } else if (joy == GUI_JOY_DOWN) {
        if (ghost->move == MOVE_DOWN) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_UP) {
            if (ghost->delta_y != 0 || bot_ghost_can_turn(xp, yp, MOVE_DOWN) != 0) {
                ghost->move = MOVE_DOWN;
                ghost->next_move = MOVE_DOWN;
            }
            return;
        }
        if (aligned != 0) {
            if (bot_ghost_can_turn(xp, yp, MOVE_DOWN) != 0) {
                ghost->delta_x = 0;
                ghost->delta_y = 0;
                ghost->move = MOVE_DOWN;
                ghost->next_move = MOVE_DOWN;
            }
        }
    } else if (joy == GUI_JOY_LEFT) {
        if (ghost->move == MOVE_LEFT) return;
        if (ghost->port != PORT_DONE) return;
        if (ghost->move == MOVE_RIGHT) {
            if (ghost->delta_x != 0 || bot_ghost_can_turn(xp, yp, MOVE_LEFT) != 0) {
                ghost->move = MOVE_LEFT;
                ghost->next_move = MOVE_LEFT;
            }
            return;
        }
        if (aligned != 0) {
            if (bot_ghost_can_turn(xp, yp, MOVE_LEFT) != 0) {
                ghost->delta_x = 0;
                ghost->delta_y = 0;
                ghost->move = MOVE_LEFT;
                ghost->next_move = MOVE_LEFT;
            }
        }
    }
}

void bot_get_nearest_player(uint32_t xp, uint32_t yp, uint32_t *txp, uint32_t *typ) {
    Player_t *target = bot_get_nearest_player_ptr(xp, yp);
    *txp = target->xp;
    *typ = target->yp;
}

Player_t* bot_get_nearest_player_ptr(uint32_t xp, uint32_t yp) {
    uint32_t p1_alive = (Player.status == PLAYER_STATUS_ALIVE) ? 1 : 0;
    uint32_t p2_alive = (Game.player2_active != 0 && Player2.status == PLAYER_STATUS_ALIVE) ? 1 : 0;

    if (p1_alive != 0 && p2_alive != 0) {
        uint32_t d1 = bot_calc_distance(xp, yp, Player.xp, Player.yp);
        uint32_t d2 = bot_calc_distance(xp, yp, Player2.xp, Player2.yp);
        if (d2 < d1) {
            return &Player2;
        }
        return &Player;
    }
    if (p2_alive != 0) {
        return &Player2;
    }
    if (p1_alive != 0) {
        return &Player;
    }
    return &Player;
}

void bot_find_safe_respawn(uint32_t start_x, uint32_t start_y, uint32_t *respawn_x, uint32_t *respawn_y) {
    uint32_t best_x = start_x;
    uint32_t best_y = start_y;
    int32_t max_min_dist = -1;
    uint32_t x, y;

    for (y = 0; y < ROOM_CNT_Y; y++) {
        for (x = 0; x < ROOM_CNT_X; x++) {
            if (Maze.Room[x][y].typ == ROOM_TYP_PATH && Maze.Room[x][y].special == ROOM_SPEC_NONE) {
                int32_t min_dist = 999999;
                
                {
                    uint32_t gi;
                    for (gi = 0; gi < GHOST_MAX; gi++) {
                        if (Ghosts[gi].status == GHOST_STATUS_ALIVE) {
                            int32_t dist = ABS((int32_t)x - (int32_t)Ghosts[gi].xp) +
                                           ABS((int32_t)y - (int32_t)Ghosts[gi].yp);
                            if (dist < min_dist) min_dist = dist;
                        }
                    }
                }
                if (bot_is_human_ghost_active() != 0 && HumanGhost.status == GHOST_STATUS_ALIVE) {
                    int32_t dist = ABS((int32_t)x - (int32_t)HumanGhost.xp) + ABS((int32_t)y - (int32_t)HumanGhost.yp);
                    if (dist < min_dist) min_dist = dist;
                }
                
                if (min_dist != 999999 && min_dist > max_min_dist) {
                    max_min_dist = min_dist;
                    best_x = x;
                    best_y = y;
                }
            }
        }
    }
    *respawn_x = best_x;
    *respawn_y = best_y;
}

void bot_kill_pacman(Player_t *p, uint32_t start_x, uint32_t start_y) {
    extern uint32_t Player_Dying_Timer_ms;
    extern uint32_t Player2_Dying_Timer_ms;
    extern void UB_Buzzer_Play_Die_NonBlocking(void);

    if (p->status != PLAYER_STATUS_ALIVE) {
        return;
    }

    if (Game.player2_active != 0) {
        // --- CHẾ ĐỘ 2 NGƯỜI CHƠI (NON-BLOCKING) ---
        if (p->lives > 0) {
            p->lives--;
        }

        p->status = PLAYER_STATUS_DYING;
        p->respawn_x = start_x;
        p->respawn_y = start_y;

        if (p == &Player) {
            Player_Dying_Timer_ms = 1000;
        } else {
            Player2_Dying_Timer_ms = 1000;
        }

        UB_Buzzer_Play_Die_NonBlocking();
    } else {
        // --- CHẾ ĐỘ 1 NGƯỜI CHƠI (BLOCKING NHƯ CŨ) ---
        // Play death sound (blocking tone sequence)
        UB_Buzzer_Play_Die();

        if (p->lives > 0) {
            p->lives--;
        }
        if (p->lives == 0) {
            p->status = PLAYER_STATUS_DEAD;
        } else {
            uint32_t rx = start_x;
            uint32_t ry = start_y;
            bot_find_safe_respawn(start_x, start_y, &rx, &ry);
            p->xp = rx;
            p->yp = ry;
            p->delta_x = 0;
            p->delta_y = 0;
            p->move = MOVE_STOP;
            p->skin = PLAYER_SKIN_LEFT1;
            p->skin_cnt = 0;
            p->port = PORT_DONE;
            p->status = PLAYER_STATUS_ALIVE;

            // Vẽ lại toàn bộ mê cung và đồng bộ 2 layer LCD để xóa hoàn toàn sprite cũ ở vị trí chết
            extern void gui_draw_maze(void);
            extern void gui_draw_bots(void);
            extern void UB_LCD_Copy_Layer2_to_Layer1(void);
            extern void UB_LCD_Refresh(void);
            gui_draw_maze();
            gui_draw_bots();
            UB_LCD_Copy_Layer2_to_Layer1();
            UB_LCD_Refresh();
        }
        bot_release_ghosts_on_pacman_death();
        GUI.refresh_value = GUI_REFRESH_VALUE;
    }
}

void bot_team_kill_pacman(void) {
    bot_kill_pacman(&Player, PLAYER_START_X, PLAYER_START_Y);
}

uint32_t bot_coop_is_game_over(void) {
    if (bot_is_2p_coop()) {
        return (Player.lives == 0 && Player2.lives == 0) ? 1 : 0;
    }
    if (bot_is_2p_vs_ghost()) {
        return (Player.lives == 0) ? 1 : 0;
    }
    return 0;
}

void bot_team_win_pacman(void) {
    Player.status = PLAYER_STATUS_WIN;
    if (bot_is_2p_coop() && Player2.status == PLAYER_STATUS_ALIVE) {
        Player2.status = PLAYER_STATUS_WIN;
    }
    GUI.refresh_value = GUI_REFRESH_VALUE;
}

void bot_ghost_eaten_by_pacman(Ghost_t *ghost, uint32_t ghost_id) {
    uint32_t pts;
    extern uint32_t HumanGhost_Eat_Invuln_Timer_ms;

    if (ghost == 0 || Game.frightened == BOOL_FALSE) {
        return;
    }
    /* Refuse if already eaten this cycle (e.g. revived in house/gate,
     * or human-ghost eat invulnerability still active). */
    if (bot_ghost_can_harm_pacman(ghost, ghost_id) == 0) {
        return;
    }

    pts = Game.frightened_points;
    bot_ghost_instant_revive(ghost, ghost_id);

    /* Player-controlled ghost revives immediately on path (spawn/same area)
     * while still ALIVE — without invuln, pixel collision re-eats every frame. */
    if (ghost_id == GHOST_HUMAN) {
        HumanGhost_Eat_Invuln_Timer_ms = HUMAN_GHOST_EAT_INVULN_MS;
    }

    /* Chain in one frightened phase: 300 → 600 → 900 → ... */
    if (Player.score > (0xFFFFFFFFu - pts)) {
        Player.score = 0xFFFFFFFFu;
    } else {
        Player.score += pts;
    }
    if (Game.frightened_points <= (0xFFFFFFFFu - GAME_FRIGHTENED_STEP_POINTS)) {
        Game.frightened_points += GAME_FRIGHTENED_STEP_POINTS;
    }

    GUI.refresh_value = GUI_REFRESH_VALUE;
}

void bot_ghost_hit_pacman(uint32_t gxp, uint32_t gyp, Ghost_t *ghost) {
    uint32_t ghost_id = ghost_id_from_ptr(ghost);

    if (Game.collision == BOOL_FALSE || bot_ghost_can_harm_pacman(ghost, ghost_id) == 0) {
        return;
    }

    if (Player.status == PLAYER_STATUS_ALIVE && Player.xp == gxp && Player.yp == gyp) {
        if (Game.frightened == BOOL_FALSE) {
            bot_kill_pacman(&Player, PLAYER_START_X, PLAYER_START_Y);
            GUI.refresh_value = GUI_REFRESH_VALUE;
        } else {
            bot_ghost_eaten_by_pacman(ghost, ghost_id);
        }
        return;
    }

    if (bot_is_2p_coop() && Player2.status == PLAYER_STATUS_ALIVE && Player2.xp == gxp && Player2.yp == gyp) {
        if (Game.frightened == BOOL_FALSE) {
            bot_kill_pacman(&Player2, PLAYER2_START_X, PLAYER2_START_Y);
            GUI.refresh_value = GUI_REFRESH_VALUE;
        } else {
            bot_ghost_eaten_by_pacman(ghost, ghost_id);
        }
    }
}

uint32_t bot_ghost_get_body_color(uint32_t ghost_id, const Ghost_t *ghost, uint16_t *color) {
    if (color == 0 || ghost == 0) {
        return 0;
    }

    if (ghost_id == GHOST_HUMAN) {
        *color = HUMAN_GHOST_COLOR;
        return 1;
    }

    if (bot_is_player_controlled_ghost(ghost_id) != 0) {
        *color = GHOST_P2_COLOR;
        return 1;
    }

    if (ghost->strategy == GHOST_TYPE_CHASE) {
        *color = GHOST_COLOR_CHASE;
    } else if (ghost->strategy == GHOST_TYPE_AMBUSH) {
        *color = GHOST_COLOR_AMBUSH;
    } else if (ghost->strategy == GHOST_TYPE_TRICKY) {
        *color = GHOST_COLOR_TRICKY;
    } else if (ghost->strategy == GHOST_TYPE_SHY) {
        *color = GHOST_COLOR_SHY;
    } else if (ghost->strategy == GHOST_TYPE_DRUNK) {
        *color = GHOST_COLOR_DRUNK;
    } else if (ghost->strategy == GHOST_TYPE_LAZY) {
        *color = GHOST_COLOR_LAZY;
    } else {
        *color = GHOST_COLOR_RANDOM;
    }
    return 1;
}

uint32_t bot_player_can_turn(uint32_t xp, uint32_t yp, uint32_t dir) {
    if (dir == MOVE_UP) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) && bot_is_walkable(xp, yp - 1, 0)) ? 1 : 0;
    }
    if (dir == MOVE_RIGHT) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) && bot_is_walkable(xp + 1, yp, 0)) ? 1 : 0;
    }
    if (dir == MOVE_DOWN) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) && bot_is_walkable(xp, yp + 1, 0)) ? 1 : 0;
    }
    if (dir == MOVE_LEFT) {
        return (((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) && bot_is_walkable(xp - 1, yp, 0)) ? 1 : 0;
    }
    return 0;
}

void bot_ghost_validate_position(Ghost_t *ghost) {
    if (!bot_is_walkable(ghost->xp, ghost->yp, 1)) {
        /* Human ghost can reverse into a wall at spawn; snap back so input works again. */
        if (ghost == &HumanGhost && bot_is_human_ghost_active() != 0) {
            if (bot_is_walkable(HumanGhost_Spawn_X, HumanGhost_Spawn_Y, 1)) {
                ghost->xp = HumanGhost_Spawn_X;
                ghost->yp = HumanGhost_Spawn_Y;
            }
        }
        ghost->move = MOVE_STOP;
        ghost->next_move = MOVE_STOP;
        ghost->delta_x = 0;
        ghost->delta_y = 0;
    }
}

void bot_ghost_unstick(Ghost_t *ghost, uint32_t ghost_id) {
    if (ghost->move != MOVE_STOP) {
        return;
    }
    if (ghost->status == GHOST_STATUS_ALIVE) {
        if (ghost == &HumanGhost && bot_is_human_ghost_active() != 0) {
            uint32_t dir;

            /* If still on a non-path cell, snap to spawn first. */
            if (!bot_is_walkable(ghost->xp, ghost->yp, 1)) {
                if (bot_is_walkable(HumanGhost_Spawn_X, HumanGhost_Spawn_Y, 1)) {
                    ghost->xp = HumanGhost_Spawn_X;
                    ghost->yp = HumanGhost_Spawn_Y;
                }
                ghost->delta_x = 0;
                ghost->delta_y = 0;
            }

            /* Honor current stick direction if open; do not AI-drive the human ghost. */
            dir = bot_calc_move_player_ghost(ghost->xp, ghost->yp, MOVE_STOP, Game.player2_joy);
            if (dir != MOVE_STOP) {
                ghost->move = dir;
                ghost->next_move = dir;
            }
            return;
        }
        ghost->next_move = bot_calc_move_by_strategy(ghost_id, ghost->strategy, ghost->xp, ghost->yp, MOVE_STOP);
        if (ghost->next_move == MOVE_STOP) {
            ghost->next_move = bot_calc_only_exit(ghost->xp, ghost->yp);
        }
        if (ghost->next_move == MOVE_STOP) {
            ghost->next_move = bot_calc_move_random(ghost->xp, ghost->yp, MOVE_STOP);
        }
        ghost->move = ghost->next_move;
        return;
    }
    if (ghost->status == GHOST_STATUS_DEAD) {
        bot_ghost_try_revive(ghost, ghost_id);
        if (ghost->status == GHOST_STATUS_ALIVE) {
            return;
        }
        ghost->next_move = bot_calc_move_dead(ghost_id, ghost->xp, ghost->yp, MOVE_STOP);
        ghost->move = ghost->next_move;
        bot_ghost_try_revive(ghost, ghost_id);
    }
}

void bot_ghost_try_revive(Ghost_t *ghost, uint32_t ghost_id) {
    uint32_t hx;
    uint32_t hy;

    if (ghost->status != GHOST_STATUS_DEAD) {
        return;
    }

    if (Game.play_type == GAME_PLAY_CUSTOM && ghost_id == GHOST_HUMAN) {
        hx = 14;
        hy = 14;
        if (ghost->xp != hx || ghost->yp != hy) {
            return;
        }

        ghost->status = GHOST_STATUS_ALIVE;
        ghost->delta_x = 0;
        ghost->delta_y = 0;
        ghost->skin = GHOST_SKIN_UP1;
        ghost->move = MOVE_UP;
        ghost->next_move = MOVE_UP;
        ghost->dot_cnt = HUMAN_GHOST_DOT_CNT_MAX;
        return;
    }

    if (ghost_id >= GHOST_MAX) {
        return;
    }

    ghosts_get_home(ghost_id, &hx, &hy);
    if (ghost->xp != hx || ghost->yp != hy) {
        return;
    }

    ghost->status = GHOST_STATUS_ALIVE;
    ghost->dot_cnt = 0;
    if (ghost_id == GHOST_0) {
        ghost->skin = GHOST_SKIN_UP1;
        ghost->delta_x = 0;
        ghost->delta_y = 0;
        ghost->move = MOVE_UP;
        ghost->next_move = MOVE_UP;
    } else if (ghost_id == GHOST_1) {
        ghost->skin = GHOST_SKIN_UP1;
        ghost->delta_x = GHOST_HOME_X_DIFF;
        ghost->delta_y = GHOST_HOME_Y_DIFF;
        ghost->move = MOVE_UP;
        ghost->next_move = MOVE_UP;
    } else if (ghost_id == GHOST_2) {
        ghost->skin = GHOST_SKIN_RIGHT1;
        ghost->delta_x = GHOST_HOME_X_DIFF;
        ghost->delta_y = GHOST_HOME_Y_DIFF;
        ghost->move = MOVE_RIGHT;
        ghost->next_move = MOVE_RIGHT;
    } else {
        ghost->skin = GHOST_SKIN_LEFT1;
        ghost->delta_x = GHOST_HOME_X_DIFF;
        ghost->delta_y = GHOST_HOME_Y_DIFF;
        ghost->move = MOVE_LEFT;
        ghost->next_move = MOVE_LEFT;
    }
}

void bot_ghost_instant_revive(Ghost_t *ghost, uint32_t ghost_id) {
    uint32_t hx;
    uint32_t hy;

    if (Game.play_type == GAME_PLAY_CUSTOM && ghost_id == GHOST_HUMAN) {
        uint32_t sx = HumanGhost_Spawn_X;
        uint32_t sy = HumanGhost_Spawn_Y;
        uint32_t init_dir;

        ghost->xp = sx;
        ghost->yp = sy;
        ghost->status = GHOST_STATUS_ALIVE;
        ghost->delta_x = 0;
        ghost->delta_y = 0;

        init_dir = bot_calc_only_exit(sx, sy);
        if (init_dir == MOVE_STOP) {
            init_dir = bot_calc_move_random(sx, sy, MOVE_STOP);
        }
        if (init_dir == MOVE_STOP) {
            init_dir = MOVE_LEFT;
        }

        ghost->skin = bot_ghost_skin_for_dir(init_dir);
        ghost->move = init_dir;
        ghost->next_move = init_dir;
        ghost->dot_cnt = HUMAN_GHOST_DOT_CNT_MAX;
        ghost->port = PORT_DONE;
        return;
    }

    if (ghost_id >= GHOST_MAX) {
        return;
    }

    ghosts_get_home(ghost_id, &hx, &hy);
    ghost->xp = hx;
    ghost->yp = hy;
    ghost->status = GHOST_STATUS_ALIVE;
    ghost->dot_cnt = ghost_dot_cnt_max(ghost_id);
    ghost->port = PORT_DONE;

    if (ghost_id == GHOST_0) {
        ghost->skin = GHOST_SKIN_UP1;
        ghost->delta_x = 0;
        ghost->delta_y = 0;
        ghost->move = MOVE_UP;
        ghost->next_move = MOVE_UP;
    } else if (ghost_id == GHOST_1) {
        ghost->skin = GHOST_SKIN_UP1;
        ghost->delta_x = GHOST_HOME_X_DIFF;
        ghost->delta_y = GHOST_HOME_Y_DIFF;
        ghost->move = MOVE_UP;
        ghost->next_move = MOVE_UP;
    } else if (ghost_id == GHOST_2) {
        ghost->skin = GHOST_SKIN_RIGHT1;
        ghost->delta_x = GHOST_HOME_X_DIFF;
        ghost->delta_y = GHOST_HOME_Y_DIFF;
        ghost->move = MOVE_RIGHT;
        ghost->next_move = MOVE_RIGHT;
    } else {
        ghost->skin = GHOST_SKIN_LEFT1;
        ghost->delta_x = GHOST_HOME_X_DIFF;
        ghost->delta_y = GHOST_HOME_Y_DIFF;
        ghost->move = MOVE_LEFT;
        ghost->next_move = MOVE_LEFT;
    }
}

void bot_release_ghosts_on_pacman_death(void) {
    uint32_t i;

    for (i = 0; i < GHOST_MAX; i++) {
        if ((Game.ghost_active_mask & ghost_move_mask(i)) != 0 &&
            Ghosts[i].status == GHOST_STATUS_ALIVE) {
            Ghosts[i].new_mode = 0;
            Ghosts[i].dot_cnt = ghost_dot_cnt_max(i);
            bot_ghost_unstick(&Ghosts[i], i);
        }
    }
    if (bot_is_human_ghost_active() != 0 && HumanGhost.status == GHOST_STATUS_ALIVE) {
        HumanGhost.new_mode = 0;
        HumanGhost.dot_cnt = HUMAN_GHOST_DOT_CNT_MAX;
        bot_ghost_unstick(&HumanGhost, GHOST_HUMAN);
    }
}

uint32_t bot_ghost_can_harm_pacman(Ghost_t *ghost, uint32_t ghost_id) {
    Room_t *room;
    extern uint32_t HumanGhost_Eat_Invuln_Timer_ms;

    if (ghost == 0 || ghost->status != GHOST_STATUS_ALIVE) {
        return 0;
    }
    /* Human-controlled ghost: brief post-eat invuln so instant revive
     * cannot be scored repeatedly while still overlapping Pacman. */
    if (ghost_id == GHOST_HUMAN && HumanGhost_Eat_Invuln_Timer_ms != 0) {
        return 0;
    }
    if (ghost->dot_cnt < bot_ghost_dot_cnt_max(ghost_id)) {
        return 0;
    }
    if (ghost->xp >= ROOM_CNT_X || ghost->yp >= ROOM_CNT_Y) {
        return 0;
    }
    room = &Maze.Room[ghost->xp][ghost->yp];
    if (room->special == ROOM_SPEC_GATE) {
        return 0;
    }
    if (ghost->yp >= 12 && ghost->yp <= 15 && ghost->xp >= 11 && ghost->xp <= 16 &&
        room->typ == ROOM_TYP_WALL && (room->door & 0x0F) != 0) {
        return 0;
    }
    return 1;
}

uint32_t bot_is_walkable(uint32_t x, uint32_t y, uint32_t for_ghost) {
    Room_t *room;

    if (x >= ROOM_CNT_X || y >= ROOM_CNT_Y) {
        return 0;
    }

    if (bot_avoid_portal != 0 && Maze.Room[x][y].special == ROOM_SPEC_PORTAL) {
        return 0;
    }

    if (bot_avoid_portal != 0 && Maze_selected_map == MAZE_MAP_CLASSIC) {
        if ((x == 12 || x == 15) && (y >= 16) && (y <= 19)) {
            return 0;
        }
    }

    room = &Maze.Room[x][y];
    if (room->typ == ROOM_TYP_PATH) {
        if (for_ghost == 0 && room->special == ROOM_SPEC_GATE &&
            (x != GHOST_HOUSE_EXIT_X || y != GHOST_HOUSE_EXIT_Y)) {
            return 0;
        }
        return 1;
    }
    if (room->special == ROOM_SPEC_PORTAL) {
        return 1;
    }
    if (for_ghost != 0) {
        if (room->special == ROOM_SPEC_GATE) {
            return 1;
        }
        if ((room->typ == ROOM_TYP_WALL) && ((room->door & 0x0F) != 0)) {
            return 1;
        }
    }
    return 0;
}


//--------------------------------------------------------------
// search the room around the actual position
// with the shortest distance to a target position
// but dont go back
// x1,y1 = actual position
// x2,y2 = target position
//--------------------------------------------------------------
uint32_t bot_calc_move(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t akt_dir) {
    uint32_t ret_wert = MOVE_STOP;
    uint32_t xn, yn;
    uint32_t d_up = INIT_DISTANCE, d_right = INIT_DISTANCE, d_down = INIT_DISTANCE, d_left = INIT_DISTANCE;
    uint32_t d_min;

    // calc for all possible directions the distance
    // but dont go back
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_U) != 0) && (akt_dir != MOVE_DOWN) && bot_is_walkable(x1, y1 - 1, 1)) {
        xn = x1;
        yn = y1 - 1;
        d_up = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_R) != 0) && (akt_dir != MOVE_LEFT) && bot_is_walkable(x1 + 1, y1, 1)) {
        xn = x1 + 1;
        yn = y1;
        d_right = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_D) != 0) && (akt_dir != MOVE_UP) && bot_is_walkable(x1, y1 + 1, 1)) {
        xn = x1;
        yn = y1 + 1;
        d_down = bot_calc_distance(x2, y2, xn, yn);
    }
    if (((Maze.Room[x1][y1].door & ROOM_DOOR_L) != 0) && (akt_dir != MOVE_RIGHT) && bot_is_walkable(x1 - 1, y1, 1)) {
        xn = x1 - 1;
        yn = y1;
        d_left = bot_calc_distance(x2, y2, xn, yn);
    }

    // search the shortest distance
    // in this order (up,left,down,right)
    d_min = INIT_DISTANCE;
    if (d_up < d_min) {
        d_min = d_up;
        ret_wert = MOVE_UP;
    }
    if (d_left < d_min) {
        d_min = d_left;
        ret_wert = MOVE_LEFT;
    }
    if (d_down < d_min) {
        d_min = d_down;
        ret_wert = MOVE_DOWN;
    }
    if (d_right < d_min) {
        d_min = d_right;
        ret_wert = MOVE_RIGHT;
    }

    // Fallback: if no valid direction is found (e.g. dead end or corner case)
    // select any available door (even if it is backwards)
    if (ret_wert == MOVE_STOP) {
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_U) != 0) && bot_is_walkable(x1, y1 - 1, 1)) {
            d_up = bot_calc_distance(x2, y2, x1, y1 - 1);
        }
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_R) != 0) && bot_is_walkable(x1 + 1, y1, 1)) {
            d_right = bot_calc_distance(x2, y2, x1 + 1, y1);
        }
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_D) != 0) && bot_is_walkable(x1, y1 + 1, 1)) {
            d_down = bot_calc_distance(x2, y2, x1, y1 + 1);
        }
        if (((Maze.Room[x1][y1].door & ROOM_DOOR_L) != 0) && bot_is_walkable(x1 - 1, y1, 1)) {
            d_left = bot_calc_distance(x2, y2, x1 - 1, y1);
        }

        d_min = INIT_DISTANCE;
        if (d_up < d_min) {
            d_min = d_up;
            ret_wert = MOVE_UP;
        }
        if (d_left < d_min) {
            d_min = d_left;
            ret_wert = MOVE_LEFT;
        }
        if (d_down < d_min) {
            d_min = d_down;
            ret_wert = MOVE_DOWN;
        }
        if (d_right < d_min) {
            d_min = d_right;
            ret_wert = MOVE_RIGHT;
        }
    }

    return (ret_wert);
}

//--------------------------------------------------------------
// pick the only walkable exit at a cell (corridor / dead-end)
//--------------------------------------------------------------
uint32_t bot_calc_only_exit(uint32_t xp, uint32_t yp) {
    if (((Maze.Room[xp][yp].door & ROOM_DOOR_U) != 0) && bot_is_walkable(xp, yp - 1, 1)) {
        return MOVE_UP;
    }
    if (((Maze.Room[xp][yp].door & ROOM_DOOR_R) != 0) && bot_is_walkable(xp + 1, yp, 1)) {
        return MOVE_RIGHT;
    }
    if (((Maze.Room[xp][yp].door & ROOM_DOOR_D) != 0) && bot_is_walkable(xp, yp + 1, 1)) {
        return MOVE_DOWN;
    }
    if (((Maze.Room[xp][yp].door & ROOM_DOOR_L) != 0) && bot_is_walkable(xp - 1, yp, 1)) {
        return MOVE_LEFT;
    }
    return MOVE_STOP;
}

//--------------------------------------------------------------
// calc the distane between two points
// c = SQRT(a*a + b*b)
//--------------------------------------------------------------
uint32_t bot_calc_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    int32_t dx = (int32_t)x1 - (int32_t)x2;
    int32_t dy = (int32_t)y1 - (int32_t)y2;
    return (uint32_t)(dx * dx + dy * dy);
}

//--------------------------------------------------------------
// integer SQRT
//--------------------------------------------------------------
uint32_t UB_SQRT(uint32_t wert) {
    uint32_t ret_wert = 0;
    uint32_t square = 1;
    uint32_t delta = 3;

    while (square <= wert) {
        square += delta;
        delta += 2;
    }

    ret_wert = (delta / 2) - 1;

    return (ret_wert);
}

//--------------------------------------------------------------
// find random path cell far enough from pacman start
//--------------------------------------------------------------
static uint32_t bot_find_spawn_pos(uint32_t *xp, uint32_t *yp) {
    uint32_t tries = 0;
    uint32_t px = Player.xp;
    uint32_t py = Player.yp;

    while (tries < 200) {
        uint32_t cx = (uint32_t)(rand() % (ROOM_CNT_X - 2)) + 1;
        uint32_t cy = (uint32_t)(rand() % (ROOM_CNT_Y - 2)) + 1;

        if (Maze.Room[cx][cy].typ == ROOM_TYP_PATH) {
            uint32_t dist = bot_calc_distance(px, py, cx, cy);
            if (bot_is_2p_coop() && Player2.status == PLAYER_STATUS_ALIVE) {
                uint32_t dist2 = bot_calc_distance(Player2.xp, Player2.yp, cx, cy);
                if (dist2 < dist) {
                    dist = dist2;
                }
            }
            if (dist >= GHOST_SPAWN_MIN_DIST) {
                *xp = cx;
                *yp = cy;
                return 1;
            }
        }
        tries++;
    }
    return 0;
}

//--------------------------------------------------------------
// find spawn cell for custom mode (unique, walkable, far from player)
//--------------------------------------------------------------
static uint32_t bot_find_custom_spawn(uint32_t *xp, uint32_t *yp,
    const uint32_t *used_x, const uint32_t *used_y, uint32_t used_cnt) {
    uint32_t x, y, u;
    uint32_t found = 0;
    uint32_t best_dist = 0;
    uint32_t bx = 0;
    uint32_t by = 0;

    for (y = 1; y < ROOM_CNT_Y - 1; y++) {
        for (x = 1; x < ROOM_CNT_X - 1; x++) {
            uint32_t collision = 0;

            if (Maze.Room[x][y].typ != ROOM_TYP_PATH) {
                continue;
            }
            for (u = 0; u < used_cnt; u++) {
                if (used_x[u] == x && used_y[u] == y) {
                    collision = 1;
                    break;
                }
            }
            if (collision != 0) {
                continue;
            }

            {
                uint32_t dist = bot_calc_distance(Player.xp, Player.yp, x, y);
                if (bot_is_2p_coop() && Player2.status == PLAYER_STATUS_ALIVE) {
                    uint32_t dist2 = bot_calc_distance(Player2.xp, Player2.yp, x, y);
                    if (dist2 < dist) {
                        dist = dist2;
                    }
                }
                if (dist < GHOST_SPAWN_MIN_DIST) {
                    continue;
                }
                if (found == 0 || dist > best_dist) {
                    best_dist = dist;
                    bx = x;
                    by = y;
                    found = 1;
                }
            }
        }
    }

    if (found != 0) {
        *xp = bx;
        *yp = by;
    }
    return found;
}

static uint32_t bot_ghost_skin_for_dir(uint32_t dir) {
    if (dir == MOVE_UP) return GHOST_SKIN_UP1;
    if (dir == MOVE_RIGHT) return GHOST_SKIN_RIGHT1;
    if (dir == MOVE_DOWN) return GHOST_SKIN_DOWN1;
    if (dir == MOVE_LEFT) return GHOST_SKIN_LEFT1;
    return GHOST_SKIN_LEFT1;
}

static uint32_t bot_ghost_dot_cnt_max(uint32_t ghost_id) {
    return ghost_dot_cnt_max(ghost_id);
}

void bot_init_human_ghost(uint32_t speed_ms, const uint32_t *used_x, const uint32_t *used_y, uint32_t used_cnt) {
    uint32_t sx = 14;
    uint32_t sy = 11;
    uint32_t init_dir;
    extern uint32_t HumanGhost_Eat_Invuln_Timer_ms;

    HumanGhost_Eat_Invuln_Timer_ms = 0;
    HumanGhost.strategy = GHOST_STRATEGY_RANDOM;
    HumanGhost.akt_speed_ms = speed_ms;
    HumanGhost.status = GHOST_STATUS_DEAD;
    HumanGhost.move = MOVE_STOP;
    HumanGhost.next_move = MOVE_STOP;
    HumanGhost.delta_x = 0;
    HumanGhost.delta_y = 0;
    HumanGhost.skin_cnt = 0;
    HumanGhost.port = PORT_DONE;
    HumanGhost.dot_cnt = HUMAN_GHOST_DOT_CNT_MAX;
    HumanGhost.new_mode = 0;
    HumanGhost.frightened_buf = HUMAN_GHOST_FRIGHTENED_BUF;

    if (bot_find_custom_spawn(&sx, &sy, used_x, used_y, used_cnt) == 0) {
        if (!bot_is_walkable(sx, sy, 1)) {
            bot_find_spawn_pos(&sx, &sy);
        }
    }

    HumanGhost_Spawn_X = sx;
    HumanGhost_Spawn_Y = sy;
    HumanGhost.status = GHOST_STATUS_ALIVE;
    HumanGhost.xp = sx;
    HumanGhost.yp = sy;

    init_dir = bot_calc_only_exit(sx, sy);
    if (init_dir == MOVE_STOP) {
        init_dir = bot_calc_move_random(sx, sy, MOVE_STOP);
    }
    /* Never force MOVE_LEFT into a wall — wait for player input instead. */
    if (init_dir != MOVE_STOP && bot_ghost_can_turn(sx, sy, init_dir) == 0) {
        init_dir = MOVE_STOP;
    }

    HumanGhost.skin = bot_ghost_skin_for_dir(init_dir);
    HumanGhost.move = init_dir;
    HumanGhost.next_move = init_dir;
    bot_ghost_validate_position(&HumanGhost);
}

//--------------------------------------------------------------
// apply custom ghost setup: count, personality, speed, spawn
//--------------------------------------------------------------
void bot_apply_custom_ghosts(uint32_t ghost_count, uint32_t strategies[4], uint32_t speed_ms) {
    uint32_t i;
    uint32_t used_x[4];
    uint32_t used_y[4];
    uint32_t used_cnt = 0;
    uint32_t ai_count = ghost_count;

    if (bot_is_2p_vs_ghost()) {
        ai_count = bot_custom_ai_ghost_count();
    }

    for (i = 0; i < GHOST_MAX; i++) {
        Ghosts[i].strategy = strategies[i];
        Ghosts[i].akt_speed_ms = speed_ms;
        Ghosts[i].status = GHOST_STATUS_DEAD;
        Ghosts[i].move = MOVE_STOP;
        Ghosts[i].next_move = MOVE_STOP;
        Ghosts[i].delta_x = 0;
        Ghosts[i].delta_y = 0;
        Ghosts[i].skin_cnt = 0;
        Ghosts[i].port = PORT_DONE;
        Ghosts[i].dot_cnt = 0;
        Ghosts[i].new_mode = 0;
        Ghosts[i].frightened_buf = GHOST_FRIGHTENED_BUF_DEFAULT;
    }

    for (i = 0; i < ai_count; i++) {
        uint32_t sx = 14;
        uint32_t sy = 11;
        uint32_t init_dir;
        uint32_t init_skin;

        if (bot_find_custom_spawn(&sx, &sy, used_x, used_y, used_cnt) == 0) {
            if (i == 0) { sx = 14; sy = 11; }
            else if (i == 1) { sx = 14; sy = 14; }
            else if (i == 2) { sx = 13; sy = 14; }
            else { sx = 15; sy = 14; }
            if (!bot_is_walkable(sx, sy, 1)) {
                bot_find_spawn_pos(&sx, &sy);
            }
        }

        used_x[used_cnt] = sx;
        used_y[used_cnt] = sy;
        used_cnt++;

        Ghost_Spawn_X[i] = sx;
        Ghost_Spawn_Y[i] = sy;

        Ghosts[i].status = GHOST_STATUS_ALIVE;
        Ghosts[i].xp = sx;
        Ghosts[i].yp = sy;
        Ghosts[i].delta_x = 0;
        Ghosts[i].delta_y = 0;
        Ghosts[i].port = PORT_DONE;

        init_dir = bot_calc_move_by_strategy(i, Ghosts[i].strategy, sx, sy, MOVE_STOP);
        if (init_dir == MOVE_STOP) {
            init_dir = bot_calc_only_exit(sx, sy);
        }
        if (init_dir == MOVE_STOP) {
            init_dir = bot_calc_move_random(sx, sy, MOVE_STOP);
        }
        init_skin = bot_ghost_skin_for_dir(init_dir);

        Ghosts[i].skin = init_skin;
        Ghosts[i].move = init_dir;
        Ghosts[i].next_move = init_dir;
        Ghosts[i].dot_cnt = bot_ghost_dot_cnt_max(i);
        Ghosts[i].new_mode = 0;

        bot_ghost_validate_position(&Ghosts[i]);
        if (Ghosts[i].move == MOVE_STOP) {
            init_dir = bot_calc_move_random(Ghosts[i].xp, Ghosts[i].yp, MOVE_STOP);
            if (init_dir != MOVE_STOP) {
                Ghosts[i].move = init_dir;
                Ghosts[i].next_move = init_dir;
                Ghosts[i].skin = bot_ghost_skin_for_dir(init_dir);
            }
        }
    }

    if (bot_is_2p_vs_ghost() != 0) {
        bot_init_human_ghost(speed_ms, used_x, used_y, used_cnt);
    } else {
        HumanGhost.status = GHOST_STATUS_DEAD;
        HumanGhost.move = MOVE_STOP;
        HumanGhost.next_move = MOVE_STOP;
    }
}
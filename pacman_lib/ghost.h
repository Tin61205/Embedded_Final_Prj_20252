//--------------------------------------------------------------
// Unified AI ghost slots (data-driven personalities)
//--------------------------------------------------------------
#ifndef __STM32F4_UB_GHOST_H
#define __STM32F4_UB_GHOST_H

#include "stm32f4xx.h"
#include "bot.h"
#include "skin.h"

//--------------------------------------------------------------
// Per-slot campaign spawn / home / scatter (maze cell coords)
//--------------------------------------------------------------
#define GHOST_START_X0  14
#define GHOST_START_Y0  11
#define GHOST_START_X1  14
#define GHOST_START_Y1  14
#define GHOST_START_X2  12
#define GHOST_START_Y2  14
#define GHOST_START_X3  16
#define GHOST_START_Y3  14

#define GHOST_HOME_X0   14
#define GHOST_HOME_Y0   14
#define GHOST_HOME_X1   14
#define GHOST_HOME_Y1   14
#define GHOST_HOME_X2   12
#define GHOST_HOME_Y2   14
#define GHOST_HOME_X3   16
#define GHOST_HOME_Y3   14

#define GHOST_SCATTER_X0  (ROOM_CNT_X - 1)
#define GHOST_SCATTER_Y0  0
#define GHOST_SCATTER_X1  0
#define GHOST_SCATTER_Y1  0
#define GHOST_SCATTER_X2  (ROOM_CNT_X - 1)
#define GHOST_SCATTER_Y2  (ROOM_CNT_Y - 1)
#define GHOST_SCATTER_X3  0
#define GHOST_SCATTER_Y3  (ROOM_CNT_Y - 1)

#define GHOST_FRIGHTENED_BUF_DEFAULT  30
#define GHOST_DOT_CNT_MAX_DEFAULT     0

//--------------------------------------------------------------
// API
//--------------------------------------------------------------
void ghosts_init(uint32_t mode);
void ghost_move(uint32_t id);
void ghosts_get_home(uint32_t id, uint32_t *hx, uint32_t *hy);
void ghosts_get_scatter(uint32_t id, uint32_t *sx, uint32_t *sy);
uint32_t ghost_dot_cnt_max(uint32_t id);
uint32_t ghost_id_from_ptr(const Ghost_t *ghost);
uint32_t ghost_move_mask(uint32_t id);

#endif /* __STM32F4_UB_GHOST_H */

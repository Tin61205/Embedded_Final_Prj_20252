//--------------------------------------------------------------
// File     : humanghost.h
//--------------------------------------------------------------
#ifndef __STM32F4_UB_HUMANGHOST_H
#define __STM32F4_UB_HUMANGHOST_H

#include "stm32f4xx.h"
#include "bot.h"
#include "skin.h"
#include "gui.h"

#define HUMAN_GHOST_DOT_CNT_MAX 0
#define HUMAN_GHOST_FRIGHTENED_BUF 30
/* After Pacman eats the human-controlled ghost it revives immediately
 * (spawn / same area). Without a short invuln window, pixel collision
 * re-triggers every frame and the score explodes. */
#define HUMAN_GHOST_EAT_INVULN_MS 1500

void humanghost_move(void);

#endif // __STM32F4_UB_HUMANGHOST_H

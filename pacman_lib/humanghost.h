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

void humanghost_move(void);

#endif // __STM32F4_UB_HUMANGHOST_H

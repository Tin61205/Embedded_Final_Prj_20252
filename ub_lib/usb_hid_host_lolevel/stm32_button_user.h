#ifndef __STM32_BUTTON_USER_H
#define __STM32_BUTTON_USER_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include <stdint.h>

#define PRESSED_BUTTON_NONE 0x00
#define PRESSED_BUTTON_USER 0x01

uint8_t Read_User_Button(void);

#endif // __STM32_BUTTON_USER_H

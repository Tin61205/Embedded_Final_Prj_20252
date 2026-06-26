//--------------------------------------------------------------
// File     : stm32_ub_buzzer.h
//--------------------------------------------------------------
#ifndef __STM32F4_UB_BUZZER_H
#define __STM32F4_UB_BUZZER_H

#include "stm32f4xx.h"

// Passive Buzzer hardware configuration
// PC9 corresponds to TIM3_CH4
#define BUZZER_GPIO_PORT       GPIOC
#define BUZZER_GPIO_PIN        GPIO_Pin_9
#define BUZZER_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define BUZZER_GPIO_PINSOURCE  GPIO_PinSource9
#define BUZZER_GPIO_AF         GPIO_AF_TIM3

void UB_Buzzer_Init(void);
void UB_Buzzer_SetTone(uint32_t freq);
void UB_Buzzer_On(uint32_t freq);
void UB_Buzzer_Off(void);
void UB_Buzzer_PlayTone(uint32_t freq, uint32_t duration_ms);
void UB_Buzzer_PlayToneNonBlocking(uint32_t freq, uint32_t duration_ms);

// Game sound effects
void UB_Buzzer_Play_MenuClick(void);
void UB_Buzzer_Play_EatDot(void);
void UB_Buzzer_Play_EatEnergizer(void);
void UB_Buzzer_Play_Die(void);
void UB_Buzzer_Play_Win(void);
void UB_Buzzer_Play_Lost(void);

extern volatile uint32_t buzzer_sequence_step;
extern volatile uint32_t buzzer_sequence_timer;
void UB_Buzzer_Play_Die_NonBlocking(void);

#endif // __STM32F4_UB_BUZZER_H

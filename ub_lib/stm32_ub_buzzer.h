//--------------------------------------------------------------
// File     : stm32_ub_buzzer.h
// Buzzer TMB12A05 (active ~2.3kHz) on PC9 — GPIO on/off
//--------------------------------------------------------------
#ifndef __STM32F4_UB_BUZZER_H
#define __STM32F4_UB_BUZZER_H

#include "stm32f4xx.h"

#define BUZZER_GPIO_PORT       GPIOC
#define BUZZER_GPIO_PIN        GPIO_Pin_9
#define BUZZER_GPIO_CLK        RCC_AHB1Periph_GPIOC

/*
 * Chọn cách đấu dây (KHÔNG cần đổi chân — luôn dùng PC9):
 *
 * BUZZER_ACTIVE_LOW = 0  (cách A)
 *   PC9 ──► (+) TMB12A05
 *   GND ──► (-) TMB12A05
 *
 * BUZZER_ACTIVE_LOW = 1  (cách B — hay gặp khi nối 3.3V thẳng vào chân +)
 *   3.3V ──► (+) TMB12A05
 *   PC9  ──► (-) TMB12A05
 *
 * Nếu vẫn kêu liên tục: đổi 0 <-> 1 rồi flash lại.
 * KHÔNG nối (+) và (-) thẳng 3.3V + GND (sẽ kêu mãi).
 */
#define BUZZER_ACTIVE_LOW      1

void UB_Buzzer_Init(void);
void UB_Buzzer_Stop(void);
void UB_Buzzer_Tick1ms(void);
void UB_Buzzer_SequenceTick(void);
void UB_Buzzer_SetTone(uint32_t freq);
void UB_Buzzer_On(uint32_t freq);
void UB_Buzzer_Off(void);
void UB_Buzzer_PlayTone(uint32_t freq, uint32_t duration_ms);
void UB_Buzzer_PlayToneNonBlocking(uint32_t freq, uint32_t duration_ms);

void UB_Buzzer_Play_MenuClick(void);
void UB_Buzzer_Play_EatDot(void);
void UB_Buzzer_Play_EatEnergizer(void);
void UB_Buzzer_Play_Die(void);
void UB_Buzzer_Play_Win(void);
void UB_Buzzer_Play_Lost(void);

typedef enum {
    BUZZER_COUNTDOWN_READY = 0,
    BUZZER_COUNTDOWN_3,
    BUZZER_COUNTDOWN_2,
    BUZZER_COUNTDOWN_1,
    BUZZER_COUNTDOWN_GO,
} BuzzerCountdownStep_t;

uint32_t UB_Buzzer_Play_Countdown(BuzzerCountdownStep_t step);

extern volatile uint32_t buzzer_sequence_step;
extern volatile uint32_t buzzer_sequence_timer;
void UB_Buzzer_Play_Die_NonBlocking(void);

#endif // __STM32F4_UB_BUZZER_H

//--------------------------------------------------------------
// File     : stm32_ub_buzzer.c
//--------------------------------------------------------------
#include "stm32_ub_buzzer.h"
#include "stm32_ub_systick.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

volatile uint32_t UB_Buzzer_Timer_ms = 0;

void UB_Buzzer_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 1. Enable Clocks for GPIOC and TIM3
    RCC_AHB1PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. Configure PC9 as Alternate Function for TIM3 CH4
    GPIO_PinAFConfig(BUZZER_GPIO_PORT, BUZZER_GPIO_PINSOURCE, BUZZER_GPIO_AF);

    GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);

    // 3. Configure TIM3 Base
    // Prescaler = 89 => Timer clock = 90MHz / 90 = 1MHz (1 tick = 1us)
    // TIM3 is on APB1, on STM32F429 APB1 clock is max 45MHz, so TIM3 clock is 45MHz * 2 = 90MHz.
    TIM_TimeBaseStructure.TIM_Period = 999; // Default 1000Hz
    TIM_TimeBaseStructure.TIM_Prescaler = 89;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 4. Configure TIM3 CH4 as PWM Mode 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0; // Off by default (Duty Cycle = 0%)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);

    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // 5. Start TIM3
    TIM_Cmd(TIM3, ENABLE);
}

void UB_Buzzer_SetTone(uint32_t freq) {
    if (freq == 0) {
        TIM3->CCR4 = 0;
        return;
    }

    // Timer clock = 1MHz (after prescaler 89)
    // Period = 1,000,000 / freq
    uint32_t period = 1000000 / freq;
    if (period < 2) period = 2;

    TIM3->ARR = period - 1;
    TIM3->CCR4 = period / 2; // Duty Cycle = 50%
}

void UB_Buzzer_On(uint32_t freq) {
    UB_Buzzer_SetTone(freq);
}

void UB_Buzzer_Off(void) {
    TIM3->CCR4 = 0;
}

void UB_Buzzer_PlayTone(uint32_t freq, uint32_t duration_ms) {
    UB_Buzzer_On(freq);
    UB_Systick_Pause_ms(duration_ms);
    UB_Buzzer_Off();
}

void UB_Buzzer_PlayToneNonBlocking(uint32_t freq, uint32_t duration_ms) {
    UB_Buzzer_On(freq);
    UB_Buzzer_Timer_ms = duration_ms;
}

// ----------------------------------------------------------------------------
// Game sound effects implementation
// ----------------------------------------------------------------------------

void UB_Buzzer_Play_MenuClick(void) {
    UB_Buzzer_PlayToneNonBlocking(1200, 50); // 1200Hz, 50ms bíp ngắn
}

void UB_Buzzer_Play_EatDot(void) {
    UB_Buzzer_PlayToneNonBlocking(800, 30); // 800Hz, 30ms waka waka ngắn
}

void UB_Buzzer_Play_EatEnergizer(void) {
    UB_Buzzer_PlayToneNonBlocking(1500, 100); // 1500Hz, 100ms
}

void UB_Buzzer_Play_Die(void) {
    // Falling tone sequence when Pacman dies
    UB_Buzzer_PlayTone(1500, 100);
    UB_Buzzer_PlayTone(1200, 100);
    UB_Buzzer_PlayTone(900, 100);
    UB_Buzzer_PlayTone(600, 100);
    UB_Buzzer_PlayTone(300, 150);
}

void UB_Buzzer_Play_Win(void) {
    // Upbeat winning melody: Do-Re-Mi-Sol-Do
    UB_Buzzer_PlayTone(523, 150);  // Do (C5)
    UB_Buzzer_PlayTone(587, 150);  // Re (D5)
    UB_Buzzer_PlayTone(659, 150);  // Mi (E5)
    UB_Buzzer_PlayTone(784, 250);  // Sol (G5)
    UB_Buzzer_PlayTone(1046, 400); // Do (C6)
}

void UB_Buzzer_Play_Lost(void) {
    // Melancholic losing melody: La-Lab-Sol-Fa
    UB_Buzzer_PlayTone(440, 250);  // La (A4)
    UB_Buzzer_PlayTone(415, 250);  // La flat (Ab4)
    UB_Buzzer_PlayTone(392, 250);  // Sol (G4)
    UB_Buzzer_PlayTone(349, 500);  // Fa (F4)
}

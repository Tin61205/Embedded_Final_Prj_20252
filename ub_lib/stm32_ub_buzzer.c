//--------------------------------------------------------------
// File     : stm32_ub_buzzer.c
//--------------------------------------------------------------
#include "stm32_ub_buzzer.h"
#include "stm32_ub_systick.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

volatile uint32_t UB_Buzzer_Timer_ms = 0;
volatile uint32_t buzzer_sequence_step = 0;
volatile uint32_t buzzer_sequence_timer = 0;

// Chỉ bật buzzer lúc countdown (starting) và thắng game
#define BUZZER_ENABLE_MENU_CLICK   0
#define BUZZER_ENABLE_EAT_DOT      0
#define BUZZER_ENABLE_EAT_ENERGY   0
#define BUZZER_ENABLE_DIE          0
#define BUZZER_ENABLE_LOST         0

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

static void UB_Buzzer_StopAll(void) {
    UB_Buzzer_Timer_ms = 0;
    buzzer_sequence_step = 0;
    buzzer_sequence_timer = 0;
    UB_Buzzer_Off();
}

void UB_Buzzer_PlayTone(uint32_t freq, uint32_t duration_ms) {
    UB_Buzzer_On(freq);
    UB_Systick_Pause_ms(duration_ms);
    UB_Buzzer_Off();
}

void UB_Buzzer_PlayToneNonBlocking(uint32_t freq, uint32_t duration_ms) {
    if (buzzer_sequence_timer > 0) {
        return; // Đang chạy nhạc chết, bỏ qua các âm thanh ngắn khác
    }
    UB_Buzzer_On(freq);
    UB_Buzzer_Timer_ms = duration_ms;
}

void UB_Buzzer_Play_Die_NonBlocking(void) {
#if BUZZER_ENABLE_DIE
    UB_Buzzer_On(1500);
    buzzer_sequence_step = 0;
    buzzer_sequence_timer = 100;
#endif
}

typedef struct {
    uint32_t freq;
    uint32_t duration_ms;
} BuzzerNote_t;

static uint32_t UB_Buzzer_PlayMelody(const BuzzerNote_t *notes, uint32_t count) {
    uint32_t i;
    uint32_t total_ms = 0;

    UB_Buzzer_StopAll();

    for (i = 0; i < count; i++) {
        UB_Buzzer_PlayTone(notes[i].freq, notes[i].duration_ms);
        total_ms += notes[i].duration_ms;
    }

    return total_ms;
}

// ----------------------------------------------------------------------------
// Game sound effects implementation
// ----------------------------------------------------------------------------

void UB_Buzzer_Play_MenuClick(void) {
#if BUZZER_ENABLE_MENU_CLICK
    UB_Buzzer_PlayToneNonBlocking(1200, 50);
#endif
}

void UB_Buzzer_Play_EatDot(void) {
#if BUZZER_ENABLE_EAT_DOT
    UB_Buzzer_PlayToneNonBlocking(800, 30);
#endif
}

void UB_Buzzer_Play_EatEnergizer(void) {
#if BUZZER_ENABLE_EAT_ENERGY
    UB_Buzzer_PlayToneNonBlocking(1500, 100);
#endif
}

void UB_Buzzer_Play_Die(void) {
#if BUZZER_ENABLE_DIE
    UB_Buzzer_PlayTone(1500, 100);
    UB_Buzzer_PlayTone(1200, 100);
    UB_Buzzer_PlayTone(900, 100);
    UB_Buzzer_PlayTone(600, 100);
    UB_Buzzer_PlayTone(300, 150);
#endif
}

// Countdown cues derived from sound/starting-game.mp3
static const BuzzerNote_t countdown_ready[] = {
    { 587,  80 }, // D5
    { 523,  80 }, // C5
    { 1047, 80 }, // C6
};

static const BuzzerNote_t countdown_3[] = {
    { 1047, 80 }, // C6
    { 784,  80 }, // G5
    { 659,  160 }, // E5
    { 1047, 80 }, // C6
    { 784,  160 }, // G5
    { 659,  160 }, // E5
};

static const BuzzerNote_t countdown_2[] = {
    { 587,  80 }, // D5
    { 1175, 80 }, // D6
    { 1047, 80 }, // C6
    { 880,  80 }, // A5
    { 698,  160 }, // F5
    { 1175, 80 }, // D6
    { 880,  80 }, // A5
};

static const BuzzerNote_t countdown_1[] = {
    { 698,  240 }, // F5
    { 262,  80 }, // C4
    { 523,  80 }, // C5
    { 1047, 160 }, // C6
    { 784,  160 }, // G5
};

static const BuzzerNote_t countdown_go[] = {
    { 1047, 160 }, // C6
    { 784,  80 }, // G5
    { 659,  320 }, // E5
    { 698,  160 }, // F5
    { 784,  320 }, // G5
    { 880,  160 }, // A5
    { 1047, 160 }, // C6
    { 262,  80 }, // C4
};

uint32_t UB_Buzzer_Play_Countdown(BuzzerCountdownStep_t step) {
    switch (step) {
        case BUZZER_COUNTDOWN_READY:
            return UB_Buzzer_PlayMelody(countdown_ready,
                sizeof(countdown_ready) / sizeof(countdown_ready[0]));
        case BUZZER_COUNTDOWN_3:
            return UB_Buzzer_PlayMelody(countdown_3,
                sizeof(countdown_3) / sizeof(countdown_3[0]));
        case BUZZER_COUNTDOWN_2:
            return UB_Buzzer_PlayMelody(countdown_2,
                sizeof(countdown_2) / sizeof(countdown_2[0]));
        case BUZZER_COUNTDOWN_1:
            return UB_Buzzer_PlayMelody(countdown_1,
                sizeof(countdown_1) / sizeof(countdown_1[0]));
        case BUZZER_COUNTDOWN_GO:
            return UB_Buzzer_PlayMelody(countdown_go,
                sizeof(countdown_go) / sizeof(countdown_go[0]));
        default:
            return 0;
    }
}

// Victory fanfare derived from sound/victory_effect.mp3 (~3.9s)
static const BuzzerNote_t win_melody[] = {
    { 523,  100 }, // C5
    { 1047, 100 }, // C6
    { 784,  100 }, // G5
    { 659,  100 }, // E5
    { 1047, 100 }, // C6
    { 784,  100 }, // G5
    { 659,  300 }, // E5
    { 523,  100 }, // C5
    { 1047, 100 }, // C6
    { 784,  100 }, // G5
    { 392,  100 }, // G4
    { 698,  100 }, // F5
    { 1047, 100 }, // C6
    { 784,  100 }, // G5
    { 698,  100 }, // F5
    { 523,  100 }, // C5
    { 294,  100 }, // D4
    { 1047, 100 }, // C6
    { 784,  100 }, // G5
    { 659,  100 }, // E5
    { 784,  100 }, // G5
    { 392,  100 }, // G4
    { 659,  300 }, // E5
    { 698,  100 }, // F5
    { 784,  400 }, // G5
    { 880,  100 }, // A5
    { 1047, 200 }, // C6
    { 440,  200 }, // A4
    { 392,  100 }, // G4
    { 330,  100 }, // E4
};

void UB_Buzzer_Play_Win(void) {
    UB_Buzzer_PlayMelody(win_melody, sizeof(win_melody) / sizeof(win_melody[0]));
}

void UB_Buzzer_Play_Lost(void) {
#if BUZZER_ENABLE_LOST
    UB_Buzzer_PlayTone(440, 250);
    UB_Buzzer_PlayTone(415, 250);
    UB_Buzzer_PlayTone(392, 250);
    UB_Buzzer_PlayTone(349, 500);
#endif
}

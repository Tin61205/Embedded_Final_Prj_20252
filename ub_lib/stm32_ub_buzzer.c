//--------------------------------------------------------------
// File     : stm32_ub_buzzer.c
// Buzzer: TMB12A05 (active, ~2.3kHz fixed) — GPIO on/off only
//--------------------------------------------------------------
#include "stm32_ub_buzzer.h"
#include "stm32_ub_systick.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

volatile uint32_t UB_Buzzer_Timer_ms = 0;
volatile uint32_t buzzer_sequence_step = 0;
volatile uint32_t buzzer_sequence_timer = 0;

#define BUZZER_NOTE_GAP_MS         25
#define BUZZER_WAKA_SHORT_MS       22
#define BUZZER_WAKA_LONG_MS        30
#define BUZZER_WAKA_GAP_MS         58


#define BUZZER_ENABLE_MENU_CLICK   1
#define BUZZER_ENABLE_EAT_DOT      1
#define BUZZER_ENABLE_EAT_ENERGY   1
#define BUZZER_ENABLE_DIE          1
#define BUZZER_ENABLE_LOST         0

static void UB_Buzzer_StopAll(void);
static volatile uint32_t buzzer_menu_cooldown_ms = 0;
static volatile uint32_t buzzer_waka_gap_ms = 0;

static void UB_Buzzer_PinOn(void) {
#if BUZZER_ACTIVE_LOW
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
#else
    GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
#endif
}

static void UB_Buzzer_PinOff(void) {
#if BUZZER_ACTIVE_LOW
    GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
#else
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
#endif
}

static void UB_Buzzer_DisableLegacyPwm(void) {
    /* Tắt TIM3 nếu firmware cũ còn bật PWM trên PC9 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_Cmd(TIM3, DISABLE);
    TIM3->CCR4 = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
}

void UB_Buzzer_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);
    UB_Buzzer_DisableLegacyPwm();

    GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);

    UB_Buzzer_StopAll();
}

void UB_Buzzer_SetTone(uint32_t freq) {
    (void)freq;
}

void UB_Buzzer_On(uint32_t freq) {
    (void)freq;
    UB_Buzzer_PinOn();
}

void UB_Buzzer_Off(void) {
    UB_Buzzer_PinOff();
}

static void UB_Buzzer_StopAll(void) {
    UB_Buzzer_Timer_ms = 0;
    buzzer_sequence_step = 0;
    buzzer_sequence_timer = 0;
    buzzer_menu_cooldown_ms = 0;
    buzzer_waka_gap_ms = 0;
    UB_Buzzer_Off();
}

void UB_Buzzer_Stop(void) {
    UB_Buzzer_StopAll();
}

void UB_Buzzer_Tick1ms(void) {
    if (buzzer_menu_cooldown_ms > 0) {
        buzzer_menu_cooldown_ms--;
    }
    if (buzzer_waka_gap_ms > 0) {
        buzzer_waka_gap_ms--;
    }
}

void UB_Buzzer_PlayTone(uint32_t freq, uint32_t duration_ms) {
    UB_Buzzer_StopAll();
    if (duration_ms == 0) {
        return;
    }
    UB_Buzzer_On(freq);
    UB_Systick_Pause_ms(duration_ms);
    UB_Buzzer_Off();
}

void UB_Buzzer_PlayToneNonBlocking(uint32_t freq, uint32_t duration_ms) {
    if (duration_ms == 0) {
        return;
    }
    if (buzzer_sequence_timer > 0) {
        return;
    }
    UB_Buzzer_On(freq);
    UB_Buzzer_Timer_ms = duration_ms;
}

void UB_Buzzer_Play_Die_NonBlocking(void) {
#if BUZZER_ENABLE_DIE
    UB_Buzzer_StopAll();
    buzzer_sequence_step = 0;
    buzzer_sequence_timer = 1;
#endif
}

void UB_Buzzer_SequenceTick(void) {
    if (buzzer_sequence_timer == 0) {
        return;
    }

    buzzer_sequence_timer--;
    if (buzzer_sequence_timer != 0) {
        return;
    }

    switch (buzzer_sequence_step) {
        case 0:
            UB_Buzzer_On(0);
            buzzer_sequence_timer = 90;
            buzzer_sequence_step = 1;
            break;
        case 1:
            UB_Buzzer_Off();
            buzzer_sequence_timer = 45;
            buzzer_sequence_step = 2;
            break;
        case 2:
            UB_Buzzer_On(0);
            buzzer_sequence_timer = 90;
            buzzer_sequence_step = 3;
            break;
        case 3:
            UB_Buzzer_Off();
            buzzer_sequence_timer = 45;
            buzzer_sequence_step = 4;
            break;
        case 4:
            UB_Buzzer_On(0);
            buzzer_sequence_timer = 130;
            buzzer_sequence_step = 5;
            break;
        default:
            UB_Buzzer_Off();
            buzzer_sequence_step = 0;
            buzzer_sequence_timer = 0;
            break;
    }
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
        UB_Buzzer_On(notes[i].freq);
        UB_Systick_Pause_ms(notes[i].duration_ms);
        UB_Buzzer_Off();
        total_ms += notes[i].duration_ms;

        if ((i + 1) < count) {
            UB_Systick_Pause_ms(BUZZER_NOTE_GAP_MS);
            total_ms += BUZZER_NOTE_GAP_MS;
        }
    }

    UB_Buzzer_StopAll();
    return total_ms;
}

void UB_Buzzer_Play_MenuClick(void) {
#if BUZZER_ENABLE_MENU_CLICK
    if (buzzer_menu_cooldown_ms > 0) {
        return;
    }
    if (buzzer_sequence_timer > 0) {
        return;
    }
    UB_Buzzer_On(0);
    UB_Buzzer_Timer_ms = 35;
    buzzer_menu_cooldown_ms = 180;
#endif
}

void UB_Buzzer_Play_EatDot(void) {
#if BUZZER_ENABLE_EAT_DOT
    static uint8_t waka_phase = 0;

    if (buzzer_sequence_timer > 0) {
        return;
    }
    if (UB_Buzzer_Timer_ms > 0) {
        return;
    }
    if (buzzer_waka_gap_ms > 0) {
        return;
    }

    waka_phase ^= 1;
    UB_Buzzer_On(0);
    UB_Buzzer_Timer_ms = waka_phase ? BUZZER_WAKA_SHORT_MS : BUZZER_WAKA_LONG_MS;
    buzzer_waka_gap_ms = BUZZER_WAKA_GAP_MS;
#endif
}

void UB_Buzzer_Play_EatEnergizer(void) {
#if BUZZER_ENABLE_EAT_ENERGY
    if (buzzer_sequence_timer > 0) {
        return;
    }
    UB_Buzzer_StopAll();
    UB_Buzzer_On(0);
    UB_Buzzer_Timer_ms = 90;
#endif
}

void UB_Buzzer_Play_Die(void) {
#if BUZZER_ENABLE_DIE
    UB_Buzzer_PlayTone(0, 90);
    UB_Systick_Pause_ms(45);
    UB_Buzzer_PlayTone(0, 90);
    UB_Systick_Pause_ms(45);
    UB_Buzzer_PlayTone(0, 130);
#endif
}

static const BuzzerNote_t countdown_ready[] = {
    { 0, 60 }, { 0, 60 },
};

static const BuzzerNote_t countdown_3[] = {
    { 0, 150 },
};

static const BuzzerNote_t countdown_2[] = {
    { 0, 150 },
};

static const BuzzerNote_t countdown_1[] = {
    { 0, 150 },
};

static const BuzzerNote_t countdown_go[] = {
    { 0, 400 },
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

static const BuzzerNote_t win_melody[] = {
    { 0, 120 }, { 0, 120 }, { 0, 120 }, { 0, 500 },
};

void UB_Buzzer_Play_Win(void) {
    UB_Buzzer_PlayMelody(win_melody, sizeof(win_melody) / sizeof(win_melody[0]));
}

void UB_Buzzer_Play_Lost(void) {
#if BUZZER_ENABLE_LOST
    UB_Buzzer_PlayTone(0, 250);
    UB_Systick_Pause_ms(40);
    UB_Buzzer_PlayTone(0, 250);
    UB_Systick_Pause_ms(40);
    UB_Buzzer_PlayTone(0, 250);
    UB_Systick_Pause_ms(40);
    UB_Buzzer_PlayTone(0, 500);
#endif
}

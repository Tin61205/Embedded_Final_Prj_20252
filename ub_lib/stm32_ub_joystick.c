//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_joystick.h"

#if JOYSTICK_USE_ADC == 1

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

//--------------------------------------------------------------
// intern helpers
//--------------------------------------------------------------
static uint16_t joystick_adc_read(uint8_t channel);
static int32_t joystick_abs(int32_t value);

//--------------------------------------------------------------
// init ADC1 + analog pins PA1 (X) and PA2 (Y)
//--------------------------------------------------------------
void UB_Joystick_Init(void) {
    GPIO_InitTypeDef gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    ADC->CCR = (ADC->CCR & ~ADC_CCR_ADCPRE) | ADC_CCR_ADCPRE_0;

    ADC1->CR1 = 0;
    ADC1->CR2 = ADC_CR2_ADON;
    ADC1->SMPR2 = (ADC1->SMPR2 & ~((uint32_t)0x3F << 3)) | ((uint32_t)0x06 << 3) | ((uint32_t)0x06 << 6);
    ADC1->SQR1 = 0;
    ADC1->SQR3 = JOY_ADC_X_CHANNEL;

    joystick_adc_read(JOY_ADC_X_CHANNEL);
    joystick_adc_read(JOY_ADC_Y_CHANNEL);
}

//--------------------------------------------------------------
// read joystick and map to one direction
//--------------------------------------------------------------
uint32_t UB_Joystick_ReadDirection(void) {
    uint16_t raw_x;
    uint16_t raw_y;
    int32_t dx;
    int32_t dy;

    raw_x = joystick_adc_read(JOY_ADC_X_CHANNEL);
    raw_y = joystick_adc_read(JOY_ADC_Y_CHANNEL);

    dx = (int32_t)raw_x - JOY_ADC_CENTER;
    dy = (int32_t)raw_y - JOY_ADC_CENTER;

    if (joystick_abs(dx) < JOY_ADC_DEADZONE && joystick_abs(dy) < JOY_ADC_DEADZONE) {
        return JOY_DIR_NONE;
    }

    if (joystick_abs(dx) >= joystick_abs(dy)) {
        if (dx > JOY_ADC_THRESHOLD) {
            return JOY_DIR_RIGHT;
        }
        if (dx < -JOY_ADC_THRESHOLD) {
            return JOY_DIR_LEFT;
        }
    } else {
        if (dy > JOY_ADC_THRESHOLD) {
            return JOY_DIR_DOWN;
        }
        if (dy < -JOY_ADC_THRESHOLD) {
            return JOY_DIR_UP;
        }
    }

    return JOY_DIR_NONE;
}

static uint16_t joystick_adc_read(uint8_t channel) {
    ADC1->SQR3 = (uint32_t)(channel & 0x1F);
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0) {
    }
    return (uint16_t)ADC1->DR;
}

static int32_t joystick_abs(int32_t value) {
    if (value < 0) {
        return -value;
    }
    return value;
}

#endif
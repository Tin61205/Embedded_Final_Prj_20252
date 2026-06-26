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
static uint16_t joystick_adc_read_avg(uint8_t channel, uint8_t samples);
static uint32_t joystick_process_dir(uint16_t raw_x, uint16_t raw_y, int32_t cx, int32_t cy);

static int32_t joy1_center_x = JOY_ADC_CENTER;
static int32_t joy1_center_y = JOY_ADC_CENTER;
static int32_t joy2_center_x = JOY_ADC_CENTER;
static int32_t joy2_center_y = JOY_ADC_CENTER;

//--------------------------------------------------------------
// init ADC1 + analog pins PA1, PA2, PA5, PA7
//--------------------------------------------------------------
void UB_Joystick_Init(void) {
    GPIO_InitTypeDef gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AN;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio);

    ADC->CCR = (ADC->CCR & ~ADC_CCR_ADCPRE) | ADC_CCR_ADCPRE_0;

    ADC1->CR1 = 0;
    ADC1->CR2 = ADC_CR2_ADON;

    // Delay for ADC stabilization (at least 3us)
    {
        volatile uint32_t delay = 10000;
        while (delay--);
    }

    // Set sample time for channels 1, 2, 5, 7 to 144 cycles
    ADC1->SMPR2 = (ADC1->SMPR2 & ~((uint32_t)0x3F << 3)) | ((uint32_t)0x06 << 3) | ((uint32_t)0x06 << 6);
    ADC1->SMPR2 = (ADC1->SMPR2 & ~((uint32_t)0x07 << 15)) | ((uint32_t)0x06 << 15);
    ADC1->SMPR2 = (ADC1->SMPR2 & ~((uint32_t)0x07 << 21)) | ((uint32_t)0x06 << 21);
    ADC1->SQR1 = 0;

    joystick_adc_read(JOY1_ADC_X_CHANNEL);
    joystick_adc_read(JOY1_ADC_Y_CHANNEL);
    joystick_adc_read(JOY2_ADC_X_CHANNEL);
    joystick_adc_read(JOY2_ADC_Y_CHANNEL);

    {
        int32_t sum1_x = 0, sum1_y = 0;
        int32_t sum2_x = 0, sum2_y = 0;
        uint8_t i;

        for (i = 0; i < JOY_ADC_CALIB_SAMPLES; i++) {
            sum1_x += joystick_adc_read(JOY1_ADC_X_CHANNEL);
            sum1_y += joystick_adc_read(JOY1_ADC_Y_CHANNEL);
            sum2_x += joystick_adc_read(JOY2_ADC_X_CHANNEL);
            sum2_y += joystick_adc_read(JOY2_ADC_Y_CHANNEL);
        }

        joy1_center_x = sum1_x / JOY_ADC_CALIB_SAMPLES;
        joy1_center_y = sum1_y / JOY_ADC_CALIB_SAMPLES;
        joy2_center_x = sum2_x / JOY_ADC_CALIB_SAMPLES;
        joy2_center_y = sum2_y / JOY_ADC_CALIB_SAMPLES;
    }
}

//--------------------------------------------------------------
// read joystick 1 and map to one direction
//--------------------------------------------------------------
uint32_t UB_Joystick1_ReadDirection(void) {
    uint16_t raw_x = joystick_adc_read_avg(JOY1_ADC_X_CHANNEL, JOY_ADC_READ_SAMPLES);
    uint16_t raw_y = joystick_adc_read_avg(JOY1_ADC_Y_CHANNEL, JOY_ADC_READ_SAMPLES);
    return joystick_process_dir(raw_x, raw_y, joy1_center_x, joy1_center_y);
}

//--------------------------------------------------------------
// read joystick 2 and map to one direction
//--------------------------------------------------------------
uint32_t UB_Joystick2_ReadDirection(void) {
    uint16_t raw_x = joystick_adc_read_avg(JOY2_ADC_X_CHANNEL, JOY_ADC_READ_SAMPLES);
    uint16_t raw_y = joystick_adc_read_avg(JOY2_ADC_Y_CHANNEL, JOY_ADC_READ_SAMPLES);
    return joystick_process_dir(raw_x, raw_y, joy2_center_x, joy2_center_y);
}

static uint32_t joystick_process_dir(uint16_t raw_x, uint16_t raw_y, int32_t cx, int32_t cy) {
    uint32_t dir_x = JOY_DIR_NONE;
    uint32_t dir_y = JOY_DIR_NONE;
    int32_t dev_x = 0;
    int32_t dev_y = 0;

    int32_t val_x = raw_x;
    int32_t val_y = raw_y;

    // Auto-scale 12-bit ADC values to 8-bit if calibration center is in 12-bit range (> 255)
    if (cx > 255 || cy > 255) {
        val_x >>= 4;
        val_y >>= 4;
        cx >>= 4;
        cy >>= 4;
    }

    // Left: X = 0-2
    if (val_x <= 2) {
        dir_x = JOY_DIR_LEFT;
        int32_t denom = cx;
        if (denom < 1) denom = 1;
        dev_x = ((cx - val_x) * 1000) / denom;
    }
    // Right: X > 100
    else if (val_x > 100) {
        dir_x = JOY_DIR_RIGHT;
        int32_t denom = 255 - cx;
        if (denom < 1) denom = 1;
        dev_x = ((val_x - cx) * 1000) / denom;
    }

    // Up: Y = 0-2
    if (val_y <= 2) {
        dir_y = JOY_DIR_UP;
        int32_t denom = cy;
        if (denom < 1) denom = 1;
        dev_y = ((cy - val_y) * 1000) / denom;
    }
    // Down: Y > 250
    else if (val_y > 250) {
        dir_y = JOY_DIR_DOWN;
        int32_t denom = 255 - cy;
        if (denom < 1) denom = 1;
        dev_y = ((val_y - cy) * 1000) / denom;
    }

    // Choose axis with the larger normalized deviation if both are active (diagonal movement)
    if (dir_x != JOY_DIR_NONE && dir_y != JOY_DIR_NONE) {
        if (dev_x >= dev_y) {
            return dir_x;
        } else {
            return dir_y;
        }
    } else if (dir_x != JOY_DIR_NONE) {
        return dir_x;
    } else if (dir_y != JOY_DIR_NONE) {
        return dir_y;
    }

    return JOY_DIR_NONE;
}

static uint16_t joystick_adc_read(uint8_t channel) {
    ADC1->SQR3 = (uint32_t)(channel & 0x1F);
    ADC1->SR = (uint32_t)~ADC_SR_EOC; // Clear EOC flag
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0) {
    }
    return (uint16_t)ADC1->DR;
}

static uint16_t joystick_adc_read_avg(uint8_t channel, uint8_t samples) {
    uint32_t sum = 0;
    uint8_t i;

    if (samples == 0) {
        return joystick_adc_read(channel);
    }

    for (i = 0; i < samples; i++) {
        sum += joystick_adc_read(channel);
    }

    return (uint16_t)(sum / samples);
}

#endif
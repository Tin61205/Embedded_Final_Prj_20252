//--------------------------------------------------------------
// File     : stm32_ub_joystick.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_JOYSTICK_H
#define __STM32F4_UB_JOYSTICK_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"

//--------------------------------------------------------------
// Enable analog joystick support (KY-023 style module)
// 1 = enabled, 0 = disabled
//--------------------------------------------------------------
#define JOYSTICK_USE_ADC   1

#if JOYSTICK_USE_ADC == 1

//--------------------------------------------------------------
// Hardware mapping
// VRx -> PA1 (ADC1_IN1), VRy -> PA2 (ADC1_IN2)
// Optional SW pin is not used by firmware (use PA0 for Start)
//--------------------------------------------------------------
#define JOY_ADC_X_CHANNEL      1
#define JOY_ADC_Y_CHANNEL      2

#define JOY_ADC_CENTER         2048
#define JOY_ADC_DEADZONE       150
#define JOY_ADC_THRESHOLD      300
#define JOY_ADC_CALIB_SAMPLES  16
#define JOY_ADC_READ_SAMPLES   4

//--------------------------------------------------------------
// Direction values (match gui.h GUI_JOY_*)
//--------------------------------------------------------------
#define JOY_DIR_NONE    0
#define JOY_DIR_UP      1
#define JOY_DIR_RIGHT   2
#define JOY_DIR_DOWN    3
#define JOY_DIR_LEFT    4

void UB_Joystick_Init(void);
uint32_t UB_Joystick_ReadDirection(void);
void UB_Joystick_GetDebugValues(uint16_t *raw_x, uint16_t *raw_y, int32_t *center_x, int32_t *center_y);

#endif

//--------------------------------------------------------------
#endif // __STM32F4_UB_JOYSTICK_H
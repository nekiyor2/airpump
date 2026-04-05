/*
 * compressor.h
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "stm32c0xx_hal.h"

// PWM межі ESC (мікросекунди)
#define ESC_MIN_US   1000   // стоп
#define ESC_MAX_US   2000   // повна швидкість
#define ESC_ARM_US   1000   // значення для ініціалізації ESC

void Compressor_Init(TIM_HandleTypeDef *htim);
void Compressor_SetSpeed(uint8_t percent); // 0..100%
void Compressor_Stop(void);

#endif

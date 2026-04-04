/*
 * compressor.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#include "compressor.h"

static TIM_HandleTypeDef *_htim;

void Compressor_Init(TIM_HandleTypeDef *htim)
{
    _htim = htim;

    // Arming sequence — ESC вимагає спочатку отримати мінімальний сигнал
    __HAL_TIM_SET_COMPARE(_htim, TIM_CHANNEL_3, ESC_ARM_US);
    HAL_TIM_PWM_Start(_htim, TIM_CHANNEL_3);
    HAL_Delay(2000); // чекаємо поки ESC ініціалізується
}

void Compressor_SetSpeed(uint8_t percent)
{
    if (percent > 100) percent = 100;

    // Переводимо % в мікросекунди: 0% = 1000мкс, 100% = 2000мкс
    uint32_t pulse = ESC_MIN_US + ((ESC_MAX_US - ESC_MIN_US) * percent / 100);
    __HAL_TIM_SET_COMPARE(_htim, TIM_CHANNEL_3, pulse);
}

void Compressor_Stop(void)
{
    __HAL_TIM_SET_COMPARE(_htim, TIM_CHANNEL_3, ESC_MIN_US);
}
В main.c:
// USER CODE BEGIN 2:
Compressor_Init(&htim2);

// Вмикаємо на 50%:
Compressor_SetSpeed(50);

// Зупиняємо:
Compressor_Stop();

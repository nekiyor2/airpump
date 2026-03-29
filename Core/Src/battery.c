/*
 * battery.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#include "battery.h"

extern ADC_HandleTypeDef hadc1; //Використання змінної з іншої папкu

void Battery_Init(void)
{
    // ADC вже ініціалізований в MX_ADC1_Init
    // Тут можна додати калібрування якщо потрібно
}

float Battery_ReadVoltage(void)
{
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100)!= HAL_OK)
    return -1.0f; //сигнал помилки
    
    uint16_t adc_value = HAL_ADC_GetValue(&hadc1);

    float v_adc     = (adc_value / 4095.0f) * 3.3f; // Перевід у напругу (0–3.3V)
    float v_battery = v_adc * 2.0f;//Перевід у напругу батареї. дільник R4=R8=100k
    return v_battery;
}

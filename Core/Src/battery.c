/*
 * battery.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */
#include "battery.h"

//Використання змінної з іншої папки
extern ADC_HandleTypeDef hadc1;

void Battery_Init(void);
{
float Battery_ReadVoltage(void)
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

    uint16_t adc_value = HAL_ADC_GetValue(&hadc1);

    // Перевід у напругу (0–3.3V)
    float v_adc = (adc_value / 4095.0f) * 3.3f;
//Перевід у напругу батареї.
    float v_battery = v_adc * 2.0f;
//Повернення назад напруги батареї реальна.
    return v_battery;
}
}


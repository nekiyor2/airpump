/*
 * battery.h
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#include "main.h"
#include "stm32c0xx_hal.h"

// 2S Li-Ion: номінал 7.4V, максимум 8.4V, мінімум ~6.0V
#define BATTERY_MIN_V   6.0f
#define BATTERY_MAX_V   8.4f

// Дільник R4=100k, R8=47k → коефіцієнт = (100+47)/47
#define BATTERY_DIVIDER (147.0f / 47.0f)

void  Battery_Init(void);
float Battery_ReadVoltage(void);
uint8_t Battery_IsLow(float voltage);


#endif

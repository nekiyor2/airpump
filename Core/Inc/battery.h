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

void Battery_Init(void);
float Battery_ReadVoltage(void);


#endif

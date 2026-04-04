/*
 * xgzp6847d.h
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#ifndef XGZP6847D_H_
#define XGZP6847D_H_

#include "main.h"
#include "stm32c0xx_hal.h"

HAL_StatusTypeDef XGZP6847D_Init(void);
float XGZP6847D_ReadPressure(void);


#endif

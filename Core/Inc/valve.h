/*
 * valve.h
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#ifndef VALVE_H
#define VALVE_H

#include "stm32c0xx_hal.h"

// PA1 — клапан через Q2 (AO3400A) + R7 (220 Ом)
#define VALVE_PORT  GPIOA
#define VALVE_PIN   GPIO_PIN_1

void Valve_Init(void);
void Valve_Open(void);
void Valve_Close(void);
uint8_t Valve_IsOpen(void);

#endif

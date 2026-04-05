/*
 * valve.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#include "valve.h"

static uint8_t _valve_state = 0; // 0 = закритий, 1 = відкритий

void Valve_Init(void)
{
    // Пін вже налаштований в MX_GPIO_Init як GPIO_Output
    // Переконуємось що клапан закритий при старті
    Valve_Close();
}

void Valve_Open(void)
{
    HAL_GPIO_WritePin(VALVE_PORT, VALVE_PIN, GPIO_PIN_SET);
    _valve_state = 1;
}

void Valve_Close(void)
{
    HAL_GPIO_WritePin(VALVE_PORT, VALVE_PIN, GPIO_PIN_RESET);
    _valve_state = 0;
}

uint8_t Valve_IsOpen(void)
{
    return _valve_state;
}

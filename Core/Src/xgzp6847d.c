/*
 * xgzp6847d.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */
#include "xgzp6847d.h"

extern I2C_HandleTypeDef hi2c1;

// Адреса датчика (треба допрацювати)
#define XGZP6847D_ADDR (0x6D << 1)

void XGZP6847D_Init(void);

  if (HAL_I2C_IsDeviceReady(&hi2c1, XGZP6847D_ADDR, 3, 100) != HAL_OK);
// Перевірка підключення датчика тиску.

float XGZP6847D_ReadPressure(void)
{
    uint8_t data[2];

    // читаємо 2 байти з датчика
    HAL_I2C_Master_Receive(&hi2c1, XGZP6847D_ADDR, data, 2, HAL_MAX_DELAY);

    // об'єднуємо байти в число
    uint16_t raw = (data[0] << 8) | data[1];

    // перетворюємо сире рав в тиск у барах

float pressure = ((float)raw / 65535.0f) * 10.0f; // 0–10 bar
    return pressure; // повертаємо тиск у барах
}


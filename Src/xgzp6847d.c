/*
 * xgzp6847d.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#include "xgzp6847d.h"

extern I2C_HandleTypeDef hi2c1;

#define XGZP6847D_ADDR  (0x6D << 1)

// Datasheet: для діапазону 500kPa → K = 256
// P(Pa) = raw_24bit / 256
// Переводимо в бари: 1 bar = 100000 Pa
#define XGZP_K  256.0f

HAL_StatusTypeDef XGZP6847D_Init(void)
{
    return HAL_I2C_IsDeviceReady(&hi2c1, XGZP6847D_ADDR, 3, 100);
}

float XGZP6847D_ReadPressure(void)
{
    // Крок 1: запускаємо вимірювання
    uint8_t cmd = 0x0A;
    if (HAL_I2C_Mem_Write(&hi2c1, XGZP6847D_ADDR,
                           0x30, I2C_MEMADD_SIZE_8BIT,
                           &cmd, 1, 100) != HAL_OK)
    {
        return -1.0f;
    }

    HAL_Delay(5); // чекаємо завершення вимірювання

    // Крок 2: читаємо 3 байти тиску з регістру 0x06
    uint8_t data[3];
    if (HAL_I2C_Mem_Read(&hi2c1, XGZP6847D_ADDR,
                          0x06, I2C_MEMADD_SIZE_8BIT,
                          data, 3, 100) != HAL_OK)
    {
        return -1.0f;
    }

    // Крок 3: збираємо 24-бітне знакове число
    int32_t raw = ((int32_t)data[0] << 16) |
                  ((int32_t)data[1] << 8)  |
                   (int32_t)data[2];

    // Крок 4: знакове розширення з 24 до 32 біт
    if (raw & 0x800000) raw |= 0xFF000000;

    // Крок 5: переводимо в бари
    float pressure_pa  = (float)raw / XGZP_K;
    float pressure_bar = pressure_pa / 100000.0f;

    return pressure_bar;
}

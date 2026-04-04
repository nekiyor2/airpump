/*
 * utils.h
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "stm32c0xx_hal.h"

// --- Пороги тиску (бари) ---
// Можуть бути перезаписані командою з ESP32
#define PRESSURE_MIN_DEFAULT     3.0f  // нижче → вмикаємо компресор
#define PRESSURE_MAX_DEFAULT     4.5f  // вище  → вимикаємо компресор
#define PRESSURE_RELIEF_DEFAULT  4.8f  // вище  → відкриваємо клапан (аварія)
#define PRESSURE_RELIEF_CLOSE    4.7f  // нижче → закриваємо клапан

// --- Швидкість компресора ---
#define COMPRESSOR_SPEED_DEFAULT 80    // % (не 100% — щадний режим)

// --- Стан системи ---
typedef enum {
    SYS_OK    = 0,
    SYS_ERROR = 1
} SystemState_t;

// --- Конфігурація (приходить з ESP32) ---
typedef struct {
    float   pressure_min;
    float   pressure_max;
    float   pressure_relief;
} ControlConfig_t;

// --- Телеметрія (іде на ESP32) ---
typedef struct {
    float    pressure;
    float    battery_voltage;
    uint8_t  compressor_on;
    uint8_t  valve_on;
    uint8_t  system_state;
} TelemetryPacket_t;

void    Control_Init(void);
void    Control_Update(float pressure, float battery_voltage);
void    Control_SetConfig(ControlConfig_t *cfg);
void    Control_GetTelemetry(TelemetryPacket_t *pkt);

#endif


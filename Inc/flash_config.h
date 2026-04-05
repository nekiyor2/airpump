/*
 * flash_config.h
 *
 *  Created on: 5 квіт. 2026 р.
 *      Author: nikit
 */

#ifndef FLASH_CONFIG_H
#define FLASH_CONFIG_H

#include "main.h"
#include "control.h"

// Остання сторінка Flash (32KB МК, розмір сторінки 2KB)
#define FLASH_CONFIG_ADDR   0x08007C00UL

HAL_StatusTypeDef FlashConfig_Save(ControlConfig_t *cfg);
HAL_StatusTypeDef FlashConfig_Load(ControlConfig_t *cfg);
uint8_t           FlashConfig_IsValid(void);

#endif

/*
 * flash_config.c
 *
 *  Created on: 5 квіт. 2026 р.
 *      Author: nikit
 */

#include "flash_config.h"
#include <string.h>

// Магічне число — перевіряємо що дані валідні
#define CONFIG_MAGIC  0xA55A0001UL

typedef struct {
    uint32_t       magic;
    ControlConfig_t cfg;
    uint32_t       checksum; // проста сума для перевірки
} FlashConfigRecord_t;

static uint32_t CalcChecksum(ControlConfig_t *cfg)
{
    uint32_t sum = 0;
    uint8_t *p = (uint8_t*)cfg;
    for (uint8_t i = 0; i < sizeof(ControlConfig_t); i++)
        sum += p[i];
    return sum;
}

HAL_StatusTypeDef FlashConfig_Save(ControlConfig_t *cfg)
{
    FlashConfigRecord_t rec;
    rec.magic    = CONFIG_MAGIC;
    rec.cfg      = *cfg;
    rec.checksum = CalcChecksum(cfg);

    HAL_FLASH_Unlock();

    // Стираємо сторінку
    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase.Page        = 15;          // остання сторінка
    erase.NbPages     = 1;

    uint32_t page_error = 0;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase, &page_error);

    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    // Пишемо подвійними словами (64 біт) — вимога STM32C0
    uint64_t *src = (uint64_t*)&rec;
    uint32_t addr = FLASH_CONFIG_ADDR;
    uint8_t  words = (sizeof(FlashConfigRecord_t) + 7) / 8;

    for (uint8_t i = 0; i < words; i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                                   addr, src[i]);
        if (status != HAL_OK) break;
        addr += 8;
    }

    HAL_FLASH_Lock();
    return status;
}

HAL_StatusTypeDef FlashConfig_Load(ControlConfig_t *cfg)
{
    FlashConfigRecord_t *rec = (FlashConfigRecord_t*)FLASH_CONFIG_ADDR;

    if (rec->magic != CONFIG_MAGIC)
        return HAL_ERROR;

    if (rec->checksum != CalcChecksum(&rec->cfg))
        return HAL_ERROR;

    *cfg = rec->cfg;
    return HAL_OK;
}

uint8_t FlashConfig_IsValid(void)
{
    FlashConfigRecord_t *rec = (FlashConfigRecord_t*)FLASH_CONFIG_ADDR;
    return (rec->magic == CONFIG_MAGIC &&
            rec->checksum == CalcChecksum(&rec->cfg)) ? 1 : 0;
}

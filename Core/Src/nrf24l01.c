/*
 * nrf24l01.c
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#include "nrf24l01.h"

static SPI_HandleTypeDef *_hspi;

// Адреса каналу — однакова на STM32 і ESP32
static uint8_t nrf_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};

// --- Низькорівневі SPI функції ---

static void CSN_Low(void)  { HAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_RESET); }
static void CSN_High(void) { HAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_SET); }
static void CE_Low(void)   { HAL_GPIO_WritePin(NRF_CE_PORT,  NRF_CE_PIN,  GPIO_PIN_RESET); }
static void CE_High(void)  { HAL_GPIO_WritePin(NRF_CE_PORT,  NRF_CE_PIN,  GPIO_PIN_SET); }

static void NRF_WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { NRF_CMD_W_REG | reg, val };
    CSN_Low();
    HAL_SPI_Transmit(_hspi, buf, 2, 100);
    CSN_High();
}

static uint8_t NRF_ReadReg(uint8_t reg)
{
    uint8_t tx = NRF_CMD_R_REG | reg;
    uint8_t rx = 0;
    CSN_Low();
    HAL_SPI_Transmit(_hspi, &tx, 1, 100);
    HAL_SPI_Receive(_hspi, &rx, 1, 100);
    CSN_High();
    return rx;
}

static void NRF_WriteRegMulti(uint8_t reg, uint8_t *data, uint8_t len)
{
    uint8_t cmd = NRF_CMD_W_REG | reg;
    CSN_Low();
    HAL_SPI_Transmit(_hspi, &cmd, 1, 100);
    HAL_SPI_Transmit(_hspi, data, len, 100);
    CSN_High();
}

static void NRF_SendCmd(uint8_t cmd)
{
    CSN_Low();
    HAL_SPI_Transmit(_hspi, &cmd, 1, 100);
    CSN_High();
}

// --- Публічні функції ---

void NRF24_Init(SPI_HandleTypeDef *hspi)
{
    _hspi = hspi;

    CE_Low();
    CSN_High();
    HAL_Delay(5); // час на старт модуля

    NRF_WriteReg(NRF_REG_CONFIG,     0x0B); // PWR_UP, CRC 1 байт, RX mode поки вимкнено
    NRF_WriteReg(NRF_REG_EN_AA,      0x01); // Auto-ACK на pipe 0
    NRF_WriteReg(NRF_REG_EN_RXADDR,  0x01); // Увімкнути pipe 0
    NRF_WriteReg(NRF_REG_SETUP_AW,   0x03); // Адреса 5 байт
    NRF_WriteReg(NRF_REG_SETUP_RETR, 0x3F); // 1000мкс затримка, 15 повторів
    NRF_WriteReg(NRF_REG_RF_CH,      0x4C); // Канал 76 (вільний від WiFi)
    NRF_WriteReg(NRF_REG_RF_SETUP,   0x06); // 1Mbps, 0dBm
    NRF_WriteReg(NRF_REG_RX_PW_P0,   NRF_PAYLOAD_SIZE);

    NRF_WriteRegMulti(NRF_REG_RX_ADDR_P0, nrf_addr, 5);
    NRF_WriteRegMulti(NRF_REG_TX_ADDR,    nrf_addr, 5);

    // Очищаємо FIFO і флаги
    NRF_SendCmd(NRF_CMD_FLUSH_TX);
    NRF_SendCmd(NRF_CMD_FLUSH_RX);
    NRF_WriteReg(NRF_REG_STATUS, 0x70);
}

void NRF24_SetRX(void)
{
    NRF_WriteReg(NRF_REG_CONFIG, 0x0B | 0x01); // PRIM_RX = 1
    NRF_SendCmd(NRF_CMD_FLUSH_RX);
    NRF_WriteReg(NRF_REG_STATUS, 0x70);
    CE_High(); // слухаємо
}

void NRF24_SetTX(void)
{
    CE_Low();
    NRF_WriteReg(NRF_REG_CONFIG, 0x0B); // PRIM_RX = 0
    HAL_Delay(1);
}

uint8_t NRF24_DataReady(void)
{
    uint8_t status = NRF_ReadReg(NRF_REG_STATUS);
    return (status & NRF_STATUS_RX_DR) ? 1 : 0;
}

void NRF24_Receive(uint8_t *data)
{
    uint8_t cmd = NRF_CMD_R_RX_PAYLOAD;
    CSN_Low();
    HAL_SPI_Transmit(_hspi, &cmd, 1, 100);
    HAL_SPI_Receive(_hspi, data, NRF_PAYLOAD_SIZE, 100);
    CSN_High();

    NRF_WriteReg(NRF_REG_STATUS, NRF_STATUS_RX_DR); // скидаємо флаг
}

uint8_t NRF24_Transmit(uint8_t *data, uint8_t len)
{
    CE_Low();
    NRF24_SetTX();

    uint8_t cmd = NRF_CMD_W_TX_PAYLOAD;
    CSN_Low();
    HAL_SPI_Transmit(_hspi, &cmd, 1, 100);
    HAL_SPI_Transmit(_hspi, data, len, 100);
    CSN_High();

    CE_High();
    HAL_Delay(1);
    CE_Low();

    // Чекаємо TX_DS або MAX_RT
    uint32_t start = HAL_GetTick();
    uint8_t status = 0;
    while (HAL_GetTick() - start < 100)
    {
        status = NRF_ReadReg(NRF_REG_STATUS);
        if (status & (NRF_STATUS_TX_DS | NRF_STATUS_MAX_RT)) break;
    }

    NRF_WriteReg(NRF_REG_STATUS, 0x70); // скидаємо всі флаги
    NRF_SendCmd(NRF_CMD_FLUSH_TX);

    NRF24_SetRX(); // повертаємось в режим прийому

    return (status & NRF_STATUS_TX_DS) ? 1 : 0; // 1 = успіх
}
uint8_t NRF24_Check(void)
{
    uint8_t reg = NRF_ReadReg(NRF_REG_CONFIG);
    return (reg == 0x0B) ? 1 : 0;
}

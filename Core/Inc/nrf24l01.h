/*
 * nrf24l01.h
 *
 *  Created on: 27 бер. 2026 р.
 *      Author: nikit
 */

#ifndef INC_NRF24L01_H
#define INC_NRF24L01_H

#include "stm32c0xx_hal.h"
#include "main.h"

// Піни
#define NRF_CE_PORT     GPIO_Output_CE_GPIO_Port
#define NRF_CE_PIN      GPIO_Output_CE_Pin
#define NRF_CSN_PORT    GPIO_Output_CSN_GPIO_Port
#define NRF_CSN_PIN     GPIO_Output_CSN_Pin

// Команди nRF24L01
#define NRF_CMD_R_REG       0x00
#define NRF_CMD_W_REG       0x20
#define NRF_CMD_R_RX_PAYLOAD 0x61
#define NRF_CMD_W_TX_PAYLOAD 0xA0
#define NRF_CMD_FLUSH_TX    0xE1
#define NRF_CMD_FLUSH_RX    0xE2
#define NRF_CMD_NOP         0xFF

// Регістри
#define NRF_REG_CONFIG      0x00
#define NRF_REG_EN_AA       0x01
#define NRF_REG_EN_RXADDR   0x02
#define NRF_REG_SETUP_AW    0x03
#define NRF_REG_SETUP_RETR  0x04
#define NRF_REG_RF_CH       0x05
#define NRF_REG_RF_SETUP    0x06
#define NRF_REG_STATUS      0x07
#define NRF_REG_RX_ADDR_P0  0x0A
#define NRF_REG_TX_ADDR     0x10
#define NRF_REG_RX_PW_P0   0x11
#define NRF_REG_FIFO_STATUS 0x17

// Біти STATUS
#define NRF_STATUS_RX_DR    (1 << 6)
#define NRF_STATUS_TX_DS    (1 << 5)
#define NRF_STATUS_MAX_RT   (1 << 4)

// Розміри пакетів
#define NRF_PAYLOAD_SIZE    12  // байт (підходить для обох структур)

// Прототипи
void NRF24_Init(SPI_HandleTypeDef *hspi);
void NRF24_SetRX(void);
void NRF24_SetTX(void);
uint8_t NRF24_DataReady(void);
void NRF24_Receive(uint8_t *data);
uint8_t NRF24_Transmit(uint8_t *data, uint8_t len);
uint8_t NRF24_Check(void); // повертає 1 якщо ок, 0 якщо помилка

#endif

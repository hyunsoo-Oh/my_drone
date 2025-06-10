/*
 * nrf24l01.c
 *
 *  Created on: Jun 9, 2025
 *      Author: USER
 */

#include "nrf24l01.h"

static HAL_StatusTypeDef nRF24L01_SPIWrite(uint8_t reg, uint8_t data)
{
	HAL_StatusTypeDef status;
    uint8_t buf[2];
    buf[0] = NRF24_CMD_W_REGISTER | (reg & 0x1F);  // W_REGISTER 명령 + 레지스터 주소
    buf[1] = data;

    NRF24_CSN_LOW();
    status = HAL_SPI_Transmit(&hspi1, buf, 2, 100);
    NRF24_CSN_HIGH();

    return status;
}

static HAL_StatusTypeDef SPI_Read(uint8_t reg, uint8_t *data, uint8_t length)
{
    HAL_StatusTypeDef status;
    uint8_t cmd = NRF24_CMD_R_REGISTER | (reg & 0x1F);  // R_REGISTER 명령 + 레지스터 주소

    NRF24_CSN_LOW();
    status = HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    if (status != HAL_OK) {
        NRF24_CSN_HIGH();
        return status;
    }

    status = HAL_SPI_Receive(&hspi1, data, length, 100);
    NRF24_CSN_HIGH();

    return status;
}

void nRF24L01_TxInit(void)
{
	nRF24L01_SPIWrite(NRF24_REG_EN_AA, 0x01);	// Pipe 0 만 Auto ACK
	nRF24L01_SPIWrite(NRF24_REG_RF_CH, 76);		// Channel 76
	nRF24L01_SPIWrite(NRF24_REG_RF_SETUP, 0x06);// 1Mbps, 0dBm
	nRF24L01_SPIWrite(NRF24_REG_CONFIG, CONFIG_PWR_UP | CONFIG_EN_CRC);	// // 전원 ON, CRC ON, TX 모드(PRIM_RX=0)

	HAL_Delay(2);
}
void nRF24L01_RxInit(void)
{

}

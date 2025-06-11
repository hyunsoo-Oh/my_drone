/*
 * nrf24l01.c
 *
 *  Created on: Jun 9, 2025
 *      Author: USER
 */

#include "nrf24l01.h"
#include "spi.h"
#include "gpio.h"

#define NRF24_SPI			&hspi1

#define NRF24_CE_HIGH()   	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)
#define NRF24_CE_LOW()    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)
#define NRF24_CSN_HIGH()  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define NRF24_CSN_LOW()   	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)

uint8_t status;

// SPI 명령 W_REGISTER (0x20) + 5비트 주소 → 해당 레지스터에 단일 값 저장
static void nRF24L01_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t tx[2];
    tx[0] = NRF24_CMD_W_REGISTER | (reg & 0x1F);  		// W_REGISTER 명령 (0x20) + 5비트 주소
    tx[1] = data;

    NRF24_CSN_LOW();
    HAL_SPI_Transmit(NRF24_SPI, tx, 2, 100);
    NRF24_CSN_HIGH();
}

// 레지스터 연속 쓰기 (버퍼) → 예: TX_ADDR, RX_ADDR_P0에 5바이트 주소 쓸 때
static void nRF24L01_WriteBuf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_W_REGISTER | (reg & 0x1F);
    NRF24_CSN_LOW();
    HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(NRF24_SPI, buf, len, 100);
    NRF24_CSN_HIGH();
}

// 명령 R_REGISTER (0x00) + 주소 후, 지정된 바이트 수만큼 수신
static void nRF24L01_ReadReg(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_R_REGISTER | (reg & 0x1F);  	// R_REGISTER 명령 (0x00) + 5비트 주소

    NRF24_CSN_LOW();
    HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);        // 명령 전송
    HAL_SPI_Receive(NRF24_SPI, buf, len, 100);     // 데이터 수신
    NRF24_CSN_HIGH();
}

// nRF24L01이 무선으로 다른 nRF24L01 모듈에게 전송할 실제 데이터 (Tx FIFO에 적재)
static void nRF24L01_WritePayload(uint8_t *buf, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD;
    NRF24_CSN_LOW();
    HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
    HAL_SPI_Transmit(NRF24_SPI, buf, len, 100);
    NRF24_CSN_HIGH();
}

void nRF24L01_TxInit(void)
{
    NRF24_CE_LOW();		// 송신/수신 중단 (대기)
    NRF24_CSN_HIGH();	// SPI 비활성화

    HAL_Delay(5);		 // 모듈 안정화 대기

    // 전원 ON, CRC ON, TX 모드(PRIM_RX=0)
    nRF24L01_WriteReg(NRF24_REG_CONFIG, CONFIG_PWR_UP | CONFIG_EN_CRC);
	HAL_Delay(100);

    // Pipe 설정 (Pipe 0 ~ 5 -> 최대 5개 수신기 가능)
	nRF24L01_WriteReg(NRF24_REG_EN_AA, 0x01);		// Pipe 0 만 Auto ACK
	nRF24L01_WriteReg(NRF24_REG_EN_RXADDR, 0x01);	// Pipe 0 활성화

	//RF 설정
	nRF24L01_WriteReg(NRF24_REG_RF_SETUP, 0x07);	// 1Mbps, 0dBm
	nRF24L01_WriteReg(NRF24_REG_SETUP_RETR, 0x3F);	// Auto ACK가 실패했을 때 15회 재전송, 750us 지연
	nRF24L01_WriteReg(NRF24_REG_RF_CH, 76);			// Channel 76

	// TX와 RX가 같은 주소를 설정해야 통신 가능 (Auto ACK를 사용하는 경우)
	// 송신자는 수신 대상마다 TX_ADDR을 변경할 필요 있음
	uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nRF24L01_WriteBuf(NRF24_REG_TX_ADDR, tx_addr, 5);		// 송신 주소
	nRF24L01_WriteBuf(NRF24_REG_RX_ADDR_P0, tx_addr, 5);  	// ACK 받을 주소
	nRF24L01_WriteReg(NRF24_REG_SETUP_AW, 0x03);          	// 5바이트 주소 (길이)

    // FIFO 클리어
    nRF24L01_WriteReg(NRF24_CMD_FLUSH_TX, 0x00);
    nRF24L01_WriteReg(NRF24_CMD_FLUSH_RX, 0x00);

    // STATUS 클리어
    nRF24L01_WriteReg(NRF24_REG_STATUS, STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT);

    NRF24_CE_LOW();
}

void nRF24L01_RxInit(void)
{
	uint8_t rx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};  // 송신기와 같은 주소

	NRF24_CE_LOW();     // 설정 전 CE는 항상 LOW
	NRF24_CSN_HIGH();
	HAL_Delay(5);        // 안정화 시간

	// CONFIG: 전원 ON, CRC ON, RX 모드 (PRIM_RX = 1)
	nRF24L01_WriteReg(NRF24_REG_CONFIG,
					  CONFIG_PWR_UP | CONFIG_EN_CRC | CONFIG_PRIM_RX);
	HAL_Delay(100);

	// Pipe 0만 활성화 + Auto ACK 사용
	nRF24L01_WriteReg(NRF24_REG_EN_AA, 0x01);       // Pipe 0만 Auto ACK
	nRF24L01_WriteReg(NRF24_REG_EN_RXADDR, 0x01);   // Pipe 0만 수신 허용

	// 주소 설정 (송신기의 TX_ADDR과 동일해야 수신 가능)
	nRF24L01_WriteReg(NRF24_REG_SETUP_AW, 0x03);     // 주소 길이: 5바이트
	nRF24L01_WriteBuf(NRF24_REG_RX_ADDR_P0, rx_addr, 5);

	// 수신 payload 길이 설정 (예: 5바이트)
	nRF24L01_WriteReg(NRF24_REG_RX_PW_P0, 5);

	// 채널 및 RF 설정
	nRF24L01_WriteReg(NRF24_REG_RF_CH, 76);         // 채널 76 (TX와 동일해야 함)
	nRF24L01_WriteReg(NRF24_REG_RF_SETUP, 0x07);    // 1Mbps, 0dBm

	// FIFO 초기화
	nRF24L01_WriteReg(NRF24_CMD_FLUSH_RX, 0x00);    // RX FIFO 비우기
	nRF24L01_WriteReg(NRF24_REG_STATUS, STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT); // 인터럽트 클리어

	NRF24_CE_HIGH();  // 수신 대기 시작
}

void nRF24L01_Send(uint8_t *payload, uint8_t len)
{
	nRF24L01_WriteReg(NRF24_REG_STATUS,
			STATUS_TX_DS | STATUS_RX_DR | STATUS_MAX_RT);

    nRF24L01_WritePayload(payload, len);
    NRF24_CE_HIGH();
    delay_us(15);  // 최소 10us 이상
    NRF24_CE_LOW();

    nRF24L01_ReadReg(NRF24_REG_STATUS, &status, 1);
}

bool nRF24L01_Read(uint8_t *payload, uint8_t len)
{
    uint8_t status;

    nRF24L01_ReadReg(NRF24_REG_STATUS, &status, 1);  // STATUS 레지스터 읽기

    // RX_DR (데이터 수신됨) 비트가 설정되었는지 확인
    if (status & STATUS_RX_DR)
    {
        uint8_t cmd = NRF24_CMD_R_RX_PAYLOAD;

        NRF24_CSN_LOW();
        HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
        HAL_SPI_Receive(NRF24_SPI, payload, len, 100);
        NRF24_CSN_HIGH();

        // STATUS 클리어 (수신 완료 비트)
        nRF24L01_WriteReg(NRF24_REG_STATUS, STATUS_RX_DR);

        return true;  // 수신 성공
    }

    return false;  // 수신 데이터 없음
}

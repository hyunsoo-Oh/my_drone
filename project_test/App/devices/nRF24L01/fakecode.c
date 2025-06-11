/*
 * fakecode.c
 *
 *  Created on: Jun 11, 2025
 *      Author: USER
 */

#include "fakecode.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"

// 핀 토글 함수
static void nrf24_csn_low(void)  { HAL_GPIO_WritePin(NRF24_CSN_GPIO, NRF24_CSN_PIN, GPIO_PIN_RESET); }
static void nrf24_csn_high(void) { HAL_GPIO_WritePin(NRF24_CSN_GPIO, NRF24_CSN_PIN, GPIO_PIN_SET); }
static void nrf24_ce_low(void)   { HAL_GPIO_WritePin(NRF24_CE_GPIO,  NRF24_CE_PIN,  GPIO_PIN_RESET); }
static void nrf24_ce_high(void)  { HAL_GPIO_WritePin(NRF24_CE_GPIO,  NRF24_CE_PIN,  GPIO_PIN_SET); }

// CSN : SPI 슬레이브선택(데이터시트6.1.4)
// LOW : nRF24L01이 명령 받기 시작, High : 명령종료



// SPI 1바이트 전송
static uint8_t nrf24_spi_rw(uint8_t data)
{
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx, 1, 100);
    return rx;
}

// 레지스터 쓰기/읽기/멀티쓰기
void nrf24_write_reg(uint8_t reg, uint8_t value)
{
    nrf24_csn_low();
    nrf24_spi_rw(0x20 | (reg & 0x1F));
    nrf24_spi_rw(value);
    nrf24_csn_high();
}

uint8_t nrf24_read_reg(uint8_t reg)
{
    uint8_t value;
    nrf24_csn_low();
    nrf24_spi_rw(0x00 | (reg & 0x1F));
    value = nrf24_spi_rw(0xFF);
    nrf24_csn_high();
    return value;
}

void nrf24_write_reg_multi(uint8_t reg, uint8_t *data, uint8_t len)
{
    nrf24_csn_low();
    nrf24_spi_rw(0x20 | (reg & 0x1F));
    for(uint8_t i=0; i<len; i++)
        nrf24_spi_rw(data[i]);
    nrf24_csn_high();
}

// 주소/채널/모드 설정
// 주소/채널/모드 설정
void nrf24_set_tx_addr(uint8_t *addr, uint8_t len)
{
    nrf24_write_reg_multi(0x10, addr, len); // TX_ADDR
    nrf24_write_reg_multi(0x0A, addr, len); // RX_ADDR_P0 (ACK용)
}

void nrf24_set_rx_addr(uint8_t *addr, uint8_t len)
{
    nrf24_write_reg_multi(0x0A, addr, len); // RX_ADDR_P0
}

void nrf24_set_channel(uint8_t ch)
{
    nrf24_write_reg(0x05, ch); // RF_CH
}

void nrf24_tx_mode(void)
{
    nrf24_ce_low();
    nrf24_write_reg(0x00, 0x0E); // CONFIG: PWR_UP=1, PRIM_RX=0(TX), CRC2
    HAL_Delay(2);
}

void nrf24_rx_mode(void)
{
    nrf24_ce_low();
    nrf24_write_reg(0x00, 0x0F); // CONFIG: PWR_UP=1, PRIM_RX=1(RX), CRC2
    HAL_Delay(2);
    nrf24_ce_high(); // 수신 활성화
    HAL_Delay(2);
}

// nRF24L01 기본 세팅
void nrf24_init(void)
{
    nrf24_ce_low();
    nrf24_csn_high();

    HAL_Delay(5);
    nrf24_write_reg(0x01, 0x3F); // EN_AA: Auto ACK
    nrf24_write_reg(0x02, 0x01); // EN_RXADDR: Pipe0 Enable
    nrf24_write_reg(0x03, 0x03); // SETUP_AW: 5bytes
    nrf24_write_reg(0x04, 0x04); // RETR: 1500us, 15 retransmit
    nrf24_write_reg(0x06, 0x07); // RF_SETUP: 1Mbps, 0dBm
    nrf24_write_reg(0x11, 2);    // RX_PW_P0: 페이로드 2바이트(원하면 수정)
    HAL_Delay(2);
}


void nrf24_set_payload_size(uint8_t len)
{
    nrf24_write_reg(0x11, len); // RX_PW_P0 레지스터 0x11
}

// 송신
void nrf24_send_data(uint8_t *data, uint8_t len)
{
    nrf24_tx_mode();

    nrf24_csn_low();
    nrf24_spi_rw(0xA0); // W_TX_PAYLOAD
    for(uint8_t i=0; i<len; i++)
        nrf24_spi_rw(data[i]);
    nrf24_csn_high();

    nrf24_ce_high();
    HAL_Delay(1); // >10us
    nrf24_ce_low();

    nrf24_write_reg(0x07, 0x20); // 송신 완료 플래그 클리어
}

// 수신 준비
uint8_t nrf24_data_ready(void)
{
    uint8_t status = nrf24_read_reg(0x07);
    return status & 0x40; // RX_DR bit (Data Ready)
}

void nrf24_receive(uint8_t *data, uint8_t len)
{
    nrf24_csn_low();
    nrf24_spi_rw(0x61); // R_RX_PAYLOAD
    for(uint8_t i=0; i<len; i++)
        data[i] = nrf24_spi_rw(0xFF);
    nrf24_csn_high();

    nrf24_write_reg(0x07, 0x40); // RX_DR 플래그 클리어
}


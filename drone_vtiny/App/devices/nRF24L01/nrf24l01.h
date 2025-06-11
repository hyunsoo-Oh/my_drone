/*
 * nrf24l01.h
 *
 *  Created on: Jun 9, 2025
 *      Author: USER
 */

#ifndef DEVICES_NRF24L01_NRF24L01_H_
#define DEVICES_NRF24L01_NRF24L01_H_

#include "hw.h"

// Base Register Address (레지스터 맵: datasheet 53~58쪽)
#define NRF24_REG_CONFIG           	0x00  // 기본 설정 (CRC, 전원, 송수신 모드 등)
#define NRF24_REG_EN_AA            	0x01  // 자동 ACK 기능 설정
#define NRF24_REG_EN_RXADDR        	0x02  // 수신 파이프 활성화 설정
#define NRF24_REG_SETUP_AW         	0x03  // 주소 길이 설정 (3~5 byte)
#define NRF24_REG_SETUP_RETR       	0x04  // 자동 재전송 설정
#define NRF24_REG_RF_CH            	0x05  // RF 채널 설정 (2400MHz + value)
#define NRF24_REG_RF_SETUP         	0x06  // 데이터 전송 속도 및 출력 파워 설정
#define NRF24_REG_STATUS           	0x07  // 상태 플래그 (IRQ, 수신 파이프 번호 등)
#define NRF24_REG_OBSERVE_TX       	0x08  // 패킷 손실률/재전송 횟수 통계
#define NRF24_REG_CD               	0x09  // 캐리어 감지 결과 (수신 채널에 신호 감지 여부)

#define NRF24_REG_RX_ADDR_P0       	0x0A  // 데이터 파이프 0의 수신 주소
#define NRF24_REG_RX_ADDR_P1       	0x0B  // 데이터 파이프 1의 수신 주소
#define NRF24_REG_RX_ADDR_P2       	0x0C  // 파이프 2~5는 LSB만 따로 설정
#define NRF24_REG_RX_ADDR_P3       	0x0D
#define NRF24_REG_RX_ADDR_P4       	0x0E
#define NRF24_REG_RX_ADDR_P5       	0x0F

#define NRF24_REG_TX_ADDR          	0x10  // 송신 주소 (TX 패킷의 대상 주소)
#define NRF24_REG_RX_PW_P0         	0x11  // 수신 파이프 0의 페이로드 길이 설정
#define NRF24_REG_RX_PW_P1         	0x12
#define NRF24_REG_RX_PW_P2         	0x13
#define NRF24_REG_RX_PW_P3         	0x14
#define NRF24_REG_RX_PW_P4         	0x15
#define NRF24_REG_RX_PW_P5         	0x16

#define NRF24_REG_FIFO_STATUS      	0x17  // FIFO 상태 (TX/RX 비어있음, 꽉참 등)
#define NRF24_REG_DYNPD            	0x1C  // 동적 페이로드 길이 파이프별 사용 여부
#define NRF24_REG_FEATURE          	0x1D  // 고급 기능 (ACK Payload, NoACK 등) 활성화

// SPI Commands (datasheet 45~46쪽)
#define NRF24_CMD_R_REGISTER       	0x00  // 레지스터 읽기 명령 (0x00 ~ 0x1F)
#define NRF24_CMD_W_REGISTER       	0x20  // 레지스터 쓰기 명령 (0x20 ~ 0x3F)

#define NRF24_CMD_R_RX_PAYLOAD     	0x61  // RX FIFO에서 수신된 페이로드 읽기
#define NRF24_CMD_W_TX_PAYLOAD     	0xA0  // TX FIFO에 송신할 페이로드 쓰기

#define NRF24_CMD_FLUSH_TX         	0xE1  // TX FIFO 비우기 (초기화)
#define NRF24_CMD_FLUSH_RX         	0xE2  // RX FIFO 비우기 (초기화)

#define NRF24_CMD_REUSE_TX_PL      	0xE3  // 이전에 전송한 페이로드 반복 전송

#define NRF24_CMD_ACTIVATE         	0x50  // 고급 기능 사용 활성화 (다음에 0x73 보내야 함)

#define NRF24_CMD_R_RX_PL_WID      	0x60  // 현재 수신 페이로드의 길이 확인
#define NRF24_CMD_W_ACK_PAYLOAD    	0xA8  // ACK 패킷에 실어보낼 페이로드 쓰기 (파이프 번호 포함)
#define NRF24_CMD_W_TX_NOACK       	0xB0  // 해당 TX 패킷을 AutoACK 없이 보냄

#define NRF24_CMD_NOP              	0xFF  // 아무 동작 없음, STATUS 레지스터 반환 용도로 자주 사용

// CONFIG Register 비트 마스크
#define CONFIG_MASK_RX_DR          	(1 << 6)  // RX_DR 인터럽트 마스킹
#define CONFIG_MASK_TX_DS          	(1 << 5)  // TX_DS 인터럽트 마스킹
#define CONFIG_MASK_MAX_RT         	(1 << 4)  // MAX_RT 인터럽트 마스킹

#define CONFIG_EN_CRC              	(1 << 3)  // CRC 사용 여부
#define CONFIG_CRCO                	(1 << 2)  // CRC 길이: 0 = 1 byte, 1 = 2 byte

#define CONFIG_PWR_UP              	(1 << 1)  // 1 = Power Up, 0 = Power Down
#define CONFIG_PRIM_RX             	(1 << 0)  // 1 = PRX 모드, 0 = PTX 모드

// STATUS Register 비트 마스크
#define STATUS_RX_DR               	(1 << 6)  // 수신 완료 인터럽트
#define STATUS_TX_DS               	(1 << 5)  // 송신 완료 인터럽트
#define STATUS_MAX_RT              	(1 << 4)  // 최대 재전송 도달 인터럽트

#define STATUS_RX_P_NO_MASK        	(0x0E)    // 수신된 데이터 파이프 번호 (3비트: [3:1])
#define STATUS_TX_FULL             	(1 << 0)  // TX FIFO가 꽉 찼는지 여부

void nRF24L01_TxInit(void);
void nRF24L01_RxInit(void);

void nRF24L01_Send(uint8_t *payload, uint8_t len);
bool nRF24L01_Read(uint8_t *payload, uint8_t len);

#endif /* DEVICES_NRF24L01_NRF24L01_H_ */

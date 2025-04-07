/*
 * icm_20948.h
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#ifndef HW_INC_ICM_20948_H_
#define HW_INC_ICM_20948_H_

#include "def.h"
#include "bsp/bsp.h"
#include "usart.h"

//#include "icm_20948_offset.h"

#define ICM20948_ADDR  			0x68 << 1

#define USER_BANK_SEL  			0x7F

#define BANK_0         			0x00
#define BANK_1         			0x10
#define BANK_2         			0x20
#define BANK_3         			0x30

#define WHO_AM_I       			0x00  // BANK0

#define LP_CONFIG      			0x05
#define PWR_MGMT_1     			0x06
#define PWR_MGMT_2     			0x07

#define ACCEL_DATA   			0x2D
#define GYRO_DATA    			0x33

#define GYRO_CONFIG_1  			0x01  // BANK2
#define GYRO_CONFIG_2  			0x02
#define ACCEL_CONFIG   			0x14
#define ACCEL_CONFIG_2 			0x15

#define ODR_ALIGN_EN			0x09

#define GYRO_SMPLRT_DIV 	    0x00
#define ACCEL_SMPLRT_DIV_1  	0x10
#define ACCEL_SMPLRT_DIV_2  	0x11

#define MST_ODR_CONFIG			0x00  // BANK3
#define MST_CTRL				0x01
#define MST_DELAY_CTRL			0x02

#define SLV0_ADDR				0x03
#define SLV0_REG				0x04
#define SLV0_CTRL				0x05
#define SLV0_DO					0x06

#define SLV1_ADDR				0x07
#define SLV1_REG				0x08
#define SLV1_CTRL				0x09
#define SLV1_DO					0x10

#define SLV_READ       			0x80
#define SLV_WRITE      			0x00

typedef enum
{
	_250dps,	_500dps,	_1000dps,	_2000dps
} gscale_t;
typedef enum
{
	_2g,	_4g, _8g, _16g
} ascale_t;

typedef enum
{
	_1xg, _2xg, _4xg, _8xg, _16xg, _32xg, _64xg, _128xg
} gyroavg_t;
typedef enum
{
	_1_4xa, _8xa, _16xa, _32xa
} accelavg_t;

// 구조체 Padding을 고려한 구조체 설계
// 자이로스코프 설정 구조체
typedef struct {
	uint8_t dlpf_en;        // DLPF Enable
	uint8_t dlpf_cfg;       // Digital Low-Pass Filter 설정
	uint8_t odr;            // Output Data Rate (Hz)
	uint8_t sensitivity;   // LSB/g (실제 물리량으로 변환 계수)
	int16_t x_data;         // X축 데이터 (GYRO_XOUT_H/L)
	int16_t y_data;         // Y축 데이터 (GYRO_YOUT_H/L)
	int16_t z_data;         // Z축 데이터 (GYRO_ZOUT_H/L)
	gscale_t fs_sel;         // Full-Scale Range 선택
	gyroavg_t sample;		// 센서 데이터 읽는 속도 조절
} GyroConfig;

// 가속도계 설정 구조체
typedef struct {
	uint8_t dlpf_en;        // DLPF Enable
	uint8_t dlpf_cfg;       // Digital Low-Pass Filter 설정
	uint16_t odr;           // Output Data Rate (Hz)
	uint16_t sensitivity;   // LSB/g (실제 물리량으로 변환 계수)
	int16_t x_data;         // X축 데이터 (ACCEL_XOUT_H/L)
	int16_t y_data;         // Y축 데이터 (ACCEL_YOUT_H/L)
	int16_t z_data;         // Z축 데이터 (ACCEL_ZOUT_H/L)
	ascale_t fs_sel;        // Full-Scale Range 선택
	accelavg_t sample;		// 센서 데이터 읽는 속도 조절
} AccelConfig;

//typedef struct {
//
//} SLVConfig;

//int16_t mag_X = 0;
//int16_t mag_Y = 0;
//int16_t mag_Z = 0;

void ICM_Write(uint8_t bank, uint8_t reg, uint8_t data);
void ICM_Read(uint8_t bank, uint8_t reg, uint8_t *data, uint8_t length);
void ICM_SLV_Write(uint8_t slv, uint16_t addr, uint8_t reg, uint8_t data);

void ICM_Init();
void ICM_Reset();
void ICM_READ_WhoAmI();
void ICM_GYRO_Config(GyroConfig *gyro);
void ICM_ACCEL_Config(AccelConfig *accel);
void ICM_SMPLRT_Divide(GyroConfig *gyro, AccelConfig *accel);

void ICM_RAW_GetData(GyroConfig *gyro, AccelConfig *accel);
void ICM_GetScaledData(GyroConfig *gyro, AccelConfig *accel);

void AK09916_MAG_Init();
void BMP280_PRESS_Init();

#endif /* HW_INC_ICM_20948_H_ */

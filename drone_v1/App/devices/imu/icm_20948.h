/*
 * icm_20948.h
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#ifndef HW_INC_ICM_20948_H_
#define HW_INC_ICM_20948_H_

#include "i2c_util.h"
#include "def.h"
#include "usart.h"

#include "ak09916.h"

#define ICM20948_ADDR  			0x68 << 1

#define USER_BANK_SEL  			0x7F

#define BANK_0         			0x00
#define BANK_1         			0x10
#define BANK_2         			0x20
#define BANK_3         			0x30

#define WHO_AM_I       			0x00  // BANK 0

#define LP_CONFIG      			0x05
#define PWR_MGMT_1     			0x06
#define PWR_MGMT_2     			0x07

#define ACCEL_DATA   			0x2D
#define GYRO_DATA    			0x33

#define INT_PIN_CFG				0x0F
#define USER_CTRL				0x03

#define SLV_SENS_DATA_00		0x3B
#define SLV_SENS_DATA_01		0x3C
#define SLV_SENS_DATA_02		0x3D
#define SLV_SENS_DATA_03		0x3E
#define SLV_SENS_DATA_04		0x3F
#define SLV_SENS_DATA_05		0x40
#define SLV_SENS_DATA_06		0x41
#define SLV_SENS_DATA_07		0x42
#define SLV_SENS_DATA_08		0x43


#define GYRO_CONFIG_1  			0x01  // BANK 2
#define GYRO_CONFIG_2  			0x02
#define ACCEL_CONFIG   			0x14
#define ACCEL_CONFIG_2 			0x15

#define ODR_ALIGN_EN			0x09

#define GYRO_SMPLRT_DIV 	    0x00
#define ACCEL_SMPLRT_DIV_1  	0x10
#define ACCEL_SMPLRT_DIV_2  	0x11

#define MST_ODR_CONFIG			0x00  // BANK 3
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

// Self-Test 관련 레지스터
#define SELF_TEST_GYRO   		0x02 	// BANK 1, Factory Trim value
#define SELF_TEST_ACCEL  		0x0E

#define GYRO_ST_DISEN     		0x00
#define ACCEL_ST_DISEN    		0x00

#define GYRO_ST_EN     			0x38
#define ACCEL_ST_EN    			0x1C

#define FACTORY_TRIM_SCALE  	2620.0f

// Accel의 Bias 제거를 위한 Offset
#define XA_OFFS_H		0x14	// BANK 1
#define XA_OFFS_L		0x15
#define YA_OFFS_H		0x17
#define YA_OFFS_L		0x18
#define ZA_OFFS_H		0x1A
#define ZA_OFFS_L		0x1B

// Accel의 Bias 제거를 위한 Offset
#define XG_OFFS_USRH	0x03	// BANK 2
#define XG_OFFS_USRL	0x04
#define YG_OFFS_USRH	0x05
#define YG_OFFS_USRL	0x06
#define ZG_OFFS_USRH	0x07
#define ZG_OFFS_USRL	0x08

typedef enum
{
	_250dps,	_500dps,	_1000dps,	_2000dps
} gscale_t;
typedef enum
{
	_2g,	_4g, 	_8g, 	_16g
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
	uint8_t sensitivity;   	// LSB/g (실제 물리량으로 변환 계수)
	uint16_t odr;           // Output Data Rate (Hz)
	int16_t x_data;         // X축 데이터 (GYRO_XOUT_H/L)
	int16_t y_data;         // Y축 데이터 (GYRO_YOUT_H/L)
	int16_t z_data;         // Z축 데이터 (GYRO_ZOUT_H/L)
	gscale_t fs_sel;        // Full-Scale Range 선택
	gyroavg_t sample;		// 센서 데이터 읽는 속도 조절
} GyroConfig;

// 보정된 자이로스코프 데이터
typedef struct {
	float x_dps;         // X축 데이터
	float y_dps;         // Y축 데이터
	float z_dps;         // Z축 데이터
} GyroData;

// 가속도계 설정 구조체
typedef struct {
	uint8_t dlpf_en;        // DLPF Enable
	uint8_t dlpf_cfg;       // Digital Low-Pass Filter 설정
	uint16_t sensitivity;   // LSB/g (실제 물리량으로 변환 계수)
	uint16_t odr;           // Output Data Rate (Hz)
	int16_t x_data;         // X축 데이터 (ACCEL_XOUT_H/L)
	int16_t y_data;         // Y축 데이터 (ACCEL_YOUT_H/L)
	int16_t z_data;         // Z축 데이터 (ACCEL_ZOUT_H/L)
	ascale_t fs_sel;        // Full-Scale Range 선택
	accelavg_t sample;		// 센서 데이터 읽는 속도 조절
} AccelConfig;

// 보정된 자이로스코프 데이터
typedef struct {
	float x_g;         // X축 데이터
	float y_g;         // Y축 데이터
	float z_g;         // Z축 데이터
} AccelData;

typedef struct {
	int16_t x_data;         // X축 데이터 (ACCEL_XOUT_H/L)
	int16_t y_data;         // Y축 데이터 (ACCEL_YOUT_H/L)
	int16_t z_data;         // Z축 데이터 (ACCEL_ZOUT_H/L)
} MagConfig;

typedef struct {
	float x_data;         // X축 데이터 (ACCEL_XOUT_H/L)
	float y_data;         // Y축 데이터 (ACCEL_YOUT_H/L)
	float z_data;         // Z축 데이터 (ACCEL_ZOUT_H/L)
} MagData;

typedef struct {
	float baseline_gyro[3];
	float baseline_accel[3];
	float selftest_gyro[3];
	float selftest_accel[3];
} SelfTest;

typedef struct {
	int16_t x_gyro;
	int16_t y_gyro;
	int16_t z_gyro;
	int16_t x_accel;
	int16_t y_accel;
	int16_t z_accel;
} IcmOffset;

void ICM_Write(uint8_t bank, uint8_t reg, uint8_t data);
void ICM_Read(uint8_t bank, uint8_t reg, uint8_t *data, uint8_t length);
void ICM_SLV_Write(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t data);
void ICM_SLV_Read(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t *data);
void ICM_SLV_Ctrl(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t length);

void ICM_Init();
void ICM_Reset();
void ICM_READ_WhoAmI();
void ICM_GYRO_Config(GyroConfig *gyro);
void ICM_ACCEL_Config(AccelConfig *accel);
void ICM_SMPLRT_Divide(GyroConfig *gyro, AccelConfig *accel);

void ICM_RAW_GetData(GyroConfig *gyro, AccelConfig *accel);
void ICM_GetScaledData(GyroConfig *gyro, AccelConfig *accel);
void ICM_Calibrate(GyroConfig gyro, AccelConfig accel, IcmOffset *offset);
void ICM_REMOVE_Offset(GyroConfig *gyro_config, AccelConfig *accel_config, IcmOffset *offset);

void ICM_SLV_Init();
void ICM_SLV_WhoAmI(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t id);
void AK09916_MAG_Init();
void AK09916_RAW_GetData(MagConfig *mag);

// Offset 관련 함수
void ICM_SetGyroOffset(int16_t offset_x, int16_t offset_y, int16_t offset_z);
void ICM_SetAccelOffset(int16_t offset_x, int16_t offset_y, int16_t offset_z);
void ICM_ClearOffsetRegisters();


#endif /* HW_INC_ICM_20948_H_ */

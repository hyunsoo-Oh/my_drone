/*
 * gy-521.h
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#ifndef HW_GY521_GY_521_H_
#define HW_GY521_GY_521_H_

#include "i2c_util.h"
#include "usart.h"

// -----------------------------------------------------------------------------
// 기본 정보
// -----------------------------------------------------------------------------
#define GY521_ADDRESS            0x68 << 1  // AD0 == LOW ? 0x68 : 0x69
#define GY521_WHO_AM_I           0x75    	// WHO_AM_I: 0x68 expected

// -----------------------------------------------------------------------------
// 전원 제어 (Power Management)
// -----------------------------------------------------------------------------
#define GY521_PWR_MGMT_1         0x6B    // Sleep mode, clock source 선택
#define GY521_PWR_MGMT_2         0x6C    // 센서별 standby 설정

// -----------------------------------------------------------------------------
// 센서 리셋 제어 (Signal Path Reset & User Control)
// -----------------------------------------------------------------------------
#define GY521_SIGNAL_PATH_RESET  0x68    // TEMP_RESET, ACCEL_RESET, GYRO_RESET
#define GY521_USER_CTRL          0x6A    // FIFO/I2C master/reset 제어

// -----------------------------------------------------------------------------
// 센서 설정 (Configuration Registers)
// -----------------------------------------------------------------------------
#define GY521_SMPLRT_DIV         0x19    // 샘플링 분주 설정
#define GY521_CONFIG             0x1A    // DLPF 설정 등
#define GY521_GYRO_CONFIG        0x1B    // Gyro Full Scale 설정
#define GY521_ACCEL_CONFIG       0x1C    // Accel Full Scale 설정

// -----------------------------------------------------------------------------
// 센서 출력 데이터 (Sensor Output Registers)
// -----------------------------------------------------------------------------
#define GY521_ACCEL_XOUT_H       0x3B
#define GY521_ACCEL_XOUT_L       0x3C
#define GY521_ACCEL_YOUT_H       0x3D
#define GY521_ACCEL_YOUT_L       0x3E
#define GY521_ACCEL_ZOUT_H       0x3F
#define GY521_ACCEL_ZOUT_L       0x40

#define GY521_TEMP_OUT_H         0x41
#define GY521_TEMP_OUT_L         0x42

#define GY521_GYRO_XOUT_H        0x43
#define GY521_GYRO_XOUT_L        0x44
#define GY521_GYRO_YOUT_H        0x45
#define GY521_GYRO_YOUT_L        0x46
#define GY521_GYRO_ZOUT_H        0x47
#define GY521_GYRO_ZOUT_L        0x48

// -----------------------------------------------------------------------------
// I2C 마스터 제어 (Master Mode to read external sensors)
// -----------------------------------------------------------------------------
#define GY521_I2C_MST_CTRL       0x24  // I2C 마스터 모드 설정 (클럭 속도, 멀티 마스터, 슬레이브 레지스터 지연 설정 등)

// I2C Slave 0 (외부 장치와의 인터페이스 설정)
#define GY521_I2C_SLV0_ADDR      0x25  // I2C 슬레이브 0 주소 (Read/Write 비트 포함)
#define GY521_I2C_SLV0_REG       0x26  // 슬레이브 장치에서 읽거나 쓸 레지스터 주소
#define GY521_I2C_SLV0_CTRL      0x27  // 슬레이브 0 제어: 읽기 길이, enable, 반복 여부 등 설정
#define GY521_I2C_SLV0_DO        0x63  // 마스터가 슬레이브로 전송할 데이터(Byte 1)

// I2C Master Delay Control
#define GY521_I2C_MST_DELAY_CTRL 0x67  // 각 I2C 슬레이브에 대한 전송 지연 삽입 여부 설정 (Slave0~3용 delay enable 비트)

// -----------------------------------------------------------------------------
// 외부 센서 데이터 버퍼 (External Sensor Data Registers)
// -----------------------------------------------------------------------------
#define GY521_EXT_SENS_DATA_0    0x49
#define GY521_EXT_SENS_DATA_1    0x4A
#define GY521_EXT_SENS_DATA_2    0x4B

// -----------------------------------------------------------------------------
// 인터럽트 제어 (Interrupt Control)
// -----------------------------------------------------------------------------
#define GY521_INT_PIN_CFG        0x37
#define GY521_INT_ENABLE         0x38
#define GY521_INT_STATUS         0x3A

// -----------------------------------------------------------------------------
// FIFO 제어 (FIFO Control Registers)
// -----------------------------------------------------------------------------
#define GY521_FIFO_EN            0x23
#define GY521_FIFO_COUNTH        0x72
#define GY521_FIFO_COUNTL        0x73
#define GY521_FIFO_R_W           0x74

typedef struct {
    uint8_t dlpf_cfg;       // CONFIG 레지스터 (0x1A)
    uint8_t gyro_fs_sel;    // GYRO_CONFIG 레지스터 (0x1B)
    uint8_t accel_fs_sel;   // ACCEL_CONFIG 레지스터 (0x1C)
    uint8_t smplrt_div;     // SMPLRT_DIV (0x19)
} GY521_Config_t;

typedef struct {
	float ax, ay, az;
	float gx, gy, gz;
	float temp;
} GY521_Data_t;

// 초기화
void GY521_Init();

// 데이터 읽기
void GY521_READ_RawAccel(int16_t ac_data[3]);
void GY521_READ_RawGyro(int16_t gy_data[3]);
void GY521_READ_RawTemp(int16_t *te_data);

// 오프셋 보정된 데이터 읽기 함수
void GY521_ReadAccel(int16_t ac_data[3]);    // 오프셋 보정된 가속도
void GY521_ReadGyro(int16_t gy_data[3]);     // 오프셋 보정된 자이로
void GY521_ReadTemp(int16_t *te_data);       // 온도 (보정 불필요)

// 오차 제거
void GY521_Calibrate(void);
void GY521_OffsetMeasure(void);

// 실제 물리 단위로 변환하는 함수
void GY521_GetAccelG(float accel_g[3]);      // 가속도를 g 단위로
void GY521_GetGyroDPS(float gyro_dps[3]);    // 자이로를 °/s 단위로
float GY521_GetTempCelsius();                // 온도를 섭씨로

// 내부 제어

#endif /* HW_GY521_GY_521_H_ */

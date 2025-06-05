/*
 * ap.c
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#include "ap.h"

//int16_t accel[3], gyro[3];
float accel[3], gyro[3];
float temp;
char msg[128];
uint8_t test;

void apInit(void)
{
//	GY521_Init();

//    HAL_UART_Transmit(&huart2, (uint8_t *)"🔍 I2C 스캔 시작...\r\n", sizeof("🔍 I2C 스캔 시작...\r\n") - 1, 100);
//
//    for (uint8_t addr = 1; addr < 127; addr++)
//    {
//        // 7비트 주소를 왼쪽으로 1비트 시프트 (HAL은 8비트 주소 사용)
//        if (HAL_I2C_IsDeviceReady(&hi2c1, (addr << 1), 1, 10) == HAL_OK)
//        {
//            snprintf(msg, sizeof(msg), "✅ I2C 기기 발견: 0x%02X\r\n", addr);
//            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
//        }
//    }
//
//    HAL_UART_Transmit(&huart2, (uint8_t *)"✅ 스캔 완료.\r\n", sizeof("✅ 스캔 완료.\r\n") - 1, 100);
//
//    I2C_Read(MPU6500_I2C_ADDR, MPU6500_WHO_AM_I, &test, 1);
//    if (test == 0x70)
//    	 HAL_UART_Transmit(&huart2, (uint8_t *)"MPU6500\r\n", sizeof("MPU6500\r\n") - 1, 100);
//    else
//    	HAL_UART_Transmit(&huart2, (uint8_t *)"Failed\r\n", sizeof("Failed\r\n") - 1, 100);

	MPU6500_Init();
}
void apMain(void)
{
	while (1)
	{
//		MPU6500_ReadAccel(accel);
//		MPU6500_ReadGyro(gyro);
//		temp = MPU6500_ReadTemperature();
//
//		snprintf(msg, sizeof(msg),
//			"ACC: X=%d Y=%d Z=%d | GYRO: X=%d Y=%d Z=%d | TEMP=%.2f\r\n",
//			 accel[0], accel[1], accel[2],
//			 gyro[0], gyro[1], gyro[2],
//			 temp
//		);
//
//		HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

//		MPU6500_OffsetMeasure();

		MPU6500_ReadAccelScaled(accel);
		MPU6500_ReadGyroScaled(gyro);
		temp = MPU6500_ReadTemperature();

		snprintf(msg, sizeof(msg),
			 "ACC: X=%.2f Y=%.2f Z=%.2f | GYRO: X=%.2f Y=%.2f Z=%.2f | TEMP=%.2f\r\n",
			 accel[0], accel[1], accel[2],
			 gyro[0], gyro[1], gyro[2],
			 temp
		);

		HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
	}
}

// GY-521 (MPU-6050 센서)
//		GY521_ReadAccel(accel);
//		GY521_ReadGyro(gyro);
//		GY521_ReadTemp(&temp);
//
//		snprintf(msg, sizeof(msg),
//			 "ACC: X=%d Y=%d Z=%d | GYRO: X=%d Y=%d Z=%d | TEMP=%.2f\r\n",
//			 accel[0], accel[1], accel[2],
//			 gyro[0], gyro[1], gyro[2],
//			 temp / 340.0f + 36.53f
//		);
//
//		HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

//		GY521_OffsetMeasure();

//		GY521_GetAccelG(accel);
//		GY521_GetGyroDPS(gyro);
//		temp = GY521_GetTempCelsius();
//
//		snprintf(msg, sizeof(msg),
//			 "ACC: X=%.2f Y=%.2f Z=%.2f | GYRO: X=%.2f Y=%.2f Z=%.2f | TEMP=%.2f\r\n",
//			 accel[0], accel[1], accel[2],
//			 gyro[0], gyro[1], gyro[2],
//			 temp
//		);
//
//		HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

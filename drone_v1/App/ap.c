/*
 * ap.c
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#include "ap.h"

extern GyroConfig gyro;
extern AccelConfig accel;

void apInit()
{
	printf("Starting ICM-20948 \n\r");
	// 센서 초기화
	ICM_Init(gyro, accel);

	// Self-Test 수행
	ICM_SELF_TEST_Init();
}

void apMain()
{
	while(1)
	{
		ICM_RAW_GetData(&gyro, &accel);
		HAL_Delay(500);
	}
}

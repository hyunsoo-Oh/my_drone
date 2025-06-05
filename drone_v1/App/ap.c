/*
 * ap.c
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#include "ap.h"

extern GyroConfig gyro;
extern AccelConfig accel;
extern MagData mag;

void apInit()
{
	printf("Starting ICM-20948 \n\r");
	// 센서 초기화
	ICM_Init(&gyro, &accel);

}

void apMain()
{
	while(1)
	{
		ICM_GetScaledData(&gyro, &accel);
//		AK09916_RAW_GetData(&mag);
		HAL_Delay(500);
	}
}

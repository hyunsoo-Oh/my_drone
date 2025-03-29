/*
 * icm_20948_test.h
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#ifndef HW_INC_ICM_20948_TEST_H_
#define HW_INC_ICM_20948_TEST_H_

#include "icm_20948.h"

#define ACCEL_DATA   			0x2D	// BANK0
#define GYRO_DATA    			0x33

#define SELF_TEST_GYRO   	0x02 	// BANK1, Factory Trim value
#define SELF_TEST_ACCEL  	0x0E

#define GYRO_CONFIG_2  		0x02  // BANK2
#define ACCEL_CONFIG_2 		0x15

#define GYRO_ST_DISEN     0x00
#define ACCEL_ST_DISEN    0x00

#define GYRO_ST_EN     		0x38
#define ACCEL_ST_EN    		0x1C

#define FACTORY_TRIM_SCALE  2620.0f

typedef struct {
	float baseline_gyro[3];
	float baseline_accel[3];
	float selftest_gyro[3];
	float selftest_accel[3];
} SelfTest;

float CALCULATE_FACTORY_Trim(uint8_t self_test_val);

void ICM_GET_SENSOR_DATA_average(float *gyro, float *accel, uint8_t samples);

void ICM_SELF_TEST_Init();

#endif /* HW_INC_ICM_20948_TEST_H_ */

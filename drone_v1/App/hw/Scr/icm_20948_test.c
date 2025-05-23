/*
 * icm_20948_test.c
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#include "Inc/icm_20948.h"

SelfTest st_data;

float CALCULATE_FACTORY_Trim(uint8_t self_test_val) {
	if (self_test_val == 0) return 0.0f; // 0이면 나누기 오류 방지
	return FACTORY_TRIM_SCALE * (1.0f - powf(1.0f + self_test_val / 32.0f, -1.0f));
}

void ICM_GET_SENSOR_DATA_average(float *gyro, float *accel, uint8_t samples)
{
	uint8_t data[12];
	float sum_gyro[3] = {0}, sum_accel[3] = {0};

	for (uint8_t i = 0; i < samples; i++)
	{
		ICM_Read(BANK_0, ACCEL_DATA, data, 12);

		sum_accel[0] = sum_accel[0] + (int16_t)(data[0] << 8 | data[1]);
		sum_accel[1] = sum_accel[1] + (int16_t)(data[2] << 8 | data[3]);
		sum_accel[2] = sum_accel[2] + (int16_t)(data[4] << 8 | data[5]);
		sum_gyro[0] = sum_gyro[0] + (int16_t)(data[6] << 8 | data[7]);
		sum_gyro[1] = sum_gyro[1] + (int16_t)(data[8] << 8 | data[9]);
		sum_gyro[2] = sum_gyro[2] + (int16_t)(data[10] << 8 | data[11]);
	}
	for (uint8_t i = 0; i < 3; i++)
	{
		gyro[i] = sum_gyro[i] / samples;
		accel[i] = sum_accel[i] / samples;
	}
}

void ICM_SELF_TEST_Init()
{
	uint8_t self_test_vals[6];
	float gyro_offset[3], accel_offset[3];

	ICM_Write(BANK_2, GYRO_CONFIG_2, GYRO_ST_DISEN);
	ICM_Write(BANK_2, ACCEL_CONFIG_2, ACCEL_ST_DISEN);

	ICM_GET_SENSOR_DATA_average(st_data.baseline_gyro, st_data.baseline_accel, 200);

	ICM_Write(BANK_2, GYRO_CONFIG_2, GYRO_ST_EN);
	ICM_Write(BANK_2, ACCEL_CONFIG_2, ACCEL_ST_EN);

	HAL_Delay(20);

	ICM_GET_SENSOR_DATA_average(st_data.selftest_gyro, st_data.selftest_accel, 200);

	ICM_Read(BANK_1, SELF_TEST_GYRO, self_test_vals, 3);
	ICM_Read(BANK_1, SELF_TEST_ACCEL, &self_test_vals[3], 3);

	for (int i = 0; i < 3; i++)
	{
		float factory_trim_gyro = CALCULATE_FACTORY_Trim(self_test_vals[i]);
		float factory_trim_accel = CALCULATE_FACTORY_Trim(self_test_vals[i + 3]);

		float gyro_response = 100.0f *
				(st_data.selftest_gyro[i] - st_data.baseline_gyro[i]) / factory_trim_gyro;
		float accel_response = 100.0f *
				(st_data.selftest_accel[i] - st_data.baseline_accel[i]) / factory_trim_accel;

		printf("Gyro[%d] Self-Test Response: %.2f%% -> %s\n", i, gyro_response,
						(gyro_response >= 50.0f && gyro_response <= 150.0f) ? "PASS" : "FAIL");
		printf("Accel[%d] Self-Test Response: %.2f%% -> %s\n", i, accel_response,
						(accel_response >= 50.0f && accel_response <= 150.0f) ? "PASS" : "FAIL");
	}
	// Offset 계싼
	for (int i = 0; i < 3; i++)
	{
		gyro_offset[i] = -st_data.baseline_gyro[i]; // 자이로스코프 오프셋 (LSB)
		if (i < 2) {
			accel_offset[i] = -st_data.baseline_accel[i]; // x, y축 오프셋
		} else {
			accel_offset[i] = -(st_data.baseline_accel[i] - 16384); // z축 오프셋 (중력 보정)
		}
		printf("Gyro[%d] Offset: %.2f LSB\n", i, gyro_offset[i]);
		printf("Accel[%d] Offset: %.2f LSB\n", i, accel_offset[i]);
	}
	ICM_Write(BANK_2, GYRO_CONFIG_2, GYRO_ST_DISEN);
	ICM_Write(BANK_2, ACCEL_CONFIG_2, ACCEL_ST_DISEN);
}

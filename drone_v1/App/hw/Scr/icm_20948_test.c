/*
 * icm_20948_test.c
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#include "Inc/icm_20948_test.h"

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
		I2C_Read(ICM20948_ADDR, ACCEL_DATA, data, 12);

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

	ICM_ChangeBank(BANK_2);
	I2C_Write(ICM20948_ADDR, GYRO_CONFIG_2, GYRO_ST_DISEN);
	I2C_Write(ICM20948_ADDR, ACCEL_CONFIG_2, ACCEL_ST_DISEN);

	ICM_ChangeBank(BANK_0);
	ICM_GET_SENSOR_DATA_average(st_data.baseline_gyro, st_data.baseline_accel, 200);

	ICM_ChangeBank(BANK_2);
	I2C_Write(ICM20948_ADDR, GYRO_CONFIG_2, GYRO_ST_EN);
	I2C_Write(ICM20948_ADDR, ACCEL_CONFIG_2, ACCEL_ST_EN);

	HAL_Delay(20);

	ICM_ChangeBank(BANK_0);
	ICM_GET_SENSOR_DATA_average(st_data.selftest_gyro, st_data.selftest_accel, 200);

  ICM_ChangeBank(BANK_1);
	I2C_Read(ICM20948_ADDR, SELF_TEST_GYRO, self_test_vals, 3);
	I2C_Read(ICM20948_ADDR, SELF_TEST_ACCEL, &self_test_vals[3], 3);

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
	ICM_ChangeBank(BANK_2);
	I2C_Write(ICM20948_ADDR, GYRO_CONFIG_2, GYRO_ST_DISEN);
	I2C_Write(ICM20948_ADDR, ACCEL_CONFIG_2, ACCEL_ST_DISEN);
}

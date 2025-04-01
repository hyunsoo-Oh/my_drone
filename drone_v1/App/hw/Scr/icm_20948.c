/*
 * icm_20948.c
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#include "Inc/icm_20948.h"

uint8_t imu_data[22];

GyroConfig gyro = {
	.fs_sel = _250dps,
	.dlpf_en = 1,
	.dlpf_cfg = 0,
	.sample = _1xg,
	.odr = 100
};
AccelConfig accel = {
	.fs_sel = _2g,
	.dlpf_en = 1,
	.dlpf_cfg = 0,
	.sample = _1_4xa,
	.odr = 100
};

ICM_Init(gyro, accel);


void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
	uint8_t buff = data;
	HAL_I2C_Mem_Write(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT,
											&buff, 1, 100);
}
void I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length)
{
	HAL_I2C_Mem_Read(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT,
											data, length, 100);
}

void ICM_Write(uint8_t bank, uint8_t reg, uint8_t data)
{
	I2C_Write(ICM20948_ADDR, USER_BANK_SEL, bank);
	I2C_Write(ICM20948_ADDR, reg, data);
}
void ICM_Read(uint8_t bank, uint8_t reg, uint8_t *data, uint8_t length)
{
	I2C_Write(ICM20948_ADDR, USER_BANK_SEL, bank);
	I2C_Read(ICM20948_ADDR, reg, data, length);
}

void ICM_SlaveWrite(uint16_t addr, uint8_t reg, uint8_t data)
{
	uint8_t buff = data;
	HAL_I2C_Mem_Write(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT,
											&buff, 1, 100);
}

void ICM_Init(GyroConfig gyro, AccelConfig accel)
{
	ICM_Reset();
	HAL_Delay(100);

	ICM_WakeUp();

	ICM_GYRO_Config(&gyro);
	ICM_ACCEL_Config(&accel);

	ICM_SMPLRT_Divide(&gyro, &accel);
	ICM_READ_WhoAmI();
}
void ICM_Reset()
{
	ICM_Write(BANK_0, PWR_MGMT_1, 0x80);
}
void ICM_READ_WhoAmI()
{
	uint8_t responseAddr = 0;
	ICM_Read(BANK_0, WHO_AM_I, &responseAddr, 1);
	if (responseAddr == 0xEA)  printf("OK \n\r");
	else  printf("Don't find ICM");
}
void ICM_WakeUp()
{
	ICM_Write(BANK_0, PWR_MGMT_1, 0x01);
	ICM_Write(BANK_0, PWR_MGMT_2, 0x00);
	ICM_Write(BANK_0, LP_CONFIG, 0x00);

	ICM_Write(BANK_2, ODR_ALIGN_EN, 0x01);
}
void ICM_GYRO_Config(GyroConfig *gyro)
{
	uint8_t config = gyro->dlpf_cfg << 3 | gyro->fs_sel << 1 | gyro->dlpf_en;
	uint8_t sampleAvg = gyro->sample;

	ICM_Write(BANK_2, GYRO_CONFIG_1, config);

	ICM_Write(BANK_2, GYRO_CONFIG_2, sampleAvg);

	switch (gyro->fs_sel) {
		case _250dps:  gyro->sensitivity = 131;   break;
		case _500dps:  gyro->sensitivity = 65;    break;
		case _1000dps: gyro->sensitivity = 32;    break;
		case _2000dps: gyro->sensitivity = 16;    break;
  }
}
void ICM_ACCEL_Config(AccelConfig *accel)
{
	uint8_t config = accel->dlpf_cfg << 3 | accel->fs_sel << 1 | accel->dlpf_en;
	uint8_t sampleAvg = accel->sample;

	ICM_Write(BANK_2, ACCEL_CONFIG, config);

	ICM_Write(BANK_2, ACCEL_CONFIG_2, sampleAvg);

	switch (accel->fs_sel) {
		case _2g:  accel->sensitivity = 16384; break;
		case _4g:  accel->sensitivity = 8192;  break;
		case _8g:  accel->sensitivity = 4096;  break;
		case _16g: accel->sensitivity = 2048;  break;
  }
}
void ICM_SMPLRT_Divide(GyroConfig *gyro, AccelConfig *accel)
{
	ICM_Write(BANK_2, GYRO_SMPLRT_DIV, gyro->odr);

	ICM_Write(BANK_2, ACCEL_SMPLRT_DIV_1, (accel->odr >> 8) & 0x0F);
	ICM_Write(BANK_2, ACCEL_SMPLRT_DIV_2, accel->odr);
}

void ICM_RAW_GetData(GyroConfig *gyro, AccelConfig *accel)
{
	imu_data = ICM_Read(BANK_0, ACCEL_DATA, data, 14);

	accel->x_data = (int16_t)(data[0] << 8 | data[1]);
	accel->y_data = (int16_t)(data[2] << 8 | data[3]);
	accel->z_data = (int16_t)(data[4] << 8 | data[5]);
	gyro->x_data = (int16_t)(data[8] << 8 | data[9]);
	gyro->y_data = (int16_t)(data[10] << 8 | data[11]);
	gyro->z_data = (int16_t)(data[12] << 8 | data[13]);

	printf("Gyro X: %d, Y: %d, Z: %d\n", gyro->x_data, gyro->y_data, gyro->z_data);
	printf("Accel X: %d, Y: %d, Z: %d\n", accel->x_data, accel->y_data, accel->z_data);
}
void ICM_CLEANING_GetData(GyroConfig *gyro, AccelConfig *accel)
{
	float gyro_x_dps = (float)gyro->x_data / gyro->sensitivity;
	float gyro_y_dps = (float)gyro->y_data / gyro->sensitivity;
	float gyro_z_dps = (float)gyro->z_data / gyro->sensitivity;

	float accel_x_g = (float)accel->x_data / accel->sensitivity;
	float accel_y_g = (float)accel->y_data / accel->sensitivity;
	float accel_z_g = (float)accel->z_data / accel->sensitivity;

	printf("Gyro (dps) - X: %.2f, Y: %.2f, Z: %.2f\n", gyro_x_dps, gyro_y_dps, gyro_z_dps);
	printf("Accel (g) - X: %.2f, Y: %.2f, Z: %.2f\n", accel_x_g, accel_y_g, accel_z_g);
}

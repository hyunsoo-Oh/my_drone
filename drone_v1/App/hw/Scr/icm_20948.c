/*
 * icm_20948.c
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#include "Inc/icm_20948.h"

uint8_t imu_data[22];

GyroConfig gyro;
AccelConfig accel;


void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
	uint8_t buff = data;
	HAL_I2C_Mem_Write(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT,
											&buff, 1, HAL_MAX_DELAY);
}
void I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length)
{
	HAL_I2C_Mem_Read(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT,
											data, length, HAL_MAX_DELAY);
}
void ICM_SlaveWrite(uint16_t addr, uint8_t reg, uint8_t data)
{
	uint8_t buff = data;
	HAL_I2C_Mem_Write(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT,
											&buff, 1, HAL_MAX_DELAY);
}

void ICM_Init()
{
	ICM_Reset();
	HAL_Delay(100);

	ICM_ChangeBank(BANK_0);
	ICM_WakeUp();

	ICM_ChangeBank(BANK_2);
	ICM_GYRO_Config(&gyro, _4xg);
	ICM_ACCEL_Config(&accel, _1_4xa);

	ICM_SMPLRT_Divide(&gyro, &accel);
	ICM_READ_WhoAmI();
}
void ICM_Reset()
{
	I2C_Write(ICM20948_ADDR, PWR_MGMT_1, 0x80);
}
void ICM_READ_WhoAmI()
{
	uint8_t responseAddr = 0;
	I2C_Read(ICM20948_ADDR, WHO_AM_I, &responseAddr, 1);
	if (responseAddr == 0xEA)  printf("OK \n\r");
	else  printf("Don't find ICM");
}
void ICM_WakeUp()
{
  I2C_Write(ICM20948_ADDR, PWR_MGMT_1, 0x01);

  I2C_Write(ICM20948_ADDR, PWR_MGMT_2, 0x00);

	I2C_Write(ICM20948_ADDR, LP_CONFIG, 0x00);
}
void ICM_ChangeBank(uint8_t bank)
{
	I2C_Write(ICM20948_ADDR, USER_BANK_SEL, bank);
}
void ICM_GYRO_Config(GyroConfig *gyro, gyroavg_t avg)
{
	uint8_t config = gyro->dlpf_cfg << 3 | gyro->fs_sel << 1 | gyro->dlpf_en;
	I2C_Write(ICM20948_ADDR, GYRO_CONFIG_1, config);

	I2C_Write(ICM20948_ADDR, GYRO_CONFIG_2, (uint8_t)avg);

	switch (gyro->fs_sel) {
    case _250dps:  gyro->sensitivity = 131;   break;
    case _500dps:  gyro->sensitivity = 65;    break;
    case _1000dps: gyro->sensitivity = 32;    break;
    case _2000dps: gyro->sensitivity = 16;    break;
  }
}
void ICM_ACCEL_Config(AccelConfig *accel, accelavg_t avg)
{
	uint8_t config = accel->dlpf_cfg << 3 | accel->fs_sel << 1 | accel->dlpf_en;
	I2C_Write(ICM20948_ADDR, ACCEL_CONFIG, config);

	I2C_Write(ICM20948_ADDR, ACCEL_CONFIG_2, (uint8_t)avg);

	switch (accel->fs_sel) {
    case _2g:  accel->sensitivity = 16384; break;
    case _4g:  accel->sensitivity = 8192;  break;
    case _8g:  accel->sensitivity = 4096;  break;
    case _16g: accel->sensitivity = 2048;  break;
  }
}
void ICM_SMPLRT_Divide(GyroConfig *gyro, AccelConfig *accel)
{
	I2C_Write(ICM20948_ADDR, USER_BANK_SEL, BANK_2);
	I2C_Write(ICM20948_ADDR, GYRO_SMPLRT_DIV, gyro->odr);
	I2C_Write(ICM20948_ADDR, ACCEL_SMPLRT_DIV_1, (accel->odr >> 8) & 0x0F);
	I2C_Write(ICM20948_ADDR, ACCEL_SMPLRT_DIV_2, accel->odr);
}

void ICM_RAW_GetData(GyroConfig *gyro, AccelConfig *accel)
{
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

/*
 * icm_20948.c
 *
 *  Created on: Mar 29, 2025
 *      Author: USER
 */

#include "Inc/icm_20948.h"

GyroConfig gyro = {
	.dlpf_en = 1,
	.dlpf_cfg = 0,
	.odr = 250,
	.sensitivity = 0,
	.x_data = 0,
	.y_data = 0,
	.z_data = 0,
	.fs_sel = _250dps,
	.sample = _1xg
};

AccelConfig accel = {
	.dlpf_en = 1,
	.dlpf_cfg = 0,
	.odr = 250,
	.sensitivity = 0,
	.x_data = 0,
	.y_data = 0,
	.z_data = 0,
	.fs_sel = _2g,
	.sample = _1_4xa
};

MagData mag = {
	.x_data = 0,
	.y_data = 0,
	.z_data = 0
};

extern IcmOffset offset;

// BANK_n의 reg 주소에 데이터 쓰기
void ICM_Write(uint8_t bank, uint8_t reg, uint8_t data)
{
	I2C_Write(ICM20948_ADDR, USER_BANK_SEL, bank);
	I2C_Write(ICM20948_ADDR, reg, data);
}
// BANK_n의 reg 주소에 데이터 일기
void ICM_Read(uint8_t bank, uint8_t reg, uint8_t *data, uint8_t length)
{
	I2C_Write(ICM20948_ADDR, USER_BANK_SEL, bank);
	I2C_Read(ICM20948_ADDR, reg, data, length);
}

void ICM_SLV_Write(uint8_t slv, uint16_t addr, uint8_t reg, uint8_t data)
{
	ICM_Write(BANK_3, slv, addr);			// SLV Sensor Address
	ICM_Write(BANK_3, slv+1, reg);			// SLV Sensor Register Address
	ICM_Write(BANK_3, slv+3, data);			// Write Data
	ICM_Write(BANK_3, slv+2, SLV_WRITE);	// Write Enable
	HAL_Delay(5);
}
void ICM_SLV_Ctrl(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t length)
{
	ICM_Write(BANK_3, slv, SLV_READ | addr);	// SLV Sensor Address
	ICM_Write(BANK_3, slv+1, reg);				// SLV Sensor Register Address
	ICM_Write(BANK_3, slv+2, 0x80 | length);	// Read Data + Length
	HAL_Delay(5);
}

void ICM_Init()
{
	ICM_Reset();
	HAL_Delay(100);

//	ICM_SLV_WhoAmI(0, AK09916_ADDR << 1, AK_DEVICE_ID, 0x09);

	ICM_Write(BANK_0, PWR_MGMT_1, 0x01);
	ICM_Write(BANK_0, PWR_MGMT_2, 0x00);
	ICM_Write(BANK_0, LP_CONFIG, 0x00);

	ICM_READ_WhoAmI();

	ICM_Write(BANK_2, ODR_ALIGN_EN, 0x01);

	ICM_GYRO_Config(&gyro);
	ICM_ACCEL_Config(&accel);

	ICM_SMPLRT_Divide(&gyro, &accel);

	ICM_REMOVE_Offset(&gyro, &accel);

	ICM_SLV_Init();
	AK09916_MAG_Init();

	ICM_SLV_Ctrl(SLV0_ADDR, AK09916_ADDR, AK_MEASURE_MAG_XL, 8);
}
void ICM_Reset()
{
	ICM_Write(BANK_0, PWR_MGMT_1, 0x80);
}
void ICM_READ_WhoAmI()
{
	uint8_t responseAddr = 0;
	ICM_Read(BANK_0, WHO_AM_I, &responseAddr, 1);
	if (responseAddr == 0xEA)	printf("OK \n\r");
	else 						printf("Don't find ICM \n\r");
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
	uint16_t gyro_div = (1125 / gyro->odr) - 1;
	uint16_t accel_div = (1125 / accel->odr) - 1;

	// 디바이더 값이 유효 범위(0~255) 내에 있는지 확인
	if (gyro_div > 255) gyro_div = 255;
	if (accel_div > 255) accel_div = 255;

	ICM_Write(BANK_2, GYRO_SMPLRT_DIV, (uint8_t)gyro_div);

	ICM_Write(BANK_2, ACCEL_SMPLRT_DIV_1, (accel_div >> 8) & 0x0F);
	ICM_Write(BANK_2, ACCEL_SMPLRT_DIV_2, accel_div);
}

void ICM_RAW_GetData(GyroConfig *gyro, AccelConfig *accel)
{
	uint8_t imu_data[14] = {0};
	ICM_Read(BANK_0, ACCEL_DATA, imu_data, 14);

	accel->x_data = (int16_t)(imu_data[0] << 8 | imu_data[1]);
	accel->y_data = (int16_t)(imu_data[2] << 8 | imu_data[3]);
	accel->z_data = (int16_t)(imu_data[4] << 8 | imu_data[5]);

	gyro->x_data = (int16_t)(imu_data[8] << 8 | imu_data[9]);
	gyro->y_data = (int16_t)(imu_data[10] << 8 | imu_data[11]);
	gyro->z_data = (int16_t)(imu_data[12] << 8 | imu_data[13]);

//	printf("Gyro X: %d, Y: %d, Z: %d\n", gyro->x_data, gyro->y_data, gyro->z_data);
//	printf("Accel X: %d, Y: %d, Z: %d\n", accel->x_data, accel->y_data, accel->z_data);
}
void ICM_GetScaledData(GyroConfig *gyro, AccelConfig *accel)
{
	ICM_RAW_GetData(gyro, accel);

//	// OFFSET 레지스터 기반 보정 시 사용 (현재 오류)
//	float gyro_x_dps = (float)gyro->x_data / gyro->sensitivity;
//	float gyro_y_dps = (float)gyro->y_data / gyro->sensitivity;
//	float gyro_z_dps = (float)gyro->z_data / gyro->sensitivity;
//
//	float accel_x_g = (float)accel->x_data / accel->sensitivity;
//	float accel_y_g = (float)accel->y_data / accel->sensitivity;
//	float accel_z_g = (float)accel->z_data / accel->sensitivity;

	float gyro_x_dps = ((float)gyro->x_data + offset.x_gyro) / gyro->sensitivity;
	float gyro_y_dps = ((float)gyro->y_data + offset.y_gyro) / gyro->sensitivity;
	float gyro_z_dps = ((float)gyro->z_data + offset.z_gyro) / gyro->sensitivity;

	float accel_x_g = ((float)accel->x_data + offset.x_accel) / accel->sensitivity;
	float accel_y_g = ((float)accel->y_data + offset.y_accel) / accel->sensitivity;
	float accel_z_g = ((float)accel->z_data + offset.z_accel) / accel->sensitivity;

	printf("Gyro (dps) - X: %.2f, Y: %.2f, Z: %.2f\n", gyro_x_dps, gyro_y_dps, gyro_z_dps);
	printf("Accel (g) - X: %.2f, Y: %.2f, Z: %.2f\n", accel_x_g, accel_y_g, accel_z_g);
}

void ICM_SLV_Init()
{
	ICM_Write(BANK_0, USER_CTRL, 0x02);		// I2C_MST_RST
	HAL_Delay(100);

	ICM_Write(BANK_0, USER_CTRL, 0x20);		// I2C_MST_EN
	HAL_Delay(10);

	ICM_Write(BANK_3, MST_CTRL, 0x07);		// I2C MST Clock 400 kHz
	HAL_Delay(10);

	ICM_Write(BANK_0, LP_CONFIG, 0x40);		// MST_CYCLE
	HAL_Delay(10);

	ICM_Write(BANK_3, MST_ODR_CONFIG, 0x03); // 1.1Hz/(2^3) = 136 Hz
	HAL_Delay(10);
}

void ICM_SLV_WhoAmI(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t id)
{
	uint8_t responseAddr = 0;
	ICM_Write(BANK_0, USER_CTRL, 0x00);
	ICM_Write(BANK_0, INT_PIN_CFG, 0x02);
	HAL_Delay(10);

	I2C_Read(addr, reg, &responseAddr, 1);
	if (responseAddr == id)		printf("SLV%d OK \n\r", slv);
	else 						printf("Don't find SLV%d \n\r", slv);

	HAL_Delay(10);
	ICM_Write(BANK_0, INT_PIN_CFG, 0x00);
	ICM_Write(BANK_0, USER_CTRL, 0x20);
}

void AK09916_MAG_Init()
{
	ICM_SLV_Write(SLV0_ADDR, AK09916_ADDR, AK_CNTL_3, 0x01); // MAG RESET
	HAL_Delay(100);

	// continuous mode 4 : 100Hz
	ICM_SLV_Write(SLV0_ADDR, AK09916_ADDR, AK_CNTL_2, 0x08);
}

void AK09916_RAW_GetData(MagData *mag)
{
	uint8_t imu_data[8] = {0};
	ICM_Read(BANK_0, SLV_SENS_DATA_01, imu_data, 8);

	mag->x_data = (int16_t)(imu_data[1] << 8 | imu_data[0]);
	mag->y_data = (int16_t)(imu_data[3] << 8 | imu_data[2]);
	mag->z_data = (int16_t)(imu_data[5] << 8 | imu_data[4]);

	printf("MAG X : %d, Y : %d, Z : %d \n", mag->x_data, mag->y_data, mag->z_data);
}

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

MagConfig mag = {
	.x_data = 0,
	.y_data = 0,
	.z_data = 0
};

GyroData scaled_gyro = {0, 0, 0};
AccelData scaled_accel = {0, 0, 0};
MagData scaled_mag = {0, 0, 0};

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

void ICM_SLV_Write(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t data)
{
	ICM_Write(BANK_3, slv, addr);				// I2C_SLVx_ADDR
	ICM_Write(BANK_3, slv+1, reg);				// I2C_SLVx_REG
	ICM_Write(BANK_3, slv+3, data);				// I2C_SLV0_DO
	ICM_Write(BANK_3, slv+2, 0x80 | 1);			// I2C_SLV0_CTRL
	HAL_Delay(5);
}
void ICM_SLV_Read(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t *data)
{
	ICM_Write(BANK_3, slv, SLV_READ | addr);	// I2C_SLVx_ADDR
	ICM_Write(BANK_3, slv+1, reg);				// I2C_SLVx_REG
	ICM_Write(BANK_3, slv+2, 0x80 | 1);			// I2C_SLV0_CTRL Read + 1 Byte
	HAL_Delay(20);
	ICM_Read(BANK_0, SLV_SENS_DATA_00, data, 1);
}

void ICM_SLV_Ctrl(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t length)
{
	ICM_Write(BANK_3, slv, SLV_READ | addr);	// I2C_SLVx_ADDR
	ICM_Write(BANK_3, slv+1, reg);				// I2C_SLVx_REG
	ICM_Write(BANK_3, slv+2, 0x80 | length);	// I2C_SLV0_CTRL Read + length
	HAL_Delay(5);
}

void ICM_Init()
{
	ICM_Reset();
	HAL_Delay(100);

	ICM_Write(BANK_0, PWR_MGMT_1, 0x01);
	ICM_Write(BANK_0, PWR_MGMT_2, 0x00);
	ICM_Write(BANK_0, LP_CONFIG, 0x00);

	ICM_READ_WhoAmI();

	ICM_Write(BANK_2, ODR_ALIGN_EN, 0x01);

	ICM_GYRO_Config(&gyro);
	ICM_ACCEL_Config(&accel);

	ICM_SMPLRT_Divide(&gyro, &accel);

	ICM_REMOVE_Offset(&gyro, &accel, &offset);

	ICM_SLV_Init();
	AK09916_MAG_Init();
	ICM_SLV_WhoAmI(0, AK09916_ADDR, AK_DEVICE_ID, 0x09);

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

	scaled_gyro.x_dps = ((float)gyro->x_data + offset.x_gyro) / gyro->sensitivity;
	scaled_gyro.y_dps = ((float)gyro->y_data + offset.y_gyro) / gyro->sensitivity;
	scaled_gyro.z_dps = ((float)gyro->z_data + offset.z_gyro) / gyro->sensitivity;

	scaled_accel.x_g = ((float)accel->x_data + offset.x_accel) / accel->sensitivity;
	scaled_accel.y_g = ((float)accel->y_data + offset.y_accel) / accel->sensitivity;
	scaled_accel.z_g = ((float)accel->z_data + offset.z_accel) / accel->sensitivity;

	printf("Gyro (dps) - X: %.2f, Y: %.2f, Z: %.2f\n", scaled_gyro.x_dps, scaled_gyro.y_dps, scaled_gyro.z_dps);
	printf("Accel (g) - X: %.2f, Y: %.2f, Z: %.2f\n", scaled_accel.x_g, scaled_accel.y_g, scaled_accel.z_g);
}

void ICM_Calibrate(GyroConfig gyro, AccelConfig accel, IcmOffset *offset)
{
	switch (gyro.fs_sel)
	{
		case _2g:
			offset->x_gyro = -172;
			offset->y_gyro = -33;
			offset->z_gyro = -833;
			break;
		case _4g:
			offset->x_gyro = -85;
			offset->y_gyro = -12;
			offset->z_gyro = -832;
			break;
		case _8g:
			offset->x_gyro = -44;
			offset->y_gyro = -7;
			offset->z_gyro = -848;
			break;
		case _16g:
			offset->x_gyro = -21;
			offset->y_gyro = -3;
			offset->z_gyro = -848;
			break;
	}
	switch (accel.fs_sel)
	{
		case _2g:
			offset->x_accel = -288; 	// -8194;
			offset->y_accel = -348;		// -4294;
			offset->z_accel = -632;		// -3410;
			break;
		case _4g:
			offset->x_accel = -4088;
			offset->y_accel = -2145;
			offset->z_accel = -1693;
			break;
		case _8g:
			offset->x_accel = -2059;
			offset->y_accel = -1079;
			offset->z_accel = -856;
			break;
		case _16g:
			offset->x_accel = -1031;
			offset->y_accel = -541;
			offset->z_accel = -430;
			break;
	}
}

// 오프셋 제거 메인 함수
void ICM_REMOVE_Offset(GyroConfig *gyro_config, AccelConfig *accel_config, IcmOffset *offset)
{
//	ICM_CalculateOffset(gyro_config, accel_config);
    ICM_Calibrate(*gyro_config, *accel_config, offset);   // Offset 설정
    HAL_Delay(100);
}

void ICM_SLV_Init()
{
	ICM_Write(BANK_0, USER_CTRL, 0x02);		// I2C_MST_RST
	HAL_Delay(100);

	ICM_Write(BANK_0, USER_CTRL, 0x20);		// I2C_MST_EN
	HAL_Delay(10);

	ICM_Write(BANK_3, MST_CTRL, 0x47);		// I2C MST Clock 400 kHz
	HAL_Delay(10);

	ICM_Write(BANK_3, MST_DELAY_CTRL, 0x01);
	HAL_Delay(10);

	ICM_Write(BANK_0, LP_CONFIG, 0x40);		// MST_CYCLE
	HAL_Delay(10);

	// 1.1Hz/(2^3) = 136 Hz, 1.1Hz/(2^4) = 68.75 Hz
	ICM_Write(BANK_3, MST_ODR_CONFIG, 0x04);
	HAL_Delay(10);
}

//void ICM_SLV_WhoAmI(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t id)
//{
//	uint8_t responseAddr = 0;
//	ICM_Write(BANK_0, USER_CTRL, 0x00);		// I2C 마스터 비활성화
//	ICM_Write(BANK_0, INT_PIN_CFG, 0x02);	// BYPASS 모드
//	HAL_Delay(10);
//
//	I2C_Read(addr, reg, &responseAddr, 1);
//	if (responseAddr == id)		printf("SLV%d OK \n\r", slv);
//	else 						printf("Don't find SLV%d \n\r", slv);
//
//	HAL_Delay(10);
//	ICM_Write(BANK_0, INT_PIN_CFG, 0x00);
//	ICM_Write(BANK_0, USER_CTRL, 0x20);
//}
void ICM_SLV_WhoAmI(uint8_t slv, uint8_t addr, uint8_t reg, uint8_t id)
{
    uint8_t responseAddr = 0;
    ICM_SLV_Read(slv, addr, reg, &responseAddr);
    if (responseAddr == id) 	printf("SLV%d OK \n\r", slv);
    else 						printf("Don't find SLV%d, ID: 0x%02x \n\r", slv, responseAddr);
}


void AK09916_MAG_Init(MagConfig *mag)
{
	ICM_SLV_Write(SLV0_ADDR, AK09916_ADDR, AK_CNTL_3, 0x01); // MAG RESET
	HAL_Delay(100);

	// continuous mode 4 : 100Hz
	ICM_SLV_Write(SLV0_ADDR, AK09916_ADDR, AK_CNTL_2, 0x08);
}

void AK09916_RAW_GetData(MagConfig *mag)
{
	uint8_t imu_data[8] = {0};
	ICM_Read(BANK_0, SLV_SENS_DATA_00, imu_data, 8);

	mag->x_data = (int16_t)(imu_data[1] << 8 | imu_data[0]);
	mag->y_data = (int16_t)(imu_data[3] << 8 | imu_data[2]);
	mag->z_data = (int16_t)(imu_data[5] << 8 | imu_data[4]);

	printf("MAG X : %d, Y : %d, Z : %d \n", mag->x_data, mag->y_data, mag->z_data);
}

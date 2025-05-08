/*
 * icm_20948_offset.c
 *
 *  Created on: Apr 3, 2025
 *      Author: USER
 */

/*
 * 현재 코드의 보정값은 레지스터 해상도 문제로 인해 오히려 센서 값을 더 이상하게 만드는 부작용을 일으킴
 * 따라서, 보정값을 구조체에 저장하고, 센서 데이터를 읽을 때 직접 오프셋을 빼고 계산
 */

#include "Inc/icm_20948.h"

IcmOffset offset = {0, 0, 0, 0, 0, 0};

void ICM_ClearOffsetRegisters()
{
    // Gyro Offset 초기화 (BANK 2)
    ICM_Write(BANK_2, XG_OFFS_USRH, 0x00);
    ICM_Write(BANK_2, XG_OFFS_USRL, 0x00);
    ICM_Write(BANK_2, YG_OFFS_USRH, 0x00);
    ICM_Write(BANK_2, YG_OFFS_USRL, 0x00);
    ICM_Write(BANK_2, ZG_OFFS_USRH, 0x00);
    ICM_Write(BANK_2, ZG_OFFS_USRL, 0x00);

    // Accel Offset 초기화 (BANK 1)
    ICM_Write(BANK_1, XA_OFFS_H, 0x00);
    ICM_Write(BANK_1, XA_OFFS_L, 0x00);
    ICM_Write(BANK_1, YA_OFFS_H, 0x00);
    ICM_Write(BANK_1, YA_OFFS_L, 0x00);
    ICM_Write(BANK_1, ZA_OFFS_H, 0x00);
    ICM_Write(BANK_1, ZA_OFFS_L, 0x00);
}

// ICM-20948 바이어스 계산 및 오프셋 설정
void ICM_Calibrate(GyroConfig *gyro, AccelConfig *accel)
{
	uint16_t samples = 200;
    int32_t sum_gx = 0, sum_gy = 0, sum_gz = 0;
    int32_t sum_ax = 0, sum_ay = 0, sum_az = 0;

    ICM_ClearOffsetRegisters(); // OFFSET 레지스터 초기화

    // 정지 상태에서 샘플 수집
    for (int i = 0; i < samples; i++) {
        ICM_RAW_GetData(gyro, accel);
        sum_gx += gyro->x_data;
        sum_gy += gyro->y_data;
        sum_gz += gyro->z_data;
        sum_ax += accel->x_data;
        sum_ay += accel->y_data;
        sum_az += accel->z_data;
        HAL_Delay(2); // 샘플 간 간격
    }

    // 바이어스 계산 (평균값의 반대 부호)
    int16_t offset_gx = -(sum_gx / samples);
    int16_t offset_gy = -(sum_gy / samples);
    int16_t offset_gz = -(sum_gz / samples);

    // 바이어스 계산 (Z축은 중력 보정)
    int16_t offset_ax = -(sum_ax / samples);
    int16_t offset_ay = -(sum_ay / samples);
    int16_t offset_az = -(sum_az / samples);

//    // 오프셋 레지스터에 설정
//    ICM_SetGyroOffset(offset_gx, offset_gy, offset_gz);
//    // 오프셋 저장
//    ICM_SetAccelOffset(offset_ax, offset_ay, offset_az);

    // 구조체에 저장 (디버깅용)
    offset.x_gyro = offset_gx;
    offset.y_gyro = offset_gy;
    offset.z_gyro = offset_gz;

    // 디버깅용으로 구조체에도 저장
    offset.x_accel = offset_ax;
    offset.y_accel = offset_ay;
    offset.z_accel = offset_az + accel->sensitivity;
}

// 오프셋 제거 메인 함수
void ICM_REMOVE_Offset(GyroConfig *gyro_config, AccelConfig *accel_config)
{
    ICM_Calibrate(gyro_config, accel_config);   // Offset 설정
    HAL_Delay(100);
}

// 하드웨어 방식으 OFFSET 제거 방법 (오류)
/*
// 자이로스코프 오프셋 레지스터 설정 함수
void ICM_SetGyroOffset(int16_t offset_x, int16_t offset_y, int16_t offset_z)
{
	ICM_Write(BANK_2, XG_OFFS_USRH, (offset_x >> 8) & 0xFF); // X축 MSB
	ICM_Write(BANK_2, XG_OFFS_USRL, offset_x & 0xFF);        // X축 LSB
	ICM_Write(BANK_2, YG_OFFS_USRH, (offset_y >> 8) & 0xFF); // Y축 MSB
	ICM_Write(BANK_2, YG_OFFS_USRL, offset_y & 0xFF);        // Y축 LSB
	ICM_Write(BANK_2, ZG_OFFS_USRH, (offset_z >> 8) & 0xFF); // Z축 MSB
	ICM_Write(BANK_2, ZG_OFFS_USRL, offset_z & 0xFF);        // Z축 LSB
}

// 가속도계 오프셋 레지스터 설정 함수
void ICM_SetAccelOffset(int16_t offset_x, int16_t offset_y, int16_t offset_z)
{
    ICM_Write(BANK_1, XA_OFFS_H, (offset_x >> 7) & 0xFF); // X축 상위 비트 (14:7)
    ICM_Write(BANK_1, XA_OFFS_L, (offset_x << 1) & 0xFE); // X축 하위 비트 (6:0)
    ICM_Write(BANK_1, YA_OFFS_H, (offset_y >> 7) & 0xFF); // Y축 상위 비트
    ICM_Write(BANK_1, YA_OFFS_L, (offset_y << 1) & 0xFE); // Y축 하위 비트
    ICM_Write(BANK_1, ZA_OFFS_H, (offset_z >> 7) & 0xFF); // Z축 상위 비트
    ICM_Write(BANK_1, ZA_OFFS_L, (offset_z << 1) & 0xFE); // Z축 하위 비트
}
*/

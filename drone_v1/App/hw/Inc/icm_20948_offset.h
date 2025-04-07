/*
 * icm_20948_offset.h
 *
 *  Created on: Apr 3, 2025
 *      Author: USER
 */

#ifndef HW_INC_ICM_20948_OFFSET_H_
#define HW_INC_ICM_20948_OFFSET_H_

#include "Inc/icm_20948.h"
#include "Inc/icm_20948_test.h"

// Accel의 Bias 제거를 위한 Offset
#define XA_OFFS_H		0x14	// BANK 1
#define XA_OFFS_L		0x15
#define YA_OFFS_H		0x17
#define YA_OFFS_L		0x18
#define ZA_OFFS_H		0x1A
#define ZA_OFFS_L		0x1B

// Accel의 Bias 제거를 위한 Offset
#define XG_OFFS_USRH	0x03
#define XG_OFFS_USRL	0x04
#define YG_OFFS_USRH	0x05
#define YG_OFFS_USRL	0x06
#define ZG_OFFS_USRH	0x07
#define ZG_OFFS_USRL	0x08

typedef struct {
	uint16_t x_gyro;
	uint16_t y_gyro;
	uint16_t z_gyro;
	uint16_t x_accel;
	uint16_t y_accel;
	uint16_t z_accel;
} IcmOffset;

void ICM_REMOVE_Offset();

#endif /* HW_INC_ICM_20948_OFFSET_H_ */

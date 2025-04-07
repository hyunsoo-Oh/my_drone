/*
 * ak09916.h
 *
 *  Created on: Apr 3, 2025
 *      Author: USER
 */

#ifndef HW_INC_AK09916_H_
#define HW_INC_AK09916_H_

#include "def.h"
#include "bsp.h"

#define AK09916_ADDR		0x0C

#define AK_COMPANY_ID		0x00 // 0x48
#define AK_DEVICE_ID		0x01 // 0x09

#define AK_STATUS_1			0x10
#define AK_STATUS_2			0x18

#define AK_MEASURE_MAG_XL	0x11
#define AK_MEASURE_MAG_XH	0x12
#define AK_MEASURE_MAG_YL	0x13
#define AK_MEASURE_MAG_YH	0x14
#define AK_MEASURE_MAG_ZL	0x15
#define AK_MEASURE_MAG_ZH	0x16

#define AK_CNTL_2			0x31
#define AK_CNTL_3			0x32

#endif /* HW_INC_AK09916_H_ */

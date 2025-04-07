/*
 * bmp280.h
 *
 *  Created on: Apr 3, 2025
 *      Author: USER
 */

#ifndef HW_INC_BMP280_H_
#define HW_INC_BMP280_H_

#include "def.h"
#include "bsp.h"

#define BMP280_ADDR			0x77

#define BMP_TEMP_XLSB		0xFC
#define	BMP_TEMP_LSB		0xFB
#define BMP_TEMP_MSB		0xFA

#define BMP_PRESS_XLSB		0xF9
#define BMP_PRESS_LSB		0xF8
#define BMP_PRESS_MSB		0xF7

#define BMP_CONFIG			0xF5
#define BMP_CTRL_MEAS		0xF4
#define BMP_STATUS			0xF3
#define BMP_RESET			0xD0

#define BMP_CALI_25			0xA1


#endif /* HW_INC_BMP280_H_ */

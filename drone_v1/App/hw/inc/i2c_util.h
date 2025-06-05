/*
 * i2c_util.h
 *
 *  Created on: Jun 5, 2025
 *      Author: USER
 */

#ifndef HW_INC_I2C_UTIL_H_
#define HW_INC_I2C_UTIL_H_

#include "usart.h"
#include "i2c.h"

#define I2C_BUS      &hi2c1

void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data);
void I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length);

#endif /* HW_INC_I2C_UTIL_H_ */

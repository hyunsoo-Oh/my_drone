/*
 * i2c_util.c
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#include "i2c_util.h"

HAL_StatusTypeDef I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
	uint8_t buff = data;
	return HAL_I2C_Mem_Write(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT, &buff, 1, 100);
}

HAL_StatusTypeDef I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length)
{
	return HAL_I2C_Mem_Read(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT, data, length, 100);
}


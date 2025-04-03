/*
 * bsp.c
 *
 *  Created on: Apr 3, 2025
 *      Author: USER
 */

#include "bsp.h"

int _write(int file, unsigned char* p, int len)
{
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, p, len, 100);
	return (status == HAL_OK ? len : 0);
}

void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
	uint8_t buff = data;
	HAL_I2C_Mem_Write(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT, &buff, 1, 100);
}
void I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length)
{
	HAL_I2C_Mem_Read(I2C_BUS, addr, reg, I2C_MEMADD_SIZE_8BIT, data, length, 100);
}

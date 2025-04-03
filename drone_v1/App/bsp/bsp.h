/*
 * bsp.h
 *
 *  Created on: Apr 3, 2025
 *      Author: USER
 */

#ifndef BSP_BSP_H_
#define BSP_BSP_H_

#include "usart.h"
#include "i2c.h"

#define I2C_BUS      &hi2c1

void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data);
void I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t length);

#endif /* BSP_BSP_H_ */

/*
 * hw.c
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#include "hw.h"

int _write(int file, unsigned char* p, int len)
{
	HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, p, len, 100);
	return (status == HAL_OK ? len : 0);
}

void hwInit()
{

}

void delay(uint32_t ms)
{
	HAL_Delay(ms);
}
uint32_t millis()
{
	return HAL_GetTick();
}

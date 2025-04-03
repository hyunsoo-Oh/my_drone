/*
 * hw.c
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#include "hw.h"

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

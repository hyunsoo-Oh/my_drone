/*
 * hw.c
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#include "hw.h"
#include "tim.h"

void hwInit()
{
	HAL_TIM_Base_Start(&htim4);
}

void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (__HAL_TIM_GET_COUNTER(&htim4) < us);
}

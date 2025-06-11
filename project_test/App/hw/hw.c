/*
 * hw.c
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#include "hw.h"
#include "tim.h"

void hwInit(void)
{
	HAL_TIM_Base_Start(&htim10);
}

void delay_us(uint32_t us)
{
	__HAL_TIM_SET_COUNTER(&htim10, 0);
	while (__HAL_TIM_GET_COUNTER(&htim10) < us);
}
uint32_t millis(void);

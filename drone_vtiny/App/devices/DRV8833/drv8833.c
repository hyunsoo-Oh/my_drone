/*
 * drv8833.c
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#include "drv8833.h"

void DRV8833_Init(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	// Rear-Right
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);	// Front-Right
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);	// Front-Left
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);	// Rear-Left
}

void DRV8833_Update(float speed[4])
{
	uint32_t pulse[4];

	pulse[FL] = (uint32_t)(speed[FL] * 9.99f);
	pulse[FL] = (uint32_t)(speed[FL] * 9.99f);
	pulse[FL] = (uint32_t)(speed[FL] * 9.99f);
	pulse[FL] = (uint32_t)(speed[FL] * 9.99f);
}

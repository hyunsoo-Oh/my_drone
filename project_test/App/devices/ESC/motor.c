/*
 * motor.c
 *
 *  Created on: May 14, 2025
 *      Author: USER
 */


#include "motor.h"

static uint16_t motor_throttle[MOTOR_COUNT] = {PWM_MIN_PULSE};
static uint8_t esc_armed = 0;
static uint32_t arming_start_time = 0;

void ESC_Init(void)
{
	// 보라색 선 Back
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);	// Right Side Back
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);	// Right Side Front
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);	// Left Side Back
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);	// Left Side Front
}

void ESC_SetThrottle(Motor_t* motor, uint8_t percent)
{
	if (percent > 100) percent = 100;
	    motor->pulse = PWM_MIN_PULSE + (percent * (PWM_MAX_PULSE - PWM_MIN_PULSE)) / 100;

	    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, motor->pulse);

	    char debug_msg[100];
	    uint8_t len = sprintf(debug_msg, "%s : %d us (%d%%)\r\n", motor->name, motor->pulse, percent);
	    HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, len, 100);
}

void ESC_ArmSequence(Motor_t motors[MOTOR_COUNT])
{
    HAL_UART_Transmit(&huart2, (uint8_t*)"ESC 아밍 시퀀스 시작...\r\n", 32, 100);

    // 1. 최대 PWM 출력 (2ms → 100%) → ESC가 "최대 스로틀" 인식
     for (int i = 0; i < MOTOR_COUNT; i++)
         ESC_SetThrottle(&motors[i], 0);
     HAL_Delay(3000);  // 2초 유지

     // 2. 최소 PWM 출력 (1ms → 0%) → ESC가 "최소 스로틀" 인식
     for (int i = 0; i < MOTOR_COUNT; i++)
         ESC_SetThrottle(&motors[i], 100);
     HAL_Delay(6000);  // 5초 유지

     esc_armed = 1;
     arming_start_time = HAL_GetTick();

    HAL_UART_Transmit(&huart2, (uint8_t*)"ESC 아밍 완료!\r\n", 20, 100);
}

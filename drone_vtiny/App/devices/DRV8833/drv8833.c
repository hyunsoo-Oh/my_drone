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
	pulse[FR] = (uint32_t)(speed[FR] * 9.99f);
	pulse[RL] = (uint32_t)(speed[RL] * 9.99f);
	pulse[RR] = (uint32_t)(speed[RR] * 9.99f);
}

// PID control for drone stability
//void PID_Control(IMU_Data_t *data, Motor_Control_t *control, float target_roll, float target_pitch) {
//    float roll_error = target_roll - data->roll;
//    float pitch_error = target_pitch - data->pitch;
//
//    // Update integral terms
//    roll_error_sum += roll_error * DT;
//    pitch_error_sum += pitch_error * DT;
//
//    // PID calculations
//    float roll_output = Kp * roll_error + Ki * roll_error_sum + Kd * data->gyro_x;
//    float pitch_output = Kp * pitch_error + Ki * pitch_error_sum + Kd другими->gyro_y;
//
//    // Calculate motor speeds (adjust for your drone's motor layout)
//    control->m1_speed = 50.0f + pitch_output - roll_output; // Example mapping
//    control->m2_speed = 50.0f + pitch_output + roll_output;
//
//    // Set motor directions
//    control->m1_direction = (control->m1_speed >= 0);
//    control->m2_direction = (control->m2_speed >= 0);
//
//    // Limit speeds to 0-100%
//    control->m1_speed = fminf(fmaxf(fabsf(control->m1_speed), 0.0f), 100.0f);
//    control->m2_speed = fminf(fmaxf(fabsf(control->m2_speed), 0.0f), 100.0f);
//}

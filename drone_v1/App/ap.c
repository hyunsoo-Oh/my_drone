/*
 * ap.c
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#include "ap.h"

//int16_t accel[3], gyro[3];
float accel[3], gyro[3];
float temp;
char msg[128];
uint8_t test;

Motor_t motors[MOTOR_COUNT] = {
    [FRONT_LEFT]  	= { "Front_Left",  &htim2, TIM_CHANNEL_2, PWM_MIN_PULSE },
    [BACK_LEFT] 	= { "Back_Left",   &htim2, TIM_CHANNEL_1, PWM_MIN_PULSE },
    [FRONT_RIGHT]   = { "Front Right", &htim1, TIM_CHANNEL_2, PWM_MIN_PULSE },
    [BACK_RIGHT]  	= { "Back_Right",  &htim1, TIM_CHANNEL_1, PWM_MIN_PULSE }
};
FlightControl_t control = {0};

void apInit()
{
	ESC_Init();
	ESC_ArmSequence(motors);

	// 센서 초기화
	MPU6500_Init();
}

void apMain()
{
	MPU6500_Read_DMA();

	while(1)
	{
		MPU6500_ProcessData(accel, gyro);

        // 일정 주기로 다시 요청
        static uint32_t t_prev = 0;
        if (HAL_GetTick() - t_prev > 10)  // 100Hz
        {
            t_prev = HAL_GetTick();

            snprintf(msg, sizeof(msg),
                     "AX:%.2f AY:%.2f AZ:%.2f | GX:%.2f GY:%.2f GZ:%.2f\r\n",
					 accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

            MPU6500_Read_DMA();  // 다음 프레임 요청
        }

        ESC_SetThrottle(&motors[FRONT_LEFT],  80);
        ESC_SetThrottle(&motors[BACK_LEFT],   80);
        ESC_SetThrottle(&motors[FRONT_RIGHT], 80);
        ESC_SetThrottle(&motors[BACK_RIGHT],  80);
	}
}

/*
 * motor.h
 *
 *  Created on: May 14, 2025
 *      Author: USER
 */

#ifndef HW_INC_MOTOR_H_
#define HW_INC_MOTOR_H_

#include "hw.h"
#include "tim.h"
#include "usart.h"

// === PWM 관련 상수 정의 ===
#define PWM_FREQUENCY       50          // ESC 제어용 50Hz
#define PWM_PERIOD          20000       // 20ms 주기
#define PWM_MIN_PULSE       1000        // 1ms (정지)
#define PWM_MAX_PULSE       2000        // 2ms (최대 스로틀)
#define PWM_ARM_PULSE       1000        // 아밍용 최소 펄스

typedef enum {
    FRONT_LEFT,    // 전방 좌측 (TIM2_CH2)
    FRONT_RIGHT,   // 전방 우측 (TIM1_CH2)
    BACK_LEFT,     // 후방 좌측 (TIM2_CH1)
    BACK_RIGHT,    // 후방 우측 (TIM1_CH1)
    MOTOR_COUNT
} MotorPosition_t;

typedef struct {
	char *name;
    TIM_HandleTypeDef* htim;  // PWM 타이머
    uint32_t channel;         // 타이머 채널
    uint16_t pulse;        // 현재 펄스값
} Motor_t;

void Motor_Init(void);
void ESC_SetThrottle(Motor_t* motor, uint8_t percent);
void ESC_ArmSequence(Motor_t motors[MOTOR_COUNT]);

#endif /* HW_INC_MOTOR_H_ */

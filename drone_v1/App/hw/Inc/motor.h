/*
 * motor.h
 *
 *  Created on: May 14, 2025
 *      Author: USER
 */

#ifndef HW_INC_MOTOR_H_
#define HW_INC_MOTOR_H_

#include "hw.h"

void ESC_CALI();
void ESC_NO_CALI();
void SET_Throttle(uint32_t* motor_throttle);

#endif /* HW_INC_MOTOR_H_ */

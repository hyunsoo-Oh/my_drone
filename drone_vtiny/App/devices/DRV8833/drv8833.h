/*
 * drv8833.h
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#ifndef DEVICES_DRV8833_DRV8833_H_
#define DEVICES_DRV8833_DRV8833_H_

#include "hw.h"
#include "tim.h"

typedef enum {
	FL,		// Front-Left
	FR, 	// Front-Right
	RL, 	// Rear-Left
	RR		// Rear-Right
} MotorPos;

typedef enum {
	CW,		// ClockWise
	CCW,	// Counter ClockWise
} MotorDir;

void DRV8833_Init(void);
void DRV8833_Update(float speed[4]);

#endif /* DEVICES_DRV8833_DRV8833_H_ */

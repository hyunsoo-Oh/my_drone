/*
 * ap.h
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#ifndef AP_H_
#define AP_H_

#include "hw.h"
#include "delay.h"
#include "devices/imu/mpu6500.h"
#include "devices/motor/motor.h"
#include "devices/motor/pid_control.h"

void apInit(void);

void apMain(void);

#endif /* AP_H_ */

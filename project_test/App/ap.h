/*
 * ap.h
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#ifndef AP_H_
#define AP_H_

#include "hw.h"
#include "usart.h"
#include "GY521/gy-521.h"
#include "MPU6500/mpu6500.h"
#include "ESC/motor.h"

void apInit(void);
void apMain(void);

#endif /* AP_H_ */

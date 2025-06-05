/*
 * hw.h
 *
 *  Created on: Mar 17, 2025
 *      Author: User
 */

#ifndef HW_HW_H_
#define HW_HW_H_

#include "def.h"
#include "usart.h"
#include "devices/imu/icm_20948.h"

void hwInit(void);

void delay(uint32_t ms);
uint32_t millis(void);

#endif /* HW_HW_H_ */

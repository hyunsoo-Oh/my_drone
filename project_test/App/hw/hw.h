/*
 * hw.h
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#ifndef HW_HW_H_
#define HW_HW_H_

#include "def.h"
#include "tim.h"

void hwInit(void);

void delay_us(uint32_t us);
uint32_t millis(void);

#endif /* HW_HW_H_ */

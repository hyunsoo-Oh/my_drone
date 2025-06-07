/*
 * bluetooth.h
 *
 *  Created on: Jun 5, 2025
 *      Author: User
 */

#ifndef DEVICES_BLUETOOTH_BLUETOOTH_H_
#define DEVICES_BLUETOOTH_BLUETOOTH_H_

#include "def.h"
#include "usart.h"
#include "devices/motor/motor.h"
#include "devices/motor/pid_control.h"


#define DMA_BUF_SIZE		64
#define COMMAND_MAX			6

void BLUETOOTH_Init(void);
//void BLUETOOTH_Parsing(MotorState *motor);

#endif /* DEVICES_BLUETOOTH_BLUETOOTH_H_ */

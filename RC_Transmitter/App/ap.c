/*
 * ap.c
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#include "ap.h"
#include "nrf24l01.h"

uint8_t payload[5] = "nRF24";

void apInit(void)
{
	nRF24L01_TxInit();
}

void apMain(void)
{
	while (1)
	{
		nRF24L01_Send(payload, sizeof(payload));
		HAL_Delay(500);
	}
}

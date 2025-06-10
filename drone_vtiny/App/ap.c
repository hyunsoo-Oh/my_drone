/*
 * ap.c
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#include "ap.h"

void apInit(void)
{
	DRV8833_Init();
	DRV8833_SetThrottle(400);
}

void apMain(void)
{
	while (1)
	{

	}
}

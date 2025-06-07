/*
 * bluetooth.c
 *
 *  Created on: Jun 5, 2025
 *      Author: User
 */

#include "bluetooth.h"

static uint8_t dmaBuff[DMA_BUF_SIZE];		// DMA 수신 버퍼
static uint8_t cmdData[COMMAND_MAX + 1];	// 명령어 파싱 버퍼 (최대 COMMAND_MAX 글자 + NULL)
static uint8_t cmdLength;					// 현재까지 수신된 명령어 길이
static volatile uint8_t btFlag = 0;			// 블루투스 데이터 수신 완료 플래그

extern Motor_t motors[MOTOR_COUNT];
extern FlightControl_t control;

void BLUETOOTH_Init(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dmaBuff, DMA_BUF_SIZE);
	__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);		// Half Transfer 비활성화
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);		// Idle 인터럽트 활성화
}

static int16_t STR_TO_Integer(char *s)
{
	int16_t val = 0;
	while (*s >= '0' && *s <= '9')
	{
		val = val * 10 + (*s - '0');
		s++;
	}
	return val;
}

static void MODE_Change(const uint8_t *cmdData, uint8_t len)
{
    if (cmdData[0] == 'Z')
    {
//    	motors.pulse = 100;
    }
    else if (cmdData[0] == 'Y')
    {
//    	motors.pulse = 0;
    }
}

// 8자리 고정 명령어: Throttle, Pitch, Roll, Yaw 설정
static void PARSE_CONTROL_DATA(const uint8_t *cmd)
{
    if (cmdLength != 8) return;

    char temp[3] = {0};

    memcpy(temp, &cmd[0], 2); control.throttle = STR_TO_Integer(temp);
    memcpy(temp, &cmd[2], 2); control.pitch    = STR_TO_Integer(temp);
    memcpy(temp, &cmd[4], 2); control.roll     = STR_TO_Integer(temp);
    memcpy(temp, &cmd[6], 2); control.yaw      = STR_TO_Integer(temp);
}

// 메인 파싱 함수 (매 루프마다 호출)
void BLUETOOTH_Parsing(void)
{
    if (btFlag == 1)
    {
        btFlag = 0;

        if (cmdLength == 1 && (cmdData[0] == 'Z' || cmdData[0] == 'Y'))
        {
            MODE_Change(cmdData, cmdLength);
        }
        else if (cmdLength == 8)
        {
            PARSE_CONTROL_DATA(cmdData);
        }
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart1)
    {
        for (uint16_t i = 0; i < Size; i++)
        {
            char c = (char)dmaBuff[i];

            if (c != '\n' && cmdLength < COMMAND_MAX)
            {
            	cmdData[cmdLength++] = c;
            }
            else if (c == '\n')
            {
            	cmdData[cmdLength] = '\0';
				btFlag = 1;
				cmdLength = 0;
            }
        }
    }
}

/*
 * ap.c
 *
 *  Created on: Jun 4, 2025
 *      Author: USER
 */

#include "ap.h"
#include "nrf24l01.h"
#include "fakecode.h"

uint8_t tx[4] = "fool";
uint8_t payload[5];
////int16_t accel[3], gyro[3];
//float accel[3], gyro[3];
//float temp;
char msg[128];
uint8_t test;

//Motor_t motors[MOTOR_COUNT] = {
//    [FRONT_LEFT]  	= { "Front_Left",  &htim2, TIM_CHANNEL_2, PWM_MAX_PULSE },
//    [BACK_LEFT] 	= { "Back_Left",   &htim2, TIM_CHANNEL_1, PWM_MAX_PULSE },
//    [FRONT_RIGHT]   = { "Front Right", &htim1, TIM_CHANNEL_2, PWM_MAX_PULSE },
//    [BACK_RIGHT]  	= { "Back_Right",  &htim1, TIM_CHANNEL_1, PWM_MAX_PULSE }
//};

void apInit(void)
{
//	GY521_Init();
//	MPU6500_Init();
//	ESC_Init();

//	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
//	nRF24L01_RxInit();
//	nRF24L01_TxInit();
	uint8_t addr[5] = {'R','x','A','A','A'};
	nrf24_init();
	nrf24_set_channel(76);
	nrf24_set_tx_addr(addr, 5);
	nrf24_set_payload_size(4);
	nrf24_tx_mode();
}

void apMain(void)
{
//    static uint8_t prev_state = 0;
//    uint8_t is_pressed;

//    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 700);
//    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 700);
	while (1)
	{
//		if (nRF24L01_Read(payload, sizeof(payload)))
//		{
//			int len = snprintf(msg, sizeof(msg),
//						   "RX: %c%c%c%c%c\r\n",
//						   payload[0], payload[1], payload[2], payload[3], payload[4]);
//			HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);
//		}
//		else
//		{
//			HAL_UART_Transmit(&huart2, (uint8_t*)"No data received\r\n", sizeof("No data received\r\n"), 100);
//		}


	      nrf24_send_data(tx, 4);
	      printf("NRF STATUS: 0x%02X\r\n", nrf24_read_reg(0x07));
	      HAL_Delay(50);
//		is_pressed = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET); // 버튼 눌림: LOW
//		test = is_pressed;
//
//		if (is_pressed != prev_state)  // 상태 변화 감지
//		{
//			if (is_pressed)
//			{
//				HAL_UART_Transmit(&huart2, (uint8_t*)"버튼 누름 → MIN\r\n", 22, 100);
//
//				// 버튼 누르고 있는 동안 → MIN 출력
//				for (int i = 0; i < MOTOR_COUNT; i++)
//					ESC_SetThrottle(&motors[i], 0);    // 0%   → 1000us → MIN
//			}
//			else
//			{
//				HAL_UART_Transmit(&huart2, (uint8_t*)"버튼 뗌 → MAX\r\n", 20, 100);
//
//				// 버튼에서 손을 떼면 → 다시 MAX 출력
//				for (int i = 0; i < MOTOR_COUNT; i++)
//					ESC_SetThrottle(&motors[i], 100);  // 100% → 2000us → MAX
//			}
//
//			prev_state = is_pressed;  // 상태 갱신은 딱 한 번
//		}
	}
}

/*
 * ESC 프로그래밍 모드 설정 항목 안내
 * -------------------------------------
 * 프로그래밍 모드 진입 방법:
 *   - PWM 신호를 MAX (2000us) 상태로 유지한 채 ESC 전원 연결
 *   - ESC가 “♪123” → 셀 수만큼 삐 → "삐삐삐삐..." 반복 → 프로그래밍 모드 진입
 *
 * 메뉴는 비프음(beep) 횟수로 안내되며,
 * 사용자는 원하는 항목에서 PWM을 MIN (1000us)로 잠깐 내려 설정 가능
 *
 * beep-beep-beep -> btn 누름 -> beeeeeeep -> btn 뗌 -> beep -> btn 누름 -> beeeeeeep
 *
 * 각 항목 설명:
 *
 * ① Beep 1회: 브레이크 설정 (Brake Setting) ->
 *    - Option 1: 브레이크 해제 (Off)   → 기본값
 *    - Option 2: 브레이크 활성화 (On)
 *
 * ② Beep 2회: 배터리 종류 설정 (Battery Type)
 *    - Option 1: LiPo     → 기본값
 *    - Option 2: NiMH
 *
 * ③ Beep 3회: 저전압 컷오프 임계값 (Low Voltage Cutoff Threshold)
 *    - Option 1: 낮음 (Low)    	beep 	→ 기본값
 *    - Option 2: 중간 (Middle)	beep-beep
 *    - Option 3: 높음 (High) 	beep-beep-beep
 *
 * ④ Beep 4회: 컷오프 모드 (Cutoff Mode)
 *    - Option 1: 출력 감소 (Reduce Power) → 기본값
 *    - Option 2: 완전 차단 (Cut Off Output)
 *
 * ⑤ Beeeep 0회: 스타트 모드 (Start Mode)
 *    - Option 1: 부드럽게 (Normal)   → 기본값
 *    - Option 2: 빠르게 (Soft)
 *    - Option 3: 아주 빠르게 (Very Soft)
 *
 * ⑥ Beeeep Beep 1회: 타이밍 모드 (Timing Mode)
 *    - Option 1: 저 (Low)
 *    - Option 2: 중 (Medium) → 기본값
 *    - Option 3: 고 (High)
 *
 * ⑦ Beeeep Beep 2회: 초기화 (Restore Defaults)
 *    - Option 1: 공장 초기 설정 복원
 *
 * Beeeep Beeeep : 종료
 *
 * 설정 방법:
 *   - 원하는 항목에서 비프음 수를 듣고, PWM을 MIN (1000us)로 변경 → 해당 항목 진입
 *   - 각 옵션을 하나씩 비프음으로 안내 → MIN으로 눌러 선택
 *   - 선택이 완료되면 긴 “삐——” 음 발생 → 다시 MAX로 돌아가면 다음 항목 안내 시작
 */


//void I2C_ADDRESS_Scan(void)
//{
//	HAL_UART_Transmit(&huart2, (uint8_t *)"🔍 I2C 스캔 시작...\r\n", sizeof("🔍 I2C 스캔 시작...\r\n") - 1, 100);
//
//	for (uint8_t addr = 1; addr < 127; addr++)
//	{
//		// 7비트 주소를 왼쪽으로 1비트 시프트 (HAL은 8비트 주소 사용)
//		if (HAL_I2C_IsDeviceReady(&hi2c1, (addr << 1), 1, 10) == HAL_OK)
//		{
//			snprintf(msg, sizeof(msg), "✅ I2C 기기 발견: 0x%02X\r\n", addr);
//			HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
//		}
//	}
//
//	HAL_UART_Transmit(&huart2, (uint8_t *)"✅ 스캔 완료.\r\n", sizeof("✅ 스캔 완료.\r\n") - 1, 100);
//
//	I2C_Read(MPU6500_I2C_ADDR, MPU6500_WHO_AM_I, &test, 1);
//	if (test == 0x70)
//		 HAL_UART_Transmit(&huart2, (uint8_t *)"MPU6500\r\n", sizeof("MPU6500\r\n") - 1, 100);
//	else
//		HAL_UART_Transmit(&huart2, (uint8_t *)"Failed\r\n", sizeof("Failed\r\n") - 1, 100);
//}
//
//void GY521_RawRead_Task(void)
//{
//	GY521_ReadAccel(accel);
//	GY521_ReadGyro(gyro);
//	GY521_ReadTemp(&temp);
//
//	snprintf(msg, sizeof(msg),
//		 "ACC: X=%d Y=%d Z=%d | GYRO: X=%d Y=%d Z=%d | TEMP=%.2f\r\n",
//		 accel[0], accel[1], accel[2],
//		 gyro[0], gyro[1], gyro[2],
//		 temp / 340.0f + 36.53f
//	);
//
//	HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
//}
//
//void GY521_OffsetMeasure_Task(void)
//{
//	GY521_OffsetMeasure();
//}
//
//void GY521_ScaledRead_Task(void)
//{
//	GY521_GetAccelG(accel);
//	GY521_GetGyroDPS(gyro);
//	temp = GY521_GetTempCelsius();
//
//	snprintf(msg, sizeof(msg),
//		 "ACC: X=%.2f Y=%.2f Z=%.2f | GYRO: X=%.2f Y=%.2f Z=%.2f | TEMP=%.2f\r\n",
//		 accel[0], accel[1], accel[2],
//		 gyro[0], gyro[1], gyro[2],
//		 temp
//	);
//
//	HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
//}
//
//void MPU6500_RawRead_Task(void)
//{
//	MPU6500_ReadAccel(accel);
//	MPU6500_ReadGyro(gyro);
//	temp = MPU6500_ReadTemperature();
//
//	snprintf(msg, sizeof(msg),
//		"ACC: X=%d Y=%d Z=%d | GYRO: X=%d Y=%d Z=%d | TEMP=%.2f\r\n",
//		 accel[0], accel[1], accel[2],
//		 gyro[0], gyro[1], gyro[2],
//		 temp
//	);
//
//	HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
//}
//
//void MPU6500_OffsetMeasure_Task(void)
//{
//	MPU6500_OffsetMeasure();
//}
//
//void MPU6500_ScaledRead_Task(void)
//{
//	MPU6500_ReadAccelScaled(accel);
//	MPU6500_ReadGyroScaled(gyro);
//	temp = MPU6500_ReadTemperature();
//
//	snprintf(msg, sizeof(msg),
//		 "ACC: X=%.2f Y=%.2f Z=%.2f | GYRO: X=%.2f Y=%.2f Z=%.2f | TEMP=%.2f\r\n",
//		 accel[0], accel[1], accel[2],
//		 gyro[0], gyro[1], gyro[2],
//		 temp
//	);
//
//	HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);
//}

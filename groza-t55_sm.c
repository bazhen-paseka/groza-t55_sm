#include "groza-t55_sm.h"
#include <string.h>

#include "average_calc_3_from_5.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

  char DataChar[100];

  uint32_t timer_u32[4];
  uint32_t channel_1_value_u32[4];
  uint32_t channel_2_value_u32[4];

  static uint32_t main_value_u32[DEVICE_QNT][CIRCLE_QNT];

  uint8_t flag_60_sec_u8 = 0;

  void Strob_A(void);
  void Strob_B(void);
  void Strob_delay(uint32_t _delay);

void Groza_t55_init (void) {
	sprintf(DataChar,"\r\n19ZH36 GROZA-T55 v1.4.0 2020-jan-09\r\nUART1 for debug on speed 115200\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
}
//*****************************************************************************

void Groza_t55_main (uint8_t circle, char* http_req_1 ) {
	uint32_t value_i32[4];

	sprintf(DataChar,"crc %d) ", (int)circle);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	for (int j=0; j<4; j++) {
		timer_u32[j] = 0;
	}

	Strob_A();

	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(500);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[1] = (int)(timer_u32[0] - timer_u32[1]);
	value_i32[2] = (int)(timer_u32[2] - timer_u32[3]);

	sprintf(DataChar,"%d (%d)  %d (%d)  ", (int)timer_u32[0], (int)value_i32[1], (int)timer_u32[2], (int)value_i32[2] );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	for (int j=0; j<4; j++)	{
		timer_u32[j] = 0;
	}

	Strob_B();

	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(500);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[3] = timer_u32[0]-timer_u32[1];
	value_i32[4] = timer_u32[3]-timer_u32[2];

	uint32_t adc_value_U = ( ADC1_GetValue(ADC_CHANNEL_5   ) * 4 ) / 10 ;
	uint32_t adc_value_T = 3700- ADC1_GetValue(ADC_CHANNEL_TEMPSENSOR)  ;

	sprintf(DataChar,"%d (%d)  %d (%d) ADC: %d temp: %d\r\n",
			(int)timer_u32[0], (int)value_i32[3], (int)timer_u32[2], (int)value_i32[4], (int)adc_value_U, (int)adc_value_T );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	main_value_u32[0][circle] = value_i32[1];
	main_value_u32[1][circle] = value_i32[2];
	main_value_u32[2][circle] = value_i32[3];
	main_value_u32[3][circle] = value_i32[4];
	main_value_u32[4][circle] = adc_value_U;
	main_value_u32[5][circle] = adc_value_T;

	if (circle == CIRCLE_QNT-1)	{
		uint32_t aver_res_u32[DEVICE_QNT];
		for (uint8_t c = 0; c < DEVICE_QNT; c++) {
			sprintf(DataChar,"dev %d) ", (int)c );
			HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

			aver_res_u32[c] = Calc_Average(main_value_u32[c], CIRCLE_QNT);

			sprintf(DataChar,"res %d\r\n", (int)aver_res_u32[c] );
			HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
		}
	sprintf(http_req_1, "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d\r\n\r\n",
			(int)aver_res_u32[0], (int)aver_res_u32[1], (int)aver_res_u32[2], (int)aver_res_u32[3], (int)aver_res_u32[4], (int)aver_res_u32[5] );
	}
}
//*****************************************************************************

void Set_Flag_60_Sec(uint8_t _flag)	{
	flag_60_sec_u8 = _flag;
}
//*****************************************************************************

uint8_t Get_Flag_60_Sec(void) {
	return flag_60_sec_u8;
}
//*****************************************************************************

void Timer_Update( uint8_t _timer_u8, uint32_t _tim_value_u32) {
	timer_u32[_timer_u8] = _tim_value_u32;
}
//*****************************************************************************

void Strob_A(void) {
	HAL_GPIO_WritePin(STROB14_GPIO_Port, STROB14_Pin, SET);
	Strob_delay(10);
	HAL_GPIO_WritePin(STROB14_GPIO_Port, STROB14_Pin, RESET);
}
//***************************************************************************

void Strob_B(void) {
	HAL_GPIO_WritePin(STROB15_GPIO_Port, STROB15_Pin, SET);
	Strob_delay(10);
	HAL_GPIO_WritePin(STROB15_GPIO_Port, STROB15_Pin, RESET);
}
//***************************************************************************

void Strob_delay(uint32_t _delay) {
	for (int w=0; w<_delay; w++) {
		__asm("nop");
	}
}

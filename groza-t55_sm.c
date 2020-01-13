#include "groza-t55_sm.h"
#include <string.h>

#include "average_calc_3_from_5.h"
#include "groza-t55_config.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

  char DataChar[100];

  uint32_t timer_u32[4];
  uint32_t channel_1_value_u32[4];
  uint32_t channel_2_value_u32[4];

  static uint32_t main_value_u32[DEVICE_QNT][CIRCLE_QNT];

  uint8_t flag_60_sec_u8 = 0;

  void Strobe_X(uint32_t _strobe_duration);
  void Strobe_Y(uint32_t _strobe_duration);
  void Strobe_Z(uint32_t _strobe_duration);
  void Local_delay(uint32_t _delay);

void Groza_t55_init (void) {
	sprintf(DataChar,"\r\n19ZH36 GROZA-T55 2020-jan-12 v1.6.0\r\nUART1 for debug on speed 115200\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
}
//*****************************************************************************

void Groza_t55_main (uint8_t circle, char* http_req_1 ) {
	uint32_t value_i32[4];

	sprintf(DataChar,"%d)", (int)circle);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	for (int j=0; j<4; j++) {
		timer_u32[j] = 0;
	}

	Strobe_Y(STROBE_DURATION);
	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(MEASUREMENT_TIME);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[1] = (timer_u32[0] - timer_u32[1]);
	value_i32[2] = (timer_u32[2] - timer_u32[3]);

	sprintf(DataChar," %d %d (%d)  %d %d (%d)", (int)timer_u32[0], (int)timer_u32[1], (int)value_i32[1], (int)timer_u32[2], (int)timer_u32[3], (int)value_i32[2] );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	for (int j=0; j<4; j++)	{
		timer_u32[j] = 0;
	}

	Strobe_X(STROBE_DURATION);
	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(MEASUREMENT_TIME);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[3] = timer_u32[0]-timer_u32[1];
	value_i32[4] = timer_u32[3]-timer_u32[2];

	uint32_t adc_value_U = ( ADC1_GetValue(ADC_CHANNEL_5   ) * 4 ) / 10 ;
	uint32_t adc_value_T = 3700- ADC1_GetValue(ADC_CHANNEL_TEMPSENSOR)  ;

	sprintf(DataChar,"  %d %d (%d)  %d %d (%d)  %dV  %dC\r\n",
			(int)timer_u32[0], (int)timer_u32[1], (int)value_i32[3], (int)timer_u32[2], (int)timer_u32[3], (int)value_i32[4], (int)adc_value_U, (int)adc_value_T );
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
			sprintf(DataChar,"%d) ", (int)(c+1) );
			HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

			aver_res_u32[c] = Calc_Average(main_value_u32[c], CIRCLE_QNT);

			sprintf(DataChar," (%d)\r\n", (int)aver_res_u32[c] );
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

void Strobe_Y(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_Y_GPIO_Port, STROBE_Y_Pin, SET);
	Local_delay(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_Y_GPIO_Port, STROBE_Y_Pin, RESET);
}
//***************************************************************************

void Strobe_X(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_X_GPIO_Port, STROBE_X_Pin, SET);
	Local_delay(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_X_GPIO_Port, STROBE_X_Pin, RESET);
}
//***************************************************************************

void Strobe_Z(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_Z_GPIO_Port, STROBE_Z_Pin, SET);
	Local_delay(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_Z_GPIO_Port, STROBE_Z_Pin, RESET);
}
//***************************************************************************

void Local_delay(uint32_t _delay) {
	for (uint32_t t=0; t<_delay; t++) {
		__asm("nop");
	}
}
//***************************************************************************

void TestStrobe (void) {
	do {
		Strobe_X(STROBE_DURATION);
		HAL_Delay(200);

		Strobe_Y(STROBE_DURATION);
		HAL_Delay(200);

		Strobe_Z(STROBE_DURATION);
		HAL_Delay(200);
	}
	while (1);
}
//***************************************************************************

//***************************************************************************

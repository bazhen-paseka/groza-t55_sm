#include "groza-t55_sm.h"
#include <string.h>

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

  char DataChar[100];

  uint32_t timer_u32[4];
  uint32_t channel_1_value_u32[4];
  uint32_t channel_2_value_u32[4];

  int value_i32[4];

  uint8_t flag_60_sec_u8 = 0;

void Groza_t55_init (void)
{
	sprintf(DataChar,"\r\n19ZH36 GROZA-T55 debug base time\r\nUART1 for debug started\r\nSpeed 38400\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
}
//*****************************************************************************

void Groza_t55_main (void)
{
	for (int j=0; j<4; j++)
	{
		timer_u32[j] = 0;
	}

	HAL_GPIO_WritePin(STROB14_GPIO_Port, STROB14_Pin, SET);
	for (int w=0; w<10; w++)
	  { __asm("nop");}
	HAL_GPIO_WritePin(STROB14_GPIO_Port, STROB14_Pin, RESET);

	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(500);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[1] = (int)(timer_u32[0] - timer_u32[1]);
	value_i32[2] = (int)(timer_u32[3] - timer_u32[2]);

	sprintf(DataChar,"%d (%d)  %d (%d)  ", (int)timer_u32[0], value_i32[1], (int)timer_u32[2], value_i32[2] );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);



	for (int j=0; j<4; j++)
	{
		timer_u32[j] = 0;
	}

	HAL_GPIO_WritePin(STROB15_GPIO_Port, STROB15_Pin, SET);
	for (int w=0; w<10; w++)
	  { __asm("nop");}
	HAL_GPIO_WritePin(STROB15_GPIO_Port, STROB15_Pin, RESET);

	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(500);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[3] = (int)(timer_u32[0]-timer_u32[1]);
	value_i32[4] = (int)(timer_u32[2]-timer_u32[3]);

	sprintf(DataChar,"%d (%d)  %d (%d)\r\n", (int)timer_u32[0], value_i32[3], (int)timer_u32[2], value_i32[4] );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

}
//*****************************************************************************
int Value_T55 (uint8_t _kanal)
{
	return value_i32[_kanal];
}

//*****************************************************************************
void Set_Flag_60_Sec(uint8_t _flag)
{
	flag_60_sec_u8 = _flag;
}

uint8_t Get_Flag_60_Sec(void)
{
	return flag_60_sec_u8;
}
//*****************************************************************************

void Timer_Update( uint8_t _timer_u8, uint32_t _tim_value_u32)
{
	timer_u32[_timer_u8] = _tim_value_u32;
}
//*****************************************************************************

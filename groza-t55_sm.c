#include "groza-t55_sm.h"
#include <string.h>

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

  char DataChar[100];
  volatile uint32_t tim_k0;
  volatile uint32_t tim_k1;
  volatile uint32_t tim_k2;
  volatile uint32_t tim_k3;

  int value[4];

  uint8_t flag_60_sec_u8 = 0;

void Groza_t55_init (void)
{
	sprintf(DataChar,"\r\n19ZH36 GROZA-T55 debug base time\r\nUART1 for debug started\r\nSpeed 38400\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
}
//*****************************************************************************

void Groza_t55_main (void)
{
	tim_k0 = 1;
	tim_k1 = 1;
	tim_k2 = 1;
	tim_k3 = 1;

	  HAL_GPIO_WritePin(STROB14_GPIO_Port, STROB14_Pin, SET);
	  for (int w=0; w<10; w++)
	  	  { __asm("nop");}
	  HAL_GPIO_WritePin(STROB14_GPIO_Port, STROB14_Pin, RESET);

	  TIM4->CNT = 0;
	  HAL_TIM_Base_Start(&htim4);
	  HAL_Delay(500);
	  HAL_TIM_Base_Stop(&htim4);

	sprintf(DataChar,"%d (%d)  %d (%d)  ", (int)tim_k0, (int)(tim_k0-tim_k1), (int)tim_k2, (int)(tim_k3-tim_k2) );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
	value[1] = (int)(tim_k0-tim_k1);
	value[2] = (int)(tim_k3-tim_k2);

	tim_k0 = 1;
	tim_k1 = 1;
	tim_k2 = 1;
	tim_k3 = 1;

	  HAL_GPIO_WritePin(STROB15_GPIO_Port, STROB15_Pin, SET);
	  for (int w=0; w<10; w++)
	  	  { __asm("nop");}
	  HAL_GPIO_WritePin(STROB15_GPIO_Port, STROB15_Pin, RESET);

	  TIM4->CNT = 0;
	  HAL_TIM_Base_Start(&htim4);
	  HAL_Delay(500);
	  HAL_TIM_Base_Stop(&htim4);

	sprintf(DataChar,"%d (%d)  %d (%d)\r\n", (int)tim_k0, (int)(tim_k0-tim_k1), (int)tim_k2, (int)(tim_k2-tim_k3) );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
	value[3] = (int)(tim_k0-tim_k1);
	value[4] = (int)(tim_k2-tim_k3);
}
//*****************************************************************************
uint32_t Value_T55 (uint8_t _kanal)
{
	return value[_kanal];
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

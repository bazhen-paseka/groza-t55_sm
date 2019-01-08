#include "groza-t55_sm.h"
#include <string.h>

#include "average_calc_3_from_5.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;
extern ADC_HandleTypeDef hadc1;

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

	#define AVER_QNT 	7
	uint32_t aver[AVER_QNT] = {60, 50, 40, 30, 20, 10, 0};
	uint32_t aver_res = Calc_Average(aver, AVER_QNT);

	sprintf(DataChar,"aver_res = %d\r\n", (int)aver_res);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
}
//*****************************************************************************

void Groza_t55_main (char* http_req_1, uint8_t* size_of_http_req )
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

	int adc_value_U = (int)( ( ADC1_GetValue(ADC_CHANNEL_5         ) * 4 ) / 10) ;
	int adc_value_T = (int)( 3700- ADC1_GetValue(ADC_CHANNEL_TEMPSENSOR)       ) ;

	sprintf(DataChar,"%d (%d)  %d (%d) ADC: %d temp: %d\r\n", (int)timer_u32[0], value_i32[3], (int)timer_u32[2], value_i32[4], adc_value_U, adc_value_T );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	sprintf(http_req_1, "GET /update?api_key=%s&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d\r\n\r\n",
									THINGSPEAK_API_KEY, value_i32[1], value_i32[2], value_i32[3], value_i32[4], adc_value_U, adc_value_T);

	*size_of_http_req = strlen(http_req_1);
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

uint32_t ADC1_GetValue(uint32_t channel)
{
    /* Config ADC channel */
    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    //sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    /* Start conversion */
    HAL_ADC_Start(&hadc1);
    /* Wait until finish */
    HAL_ADC_PollForConversion(&hadc1, 100);
    uint32_t value = HAL_ADC_GetValue(&hadc1);
    return value;
}
//*****************************************************************************

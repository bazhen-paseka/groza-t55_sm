//******************************************************************************************

	#include "groza-t55_sm.h"

//******************************************************************************************

	extern UART_HandleTypeDef huart1;
	extern TIM_HandleTypeDef htim4;

//******************************************************************************************

  char DataChar[0xFF];

  uint32_t timer_u32[4];
  uint32_t channel_1_value_u32[4];
  uint32_t channel_2_value_u32[4];

  static uint32_t main_value_u32[DEVICE_QNT][CIRCLE_QNT];

  uint8_t flag_1_sec_u8 = 0;

//	lcd1602_fc113_struct h1_lcd1602_fc113 =
//	{
//		.i2c = &hi2c1,
//		.device_i2c_address = ADR_I2C_FC113
//	};

//	uint8_t  dataIn[32];
//	NRF24L01_Transmit_Status_t transmissionStatus;	/* NRF transmission status */
//
//	#ifdef SLAVE_21
//		uint8_t MyAddress[] = { 0, 0, 0, 0, 0x21 };	/* My address */
//		uint8_t TxAddress[] = { 0, 0, 0, 0, 0x10 };	/* Other end address */
//	#endif
//
//	uint32_t lastTime = 0;
//	int16_t  waitTime = 0;

//******************************************************************************************

  void Strobe_X(uint32_t _strobe_duration);
  void Strobe_Y(uint32_t _strobe_duration);
  void Strobe_Z(uint32_t _strobe_duration);
  void Local_delay(uint32_t _delay);
//******************************************************************************************

void Groza_t55_init (void) {
	int soft_version_arr_int[3];
	soft_version_arr_int[0] = ((SOFT_VERSION) / 100)     ;
	soft_version_arr_int[1] = ((SOFT_VERSION) /  10) %10 ;
	soft_version_arr_int[2] = ((SOFT_VERSION)      ) %10 ;

	sprintf(DataChar,"\r\n19ZH36 GROZA-T55 2021-dec-05 v%d.%d.%d\r\nUART1 for debug on speed 115200\r\n\r\n",
			soft_version_arr_int[0], soft_version_arr_int[1], soft_version_arr_int[2]);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

//	I2Cdev_init(&hi2c1);
//	I2C_ScanBusFlow(&hi2c1, &huart1);
//
//	LCD1602_Init(&h1_lcd1602_fc113);
//	I2C_ScanBus_to_LCD1602(&h1_lcd1602_fc113);
//
//	LCD1602_Clear(&h1_lcd1602_fc113);
//	LCD1602_Cursor_Return(&h1_lcd1602_fc113);
//	sprintf(DataChar,"19zh6 Groza-T55");
//	LCD1602_Print_Line(&h1_lcd1602_fc113, DataChar, strlen(DataChar));

//	NRF24L01_Init(&hspi2, MY_CHANNEL, 32);
//	NRF24L01_SetRF(NRF24L01_DataRate_250k, NRF24L01_OutputPower_M6dBm);	/* Set 250kBps data rate and -6dBm output power */
//	NRF24L01_SetMyAddress(MyAddress);	/* Set my address, 5 bytes */
//	LCD1602_Clear(&h1_lcd1602_fc113);
}
//*****************************************************************************

void Groza_t55_main (uint8_t circle, char* http_req_1 ) {
	uint32_t 	value_i32[4];
//	uint32_t 	adc_value_U = ( ADC1_GetValue( ADC_CHANNEL_5 ) * 4 ) / 10 ;
//	uint32_t 	adc_value_T = 3700 - ADC1_GetValue( ADC_CHANNEL_TEMPSENSOR ) ;

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
	value_i32[2] = (timer_u32[3] - timer_u32[2]);

	sprintf(DataChar," %04d %04d (%03d)  %04d %04d (%03d)",
						(int)timer_u32[0],
						(int)timer_u32[1],
						(int)value_i32[1],
						(int)timer_u32[2],
						(int)timer_u32[3],
						(int)value_i32[2] );
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
	value_i32[4] = timer_u32[2]-timer_u32[3];

	uint32_t adc_value_U = 		(	ADC1_GetValue( &hadc1, ADC_CHANNEL_5		 ) * 4 ) / 10 ;
	uint32_t adc_value_T = 3700 - 	ADC1_GetValue( &hadc1, ADC_CHANNEL_TEMPSENSOR)  ;

	sprintf(DataChar,"  %04d %04d (%03d)  %04d %04d (%03d)  %04dV  %04dC\r\n",
						(int)timer_u32[0],
						(int)timer_u32[1],
						(int)value_i32[3],
						(int)timer_u32[2],
						(int)timer_u32[3],
						(int)value_i32[4],
						(int)adc_value_U,
						(int)adc_value_T );
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
						(int)aver_res_u32[0],
						(int)aver_res_u32[1],
						(int)aver_res_u32[2],
						(int)aver_res_u32[3],
						(int)aver_res_u32[4],
						(int)aver_res_u32[5] );
	}
}
//*****************************************************************************

void Set_Flag_1_Sec(uint8_t _flag)	{
	flag_1_sec_u8 = _flag;
}
//*****************************************************************************

uint8_t Get_Flag_1_Sec(void) {
	return flag_1_sec_u8;
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

void TestStrobe (uint8_t _counter) {
	Strobe_X(STROBE_DURATION);
	HAL_Delay(200);

	Strobe_Y(STROBE_DURATION);
	HAL_Delay(200);

	Strobe_Z(STROBE_DURATION);
	HAL_Delay(200);

//	char uart_buffer[0xFF];
//	sprintf(uart_buffer,"counter %02d", _counter );
//	LCD1602_Cursor_Return(&h1_lcd1602_fc113);
//	LCD1602_Print_Line(&h1_lcd1602_fc113, uart_buffer, strlen(uart_buffer));
}
//***************************************************************************

//void NRF24L01_Module(void) {
//	if (NRF24L01_DataReady()) {	/* If data is ready on NRF24L01+ */
//		NRF24L01_GetData(dataIn);	/* Get data from NRF24L01+ */
//		HAL_Delay(1);
//		sprintf(DataChar,"%s", dataIn);
//		HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
//		LCD1602_Cursor_Return(&h1_lcd1602_fc113);
//		LCD1602_Print_Line(&h1_lcd1602_fc113, DataChar, strlen(DataChar));
//
//		/* Send it back, automatically goes to TX mode */
//		NRF24L01_SetTxAddress(TxAddress);	/* Set TX address, 5 bytes */
//		NRF24L01_Transmit(dataIn);
//		/* Wait for data to be sent */
//		do {		/* Wait till sending */
//			transmissionStatus = NRF24L01_GetTransmissionStatus();
//		} while (transmissionStatus == NRF24L01_Transmit_Status_Sending);
//		/* Send done */
//
//		if (transmissionStatus == NRF24L01_Transmit_Status_Ok) {	/* Check data & transmit status */
//			sprintf(DataChar,"; Send back: OK\r\n");				/* Transmit went OK */
//		} else {
//			sprintf(DataChar,"; Send back: ERROR\r\n");			/* Message was LOST */
//		}
//		HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
//
//		NRF24L01_PowerUpRx();	/* Go back to RX mode */
//		waitTime = 0;
//	} else {
//		if (HAL_GetTick() - lastTime > 250) {
//			if (waitTime == 0) {
//				sprintf(DataChar,"Waiting for data");
//				waitTime++;
//			} else if (waitTime > 17) {
//				sprintf(DataChar,"\r\n");
//				waitTime = 0;
//			} else {
//				sprintf(DataChar,".");
//				waitTime++;
//			}
//			HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
//			lastTime = HAL_GetTick();
//		}
//	}
//}
//***************************************************************************

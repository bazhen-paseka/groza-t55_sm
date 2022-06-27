//******************************************************************************************

	#include "groza-t55_sm.h"

//******************************************************************************************

	extern UART_HandleTypeDef huart1;
	extern TIM_HandleTypeDef htim4;
//******************************************************************************************

	#define TIM_QNT		4

//******************************************************************************************

	char DataChar[0xFF];
	uint32_t timer_u32[ TIM_QNT ];
	//  uint32_t channel_1_value_u32[4];
	//  uint32_t channel_2_value_u32[4];
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
  void local_delay(uint32_t _delay);
//******************************************************************************************

void Groza_t55_init (void) {
	int soft_version_arr_int[3];
	soft_version_arr_int[0] = ((SOFT_VERSION) / 100)     ;
	soft_version_arr_int[1] = ((SOFT_VERSION) /  10) %10 ;
	soft_version_arr_int[2] = ((SOFT_VERSION)      ) %10 ;

	sprintf(DataChar,"\r\n\t 19ZH36 GROZA-T55 v%d.%d.%d\r\n",
			soft_version_arr_int[0], soft_version_arr_int[1], soft_version_arr_int[2]);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, sizeof(DataChar), 100);

	#define DATE_as_int_str 	(__DATE__)
	#define TIME_as_int_str 	(__TIME__)
	sprintf(DataChar,"\t build: %s time: %s\r\n" , DATE_as_int_str , TIME_as_int_str ) ;
	HAL_UART_Transmit( &huart1, (uint8_t *)DataChar , sizeof(DataChar) , 100 ) ;

	sprintf(DataChar,"\t UART1 for debug on speed 115200\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, sizeof(DataChar), 100);

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

void Measurement (PointStr *myStr, uint8_t circle) {
	uint32_t 	value_i32[DEVICE_QNT];
	sprintf(DataChar,"%d)", (int)circle);
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

		// ZONE X
	for (int j=0; j<TIM_QNT; j++) {
		timer_u32[j] = 0;
	}

	Strobe_X(STROBE_DURATION);
	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(MEASUREMENT_TIME);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[0] = timer_u32[0] ;
	value_i32[1] = timer_u32[1] ;
	value_i32[2] = timer_u32[2] ;
	value_i32[3] = timer_u32[3] ;

	myStr->zerone_u32[0] += value_i32[0] % 2 ;
	myStr->zerone_u32[1] += value_i32[1] % 2 ;
	myStr->zerone_u32[2] += value_i32[2] % 2 ;
	myStr->zerone_u32[3] += value_i32[3] % 2 ;

	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
	HAL_Delay( 5);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
	HAL_Delay(50);
		// ZONE X

		// ZONE Y
	for (int j=0; j<TIM_QNT; j++)	{
		timer_u32[j] = 0;
	}

	Strobe_Y(STROBE_DURATION);
	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(MEASUREMENT_TIME);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[4] = timer_u32[0] ;
	value_i32[5] = timer_u32[1] ;
	value_i32[6] = timer_u32[2] ;
	value_i32[7] = timer_u32[3] ;

	myStr->zerone_u32[4] += value_i32[4] % 2 ;
	myStr->zerone_u32[5] += value_i32[5] % 2 ;
	myStr->zerone_u32[6] += value_i32[6] % 2 ;
	myStr->zerone_u32[7] += value_i32[7] % 2 ;

	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
	HAL_Delay( 5);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
	HAL_Delay(50);
		// ZONE Y

		// ZONE Z
	for (int j=0; j<TIM_QNT; j++)	{
		timer_u32[j] = 0;
	}

	Strobe_Z(STROBE_DURATION);
	TIM4->CNT = 0;
	HAL_TIM_Base_Start(&htim4);
	HAL_Delay(MEASUREMENT_TIME);
	HAL_TIM_Base_Stop(&htim4);

	value_i32[ 8] = timer_u32[0] ;
	value_i32[ 9] = timer_u32[1] ;
	value_i32[10] = timer_u32[2] ;
	value_i32[11] = timer_u32[3] ;

	myStr->zerone_u32[ 8] += value_i32[ 8] % 2 ;
	myStr->zerone_u32[ 9] += value_i32[ 9] % 2 ;
	myStr->zerone_u32[10] += value_i32[10] % 2 ;
	myStr->zerone_u32[11] += value_i32[11] % 2 ;

	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
	HAL_Delay( 5);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
	HAL_Delay(50);
		// ZONE Z

	uint32_t adc_value_U1 = ADC1_GetValue( &hadc1, ADC_CHANNEL_5 ) ;
///	uint32_t adc_value_U2 = ADC1_GetValue( &hadc1, ADC_CHANNEL_6 ) ;	///	the temperature is now right
	uint32_t adc_value_T0 = 3700 - 	ADC1_GetValue( &hadc1, ADC_CHANNEL_TEMPSENSOR)  ;

	myStr->point_u32[ 0][circle] = value_i32[ 0];
	myStr->point_u32[ 1][circle] = value_i32[ 1];
	myStr->point_u32[ 2][circle] = value_i32[ 2];
	myStr->point_u32[ 3][circle] = value_i32[ 3];

	myStr->point_u32[ 4][circle] = value_i32[ 4];
	myStr->point_u32[ 5][circle] = value_i32[ 5];
	myStr->point_u32[ 6][circle] = value_i32[ 6];
	myStr->point_u32[ 7][circle] = value_i32[ 7];

	myStr->point_u32[ 8][circle] = value_i32[ 8];
	myStr->point_u32[ 9][circle] = value_i32[ 9];
	myStr->point_u32[10][circle] = value_i32[10];
	myStr->point_u32[11][circle] = value_i32[11];


	myStr->point_u32[12][circle] = adc_value_U1;
///	myStr->point_u32[13][circle] = adc_value_U2;	///	the temperature is now right
	myStr->point_u32[14][circle] = adc_value_T0 ;

	sprintf(DataChar," x0:%05d %05d x1:%05d %05d  y0:%05d %05d y1:%05d %05d  z0:%05d %05d z1:%05d %05d U1:%04d U2:%04d T0:%04d",
						(int)myStr->point_u32[ 0][circle],
						(int)myStr->point_u32[ 1][circle],
						(int)myStr->point_u32[ 2][circle],
						(int)myStr->point_u32[ 3][circle],
						(int)myStr->point_u32[ 4][circle],
						(int)myStr->point_u32[ 5][circle],
						(int)myStr->point_u32[ 6][circle],
						(int)myStr->point_u32[ 7][circle],
						(int)myStr->point_u32[ 8][circle],
						(int)myStr->point_u32[ 9][circle],
						(int)myStr->point_u32[10][circle],
						(int)myStr->point_u32[11][circle],
						(int)myStr->point_u32[12][circle],
						(int)myStr->point_u32[13][circle],
						(int)myStr->point_u32[14][circle] );
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);

	sprintf(DataChar,"\t %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d \r\n",
						(int) myStr->zerone_u32[ 0] ,
						(int) myStr->zerone_u32[ 1] ,
						(int) myStr->zerone_u32[ 2] ,
						(int) myStr->zerone_u32[ 3] ,
						(int) myStr->zerone_u32[ 4] ,
						(int) myStr->zerone_u32[ 5] ,
						(int) myStr->zerone_u32[ 6] ,
						(int) myStr->zerone_u32[ 7] ,
						(int) myStr->zerone_u32[ 8] ,
						(int) myStr->zerone_u32[ 9] ,
						(int) myStr->zerone_u32[10] ,
						(int) myStr->zerone_u32[11] ) ;
	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
}


//*****************************************************************************

//*****************************************************************************

void Set_Flag_1_Sec(uint8_t _flag)	{
	if ( _flag == 0 ) {
		flag_1_sec_u8 = 0;
	} else {
		flag_1_sec_u8 = 1;
	}
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
	local_delay(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_Y_GPIO_Port, STROBE_Y_Pin, RESET);
}
//***************************************************************************

void Strobe_X(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_X_GPIO_Port, STROBE_X_Pin, SET);
	local_delay(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_X_GPIO_Port, STROBE_X_Pin, RESET);
}
//***************************************************************************

void Strobe_Z(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_Z_GPIO_Port, STROBE_Z_Pin, SET);
	local_delay(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_Z_GPIO_Port, STROBE_Z_Pin, RESET);
}
//***************************************************************************

void local_delay(uint32_t _delay) {
	for (uint32_t t=0; t<_delay; t++) {
		__asm("nop");
	}
}
//***************************************************************************

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

//*****************************************************************************
//
//void Groza_t55_test ( void ) {
//	uint32_t 	value_i32[12];
//
//	for (int j=0; j<TIM_QNT; j++) {
//		timer_u32[j] = 0;
//	}
//
//	Strobe_X(STROBE_DURATION);
//	TIM4->CNT = 0;
//	HAL_TIM_Base_Start(&htim4);
//	HAL_Delay(MEASUREMENT_TIME);
//	HAL_TIM_Base_Stop(&htim4);
//
//	value_i32[0] = timer_u32[0] ;
//	value_i32[1] = timer_u32[1] ;
//	value_i32[2] = timer_u32[2] ;
//	value_i32[3] = timer_u32[3] ;
//
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
//	HAL_Delay( 5);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
//	HAL_Delay(50);
//
//	for (int j=0; j<TIM_QNT; j++)	{
//		timer_u32[j] = 0;
//	}
//
//	Strobe_Y(STROBE_DURATION);
//	TIM4->CNT = 0;
//	HAL_TIM_Base_Start(&htim4);
//	HAL_Delay(MEASUREMENT_TIME);
//	HAL_TIM_Base_Stop(&htim4);
//
//	value_i32[4] = timer_u32[0] ;
//	value_i32[5] = timer_u32[1] ;
//	value_i32[6] = timer_u32[2] ;
//	value_i32[7] = timer_u32[3] ;
//
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
//	HAL_Delay( 5);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
//	HAL_Delay(50);
//
//	for (int j=0; j<TIM_QNT; j++)	{
//		timer_u32[j] = 0;
//	}
//
//	Strobe_Z(STROBE_DURATION);
//	TIM4->CNT = 0;
//	HAL_TIM_Base_Start(&htim4);
//	HAL_Delay(MEASUREMENT_TIME);
//	HAL_TIM_Base_Stop(&htim4);
//
//	value_i32[ 8] = timer_u32[0] ;
//	value_i32[ 9] = timer_u32[1] ;
//	value_i32[10] = timer_u32[2] ;
//	value_i32[11] = timer_u32[3] ;
//
//	HAL_Delay(10);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
//	HAL_Delay(5);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
//	HAL_Delay(50);
//
//	uint32_t adc_value_U = 		(	ADC1_GetValue( &hadc1, ADC_CHANNEL_5		 ) * 4 ) / 10 ;
//	uint32_t adc_value_T = 3700 - 	ADC1_GetValue( &hadc1, ADC_CHANNEL_TEMPSENSOR)  ;
//
//	sprintf(DataChar,"%05d\t%05d\t%05d\t%05d\t%05d\t%05d\t%05d\t%05d\t%04d\t%04d\r\n",
//						(int)value_i32[0],
//						(int)value_i32[1],
//						(int)value_i32[2],
//						(int)value_i32[3],
//						(int)value_i32[4],
//						(int)value_i32[5],
//						(int)value_i32[6],
//						(int)value_i32[7],
//						(int)adc_value_U,
//						(int)adc_value_T );
//	HAL_UART_Transmit(&huart1, (uint8_t *)DataChar, strlen(DataChar), 100);
//}
//*****************************************************************************

//******************************************************************************************

	#include "groza-t55_sm.h"

//******************************************************************************************

	extern 	UART_HandleTypeDef 	huart1;
	extern 	UART_HandleTypeDef 	huart3;
	extern 	TIM_HandleTypeDef 	htim3;
	extern 	TIM_HandleTypeDef 	htim4;

//******************************************************************************************

	#define TIM_QNT		4

//******************************************************************************************

	//char DataChar[0xFF];
	uint32_t timer_u32[ TIM_QNT ];
	//  uint32_t channel_1_value_u32[4];
	//  uint32_t channel_2_value_u32[4];
	uint8_t flag_1_sec_u8 = 0;

	PointStr MyStr0 = {0};
	PointStr MyStr1 = {0};
	PointStr MyStr2 = {0};
	PointStr MyStr3 = {0};

	HAL_StatusTypeDef send_status = HAL_ERROR;

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

  void 		Strobe_X			(uint32_t _strobe_duration);
  void 		Strobe_Y			(uint32_t _strobe_duration);
  void 		Strobe_Z			(uint32_t _strobe_duration);
  void 		local_delay_GRZ		(uint32_t _delay);

//******************************************************************************************

void Groza_2017_Init (void) {
	DebugSoftVersion(SOFT_VERSION);
	DBG1("\t UART1 for debug on speed 62500\r\n");

	DBG1("\t Start.Ds18b20:\r\n");
	Ds18b20_Init_DWT_Delay();
	Ds18b20_Print_serial_number();
	Ds18b20_ConvertTemp_SkipROM();
	HAL_Delay(1000);
	int temp_int = Ds18b20_Get_Temp_SkipROM ();
	DBG1( "DS18b20 = %d;\r\n",temp_int);

	Esp8266_Init();
	Esp8266_WakeUp();
	Esp8266_Reset();
	Esp8266_Connect_to_WIFI( ATTEMPT_TO_WIFI );

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
	HAL_GPIO_WritePin(BUTTON_GND_GPIO_Port, BUTTON_GND_Pin, RESET );

	while (HAL_GPIO_ReadPin(BUTTON_INPUT_GPIO_Port, BUTTON_INPUT_Pin ) == GPIO_PIN_RESET ) {
		Measurement( &MyStr0, 0 );
	}

	DBG1( "Measurement.wait.for.router.ready\r\n");
	#if ( FIRST8 == 1 )
		for (int i=6; i>=0; i--) {	// wait for router ready
			Measurement( &MyStr0, i );
			HAL_Delay(300);
		}

	#elif ( NEXT12	== 1)
		for (int i=0; i<70; i++) {	// wait for router ready
			Measurement( &MyStr0, i );
			HAL_Delay(300);
		}
	#endif

	HAL_TIM_Base_Start_IT(&GROZA_TIM3);
	DBG1("End.Init.\r\n\r\n");
} //*****************************************************************************

void Groza_2017_Main (void) {
	//	NRF24L01_Module();
	while (HAL_GPIO_ReadPin(BUTTON_INPUT_GPIO_Port, BUTTON_INPUT_Pin ) == GPIO_PIN_RESET ) {
		if (Get_Flag_1_Sec() == 1) {
			Measurement( &MyStr0, 0);
			Set_Flag_1_Sec(0);
		}
	}
	for (				uint8_t line3=0; line3 < CIRCLE_QNT; line3++ )	{
		for (			uint8_t line2=0; line2 < CIRCLE_QNT; line2++ )	{
			for (		uint8_t line1=0; line1 < CIRCLE_QNT; line1++ )	{
				for (	uint8_t line0=0; line0 < CIRCLE_QNT; line0++ )	{
					while (Get_Flag_1_Sec() == 0) {	/* wait on flag 1 Sec */ }
					Set_Flag_1_Sec(0);
					Measurement( &MyStr0, line0);
				}//for(line0)
				for (uint8_t device = 0; device < DEVICE_QNT; device++) {
					DBG1("  A%d%d%d\t", (int)line3, (int)line2, (int)line1 ); fflush(stdout);
					MyStr1.point_i[device][line1] = Calc_Average(MyStr0.point_i[device], CIRCLE_QNT);
				}
			}//for(line1)
			for (uint8_t device = 0; device < DEVICE_QNT; device++) {
				DBG1("  B%d%d\t", (int)line3, (int)line2 ); fflush(stdout);
				MyStr2.point_i[device][line2] = Calc_Average(MyStr1.point_i[device], CIRCLE_QNT);
			}
		}//for(line2)
		for (uint8_t device = 0; device < DEVICE_QNT; device++) {
			DBG1("  C%d\t", (int)line3 ); fflush(stdout);
			MyStr3.point_i[device][line3] = Calc_Average(MyStr2.point_i[device], CIRCLE_QNT);
		}
	}//for(line3)

	uint32_t aver_res_u32[DEVICE_QNT];
	for (uint8_t device = 0; device < DEVICE_QNT; device++) {
		DBG1("  D\t" ); fflush(stdout);
		aver_res_u32[device] = Calc_Average(MyStr3.point_i[device], CIRCLE_QNT);
	}
	Ds18b20_ConvertTemp_SkipROM();

	Esp8266_Connect_to_WIFI( ATTEMPT_TO_WIFI );

	char http_req[0xFF] = { 0 } ;
	sprintf(http_req, "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d&field8=%d\r\n\r\n",
					(int) MyStr0.zerone_i[ 0] ,
					(int) MyStr0.zerone_i[ 1] ,
					(int) MyStr0.zerone_i[ 2] ,
					(int) MyStr0.zerone_i[ 3] ,
					(int) MyStr0.zerone_i[ 4] ,
					(int) MyStr0.zerone_i[ 5] ,
					(int) MyStr0.zerone_i[ 6] ,
					(int) MyStr0.zerone_i[ 7] ) ;
	char apiKey_2[] = THINGSPEAK_API_KEY_2 ;
	Esp8266_Send_to_Inet(http_req, apiKey_2, ATTEMPT_TO_INET );
	HAL_Delay(500);

	sprintf(http_req, "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d&field8=%d\r\n\r\n",
					(int) MyStr0.zerone_i[ 8] ,
					(int) MyStr0.zerone_i[ 9] ,
					(int) MyStr0.zerone_i[10] ,
					(int) MyStr0.zerone_i[11] ,
					(int) MyStr0.zerone_i[12] ,
					(int) MyStr0.zerone_i[13] ,
					(int) MyStr0.zerone_i[14] ,
					(int) MyStr0.zerone_i[15] ) ;
	char apiKey_3[] = THINGSPEAK_API_KEY_3 ;
	Esp8266_Send_to_Inet(http_req, apiKey_3, ATTEMPT_TO_INET );

	sprintf(http_req, "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d&field8=%d\r\n\r\n",
					(int) aver_res_u32[ 0] ,
					(int) aver_res_u32[ 1] ,
					(int) aver_res_u32[ 2] ,
					(int) aver_res_u32[ 3] ,
					(int) aver_res_u32[ 4] ,
					(int) aver_res_u32[ 5] ,
					(int) aver_res_u32[ 6] ,
					(int) aver_res_u32[ 7] );
	char apiKey_0[] = THINGSPEAK_API_KEY_0 ;
	Esp8266_Send_to_Inet(http_req, apiKey_0, ATTEMPT_TO_INET );
	HAL_Delay(500);

	int ds18b20_int = Ds18b20_Get_Temp_SkipROM ();

#if ( FIRST8 == 1 )
	sprintf(http_req, "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d&field8=%d\r\n\r\n",
					(int)((aver_res_u32[12]*4)/10),
					(int)aver_res_u32[14],
					(int)aver_res_u32[ 8],
					(int)aver_res_u32[ 9],
					(int)aver_res_u32[10],
					(int)aver_res_u32[11],
					(int)aver_res_u32[12],
					(int) ds18b20_int      );
	char apiKey_1[] = THINGSPEAK_API_KEY_1 ;
	Esp8266_Send_to_Inet(http_req, apiKey_1, ATTEMPT_TO_INET );
	HAL_Delay(500);

#elif ( NEXT12	== 1)
	sprintf(http_req, "&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%d&field7=%d&field8=%d\r\n\r\n",
					(int) aver_res_u32[ 8] ,
					(int) aver_res_u32[ 9] ,
					(int) aver_res_u32[10] ,
					(int) aver_res_u32[11] ,
					(int) aver_res_u32[12] ,
					(int) aver_res_u32[13] ,
					(int) aver_res_u32[14] ,
					(int) ds18b20_int       );
	char apiKey_1[] = THINGSPEAK_API_KEY_1 ;
	Esp8266_Send_to_Inet(http_req, apiKey_1, ATTEMPT_TO_INET );
	HAL_Delay(500);
#endif

	for (int d=0; d < DEVICE_QNT; d++) {
		MyStr0.zerone_i[d] = 0;
	}
} //*****************************************************************************

void Measurement (PointStr *myStr, uint8_t circle) {
	uint32_t 	value_i32[DEVICE_QNT];
	DBG1("%d)", (int)circle); fflush(stdout);


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

	myStr->zerone_i[0] += value_i32[0] % 2 ;
	myStr->zerone_i[1] += value_i32[1] % 2 ;
	myStr->zerone_i[2] += value_i32[2] % 2 ;
	myStr->zerone_i[3] += value_i32[3] % 2 ;

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

	myStr->zerone_i[4] += value_i32[4] % 2 ;
	myStr->zerone_i[5] += value_i32[5] % 2 ;
	myStr->zerone_i[6] += value_i32[6] % 2 ;
	myStr->zerone_i[7] += value_i32[7] % 2 ;

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

	myStr->zerone_i[ 8] += value_i32[ 8] % 2 ;
	myStr->zerone_i[ 9] += value_i32[ 9] % 2 ;
	myStr->zerone_i[10] += value_i32[10] % 2 ;
	myStr->zerone_i[11] += value_i32[11] % 2 ;

	HAL_Delay(10);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET) ;
	HAL_Delay( 5);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET) ;
	HAL_Delay(50);
		// ZONE Z

	//	uint32_t adc_value_U1 = ADC1_GetValue( &hadc1, ADC_CHANNEL_5 ) ;
	uint32_t adc_value_U1 = 3000;
	//DBG1("adc_value_U1: %lu\r\n", adc_value_U1);
	///	uint32_t adc_value_U2 = ADC1_GetValue( &hadc1, ADC_CHANNEL_6 ) ;	///	the temperature is now right
	//uint32_t adc_value_T0 = 3700 - 	ADC1_GetValue( &hadc1, ADC_CHANNEL_TEMPSENSOR)  ;
	uint32_t adc_value_T0 = 2050;
	//DBG1("adc_value_T0: %lu\r\n", adc_value_T0);

	myStr->point_i[ 0][circle] = value_i32[ 0];
	myStr->point_i[ 1][circle] = value_i32[ 1];
	myStr->point_i[ 2][circle] = value_i32[ 2];
	myStr->point_i[ 3][circle] = value_i32[ 3];

	myStr->point_i[ 4][circle] = value_i32[ 4];
	myStr->point_i[ 5][circle] = value_i32[ 5];
	myStr->point_i[ 6][circle] = value_i32[ 6];
	myStr->point_i[ 7][circle] = value_i32[ 7];

	myStr->point_i[ 8][circle] = value_i32[ 8];
	myStr->point_i[ 9][circle] = value_i32[ 9];
	myStr->point_i[10][circle] = value_i32[10];
	myStr->point_i[11][circle] = value_i32[11];


	myStr->point_i[12][circle] = adc_value_U1;
///	myStr->point_i[13][circle] = adc_value_U2;	///	the temperature is now right
	myStr->point_i[14][circle] = adc_value_T0 ;

	DBG1(" x0:%05d %05d x1:%05d %05d  y0:%05d %05d y1:%05d %05d  z0:%05d %05d z1:%05d %05d U1:%04d U2:%04d T0:%04d\r\n",
						(int)myStr->point_i[ 0][circle],
						(int)myStr->point_i[ 1][circle],
						(int)myStr->point_i[ 2][circle],
						(int)myStr->point_i[ 3][circle],
						(int)myStr->point_i[ 4][circle],
						(int)myStr->point_i[ 5][circle],
						(int)myStr->point_i[ 6][circle],
						(int)myStr->point_i[ 7][circle],
						(int)myStr->point_i[ 8][circle],
						(int)myStr->point_i[ 9][circle],
						(int)myStr->point_i[10][circle],
						(int)myStr->point_i[11][circle],
						(int)myStr->point_i[12][circle],
						(int)myStr->point_i[13][circle],
						(int)myStr->point_i[14][circle] );

	DBG1("\t %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d \r\n",
						(int) myStr->zerone_i[ 0] ,
						(int) myStr->zerone_i[ 1] ,
						(int) myStr->zerone_i[ 2] ,
						(int) myStr->zerone_i[ 3] ,
						(int) myStr->zerone_i[ 4] ,
						(int) myStr->zerone_i[ 5] ,
						(int) myStr->zerone_i[ 6] ,
						(int) myStr->zerone_i[ 7] ,
						(int) myStr->zerone_i[ 8] ,
						(int) myStr->zerone_i[ 9] ,
						(int) myStr->zerone_i[10] ,
						(int) myStr->zerone_i[11] );
} //*****************************************************************************

//*****************************************************************************

void Set_Flag_1_Sec(uint8_t _flag)	{
	if ( _flag == 0 ) {
		flag_1_sec_u8 = 0;
	} else {
		flag_1_sec_u8 = 1;
	}
} //*****************************************************************************

uint8_t Get_Flag_1_Sec(void) {
	return flag_1_sec_u8;
} //*****************************************************************************

void Timer_Update( uint8_t _timer_u8, uint32_t _tim_value_u32) {
	timer_u32[_timer_u8] = _tim_value_u32;
} //*****************************************************************************

void Strobe_Y(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_Y_GPIO_Port, STROBE_Y_Pin, SET);
	local_delay_GRZ(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_Y_GPIO_Port, STROBE_Y_Pin, RESET);
} //***************************************************************************

void Strobe_X(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_X_GPIO_Port, STROBE_X_Pin, SET);
	local_delay_GRZ(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_X_GPIO_Port, STROBE_X_Pin, RESET);
} //***************************************************************************

void Strobe_Z(uint32_t _strobe_duration) {
	HAL_GPIO_WritePin(STROBE_Z_GPIO_Port, STROBE_Z_Pin, SET);
	local_delay_GRZ(_strobe_duration);
	HAL_GPIO_WritePin(STROBE_Z_GPIO_Port, STROBE_Z_Pin, RESET);
} //***************************************************************************

void local_delay_GRZ(uint32_t _delay) {
	for (uint32_t t=0; t<_delay; t++) {
		__asm("nop");
	}
} //***************************************************************************

//***************************************************************************
//void NRF24L01_Module(void) {
//	if (NRF24L01_DataReady()) {	/* If data is ready on NRF24L01+ */
//		NRF24L01_GetData(dataIn);	/* Get data from NRF24L01+ */
//		HAL_Delay(1);
//		DBG1("%s", dataIn); fflush(stdout);
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
//			DBG1("; Send back: OK\r\n");				/* Transmit went OK */
//		} else {
//			DBG1("; Send back: ERROR\r\n");			/* Message was LOST */
//		}
//
//		NRF24L01_PowerUpRx();	/* Go back to RX mode */
//		waitTime = 0;
//	} else {
//		if (HAL_GetTick() - lastTime > 250) {
//			if (waitTime == 0) {
//				DBG1("Waiting for data"); fflush(stdout);
//				waitTime++;
//			} else if (waitTime > 17) {
//				DBG1("\r\n");
//				waitTime = 0;
//			} else {
//				DBG1("."); fflush(stdout);
//				waitTime++;
//			}
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
//	DBG1("%05d\t%05d\t%05d\t%05d\t%05d\t%05d\t%05d\t%05d\t%04d\t%04d\r\n",
//						(int)value_i32[0],
//						(int)value_i32[1],
//						(int)value_i32[2],
//						(int)value_i32[3],
//						(int)value_i32[4],
//						(int)value_i32[5],
//						(int)value_i32[6],
//						(int)value_i32[7],
//						(int)adc_value_U,
//						(int)adc_value_T ); fflush(stdout);
//} //*****************************************************************************

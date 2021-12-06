#ifndef GROZA_T55_SM_H_
#define GROZA_T55_SM_H_
//******************************************************************************************

	#include "stm32f1xx_hal.h"
	#include "usart.h"
	#include "gpio.h"
	#include "stdio.h"
	#include <string.h>

	#include "groza-t55_config.h"
	#include "ringbuffer_dma_sm.h"
	#include "average_calc_3_from_5.h"
	#include "adc_light_stm32f103_hal_sm.h"
	#include "lcd1602_fc113_sm.h"
	#include "i2c_techmaker_sm.h"
	#include "nrf24l01_config.h"
	#include "nRF24L01_sm.h"

//******************************************************************************************

	void Groza_t55_init 	( void );
	void Groza_t55_main 	( uint8_t, char*, char* );
	void NRF24L01_Module	( void );
	int Value_T55 			( uint8_t );
	void Set_Flag_1_Sec		( uint8_t );
	uint8_t Get_Flag_1_Sec	( void );
	void Timer_Update		( uint8_t _timer_u8, uint32_t _tim_value_u32 );
	void TestStrobe 		( uint8_t _counter );

//******************************************************************************************
#endif

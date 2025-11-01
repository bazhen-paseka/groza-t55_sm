#ifndef GROZA_T55_SM_H_
#define GROZA_T55_SM_H_
//******************************************************************************************

	#include "stdio.h"
	#include <string.h>
	#include "main.h"
	#include "usart.h"
	#include "average_calc_3_from_5.h"
	#include "adc_light_stm32f103_hal_sm.h"
	#include "ds18b20_sm.h"
	#include "debug_gx.h"
	#include "local_key.h"
	#include "ringbuffer-gx.h"
	#include "esp8266_sm.h"

//******************************************************************************************

	typedef struct {
		int		point_i[DEVICE_QNT][CIRCLE_QNT];
		int		zerone_i[DEVICE_QNT] ;
	} PointStr;

//******************************************************************************************

	void Groza_2017_Init 	( void );
	void Groza_2017_Main 	( void );
//	void Groza_t55_test 	( void );
//	void NRF24L01_Module	( void );
	void Set_Flag_1_Sec		( uint8_t );
	uint8_t Get_Flag_1_Sec	( void );
	void Timer_Update		( uint8_t _timer_u8, uint32_t _tim_value_u32 );
	void Measurement 		( PointStr *myStr, uint8_t circle) ;

//******************************************************************************************
#endif

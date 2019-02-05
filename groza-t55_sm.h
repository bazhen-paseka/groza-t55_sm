#ifndef GROZA_T55_SM_H_
#define GROZA_T55_SM_H_

	#include "stm32f1xx_hal.h"
	#include "usart.h"
	#include "gpio.h"
	#include "adc_light_stm32f103_hal_sm.h"

	#define DEVICE_QNT 6
	#define CIRCLE_QNT 5

	void Groza_t55_init (void);
	void Groza_t55_main (uint8_t circle, char* http_req_1 );
	int Value_T55 (uint8_t);
	void Set_Flag_60_Sec(uint8_t);
	uint8_t Get_Flag_60_Sec(void);
	void Timer_Update( uint8_t _timer_u8, uint32_t _tim_value_u32);

#endif

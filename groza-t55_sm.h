#ifndef GROZA_T55_SM_H_
#define GROZA_T55_SM_H_

#include "stm32f1xx_hal.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

void Groza_t55_init (void);
void Groza_t55_main (void);

uint32_t Value_T55 (uint8_t);
void Set_Flag_60_Sec(uint8_t);
uint8_t Get_Flag_60_Sec(void);

#endif

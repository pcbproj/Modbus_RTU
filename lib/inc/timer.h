#ifndef TIMER_H
#define TIMER_H

#include "stm32f407xx.h"


void TIM2_InitIrqDisable(void);

void TIM2_StartIrqEnable(uint16_t cycles_number);

//uint8_t TIM2_Check(void);




#endif
#ifndef TIMER_H
#define TIMER_H

#include "stm32f407xx.h"


/******
Инициализация таймера TIM2 бес включения прерываний
*****/
void TIM2_Init(void);


/******
Запуск таймера TIM2 с прерыванием
*****/
void TIM2_StartIrq(uint16_t cycles_number);


/******
Запуск таймера TIM2 без прерываний
*****/
void TIM2_Start(uint16_t cycles_number);


/*******
Проверка, сработал таймер TIM2 или нет/
если сработал, то ф-ия возвращает 1
если не сработал, то ф-ия возвращает 0
********/
uint8_t TIM2_Check(void);



/*******
Остановка таймера TIM2 и сброс в 0
********/
void TIM2_StopClear(void);


#endif
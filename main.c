#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"

#include "main.h"

/*
Добавление протокола Modbus в ранее написанную программу. 
Команды передаются от ПК, исполнитель - МК.  
МК засвечивает светодиоды по команде с ПК.
отправляет состояние кнопок и отсчеты ADC по запросам.  

модель данных MODBUS RTU у нас будет такая: 
для каждого блока своя таблица. 

LEDS = Coils
Кнопки = Discretes Inputs
ADC = Input Registers

*/



void SysTick_Handler(void){		// прервание от Systick таймера, выполняющееся с периодом 1 мкс

	timer_counter();

	modbus_timers();

}


//void TIM2_IRQHandler(void){
//	TIM2->SR &= ~(TIM_SR_UIF);	// clear UIF flag
//	TIM2->CR1 &= ~(TIM_CR1_CEN);	// stop timer2 counting

//}



int main(void) {
	uint8_t rs485_rx_byte;
	uint8_t rs485_tx_array[12] = {"RS485 RX OK\n"};

	RCC_Init();

	__enable_irq();                         // global IRQ enable
		
	GPIO_Init();

	USART6_Init();

	SysTick_Config(SYSTICK_TIMER_CONST);		// systick period 1 us

	LED1_OFF();
	LED2_OFF();
	LED3_OFF();

	ModbusTimerStart(TIMER35_ID);		// wait 3.5 byte pause before Modbus TRU packer reception

	while(1){
			

		if( !( usart6_receive_byte( &rs485_rx_byte) ) ){	// if received byte
			LED2_ON();
			Delay_ms(LED_BLINK_300ms);
			
			LED2_OFF();

			usart6_send(rs485_tx_array, sizeof(rs485_tx_array));	// send this byte back


		}

	}	// while(1)
}	// main()

/*************************** End of file ****************************/

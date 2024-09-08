#include "timer.h"




void TIM2_InitIrqDisable(void){
	RCC->APB2ENR |= RCC_APB1ENR_TIM12EN;

	/* settings timer TIM2*/
	TIM2->PSC = 83;					// TIM2 clock 1 MHz
	TIM2->CR1 &= ~(TIM_CR1_DIR);	// TIM2 upcounter
	//TIM2->ARR = 999;				// counts number 
	//TIM2->CR1  |= TIM_CR1_CEN;
	//TIM2->EGR  |= TIM_EGR_UG; 
	NVIC_DisableIRQ(TIM2_IRQn);
}



void TIM2_StartIrqEnable(uint16_t cycles_number){
	TIM2->ARR = cycles_number - 1;
	TIM2->CR1  |= TIM_CR1_CEN;		// timer 2 clock enable
	TIM2->EGR  |= TIM_EGR_UG;		// timer 2 counter clear
	TIM2->SR &= ~(TIM_SR_UIF);		// clear UIF flag
	NVIC_EnableIRQ(TIM2_IRQn);		// timer 2 interrupt enable
}

#include "modbus_rtu.h"

uint8_t timer15_done_flag = 0;
uint8_t timer35_done_flag = 0;

uint16_t timer15 = 0;
uint16_t timer35 = 0;

uint16_t Enable15 = 0;
uint16_t Enable35 = 0;



void modbus_timers(void){
	if(Enable15){ 
		if(timer15 < DELAY_1_5_BYTE_US) {
			timer15_done_flag = 0;
			timer15++;
		}
		else timer15_done_flag = 1;
	}
	
	
	if(Enable35){
		if(timer35 < DELAY_3_5_BYTE_US){
			timer35_done_flag = 0;
			timer35++;
		}
		else timer35_done_flag = 1;
	}
}



void ModbusTimerStart(uint8_t timer_ID){
	switch(timer_ID){
		case TIMER15_ID:
			timer15 = 0;
			Enable15 = 1;
			timer15_done_flag = 0;
			break;

		case TIMER35_ID:
			timer35 = 0;
			Enable35 = 1;
			timer35_done_flag = 0;
			break;

		default:
			__NOP();
	}

}


void ModbusTimerStopClear(uint8_t timer_ID){
	switch(timer_ID){
		case TIMER15_ID:
			timer15 = 0;
			Enable15 = 0;
			timer15_done_flag = 0;
			break;

		case TIMER35_ID:
			timer35 = 0;
			Enable35 = 0;
			timer35_done_flag = 0;
			break;

		default:
			__NOP();
	}

}



uint8_t ModbusReceiveByte(uint8_t usart_number, uint8_t *rx_byte){
	uint8_t timer = 0;
	
	switch(usart_number){
	case 1:
		while(!(USART1->SR & USART_SR_RXNE)){
			if(timer15_done_flag) return MODBUS_RX_DONE;
		}	
		*rx_byte = USART1 -> DR;
		break;
	
	case 2:
		while(!(USART2->SR & USART_SR_RXNE)){
			if(timer15_done_flag) return MODBUS_RX_DONE;
		}	
		*rx_byte = USART2 -> DR;
		break;
	
	case 6:
		while(!(USART6->SR & USART_SR_RXNE)){
			if(timer15_done_flag) return MODBUS_RX_DONE;
		}	
		*rx_byte = USART6 -> DR;
		break;
	
	default: 
		__NOP();
	}
	ModbusTimerStopClear(TIMER15_ID);
	return MODBUS_OK;
}



uint8_t RequestReceive(uint8_t rx_array[], uint8_t *rx_array_len){
	uint8_t rx_byte;
	uint8_t rx_byte_num = 0;
	uint8_t rx_err;

	while(!timer35_done_flag){}		// wait for 3.5 byte time 
	ModbusTimerStopClear(TIMER35_ID);

	// тут цикл пока не получим MODBUS_RX_DONE. 
	// Т.е. пока пауза м/у байтами не будет больше 1,5 байта по времени
	while(rx_err != MODBUS_RX_DONE){
		rx_err = ModbusReceiveByte(MODBUS_USART, &rx_byte);	// receive first byte = Address
		ModbusTimerStart(TIMER15_ID);
		rx_array[rx_byte_num] = rx_byte;
		rx_byte_num++;
		if(rx_byte_num == 0xFF) return ERROR_PACK_LEN;
	}
	
	*rx_array_len = rx_byte_num;
	ModbusTimerStart(TIMER35_ID);
	return MODBUS_OK;
}



uint8_t OperationExec(uint8_t rx_request[], 
						uint8_t *request_len,
						uint8_t tx_answer[], 
						uint8_t *answer_len )
						{

}



uint8_t AnswerTransmit(uint8_t tx_array[], uint8_t *tx_array_len){

}
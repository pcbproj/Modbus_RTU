#include "modbus_rtu.h"

//uint8_t timer15_done_flag = 0;
//uint8_t timer35_done_flag = 0;

//uint16_t timer15 = 0;
//uint16_t timer35 = 0;

//uint16_t Enable15 = 0;
//uint16_t Enable35 = 0;



//void modbus_timers(void){
//	if(Enable15){ 
//		if(timer15 < DELAY_1_5_BYTE_US) {
//			timer15_done_flag = 0;
//			timer15++;
//		}
//		else timer15_done_flag = 1;
//	}
//	else{
//		if(Enable35){
//			if(timer35 < DELAY_3_5_BYTE_US){
//				timer35_done_flag = 0;
//				timer35++;
//			}
//			else timer35_done_flag = 1;
//		}
//	}
//}



void ModbusTimerStart(uint16_t timer_cycles){
	TIM2_Start(timer_cycles);
}


void ModbusTimerStopClear(void){
	TIM2_StopClear();
}


uint8_t ModbusReceiveFirstByte(uint8_t usart_number, uint8_t *rx_byte){
	ModbusTimerStart(DELAY_3_5_BYTE_US);
	while(!TIM2_Check()){
		
		if((USART6->SR & USART_SR_RXNE)){
			*rx_byte = USART6 -> DR;
			ModbusTimerStart(DELAY_2_5_BYTE_US);	// start timer (1.5 + 1) byte  
			return MODBUS_OK;
		}

	}	// wait for 3.5 byte time pause
	
	return ERROR_PACK_LEN;
	
}


uint8_t ModbusReceiveByte(uint8_t usart_number, uint8_t *rx_byte){
	
	while(!(USART6->SR & USART_SR_RXNE)){
			if(TIM2_Check()) return MODBUS_RX_DONE;	 // if timer(1.5 + 1) byte done before byte was received
	}
	*rx_byte = USART6 -> DR;
	ModbusTimerStart(DELAY_2_5_BYTE_US);	// start timer (1.5 + 1) byte  
	
	return MODBUS_OK;
}





uint8_t RequestReceive(uint8_t rx_array[], uint8_t *rx_array_len){
	uint8_t rx_byte;
	uint8_t rx_byte_num = 0;
	uint8_t rx_err;
	
	
	
	// waiting of first Request byte
	rx_byte_num = 0;
	rx_err = ModbusReceiveFirstByte(MODBUS_USART, &rx_byte); // started timer (1.5 + 1) byte
	rx_array[rx_byte_num] = rx_byte;
	
	// цикл пока не получим MODBUS_RX_DONE. 
	// Т.е. пока пауза м/у приемами байтов не будет больше 1,5 + 1 байт по времени
	while(rx_err != MODBUS_RX_DONE){
		if( rx_err != ERROR_PACK_LEN){
			rx_array[rx_byte_num] = rx_byte;
			rx_byte_num++;
			rx_err = ModbusReceiveByte(MODBUS_USART, &rx_byte);	// receive first byte = Address
			if(rx_byte_num == 0xFF) return ERROR_PACK_LEN;
		}
		else return ERROR_PACK_LEN;
	}
	
	*rx_array_len = rx_byte_num;
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
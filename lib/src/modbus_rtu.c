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

// TODO: переписать функцию.
// чтобы пауза в 3,5 байта выдерживалась и только после этого проверялся приемник USART
// а если во время паузы 3,5 байта приходит байт на USART, то перезапускать таймер 3,5 байта
// лучше начинать прием пакета по прерыванию от USART, чтобы не звисать в ожидании начала пакета.
uint8_t ModbusReceiveFirstByte(uint8_t usart_number, uint8_t *rx_byte){
	ModbusTimerStart(DELAY_3_5_BYTE_US);
	
	// TODO??? переписать while(!TIM2_Check()){
	// пока не TIM2_CHECK() и если приходит байт, то таймер должен перезапускаться, 
	// чтобы пауза была не менее 3,5 байта длительностью.
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
	
	
	
	// waiting for first byte
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






uint8_t GetOperationCode(uint8_t rx_request[], uint8_t *op_code_out){
	uint8_t op_code_rx = rx_request[1];
	if(( op_code_rx == READ_COILS ) ||
		( op_code_rx == READ_DISCRETE_INPUTS ) ||
		( op_code_rx == READ_INPUT_REGISTERS ) ||
		( op_code_rx == WRITE_SINGLE_COIL ) ||
		( op_code_rx == WRITE_MULTI_COILS ) ){

		*op_code_out = op_code_rx;
		return MODBUS_OK;
	} 
	else return ERROR_OP_CODE;
}




uint8_t CheckDataAddress(uint8_t op_code_in, uint8_t rx_request[]){
	uint16_t start_addr_rx = (rx_request[2] << 8) + rx_request[3];
	
	switch(op_code_in){
	case(READ_COILS):
		if((start_addr_rx >= 0) && (start_addr_rx < COILS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_ADDR;
		break;

	case(READ_DISCRETE_INPUTS):
		if((start_addr_rx >= 0) && (start_addr_rx < DISCRETE_INPUTS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_ADDR;
		break;

	case(READ_INPUT_REGISTERS):
		if((start_addr_rx >= 0) && (start_addr_rx < INPUT_REGS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_ADDR;
		break;

	case(WRITE_SINGLE_COIL):
		if((start_addr_rx >= 0) && (start_addr_rx < COILS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_ADDR;
		break;

	case(WRITE_MULTI_COILS):
		if((start_addr_rx >= 0) && (start_addr_rx < COILS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_ADDR;
		break;

	}

}


uint8_t ExecOperation(uint8_t op_code, 
						uint8_t rx_request[], 
						uint8_t req_len, 
						uint8_t tx_answer[], 
						uint8_t *answer_len){
	
	uint16_t start_addr_rx;
	uint16_t quantity_rx;
	uint8_t bytes_number_rx;

	// switch(OperationCode) для обработки полей DATA
		
			
	// для каждого case написать свою функцию выполнения операции		
	switch(op_code){
	case(READ_COILS):
		
		return ERROR_EXECUTION;
		break;

	case(READ_DISCRETE_INPUTS):
		
		return ERROR_EXECUTION;
		break;

	case(READ_INPUT_REGISTERS):
		
		return ERROR_EXECUTION;
		break;

	case(WRITE_SINGLE_COIL):

		break;

	case(WRITE_MULTI_COILS):

		break;

	}
}


// Адрес данных верный?
// Значение данных верное? В адекватном диапазоне?
// Выполнение требуемой операции
// вычисление CRC16 для ответного пакета
// формирование ответного пакета
uint8_t RequestParsingOperationExec(uint8_t rx_request[], 
						uint8_t *request_len,
						uint8_t tx_answer[], 
						uint8_t *answer_len )
						{

	uint8_t err;
	uint16_t crc;
	uint16_t crc_rx;
	uint8_t op_code_rx;
	uint16_t start_addr_rx;
	uint16_t quantity_rx;
	uint8_t bytes_number_rx;


	// if Device Address match
	if(rx_request[0] == DEVICE_ADDR){
		
		// TODO: check this formula!!
		crc_rx = (rx_request[((*request_len) - 1)] << 8) + (rx_request[((*request_len) - 2)] & 0x00FF);
		// CRC16 compare
		crc = CRC16_Calc(rx_request, ((*request_len) - 2));
		
		if(crc == crc_rx) {		// Get OperationCode value
			err = GetOperationCode(rx_request, &op_code_rx);
		}
		else 
			return ERROR_CRC;

		if(err == MODBUS_OK){	// check data address 
			err = CheckDataAddress(op_code_rx, rx_request);
		}
		else 
			return ERROR_DATA_ADDR;
		
		if(err == MODBUS_OK){	// check data value
			//err = CheckDataValue(); 
		}
		else 
			return ERROR_DATA_VAL;

		if(err == MODBUS_OK){	// operation execution
			//err = OperationExec();
		}
		else 
			return ERROR_EXECUTION;

	}
	else{ // TODO: answer error array assebmly
		err = ERROR_DEV_ADDR;
	}
	return err;
}



uint8_t AnswerTransmit(uint8_t tx_array[], uint8_t *tx_array_len){

}
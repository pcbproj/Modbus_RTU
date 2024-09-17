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


/*
	выход с номером 1 адресуется как 0.
*/
uint8_t CheckDataValue(uint8_t op_code_in, uint8_t rx_request[]){
	uint16_t start_addr_rx = (rx_request[2] << 8) + rx_request[3];
	uint16_t quantity_rx = (rx_request[4] << 8) + rx_request[5];
	uint16_t rx_data_range = start_addr_rx + quantity_rx;


	switch(op_code_in){
	case(READ_COILS):
		if((rx_data_range >= 0) && (rx_data_range <= COILS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_VAL;
		break;

	case(READ_DISCRETE_INPUTS):
		if((rx_data_range > 0) && (rx_data_range <= DISCRETE_INPUTS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_VAL;
		break;

	case(READ_INPUT_REGISTERS):
		if((rx_data_range > 0) && (rx_data_range <= INPUT_REGS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_VAL;
		break;

	case(WRITE_SINGLE_COIL):
		if((rx_data_range > 0) && (rx_data_range <= COILS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_VAL;
		break;

	case(WRITE_MULTI_COILS):
		if((rx_data_range > 0) && (rx_data_range <= COILS_NUM)){
			return MODBUS_OK;
		}
		else return ERROR_DATA_VAL;
		break;

	}


}



uint8_t Exec_READ_COILS( uint16_t start_addr_in, 
							uint16_t quantity_in, 
							uint8_t answer_tx[],
							uint8_t *answer_len){
	uint8_t bytes_num = 1;
	uint8_t Value_D0 = (((LED1_PORT -> ODR) & 0xE0) >> (LED1_PIN_NUM + start_addr_in));	// 0xE0 = pins 13 - 15 masked
	answer_tx[0] = DEVICE_ADDR;
	answer_tx[1] = READ_COILS;
	answer_tx[2] = bytes_num;
	answer_tx[3] = Value_D0;
	*answer_len = bytes_num + 3; // answer_len = all listed bytes, without CRC16 bytes

	return MODBUS_OK;

}


uint8_t Exec_READ_DISCRETE_INPUTS( uint16_t start_addr_in, 
							uint16_t quantity_in, 
							uint8_t answer_tx[],
							uint8_t *answer_len){

	uint8_t bytes_num = 1;
	
	uint8_t Value_D0 = (((BTN_PORT -> IDR) & 0x1C) >> (BTN1_PIN_NUM + start_addr_in));	// 0x1C = pins 10 - 12 masked

	answer_tx[0] = DEVICE_ADDR;
	answer_tx[1] = READ_DISCRETE_INPUTS;
	answer_tx[2] = bytes_num;
	answer_tx[3] = Value_D0;
	*answer_len = bytes_num + 3; // answer_len = all listed bytes, without CRC16 bytes

	return MODBUS_OK;

}





uint8_t Exec_READ_INPUT_REGISTERS( uint16_t start_addr_in, 
							uint16_t quantity_in, 
							uint8_t answer_tx[],
							uint8_t *answer_len){
	uint8_t bytes_num = 2;
	uint16_t input_reg = ADC1_Read();
	
	answer_tx[0] = DEVICE_ADDR;
	answer_tx[1] = READ_INPUT_REGISTERS;
	answer_tx[2] = bytes_num;
	answer_tx[3] = (input_reg >> 8);		// MSB of input_reg
	answer_tx[4] = (input_reg & 0x00FF);	// LSB of input_reg

	*answer_len = bytes_num + 3; // answer_len = all listed bytes, without CRC16 bytes
	
	return MODBUS_OK;
}







/*
	выход с номером 1 адресуется как 0.
*/
uint8_t Exec_WRITE_SINGLE_COIL( uint16_t start_addr_in, 
							uint16_t value_in, 
							uint8_t answer_tx[],
							uint8_t *answer_len){
	uint8_t bytes_num = 1;
	if(value_in = COIL_ON_CODE){
		LED1_PORT -> ODR &= ~( 1 << ( LED1_PIN_NUM + start_addr_in ) );	// LED ON
		
		answer_tx[0] = DEVICE_ADDR;
		answer_tx[1] = WRITE_SINGLE_COIL;
		answer_tx[2] = ( start_addr_in >> 8 );
		answer_tx[3] = ( start_addr_in & 0x00FF );
		answer_tx[4] = ( value_in >> 8 );
		answer_tx[5] = ( value_in & 0x00FF );
	
		*answer_len = 6;

		return MODBUS_OK;

	}
	else{
		if(value_in == COIL_OFF_CODE){
			LED1_PORT -> ODR |= ( 1 << ( LED1_PIN_NUM + start_addr_in ) );	// LED OFF

			answer_tx[0] = DEVICE_ADDR;
			answer_tx[1] = WRITE_SINGLE_COIL;
			answer_tx[2] = ( start_addr_in >> 8 );
			answer_tx[3] = ( start_addr_in & 0x00FF );
			answer_tx[4] = ( value_in >> 8 );
			answer_tx[5] = ( value_in & 0x00FF );
			
			*answer_len = 6;

			return MODBUS_OK;

		}
		else return ERROR_EXECUTION;
	}
}






uint8_t Exec_WRITE_MULTI_COILS(uint8_t rx_request[],
							uint8_t req_len, 
							uint8_t answer_tx[],
							uint8_t *answer_len){
	
	uint16_t start_addr_in = ( rx_request[2] << 8 ) +  rx_request[3];
	uint16_t quantity_rx = ( rx_request[4] << 8 ) +  rx_request[5];
	
	uint8_t bytes_num = rx_request[6];
	uint8_t CoilsPortValue = ( ( ( rx_request[7] << 8 ) + rx_request[8] ) & 0x0007) ^ 0x0007; // (XOR ^) inversion couse LEDs turned by zero.

	uint16_t turn_on_coils_num = 0;


	LED1_PORT->ODR = ( CoilsPortValue << LED1_PIN_NUM );
	
	// calculate turn on coils number
	for(uint8_t j = 0; j < 2 * quantity_rx; j++){
		uint8_t data_byte = rx_request[7+j];
		
		for(uint8_t i = 0; i < 8; i++){
			if (data_byte & 0x01) turn_on_coils_num++;
			data_byte = data_byte >> 1;
		}
	}
	
	answer_tx[0] = DEVICE_ADDR;
	answer_tx[1] = WRITE_MULTI_COILS;
	answer_tx[2] = ( start_addr_in >> 8 );
	answer_tx[3] = ( start_addr_in & 0x00FF );
	answer_tx[4] = ( turn_on_coils_num >> 8 );
	answer_tx[5] = ( turn_on_coils_num & 0x00FF );
	
	*answer_len = 6;

	return MODBUS_OK;
}






uint8_t ExecOperation(uint8_t op_code, 
						uint8_t rx_request[], 
						uint8_t req_len, 
						uint8_t tx_answer[], 
						uint8_t *answer_len){
	
	uint16_t start_addr_rx = (rx_request[2] << 8) + rx_request[3];
	uint16_t quantity_rx = (rx_request[4] << 8) + rx_request[5];
	uint8_t bytes_number_rx;
	uint8_t err;
	uint8_t answer_array[256];
	uint8_t array_answer_len = 0;

			
	// для каждого case написать свою функцию выполнения операции		
	switch(op_code){
	case(READ_COILS):
		err = Exec_READ_COILS(start_addr_rx, quantity_rx, answer_array, &array_answer_len);
		break;

	case(READ_DISCRETE_INPUTS):
		err = Exec_READ_DISCRETE_INPUTS(start_addr_rx, quantity_rx, answer_array, &array_answer_len);
		break;

	case(READ_INPUT_REGISTERS):
		err = Exec_READ_INPUT_REGISTERS(start_addr_rx, quantity_rx, answer_array, &array_answer_len);
		break;

	case(WRITE_SINGLE_COIL):
		err = Exec_WRITE_SINGLE_COIL(start_addr_rx, quantity_rx, answer_array, &array_answer_len);
		break;

	case(WRITE_MULTI_COILS):
		err = Exec_WRITE_MULTI_COILS(rx_request, req_len, answer_array, &array_answer_len);
		break;

	}
	
	*answer_len = array_answer_len;
	for(uint8_t i=0; i < array_answer_len; i++) tx_answer[i] = answer_array[i];
	
	return err;
}


// Адрес данных верный?
// Значение данных верное? В адекватном диапазоне?
// Выполнение требуемой операции
// вычисление CRC16 для ответного пакета
// формирование ответного пакета
uint8_t RequestParsingOperationExec(uint8_t rx_request[], 
						uint8_t request_len,
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
	uint8_t tx_answer_tmp[256];
	uint8_t answer_len_tmp;


	// if Device Address match
	if(rx_request[0] == DEVICE_ADDR){
		
		// TODO: check this formula!!
		crc_rx = (rx_request[((request_len) - 1)] << 8) + (rx_request[((request_len) - 2)] & 0x00FF);
		// CRC16 compare
		crc = CRC16_Calc(rx_request, ((request_len) - 2));
		
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
			err = CheckDataValue(op_code_rx, rx_request); 
		}
		else 
			return ERROR_DATA_VAL;

		if(err == MODBUS_OK){	// operation execution
			err = ExecOperation(op_code_rx, rx_request, request_len, tx_answer_tmp, &answer_len_tmp);
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
#ifndef MODBUS_H
#define MODBUS_H

#include "stm32f407xx.h"
#include "usart.h"
#include "crc16.h"

//---- Modbus command codes ------------
#define READ_COILS				0x01
#define READ_DISCRETE_INPUTS	0x02
#define READ_INPUT_REGISTERS	0x04
#define WRITE_SINGLE_COIL		0x05	
#define WRITE_MULTI_COILS		0x0F


//-------- Modbus Error codes ----------
#define ERROR_OP_CODE			0x01
#define ERROR_ADDR				0x02
#define ERROR_DATA_VAL			0x03
#define ERROR_EXECUTION			0x04	

//------- Modbus device address----------
#define DEVICE_ADDR				0xAD

//------Modbus internal addresses--------
// LEDS, BTNS, ADC_data



#endif

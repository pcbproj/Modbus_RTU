#ifndef MODBUS_H
#define MODBUS_H

#include "stm32f407xx.h"
#include "usart.h"
#include "gpio.h"
#include "crc16.h"
#include "delay.h"
#include "timer.h"

#define MODBUS_USART		6

#define USART_BAUD_USED		115200
#define USART_BIT_TIME_US	1000000 / USART_BAUD_USED
#define USART_BYTE_TIME_US	USART_BIT_TIME_US * 8

#define DELAY_1_5_BYTE_US	( USART_BYTE_TIME_US * 1.5 + 1 )
#define DELAY_3_5_BYTE_US	( USART_BYTE_TIME_US * 3.5 + 1 )

#define TIMER15_ID				15
#define TIMER35_ID				35


//---- Modbus command codes ------------
#define READ_COILS				0x01
#define READ_DISCRETE_INPUTS	0x02
#define READ_INPUT_REGISTERS	0x04
#define WRITE_SINGLE_COIL		0x05	
#define WRITE_MULTI_COILS		0x0F

#define ANSWER_ADD				0x80	// add for answer command code


//-------- Modbus Error codes ----------
#define MODBUS_OK				0x00
#define ERROR_OP_CODE			0x01
#define ERROR_ADDR				0x02
#define ERROR_DATA_VAL			0x03
#define ERROR_EXECUTION			0x04	
#define ERROR_05				0x05	// reserved
#define ERROR_06				0x06	// reserved


#define ERROR_CRC				0x0F
#define ERROR_PACK_LEN			0x1F


//------- Modbus device address----------
#define DEVICE_ADDR				0xAD

//------Modbus internal addresses--------
// LEDS, BTNS, ADC_data
#define COILS_NUM				0x03	// LEDS
#define DISCRETE_INPUTS_NUM		0x03	// BTNS
#define INPUT_REGS_NUM			0x01	// ADC data

#define TIMER_DONE				0x01
#define TIMER_WORKS				0x00


/*******
Функция реализации таймеров для определения временных задержек протокола Modbus RTU
вызывается только в прерывании Systick timer с частотой 1 МГц
*******/
void modbus_timers(void);

/******
Функция запуска программного таймера timer15 или timer35.
если timer_ID = 15, запускается timer15
	выставляется Enable15 = 1;
	сбрасывается счетчик timer15 = 0;
если timer_ID = 35, запускается timer35
	выставляется Enable35 = 1;
	сбрасывается счетчик timer35 = 0;

*******/
void ModbusTimerStart(uint8_t timer_ID);


void ModbusTimerStopClear(uint8_t timer_ID);





/*******
Функция принимает байты запроса с определеием максимального времени паузы между байтами = 1,5 байта.
Если пауза между байтами больше чем 1,5 байта, то пакет считается поврежденным.
Функция возвращает код ошибки: 
	MODBUS_OK - если байт принят с правильной паузой. 
	ERROR_PACK_LEN - если байт принят с длинной задержкой.
*******/
uint8_t ModbusReceiveByte(uint8_t usart_number, uint8_t *rx_byte);


/*********
Ф-ия принимает пакет согласно протоколу Modbus RTU.
Измеряются и выдержтваются временные интервалы 1,5 байта м/у байтами
и 3,5 байта м/у пакетами и вычисляется CRC16.
Ф-ия возвращает код ошибки Modbus Error codes или MODBUS_OK или ERROR_CRC.
Принятый пакет сохраняет в массив rx_array[] с указанием общей длины принятого пакета в rx_array_len
Последние 2 байта в принятом массиве это CRC16. 
*********/
uint8_t RequestReceive(uint8_t rx_array[], uint8_t *rx_array_len);



/******
Ф-ия разбирает массив принятого пакета и выполняет требуемые операции.
В зависимости от кода операции по разному обрабатываются поле DATA
возвращает коды ошибок Modbus Error codes если были ошибки,
или MODBUS_OK, если операция выполнена успешно.
входные параметры - массив с запросом rx_request[] и длина этого массива request_len.
функция формирует массив для ответного пакета - tx_answer[] и длина его - answer_len . 
CRC16 также вычисляется для выходного пакета.
******/
uint8_t OperationExec(uint8_t rx_request[],		// received request array
						uint8_t *request_len,		// request array length in bytes
						uint8_t tx_answer[],		// tx_answer array
						uint8_t *answer_len			// answer array length in bytes
						);



/********
Ф-ия отправки ответного сообщения. 
********/
uint8_t AnswerTransmit(uint8_t tx_array[], uint8_t *tx_array_len);





#endif

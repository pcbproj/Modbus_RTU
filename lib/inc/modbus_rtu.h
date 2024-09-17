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
#define USART_BYTE_TIME_US	USART_BIT_TIME_US * 12	// BYTE send in 12 bits maximum

#define DELAY_1_5_BYTE_US	( USART_BYTE_TIME_US * 1.5 + 1 )
#define DELAY_2_5_BYTE_US	( USART_BYTE_TIME_US * 2.5 + 1 )
#define DELAY_3_5_BYTE_US	( USART_BYTE_TIME_US * 3.5 + 1 )

//#define TIMER15_CYCLES				15
//#define TIMER35_CYCLES				35


//---- Modbus command codes ------------
#define READ_COILS				0x01
#define READ_DISCRETE_INPUTS	0x02
#define READ_INPUT_REGISTERS	0x04
#define WRITE_SINGLE_COIL		0x05	
#define WRITE_MULTI_COILS		0x0F


#define ANSWER_ADD				0x80	// add for answer command code


//-------- Modbus ERROR codes ----------
#define ERROR_OP_CODE			0x01
#define ERROR_DATA_ADDR			0x02
#define ERROR_DATA_VAL			0x03
#define ERROR_EXECUTION			0x04	
#define ERROR_05				0x05	// reserved
#define ERROR_06				0x06	// reserved

//-------- Internal ERROR Codes --------------
#define MODBUS_OK				0x00
#define ERROR_CRC				0x0F	// ошибка по CRC16 
#define ERROR_PACK_LEN			0x1F	// неверная длина пакета
#define MODBUS_RX_DONE			0x2F	// прием пакета завершен
#define ERROR_DEV_ADDR			0x3F	// неверный адрес устройства в пакете

//------- Modbus device address----------
#define DEVICE_ADDR				0xAD

//------Modbus internal addresses--------
// LEDS, BTNS, ADC_data
#define COILS_NUM				3	// LEDS
#define DISCRETE_INPUTS_NUM		3	// BTNS
#define INPUT_REGS_NUM			1	// ADC data

//#define TIMER_DONE				0x01
//#define TIMER_WORKS				0x00


/*******
Функция реализации таймеров для определения временных задержек протокола Modbus RTU
вызывается только в прерывании Systick timer с частотой 1 МГц
*******/
//void modbus_timers(void);

/******
Функция запуска таймера Modbus.
*******/
void ModbusTimerStart(uint16_t timer_cycles);


void ModbusTimerStopClear(void);





/*******
Функция принимает байты запроса с определеием максимального времени паузы между байтами = 1,5 байта.
Если пауза между байтами больше чем 1,5 байта, то пакет считается принятым.
Функция возвращает код ошибки: 
	MODBUS_OK - если байт принят с правильной паузой. 
	MODBUS_RX_DONE - предыдущий принятый байт был последним байтом пакета.
*******/
uint8_t ModbusReceiveByte(uint8_t usart_number, uint8_t *rx_byte);


/*********
Ф-ия принимает пакет согласно протоколу Modbus RTU.
Измеряются и выдержтваются временные интервалы 1,5 байта м/у байтами

Ф-ия возвращает: 
	код ошибки ERROR_PACK_LEN если пакет больше максимальной длины
	MODBUS_OK если длина пакета на максимальная.
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
uint8_t RequestParsingOperationExec(uint8_t rx_request[],		// received request array
						uint8_t *request_len,		// request array length in bytes
						uint8_t tx_answer[],		// tx_answer array
						uint8_t *answer_len			// answer array length in bytes
						);





/*******
ф-ия возвращает код ошибки если полученный код операции не поддерживается
или возвращает MODBUS_OK если полученный код операции поддерживается 
в выходной параметр op_code_out сохраняется значение кода операции
*******/
uint8_t GetOperationCode(uint8_t rx_request[], uint8_t *op_code_out);
	
	
	


/********
Ф-ия возвращает код ошибки, если адрес данных больше чем адресуемых объектов в устройстве
или возвращает MODBUS_OK, если адресация данных верная.
*******/
uint8_t CheckDataAddress(uint8_t op_code, uint8_t rx_request[]);		






/*******
Ф-ия проверяет правильность поля DATA и выполняет команду по запросу.
	op_code			- код операции в принятом запросе
	rx_request[]	- массив запроса
	req_len,		- длина массива запроса
	tx_answer[]		- выходной массив ответа. CRC16 посчитано и добавлено в конец массива 
					(младший байт CRC идет первый, потом - старший)
	*answer_len		- длина выходного массива ответа


*******/
uint8_t ExecOperation(uint8_t op_code, 
						uint8_t rx_request[], 
						uint8_t req_len, 
						uint8_t tx_answer[], 
						uint8_t *answer_len);
						
									

/********
Ф-ия отправки ответного сообщения. 
********/
uint8_t AnswerTransmit(uint8_t tx_array[], uint8_t *tx_array_len);





#endif

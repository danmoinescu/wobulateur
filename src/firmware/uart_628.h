#include <avr/io.h>

#define F_CPU 16000000

//set desired baud rate
#define BAUDRATE 9600

//calculate UBRR value
#define UBRRVAL ((F_CPU/(BAUDRATE*16UL))-1)

//define receive parameters
#define SYNC 0XAA// synchro signal
#define RADDR 0x44
#define LEDON 0x11//switch led on command
#define LEDOFF 0x22//switch led off command

/*
** function prototypes
*/

extern void USART_Init(void);

extern uint8_t USART_RxByte(void);

extern void USART_TxByte(uint8_t u8Data);

extern void USART_TxStr(char *CH1);


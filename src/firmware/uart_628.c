
#include "uart_628.h"



void USART_Init(void)
{
//Set baud rate
	UBRRL=(uint8_t)UBRRVAL;	//low byte
	UBRRH=(UBRRVAL>>8);		//high byte

//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|

	(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);	

//Enable Transmitter and Receiver and Interrupt on receive complete
	UCSRB=(1<<RXEN)|(1<<RXCIE) | (1<<TXEN);
	
}


uint8_t USART_RxByte(void)
{
    // Wait until a byte has been received
    while((UCSRA&(1<<RXC)) == 0);

    // Return received data
    return UDR;
}


void USART_TxByte(uint8_t u8Data)
{
// Wait if a byte is being transmitted
    while((UCSRA&(1<<UDRE)) == 0);

// Transmit data
    UDR = u8Data;  
}


void USART_TxStr(char *CH1)
{
	uint8_t i;
	i=0;
	while (CH1[i] != '\0')
	{
		USART_TxByte(CH1[i]);
		i++;
	}
}



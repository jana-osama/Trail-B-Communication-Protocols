
#include <avr/io.h>
#include "uart.h"
#include "BIT_MATH.h"
#include "STD_TYPES.h"

#define F_CPU 1000000UL
#define SCL_FREQ 50000UL
#include <util/delay.h>

void UART_Init(uint32_t baudrate)
{
	//UBRR0H = 0;
	//UBRR0L = 103;
	
	/*Set baud rate */
	u8 ubrr = (F_CPU / (SCL_FREQ * baudrate)) - 1;
	
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	
	
	/* Mode Select Asynchronous*/
	CLR_BIT(UCSR0C,UMSEL01);
	CLR_BIT(UCSR0C,UMSEL00);
	
	/*No Parity*/
	CLR_BIT(UCSR0C,UPM01);
	CLR_BIT(UCSR0C,UPM00);
	
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/*1 Stop Bit*/
	CLR_BIT(UCSR0C,USBS0);
	
	/*Character Size --> 8*/
	CLR_BIT(UCSR0B,UCSZ02);
	SET_BIT(UCSR0C,UCSZ01);
	SET_BIT(UCSR0C,UCSZ00);
}


void UART_SendChar(char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void UART_SendString(const char *str)
{
	while(*str)
	{
		UART_SendChar(*str);
		str++;
	}
}

char UART_ReceiveChar(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	
	/* Get and return received data from buffer */
	return UDR0;
}

void UART_SendHex(uint8_t data)
{
	const char hex[] = "0123456789ABCDEF";

	UART_SendChar(hex[(data >> 4) & 0x0F]);
	UART_SendChar(hex[data & 0x0F]);
}

void UART_ReceiveString(char *str)     //read full line until enter
{
	if(*str = UART_ReceiveChar() != '\r')
	{		
		str++;
	}
	*str = '\0';
}
uint8_t UART_DataAvailable(void)
{
	return (UCSR0A & (1 << RXC0));
}
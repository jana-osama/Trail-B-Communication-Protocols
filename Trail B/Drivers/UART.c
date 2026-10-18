
#include <avr/io.h>
#include "uart.h"

#define F_CPU 1000000UL
#include <util/delay.h>

void UART_Init(uint32_t baudrate)
{

}


void UART_SendChar(char data)
{

}

void UART_SendString(const char *str)
{

}

char UART_ReceiveChar(void)
{
	
}
void UART_SendHex(uint8_t data)
{
	const char hex[] = "0123456789ABCDEF";

	UART_SendChar(hex[(data >> 4) & 0x0F]);
	UART_SendChar(hex[data & 0x0F]);
}
void UART_ReceiveString(char *str)     //read full line until enter
{
	
	
}
uint8_t UART_DataAvailable(void)
{
	return (UCSR0A & (1 << RXC0));
}


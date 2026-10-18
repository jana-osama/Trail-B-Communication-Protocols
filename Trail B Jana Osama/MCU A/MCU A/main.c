/*
 * MCU A.c
 *
 * Created: 10/11/2026 12:05:23 PM
 * Author : Jana.Osama
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "UART.h"
#include "SPI.h"


#define B1_PIN         PD2
#define B2_PIN         PD3

#define DIP_MASK       0x0F
#define SPI_START      0xAA


void DIP_Init(void)
{
    /* INPUT*/

    DDRC &= ~(1 << PC0);
    DDRC &= ~(1 << PC1);
    DDRC &= ~(1 << PC2);
    DDRC &= ~(1 << PC3);
	
	/*OUt put*/
    PORTC |= (1 << PC0);
    PORTC |= (1 << PC1);
    PORTC |= (1 << PC2);
    PORTC |= (1 << PC3);
}


// read dip
uint8_t DIP_Read(void)
{
    return ((~PINC) & DIP_MASK);
}


uint8_t DIP_To_Temperature(void)
{
    return DIP_Read() + 15;
}


// init buttons
void Buttons_Init(void)
{
	// input 
    DDRD &= ~(1 << B1_PIN);
    DDRD &= ~(1 << B2_PIN);


   //output Pull up
    PORTD |= (1 << B1_PIN);
    PORTD |= (1 << B2_PIN);
}


	
uint8_t B1_Read(void)
{
    
     //releas = 1
     // bress  = 0
     

    return ((PIND >> B1_PIN) & 1);
}



uint8_t B2_Read(void)
{
   
   //releas = 1
   // bress  = 0
   

    return ((PIND >> B2_PIN) & 1);
}

void Send_SetMin(uint8_t temperature)
{
	UART_SendString("SETMIN ");
	UART_SendHex(temperature);
	UART_SendChar('\r');
}

void Send_SetMax(uint8_t temperature)
{
	UART_SendString("SETMax ");
	UART_SendHex(temperature);
	UART_SendChar('\r');
}
 
 
void SPI_Send_Panel(void)
{
	u8 temperature = DIP_To_Temperature();
	u8 b1 = B1_Read();
	u8 b2 = B2_Read();

	SPI_u8Transceive(SPI_START);
	SPI_u8Transceive(temperature);
	SPI_u8Transceive(b1);
	SPI_u8Transceive(b2);
}
 
int main(void)
{
	  // Store the current and previous button states to detect new button presses.
	  uint8_t b1_current, b2_current;
	  uint8_t b1_previous = 1, b2_previous = 1;

	  // save temp by dip
	  uint8_t temperature;
	  
	   DIP_Init();
	   Buttons_Init();
	   UART_Init(9600);
	   SPI_voidMaster_Init();

    while (1) 
    {
		   // Read the current states of buttons B1 and B2.
        b1_current = B1_Read();
        b2_current = B2_Read();

        // Convert the current DIP switch setting into a temperature.
        temperature = DIP_To_Temperature();

        // Send SETMIN only when B1 changes from released to pressed.
        if (b1_current == 0 && b1_previous == 1)
        {
            Send_SetMin(temperature);
        }

        // Send SETMAX only when B2 changes from released to pressed.
        if (b2_current == 0 && b2_previous == 1)
        {
            Send_SetMax(temperature);
        }

        // Send the current panel information to the other microcontroller.
        SPI_Send_Panel();

        // Save the current button states for comparison during the next loop.
        b1_previous = b1_current;
        b2_previous = b2_current;

        _delay_ms(100);
    }
}


#include "SPI.h"
#include <avr/io.h>


#define SPI_SS      PB2
#define SPI_MOSI    PB3
#define SPI_MISO    PB4
#define SPI_SCK     PB5


/* =========================================
   MASTER INITIALIZATION
   ========================================= */

void SPI_voidMaster_Init(void)
{
	
	DDRB |= (1 << SPI_MOSI)
	
	DDRB &= ~(1 << SPI_MISO);
	DDRB |= (1 << SPI_SCK);
	DDRB |= (1 << SPI_SS);
	
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


/* =========================================
   SLAVE INITIALIZATION
   ========================================= */

void SPI_voidSlave_Init(void)
{
    /* MISO -> OUTPUT */
    DDRB |= (1 << SPI_MISO);

    /* MOSI, SCK, SS -> INPUT */
    DDRB &= ~(1 << SPI_MOSI);
    DDRB &= ~(1 << SPI_SCK);
    DDRB &= ~(1 << SPI_SS);

    /* SPI Enable - Slave Mode */
    SPCR = (1 << SPE);
}


/* =========================================
   SEND
   ========================================= */

void SPI_u8Transceive(uint8_t data)
{
   /* Start transmission */
   SPDR = data;
   
   /* Wait for transmission complete */
   while(!(SPSR & (1<<SPIF))) ;
}


/* =========================================
   RECEIVE
   ========================================= */

u8 SPI_u8Receive(void)
{
   /* Wait for reception complete */
   while(!(SPSR & (1<<SPIF)));
   
   /* Return Data Register */
   return SPDR;
}
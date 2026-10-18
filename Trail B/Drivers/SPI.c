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
   
}


/* =========================================
   RECEIVE
   ========================================= */

u8 SPI_u8Receive(void)
{
   
}
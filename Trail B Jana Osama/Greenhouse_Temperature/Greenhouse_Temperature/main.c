/*
 * Greenhouse_Temperature.c
 *
 * Created: 10/11/2026 10:38:04 AM
 * Author : Jana.Osama
 */ 


#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

#include "I2C.h"
#include "LCD.h"
#include "LM75.h"
#include "EEPROM.h"
#include "UART.h"
#include "SPI.h"
#include "BIT_MATH.h"
#include "STD_TYPES.h"


/* gpio */
#define HEATER_PIN          PB0
#define FAN_PIN             PB1
#define YELLOW_LED_PIN      PB3
#define BUZZER_PIN          PB2

//eeprom
#define EEPROM_MIN_ADDR     0x00
#define EEPROM_MAX_ADDR     0x0001
#define EEPROM_MAGIC_ADDR   0x0002
#define EEPROM_MAGIC        0xA5

/* temp */
#define DEFAULT_MIN_TEMP    18
#define DEFAULT_MAX_TEMP    28
#define HIGH_TEMP           35
#define ALARM_TEMP          50

#define SPI_START           0xAA

#define SPI_WAIT_START      0
#define SPI_READ_DIP        1
#define SPI_READ_B1         2
#define SPI_READ_B2         3

#define BEEP_MS            100

u8 spi_state=0;
u8 min_temp;
u8 max_temp;

u8 sensor_error_count = 0;
u8 high_temp_warning = 0;
u8 temp_alarm = 0;

u8 panel_b1=0;
u8 panel_b2=0;

u8 panel_dip = 0;


//u8 spi_state = SPI_WAIT_START;

void Control_Heater_Fan(uint8_t temperature)
{
	/* Heater */
	if (temperature < min_temp)
	{
		SET_BIT(PORTB, HEATER_PIN);
	}
	else if (temperature >= min_temp + 1)
	{
		CLR_BIT(PORTB, HEATER_PIN);
	}

	/* Fan */
	if (temperature > max_temp)
	{
		SET_BIT(PORTB, FAN_PIN);
	}
	else if (temperature <= max_temp - 1)
	{
		CLR_BIT(PORTB, FAN_PIN);
	}
}

uint8_t Read_Temperature(uint8_t *temperature)
{
	uint8_t temp;

	temp = LM75_ReadTemperature();

	if (temp == 255) //  255 not temp  or invalid temp
	{
		sensor_error_count++;

		if (sensor_error_count >= 3)
		{
			CLR_BIT(PORTB, HEATER_PIN);
			CLR_BIT(PORTB, FAN_PIN);

			SET_BIT(PORTD, YELLOW_LED_PIN);

			LCD_Clear();
			LCD_String("Sensor Fault");
		}

		return 0;
	}

	sensor_error_count = 0;

	CLR_BIT(PORTD, YELLOW_LED_PIN);

	*temperature = temp;

	return 1;
}

void High_Temperature_Warning(uint8_t temperature)
{
	if (temperature > HIGH_TEMP)
	{
		if (high_temp_warning == 0)
		{
			SET_BIT(PORTD, BUZZER_PIN);
			high_temp_warning = 1;
		}
	}
	else
	{
		CLR_BIT(PORTD, BUZZER_PIN);
		high_temp_warning = 0;
	}
}


void Display_Temperature(uint8_t temperature)
{
	LCD_Clear();

	LCD_String("Temp:");
	LCD_Number(temperature);
	LCD_String("c");
	
	LCD_String("H:");
	if (GET_BIT(PORTB, HEATER_PIN))
	{
		LCD_String("ON ");
	}
	else
	{
		LCD_String("OFF");
	}

	LCD_String(" F:");
	if (GET_BIT(PORTB, FAN_PIN))
	{
		LCD_String("ON ");
	}
	else
	{
		LCD_String("OFF");
	}
}

void Temperature_Alarm(uint8_t temperature)
{
	if (temperature > ALARM_TEMP && temp_alarm == 0)
	{
		SET_BIT(PORTD, BUZZER_PIN);
		_delay_ms(BEEP_MS);
		CLR_BIT(PORTD, BUZZER_PIN);
		
		temp_alarm = 1;
	}

	if (temperature <= ALARM_TEMP)
	{
		temp_alarm = 0;
	}
}

void Safe_State(void)
{
	CLR_BIT(PORTB, HEATER_PIN);
	CLR_BIT(PORTB, FAN_PIN);
	SET_BIT(PORTD, YELLOW_LED_PIN);

	LCD_Clear();
	LCD_String("Sensor Fault");
}

void Load_Settings(void)
{
	uint8_t magic;

	magic = EEPROM_Read(EEPROM_MAGIC_ADDR);

	if (magic == EEPROM_MAGIC)
	{
		min_temp = EEPROM_Read(EEPROM_MIN_ADDR);
		max_temp = EEPROM_Read(EEPROM_MAX_ADDR);
	}
	else
	{
		min_temp = DEFAULT_MIN_TEMP;
		max_temp = DEFAULT_MAX_TEMP;

		EEPROM_Write(EEPROM_MIN_ADDR, min_temp);
		EEPROM_Write(EEPROM_MAX_ADDR, max_temp);
		EEPROM_Write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);
	}
}


void SPI_Byte(void)
{
	uint8_t data;

	if (SPSR & (1 << SPIF))
	{
		data = SPI_u8Receive();

		if (spi_state == SPI_WAIT_START)
		{
			if (data == SPI_START)
			{
				spi_state = SPI_READ_DIP;
			}
		}
		else if (spi_state == SPI_READ_DIP)
		{
			panel_dip = data;
			spi_state = SPI_READ_B1;
		}
		else if (spi_state == SPI_READ_B1)
		{
			panel_b1 = data;
			spi_state = SPI_READ_B2;
		}
		else if (spi_state == SPI_READ_B2)
		{
			panel_b2 = data;
			spi_state = SPI_WAIT_START;
		}
		else
		{
			spi_state = SPI_WAIT_START;
		}
	}
}




int main(void)
{
	u8 temperature;
	SET_BIT(DDRB, HEATER_PIN);
	SET_BIT(DDRB, FAN_PIN);

	SET_BIT(DDRD, YELLOW_LED_PIN);
	SET_BIT(DDRD, BUZZER_PIN);

	CLR_BIT(PORTB, HEATER_PIN);
	CLR_BIT(PORTB, FAN_PIN);

	CLR_BIT(PORTD, YELLOW_LED_PIN);
	CLR_BIT(PORTD, BUZZER_PIN);
	
	/*Init*/
	UART_Init(9600);
	I2C_Slave_Init(temperature);
	SPI_voidSlave_Init();
	LCD_Init();
	Load_Settings();

    while (1) 
    {
		 if (Read_Temperature(&temperature))
		 {
			 /* Control Heater and Fan */
			 Control_Heater_Fan(temperature);

			 /* High Temperature Warning */
			 High_Temperature_Warning(temperature);

			 /* Display Temperature */
			 Display_Temperature(temperature);
		 }

		 _delay_ms(500);
    }
}


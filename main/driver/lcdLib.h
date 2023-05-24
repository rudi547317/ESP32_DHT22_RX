/*********************************************************************
LCD Driver Functions

by: Elliott Gurrola

	LCD Pinouts

	Pin 1	Ground
	Pin 2	VCC (+3.3 to +5V)
	Pin 3	Contrast adjustment
	Pin 4	Register Select (RS). 0: Command, 1: Data 	-	D32 (GPIO 5)
	Pin 5	Read/Write (R/W). 0: Write, 1: Read		-> Set to GND for constant write
	Pin 6	Clock (Enable). Falling edge triggered		-	D33 (GPIO 4)
	Pin 7	Bit 0 (Not used in 4-bit operation)	
	Pin 8	Bit 1 (Not used in 4-bit operation)	
	Pin 9	Bit 2 (Not used in 4-bit operation)
	Pin 10	Bit 3 (Not used in 4-bit operation)
	Pin 11	Bit 4										-	D14 (GPIO 0)
	Pin 12	Bit 5										-	D27 (GPIO 1)
	Pin 13	Bit 6										-	D26 (GPIO 2)
	Pin 14	Bit 7										-	D25 (GPIO 3)
	Pin 15	Backlight Anode (+)
	Pin 16	Backlight Cathode (-)

	Top level functions available
		- lcd_init();			// Initialize LCD
		- write_byte(char l);		// Send a single character (1 byte)
	 	- write_string(char s[]);	// Print a character string
		- write_int(int num);		// Write an integer
		- gotoXy(x, y);			// Move cursor to (x, y) location (Lines and columns start at 0)
		- clear_lcd();			// Clear LCD and move cursor to (0, 0)

	Not important to the user:
		- send_cmd(char cmd);		// Send a command to the LCD
		- write_nibble(char l);		// Send a nibble to the LCD
		- void trigger_EN();

	*** DELAY FUNCTION ASSUMES 1 MHz CLOCK ***


Modified on: Apr 15, 2013
by: Luiz (Luis Carlos Bañuelos-Chacon)

Modified on: May 24, 2013
by: Elias N Jaquez
*********************************************************************/

#ifndef _LCD_LIB_H_
#define _LCD_LIB_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Delay Functions
#define delay_ms(x)		vTaskDelay(x / portTICK_PERIOD_MS)
#define delay_us(x)		vTaskDelay(x / portTICK_PERIOD_MS / 1000)

// Pins
#define RS		32
#define EN		33
#define DATA3	25
#define DATA2	26
#define DATA1	27
#define DATA0	14

// Commands
#define CLEAR	0x01

// GPIO States
#define LOW		0
#define HIGH	1

// Functions
void lcdInit();								// Initialize LCD
void lcdTriggerEN();						// Trigger Enable
void lcdWriteData(unsigned char data);		// Send Data (Characters)
void lcdWriteCmd(unsigned char cmd);		// Send Commands
void lcdClear();							// Clear LCD
void lcdSetText(char * text, int x, int y);	// Write string
void lcdSetInt(int val, int x, int y);		// Write integer
void lcdLowNibble(unsigned char x);			// Implements LOWNIB(x) macro

#endif /* LCDLIB_432_H_ */

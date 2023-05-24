#include "driver/lcdLib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_idf_version.h"

#define	LOWNIB(x)	lcdLowNibble(x)

void lcdInit() {
	// Wait for 100ms after power is applied.
	delay_ms(100);

	// Make pins outputs
	// P4->DIR = EN | RS | DATA;  
	uint8_t out[] = {EN, RS, DATA3, DATA2, DATA1, DATA0};
	for (int i = 0; i < 6; i++) {
		gpio_pad_select_gpio(out[i]);
		gpio_set_direction(out[i], GPIO_MODE_OUTPUT);
	}
	
	// Start LCD (send 0x03)
	// P4->OUT = 0x03; 
	gpio_set_level(RS, LOW);
	gpio_set_level(EN, LOW);
	gpio_set_level(DATA3, LOW);
	gpio_set_level(DATA2, LOW);
	gpio_set_level(DATA1, HIGH);
	gpio_set_level(DATA0, HIGH);

	// Send 0x03 3 times at 5ms then 100 us
	lcdTriggerEN(); 
	delay_ms(5);
	lcdTriggerEN();
	delay_ms(5);
	lcdTriggerEN();
	delay_ms(5);

	// Switch to 4-bit mode
	// P4->OUT = 0x02;
	gpio_set_level(RS, LOW);
	gpio_set_level(EN, LOW);
	gpio_set_level(DATA3, LOW);
	gpio_set_level(DATA2, LOW);
	gpio_set_level(DATA1, HIGH);
	gpio_set_level(DATA0, LOW);
	lcdTriggerEN();
	delay_ms(5);

	lcdWriteCmd(0x28); // 4-bit, 2 line, 5x8
	lcdWriteCmd(0x08); // Instruction Flow
	lcdWriteCmd(0x01); // Clear LCD
	lcdWriteCmd(0x06); // Auto-Increment
	lcdWriteCmd(0x0C); // Display On, No blink
}

void lcdTriggerEN() {
	// P4->OUT |= EN;
	// P4->OUT &= ~EN;
	gpio_set_level(EN, HIGH);
	delay_us(50);
	gpio_set_level(EN, LOW);
}

void lcdWriteData(unsigned char data) {
	// Set RS to Data
	// P4->OUT |= RS;
	gpio_set_level(RS, HIGH);
	
	LOWNIB(data >> 4); // Upper nibble
	lcdTriggerEN();
	LOWNIB(data); // Lower nibble
	lcdTriggerEN();
	delay_us(50); // Delay > 47 us
}

void lcdWriteCmd(unsigned char cmd) {
	// Set RS to Cmd
	// P4->OUT &= ~RS;
	gpio_set_level(RS, LOW);
	
	LOWNIB(cmd >> 4); // Upper nibble
	lcdTriggerEN();
	LOWNIB(cmd); // Lower nibble
	lcdTriggerEN();
	delay_ms(5); // Delay > 1.5ms
}

void lcdSetText(char* text, int x, int y) {
	int i;
	if (x < 16) {
		x |= 0x80; // Set LCD for first line write
		switch (y){
		case 1:
			x |= 0x40; // Set LCD for second line write
			break;
		case 2:
			x |= 0x60; // Set LCD for first line write reverse
			break;
		case 3:
			x |= 0x20; // Set LCD for second line write reverse
			break;
		}
		lcdWriteCmd(x);
	}
	i = 0;

	while (text[i] != '\0') {
		lcdWriteData(text[i]);
		i++;
	}
}

void lcdSetInt(int val, int x, int y){
	char number_string[16];
	sprintf(number_string, "%d", val); // Convert the integer to character string
	lcdSetText(number_string, x, y);
}

void lcdClear() {
	lcdWriteCmd(CLEAR);
}

void lcdLowNibble(unsigned char x){
	// P4->OUT = (P4->OUT & 0xF0) + (x & 0x0F)
	
	x &= 0x0F;
	
	switch (x) {
	case 0x00:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x01:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x02:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x03:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x04:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x05:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x06:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x07:
		gpio_set_level(DATA3, LOW);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x08:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x09:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x0A:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x0B:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, LOW);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x0C:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x0D:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, LOW);
		gpio_set_level(DATA0, HIGH);
		break;
	case 0x0E:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, LOW);
		break;
	case 0x0F:
		gpio_set_level(DATA3, HIGH);
		gpio_set_level(DATA2, HIGH);
		gpio_set_level(DATA1, HIGH);
		gpio_set_level(DATA0, HIGH);
		break;
	default:
		printf("lcdLib.c - LOWNIB(x) invalid input\n");
	}
	
	delay_ms(100);

}

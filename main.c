// Receive a string via RS232
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"
#include "driver/gpio.h"
#include "driver/lcdLib.h"
#include "driver/ledc.h"

#define UART_0_TX 17
#define UART_0_RX 16
// Set the PWM frequency to 2489 Hz which is the resonance frequency of the MH-FMD
#define PWM_FREQ 2489


bool buzzer_should_ring = false;




void init_RS232()
{
    const uart_port_t uart_num = UART_NUM_2;
    const int uart_buffer_size = 1024;
    QueueHandle_t uart_queue;

    // 1 - Setting Communication Parameters
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(uart_num, &uart_config);

    // 2 - Setting Communication Pins
    uart_set_pin(uart_num, UART_0_TX, UART_0_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 3 - Driver Installation
    uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0);
}




void init_Buzzer()
{
	 // Configure the GPIO pin for PWM output
	    gpio_pad_select_gpio(GPIO_NUM_18);
	    gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);

	    // Configure LEDC timer
	    ledc_timer_config_t ledc_timer = {
	        .duty_resolution = LEDC_TIMER_10_BIT,
	        .freq_hz = PWM_FREQ,
	        .speed_mode = LEDC_HIGH_SPEED_MODE,
	        .timer_num = LEDC_TIMER_0
	    };
	    ledc_timer_config(&ledc_timer);

	    // Configure LEDC channel
	    ledc_channel_config_t ledc_channel = {
	        .channel = LEDC_CHANNEL_0,
	        .duty = 512, // 50% duty cycle
	        .gpio_num = GPIO_NUM_18,
	        .speed_mode = LEDC_HIGH_SPEED_MODE,
	        .timer_sel = LEDC_TIMER_0
	    };
	    ledc_channel_config(&ledc_channel);

	    // Start LEDC channel
	    ledc_fade_func_install(0);
	    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, PWM_FREQ);
    	ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 1); // Stop LEDC channel 0

}







static void buzzer_task()
{


    while (1)
    {
    	vTaskDelay(pdMS_TO_TICKS(5000));
    	printf("Buzz Buzz Buzz \n");


        if (buzzer_should_ring) {
             ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 512);
             ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
         } else {
             ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 1);
         }
    }


}







static void rx_task()
{
    const uart_port_t uart_num = UART_NUM_2;
    int length = 0;
    uint8_t data[32];
    char myString[32];
    float myFloat;

  //  char temp [32];
    const float threshold_temp = 23.0; // Set threshold temperature to 30 degrees Celsius

    while (1)
    {
        uart_get_buffered_data_len(uart_num, (size_t *)&length); // Read data string length
        uart_read_bytes(uart_num, data, length, 100); // Read data string from the buffer
        printf("data - %.*s\n", length, data);
        vTaskDelay(2000 / portTICK_PERIOD_MS);


        for (int i = 0; i < length; i++) {
            if (data[i] == ' ' || data[i] == '\t' || data[i] == '\n' || data[i] == '\r') {
                myString[i] = '\0'; // terminate stringaar
                break; // exit loop
            }
            myString[i] = (char)data[i];
        }

	  //	uart_float = data;

        myFloat = atof(myString);
        printf("Conversion = %.1f\n", myFloat);








	  	printf("Temp is in string = %s\n",myString);



		// Output temperature to LCD Screen
		lcdSetText(myString, 0, 1);

		// Wait 1 sec, clear screen
		delay_ms(1000);
		lcdSetText("                ", 0, 1);
		delay_ms(100);

		memset(myString, 0, sizeof(myString));
		memset(data, 0, sizeof(myString));





		 if (myFloat > threshold_temp) {
		            // Set global flag variable to indicate that the buzzer should ring
		            buzzer_should_ring = true;
		        }

		 else
		 {
			 buzzer_should_ring = false;
		 }


    }
}




void app_main()
{
 //   printf("Receive data:\n");
	// Initialize LCD (Do it twice in case of errors)
		lcdInit();
		delay_ms(500);
		lcdClear();
		delay_ms(500);
		lcdInit();
		delay_ms(500);
		lcdClear();
		delay_ms(500);

		// Set "Temperature:" to display on top row
		lcdSetText("Temperature:", 0, 0);
		delay_ms(1000);





    vTaskDelay(1000 / portTICK_PERIOD_MS);
    init_Buzzer();
    init_RS232();
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(buzzer_task, "buzzer_task", 1024 * 2, NULL, configMAX_PRIORITIES - 2, NULL);

}




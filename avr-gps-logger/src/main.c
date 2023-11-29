
/*
https://docs.novatel.com/OEM7/Content/Logs/Core_Logs.htm?tocpath=Commands%20%2526%20Logs%7CLogs%7CGNSS%20Logs%7C_____0
https://dratek.cz/arduino/1510-gps-neo-6m-gyneo6mv2-modul-s-antenou.html
*/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <twi.h>            // I2C/TWI library for AVR-GCC
#include <oled.h>
#include <stdlib.h>         // C library. Needed for number conversions

#include <uart.h>

// Slave and internal addresses of temperature/humidity sensor DHT12
#define SENSOR_ADR 0x5c
#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4


/* Global variables --------------------------------------------------*/
// Declaration of "dht12" variable with structure "DHT_values_structure"
struct DHT_values_structure {
    uint8_t hum_int;
    uint8_t hum_dec;
    uint8_t temp_int;
    uint8_t temp_dec;
    uint8_t checksum;
} dht12;

struct GPS_data
{
    char header[5];
    char latitude[20];
    char latitude_dir[1];
    char longitude[20];
    char longitude_dir[1];
    char utc[10];
    char data_status[1];
    char mode_ind[1];
} gps_data;

char message_buffer[100] = {0};
int current_index = 0;
int read_line = 0;

void print_data();

// Flag for printing new data from sensor
volatile uint8_t new_sensor_data = 0;


/* Function definitions ----------------------------------------------*/
int main(void)
{
    char string[2];  // String for converting numbers by itoa()

    uart_init(UART_BAUD_SELECT(9600, F_CPU));

    // Init OLED
    oled_init(OLED_DISP_ON);
    oled_clrscr();

    oled_charMode(DOUBLESIZE);
    oled_puts("OLED disp.");

    oled_charMode(NORMALSIZE);

    // oled_gotoxy(x, y)
    oled_gotoxy(0, 2);
    oled_puts("128x64, SHH1106");

    // oled_drawLine(x1, y1, x2, y2, color)
    oled_drawLine(0, 25, 120, 25, WHITE);

    oled_gotoxy(0, 4);
    oled_puts("AVR course, Brno");

    oled_gotoxy(0, 6);
    oled_puts("Temperature [C]:");
    oled_gotoxy(0, 7);
    oled_puts("Humidity    [\%]:");

    // Copy buffer to display RAM
    oled_display();

    // Timer1
    TIM1_OVF_1SEC
    TIM1_OVF_ENABLE

    TIM0_OVF_128US
    TIM0_OVF_ENABLE

    sei();

    // Infinite loop
    while (1) {
        if (new_sensor_data == 1) {
            // Clear previous value
            oled_gotoxy(17, 6);
            oled_puts("    ");
            // Display DHT12 temp data
            oled_gotoxy(17, 6);
            itoa(dht12.temp_int, string, 10);
            oled_puts(string);
            // uart_puts(string);
            oled_puts(".");
            /// uart_puts(".");
            itoa(dht12.temp_dec, string, 10);
            oled_puts(string);
            // uart_puts(string);
            // uart_putc('\n');

            // Clear previous value
            oled_gotoxy(17, 7);
            oled_puts("    ");

            // Display DHT12 temp data
            oled_gotoxy(17, 7);
            itoa(dht12.hum_int, string, 10);
            oled_puts(string);
            oled_puts(".");
            itoa(dht12.hum_dec, string, 10);
            oled_puts(string);

            oled_display();


            // Do not print it again and wait for the new data
            new_sensor_data = 0;
        }
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Read temperature and humidity from DHT12, SLA = 0x5c.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    static uint8_t n_ovfs = 0;

    n_ovfs++;
    // Read the data every 5 secs
    if (n_ovfs >= 5) {
        n_ovfs = 0;

        // Test ACK from Temp/Humid sensor
        twi_start();
        if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
            // Set internal memory location
            twi_write(SENSOR_HUM_MEM);
            twi_stop();
            // Read data from internal memory
            twi_start();
            twi_write((SENSOR_ADR<<1) | TWI_READ);
            dht12.hum_int = twi_read(TWI_ACK);
            dht12.hum_dec = twi_read(TWI_ACK);
            dht12.temp_int = twi_read(TWI_ACK);
            dht12.temp_dec = twi_read(TWI_NACK);

            new_sensor_data = 1;
        }
        twi_stop();
    }

}

ISR(TIMER0_OVF_vect)
{
    char gnss_log[] = "GPGLL";
    uint8_t value;
    uint8_t index = 0;

    static char message_buffer[100] = {0};
    static int reading_line = 0;
    static char line_identifier_buffer[5] = {0};
    static int current_index = 0;

    value = uart_getc();

    if (value == '\0') {  // Data available from UART
        return;
    }

    if (value == '$') {  // Data available from UART
        reading_line = 1;
        return;
    }

    if (reading_line != 1) {
        return;
    }

    if (current_index < 5) {
        line_identifier_buffer[current_index] = value;
    }
    else
    {
        
    }

    return;
    if (value == '$') {
        for (size_t i = 0; i < 100; i++)
        {
            message_buffer[i] = '\0';
        };
        
        reading_line = 1;
        uart_puts("Reading new line \n");
        return;
    }

    if (reading_line != 1) {
        return;
    }

    while (message_buffer[index] != '\0')
    {
        uart_puts("increasing index \n");
        index++;
    }

    message_buffer[index] = value;

    if (value != '\n') {
        uart_puts("not new line");
        return;
    }

    // check if row is GPGLL
    for (size_t i = 0; i < 5; i++)
    {
        if (message_buffer[i] != gnss_log[i]) {
            uart_puts("not GPGLL: ");

            for (size_t j = 0; j < 5; j++)
            {
                uart_putc(message_buffer[j]);
            }
            

            uart_putc('\n');
            return;
        }
    }

    int current_data = 0;
    int current_char = 0;
    int char_offset = 0;

    while(current_char < 100) {

        if (message_buffer[current_char] == ',') {
            uart_puts("skipping on ,");
            current_data++;
            char_offset = 0;
        }

        if (message_buffer[current_char] == '*') {
            uart_puts("breaking on *");
            break;
        }

        switch (current_data)
        {
        case 0:
            gps_data.header[char_offset] = message_buffer[current_char];
            break;
        
        case 1:
            gps_data.latitude[char_offset] = message_buffer[current_char];
            break;

        case 2:
            gps_data.latitude_dir[char_offset] = message_buffer[current_char];
            break;

        case 3:
            gps_data.longitude[char_offset] = message_buffer[current_char];
            break;

        case 4:
            gps_data.longitude_dir[char_offset] = message_buffer[current_char];
            break;

        case 5:
            gps_data.utc[char_offset] = message_buffer[current_char];
            break;

        case 6:
            gps_data.data_status[char_offset] = message_buffer[current_char];
            break;

        case 7:
            gps_data.mode_ind[char_offset] = message_buffer[current_char];
            break;
        
        default:
            break;
        }
        
        current_char++;
    }

    print_data();
}

void print_data() {
    uart_puts(gps_data.header);
    uart_putc('\n');

    uart_puts(gps_data.latitude);
    uart_putc('\n');

    uart_puts(gps_data.longitude);
    uart_putc('\n');
}
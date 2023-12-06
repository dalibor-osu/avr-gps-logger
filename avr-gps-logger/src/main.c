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

// Declaration of gps_data variable with stucture "GPS_data"
struct GPS_data
{
    char header[5];
    char latitude[20];
    char latitude_dir[1];
    char longitude[20];
    char longitude_dir[1];
    char utc[10];
    char data_status;
    char mode_ind[1];
} gps_data;

char message_buffer[100] = {0};     // buffer for GPS message
const char gnss_log[5] = "GPGLL";   // header of GPS message we need to analyze 

// Flags for proper message analysis and printing
uint8_t current_index = 0;
uint8_t line_finished = 0;
uint8_t read_line = 0;
uint8_t reset_buffer = 0;
uint8_t correct_line = 0;
uint8_t print_line = 0;

void print_data();
void reset_message_buffer();

// Flag for printing new data from sensor
volatile uint8_t new_sensor_data = 0;


/* Function definitions ----------------------------------------------*/
int main(void)
{
    char string[2];

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

    TIM0_OVF_1MS
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

        // Check if message buffer should be reset, if yes, call reset_message_buffer()
        if (reset_buffer == 1) {
            reset_message_buffer();
        }

        // Check if reading the line was finished, then check if it's a line with correct header
        if (line_finished == 1) {
            correct_line = 1;
            for (size_t i = 0; i < 5; i++)
            {
                if (message_buffer[i] != gnss_log[i]) {
                    correct_line = 0;
                    line_finished = 0;
                    reset_buffer = 1;
                    reset_message_buffer();
                    break;
                }
            }
        }

        // If the line is incorrect, continue the loop
        if (correct_line != 1) {
            continue;
        }

        // Parse the line to gps_data variable
        if (correct_line == 1) {
            uint8_t current_data = 0;
            uint8_t char_offset = 0;
            for (size_t i = 0; i < 100; i++)
            {
                char current_char = message_buffer[i];

                if (current_char == ',') {
                    current_data++;
                    char_offset = 0;
                    continue;
                }

                if (current_char == '\0') {
                    break;
                }


                switch (current_data)
                {
                case 0:
                    gps_data.header[char_offset] = current_char;
                    break;
                
                case 1:
                    gps_data.latitude[char_offset] = current_char;
                    break;

                case 2:
                    gps_data.latitude_dir[char_offset] = current_char;
                    break;

                case 3:
                    gps_data.longitude[char_offset] = current_char;
                    break;

                case 4:
                    gps_data.longitude_dir[char_offset] = current_char;
                    break;

                case 5:
                    gps_data.utc[char_offset] = current_char;
                    break;

                case 6:
                    gps_data.data_status = current_char;
                    break;

                case 7:
                    gps_data.mode_ind[char_offset] = current_char;
                    break;
                
                default:
                    continue;
                }

                char_offset++;
            }

            print_line = 1;
            line_finished = 0;
            correct_line = 0;
        }

        // Print the line to uart
        if (print_line == 1) {
            print_data();
            print_line = 0;
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
    if (line_finished == 1) {
        return;
    }

    uint8_t value = uart_getc();

    if (value == '$') {
        line_finished = 0;
        read_line = 1;
        current_index = 0;
        return;
    }

    if (value == '\n') {
        line_finished = 1;
        read_line = 0;
        return;
    }

    if (read_line == 1) {
        message_buffer[current_index] = value;
        current_index++;
    }
}

void print_data() {
    if (gps_data.data_status == 'V') {
        uart_puts("GPS Status: Invalid, ");
    }
    else
    {
        uart_puts("UTC: ");
        uart_puts(gps_data.utc);
        uart_puts(", ");

        uart_puts("Lat: ");
        uart_puts(gps_data.latitude);
        uart_putc(' ');
        uart_puts(gps_data.latitude_dir);
        uart_puts(", ");

        uart_puts("Long: ");
        uart_puts(gps_data.longitude);
        uart_putc(' ');
        uart_puts(gps_data.longitude_dir);
        uart_puts(", ");
    }

    char string[2];  // String for converting numbers by itoa()

    uart_puts("Temp: ");
    itoa(dht12.temp_int, string, 10);
    uart_puts(string);
    uart_puts(".");
    itoa(dht12.temp_dec, string, 10);
    uart_puts(string);
    uart_puts(" °C, ");

    uart_puts("Hum: ");
    itoa(dht12.hum_int, string, 10);
    uart_puts(string);
    uart_puts(".");
    itoa(dht12.hum_dec, string, 10);
    uart_puts(string);
    uart_puts(" %\n");
}

void reset_message_buffer() {
    for (size_t i = 0; i < 100; i++)
    {
        message_buffer[i] = '\0';
    }
    
    reset_buffer = 0;
}
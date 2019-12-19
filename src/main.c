#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>

#define FOSC 2500000 // Clock Speed
#define BAUD 1200
#define MYUBRR FOSC/16/BAUD-1

uint8_t brygger_status = 0;
uint8_t nybryggt_status = 0;

volatile uint16_t second_counter = 0;

uint8_t average = 0;
double average_factor = 0.2; // "constant smoothing factor
// between 0 and 1. A higher α discounts older observations faster"

char string[20];
char time[5];

int main(void)
{

  init_timer();
  init_uart(MYUBRR);
  init_adc();
  srand(read_adc());

  // Setup the I/O for the status LED
  DDRB |= (1<<7);         // Set PortB Pin7 as an output
  PORTB &= ~(1<<7);        // Set PortB Pin7 LOW to turn ON LED
  //PORTB |= (1<<7);        // Set PortB Pin7 HIGH to turn OFF LED

  uart_send(0x3C);  // Enable Screen Saver, dims after 10 min idle
  uart_send(0x0E);  // Remove cursor
  uart_send(0x15);  // Clear display

  while(1) {
    // Read kaffebryggare current
    _delay_ms(1000);
    uint8_t data = read_adc();
    uint8_t result = average_value(data);

    // Check current to kaffebryggare
    if(result > ADC_threshold) {  // Current is high
      if(brygger_status != 1) {   // If starting to brew
        uart_send(0x15);  // Clear display

        // Write do display
        int random = rand()%200;
        if(random == 0) {
          uart_str(egg);
        }
        else {
          uart_str(brygger_kaffe);
        }

        brygger_status = 1;
        nybryggt_status = 1;
      }
    }
    else {    // Current is low
      if(nybryggt_status == 1) {  // If brew completed
        activate_counter1();
        // Send CAN message
      }
      if(second_counter > 60*90 && brygger_status != 0) {  // If coffee is old
        deactivate_counter1();
        uart_send(0x15);  // Clear display
        uart_str(brygg_nytt);
        brygger_status = 0;
      }
      else if(brygger_status != 0) {   // If brew is finished
        uart_send(0x15);  // Clear display
        reset_string(string);
        reset_string(time);

        itoa(second_counter/60, time, 10);  // Convert time to string

        // Build output string
        strcat(string, time);
        strcat(string, kaffe_gammalt);

        uart_str(string);

        _delay_ms(10000); // Delay 1 min, to reduce flicker
      }
    }
  }
}

// Implements an Exponential moving average filter
uint8_t average_value(uint8_t last_value) {
  average = average_factor * last_value + (1-average_factor) * average;
  return average;
}

void activate_counter1() {
  second_counter = 0;
  TCNT1 = 0;                  // Set initial count value to zero
  TIMSK1 |= 1<<OCIE1A;        // Enable timer compare interrupt
}
void deactivate_counter1() {
  TIMSK1 &= ~(1<<OCIE1A);        // Disable timer compare interrupt
}

// Sends a single char. Blocks processor.
void uart_send(unsigned char data) {
  // Wait for empty transmit buffer
  while ( !( UCSR0A & (1<<UDRE0)) );

  // Send message buffer content
  UDR0 = data;
}
void uart_str(char *msg) {
  for(uint8_t x = 0 ; msg[x] != '\0' ; x++) {
    uart_send(msg[x]);
  }
}

void init_timer(void) {
  //Setup the clock
  cli();                        // Disable global interrupts
  TCCR1B |= 1<<CS11 | 1<<CS10;  // Divide by 64 -> Clock freq = 39062 Hz
  OCR1A = 39062;                // Count 15624 cycles for 1 second interrupt
  TCCR1B |= 1<<WGM12;           // Put Timer/Counter1 in CTC mode
  TCNT1 = 0;                    // Reset count register
  TIFR1 &= !(1<<OCF1A);          // Reset Timer/Counter1, output compare A match flag
  sei();                        // Enable global interrupts
}

ISR(TIMER1_COMPA_vect) {
  second_counter++;

  TIFR1 &= !(1<<OCF1A);          // Reset Timer/Counter1, output compare A match flag
}

void init_uart(unsigned int ubrr) {
  cli();            //Disable global interrupts

  // Set baud rate
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;

  UCSR0C &= ~((1 << UPM00) | (1 << UPM01)); // No parity
  UCSR0C &= ~(1 << USBS0);  // 1 stop bit
  UCSR0B |= (1 << TXEN0);   // Enable transmitter

  sei();            //Enable global interrupts
}

void init_adc() {
  cli();

  ADMUX |= (1 << MUX1); // Use ADC2
  ADMUX |= (1 << ADLAR); // Left adjust result (highest 8-bits in ADCH reg)
  ADCSRA |= (1 << ADEN);  // Activate ADC
  ADCSRA |= (1 << ADPS2); // Set ADC prescale to .
  ADCSRA |= (1 << ADPS1);
  ADCSRA |= (1 << ADPS0);

  sei();
}
uint8_t read_adc() {
  ADCSRA |= (1 << ADSC); // Start ADC conversion

  while(ADCSRA & (1 << ADSC));  // Wait for measurement to complete

  return ADCH;
}

void reset_string(char *str) {
  for(uint8_t x = 0 ; str[x] != '\0' ; x++) {
    str[x] = 0;
  }
}

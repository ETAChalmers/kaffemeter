#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "main.h"

#define FOSC 2500000 // Clock Speed
//#define FOSC 8000000 // Clock Speed
//#define BAUD 19200
#define BAUD 1200
#define MYUBRR FOSC/16/BAUD-1

int main(void)
{

  //init_clock();
  init_uart(MYUBRR);
  init_adc();

  // Setup the I/O for the status LED
  DDRB |= (1<<7);         // Set PortB Pin7 as an output
  PORTB &= ~(1<<7);        // Set PortB Pin7 LOW to turn ON LED
  //PORTB |= (1<<7);        // Set PortB Pin7 HIGH to turn OFF LED

  // Testa om USART0 output PD1 funkar. Det gör den.
  //DDRD |= (1<<1);
  //PORTD |= (1<<1);

  //uart_send(0x0E);
  //uart_send('h');

  char string[3];

  while(1) {
    uart_send(0x15);  // Clear display

    uint8_t data = read_adc();
    string[0] = 0;
    string[1] = 0;
    string[2] = 0;
    ltoa(data, string, 10);

    uart_send(string[2]);
    uart_send(string[1]);
    uart_send(string[0]);

    _delay_ms(100);
  }          // Loop forever, interrupts do the rest
}

void activate_counter1() {
  TCNT1 = 0;                  // Set initial count value to zero
  TIMSK1 |= 1<<OCIE1A;        // Enable timer compare interrupt
}
void deactivate_counter1() {
  TIMSK1 &= ~(1<<OCIE1A);        // Disable timer compare interrupt
}

// Sends a single char. Blocks processor.
void uart_send(unsigned char data){
  // Wait for empty transmit buffer
  while ( !( UCSR0A & (1<<UDRE0)) );

  // Send message buffer content
  UDR0 = data;
}

void init_clock(void) {
  //Setup the clock
  cli();                        // Disable global interrupts
  TCCR1B |= 1<<CS11 | 1<<CS10;  // Divide by 64
  OCR1A = 15624;                // Count 15624 cycles for 1 second interrupt
  TCCR1B |= 1<<WGM12;           // Put Timer/Counter1 in CTC mode
  sei();                        // Enable global interrupts
}

void init_uart(unsigned int ubrr) {
  cli();            //Disable global interrupts

  // Set baud rate
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;

  UCSR0C &= ~((1 << UPM00) | (1 << UPM01)); // No parity
  UCSR0C &= ~(1 << USBS0);  // 1 stop bit
  //UCSR0C &= ~(1 << UCSZ02);
  //UCSR0C |= (1 << UCSZ01);
  //UCSR0C |= (1 << UCSZ00);  // Set frate data size to 8
  UCSR0B |= (1 << TXEN0);   // Enable transmitter

  /* Set frame format: 8data, 2stop bit */
  //UCSR0C = (1<<USBS0)|(3<<UCSZ00);

  sei();            //Enable global interrupts
}

void init_adc() {
  cli();

  ADMUX |= (1 << MUX1); // Use ADC2
  ADMUX |= (1 << ADLAR); // Left adjust result (highest 8-bits in ADCH reg)
  ADCSRA |= (1 << ADEN);  // Activate ADC
  ADCSRA |= (1 << ADPS2); // Set ADC prescale to 16. Results in 156 kHz.

  sei();
}

uint8_t read_adc() {
  ADCSRA |= (1 << ADSC); // Start ADC conversion

  while(ADCSRA & (1 << ADSC));  // Wait for measurement to complete

  return ADCH;
}

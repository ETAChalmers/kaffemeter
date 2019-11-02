#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{

  init_clock();

  //Setup the I/O for the LED
  DDRD |= (1<<0);     //Set PortD Pin0 as an output
  PORTD |= (1<<0);        //Set PortD Pin0 high to turn on LED

  while(1) { }          //Loop forever, interrupts do the rest
}

void activate_counter1() {
  TCNT1 = 0;        // Set initial count value to zero
  TIMSK1 |= 1<<OCIE1A;        //enable timer compare interrupt
}
void deactivate_counter1() {
  TIMSK1 &= ~(1<<OCIE1A);        //disable timer compare interrupt
}

void uart_send(char* message, uint8_t length){
  // Send message buffer content
}

void init_clock(void) {
  //Setup the clock
  cli();            //Disable global interrupts
  TCCR1B |= 1<<CS11 | 1<<CS10;  //Divide by 64
  OCR1A = 15624;        //Count 15624 cycles for 1 second interrupt
  TCCR1B |= 1<<WGM12;     //Put Timer/Counter1 in CTC mode
  sei();            //Enable global interrupts
}

void init_uart(void) {
  cli();            //Disable global interrupts
  // Init BAUD
  #undef BAUD  // avoid compiler warning
  #define BAUD 19200
  #include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  #if USE_2X
  UCSR0A |= (1 << U2X0);
  #else
  UCSR0A &= ~(1 << U2X0);
  #endif

  // Init interrupts
  
  sei();            //Enable global interrupts
}

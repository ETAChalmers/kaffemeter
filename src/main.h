int main(void);

void activate_counter1();
void deactivate_counter1();

void init_clock(void);
void init_uart(unsigned int ubrr);
void init_adc();

void uart_send(unsigned char data);
uint8_t read_adc();

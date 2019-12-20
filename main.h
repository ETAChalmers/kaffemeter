uint8_t ADC_threshold = 60;

char brygger_kaffe[] = "Brygger gott kaffe..\0";
char brygg_nytt[] = "Brygg nytt kaffe!\0";
char kaffe_gammalt[] = " minuter gammalt.\0";
char kaffe_gammalt_singular[] = " minut gammalt.\0";

int main(void);

void activate_counter1();
void deactivate_counter1();

void init_timer(void);
void init_uart(unsigned int ubrr);
void init_adc();

void uart_send(unsigned char data);
void uart_str(char *msg);

uint8_t read_adc();
uint8_t average_value(uint8_t last_value);

void reset_string(char *str);

char egg[] = "El Psy Kongroo!!\0";
char golv[] = "Brygger golvkaffe??\0";

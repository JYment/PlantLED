#define CR 0x0D
#define LF 0x0A
#define BS 0x08

#define TRUE 0x01
#define FALSE 0x00

void circular_buffer_init();
void putchar2buffer(unsigned char c);
unsigned char getchar_buffer(void);
void backspace2buffer(void);

extern volatile signed int rx1_data_counter;
extern volatile signed char buffer_full_flag;

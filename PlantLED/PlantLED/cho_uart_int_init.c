// S/W Environment : AVR Studio + WINAVR Compiler
// Crystal: 16Mhz
//
// Made by chowk.
// UART0 Init

#include <avr/io.h>
#include <avr/interrupt.h>

#include "cho_uart_int_init.h"
#include "cho_circular_buffer.h"

#define FOSC 16000000    // Clock Speed
#define BAUD 19200
#define MYUBRR FOSC/16/BAUD-1

// UART initialize
// baud rate: 19200
// char size: 8 bit
// parity: Disabled
void uart0_init()
{
	unsigned int ubrr;

	ubrr = MYUBRR;
	// Set baud rate
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// Set frame format: 8data, 2stop bit
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void uart_init()
{
	uart0_init();
	//RX1 Complete Interrupt Enable
	UCSR0B |= (1 << RXCIE0);
}

unsigned char rx0_char(void)
{
	// Wait for data to be received
	while ( !(UCSR0A & (1 << RXC0)) );
	// Get and return received data from buffer
	return UDR0;
}

// UART1 을 이용한 출력
void tx0_char(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !(UCSR0A & (1 << UDRE0)));
	// Put data into buffer, sends the data
	UDR0 = data;
}

// USART1, Rx Interrupt 처리 Routine
ISR(USART0_RX_vect)
{
	char c;

	// 입력된 문자를 처리 하는 동안 새로운 Interrupt가 발생 하지 않도록
	// Rx Interrupt를 Disable 한다.
	UCSR0B &= ~(1 << RXCIE0);	// Rx Complete Interrupt disable

	c = UDR0;
	// 입력된 문자가 CR or BS Code 가 아닌 경우
	if(!((c == CR) | (c == BS))){
		// Buffer에 문자를 저장 한다.
		putchar2buffer(c);
		// Buffer에 문자를 저장 하는 것이 성공한 경우 문자를 모니터에 출력 한다.
		// buffer_full_flag 가 TRUE 인 경우는 현재 Buffer 가 Full 상태이기 때문에
		// 문자를 Buffer에 저장 하지 못한 경우 이다.
		if(buffer_full_flag == FALSE)tx1_char(c);
	}
	// 만약 입력된 문자가 BS Code인 경우 Buffer에서 문자 하나를 재거 한다.
	if(c == BS){
		backspace2buffer();
		tx1_char(BS);
	}

	// 새로운 문자를 받을 수 있도록 Rx Interrupt를 Ensable 한다.
	UCSR0B |= (1 << RXCIE0); // Rx Complete Interrupt enable

	// 만약 입력된 문자가 RC Code인 경우 Tx Interrupt를 Enable 한다.
	// CTx Interrupt가 Enable 되면 Tx Interrupt 처리 Routine 에서
	// Buffer의 내용을 모니터에 출력 하게 된다.
	if(c == CR){
		UCSR0B |= (1 << TXCIE0); // Tx Complete Interrupt enable
		tx1_char(CR); tx1_char(LF);
	}
} 

// USART1, Tx Interrupt 처리 Routine
ISR(USART0_TX_vect)
{
	char c;

	// Tx Interrupt를 처리 하는 동안 새로운 Rx Interrupt가 발생 하지 않도록
	// Rx Interrupt를 Disable 한다.
	UCSR0B &= ~(1 << RXCIE0);	// Rx Complete Interrupt disable

	// Interrupt 처리를 하는 동안 새로운 Interrupt가 발생 하지 않도록
	// Tx Interrupt를 Disable 한다.
	UCSR0B &= ~(1 << TXCIE0);	// Tx Complete Interrupt disable

	// Buffer로 부터 하나의 문자를 가져 온다.
	c = getchar_buffer();
	// 만약 문자가 소문자이면 대문자로 변환 한다.
	if((c >= 'a') && (c <= 'z')) c &= 0b11011111;
	// Buffer로 부터 성공적으로 문자를 가져온 경우  
	if(c != 0){
		// 모니터에 문자를 출력 한다.
		tx1_char(c);
		// 새로운 문자를 출력 할 수 있도록 Tx Interrupt를 Ensable 한다.
		UCSR0B |= (1 << TXCIE0); // Tx Complete Interrupt enable
    }
	// 새로운 문자를 받을 수 있도록 Rx Interrupt를 Ensable 한다.
	else {
		tx1_char(CR); tx1_char(LF);
		UCSR0B |= (1 << RXCIE0); // Rx Complete Interrupt enable
	}
}

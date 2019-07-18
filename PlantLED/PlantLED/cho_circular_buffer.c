// S/W Environment : AVR Studio + WINAVR Compiler
// Crystal: 16Mhz
//
// Made by chowk.
// Circular Buffer

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "cho_circular_buffer.h"

#define UART_BUF_SIZE 16

unsigned char rx1_buf[UART_BUF_SIZE];
volatile signed int p_rx1_wr, p_rx1_rd;
volatile signed int rx1_data_counter;
volatile signed char buffer_full_flag;

unsigned char get_char_uart1 (void);


void circular_buffer_init()
{
	unsigned int i;

	buffer_full_flag = FALSE;
	p_rx1_wr = 0;
	p_rx1_rd = 0;
	rx1_data_counter = 0;
	for (i=0; i<UART_BUF_SIZE; i++) rx1_buf[i] = 0;
}

// Buffer에 문자를 추가 한다.
// Buffer 가 Full 상태 이면 Buffer Full 메세지를 출력 한다.
void putchar2buffer(unsigned char c) 
{
	// Buffer 가 Full 상태가 아니면 전달된 문자를 Buffer에 저장 한다.
	if (rx1_data_counter < UART_BUF_SIZE)
	{
		rx1_data_counter++;
		rx1_buf[p_rx1_wr] = c;
//printf(" INT rx1: %c  UDR0: %c pt_w: %d   ct: %d\r\n",rx1_buf[p_rx1_wr],UDR0,p_rx1_wr,rx1_data_counter);


		p_rx1_wr++;
		if (p_rx1_wr >= (UART_BUF_SIZE))p_rx1_wr = 0;
		//		SCI_OutString("Error: Uart0 buffer full");	
	}
	// Buffer 가 Full 상태 이면 buffer_full_flag를 TRUE로 하고 Input Buffer Full 메세지를 출력 한다.
	else 
	{
		buffer_full_flag = TRUE;
		printf("\r\nInput Buffer Full.");
	}
}

// Buffer로 부터 문자 하나를 읽어 간다. 
// Buffer가 비어 있는 경우 Buffer Empty 메세지를 출력 하고, 0를 return 한다.
unsigned char getchar_buffer(void)
{
	char c;

	// Buffer 가 Empty 상태가 아니면 문자 한자를 Return 한다.
	if(rx1_data_counter > 0)
	{
		c = rx1_buf[p_rx1_rd];
//printf(" Get rx1: %c          pt_r: %d  ct: %d\r\n",rx1_buf[p_rx1_rd],p_rx1_rd,rx1_data_counter);
		rx1_data_counter--;
		buffer_full_flag = FALSE;
		p_rx1_rd++;
		if (p_rx1_rd >= (UART_BUF_SIZE)) p_rx1_rd = 0;
		return c;
	}
	// Buffer가 비어 있는 경우 Buffer Empty 메세지를 출력 하고, 0를 return 한다.
	else 
	{
		printf("\r\nInput Buffer Empty.");		
		return 0;
	}
}

// Backspace 문자가 입력 되면 Buffer에 저장된 문자 하나를 제거 한다.
void backspace2buffer(void) 
{
	// 현재 Buffer에 저장된 문자가 있는 경우 저장된 문자 하나를 제거 한다.
	if (rx1_data_counter > 0)
	{
		rx1_data_counter--;
		buffer_full_flag = FALSE;

		p_rx1_wr--;
		if (p_rx1_wr < 0) p_rx1_wr = UART_BUF_SIZE - 1;
//printf("  rx1_data_counter:%d  p_rx1_wr:%d \r\n",rx1_data_counter, p_rx1_wr );
	}
	// Buffer가 비어 있는 경우 Buffer Empty 메세지를 출력 한다.
	else 
	{
		printf("\r\nInput Buffer Empty.");
	}
}

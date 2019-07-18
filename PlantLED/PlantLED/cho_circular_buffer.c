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

// Buffer�� ���ڸ� �߰� �Ѵ�.
// Buffer �� Full ���� �̸� Buffer Full �޼����� ��� �Ѵ�.
void putchar2buffer(unsigned char c) 
{
	// Buffer �� Full ���°� �ƴϸ� ���޵� ���ڸ� Buffer�� ���� �Ѵ�.
	if (rx1_data_counter < UART_BUF_SIZE)
	{
		rx1_data_counter++;
		rx1_buf[p_rx1_wr] = c;
//printf(" INT rx1: %c  UDR0: %c pt_w: %d   ct: %d\r\n",rx1_buf[p_rx1_wr],UDR0,p_rx1_wr,rx1_data_counter);


		p_rx1_wr++;
		if (p_rx1_wr >= (UART_BUF_SIZE))p_rx1_wr = 0;
		//		SCI_OutString("Error: Uart0 buffer full");	
	}
	// Buffer �� Full ���� �̸� buffer_full_flag�� TRUE�� �ϰ� Input Buffer Full �޼����� ��� �Ѵ�.
	else 
	{
		buffer_full_flag = TRUE;
		printf("\r\nInput Buffer Full.");
	}
}

// Buffer�� ���� ���� �ϳ��� �о� ����. 
// Buffer�� ��� �ִ� ��� Buffer Empty �޼����� ��� �ϰ�, 0�� return �Ѵ�.
unsigned char getchar_buffer(void)
{
	char c;

	// Buffer �� Empty ���°� �ƴϸ� ���� ���ڸ� Return �Ѵ�.
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
	// Buffer�� ��� �ִ� ��� Buffer Empty �޼����� ��� �ϰ�, 0�� return �Ѵ�.
	else 
	{
		printf("\r\nInput Buffer Empty.");		
		return 0;
	}
}

// Backspace ���ڰ� �Է� �Ǹ� Buffer�� ����� ���� �ϳ��� ���� �Ѵ�.
void backspace2buffer(void) 
{
	// ���� Buffer�� ����� ���ڰ� �ִ� ��� ����� ���� �ϳ��� ���� �Ѵ�.
	if (rx1_data_counter > 0)
	{
		rx1_data_counter--;
		buffer_full_flag = FALSE;

		p_rx1_wr--;
		if (p_rx1_wr < 0) p_rx1_wr = UART_BUF_SIZE - 1;
//printf("  rx1_data_counter:%d  p_rx1_wr:%d \r\n",rx1_data_counter, p_rx1_wr );
	}
	// Buffer�� ��� �ִ� ��� Buffer Empty �޼����� ��� �Ѵ�.
	else 
	{
		printf("\r\nInput Buffer Empty.");
	}
}

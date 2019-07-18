/*
 * PlantLED.c
 *
 * Created: 2019-07-18 오전 10:18:30
 * Author : jyment

 * 식물용 LED를 터치에 따라 제어를 동작하게 함
 * MP3플레이어를 이용하여 해당 터치 동작시 각 동작에 맞는 MP3를 실행하도록 함.
 * 1번 터치 -> 소형 원형 LED 회전 2회 
 * 2번 터치 -> 대형 원형 LED ON
 * 3번 터치 -> 소형 원형 LED 회전 1회 후 전체 OFF
 */ 


#define F_CPU	16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "cho_circular_buffer.h"
#include "cho_uart_int_init.h"

#define ON_TIME		50
#define ROTATE_CNT	3


static int put_char(char c, FILE *stream);
static int put_char(char c, FILE *stream)
{
	tx0_char(c);
	return 0;
}

volatile uint8_t mode = 0;

uint8_t run1[10] = {0x7E, 0xFF, 0x06, 0x12, 0x00, 0x00, 0x01, 0xFE, 0xE8, 0xEF};


// 터치버튼 외부 인터럽트 동작
ISR(INT0_vect)
{
	_delay_ms(2);
	if((PIND & 0x04) == 0)
	{
		mode++;
		if(mode > 3)	mode = 0;
	}
}


// 전체 장비 초기화
void initDevice()
{
	// 상판 LED 5개 출력 설정
	DDRB |= (1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0);
	
	// 터치버튼 외부 인터럽트0으로 설정 FALLING EDGE에서 인터럽트 발생
	EICRA &= ~(1 << ISC00);
	EICRA |= (1 << ISC01);
	EIMSK = (1 << INT0);
	EIFR = (1 << INTF0);
	
	// 전체 LED OFF
	PORTB &= ~((1 << PORTB5) | (1 << PORTB4) | (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB0));
	mode = 0;		// mode 초기값 = 0으로 설정
}

void TouchOneClicked()
{
	for(int i=0; i<ROTATE_CNT; i++)
	{
		PORTB |= (1 << PORTB4) | (1 << PORTB3);		// 4번과 3번 ON
		_delay_ms(ON_TIME);
		PORTB &= ~(1 << PORTB4);					// 4번 OFF
		PORTB |= (1 << PORTB3) | (1 << PORTB2);		// 3번과 2번 ON
		_delay_ms(ON_TIME);
		PORTB &= ~(1 << PORTB3);					// 3번 OFF
		PORTB |= (1 << PORTB2) | (1 << PORTB1);		// 2번과 1번 ON
		_delay_ms(ON_TIME);
		PORTB &= ~(1 << PORTB2);					// 2번 OFF
		PORTB |= (1 << PORTB1) | (1 << PORTB0);		// 1번과 0번 ON
		_delay_ms(ON_TIME);
		PORTB &= ~(1 << PORTB1);					// 1번 OFF
		PORTB |= (1 << PORTB0) | (1 << PORTB4);		// 0번과 4번 ON
		_delay_ms(ON_TIME);
		PORTB &= ~(1 << PORTB0);					// 0번 OFF
	}
}

int main(void)
{
	initDevice();		// 장치 초기화 실행
	_delay_ms(200);
	
	uart_init();              // UART 초기화
	circular_buffer_init();
	fdevopen(put_char,0);
	
	sei();	// 전체 인터럽트 실행
	
	
	while (1)
	{
		switch(mode)
		{
			case 1:
			mp3_run(1);
			while(mode == 1);
			break;
			case 2:
			break;
			case 3:
			break;
			default:
			break;
		}
	}
}

void mp3_run(uint8_t num)
{
	switch(num)
	{
		case 1:
			for(int i=0; i<10; i++)			tx0_char(run1[i]);
			while((PINC & 0x01) == 0);
			break;
	}
}
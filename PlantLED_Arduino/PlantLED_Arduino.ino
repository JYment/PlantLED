#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

#define ON_TIME         3         // 3 * 100ms = 300ms LED ON TIME
#define MP3_Volume      16

#define LED_1   5
#define LED_2   3
#define LED_3   6
#define LED_4   9
#define LED_5   10
#define BOT_LED     13

#define MP3_BUSY    12

SoftwareSerial mySerial(8, 4);

uint8_t led_arr[5] = {LED_1, LED_2, LED_3, LED_4, LED_5};
boolean led_value[5] = {LOW, LOW, LOW, LOW, LOW};
boolean led_bot_value = HIGH;

volatile int mode = 0;
volatile uint32_t timer_cnt_1ms = 0;
volatile uint8_t timer_cnt_100ms = 0;
uint8_t rotation_cnt = 0;

uint8_t mp3_flag = 0;


// 외부 인터럽트 버튼 동작 확인 Mode 카운트
ISR(INT0_vect) {
    delayMicroseconds(2);       // 디바운스 처리
    if((PIND & 0x04) == LOW) {  // 현재 버튼 상태 확인
        mp3_flag = 0;
        timer_cnt_100ms = 0;
        TIMSK1 |= (1 << TOIE1);
        mp3_stop();     delay(20);
        mode++;                 // 카운트
        if(mode > 4)    mode = 1;
    }
}

ISR(TIMER1_OVF_vect) {
    timer_cnt_1ms++;
    if(timer_cnt_1ms > 100) {
        timer_cnt_100ms++;
        timer_cnt_1ms = 0;
    }

    switch(mode) {
        case 1:
            if(rotation_cnt < 2) {
                if(timer_cnt_100ms < ON_TIME) {
                    led_value[0] = HIGH; led_value[1] = HIGH; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = LOW;
                }
                else if(timer_cnt_100ms >= ON_TIME && timer_cnt_100ms < ON_TIME*2) {
                    led_value[0] = LOW; led_value[1] = HIGH; led_value[2] = HIGH; led_value[3] = LOW; led_value[4] = LOW;
                }    
                else if(timer_cnt_100ms >= ON_TIME*2 && timer_cnt_100ms < ON_TIME*3) {
                    led_value[0] = LOW; led_value[1] = LOW; led_value[2] = HIGH; led_value[3] = HIGH; led_value[4] = LOW;
                }  
                else if(timer_cnt_100ms >= ON_TIME*3 && timer_cnt_100ms < ON_TIME*4) {
                    led_value[0] = LOW; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = HIGH; led_value[4] = HIGH;
                } 
                else if(timer_cnt_100ms >= ON_TIME*4 && timer_cnt_100ms < ON_TIME*5) {
                    led_value[0] = HIGH; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = HIGH;
                }  
                else {
                    timer_cnt_100ms = 0;
                    rotation_cnt++;
                }
            }
            else {
                led_value[0] = HIGH; led_value[1] = HIGH; led_value[2] = HIGH; led_value[3] = HIGH; led_value[4] = HIGH;
                led_bot_value = LOW;
                mp3_flag = 1;
            }
            break;
        
        case 2:
            led_bot_value = HIGH;
            mp3_flag = 0;
            break;
        case 3:
            led_value[0] = LOW; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = LOW;
            led_bot_value = LOW;
            mp3_flag = 0;
            break;
        case 4:  
            if(rotation_cnt == 2) {
                if(timer_cnt_100ms < ON_TIME) {
                    led_value[0] = HIGH; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = LOW;
                }
                else if(timer_cnt_100ms >= ON_TIME && timer_cnt_100ms < ON_TIME*2) {
                    led_value[0] = LOW; led_value[1] = HIGH; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = LOW;
                }    
                else if(timer_cnt_100ms >= ON_TIME*2 && timer_cnt_100ms < ON_TIME*3) {
                    led_value[0] = LOW; led_value[1] = LOW; led_value[2] = HIGH; led_value[3] = LOW; led_value[4] = LOW;
                }  
                else if(timer_cnt_100ms >= ON_TIME*3 && timer_cnt_100ms < ON_TIME*4) {
                    led_value[0] = LOW; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = HIGH; led_value[4] = LOW;
                } 
                else if(timer_cnt_100ms >= ON_TIME*4 && timer_cnt_100ms < ON_TIME*5) {
                    led_value[0] = LOW; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = HIGH;
                }  
                else {
                    timer_cnt_100ms = 0;
                    rotation_cnt = 0;
                }
            }
            else {
                led_value[0] = LOW; led_value[1] = LOW; led_value[2] = LOW; led_value[3] = LOW; led_value[4] = LOW;
                led_bot_value = HIGH;
            }
            break;
        default:
            break;
    }
}

void setup() {
    delay(500);
    
    init_Device();
    
    // Debug USART
    Serial.begin(115200); 
    Serial.println("START\n");  
}

 
void loop() {
    switch(mode) {
        case 1:
//            Serial.print(" mode = ");   Serial.println(mode);
            oneClicked();
            if(mp3_flag == 0) {
                if(digitalRead(MP3_BUSY) == HIGH)   mp3_play_physical(0001);
            }
            else {
                mp3_stop();
            }
            break;
        case 2:
//            Serial.print(" mode = ");   Serial.println(mode);
            Serial.print(" mode = ");   Serial.println(mode);
            oneClicked();
            if(mp3_flag == 0) {
                if(digitalRead(MP3_BUSY) == HIGH)   mp3_play_physical(0002);
            }
            else {
                mp3_stop();
            }
            break;        
        case 3:
            Serial.print(" mode = ");   Serial.println(mode);
            oneClicked();
            if(mp3_flag == 0) {
                if(digitalRead(MP3_BUSY) == HIGH)   mp3_play_physical(0003);
            }
            else {
                mp3_stop();
            }
            break;
        case 4:
            Serial.print(" mode = ");   Serial.println(mode);
            oneClicked();
            if(mp3_flag == 0) {
                if(digitalRead(MP3_BUSY) == HIGH)   mp3_play_physical(0004);
            }
            else {
                mp3_stop();
            }
            break;
        default:
            mp3_stop();     delay(20);
            break;        
    }
}


void init_Device() {
    // LED 상판 OUTPUT 
    for(int i=0; i<5; i++){
        pinMode(led_arr[i], OUTPUT);        // LED상판 OUTPUT
        digitalWrite(led_arr[i], LOW);      // LED상판 OFF
    }

    // LED 하판 OUPUT 
    pinMode(BOT_LED, OUTPUT);           // 주광색 LED용 FET GATE 신호
    digitalWrite(BOT_LED, HIGH);        // LED하판 OFF

    // Debug 
    pinMode(A0, OUTPUT);
    
    // TOUCH BUTTON INPUT
    //pinMode(2, INPUT);          // 터치센서 확인용
    
    // EXTERNAL INT0 : FALLING EDGE
    EICRA &= ~(1 << ISC00);
    EICRA |= (1 << ISC01);
    EIMSK = (1 << INT0);
    EIFR = (1 << INTF0);

    // TIMER1 50msec
    timer_init();
    sei();
  
    // MP3 MODULE Init
    mySerial.begin (9600);  
    mp3_set_serial (mySerial);
    delay(20);
    mp3_set_volume (MP3_Volume);                // Volume (1 ~ 30)
    delay(20);      
}

void timer_init() {
    TCCR1B = ((0 < CS12) | (1 << CS11) | (0 << CS10));          // EXTERNAL 16MHz
    TCNT1 = 250;        //  (1/16MHz) * 64 * 250 = 1ms
}


// Touch 1번 클릭했을 때 동작
void oneClicked() {
    digitalWrite(led_arr[0], led_value[0]);
    digitalWrite(led_arr[1], led_value[1]);
    digitalWrite(led_arr[2], led_value[2]);
    digitalWrite(led_arr[3], led_value[3]);    
    digitalWrite(led_arr[4], led_value[4]); 
    digitalWrite(BOT_LED, led_bot_value);
}

// 2번 클릭했을 때 동작 
void twoClicked() {       
    digitalWrite(7, LOW);       // 원형 주광색 LED ON (GATE신호 확인)
}

// 3번 클릭했을 때 동작
void threeClicked() {
    for(int i=8; i<13; i++) {   // 개별 LED 전체 OFF
        digitalWrite(i, LOW);
    }    

    for(int j=8; j<13; j++) {   // 1회 회전하면서 LED 개별 ON OFF
        digitalWrite(j, HIGH);
        delay(ON_TIME-100);
        digitalWrite(j, LOW);
    }
    
    for(int i=7; i<13; i++) {   // 모든 LED OFF
        digitalWrite(i, LOW);
    }
}

#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

#define ON_TIME     300         // LED 개별 동작시 ON되는 시간
#define LED_1   3
#define LED_2   5
#define LED_3   6
#define LED_4   9
#define LED_5   10


SoftwareSerial mySerial(8, 4);

volatile int mode = 0;

// 외부 인터럽트 버튼 동작 확인 Mode 카운트
ISR(INT0_vect) {
    delayMicroseconds(2);       // 디바운스 처리
    if((PIND & 0x04) == LOW) {  // 현재 버튼 상태 확인
        mode++;                 // 카운트
        if(mode > 3)    mode = 1;
    }
}

void setup() {
    // 개별 LED 동작용
    pinMode(LED_1, OUTPUT);  
    pinMode(LED_2, OUTPUT);  
    pinMode(LED_3, OUTPUT);  
    pinMode(LED_4, OUTPUT);  
    pinMode(LED_5, OUTPUT);  
    pinMode(12, OUTPUT);
    pinMode(A5, OUTPUT);     // 주광색 LED용 FET GATE 신호
    pinMode(2, INPUT);      // 터치센서 확인용
    pinMode(A0, INPUT);


    digitalWrite(LED_1,  HIGH);  
    digitalWrite(LED_2, HIGH);  
    digitalWrite(LED_3, HIGH);  
    digitalWrite(LED_4, HIGH);  
    digitalWrite(LED_5, HIGH);  
    
    digitalWrite(12, HIGH);  
    digitalWrite(A5, HIGH);  
    
    // EXTERNAL INT0 : FALLING EDGE
    EICRA &= ~(1 << ISC00);
    EICRA |= (1 << ISC01);
    EIMSK = (1 << INT0);
    EIFR = (1 << INTF0);
    
    sei();
    mode = 0;
    
    Serial.begin(115200);
    mySerial.begin (9600);  
    mp3_set_serial (mySerial);
    delay(20);
    mp3_set_volume (5);                 // 볼륨은 중간 볼륨인 15로 설정(0~30)
    delay(20);                         // 볼륨이 설정될 동안 10ms 대기
}

 
void loop() {
    switch(mode) {
        case 1:
            mp3_stop();     delay(20);
            mp3_play_physical(0002);
            oneClicked();
            Serial.print(" mode = ");   Serial.println(mode);
            while(mode == 1);
            break;
        case 2:
            mp3_stop();     delay(20);
            mp3_play_physical(0003);
            twoClicked();
            Serial.print(" mode = ");   Serial.println(mode);
            while(mode == 2);
            break;        
        case 3:
            mp3_stop();     delay(20);
            mp3_play_physical(0004);
            threeClicked();
            Serial.print(" mode = ");   Serial.println(mode);
            while(mode == 3);
            break;
        default:
            break;        
    }
}

// 1번 클릭했을 때 동작
void oneClicked() {
    for(int i=0; i<2; i++) {    // 두번 회전하면서 LED 개별 ON OFF 
        for(int j=8; j<13; j++) {
            digitalWrite(j, HIGH);
            delay(ON_TIME-100);
            delay(100);
            digitalWrite(j, LOW);
        }
    }
    for(int i=7; i<13; i++) {   // 최종 LED ON 상태
        digitalWrite(i, HIGH);
    }
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

/*
 * 데이터시트에 나와 있는 MP3 폴더 안에 0001번 0002번 0255번 실행
 */
void mp3_run(unsigned int num) {
     switch(num) {
          case 1:                            // 0001번
               mySerial.write(0x7E);
               mySerial.write(0xFF);
               mySerial.write(0x06);
               mySerial.write(0x12);
               mySerial.write((byte)0x00);
               mySerial.write((byte)0x00);
               mySerial.write(0x01);
               mySerial.write(0xFE);
               mySerial.write(0xE8);
               mySerial.write(0xEF);
               while(digitalRead(A0) == LOW);
               break;
          case 2:                            // 0002번
               mySerial.write(0x7E);
               mySerial.write(0xFF);
               mySerial.write(0x06);
               mySerial.write(0x12);
               mySerial.write((byte)0x00);
               mySerial.write((byte)0x00);
               mySerial.write(0xFF);
               mySerial.write(0xFD);
               mySerial.write(0xEA);
               mySerial.write(0xEF);
               while(digitalRead(A0) == LOW);
               break;
          case 3:                            // 0255번
               mySerial.write(0x7E);
               mySerial.write(0xFF);
               mySerial.write(0x06);
               mySerial.write(0x12);
               mySerial.write((byte)0x00);
               mySerial.write((byte)0x00);
               mySerial.write(0x02);
               mySerial.write(0xFE);
               mySerial.write(0xE7);
               mySerial.write(0xEF);
               while(digitalRead(A0) == LOW);
               break;
     }
}
 
 
/*
 * checksum 적용 된 volume 조절
 */
void mp3_volume(unsigned int level) {
     unsigned char check_MSB = mp3_volume_checkSum(level) >> 8;
     unsigned char check_LSB = mp3_volume_checkSum(level) & 0x00FF;
     mySerial.write(0x7E);              // $S
     mySerial.write(0xFF);              // Ver
     mySerial.write(0x06);              // Length
     mySerial.write(0x06);              // CMD
     mySerial.write((byte)0x00);        // FeedBack
     mySerial.write((byte)0x00);        // Param_MSB
     mySerial.write(level);             // Param_LSB
     mySerial.write(check_MSB);         // Check_MSB
     mySerial.write(check_LSB);         // Check_LSB     
     mySerial.write(0xEF);              // $O
}
 
 
/*
 * volume cmd 적용된 check sum 생성
 */
unsigned int mp3_volume_checkSum(unsigned int level) {
     unsigned int checksum = 0;
     checksum = 0xFFFF - (0xFF + 0x06 + 0x06 + level) + 1;
     return checksum;
}

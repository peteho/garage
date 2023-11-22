// original code from https://jasper.sikken.nl/garage/index.html
// adopted for digispark and H34S 433MHz Wireless RF Transmitter Module
//
// digispark pinout
// P0 (MOSI)       PB0
// P1 (MISO / LED) PB1
// P2 (SCLK)       PB2
// P3 (USB+)       PB3
// P4 (USB-)       PB4
// P5 (RESET)      PB5

#include <Arduino.h>

#define RF      PB1
#define BUTTON1 PB0
#define BUTTON2 PB2
#define ON      1
#define OFF     0
#define DELAY1  360L  // 268  H
#define DELAY2  1940L // 2058 L
#define DELAY3  250L  // 336  L
#define DELAY4  2000L // 1993 H

uint32_t time1 = 0;

void dWrite(uint8_t port, uint8_t bit) {
  PORTB = bit ? (PORTB | (port << 1)) : (PORTB & ~(port << 1)); 
}

void sendbit(char a){
  if(a==0){ //pin low
    dWrite(RF, ON);
    delayMicroseconds(DELAY1);
    dWrite(RF, OFF);
    delayMicroseconds(DELAY2);
    dWrite(RF, ON);
    delayMicroseconds(DELAY1);
    dWrite(RF, OFF);
    delayMicroseconds(DELAY2);
  }
  if(a==1){ //pin high
    dWrite(RF, ON);
    delayMicroseconds(DELAY4);
    dWrite(RF, OFF);
    delayMicroseconds(DELAY3);
    dWrite(RF, ON);
    delayMicroseconds(DELAY4);
    dWrite(RF, OFF);
    delayMicroseconds(DELAY3);
  }
  if(a==2){ //pin floating
    dWrite(RF, ON);
    delayMicroseconds(DELAY4);
    dWrite(RF, OFF);
    delayMicroseconds(DELAY3);
    dWrite(RF, ON);
    delayMicroseconds(DELAY1);
    dWrite(RF, OFF);
    delayMicroseconds(DELAY2);
  }
}

// 123456789
// ++-+0+-0-  Einfahrt
// ++-+0+-00  Ausfahrt
// 1905H,443L,1900H,443L,1900H,443L,1900H,443L
// 1860H,480L...,207H,2130L

void send1(){
  sendbit(1);//address A1 0 is low, 1 is high, 2 is floating pin
  sendbit(1);//address A2
  sendbit(0);//address A3
  sendbit(1);//address A4
  sendbit(2);//address A5
  sendbit(1);//address A6 /data D6
  sendbit(0);//address A7 /data D7
  sendbit(2);//address A8 /data D8
  sendbit(0);//address A9 /data D9
  delay(14); 
}

void send2(){
  sendbit(1);//address A1 0 is low, 1 is high, 2 is floating pin
  sendbit(1);//address A2
  sendbit(0);//address A3
  sendbit(1);//address A4
  sendbit(2);//address A5
  sendbit(1);//address A6 /data D6
  sendbit(0);//address A7 /data D7
  sendbit(2);//address A8 /data D8
  sendbit(2);//address A9 /data D9
  delay(14); 
}

void delayUs(uint16_t dly) {
  for (uint16_t i = 0; i < dly; i++) {
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
  }
}

//#define F_CPU 16500000UL
#define COMPVAL 9
#define TICK (COMPVAL * 10000000UL / F_CPU)

void timer_comp()
{
 TCCR0A = 0x00;
 TCCR0B = 0x00;
 TCCR0B |= (1<<CS01);  // prescale 8  |(1<<CS02);   //prescaling with 1024
 TCCR0A |= (1<<WGM01); //toggle mode and compare match  mode
 OCR0A = COMPVAL;      //compare value ~ 4 us
 TCNT0 = 0;
}

void delayMic(uint16_t mics)
{
 uint16_t i = 0;
 while (i <= (mics-17) / TICK * 10)
 {
  while((TIFR & (1 << OCF0A) )==0);  //waiting to OCF0A flag bit high
  TIFR |= (1<<OCF0A);    //toggling OCF0A
  i++;                   //increment by one
 }
}

void setup() {
  pinMode(RF, OUTPUT);     // DDRB |= (1<<RF)
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  timer_comp();
}

void loop() {
  if      (digitalRead(BUTTON1) && !digitalRead(BUTTON2)) send1();
  else if (digitalRead(BUTTON2) && !digitalRead(BUTTON1)) send2();
  else {
    /*uint32_t time2 = millis();
    if (time2 - time1 >= 500) {
      time1 = time2;
      dWrite(RF, 1 - digitalRead(RF));
    }*/
    dWrite(RF, ON);
    delayMic(268);
    dWrite(RF, OFF);
    delayMic(335);
    dWrite(RF, ON);
    delayMic(1994);
    dWrite(RF, OFF);
    /*delayMicroseconds(200);
    dWrite(RF, ON);
    delayMicroseconds(500);
    dWrite(RF, OFF);
    delayMicroseconds(500);
    dWrite(RF, ON);
    delayMicroseconds(1000);
    dWrite(RF, OFF);
    delayMicroseconds(1000);
    dWrite(RF, ON);
    delayMicroseconds(2000);
    dWrite(RF, OFF);
    delayMicroseconds(2000);*/
    delay(1);
  }
}

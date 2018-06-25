#ifndef _AT_KEYBOARD_H
#define _AT_KEYBOARD_H

#include <QueueArray.h>

QueueArray<unsigned char> at_keybuffer;
boolean selftest = true;

#define at_clk 0
#define at_data 1

#define ledd 16

void at_setup();
void at_loop();
void at_waitForCts();
void at_sendBit(boolean bit);
int at_readBit();
unsigned char getOddParity(unsigned char p);
unsigned char at_read();
void at_write(unsigned char value);
void at_enqueue(unsigned char* value, int sz);
void handleHostData();


void at_setup() {
  pinMode(at_clk, OUTPUT); 
  pinMode(at_data, OUTPUT); 
  digitalWrite(at_clk, HIGH); 
  digitalWrite(at_data, LOW);
  delayMicroseconds(50);
  pinMode(at_clk, INPUT);
  pinMode(at_data, INPUT); 

  pinMode(ledd, OUTPUT);
  digitalWrite(ledd, LOW);
}

void at_loop() {
  noInterrupts();
  at_waitForCts();
  while(!at_keybuffer.isEmpty()) {
    unsigned char keycode = at_keybuffer.dequeue();
    at_write(keycode);
  }
  interrupts();
}

void at_waitForCts() {
  while (digitalRead(at_clk) == LOW || digitalRead(at_data) == LOW) {
    delayMicroseconds(15);
  }
  delayMicroseconds(50);
}

void at_sendBit(boolean bit) {
  digitalWrite(at_data, bit ? HIGH : LOW);
  digitalWrite(at_clk, HIGH);
  delayMicroseconds(40);
  digitalWrite(at_clk, LOW);
  delayMicroseconds(40);
}

int at_readBit() {
  digitalWrite(at_clk, LOW);
  delayMicroseconds(40);
  int rbit = digitalRead(at_data) == HIGH ? 1 : 0;
  digitalWrite(at_clk, HIGH);
  delayMicroseconds(40);
  return rbit;
}

unsigned char getOddParity(unsigned char p) {
  p = p ^ (p >> 4 | p << 4);
  p = p ^ (p >> 2);
  p = p ^ (p >> 1);
  return p & 1;
}

unsigned char at_read() {
  pinMode(at_clk, OUTPUT);
  pinMode(at_data, INPUT);

  // prereq for entry: clk is low by host, data is low by host and is start bit value.
  // so we just need to read the 8bits data, 1 parity, and 1 stop bit. 
  // transition to data first
  digitalWrite(at_clk, HIGH);
  delayMicroseconds(40);

  unsigned char data = 0 ; 

  for (int i=0; i < 8; i++) {
    data += at_readBit() << i; 
  } 

  int hostParity = at_readBit();
  if (hostParity != getOddParity(data)) {
    // error, request resend
  }

  at_readBit(); // stop bit.

  pinMode(at_data, OUTPUT);
  at_sendBit(0); // send the ack bit

  pinMode(at_clk, INPUT);
  pinMode(at_data, INPUT);
  return data;
}

void at_write(unsigned char value, boolean response) { 

  unsigned char parity = getOddParity(value);
  unsigned char bits[8] ;
  byte p = 0 ; 
  byte j = 0 ;

  for (j=0 ; j < 8; j++) {
    if (value & 1) bits[j] = 1 ;
    else bits[j] = 0 ; 
    value = value >> 1 ; 
  }


  pinMode(at_clk, OUTPUT);
  pinMode(at_data, OUTPUT);

  at_sendBit(0); // start bit.
      
  byte i = 0 ; 

  for (i=0; i < 8; i++) {
    at_sendBit(bits[p]);
    p++ ; 
  } 

  at_sendBit(parity);

  at_sendBit(1); // stop bit.

  digitalWrite(at_clk, HIGH); 
  digitalWrite(at_data, HIGH);  
   
  pinMode(at_clk, INPUT);
  pinMode(at_data, INPUT);
}

void at_write(unsigned char value) {
  at_write(value, false);
}

void at_enqueue(unsigned char* value, int sz) {
  noInterrupts();
  for(int i = 0; i < sz; i++) {
    at_keybuffer.enqueue(value[i]);
  }
  interrupts();
}

void handleHostData() {
  unsigned char command = at_read();
  at_write(0xFA, true);
}

#endif


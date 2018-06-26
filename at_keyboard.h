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
boolean at_isHostRTS();
void at_sendBit(boolean bit);
int at_readBit();
unsigned char getOddParity(unsigned char p);
unsigned char at_read();
void at_write(unsigned char value);
void at_enqueue(unsigned char* value, int sz);
void handleHostData();

void opencWrite(int pin, int value) {
  if (value == HIGH) {
    pinMode(pin, INPUT);
  } else {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

int opencRead(int pin) {
  pinMode(pin, INPUT);
  delayMicroseconds(5);
  return digitalRead(pin);
}

void at_setup() {
  opencWrite(at_clk, HIGH); 
  opencWrite(at_data, LOW);

  pinMode(ledd, OUTPUT);
  digitalWrite(ledd, LOW);
}

boolean at_isHostRTS() {
  if (opencRead(at_clk) == LOW) {
    int duration = 65;
    while(duration > 0 && opencRead(at_clk) == LOW) {
      duration -= 5;
      delayMicroseconds(5);
    }
    if (opencRead(at_clk) == HIGH && opencRead(at_data) == LOW) {
      return true;
    }
  }
  return false;
}

void at_loop() {
  noInterrupts();
  boolean rts = at_isHostRTS();
  if (rts) {
    handleHostData();
  } else {
    while(!at_keybuffer.isEmpty()) {
      unsigned char keycode = at_keybuffer.dequeue();
      at_write(keycode);
    }
  }
  interrupts();
}

void at_sendBit(boolean bit) {
  opencWrite(at_clk, HIGH);
  opencWrite(at_data, bit ? HIGH : LOW);
  delayMicroseconds(40);
  opencWrite(at_clk, LOW);
  delayMicroseconds(40);
  opencWrite(at_clk, HIGH);
}

int at_readBit() {
  opencWrite(at_clk, HIGH);
  delayMicroseconds(40);
  opencWrite(at_clk, LOW);
  int rbit = opencRead(at_data) == HIGH ? 1 : 0;
  delayMicroseconds(40);
  opencWrite(at_clk, HIGH);

  return rbit;
}

unsigned char getOddParity(unsigned char p) {
  p = p ^ (p >> 4 | p << 4);
  p = p ^ (p >> 2);
  p = p ^ (p >> 1);
  return p & 1;
}

unsigned char at_read() {
  unsigned char data = 0;

  at_readBit(); // clock out the start bit.

  for (int i=0; i < 8; i++) {
    data += at_readBit() << i;
  } 

  int hostParity = at_readBit();
  if (hostParity != getOddParity(data)) {
    digitalWrite(ledd, HIGH);
  } else {
    digitalWrite(ledd, LOW);
  }

  at_readBit(); // read stop bit.

  at_sendBit(0); // send the ack bit

  delayMicroseconds(40);
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

  at_sendBit(0); // start bit.
      
  byte i = 0 ; 

  for (i=0; i < 8; i++) {
    at_sendBit(bits[p]);
    p++ ; 
  }

  at_sendBit(parity);

  at_sendBit(1); // stop bit.

  opencWrite(at_clk, HIGH); 
  opencWrite(at_data, HIGH);  
   
  delayMicroseconds(40);
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
  delayMicroseconds(30);

  if (command == 0xED) {
    at_write(0xFA);
    unsigned char indic = at_read();
    if (indic & 0x04) {
      //digitalWrite(ledd, HIGH);
    } else {
      //digitalWrite(ledd, LOW);
    }
    at_write(0xFA);
  } else if (command == 0xFF) {
    // supposed to flash my LEDs too - LOL, but this keyboard doesn't have LEDS
    at_write(0xAA);
  } else {
    at_write(0xFA);
  }
}

#endif


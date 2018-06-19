#ifndef _AT_KEYBOARD_H
#define _AT_KEYBOARD_H

#include <QueueArray.h>

QueueArray<unsigned char> at_keybuffer;
boolean selftest = true;

#define at_clk 0
#define at_data 1

void at_write(unsigned char value);

void at_setup() {
  pinMode(at_clk, OUTPUT); 
  pinMode(at_data, OUTPUT); 
  digitalWrite(at_clk, HIGH); 
  digitalWrite(at_data, LOW);
  delayMicroseconds(40);
  pinMode(at_clk, INPUT);
  pinMode(at_data, INPUT); 
}

void at_loop() {
  // Respond to power-on self test
  //if (selftest && digitalRead(at_clk) == LOW) {
  //  delay(10);
  //  at_write(0xAA);
  //  selftest = false;
  //} else {
    noInterrupts();
    while(!at_keybuffer.isEmpty()) {
      unsigned char keycode = at_keybuffer.dequeue();
      at_write(keycode);      
    }
    interrupts();
  //}
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

unsigned char getOddParity(unsigned char p) {
  p = p ^ (p >> 4 | p << 4);
  p = p ^ (p >> 2);
  p = p ^ (p >> 1);
  return p & 1;
}

void at_write(unsigned char value) { 

   unsigned char parity = getOddParity(value);
   unsigned char bits[8] ;
   byte p = 0 ; 
   byte j = 0 ;

   for (j=0 ; j < 8; j++) {
     if (value & 1) bits[j] = 1 ;
     else bits[j] = 0 ; 
     value = value >> 1 ; 
   }

   at_waitForCts();

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

void at_break(unsigned char value) {
  noInterrupts();
  at_keybuffer.enqueue(0xF0);
  at_keybuffer.enqueue(value);
  interrupts();
}

void at_make(unsigned char value) {
  noInterrupts();
  at_keybuffer.enqueue(value);
  interrupts();
}



#endif


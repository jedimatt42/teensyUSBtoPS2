#include <hidboot.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

const int ledPin = 13;

// Reboot support
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#define NUMLOCK_STARTUP true
#define CAPSLOCK_STARTUP false
#define SCROLLLOCK_STARTUP false

void setKeyboardLocks(int num, int caps, int scrol);

#include "UsbKbdRptParser.h"
#include "at_keyboard.h"

USB     Usb;
USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

UsbKbdRptParser Prs;

long lastGoodState;
long firstBoot;

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  firstBoot = lastGoodState = millis();

  // toggle led while waiting for usb to initialize
  int toggle = HIGH;
  // Wait for keyboard to be up
  while (Usb.Init() == -1) {
    delay(20);
    toggle = toggle == HIGH ? LOW : HIGH;
    digitalWrite(ledPin, toggle);
  }
  digitalWrite(ledPin, HIGH);

  HidKeyboard.SetReportParser(0, (HIDReportParser*)&Prs);
}

void loop()
{
  // Read USB input which queues scancodes. 
  digitalWrite(ledPin, LOW);
  Usb.Task();

  long loopMillis = millis();
  uint8_t state = Usb.getUsbTaskState();
  
  if (state != USB_STATE_RUNNING) {
    if ( (loopMillis - lastGoodState) > 5000 ) {
      CPU_RESTART;
    }
  } else {
    lastGoodState = loopMillis;
    if (firstBoot != 0) {
      // Set numlock and capslock on, leave scroll lock off.
      setKeyboardLocks(NUMLOCK_STARTUP, CAPSLOCK_STARTUP, SCROLLLOCK_STARTUP);
      firstBoot = 0; 
      at_setup();
    }
  }
  if (firstBoot == 0) {
    at_loop();
  }
}

void setKeyboardLocks(int numlock, int capslock, int scrolllock) {
  Prs.setKeyLocks(&HidKeyboard, numlock, capslock, scrolllock);
}



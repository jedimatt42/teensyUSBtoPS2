#ifndef _USB_KBD_RPT_PARSER_H
#define _USB_KBD_RPT_PARSER_H

#include <hidboot.h>

#include "USBCodes.h"
#include "ATCodes.h"
#include "at_keyboard.h"

//-----------------------------------------------
// USB Keyboard input handling - mostly taken
//   from the USB Host Boot Keyboard example.

class UsbKbdRptParser : public KeyboardReportParser
{
  protected:
    virtual void OnControlKeysChanged(uint8_t before, uint8_t after);
    virtual void OnKeyDown	(uint8_t mod, uint8_t key);
    virtual void OnKeyUp	(uint8_t mod, uint8_t key);

  private:
    void updateModifier(uint8_t mask, uint8_t before, uint8_t after, uint16_t xtkey);
    void setKeyState(uint16_t xtkey, boolean state);
    void handleKey(uint8_t usbkey, boolean state);

  public:
    UsbKbdRptParser();
    void setKeyLocks(USBHID* hid, boolean numLock, boolean capsLock, boolean scrollLock);
};

UsbKbdRptParser::UsbKbdRptParser() {
}

void UsbKbdRptParser::setKeyLocks(USBHID* hid, boolean numLock, boolean capsLock, boolean scrollLock) { 
  if (numLock) {
    kbdLockingKeys.kbdLeds.bmNumLock = 1;
  }
  if (capsLock) {
    kbdLockingKeys.kbdLeds.bmCapsLock = 1;
  }
  if (scrollLock) {
    kbdLockingKeys.kbdLeds.bmScrollLock = 1;
  }
  hid->SetReport(0, 0, 2, 0, 1, &(kbdLockingKeys.bLeds));
}

void UsbKbdRptParser::updateModifier(uint8_t mask, uint8_t before, uint8_t after, uint16_t xtkey) {
  boolean wasMod = before & mask;
  boolean isMod = after & mask;
  if (wasMod && (!isMod)) {
    // RELEASE MODIFIER
    setKeyState(xtkey, false);
  } else if (isMod && (!wasMod)) {
    // PRESS MODIFIER
    setKeyState(xtkey, true);
  }
}

void UsbKbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  updateModifier(U_RIGHTSHIFT, before, after, AT_RSHIFT);
  updateModifier(U_LEFTSHIFT, before, after, AT_LSHIFT);
  updateModifier(U_RIGHTALT, before, after, AT_RALT);
  updateModifier(U_LEFTALT, before, after, AT_LALT);
  updateModifier(U_RIGHTCTRL, before, after, AT_RCTRL);
  updateModifier(U_LEFTCTRL, before, after, AT_LCTRL);
}

void UsbKbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  handleKey(key, true);
}

void UsbKbdRptParser::OnKeyUp(uint8_t mod, uint8_t key) {
  handleKey(key, false);
}

void UsbKbdRptParser::setKeyState(uint16_t xtkey, boolean make) {
  uint8_t prefix = (uint8_t) (0xFF & (xtkey >> 8));
  if (prefix) {
    at_make(prefix);
  }

  if (make) {
    at_make((uint8_t)(xtkey & 0xFF));
  } else {
    at_break((uint8_t)(xtkey & 0xFF));
  }
}

#define NUMLOCK(X,Y) (kbdLockingKeys.kbdLeds.bmNumLock?X:Y)  

void UsbKbdRptParser::handleKey(uint8_t usbkey, boolean state) {
  Serial.println("key pressed");
  switch(usbkey) {
    case U_BACKQUOTE: 
      setKeyState(AT_BACKQUOTE, state); break;
    case U_OPENSQUARE: 
      setKeyState(AT_OPENSQUARE, state); break;
    case U_CLOSESQUARE:
      setKeyState(AT_CLOSESQUARE, state); break;
    case U_BACKSLASH:
      setKeyState(AT_BACKSLASH, state); break;
    case U_SLASH:
      setKeyState(AT_SLASH, state); break;
    case U_HYPHEN:
      setKeyState(AT_HYPHEN, state); break;
    case U_QUOTE:
      setKeyState(AT_QUOTE, state); break;
    case U_EQUAL:
      setKeyState(AT_EQUAL, state); break;
    case U_SEMICOLON:
      setKeyState(AT_SEMICOLON, state); break;
    case U_ENTER:
      setKeyState(AT_ENTER, state); break;
    case U_COMMA:
      setKeyState(AT_COMMA, state); break;
    case U_PERIOD:
      setKeyState(AT_PERIOD, state); break;
    case U_SPACE: 
      setKeyState(AT_SPACE, state); break;
    case U_BACKSPACE:
      setKeyState(AT_BACKSPACE, state); break;
    case U_LEFTARROW:
      setKeyState(AT_LEFT, state); break;
    case U_RIGHTARROW:
      setKeyState(AT_RIGHT, state); break;
    case U_UPARROW: 
      setKeyState(AT_UP, state); break;
    case U_DOWNARROW: 
      setKeyState(AT_DOWN, state); break;
    case U_HOME:
      setKeyState(AT_HOME, state); break;
    case U_END:
      setKeyState(AT_END, state); break;
    case U_F1:
      setKeyState(AT_F1, state); break;
    case U_F2:
      setKeyState(AT_F2, state); break;
    case U_F3:
      setKeyState(AT_F3, state); break;
    case U_F4:
      setKeyState(AT_F4, state); break;
    case U_F5:
      setKeyState(AT_F5, state); break;
    case U_F6:
      setKeyState(AT_F6, state); break;
    case U_F7:
      setKeyState(AT_F7, state); break;
    case U_F8:
      setKeyState(AT_F8, state); break;
    case U_F9:
      setKeyState(AT_F9, state); break;
    case U_F10:
      setKeyState(AT_F10, state); break;
    case U_F11:
      setKeyState(AT_F11, state); break;
    case U_F12:
      setKeyState(AT_F12, state); break;
    case U_DELETE:
      setKeyState(AT_DEL, state); break;
    case U_INSERT:
      setKeyState(AT_INSERT, state); break;
    case U_BREAK:
      setKeyState(AT_PAUSE, state); break;
    case U_PGDN:
      setKeyState(AT_PGDN, state); break;
    case U_PGUP: 
      setKeyState(AT_PGUP, state); break;
    case U_TAB: 
      setKeyState(AT_TAB, state); break;
    case U_ESC:
      setKeyState(AT_ESC, state); break;
    case U_CAPSLOCK:
      setKeyState(AT_CAPS_LOCK, state); break;
    case U_NUM1: 
      setKeyState(AT_NUM1, state); break;
    case U_NUM2:
      setKeyState(AT_NUM2, state); break;
    case U_NUM3:
      setKeyState(AT_NUM3, state); break;
    case U_NUM4:
      setKeyState(AT_NUM4, state); break;
    case U_NUM5:
      setKeyState(AT_NUM5, state); break;
    case U_NUM6:
      setKeyState(AT_NUM6, state); break;
    case U_NUM7:
      setKeyState(AT_NUM7, state); break;
    case U_NUM8:
      setKeyState(AT_NUM8, state); break;
    case U_NUM9:
      setKeyState(AT_NUM9, state); break;
    case U_NUM0: 
      setKeyState(AT_NUM0, state); break;
    case U_NUMPAD_1:
      setKeyState(NUMLOCK(AT_NUM1,AT_NUMPAD_1), state); break;
    case U_NUMPAD_2:
      setKeyState(NUMLOCK(AT_NUM2,AT_NUMPAD_2), state); break;
    case U_NUMPAD_3:
      setKeyState(NUMLOCK(AT_NUM3,AT_NUMPAD_3), state); break;
    case U_NUMPAD_4:
      setKeyState(NUMLOCK(AT_NUM4,AT_NUMPAD_4), state); break;
    case U_NUMPAD_5:
      setKeyState(NUMLOCK(AT_NUM5,AT_NUMPAD_5), state); break;
    case U_NUMPAD_6:
      setKeyState(NUMLOCK(AT_NUM6,AT_NUMPAD_6), state); break;
    case U_NUMPAD_7:
      setKeyState(NUMLOCK(AT_NUM7,AT_NUMPAD_7), state); break;
    case U_NUMPAD_8:
      setKeyState(NUMLOCK(AT_NUM8,AT_NUMPAD_8), state); break;
    case U_NUMPAD_9:
      setKeyState(NUMLOCK(AT_NUM9,AT_NUMPAD_9), state); break;
    case U_NUMPAD_0:
      setKeyState(NUMLOCK(AT_NUM0,AT_NUMPAD_0), state); break;
    case U_NUMPAD_PERIOD:
      setKeyState(NUMLOCK(AT_PERIOD,AT_NUMPAD_PERIOD), state); break;
    case U_NUMSLASH:
      setKeyState(AT_NUMPAD_SLASH, state); break;
    case U_NUMPAD_STAR:
      setKeyState(AT_NUMPAD_ASTERISK, state); break;
    case U_NUMPAD_HYPHEN:
      setKeyState(AT_NUMPAD_HYPHEN, state); break;
    case U_NUMPAD_PLUS:
      setKeyState(AT_NUMPAD_PLUS, state); break;
    case U_NUMPAD_ENTER:
      setKeyState(AT_NUMPAD_ENTER, state); break;
    case U_Q:
      setKeyState(AT_Q, state); break;
    case U_W:
      setKeyState(AT_W, state); break;
    case U_E: 
      setKeyState(AT_E, state); break;
    case U_R: 
      setKeyState(AT_R, state); break;
    case U_T: 
      setKeyState(AT_T, state); break;
    case U_Y: 
      setKeyState(AT_Y, state); break;
    case U_U: 
      setKeyState(AT_U, state); break;
    case U_I: 
      setKeyState(AT_I, state); break;
    case U_O: 
      setKeyState(AT_O, state); break;
    case U_P: 
      setKeyState(AT_P, state); break;
    case U_A: 
      setKeyState(AT_A, state); break;
    case U_S:
      setKeyState(AT_S, state); break;
    case U_D: 
      setKeyState(AT_D, state); break;
    case U_F: 
      setKeyState(AT_F, state); break;
    case U_G: 
      setKeyState(AT_G, state); break;
    case U_H:
      setKeyState(AT_H, state); break;
    case U_J:
      setKeyState(AT_J, state); break;
    case U_K:
      setKeyState(AT_K, state); break;
    case U_L:
      setKeyState(AT_L, state); break;
    case U_Z: 
      setKeyState(AT_Z, state); break;
    case U_X:
      setKeyState(AT_X, state); break;
    case U_C:
      setKeyState(AT_C, state); break;
    case U_V: 
      setKeyState(AT_V, state); break;
    case U_B:
      setKeyState(AT_B, state); break;
    case U_N:
      setKeyState(AT_N, state); break;
    case U_M:
      setKeyState(AT_M, state); break;
  }
}



#endif


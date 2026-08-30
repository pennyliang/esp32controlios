#include "BleComboMouse.h"

void BleComboMouse::click(uint8_t b)
{
  _buttons = b;
  move(0,0,0,0);
  _buttons = 0;
  move(0,0,0,0);
}

void BleComboMouse::move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
  if (_keyboard->isConnected())
  {
    uint8_t m[5];
    m[0] = _buttons;
    m[1] = x;
    m[2] = y;
    m[3] = wheel;
    m[4] = hWheel;
    _keyboard->inputMouse->setValue(m, 5);
    _keyboard->inputMouse->notify();
  }
}

void BleComboMouse::moveTo(uint16_t x, uint16_t y)
{
  if (_keyboard->isConnected())
  {
    if (x > 32767) x = 32767;
    if (y > 32767) y = 32767;
    uint8_t m[5];
    m[0] = _buttons;              // 复用当前按键状态,方便"按住+定位"组合
    m[1] = x & 0xFF;             // X 低字节
    m[2] = (x >> 8) & 0xFF;      // X 高字节
    m[3] = y & 0xFF;             // Y 低字节
    m[4] = (y >> 8) & 0xFF;      // Y 高字节
    _keyboard->inputMouseAbs->setValue(m, 5);
    _keyboard->inputMouseAbs->notify();
  }
}


void BleComboMouse::buttons(uint8_t b)
{
  if (b != _buttons)
  {
    _buttons = b;
    move(0,0,0,0);
  }
}

void BleComboMouse::press(uint8_t b)
{
  buttons(_buttons | b);
}

void BleComboMouse::release(uint8_t b)
{
  buttons(_buttons & ~b);
}

bool BleComboMouse::isPressed(uint8_t b)
{
  if ((b & _buttons) > 0)
    return true;
  return false;
}

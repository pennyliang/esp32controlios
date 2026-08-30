#include <Arduino.h>
#include "BleComboMouse.h"

void BleComboMouse::click(uint8_t b)
{
  // 注意:普通 click 走相对通道(inputMouse),在绝对定位场景下会点在错误位置。
  // 绝对定位场景请用 clickAbs() 或 moveToWithButtons()。此函数保留兼容。
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

// 内部:发一帧绝对报告(带按键位)
void BleComboMouse::sendAbsFrame(uint16_t x, uint16_t y, uint8_t btns)
{
  uint8_t m[5];
  m[0] = btns;
  m[1] = x & 0xFF;
  m[2] = (x >> 8) & 0xFF;
  m[3] = y & 0xFF;
  m[4] = (y >> 8) & 0xFF;
  _keyboard->inputMouseAbs->setValue(m, 5);
  _keyboard->inputMouseAbs->notify();
}

// 绝对移动:从上一次坐标"滑"到目标(多帧),等同一次微型拖动。
// iOS 会忽略孤立的单帧绝对位置报告,但对连续多帧(每帧坐标不同)正常响应。
void BleComboMouse::moveTo(uint16_t x, uint16_t y)
{
  if (!_keyboard->isConnected()) return;
  if (x > 32767) x = 32767;
  if (y > 32767) y = 32767;

  const int steps = 8;                  // 分 8 帧滑过去,iOS 稳定注册
  int32_t fx = _lastX, fy = _lastY;     // 从上一次坐标出发
  for (int i = 1; i <= steps; i++) {
    uint16_t ix = (uint16_t)(fx + (int32_t)(x - fx) * i / steps);
    uint16_t iy = (uint16_t)(fy + (int32_t)(y - fy) * i / steps);
    sendAbsFrame(ix, iy, _buttons);
    delay(6);
  }
  _lastX = x; _lastY = y;               // 记住终点,供 clickAbs()/下次滑动起点
}

// 绝对坐标 + 按键状态一起发(点击/拖拽用,和 moveTo 走同一绝对通道)
void BleComboMouse::moveToWithButtons(uint16_t x, uint16_t y, uint8_t btns)
{
  if (!_keyboard->isConnected()) return;
  if (x > 32767) x = 32767;
  if (y > 32767) y = 32767;
  _lastX = x; _lastY = y;
  sendAbsFrame(x, y, btns);
}

// 在最后一次绝对定位的坐标处点击左键(按下→抬起,都走绝对通道)
void BleComboMouse::clickAbs()
{
  moveToWithButtons(_lastX, _lastY, 1);   // 按下左键
  delay(60);
  moveToWithButtons(_lastX, _lastY, 0);   // 抬起
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

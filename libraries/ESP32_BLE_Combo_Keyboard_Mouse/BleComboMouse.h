#ifndef ESP32_BLE_COMBO_MOUSE_H
#define ESP32_BLE_COMBO_MOUSE_H
#include "BleComboKeyboard.h"

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) // For compatibility with the Mouse library

class BleComboMouse {
private:
  BleComboKeyboard* _keyboard;
  uint8_t _buttons;
  void buttons(uint8_t b);
  void sendAbsFrame(uint16_t x, uint16_t y, uint8_t btns);  // 内部:发一帧绝对报告
public:
  BleComboMouse(BleComboKeyboard* keyboard) { _keyboard = keyboard; };
  void begin(void) {};
  void end(void) {};
  void click(uint8_t b = MOUSE_LEFT);
  void move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
  void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
  void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
  bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default

  // ---- 绝对定位扩展(走 inputMouseAbs 绝对通道)----
  void moveTo(uint16_t x, uint16_t y);                          // 绝对移动(多帧滑动,iOS 稳定注册)
  void moveToWithButtons(uint16_t x, uint16_t y, uint8_t btns); // 绝对坐标 + 按键位一起发
  void clickAbs();                                              // 在最后一次绝对坐标处点击左键
  uint16_t _lastX = 16383;   // 最后一次绝对定位的 X(默认屏幕中央)
  uint16_t _lastY = 16383;   // 最后一次绝对定位的 Y
};

#endif // ESP32_BLE_COMBO_MOUSE_H

#include "devices/InputPanel.h"

#include <Arduino.h>

#include "AppConfig.h"

void InputPanel::begin() {
  pinMode(AppConfig::PIN_FAN_SELECT, INPUT_PULLUP);
  pinMode(AppConfig::PIN_LID_SWITCH, INPUT_PULLUP);
  pinMode(AppConfig::PIN_IOT_ONOFF, INPUT_PULLUP);
  pinMode(AppConfig::PIN_SW_RESET, INPUT_PULLUP);
  refresh();
}

void InputPanel::refresh() {
  snapshot_.fanSelectActive = digitalRead(AppConfig::PIN_FAN_SELECT) == LOW;
  snapshot_.lidOpen = digitalRead(AppConfig::PIN_LID_SWITCH) == LOW;
  snapshot_.iotOnOffPressed = digitalRead(AppConfig::PIN_IOT_ONOFF) == LOW;
  snapshot_.softResetPressed = digitalRead(AppConfig::PIN_SW_RESET) == LOW;
  snapshot_.tachALevel = digitalRead(AppConfig::PIN_FAN_A_TACH) == HIGH;
  snapshot_.tachBLevel = digitalRead(AppConfig::PIN_FAN_B_TACH) == HIGH;
}

const InputSnapshot &InputPanel::getSnapshot() const {
  return snapshot_;
}

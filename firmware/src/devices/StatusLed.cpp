#include "devices/StatusLed.h"

#include <Arduino.h>

#include "AppConfig.h"

void StatusLed::begin() {
  pinMode(AppConfig::PIN_LED_STATUS, OUTPUT);
  digitalWrite(AppConfig::PIN_LED_STATUS, LOW);
}

void StatusLed::update(bool degradedState) {
  const unsigned long blinkInterval =
      degradedState ? AppConfig::LED_WARNING_BLINK_MS : AppConfig::LED_OK_BLINK_MS;
  const unsigned long now = millis();

  if (now - lastToggleMs_ < blinkInterval) {
    return;
  }

  lastToggleMs_ = now;
  ledState_ = !ledState_;
  digitalWrite(AppConfig::PIN_LED_STATUS, ledState_ ? HIGH : LOW);
}

#pragma once

class StatusLed {
 public:
  void begin();
  void update(bool degradedState);

 private:
  unsigned long lastToggleMs_ = 0;
  bool ledState_ = false;
};

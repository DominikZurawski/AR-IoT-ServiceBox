#pragma once

#include "TelemetryTypes.h"

class InputPanel {
 public:
  void begin();
  void refresh();
  const InputSnapshot &getSnapshot() const;

 private:
  InputSnapshot snapshot_ = {};
};

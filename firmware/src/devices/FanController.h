#pragma once

#include "TelemetryTypes.h"

class FanController {
 public:
  void begin();
  void sampleTachometers();
  void updateAutoPwm();
  void updateRpmEstimate();
  void setManualPwm(uint8_t pwmValue);
  void resumeAutoPwm();
  FanTelemetry getTelemetry() const;

 private:
 void applyPwm(uint8_t pwmValue);
  bool tachometersEnabled() const;
  bool fanPowerDiagnosticEnabled() const;

  unsigned long lastFanStepMs_ = 0;
  unsigned long lastTachSampleMs_ = 0;
  unsigned long lastRpmComputeMs_ = 0;
  bool lastTachALevel_ = false;
  bool lastTachBLevel_ = false;
  bool manualPwmActive_ = false;
  size_t fanStepIndex_ = 0;
  uint8_t currentPwm_ = 0;
  uint32_t tachAEdgesWindow_ = 0;
  uint32_t tachBEdgesWindow_ = 0;
  uint32_t fanARpm_ = 0;
  uint32_t fanBRpm_ = 0;
};

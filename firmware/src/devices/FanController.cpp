#include "devices/FanController.h"

#include <Arduino.h>

#include "AppConfig.h"

void FanController::begin() {
  pinMode(AppConfig::PIN_FAN_A_PWM, OUTPUT);
  pinMode(AppConfig::PIN_FAN_B_PWM, OUTPUT);

  if (tachometersEnabled()) {
    pinMode(AppConfig::PIN_FAN_A_TACH, INPUT);
    pinMode(AppConfig::PIN_FAN_B_TACH, INPUT);
  }

  ledcSetup(AppConfig::FAN_PWM_CHANNEL_A, AppConfig::FAN_PWM_FREQUENCY_HZ, AppConfig::FAN_PWM_RESOLUTION_BITS);
  ledcSetup(AppConfig::FAN_PWM_CHANNEL_B, AppConfig::FAN_PWM_FREQUENCY_HZ, AppConfig::FAN_PWM_RESOLUTION_BITS);
  ledcAttachPin(AppConfig::PIN_FAN_A_PWM, AppConfig::FAN_PWM_CHANNEL_A);
  ledcAttachPin(AppConfig::PIN_FAN_B_PWM, AppConfig::FAN_PWM_CHANNEL_B);

  if (tachometersEnabled()) {
    lastTachALevel_ = digitalRead(AppConfig::PIN_FAN_A_TACH) == HIGH;
    lastTachBLevel_ = digitalRead(AppConfig::PIN_FAN_B_TACH) == HIGH;
  } else {
    lastTachALevel_ = false;
    lastTachBLevel_ = false;
    fanARpm_ = 0;
    fanBRpm_ = 0;
  }

  if (fanPowerDiagnosticEnabled()) {
    applyPwm(AppConfig::FAN_POWER_DIAGNOSTIC_PWM);
  } else {
    applyPwm(AppConfig::FAN_PWM_STEPS[0]);
  }
}

void FanController::sampleTachometers() {
  if (!tachometersEnabled()) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastTachSampleMs_ < AppConfig::TACH_SAMPLE_INTERVAL_MS) {
    return;
  }

  lastTachSampleMs_ = now;

  const bool tachALevel = digitalRead(AppConfig::PIN_FAN_A_TACH) == HIGH;
  const bool tachBLevel = digitalRead(AppConfig::PIN_FAN_B_TACH) == HIGH;

  if (tachALevel != lastTachALevel_) {
    ++tachAEdgesWindow_;
    lastTachALevel_ = tachALevel;
  }

  if (tachBLevel != lastTachBLevel_) {
    ++tachBEdgesWindow_;
    lastTachBLevel_ = tachBLevel;
  }
}

void FanController::updateAutoPwm() {
  if (fanPowerDiagnosticEnabled()) {
    return;
  }

  if (manualPwmActive_) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastFanStepMs_ < AppConfig::FAN_STEP_INTERVAL_MS) {
    return;
  }

  lastFanStepMs_ = now;
  fanStepIndex_ = (fanStepIndex_ + 1U) % (sizeof(AppConfig::FAN_PWM_STEPS) / sizeof(AppConfig::FAN_PWM_STEPS[0]));
  applyPwm(AppConfig::FAN_PWM_STEPS[fanStepIndex_]);
}

void FanController::updateRpmEstimate() {
  if (!tachometersEnabled()) {
    fanARpm_ = 0;
    fanBRpm_ = 0;
    tachAEdgesWindow_ = 0;
    tachBEdgesWindow_ = 0;
    return;
  }

  const unsigned long now = millis();
  const unsigned long elapsedMs = now - lastRpmComputeMs_;
  if (elapsedMs < AppConfig::TELEMETRY_INTERVAL_MS) {
    return;
  }

  lastRpmComputeMs_ = now;

  const float elapsedMinutes = static_cast<float>(elapsedMs) / 60000.0f;
  const float pulsesA = static_cast<float>(tachAEdgesWindow_) / static_cast<float>(AppConfig::TACH_PULSES_PER_REV);
  const float pulsesB = static_cast<float>(tachBEdgesWindow_) / static_cast<float>(AppConfig::TACH_PULSES_PER_REV);

  fanARpm_ = elapsedMinutes > 0.0f ? static_cast<uint32_t>(pulsesA / elapsedMinutes) : 0U;
  fanBRpm_ = elapsedMinutes > 0.0f ? static_cast<uint32_t>(pulsesB / elapsedMinutes) : 0U;

  tachAEdgesWindow_ = 0;
  tachBEdgesWindow_ = 0;
}

void FanController::setManualPwm(uint8_t pwmValue) {
  if (fanPowerDiagnosticEnabled()) {
    return;
  }

  manualPwmActive_ = true;
  applyPwm(pwmValue);
}

void FanController::resumeAutoPwm() {
  if (fanPowerDiagnosticEnabled()) {
    return;
  }

  manualPwmActive_ = false;
}

FanTelemetry FanController::getTelemetry() const {
  return {fanARpm_, fanBRpm_, currentPwm_};
}

void FanController::applyPwm(uint8_t pwmValue) {
  currentPwm_ = pwmValue;
  ledcWrite(AppConfig::FAN_PWM_CHANNEL_A, currentPwm_);
  ledcWrite(AppConfig::FAN_PWM_CHANNEL_B, currentPwm_);
}

bool FanController::tachometersEnabled() const {
  return AppConfig::ENABLE_TACHOMETERS;
}

bool FanController::fanPowerDiagnosticEnabled() const {
  return AppConfig::ENABLE_FAN_POWER_DIAGNOSTIC;
}

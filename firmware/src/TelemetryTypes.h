#pragma once

#include <Arduino.h>

struct InputSnapshot {
  bool fanSelectActive;
  bool lidOpen;
  bool iotOnOffPressed;
  bool softResetPressed;
  bool tachALevel;
  bool tachBLevel;
};

struct Sht31Reading {
  bool available;
  float temperatureC;
  float humidityPct;
};

struct FanTelemetry {
  uint32_t fanARpm;
  uint32_t fanBRpm;
  uint8_t fanPwm;
};

enum class FaultMode {
  None,
  Overtemperature,
  FanBlocked,
};

struct ServiceBoxTelemetry {
  const char *deviceId;
  unsigned long timestampMs;
  const char *systemState;
  float temperatureEnvC;
  float temperaturePowerC;
  float humidityPct;
  uint32_t fanARpm;
  uint32_t fanBRpm;
  bool lidOpen;
  bool fanSelectActive;
  bool iotOnOffPressed;
  bool softResetPressed;
  uint8_t fanPwm;
  const char *errorCode;
  bool sensorOk;
  bool wifiConnected;
  bool mqttConnected;
  const char *activeFault;
};

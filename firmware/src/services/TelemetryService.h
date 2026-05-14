#pragma once

#include "TelemetryTypes.h"

class TelemetryService {
 public:
  explicit TelemetryService(const char *deviceId);

  ServiceBoxTelemetry build(
      const Sht31Reading &sensorReading,
      const InputSnapshot &inputs,
      const FanTelemetry &fanTelemetry,
      FaultMode faultMode,
      bool wifiConnected,
      bool mqttConnected) const;

  size_t serialize(const ServiceBoxTelemetry &telemetry, char *buffer, size_t bufferSize) const;
  bool isDegraded(const ServiceBoxTelemetry &telemetry) const;

 private:
  float computePowerTemperatureC(uint8_t fanPwm) const;
  const char *faultModeToString(FaultMode faultMode) const;
  void applyFaultInjection(ServiceBoxTelemetry &telemetry, FaultMode faultMode) const;
  void updateSystemState(ServiceBoxTelemetry &telemetry, FaultMode faultMode) const;

  const char *deviceId_;
};

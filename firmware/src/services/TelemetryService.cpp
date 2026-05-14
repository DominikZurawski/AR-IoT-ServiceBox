#include "services/TelemetryService.h"

#include <ArduinoJson.h>
#include <cstring>

#include "AppConfig.h"

namespace {

void setNullableFloat(StaticJsonDocument<512> &doc, const char *key, bool available, float value) {
  if (available) {
    doc[key] = serialized(String(value, 1));
  } else {
    doc[key] = nullptr;
  }
}

}  // namespace

TelemetryService::TelemetryService(const char *deviceId) : deviceId_(deviceId) {}

ServiceBoxTelemetry TelemetryService::build(
    const Sht31Reading &sensorReading,
    const InputSnapshot &inputs,
    const FanTelemetry &fanTelemetry,
    FaultMode faultMode,
    bool wifiConnected,
    bool mqttConnected) const {
  ServiceBoxTelemetry telemetry = {};
  telemetry.deviceId = deviceId_;
  telemetry.timestampMs = millis();
  telemetry.temperatureEnvC = sensorReading.available ? sensorReading.temperatureC : NAN;
  telemetry.temperaturePowerC = computePowerTemperatureC(fanTelemetry.fanPwm);
  telemetry.humidityPct = sensorReading.available ? sensorReading.humidityPct : NAN;
  telemetry.fanARpm = fanTelemetry.fanARpm;
  telemetry.fanBRpm = fanTelemetry.fanBRpm;
  telemetry.lidOpen = inputs.lidOpen;
  telemetry.fanSelectActive = inputs.fanSelectActive;
  telemetry.iotOnOffPressed = inputs.iotOnOffPressed;
  telemetry.softResetPressed = inputs.softResetPressed;
  telemetry.fanPwm = fanTelemetry.fanPwm;
  telemetry.sensorOk = sensorReading.available;
  telemetry.wifiConnected = wifiConnected;
  telemetry.mqttConnected = mqttConnected;
  telemetry.activeFault = faultModeToString(faultMode);

  applyFaultInjection(telemetry, faultMode);
  updateSystemState(telemetry, faultMode);
  return telemetry;
}

size_t TelemetryService::serialize(const ServiceBoxTelemetry &telemetry, char *buffer, size_t bufferSize) const {
  StaticJsonDocument<512> doc;
  doc["device_id"] = telemetry.deviceId;
  doc["timestamp_ms"] = telemetry.timestampMs;
  doc["system_state"] = telemetry.systemState;
  setNullableFloat(doc, "temperature_env_c", telemetry.sensorOk, telemetry.temperatureEnvC);
  doc["temperature_power_c"] = serialized(String(telemetry.temperaturePowerC, 1));
  setNullableFloat(doc, "humidity_pct", telemetry.sensorOk, telemetry.humidityPct);
  doc["fan_a_rpm"] = telemetry.fanARpm;
  doc["fan_b_rpm"] = telemetry.fanBRpm;
  doc["fan_pwm"] = telemetry.fanPwm;
  doc["fan_select"] = telemetry.fanSelectActive;
  doc["lid_open"] = telemetry.lidOpen;
  doc["iot_onoff_pressed"] = telemetry.iotOnOffPressed;
  doc["soft_reset_pressed"] = telemetry.softResetPressed;
  doc["sensor_ok"] = telemetry.sensorOk;
  doc["wifi_connected"] = telemetry.wifiConnected;
  doc["mqtt_connected"] = telemetry.mqttConnected;
  doc["error_code"] = telemetry.errorCode;
  doc["active_fault"] = telemetry.activeFault;

  return serializeJson(doc, buffer, bufferSize);
}

bool TelemetryService::isDegraded(const ServiceBoxTelemetry &telemetry) const {
  return std::strcmp(telemetry.systemState, "OK") != 0 || !telemetry.wifiConnected || !telemetry.mqttConnected;
}

float TelemetryService::computePowerTemperatureC(uint8_t fanPwm) const {
  const float pwmRatio = static_cast<float>(fanPwm) / 255.0f;
  return AppConfig::POWER_TEMPERATURE_IDLE_C + (AppConfig::POWER_TEMPERATURE_GAIN_C * pwmRatio);
}

const char *TelemetryService::faultModeToString(FaultMode faultMode) const {
  switch (faultMode) {
    case FaultMode::Overtemperature:
      return "overtemperature";
    case FaultMode::FanBlocked:
      return "fan_blocked";
    case FaultMode::None:
    default:
      return "";
  }
}

void TelemetryService::applyFaultInjection(ServiceBoxTelemetry &telemetry, FaultMode faultMode) const {
  if (faultMode == FaultMode::Overtemperature) {
    telemetry.temperatureEnvC = AppConfig::CRITICAL_THRESHOLD_C + 5.0f;
    telemetry.temperaturePowerC = AppConfig::CRITICAL_THRESHOLD_C + 8.0f;
    telemetry.sensorOk = true;
  }

  if (faultMode == FaultMode::FanBlocked) {
    telemetry.fanARpm = 0;
    telemetry.fanBRpm = 0;
  }
}

void TelemetryService::updateSystemState(ServiceBoxTelemetry &telemetry, FaultMode faultMode) const {
  telemetry.systemState = "OK";
  telemetry.errorCode = "";

  if (!telemetry.sensorOk) {
    telemetry.systemState = "WARNING";
    telemetry.errorCode = "SENSOR_READ_FAILED";
    return;
  }

  if (faultMode == FaultMode::FanBlocked) {
    telemetry.systemState = "ERROR";
    telemetry.errorCode = "FAN_BLOCKED";
    return;
  }

  if (telemetry.temperatureEnvC >= AppConfig::CRITICAL_THRESHOLD_C ||
      telemetry.temperaturePowerC >= AppConfig::CRITICAL_THRESHOLD_C) {
    telemetry.systemState = "ERROR";
    telemetry.errorCode = "OVERHEAT";
    return;
  }

  if (telemetry.temperatureEnvC >= AppConfig::OVERHEAT_THRESHOLD_C ||
      telemetry.temperaturePowerC >= AppConfig::OVERHEAT_THRESHOLD_C) {
    telemetry.systemState = "WARNING";
    telemetry.errorCode = "OVERHEAT";
    return;
  }

  if (telemetry.lidOpen) {
    telemetry.systemState = "WARNING";
    telemetry.errorCode = "LID_OPEN";
  }
}

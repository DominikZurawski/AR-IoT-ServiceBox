#pragma once

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "TelemetryTypes.h"

class FanController;

class MqttService {
 public:
  MqttService(const char *deviceId, FanController &fanController);

  void begin();
  void loop();
  void publishTelemetry(const char *payload);
  bool isWifiConnected() const;
  bool isMqttConnected();
  FaultMode getFaultMode() const;
  const char *getTelemetryTopic() const;
  const char *getCommandTopic() const;

 private:
  static void mqttCallbackRouter(char *topic, byte *payload, unsigned int length);
  void connectWifi();
  void connectMqtt();
  void printPayload(byte *payload, unsigned int length);
  void handleSetFanPwm(JsonVariantConst root);
  void handleSimulateFault(JsonVariantConst root);
  void handleCommand(const JsonDocument &doc);
  void mqttCallback(char *topic, byte *payload, unsigned int length);
  void buildTopics();

  const char *deviceId_;
  FanController &fanController_;
  WiFiClient wifiClient_;
  PubSubClient mqttClient_;
  FaultMode faultMode_ = FaultMode::None;
  unsigned long lastWifiRetryMs_ = 0;
  unsigned long lastMqttRetryMs_ = 0;
  char telemetryTopic_[96] = {};
  char commandTopic_[96] = {};

  static MqttService *instance_;
};

#include "services/MqttService.h"

#include <ArduinoJson.h>
#include <cstring>

#include "AppConfig.h"
#include "devices/FanController.h"

MqttService *MqttService::instance_ = nullptr;

MqttService::MqttService(const char *deviceId, FanController &fanController)
    : deviceId_(deviceId), fanController_(fanController), mqttClient_(wifiClient_) {}

void MqttService::begin() {
  instance_ = this;
  buildTopics();
  mqttClient_.setServer(AppConfig::MQTT_HOST, AppConfig::MQTT_PORT);
  mqttClient_.setCallback(MqttService::mqttCallbackRouter);
  mqttClient_.setBufferSize(512);
}

void MqttService::loop() {
  connectWifi();
  connectMqtt();
  mqttClient_.loop();
}

void MqttService::publishTelemetry(const char *payload) {
  if (!mqttClient_.connected()) {
    return;
  }

  if (!mqttClient_.publish(telemetryTopic_, payload, true)) {
    Serial.println("MQTT publish failed");
  }
}

bool MqttService::isWifiConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

bool MqttService::isMqttConnected() {
  return mqttClient_.connected();
}

FaultMode MqttService::getFaultMode() const {
  return faultMode_;
}

const char *MqttService::getTelemetryTopic() const {
  return telemetryTopic_;
}

const char *MqttService::getCommandTopic() const {
  return commandTopic_;
}

void MqttService::mqttCallbackRouter(char *topic, byte *payload, unsigned int length) {
  if (instance_ != nullptr) {
    instance_->mqttCallback(topic, payload, length);
  }
}

void MqttService::connectWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastWifiRetryMs_ < AppConfig::WIFI_RETRY_INTERVAL_MS) {
    return;
  }

  lastWifiRetryMs_ = now;

  if (std::strcmp(AppConfig::WIFI_SSID, "YOUR_WIFI_SSID") == 0) {
    Serial.println("WiFi not configured. Update WIFI_SSID/WIFI_PASSWORD in AppConfig.h.");
    return;
  }

  Serial.printf("Connecting to WiFi SSID '%s'...\n", AppConfig::WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AppConfig::WIFI_SSID, AppConfig::WIFI_PASSWORD);
}

void MqttService::connectMqtt() {
  if (WiFi.status() != WL_CONNECTED || mqttClient_.connected()) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastMqttRetryMs_ < AppConfig::MQTT_RETRY_INTERVAL_MS) {
    return;
  }

  lastMqttRetryMs_ = now;

  Serial.printf("Connecting to MQTT broker %s:%u...\n", AppConfig::MQTT_HOST, AppConfig::MQTT_PORT);

  bool connected = false;
  if (std::strlen(AppConfig::MQTT_USERNAME) > 0U) {
    connected = mqttClient_.connect(deviceId_, AppConfig::MQTT_USERNAME, AppConfig::MQTT_PASSWORD);
  } else {
    connected = mqttClient_.connect(deviceId_);
  }

  if (!connected) {
    Serial.printf("MQTT connect failed, rc=%d\n", mqttClient_.state());
    return;
  }

  Serial.println("MQTT connected");
  mqttClient_.subscribe(commandTopic_);
  Serial.printf("Subscribed to %s\n", commandTopic_);
}

void MqttService::printPayload(byte *payload, unsigned int length) {
  Serial.print("MQTT payload: ");
  for (unsigned int i = 0; i < length; ++i) {
    Serial.print(static_cast<char>(payload[i]));
  }
  Serial.println();
}

void MqttService::handleSetFanPwm(JsonVariantConst root) {
  if (!root["value"].is<int>()) {
    Serial.println("MQTT command rejected: missing integer 'value'");
    return;
  }

  const int requestedPwm = root["value"].as<int>();
  const int normalizedPwm = requestedPwm <= 100 ? map(constrain(requestedPwm, 0, 100), 0, 100, 0, 255)
                                                : constrain(requestedPwm, 0, 255);
  const uint8_t pwmValue = static_cast<uint8_t>(normalizedPwm);

  fanController_.setManualPwm(pwmValue);
  Serial.printf("MQTT command applied: set_fan_pwm=%u (requested=%d)\n", pwmValue, requestedPwm);
}

void MqttService::handleSimulateFault(JsonVariantConst root) {
  const char *fault = root["fault"] | "";

  if (std::strcmp(fault, "overtemperature") == 0) {
    faultMode_ = FaultMode::Overtemperature;
    Serial.println("MQTT command applied: simulate_fault=overtemperature");
    return;
  }

  if (std::strcmp(fault, "fan_blocked") == 0) {
    faultMode_ = FaultMode::FanBlocked;
    Serial.println("MQTT command applied: simulate_fault=fan_blocked");
    return;
  }

  if (std::strcmp(fault, "clear") == 0) {
    faultMode_ = FaultMode::None;
    Serial.println("MQTT command applied: clear fault state");
    return;
  }

  Serial.printf("MQTT command rejected: unknown fault '%s'\n", fault);
}

void MqttService::handleCommand(const JsonDocument &doc) {
  const char *command = doc["command"] | "";

  if (std::strcmp(command, "set_fan_pwm") == 0) {
    handleSetFanPwm(doc.as<JsonVariantConst>());
    return;
  }

  if (std::strcmp(command, "simulate_fault") == 0) {
    handleSimulateFault(doc.as<JsonVariantConst>());
    return;
  }

  if (std::strcmp(command, "resume_auto_pwm") == 0) {
    fanController_.resumeAutoPwm();
    Serial.println("MQTT command applied: resume_auto_pwm");
    return;
  }

  Serial.printf("MQTT command rejected: unknown command '%s'\n", command);
}

void MqttService::mqttCallback(char *topic, byte *payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  const DeserializationError error = deserializeJson(doc, payload, length);

  Serial.printf("MQTT message received on %s\n", topic);
  printPayload(payload, length);

  if (error) {
    Serial.printf("MQTT command parse failed: %s\n", error.c_str());
    return;
  }

  handleCommand(doc);
}

void MqttService::buildTopics() {
  snprintf(telemetryTopic_, sizeof(telemetryTopic_), "arbox/%s/telemetry", deviceId_);
  snprintf(commandTopic_, sizeof(commandTopic_), "arbox/%s/command", deviceId_);
}

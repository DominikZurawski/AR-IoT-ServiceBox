#include <Arduino.h>

#include "AppConfig.h"
#include "devices/FanController.h"
#include "devices/InputPanel.h"
#include "devices/Sht31Sensor.h"
#include "devices/StatusLed.h"
#include "services/MqttService.h"
#include "services/TelemetryService.h"

namespace {

InputPanel g_inputs;
FanController g_fans;
Sht31Sensor g_sensor;
StatusLed g_statusLed;
TelemetryService g_telemetryService(AppConfig::DEVICE_ID);
MqttService g_mqttService(AppConfig::DEVICE_ID, g_fans);

ServiceBoxTelemetry g_currentTelemetry = {};
char g_telemetryBuffer[512] = {};
unsigned long g_lastTelemetryMs = 0;

void printStartupSummary() {
  Serial.println();
  Serial.println("AR IoT ServiceBox Lite MQTT telemetry test");
  Serial.printf("Device ID: %s\n", AppConfig::DEVICE_ID);
  Serial.printf("I2C SHT31: SDA=GPIO%u, SCL=GPIO%u\n", AppConfig::PIN_I2C_SDA, AppConfig::PIN_I2C_SCL);
  Serial.printf("Fans PWM: A=GPIO%u, B=GPIO%u\n", AppConfig::PIN_FAN_A_PWM, AppConfig::PIN_FAN_B_PWM);
  Serial.printf("Fans TACH: A=GPIO%u, B=GPIO%u\n", AppConfig::PIN_FAN_A_TACH, AppConfig::PIN_FAN_B_TACH);
  Serial.printf(
      "Inputs: FAN_SELECT=GPIO%u, LID=GPIO%u, IOT_ONOFF=GPIO%u, SW_RESET=GPIO%u\n",
      AppConfig::PIN_FAN_SELECT,
      AppConfig::PIN_LID_SWITCH,
      AppConfig::PIN_IOT_ONOFF,
      AppConfig::PIN_SW_RESET);
  Serial.printf("Status LED: GPIO%u\n", AppConfig::PIN_LED_STATUS);
  Serial.printf("Fan PWM frequency: %lu Hz\n", AppConfig::FAN_PWM_FREQUENCY_HZ);
  if (AppConfig::ENABLE_FAN_POWER_DIAGNOSTIC) {
    Serial.printf(
        "Fan power diagnostic: ENABLED, forcing both channels to PWM=%u\n",
        AppConfig::FAN_POWER_DIAGNOSTIC_PWM);
  }
  Serial.printf("MQTT telemetry topic: %s\n", g_mqttService.getTelemetryTopic());
  Serial.printf("MQTT command topic: %s\n", g_mqttService.getCommandTopic());
}

void refreshTelemetry() {
  if (g_sensor.isDetected()) {
    g_sensor.refresh();
  }

  g_inputs.refresh();

  g_currentTelemetry = g_telemetryService.build(
      g_sensor.getReading(),
      g_inputs.getSnapshot(),
      g_fans.getTelemetry(),
      g_mqttService.getFaultMode(),
      g_mqttService.isWifiConnected(),
      g_mqttService.isMqttConnected());

  g_telemetryService.serialize(g_currentTelemetry, g_telemetryBuffer, sizeof(g_telemetryBuffer));
  Serial.println(g_telemetryBuffer);
  g_mqttService.publishTelemetry(g_telemetryBuffer);
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(250);

  g_statusLed.begin();
  g_inputs.begin();
  g_fans.begin();
  g_sensor.begin();
  g_mqttService.begin();

  printStartupSummary();
  refreshTelemetry();
}

void loop() {
  g_mqttService.loop();
  g_fans.sampleTachometers();
  g_fans.updateAutoPwm();
  g_fans.updateRpmEstimate();

  const unsigned long now = millis();
  if (now - g_lastTelemetryMs >= AppConfig::TELEMETRY_INTERVAL_MS) {
    g_lastTelemetryMs = now;
    refreshTelemetry();
  }

  g_statusLed.update(g_telemetryService.isDegraded(g_currentTelemetry));
}

#pragma once

#include <Arduino.h>

#if __has_include("AppSecrets.h")
#include "AppSecrets.h"
#else
#error "Missing AppSecrets.h. Copy AppSecrets.example.h to AppSecrets.h and fill in local credentials."
#endif

namespace AppConfig {

constexpr const char *DEVICE_ID = AppSecrets::DEVICE_ID;
constexpr const char *WIFI_SSID = AppSecrets::WIFI_SSID;
constexpr const char *WIFI_PASSWORD = AppSecrets::WIFI_PASSWORD;
constexpr const char *MQTT_HOST = AppSecrets::MQTT_HOST;
constexpr uint16_t MQTT_PORT = AppSecrets::MQTT_PORT;
constexpr const char *MQTT_USERNAME = AppSecrets::MQTT_USERNAME;
constexpr const char *MQTT_PASSWORD = AppSecrets::MQTT_PASSWORD;

// Hardware bring-up flags
constexpr bool ENABLE_TACHOMETERS = false;
constexpr bool ENABLE_FAN_POWER_DIAGNOSTIC = false;
constexpr uint8_t FAN_POWER_DIAGNOSTIC_PWM = 255;

constexpr uint8_t PIN_I2C_SDA = 21;
constexpr uint8_t PIN_I2C_SCL = 22;

constexpr uint8_t PIN_FAN_A_PWM = 25;
constexpr uint8_t PIN_FAN_B_PWM = 26;

constexpr uint8_t PIN_FAN_A_TACH = 34;
constexpr uint8_t PIN_FAN_B_TACH = 35;

constexpr uint8_t PIN_FAN_SELECT = 32;
constexpr uint8_t PIN_LID_SWITCH = 33;
constexpr uint8_t PIN_IOT_ONOFF = 27;
constexpr uint8_t PIN_SW_RESET = 14;

constexpr uint8_t PIN_LED_STATUS = 13;

constexpr uint8_t SHT31_PRIMARY_ADDRESS = 0x44;
constexpr uint8_t SHT31_SECONDARY_ADDRESS = 0x45;
constexpr uint16_t SHT31_CMD_SINGLE_SHOT_HIGH = 0x2400;

constexpr uint8_t FAN_PWM_CHANNEL_A = 0;
constexpr uint8_t FAN_PWM_CHANNEL_B = 1;
constexpr uint32_t FAN_PWM_FREQUENCY_HZ = 25000;
constexpr uint8_t FAN_PWM_RESOLUTION_BITS = 8;

constexpr unsigned long TELEMETRY_INTERVAL_MS = 1000;
constexpr unsigned long FAN_STEP_INTERVAL_MS = 3000;
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 5000;
constexpr unsigned long MQTT_RETRY_INTERVAL_MS = 3000;
constexpr unsigned long LED_OK_BLINK_MS = 500;
constexpr unsigned long LED_WARNING_BLINK_MS = 150;
constexpr unsigned long TACH_SAMPLE_INTERVAL_MS = 25;

constexpr uint8_t FAN_PWM_STEPS[] = {0, 128, 255, 0};
constexpr float OVERHEAT_THRESHOLD_C = 35.0f;
constexpr float CRITICAL_THRESHOLD_C = 45.0f;
constexpr float POWER_TEMPERATURE_IDLE_C = 28.0f;
constexpr float POWER_TEMPERATURE_GAIN_C = 18.0f;
constexpr uint8_t TACH_PULSES_PER_REV = 2;

}  // namespace AppConfig

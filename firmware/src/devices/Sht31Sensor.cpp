#include "devices/Sht31Sensor.h"

#include <Arduino.h>
#include <Wire.h>

#include "AppConfig.h"

void Sht31Sensor::begin() {
  Wire.begin(AppConfig::PIN_I2C_SDA, AppConfig::PIN_I2C_SCL);
  scanI2cBus();

  detected_ = detectAddress();
  if (!detected_) {
    Serial.printf(
        "SHT31 not responding at 0x%02X or 0x%02X\n",
        AppConfig::SHT31_PRIMARY_ADDRESS,
        AppConfig::SHT31_SECONDARY_ADDRESS);
    reading_ = {false, NAN, NAN};
    return;
  }

  Serial.printf("SHT31 responded at 0x%02X\n", address_);
  refresh();

  if (reading_.available) {
    Serial.printf(
        "SHT31 detected at 0x%02X, initial reading: %.1fC, %.1f%%\n",
        address_,
        reading_.temperatureC,
        reading_.humidityPct);
  } else {
    Serial.printf("SHT31 detected at 0x%02X but measurement read failed\n", address_);
  }
}

void Sht31Sensor::refresh() {
  if (!detected_) {
    reading_ = {false, NAN, NAN};
    return;
  }

  readMeasurement(address_, reading_);
}

bool Sht31Sensor::isDetected() const {
  return detected_;
}

uint8_t Sht31Sensor::getAddress() const {
  return address_;
}

const Sht31Reading &Sht31Sensor::getReading() const {
  return reading_;
}

uint8_t Sht31Sensor::computeCrc(const uint8_t *data, size_t length) const {
  uint8_t crc = 0xFF;

  for (size_t i = 0; i < length; ++i) {
    crc ^= data[i];

    for (uint8_t bit = 0; bit < 8; ++bit) {
      if ((crc & 0x80U) != 0U) {
        crc = static_cast<uint8_t>((crc << 1U) ^ 0x31U);
      } else {
        crc <<= 1U;
      }
    }
  }

  return crc;
}

bool Sht31Sensor::probeAddress(uint8_t address) const {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

void Sht31Sensor::scanI2cBus() const {
  bool foundAnyDevice = false;

  Serial.println("Scanning I2C bus...");

  for (uint8_t address = 1; address < 127; ++address) {
    if (!probeAddress(address)) {
      continue;
    }

    foundAnyDevice = true;
    Serial.printf("  I2C device found at 0x%02X\n", address);
  }

  if (!foundAnyDevice) {
    Serial.println("  No I2C devices found");
  }
}

bool Sht31Sensor::detectAddress() {
  if (probeAddress(AppConfig::SHT31_PRIMARY_ADDRESS)) {
    address_ = AppConfig::SHT31_PRIMARY_ADDRESS;
    return true;
  }

  if (probeAddress(AppConfig::SHT31_SECONDARY_ADDRESS)) {
    address_ = AppConfig::SHT31_SECONDARY_ADDRESS;
    return true;
  }

  address_ = 0;
  return false;
}

bool Sht31Sensor::readMeasurement(uint8_t address, Sht31Reading &reading) const {
  reading = {false, NAN, NAN};

  Wire.beginTransmission(address);
  Wire.write(static_cast<uint8_t>(AppConfig::SHT31_CMD_SINGLE_SHOT_HIGH >> 8));
  Wire.write(static_cast<uint8_t>(AppConfig::SHT31_CMD_SINGLE_SHOT_HIGH & 0xFF));

  if (Wire.endTransmission() != 0) {
    return false;
  }

  delay(20);

  constexpr uint8_t responseLength = 6;
  const uint8_t bytesRead = Wire.requestFrom(address, responseLength);
  if (bytesRead != responseLength) {
    return false;
  }

  uint8_t buffer[responseLength] = {};
  for (uint8_t i = 0; i < responseLength; ++i) {
    buffer[i] = Wire.read();
  }

  if (computeCrc(buffer, 2) != buffer[2] || computeCrc(buffer + 3, 2) != buffer[5]) {
    return false;
  }

  const uint16_t rawTemperature = static_cast<uint16_t>((buffer[0] << 8U) | buffer[1]);
  const uint16_t rawHumidity = static_cast<uint16_t>((buffer[3] << 8U) | buffer[4]);

  reading.available = true;
  reading.temperatureC = -45.0f + (175.0f * static_cast<float>(rawTemperature) / 65535.0f);
  reading.humidityPct = 100.0f * static_cast<float>(rawHumidity) / 65535.0f;
  return true;
}

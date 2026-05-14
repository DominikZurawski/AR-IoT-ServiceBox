#pragma once

#include "TelemetryTypes.h"

class Sht31Sensor {
 public:
  void begin();
  void refresh();
  bool isDetected() const;
  uint8_t getAddress() const;
  const Sht31Reading &getReading() const;

 private:
  uint8_t computeCrc(const uint8_t *data, size_t length) const;
  bool probeAddress(uint8_t address) const;
  void scanI2cBus() const;
  bool detectAddress();
  bool readMeasurement(uint8_t address, Sht31Reading &reading) const;

  uint8_t address_ = 0;
  bool detected_ = false;
  Sht31Reading reading_ = {};
};

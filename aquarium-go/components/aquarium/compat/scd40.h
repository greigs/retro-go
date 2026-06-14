#pragma once
// Stub SCD40 CO2/temperature/humidity sensor. The aquarium app runs with
// FIXED_ENVIRONMENT, so these readings are never actually consulted at runtime;
// the class exists only to satisfy the Aquarium constructor signature.

#include "Arduino.h"

class SCD40 {
 public:
  void init() {}
  bool isFirstReadingReceived() { return false; }
  bool isConnected() { return false; }
  float getTemperature() { return 22.0f; }
  float getTemperatureFahrenheit() { return 71.6f; }
  float getHumidity() { return 50.0f; }
  uint16_t getCO2() { return 420; }
};

#pragma once
// Minimal StateManager stub. The full firmware uses this for settings/sensor
// state; the aquarium app runs with a fixed environment and never reads live
// sensor data, so only the small surface referenced by Aquarium.h is provided.

#include "Arduino.h"
#include "StateDefaults.h"

typedef enum {
  CELSIUS = 0,
  FAHRENHEIT,
} TemperatureUnit;

struct State {
  TemperatureUnit temperatureUnit = CELSIUS;
  struct {
    struct {
      float value = DEFAULT_TEMPERATURE_VALUE * 9.0f / 5.0f + 32.0f;
    } temperature_fahrenheit;
  } environment;
};

class StateManager {
 public:
  explicit StateManager(unsigned long /*saveIntervalMinutes*/ = 10) {}
  State *getState() { return &_state; }

 private:
  State _state;
};

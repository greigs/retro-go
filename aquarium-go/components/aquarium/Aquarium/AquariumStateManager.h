#pragma once
// Persistence for the aquarium tank. In the retro-go app the tank is stored as
// a JSON string in NVS via rg_settings (NS_APP namespace), replacing the
// LittleFS file used by the standalone firmware. The ArduinoJson payload format
// is unchanged so the same (de)serialisation logic is reused.

#include <Arduino.h>
#include <ArduinoJson.h>

#include <memory>
#include <vector>

#include "Fish.h"
#include "Matrix.h"

class AquariumStateManager {
 public:
  AquariumStateManager();
  void saveState(const std::vector<std::unique_ptr<Fish>>& fishArray);
  bool loadState(std::vector<std::unique_ptr<Fish>>& fishArray, Matrix* matrix);

 private:
  static const char* SETTINGS_KEY;
};

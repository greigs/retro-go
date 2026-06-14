#include "AquariumStateManager.h"

#include <stdlib.h>
#include <string>

extern "C" {
#include "rg_settings.h"
}

const char* AquariumStateManager::SETTINGS_KEY = "tank";

AquariumStateManager::AquariumStateManager() {}

void AquariumStateManager::saveState(
    const std::vector<std::unique_ptr<Fish>>& fishArray) {
  JsonDocument doc;
  JsonArray fishesJson = doc["fishes"].to<JsonArray>();

  for (const auto& fish : fishArray) {
    JsonObject fishJson = fishesJson.add<JsonObject>();
    fishJson["age"] = fish->getAge();
    fishJson["health"] = fish->getHealth();
    fishJson["bodyType"] = fish->getBodyType().c_str();
    fishJson["headType"] = fish->getHeadType().c_str();
    fishJson["tailType"] = fish->getTailType().c_str();
    fishJson["finType"] = fish->getFinType().c_str();
    fishJson["motionType"] = fish->getMotionType().c_str();

    JsonArray colorsJson = fishJson["colors"].to<JsonArray>();
    for (const auto& color : fish->getColorsHSV()) {
      JsonObject colorJson = colorsJson.add<JsonObject>();
      colorJson["h"] = color.hue;
      colorJson["s"] = color.sat;
      colorJson["v"] = color.val;
    }
  }

  std::string buffer;
  serializeJson(doc, buffer);
  if (buffer.empty()) {
    log_e("[SAVE] Failed to serialize aquarium state");
    return;
  }

  rg_settings_set_string(NS_APP, SETTINGS_KEY, buffer.c_str());
  rg_settings_commit();
  log_i("[SAVE] Aquarium state saved (%u bytes)",
        (unsigned)buffer.length());
}

bool AquariumStateManager::loadState(
    std::vector<std::unique_ptr<Fish>>& fishArray, Matrix* matrix) {
  char* json = rg_settings_get_string(NS_APP, SETTINGS_KEY, NULL);
  if (json == NULL) {
    log_w("[LOAD] No saved aquarium state");
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    log_e("[LOAD] Failed to parse saved aquarium state");
    free(json);
    return false;
  }

  fishArray.clear();
  JsonArray fishesJson = doc["fishes"];
  for (JsonObject fishJson : fishesJson) {
    Fish::FishDefinition fishDef;
    fishDef.age = fishJson["age"];
    fishDef.health = fishJson["health"];
    fishDef.bodyType = fishJson["bodyType"].as<const char*>();
    fishDef.headType = fishJson["headType"].as<const char*>();
    fishDef.tailType = fishJson["tailType"].as<const char*>();
    fishDef.finType = fishJson["finType"].as<const char*>();
    fishDef.motionType = fishJson["motionType"].as<const char*>();

    JsonArray colorsJson = fishJson["colors"];
    for (JsonObject colorJson : colorsJson) {
      CHSV color;
      color.hue = colorJson["h"];
      color.sat = colorJson["s"];
      color.val = colorJson["v"];
      fishDef.colors.push_back(color);
    }

    fishArray.emplace_back(std::make_unique<Fish>(matrix, fishDef));
  }

  free(json);
  log_i("[LOAD] Aquarium state loaded (%u fish)",
        (unsigned)fishArray.size());
  return true;
}

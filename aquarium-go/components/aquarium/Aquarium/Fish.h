#ifndef FISH_H
#define FISH_H

#include <Arduino.h>
#include <Matrix.h>
#include <PVector.h>

#include <chrono>
#include <random>
#include <utility>
#include <vector>

#include "AquariumSettings.h"
#include "Body/BodyVariations/BodyFactory.h"
#include "Food.h"
#include "Motion/MotionFactory.h"

#ifndef CYD_RICH_CREATURE_MIX
#define CYD_RICH_CREATURE_MIX 0
#endif

class Fish {
  Matrix* matrix = nullptr;
  PVector pos;
  float age;
  float health;
  BodyFactory* bodyFactory = nullptr;
  uint8_t offspringCount = 0;

  std::unique_ptr<Motion>
      motion;  // Use smart pointer for automatic memory management
  std::unique_ptr<Body> body;

  Food* food = nullptr;

  static PVector randomSafePosition(Matrix* matrix) {
    const uint8_t width = matrix->getXResolution();
    const uint8_t height = matrix->getYResolution();

    uint8_t marginX = width > 24 ? 8 : 1;
    uint8_t marginTop = height > 48 ? 16 : 1;
    uint8_t marginBottom = height > 48 ? 16 : 1;

    if (marginTop + marginBottom >= height) {
      marginTop = 1;
      marginBottom = 1;
    }
    if (marginX * 2 >= width) {
      marginX = 1;
    }

    const uint8_t minX = marginX;
    const uint8_t maxX = width > marginX ? width - marginX : width;
    const uint8_t minY = marginTop;
    const uint8_t maxY = height > marginBottom ? height - marginBottom : height;

    return PVector(random(minX, maxX), random(minY, maxY));
  }

  struct BodyMotionType {
    String bodyType;
    String motionType;
    double probability;
  };

 public:
  struct FishDefinition {
    float age;
    float health;
    String bodyType;
    String headType;
    String tailType;
    String finType;
    String motionType;
    std::vector<CHSV> colors;
  };

  FishDefinition fishDefinition;
  const std::vector<CHSV>& getColorsHSV() const {
    return body->getColorPaletteHSV();
  }

  Fish(Matrix* matrix, const FishDefinition& fishDef)
      : matrix(matrix),
        pos(PVector(0, 0)),
        age(fishDef.age),
        health(fishDef.health) {  // Member initializer list
    BodyFactory bodyFactory(matrix);
    initializeAgingRate();
    pos = randomSafePosition(matrix);

    this->body = std::unique_ptr<Body>(bodyFactory.createBody(
        fishDef.bodyType, fishDef.headType, fishDef.tailType, fishDef.finType));
    this->motion =
        MotionFactory::createMotion(fishDef.motionType, pos * PHYSICS_SCALE,
                                    matrix->getXResolution() * PHYSICS_SCALE,
                                    matrix->getYResolution() * PHYSICS_SCALE);

    if (!fishDef.colors.empty()) {
      body->setColorPaletteHSV(fishDef.colors);
    }

    fishDefinition = fishDef;
  }

  Fish(Matrix* matrix, PVector pos = PVector(0, 0), float age = 0,
       float health = 1)
      : matrix(matrix), pos(pos), age(age), health(health) {
    BodyFactory bodyFactory(matrix);
    initializeAgingRate();

    std::vector<BodyMotionType> types = {{"Fish", "Fish", 0.5},
                                         {"Star", "Star", 0.1},
                                         {"Turtle", "Turtle", 0.2},
                                         {"Snake", "Snake", 0.1},
                                         {"Octopus", "Octopus", 0.1}};
#if CYD_RICH_CREATURE_MIX
    types = {{"Fish", "Fish", 0.62},
             {"Star", "Star", 0.18},
             {"Turtle", "Turtle", 0.08},
             {"Snake", "Snake", 0.06},
             {"Octopus", "Octopus", 0.06}};
#endif

    auto selectedType = selectType(types);

    this->body =
        std::unique_ptr<Body>(bodyFactory.createBody(selectedType.bodyType));
    if (!this->body) {
      // Fallback to a default body type if creation fails
      this->body = std::unique_ptr<Body>(bodyFactory.createBody("Fish"));
    }

    if (this->pos.x == 0 && this->pos.y == 0) {
      this->pos = randomSafePosition(matrix);
    }

    // Ensure the motion is created successfully
    this->motion = MotionFactory::createMotion(
        selectedType.motionType, this->pos * PHYSICS_SCALE,
        matrix->getXResolution() * PHYSICS_SCALE,
        matrix->getYResolution() * PHYSICS_SCALE);
    if (!this->motion) {
      // Fallback to a default motion type if creation fails
      this->motion = MotionFactory::createMotion(
          "Fish", this->pos * PHYSICS_SCALE,
          matrix->getXResolution() * PHYSICS_SCALE,
          matrix->getYResolution() * PHYSICS_SCALE);
    }

    fishDefinition = {age,
                      health,
                      selectedType.bodyType,
                      this->body->getHeadType(),
                      this->body->getTailType(),
                      this->body->getFinType(),
                      selectedType.motionType,
                      this->body->getColorPaletteHSV()};
  }

  Fish(Matrix* matrix, PVector pos, float age, const char* bodyType,
       float health = 1)
      : matrix(matrix), pos(pos), age(age), health(health) {
    BodyFactory bodyFactory(matrix);
    initializeAgingRate();

    String selectedType = bodyType;
    if (selectedType != "Fish" && selectedType != "Star" &&
        selectedType != "Turtle" && selectedType != "Snake" &&
        selectedType != "Octopus") {
      selectedType = "Fish";
    }

    this->body =
        std::unique_ptr<Body>(bodyFactory.createBody(selectedType));
    if (!this->body) {
      selectedType = "Fish";
      this->body = std::unique_ptr<Body>(bodyFactory.createBody("Fish"));
    }

    if (this->pos.x == 0 && this->pos.y == 0) {
      this->pos = randomSafePosition(matrix);
    }

    this->motion = MotionFactory::createMotion(
        selectedType, this->pos * PHYSICS_SCALE,
        matrix->getXResolution() * PHYSICS_SCALE,
        matrix->getYResolution() * PHYSICS_SCALE);
    if (!this->motion) {
      selectedType = "Fish";
      this->motion = MotionFactory::createMotion(
          "Fish", this->pos * PHYSICS_SCALE,
          matrix->getXResolution() * PHYSICS_SCALE,
          matrix->getYResolution() * PHYSICS_SCALE);
    }

    fishDefinition = {age,
                      health,
                      selectedType,
                      this->body->getHeadType(),
                      this->body->getTailType(),
                      this->body->getFinType(),
                      selectedType,
                      this->body->getColorPaletteHSV()};
  }
  // ~Fish(); // Destructor

  bool update(long co2 = 600, bool stayInside = false) {
    if (!motion) {
      log_e(
          "Error: motion is null in Fish::update for fish type: %s, motion "
          "type: %s",
          fishDefinition.bodyType.c_str(), fishDefinition.motionType.c_str());
      // Attempt to recreate the motion object
      this->motion = MotionFactory::createMotion(
          fishDefinition.motionType, pos * PHYSICS_SCALE,
          matrix->getXResolution() * PHYSICS_SCALE,
          matrix->getYResolution() * PHYSICS_SCALE);
      if (!this->motion) {
        log_e("Failed to recreate motion for fish type: %s, motion type: %s",
              fishDefinition.bodyType.c_str(),
              fishDefinition.motionType.c_str());
        // If recreation fails, return early to prevent crash
        return false;
      } else {
        log_i(
            "Successfully recreated motion for fish type: %s, motion type: %s",
            fishDefinition.bodyType.c_str(), fishDefinition.motionType.c_str());
      }
    }
    motion->update(age, co2, stayInside);
    pos = motion->getPosition() / PHYSICS_SCALE;
    updateAge(co2);
    updateHealth(co2);
    body->update(pos, motion->getVelocity(), motion->getAngle(), age, health);
    if (food != nullptr) {
      PVector foodDistance = food->getPosition() - pos;
      if (food->isOffScreen()) {
        food = nullptr;
      } else if (foodDistance.mag() < 1) {
        food->eat();
        food = nullptr;
      } else {
        motion->followFood(food->getPosition() * PHYSICS_SCALE);
        // matrix->foreground->drawLine(pos.x,pos.y,food->getPosition().x,food->getPosition().y,
        // CRGB(255,0,0));
      }
    }
    if (age > 1.0)
      age = 0;
    return age > 1.0;
  }

  bool tryReproduce() {
    // Fish can only reproduce if they're between 0.5 and 0.9 age and have less
    // than 2 offspring
    if (age > 0.5 && age < 0.9 && offspringCount < 2) {
      auto seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::mt19937 gen(seed);
      std::uniform_real_distribution<> dis(0.0, 1.0);
      if (dis(gen) < 0.01) {  // 1% chance of reproduction per update
        offspringCount++;     // Increment offspring count
        return true;
      }
    }
    return false;
  }

  PVector getPosition() const {
    return PVector(pos.x, pos.y);
  }

  PVector getVelocity() const {
    return motion->getVelocity();
  }

  void display() {
    if (age < AGE_EGG)
      body->displayEgg();
    else
      body->display();
  }

  float getAge() {
    return age;
  }

  void setFood(Food* assignedFood) {
    food = assignedFood;
  }

  Food* getFood() {
    return food;
  }

  float getHealth() const {
    return health;
  }
  String getBodyType() const {
    return fishDefinition.bodyType;
  }
  String getHeadType() const {
    return fishDefinition.headType;
  }
  String getTailType() const {
    return fishDefinition.tailType;
  }
  String getFinType() const {
    return fishDefinition.finType;
  }
  String getMotionType() const {
    return fishDefinition.motionType;
  }

 private:
  unsigned long lastUpdateTime = millis();
  float agingRate;

  void initializeAgingRate() {
    float baseRate = 1.0f / (FISH_LIFESPAN_DAYS * 24 * 60 * 60 *
                             1000);  // Convert days to milliseconds
    float variation = FISH_LIFESPAN_VARIATION *
                      ((float)random(200) / 100.0f -
                       1.0f);  // Random variation between -20% and +20%
    agingRate = baseRate * (1.0f + variation);
  }

  void updateAge(long co2) {
    unsigned long currentTime = millis();
    float timeDiff = currentTime - lastUpdateTime;
    if (co2 < 2000) {
      age += timeDiff * agingRate;
    }
    lastUpdateTime = currentTime;
  }

  void updateHealth(long co2) {
    if (co2 >= CO2_REALBAD) {
      health -= HEALTH_REDUCTION_RATE_REALBAD;
    } else if (co2 >= CO2_BAD) {
      health -= HEALTH_REDUCTION_RATE_BAD;
    } else if (co2 < CO2_BAD) {
      health += HEALTH_INCREASE_RATE_GOOD;
    }
    health = max(0.0f, min(1.0f, health));
  }

  BodyMotionType selectType(const std::vector<BodyMotionType>& types) {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);

    // Extract probabilities
    std::vector<double> probabilities;
    for (const auto& type : types) {
      probabilities.push_back(type.probability);
    }

    std::discrete_distribution<> dist(probabilities.begin(),
                                      probabilities.end());
    return types[dist(gen)];
  }
};

#endif  // FISH_H

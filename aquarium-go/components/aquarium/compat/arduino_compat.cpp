#include "Arduino.h"

#include <esp_timer.h>
#include <esp_random.h>
#include <rom/ets_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

SerialClass Serial;

unsigned long millis(void) {
  return static_cast<unsigned long>(esp_timer_get_time() / 1000LL);
}

unsigned long micros(void) {
  return static_cast<unsigned long>(esp_timer_get_time());
}

void delay(uint32_t ms) {
  if (ms == 0) {
    taskYIELD();
    return;
  }
  vTaskDelay((ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS);
}

void delayMicroseconds(uint32_t us) {
  ets_delay_us(us);
}

long random(long howbig) {
  if (howbig <= 0) {
    return 0;
  }
  return static_cast<long>(esp_random() % static_cast<uint32_t>(howbig));
}

long random(long howsmall, long howbig) {
  if (howbig <= howsmall) {
    return howsmall;
  }
  const uint32_t span = static_cast<uint32_t>(howbig - howsmall);
  return howsmall + static_cast<long>(esp_random() % span);
}

void randomSeed(unsigned long) {
  // The ESP32 hardware RNG (esp_random) is already seeded by the radio/RC
  // oscillators, so there is nothing to seed here. Kept for API compatibility.
}

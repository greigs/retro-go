#pragma once
// Arduino-compatibility shim for building the aquarium simulation as an
// ESP-IDF / retro-go component. Only the surface actually used by the vendored
// sources is provided. Hardware-specific Arduino APIs (GPIO/PWM) are stubbed
// because the retro-go HAL owns the panel and inputs.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <cmath>
#include <cstdlib>

#include "pgmspace.h"
#include "WString.h"
#include "Print.h"

// ---------------------------------------------------------------------------
// Types
// ---------------------------------------------------------------------------
typedef uint8_t byte;
typedef bool boolean;
typedef uint16_t word;

typedef char __FlashStringHelper;
#ifndef PSTR
#define PSTR(s) (s)
#endif
#ifndef F
#define F(s) (reinterpret_cast<const __FlashStringHelper *>(PSTR(s)))
#endif

// ---------------------------------------------------------------------------
// Math constants and helpers
// ---------------------------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#ifndef HALF_PI
#define HALF_PI 1.5707963267948966192313216916398
#endif
#ifndef TWO_PI
#define TWO_PI 6.283185307179586476925286766559
#endif
#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif
#ifndef EULER
#define EULER 2.718281828459045235360287471352
#endif

using std::abs;

// NOTE: min/max are provided as function templates rather than macros so they
// do not break ArduinoJson / STL headers (which use std::min / std::max). No
// vendored source does `using namespace std`, so unqualified min()/max() bind
// to these, while qualified std::min/std::max keep working.
template <class T, class U>
static inline auto _arduino_min(T a, U b) -> decltype(a < b ? a : b) {
  return a < b ? a : b;
}
template <class T, class U>
static inline auto _arduino_max(T a, U b) -> decltype(a > b ? a : b) {
  return a > b ? a : b;
}
template <class T, class U>
static inline auto min(T a, U b) -> decltype(a < b ? a : b) {
  return a < b ? a : b;
}
template <class T, class U>
static inline auto max(T a, U b) -> decltype(a > b ? a : b) {
  return a > b ? a : b;
}

template <class T, class L, class H>
static inline T constrain(T x, L lo, H hi) {
  return x < lo ? static_cast<T>(lo) : (x > hi ? static_cast<T>(hi) : x);
}

static inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  const long divisor = (in_max - in_min);
  if (divisor == 0) return out_min;
  return (x - in_min) * (out_max - out_min) / divisor + out_min;
}

template <class T>
static inline T sq(T x) {
  return x * x;
}

static inline double radians(double deg) { return deg * DEG_TO_RAD; }
static inline double degrees(double rad) { return rad * RAD_TO_DEG; }

#ifndef lowByte
#define lowByte(w) ((uint8_t)((w) & 0xff))
#endif
#ifndef highByte
#define highByte(w) ((uint8_t)((w) >> 8))
#endif
#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bit
#define bit(b) (1UL << (b))
#endif

// ---------------------------------------------------------------------------
// Timing
// ---------------------------------------------------------------------------
unsigned long millis(void);
unsigned long micros(void);
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);
static inline void yield(void) {}

// ---------------------------------------------------------------------------
// Random (Arduino semantics: random(max) -> [0,max), random(a,b) -> [a,b))
// ---------------------------------------------------------------------------
long random(long howbig);
long random(long howsmall, long howbig);
void randomSeed(unsigned long seed);

// ---------------------------------------------------------------------------
// GPIO / PWM stubs (the retro-go HAL owns hardware; these are no-ops kept for
// source compatibility with vendored code that references them).
// ---------------------------------------------------------------------------
#ifndef HIGH
#define HIGH 1
#endif
#ifndef LOW
#define LOW 0
#endif
#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x3
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x5
#endif

static inline void pinMode(uint8_t, uint8_t) {}
static inline void digitalWrite(uint8_t, uint8_t) {}
static inline int digitalRead(uint8_t) { return 0; }
static inline int analogRead(uint8_t) { return 0; }

// ---------------------------------------------------------------------------
// Serial -> stdout
// ---------------------------------------------------------------------------
class SerialClass : public Print {
 public:
  void begin(unsigned long = 115200) {}
  void end() {}
  int available() { return 0; }
  int read() { return -1; }
  void flush() {}
  operator bool() const { return true; }

  size_t write(uint8_t c) override {
    putchar(c);
    return 1;
  }
  size_t printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vprintf(fmt, args);
    va_end(args);
    return n < 0 ? 0 : n;
  }
  using Print::print;
  using Print::println;
};

extern SerialClass Serial;

// ---------------------------------------------------------------------------
// ESP-IDF style logging macros used by the simulation
// ---------------------------------------------------------------------------
#ifndef log_e
#define log_e(fmt, ...) printf("[E] " fmt "\n", ##__VA_ARGS__)
#endif
#ifndef log_w
#define log_w(fmt, ...) printf("[W] " fmt "\n", ##__VA_ARGS__)
#endif
#ifndef log_i
#define log_i(fmt, ...) printf("[I] " fmt "\n", ##__VA_ARGS__)
#endif
#ifndef log_d
#define log_d(fmt, ...) ((void)0)
#endif
#ifndef log_v
#define log_v(fmt, ...) ((void)0)
#endif

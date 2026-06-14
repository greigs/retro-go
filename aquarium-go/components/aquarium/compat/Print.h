#pragma once
// Minimal Arduino Print base class. GFX (from GFX_Lite) derives from this and
// overrides write(uint8_t); the simulation only ever calls print(const String&)
// via the (compile-time only) text overlay path.

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "WString.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print {
 public:
  virtual ~Print() {}

  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (write(*buffer++)) {
        n++;
      } else {
        break;
      }
    }
    return n;
  }
  size_t write(const char *str) {
    if (!str) return 0;
    return write(reinterpret_cast<const uint8_t *>(str), strlen(str));
  }

  size_t print(const char *s) { return write(s); }
  size_t print(char c) { return write(static_cast<uint8_t>(c)); }
  size_t print(const String &s) { return write(s.c_str()); }
  size_t print(int v, int base = DEC) { return printNumber(v, base); }
  size_t print(unsigned v, int base = DEC) { return printNumber(v, base); }
  size_t print(long v, int base = DEC) { return printNumber(v, base); }
  size_t print(unsigned long v, int base = DEC) { return printNumber(v, base); }
  size_t print(double v) {
    char buf[40];
    int n = snprintf(buf, sizeof(buf), "%f", v);
    return write(reinterpret_cast<const uint8_t *>(buf), n > 0 ? n : 0);
  }

  size_t println() { return write(reinterpret_cast<const uint8_t *>("\r\n"), 2); }
  size_t println(const char *s) { return print(s) + println(); }
  size_t println(const String &s) { return print(s) + println(); }
  size_t println(int v, int base = DEC) { return print(v, base) + println(); }
  size_t println(long v, int base = DEC) { return print(v, base) + println(); }
  size_t println(double v) { return print(v) + println(); }

 private:
  size_t printNumber(long value, int base) {
    char buf[34];
    int n;
    if (base == HEX) {
      n = snprintf(buf, sizeof(buf), "%lx", value);
    } else if (base == OCT) {
      n = snprintf(buf, sizeof(buf), "%lo", value);
    } else {
      n = snprintf(buf, sizeof(buf), "%ld", value);
    }
    return write(reinterpret_cast<const uint8_t *>(buf), n > 0 ? n : 0);
  }
};

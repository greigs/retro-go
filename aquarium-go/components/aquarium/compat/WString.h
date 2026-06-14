#pragma once
// Tiny Arduino String replacement backed by std::string. Only the surface used
// by the vendored aquarium simulation is implemented.

#include <stdint.h>
#include <stdio.h>
#include <string>

class String {
 public:
  String() {}
  String(const char *s) : _s(s ? s : "") {}
  String(const std::string &s) : _s(s) {}
  String(char c) : _s(1, c) {}
  String(const String &o) : _s(o._s) {}

  explicit String(int value, int base = 10) { fromInt(value, base); }
  explicit String(unsigned value, int base = 10) { fromUInt(value, base); }
  explicit String(long value, int base = 10) { fromInt(value, base); }
  explicit String(unsigned long value, int base = 10) { fromUInt(value, base); }
  explicit String(float value, int decimals = 2) { fromFloat(value, decimals); }
  explicit String(double value, int decimals = 2) { fromFloat(value, decimals); }

  String &operator=(const char *s) { _s = s ? s : ""; return *this; }
  String &operator=(const std::string &s) { _s = s; return *this; }
  String &operator=(const String &o) { _s = o._s; return *this; }
  String &operator=(char c) { _s.assign(1, c); return *this; }

  String &operator+=(const char *s) { if (s) _s += s; return *this; }
  String &operator+=(const String &o) { _s += o._s; return *this; }
  String &operator+=(char c) { _s += c; return *this; }

  String operator+(const String &o) const { String r(*this); r._s += o._s; return r; }
  String operator+(const char *s) const { String r(*this); if (s) r._s += s; return r; }
  String operator+(char c) const { String r(*this); r._s += c; return r; }

  bool operator==(const String &o) const { return _s == o._s; }
  bool operator==(const char *s) const { return _s == (s ? s : ""); }
  bool operator!=(const String &o) const { return _s != o._s; }
  bool operator!=(const char *s) const { return _s != (s ? s : ""); }
  bool operator<(const String &o) const { return _s < o._s; }

  char operator[](size_t i) const { return i < _s.size() ? _s[i] : 0; }
  char &operator[](size_t i) { return _s[i]; }

  const char *c_str() const { return _s.c_str(); }
  size_t length() const { return _s.size(); }
  bool isEmpty() const { return _s.empty(); }
  void clear() { _s.clear(); }
  void reserve(size_t n) { _s.reserve(n); }

  bool equals(const String &o) const { return _s == o._s; }
  bool equals(const char *s) const { return _s == (s ? s : ""); }

  int indexOf(char c) const {
    auto p = _s.find(c);
    return p == std::string::npos ? -1 : static_cast<int>(p);
  }

  const std::string &str() const { return _s; }
  operator const std::string &() const { return _s; }

 private:
  std::string _s;

  void fromInt(long value, int base) {
    char buf[34];
    if (base == 10) {
      snprintf(buf, sizeof(buf), "%ld", value);
    } else if (base == 16) {
      snprintf(buf, sizeof(buf), "%lx", value);
    } else {
      snprintf(buf, sizeof(buf), "%ld", value);
    }
    _s = buf;
  }
  void fromUInt(unsigned long value, int base) {
    char buf[34];
    if (base == 16) {
      snprintf(buf, sizeof(buf), "%lx", value);
    } else {
      snprintf(buf, sizeof(buf), "%lu", value);
    }
    _s = buf;
  }
  void fromFloat(double value, int decimals) {
    char buf[40];
    snprintf(buf, sizeof(buf), "%.*f", decimals, value);
    _s = buf;
  }
};

inline String operator+(const char *lhs, const String &rhs) {
  return String(lhs) + rhs;
}

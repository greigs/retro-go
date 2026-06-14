#pragma once
// AVR I/O registers do not exist on the ESP32; this shim exists only so that
// vendored Adafruit-style font sources that #include <avr/io.h> still compile.

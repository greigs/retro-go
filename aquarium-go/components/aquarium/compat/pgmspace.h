#pragma once
// Minimal PROGMEM shim for ESP-IDF (no AVR flash address space). All data
// lives in regular RAM/flash that is directly addressable, so the pgm_read_*
// accessors are plain dereferences.

#include <stdint.h>
#include <string.h>

#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef PGM_P
#define PGM_P const char *
#endif
#ifndef PGM_VOID_P
#define PGM_VOID_P const void *
#endif
#ifndef FPSTR
#define FPSTR(p) ((const char *)(p))
#endif

// C-style casts so this header is usable from both C (glcdfont.c) and C++.
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const uint32_t *)(addr))
#endif
#ifndef pgm_read_float
#define pgm_read_float(addr) (*(const float *)(addr))
#endif
#ifndef pgm_read_ptr
#define pgm_read_ptr(addr) (*(void *const *)(addr))
#endif

#ifndef pgm_read_byte_near
#define pgm_read_byte_near(addr) pgm_read_byte(addr)
#endif
#ifndef pgm_read_word_near
#define pgm_read_word_near(addr) pgm_read_word(addr)
#endif
#ifndef pgm_read_dword_near
#define pgm_read_dword_near(addr) pgm_read_dword(addr)
#endif
#ifndef pgm_read_byte_far
#define pgm_read_byte_far(addr) pgm_read_byte(addr)
#endif

#ifndef memcpy_P
#define memcpy_P(dest, src, n) memcpy((dest), (src), (n))
#endif
#ifndef strcpy_P
#define strcpy_P(dest, src) strcpy((dest), (src))
#endif
#ifndef strncpy_P
#define strncpy_P(dest, src, n) strncpy((dest), (src), (n))
#endif
#ifndef strcmp_P
#define strcmp_P(a, b) strcmp((a), (b))
#endif
#ifndef strlen_P
#define strlen_P(s) strlen((s))
#endif
#ifndef sprintf_P
#define sprintf_P(s, fmt, ...) sprintf((s), (fmt), ##__VA_ARGS__)
#endif

#pragma once
// Single source of truth for the aquarium build configuration. This header is
// force-included (-include AquariumConfig.h) into every translation unit of
// both the `aquarium` component and the app `main`, so the header-only
// simulation (Aquarium.h, Fish.h, ...) is compiled with identical macros
// everywhere (avoiding ODR mismatches). Values mirror the verified
// openbrickgb-s3 platformio.ini build.

// --- Platform / framework selection -----------------------------------------
#ifndef PANEL_CYD_TFT
#define PANEL_CYD_TFT 1
#endif
#ifndef AQUARIUM_ONLY
#define AQUARIUM_ONLY 1
#endif
#ifndef FIXED_ENVIRONMENT
#define FIXED_ENVIRONMENT 1
#endif
#ifndef USE_GFX_LITE
#define USE_GFX_LITE 1
#endif
#ifndef BOARD_HAS_PSRAM
#define BOARD_HAS_PSRAM 1
#endif
// Use direct RAM access for fonts/palettes (no AVR flash address space here).
#ifndef FASTLED_USE_PROGMEM
#define FASTLED_USE_PROGMEM 0
#endif

// --- Renderer ----------------------------------------------------------------
#ifndef CYD_FRAMEBUFFER_RENDERER
#define CYD_FRAMEBUFFER_RENDERER 0
#endif
#ifndef CYD_DOT_RENDERER
#define CYD_DOT_RENDERER 1
#endif

// --- Colour / tone tuning ----------------------------------------------------
#ifndef CYD_TFT_TONE_CORRECTION
#define CYD_TFT_TONE_CORRECTION 1
#endif
#ifndef CYD_TFT_TONE_CURVE_STRENGTH
#define CYD_TFT_TONE_CURVE_STRENGTH 150
#endif
#ifndef CYD_TFT_TONE_BLACK_THRESHOLD
#define CYD_TFT_TONE_BLACK_THRESHOLD 2
#endif
#ifndef CYD_TFT_BACKGROUND_TONE_CURVE_STRENGTH
#define CYD_TFT_BACKGROUND_TONE_CURVE_STRENGTH 0
#endif
#ifndef CYD_TFT_FOREGROUND_TONE_CURVE_STRENGTH
#define CYD_TFT_FOREGROUND_TONE_CURVE_STRENGTH 0
#endif
#ifndef CYD_TFT_FOREGROUND_GAIN
#define CYD_TFT_FOREGROUND_GAIN 165
#endif
#ifndef CYD_TFT_FOREGROUND_SATURATION
#define CYD_TFT_FOREGROUND_SATURATION 235
#endif
#ifndef CYD_BACKGROUND_BRIGHTNESS
#define CYD_BACKGROUND_BRIGHTNESS 68
#endif
#ifndef CYD_BACKGROUND_BLACK_THRESHOLD
#define CYD_BACKGROUND_BLACK_THRESHOLD 2
#endif
#ifndef CYD_FOREGROUND_BRIGHTNESS
#define CYD_FOREGROUND_BRIGHTNESS 235
#endif
#ifndef CYD_FOREGROUND_SATURATION
#define CYD_FOREGROUND_SATURATION 160
#endif

// --- Simulation behaviour ----------------------------------------------------
#ifndef CYD_AUTONOMOUS_LIFE
#define CYD_AUTONOMOUS_LIFE 1
#endif
#ifndef CYD_CURATED_BOOT_POPULATION
#define CYD_CURATED_BOOT_POPULATION 1
#endif
#ifndef CYD_RICH_CREATURE_MIX
#define CYD_RICH_CREATURE_MIX 1
#endif
#ifndef CYD_KEEP_CREATURES_ON_SCREEN
#define CYD_KEEP_CREATURES_ON_SCREEN 1
#endif
#ifndef CYD_KEEP_INSIDE_MARGIN_X
#define CYD_KEEP_INSIDE_MARGIN_X 6
#endif
#ifndef CYD_KEEP_INSIDE_MARGIN_TOP
#define CYD_KEEP_INSIDE_MARGIN_TOP 14
#endif
#ifndef CYD_KEEP_INSIDE_MARGIN_BOTTOM
#define CYD_KEEP_INSIDE_MARGIN_BOTTOM 12
#endif
#ifndef CYD_AUTONOMOUS_BOOT_MIN_MS
#define CYD_AUTONOMOUS_BOOT_MIN_MS 1500
#endif
#ifndef CYD_AUTONOMOUS_BOOT_MAX_MS
#define CYD_AUTONOMOUS_BOOT_MAX_MS 3500
#endif
#ifndef CYD_AUTONOMOUS_FOOD_MIN_MS
#define CYD_AUTONOMOUS_FOOD_MIN_MS 5500
#endif
#ifndef CYD_AUTONOMOUS_FOOD_MAX_MS
#define CYD_AUTONOMOUS_FOOD_MAX_MS 12000
#endif
#ifndef CYD_AUTONOMOUS_MAX_FOOD
#define CYD_AUTONOMOUS_MAX_FOOD 3
#endif
#ifndef CYD_FIXED_HUMIDITY_PERCENT
#define CYD_FIXED_HUMIDITY_PERCENT 50
#endif

// --- Persistence -------------------------------------------------------------
#ifndef CYD_PERSIST_AQUARIUM
#define CYD_PERSIST_AQUARIUM 1
#endif
#ifndef AQUARIUM_SAVE_INTERVAL
#define AQUARIUM_SAVE_INTERVAL 1  // minutes
#endif
#ifndef CYD_RESET_HOLD_MS
#define CYD_RESET_HOLD_MS 5000
#endif

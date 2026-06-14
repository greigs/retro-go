#include "RgMatrix.h"

#include <string.h>

#include "Arduino.h"

extern "C" {
#include "rg_surface.h"
#include "rg_display.h"
}

// Render tuning. These mirror the openbrickgb-s3 platformio.ini values so the
// retro-go app looks identical to the standalone firmware. They can be
// overridden from the component build flags.
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

namespace {

constexpr uint8_t kColorProfileBackground = 1;
constexpr uint8_t kColorProfileForeground = 2;

constexpr float dotRadiusRatio() { return 0.43f; }

inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

uint8_t scaleChannelByBrightness(uint8_t value, uint8_t brightness) {
  return static_cast<uint8_t>(
      (static_cast<uint16_t>(value) * brightness + 127) / 255);
}

uint8_t applyTftToneCurve(uint8_t value, uint16_t strength,
                          uint8_t blackThreshold) {
#if CYD_TFT_TONE_CORRECTION
  if (value == 0 || value == 255) {
    return value;
  }
  strength = strength > 255 ? 255 : strength;
  const uint16_t linearWeight = 255 - strength;
  const uint16_t quadratic = (static_cast<uint16_t>(value) * value + 127) / 255;
  const uint16_t corrected =
      (static_cast<uint16_t>(value) * linearWeight + quadratic * strength +
       127) /
      255;
  if (blackThreshold > 0 && corrected < blackThreshold) {
    return 0;
  }
  return static_cast<uint8_t>(corrected > 255 ? 255 : corrected);
#else
  (void)strength;
  (void)blackThreshold;
  return value;
#endif
}

uint8_t maxChannel(uint8_t r, uint8_t g, uint8_t b) {
  return _arduino_max(r, _arduino_max(g, b));
}

uint8_t luma8(uint8_t r, uint8_t g, uint8_t b) {
  return static_cast<uint8_t>(
      (static_cast<uint16_t>(r) * 77 + static_cast<uint16_t>(g) * 150 +
       static_cast<uint16_t>(b) * 29) >>
      8);
}

uint8_t saturateChannel(uint8_t value, uint8_t luma, uint16_t saturation) {
  const int16_t delta = static_cast<int16_t>(value) - luma;
  const int16_t saturated =
      static_cast<int16_t>(luma) + ((delta * saturation) / 128);
  return static_cast<uint8_t>(constrain(saturated, 0, 255));
}

uint8_t gainChannel(uint8_t value, uint16_t gain) {
  return static_cast<uint8_t>(
      constrain((static_cast<uint16_t>(value) * gain + 64) / 128, 0, 255));
}

CRGB applyTftForegroundProfile(CRGB color) {
  if (color.r == 0 && color.g == 0 && color.b == 0) {
    return color;
  }
  const uint8_t luma = luma8(color.r, color.g, color.b);
  color.r = saturateChannel(color.r, luma, CYD_TFT_FOREGROUND_SATURATION);
  color.g = saturateChannel(color.g, luma, CYD_TFT_FOREGROUND_SATURATION);
  color.b = saturateChannel(color.b, luma, CYD_TFT_FOREGROUND_SATURATION);
  color.r = gainChannel(color.r, CYD_TFT_FOREGROUND_GAIN);
  color.g = gainChannel(color.g, CYD_TFT_FOREGROUND_GAIN);
  color.b = gainChannel(color.b, CYD_TFT_FOREGROUND_GAIN);
  return color;
}

CRGB applyDisplayProfile(CRGB color, uint8_t brightness, uint16_t saturation,
                         uint8_t blackThreshold) {
  if (color.r == 0 && color.g == 0 && color.b == 0) {
    return CRGB(0, 0, 0);
  }
  if (saturation != 128) {
    const uint8_t luma = luma8(color.r, color.g, color.b);
    color.r = saturateChannel(color.r, luma, saturation);
    color.g = saturateChannel(color.g, luma, saturation);
    color.b = saturateChannel(color.b, luma, saturation);
  }
  color.r = scaleChannelByBrightness(color.r, brightness);
  color.g = scaleChannelByBrightness(color.g, brightness);
  color.b = scaleChannelByBrightness(color.b, brightness);
  if (blackThreshold > 0 &&
      maxChannel(color.r, color.g, color.b) < blackThreshold) {
    return CRGB(0, 0, 0);
  }
  return color;
}

uint16_t packRgb565ForDisplay(uint8_t r, uint8_t g, uint8_t b,
                              uint8_t brightnessScale, uint8_t colorProfile) {
  CRGB color(scaleChannelByBrightness(r, brightnessScale),
             scaleChannelByBrightness(g, brightnessScale),
             scaleChannelByBrightness(b, brightnessScale));

  if (colorProfile == kColorProfileForeground) {
    color = applyTftForegroundProfile(color);
    color.r = applyTftToneCurve(color.r, CYD_TFT_FOREGROUND_TONE_CURVE_STRENGTH, 0);
    color.g = applyTftToneCurve(color.g, CYD_TFT_FOREGROUND_TONE_CURVE_STRENGTH, 0);
    color.b = applyTftToneCurve(color.b, CYD_TFT_FOREGROUND_TONE_CURVE_STRENGTH, 0);
  } else {
    color.r = applyTftToneCurve(color.r, CYD_TFT_BACKGROUND_TONE_CURVE_STRENGTH,
                                CYD_TFT_TONE_BLACK_THRESHOLD);
    color.g = applyTftToneCurve(color.g, CYD_TFT_BACKGROUND_TONE_CURVE_STRENGTH,
                                CYD_TFT_TONE_BLACK_THRESHOLD);
    color.b = applyTftToneCurve(color.b, CYD_TFT_BACKGROUND_TONE_CURVE_STRENGTH,
                                CYD_TFT_TONE_BLACK_THRESHOLD);
  }
  return color565(color.r, color.g, color.b);
}

}  // namespace

RgMatrix::RgMatrix() {
  fontSize = 2;
  rotation = 0;
  brightness = 100;
}

RgMatrix::~RgMatrix() {
  delete background;
  delete foreground;
  if (surface_) {
    rg_surface_free(surface_);
  }
}

void RgMatrix::init() {
  surface_ = rg_surface_create(PANEL_WIDTH, PANEL_HEIGHT, RG_PIXEL_565_LE, 0);
  fb_ = surface_ ? static_cast<uint16_t *>(surface_->data) : nullptr;
  clearSurface();

  background = new GFX_Layer(
      LOGICAL_WIDTH, LOGICAL_HEIGHT,
      [this](int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
        drawPixelRGB888(x, y, r, g, b);
      });
  foreground = new GFX_Layer(
      LOGICAL_WIDTH, LOGICAL_HEIGHT,
      [this](int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b) {
        drawPixelRGB888(x, y, r, g, b);
      });

  rg_display_set_scaling(RG_DISPLAY_SCALING_OFF);
  rg_display_set_backlight((display_backlight_t)backlightPercent);
}

uint16_t RgMatrix::toRgb565(uint8_t r, uint8_t g, uint8_t b) {
  return packRgb565ForDisplay(r, g, b, brightness, kColorProfileBackground);
}

void RgMatrix::clearSurface() {
  if (fb_) {
    memset(fb_, 0, static_cast<size_t>(PANEL_WIDTH) * PANEL_HEIGHT *
                       sizeof(uint16_t));
  }
}

void RgMatrix::writeScaledLogicalPixel(uint16_t x, uint16_t y, uint16_t color) {
  if (fb_ == nullptr || x >= LOGICAL_WIDTH || y >= LOGICAL_HEIGHT) {
    return;
  }
  // Black pixels stay as the pre-cleared background; skipping them also gives
  // the dot grid its dark gaps (matching CYD_DOT_RENDERER).
  if (color == 0) {
    return;
  }

  const uint16_t x0 = screenXForLogicalEdge(x);
  uint16_t x1 = screenXForLogicalEdge(x + 1);
  const uint16_t y0 = screenYForLogicalEdge(y);
  uint16_t y1 = screenYForLogicalEdge(y + 1);
  if (x1 <= x0) x1 = x0 + 1;
  if (y1 <= y0) y1 = y0 + 1;

  const uint16_t width = x1 - x0;
  const uint16_t height = y1 - y0;
  const uint16_t minDimension = width < height ? width : height;
  const float radius = _arduino_max(0.75f, minDimension * dotRadiusRatio());
  const float radiusSq = radius * radius;
  const float centerX = (static_cast<float>(x0) + x1 - 1) * 0.5f;
  const float centerY = (static_cast<float>(y0) + y1 - 1) * 0.5f;

  for (uint16_t py = y0; py < y1 && py < PANEL_HEIGHT; ++py) {
    uint16_t *row = fb_ + static_cast<uint32_t>(py) * PANEL_WIDTH;
    for (uint16_t px = x0; px < x1 && px < PANEL_WIDTH; ++px) {
      const float dx = static_cast<float>(px) - centerX;
      const float dy = static_cast<float>(py) - centerY;
      if (dx * dx + dy * dy <= radiusSq) {
        row[px] = color;
      }
    }
  }
}

void RgMatrix::drawPixelRGB888(uint16_t x, uint16_t y, uint8_t r, uint8_t g,
                               uint8_t b) {
  writeScaledLogicalPixel(x, y, toRgb565(r, g, b));
}

void RgMatrix::compositeLayers() {
  if (background == nullptr || foreground == nullptr || fb_ == nullptr) {
    return;
  }

  clearSurface();

  for (uint16_t y = 0; y < LOGICAL_HEIGHT; ++y) {
    for (uint16_t x = 0; x < LOGICAL_WIDTH; ++x) {
      const CRGB foregroundPixel = foreground->pixels->data[y][x];
      const bool hasForeground =
          foregroundPixel != foreground->transparency_colour;
      const CRGB source =
          hasForeground ? foregroundPixel : background->pixels->data[y][x];
      const CRGB corrected =
          hasForeground
              ? applyDisplayProfile(source, CYD_FOREGROUND_BRIGHTNESS,
                                    CYD_FOREGROUND_SATURATION, 0)
              : applyDisplayProfile(source, CYD_BACKGROUND_BRIGHTNESS, 128,
                                    CYD_BACKGROUND_BLACK_THRESHOLD);

      writeScaledLogicalPixel(
          x, y,
          packRgb565ForDisplay(corrected.r, corrected.g, corrected.b, 255,
                               hasForeground ? kColorProfileForeground
                                             : kColorProfileBackground));
    }
  }

  foreground->clear();
}

void RgMatrix::update() {
  if (surface_) {
    rg_display_submit(surface_, 0);
  }
}

void RgMatrix::setBrightness(uint8_t newBrightness) {
  brightness = newBrightness;
}

uint8_t RgMatrix::getBrightness() const { return brightness; }

uint8_t RgMatrix::getXResolution() { return LOGICAL_WIDTH; }
uint8_t RgMatrix::getYResolution() { return LOGICAL_HEIGHT; }

void RgMatrix::setRotation(uint8_t newRotation) { rotation = newRotation; }
void RgMatrix::rotate90() { rotation = (rotation + 1) % 4; }

void RgMatrix::clearScreen() { clearSurface(); }

void RgMatrix::setBacklightPercent(uint8_t percent) {
  backlightPercent = constrain(percent, 1, 100);
  rg_display_set_backlight((display_backlight_t)backlightPercent);
}

#pragma once
// RgMatrix: a Matrix backend that renders the aquarium simulation into a
// retro-go rg_surface_t (RGB565) and pushes it to the panel via
// rg_display_submit(). It reuses the brightness/tone/saturation math and the
// "dot" look from the original TFT_eSPI CydMatrix backend so the aquarium looks
// the same as the standalone firmware.

#include <stdint.h>

#include "Matrix.h"

struct rg_surface_t;

class RgMatrix : public Matrix {
 public:
  // Logical aquarium panel (matches the browser preview's 80x106 device mode
  // and the original CydMatrixSettings).
  static constexpr uint16_t LOGICAL_WIDTH = 80;
  static constexpr uint16_t LOGICAL_HEIGHT = 106;

  // Physical panel and centred viewport (80x106 * 3 = 240x318 inside 240x320).
  static constexpr uint16_t PANEL_WIDTH = 240;
  static constexpr uint16_t PANEL_HEIGHT = 320;
  static constexpr uint16_t VIEWPORT_X = 0;
  static constexpr uint16_t VIEWPORT_Y = 1;
  static constexpr uint16_t VIEWPORT_WIDTH = 240;
  static constexpr uint16_t VIEWPORT_HEIGHT = 318;

  RgMatrix();
  ~RgMatrix() override;

  void init() override;
  void drawPixelRGB888(uint16_t x, uint16_t y, uint8_t r, uint8_t g,
                       uint8_t b) override;
  void setBrightness(uint8_t newBrightness) override;
  uint8_t getBrightness() const override;
  uint8_t getXResolution() override;
  uint8_t getYResolution() override;
  void setRotation(uint8_t newRotation) override;
  void rotate90() override;
  void clearScreen() override;
  void update() override;
  void compositeLayers() override;

  void setBacklightPercent(uint8_t percent);
  uint8_t getBacklightPercent() const { return backlightPercent; }

  rg_surface_t *surface() { return surface_; }

 private:
  static constexpr uint16_t screenXForLogicalEdge(uint16_t logicalX) {
    return VIEWPORT_X + (logicalX * VIEWPORT_WIDTH) / LOGICAL_WIDTH;
  }
  static constexpr uint16_t screenYForLogicalEdge(uint16_t logicalY) {
    return VIEWPORT_Y + (logicalY * VIEWPORT_HEIGHT) / LOGICAL_HEIGHT;
  }

  uint16_t toRgb565(uint8_t r, uint8_t g, uint8_t b);
  void writeScaledLogicalPixel(uint16_t x, uint16_t y, uint16_t color);
  void clearSurface();

  rg_surface_t *surface_ = nullptr;
  uint16_t *fb_ = nullptr;  // surface->data as uint16_t*
  uint8_t backlightPercent = 100;
};

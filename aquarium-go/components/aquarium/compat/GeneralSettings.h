#pragma once
// Trimmed GeneralSettings for the retro-go aquarium app. The app always builds
// the CYD/TFT (portrait 80x106) flavour of the simulation, so PANEL_CYD_TFT is
// expected to be defined by the component build flags.

#define FIRMWARE_VERSION_MAJOR 2
#define FIRMWARE_VERSION_MINOR 0
#define FIRMWARE_VERSION_PATCH 0

#ifndef PANEL_CYD_TFT
#define PANEL_UPCYCLED 1
#endif

#define AQUARIUM_ENABLED 1

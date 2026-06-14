// aquarium-go: the OpenBrickGB aquarium simulation packaged as a retro-go app.
//
// The simulation (vendored under components/aquarium) renders through the
// abstract Matrix interface into GFX_Lite layers. Here we drive it with an
// RgMatrix backend that composites into an rg_surface_t and pushes frames via
// rg_display_submit(). Input, persistence (NVS) and the system menu come from
// the retro-go HAL.

extern "C" {
#include <rg_system.h>
}

#include <esp_random.h>

#include "Aquarium.h"
#include "RgMatrix.h"
#include "StateManager.h"
#include "scd40.h"

#ifndef CYD_RESET_HOLD_MS
#define CYD_RESET_HOLD_MS 5000
#endif

// Frame pacing: the standalone firmware runs the sim at ~30 fps.
#define AQ_FRAME_INTERVAL_MS 33

// Buttons that feed the fish. MENU/OPTION are reserved for the retro-go system
// menu so the user can always change options or return to the launcher.
#define AQ_FEED_KEYS                                                      \
  (RG_KEY_A | RG_KEY_B | RG_KEY_X | RG_KEY_Y | RG_KEY_START |             \
   RG_KEY_SELECT | RG_KEY_UP | RG_KEY_DOWN | RG_KEY_LEFT | RG_KEY_RIGHT | \
   RG_KEY_L | RG_KEY_R)

static rg_app_t *app;
static RgMatrix *matrix;
static Aquarium *aquarium;

static bool save_state_handler(const char *filename) {
  (void)filename;
  if (aquarium) {
    aquarium->saveState();
  }
  return true;
}

static bool load_state_handler(const char *filename) {
  (void)filename;
  return false;
}

static bool reset_handler(bool hard) {
  (void)hard;
  if (aquarium) {
    aquarium->resetToFreshTank();
  }
  return true;
}

static void event_handler(int event, void *arg) {
  (void)arg;
  if (event == RG_EVENT_SHUTDOWN) {
    if (aquarium) {
      aquarium->saveState();
    }
  } else if (event == RG_EVENT_REDRAW) {
    if (matrix) {
      matrix->update();
    }
  }
}

extern "C" void app_main(void) {
  const rg_handlers_t handlers = {
      .loadState = &load_state_handler,
      .saveState = &save_state_handler,
      .reset = &reset_handler,
      .screenshot = NULL,
      .event = &event_handler,
      .memRead = NULL,
      .memWrite = NULL,
      .options = NULL,
      .about = NULL,
  };

  // sampleRate 0: the aquarium has no audio.
  app = rg_system_init(0, &handlers, NULL);

  matrix = new RgMatrix();
  matrix->init();
  matrix->setBacklightPercent(100);

  // Fixed environment + no live sensors, so these are inert stubs that only
  // satisfy the Aquarium constructor.
  static SCD40 scd40;
  static StateManager stateManager;

  randomSeed(esp_random());

  aquarium = new Aquarium(matrix, &scd40, &stateManager);
  aquarium->begin();

  uint32_t prevJoystick = 0;
  bool resetHoldActive = false;
  bool resetHoldFired = false;
  int64_t resetHoldStart = 0;
  int menuHoldTicks = 0;

  while (true) {
    const int64_t frameStart = rg_system_timer();
    const uint32_t joystick = rg_input_read_gamepad();
    const uint32_t pressed = (joystick ^ prevJoystick) & joystick;

    // System menu: OPTION opens options immediately, a MENU long-press opens
    // the game menu (where the user can exit back to the launcher).
    if (joystick & (RG_KEY_MENU | RG_KEY_OPTION)) {
      if (joystick & RG_KEY_OPTION) {
        rg_gui_options_menu();
      } else if (menuHoldTicks++ == 15) {  // ~0.5 s at 30 fps
        rg_gui_game_menu();
      }
    } else {
      menuHoldTicks = 0;
    }

    // Any feed-button press drops food.
    if (pressed & AQ_FEED_KEYS) {
      aquarium->onTouchStarted();
      aquarium->onTouchReleased();
    }

    // Holding any feed button for CYD_RESET_HOLD_MS wipes the tank and starts a
    // fresh curated population (persisted immediately). Fires once per hold.
    const int64_t now = rg_system_timer();
    if (joystick & AQ_FEED_KEYS) {
      if (!resetHoldActive) {
        resetHoldActive = true;
        resetHoldFired = false;
        resetHoldStart = now;
      } else if (!resetHoldFired &&
                 (now - resetHoldStart) >= (int64_t)CYD_RESET_HOLD_MS * 1000) {
        resetHoldFired = true;
        aquarium->resetToFreshTank();
      }
    } else {
      resetHoldActive = false;
    }

    aquarium->update(false);
    aquarium->display();
    matrix->update();

    prevJoystick = joystick;

    rg_system_tick(rg_system_timer() - frameStart);

    const int64_t elapsedMs = (rg_system_timer() - frameStart) / 1000;
    if (elapsedMs < AQ_FRAME_INTERVAL_MS) {
      rg_task_delay(AQ_FRAME_INTERVAL_MS - elapsedMs);
    } else {
      rg_task_yield();
    }
  }
}

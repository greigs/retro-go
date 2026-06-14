# Vendored aquarium simulation + Arduino-compat shim + retro-go Matrix backend.
set(COMPONENT_SRCS
    "compat/arduino_compat.cpp"
    "rg/RgMatrix.cpp"
    "Aquarium/BoidManager.cpp"
    "Aquarium/AquariumStateManager.cpp"
    "EffectManager/Boid.cpp"
    "GFX_Lite/GFX_Layer.cpp"
    "GFX_Lite/GFX_Lite.cpp"
    "GFX_Lite/bitswap.cpp"
    "GFX_Lite/colorpalettes.cpp"
    "GFX_Lite/colorutils.cpp"
    "GFX_Lite/hsv2rgb.cpp"
    "GFX_Lite/lib8tion.cpp"
    "GFX_Lite/noise.cpp"
    "GFX_Lite/glcdfont.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "compat"
    "rg"
    "Matrix"
    "Aquarium"
    "Aquarium/Body"
    "Aquarium/Body/BodyVariations"
    "Aquarium/Motion"
    "EffectManager"
    "GFX_Lite"
    "ArduinoJson"
)

set(COMPONENT_REQUIRES "retro-go" "esp_timer" "esp_hw_support")

register_component()

# -w: the vendored third-party + simulation sources are not warning-clean and
# are built as-is. -include forces the shared build configuration into every
# translation unit so the header-only sim is compiled identically everywhere.
rg_setup_compile_options(
    -w
    -O2
    -include AquariumConfig.h
)

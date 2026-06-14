set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_REQUIRES "retro-go" "aquarium")
register_component()

# Force-include the shared aquarium build configuration so the header-only
# simulation that gets compiled here matches the `aquarium` component exactly.
rg_setup_compile_options(
    -include AquariumConfig.h
)

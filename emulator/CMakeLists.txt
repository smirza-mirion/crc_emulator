# emulator/CMakeLists.txt
# Build the emulator executable

set(EMULATOR_SOURCES
    main_emulator.c
    hal/hal.h
    hal/hal_coldfire.c
    hal/hal_uart.c
    hal/hal_i2c.c
    hal/hal_qspi.c
    hal/hal_pit.c
    hal/hal_fatfs.c
    hal/hal_gpio.c
    hal/hal_usb.c
    hal/hal_rtc.c
    hal/hal_watchdog.c
    hal/hal_screen.c
    hal/hal_keyboard.c
    hal/hal_ethernet.c
    hal/hal_stubs.c
    amulet_bridge/amulet_bridge.c
    amulet_bridge/ws_server.c
    simulation/sim_chamber.c
    simulation/sim_mca.c
    simulation/sim_scenarios.c
)

# Platform-specific sources
if(APPLE)
    list(APPEND EMULATOR_SOURCES platform/platform_macos.c)
elseif(WIN32)
    list(APPEND EMULATOR_SOURCES platform/platform_windows.c)
endif()

add_executable(crc_emulator ${EMULATOR_SOURCES})

target_include_directories(crc_emulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/compat
    ${CMAKE_CURRENT_SOURCE_DIR}/amulet_bridge
    ${CMAKE_CURRENT_SOURCE_DIR}/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
    ${CMAKE_SOURCE_DIR}/firmware/CRC-25R_Calib/Headers
    ${CMAKE_SOURCE_DIR}/firmware/CRC-25R_Calib/fatfs/src
    ${CMAKE_SOURCE_DIR}/firmware/CRC-25R_Calib/USB
    ${CMAKE_SOURCE_DIR}/firmware/CRC-25R_Calib/Mmc
    ${CMAKE_SOURCE_DIR}/firmware/CRC-25R_Calib
)

target_link_libraries(crc_emulator PRIVATE firmware)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(crc_emulator PRIVATE "-framework CoreFoundation" pthread m)
elseif(WIN32)
    target_link_libraries(crc_emulator PRIVATE ws2_32 winmm)
else()
    target_link_libraries(crc_emulator PRIVATE pthread m rt)
endif()

# Install
install(TARGETS crc_emulator DESTINATION bin)

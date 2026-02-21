# firmware/CMakeLists.txt
# Build all firmware C files as a static library, excluding hardware-specific files

# Collect all .c files from CRC-25R_Calib/
file(GLOB FIRMWARE_SOURCES_TOP "CRC-25R_Calib/*.c")
file(GLOB FIRMWARE_SOURCES_MMC "CRC-25R_Calib/Mmc/*.c")
file(GLOB FIRMWARE_SOURCES_USB "CRC-25R_Calib/USB/*.c")

# Exclude all Mmc files (replaced by hal_fatfs.c)
set(FIRMWARE_SOURCES ${FIRMWARE_SOURCES_TOP} ${FIRMWARE_SOURCES_USB})

# Files to exclude (replaced by emulator HAL)
set(FIRMWARE_EXCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/CalibratorMain_R.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Globals_25.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/UartRoutines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/PitRoutines_R.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/I2cRoutines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/EepromRoutines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/RealTimeClock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Qspi_25.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/ChipSelectIO_RP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Screen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/ScreenFonts_25.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Ethernet.c
    # Mmc/ excluded entirely above (not globbed)
    # fatfs/src/ not in glob path
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/USB/sl811h_101508.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/USB/sl811s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/HaltMain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/AppendLanguageStringMain.c
)

# Remove excluded files
list(REMOVE_ITEM FIRMWARE_SOURCES ${FIRMWARE_EXCLUDE})

# Add sqlite3.c separately (it's large and needs special handling)
# sqlite3.c is in the top-level CRC-25R_Calib/ directory and will be included
# by the glob. It just needs some extra defines.

add_library(firmware STATIC ${FIRMWARE_SOURCES})

target_include_directories(firmware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/fatfs/src
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/USB
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib/Mmc
    ${CMAKE_CURRENT_SOURCE_DIR}/CRC-25R_Calib
    ${CMAKE_SOURCE_DIR}/emulator/compat
    ${CMAKE_SOURCE_DIR}/emulator/hal
    ${CMAKE_SOURCE_DIR}/emulator/amulet_bridge
    ${CMAKE_SOURCE_DIR}/emulator/simulation
)

# SQLite3 needs these defines
# SQLITE_OS_OTHER=1 forces SQLite to use the custom capintec VFS (FatFS-based)
# instead of the UNIX VFS. The firmware's sqlite3.c contains a custom VFS at
# the end that uses FatFS for all file I/O, which the emulator's HAL maps to POSIX.
target_compile_definitions(firmware PRIVATE
    SQLITE_OMIT_LOAD_EXTENSION=1
    SQLITE_THREADSAFE=0
    SQLITE_OS_OTHER=1
)

# Disable specific warnings for sqlite3.c (it's third-party code)
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    set_source_files_properties(
        CRC-25R_Calib/sqlite3.c
        PROPERTIES COMPILE_FLAGS "-w"
    )
endif()

cmake_minimum_required(VERSION 3.20)
project(crc_emulator C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Emulator-specific defines
add_definitions(
    -DEMULATOR=1
    -DDEMO=1
    -DMCA_SIMULATION=1
    -DWATCHDOG_OFF=1
    -DFASTSTART=1
    -DINVERSION=1
    -DCRC_R=1
    -DPRINTF_ERRORS=1
)

# Rename firmware functions that conflict with C standard library names.
# The firmware defines toupper(char*) which conflicts with ctype.h toupper(int).
add_definitions(
    -Dtoupper=firmware_toupper
)

# Ensure each global variable gets its own storage (no common section merging).
# This is critical for ASAN to detect global buffer overflows correctly.
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fno-common)
endif()

# Suppress warnings for legacy C code
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wno-implicit-function-declaration
        -Wno-incompatible-pointer-types
        -Wno-int-conversion
        -Wno-pointer-sign
        -Wno-format
        -Wno-return-type
        -Wno-parentheses
        -Wno-switch
        -Wno-unused-variable
        -Wno-unused-but-set-variable
        -Wno-sometimes-uninitialized
        -Wno-tautological-compare
        -Wno-dangling-else
        -Wno-absolute-value
        -Wno-implicit-int
        -Wno-extra-tokens
        -Wno-deprecated-non-prototype
        -Wno-macro-redefined
        -Wno-incompatible-function-pointer-types
        -Wno-nonportable-include-path
        -Wno-incompatible-library-redeclaration
        -Wno-#warnings
    )
endif()

# Platform detection
if(APPLE)
    add_definitions(-DPLATFORM_MACOS=1)
elseif(WIN32)
    add_definitions(-DPLATFORM_WINDOWS=1)
endif()

add_subdirectory(firmware)
add_subdirectory(emulator)

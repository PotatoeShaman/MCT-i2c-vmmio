# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
#include(pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(EMBEDDED_NODE_RP2040 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.c
    i2c/i2c_protocal.c
    vmmio/vmmio.c
    core_1/core_1_process.c
)


# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_ (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib
    pico_multicore
    hardware_i2c
    hardware_timer
    hardware_pwm
    hardware_irq
    
    "${CMAKE_CURRENT_SOURCE_DIR}/rp2040_nodeConstants.h"
)

add_library(vmmio vmmio/vmmio.c vmmio/vmmio.h)
add_library(i2c i2c/i2c_protocal.c i2c/i2c_protocal.h)
add_library(core_1 core_1/core_1_process.c core_1/core_1_process.h)

target_link_libraries(vmmio
    pico_stdlib
    hardware_pwm
    hardware_timer
)

target_link_libraries(i2c
    pico_stdlib
    hardware_i2c
    hardware_irq
)

target_link_libraries(core_1
    pico_multicore
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

#pico_add_extra_outputs(${PROJECT_NAME})
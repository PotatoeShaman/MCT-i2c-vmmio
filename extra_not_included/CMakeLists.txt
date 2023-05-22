
# Include build functions from Pico SDK
#include_directories($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
#include(pico_sdk_import.cmake)

#pico_sdk_init()


#add_library(vmmio vmmio.c)

# Link to pico_ (gpio, time, etc. functions)
#target_link_libraries(vmmio
#    hardware_timer
#    hardware_pwm
#
#    vmmio.h
#)

#target_include_directories(vmmio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
add_library(vmmio STATIC vmmio.c)

#target_sources(vmmio    
#    "vmmio.h"
#    "vmmio.c"
#)
target_link_libraries(vmmio LINK_PUBLIC
    pico_stdlib
    hardware_gpio
)
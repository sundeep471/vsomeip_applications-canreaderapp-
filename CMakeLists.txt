# CMakeLists.txt for vsomeip application

# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.0)

# Project name
project(vsomeip_example)

# Source file
set(SRC main.cpp)

# Define the vsomeipClient target
add_executable(cumns_responder ${SRC})

# Find vsomeip3 libraries
find_library(VSOMEIP3_LIBRARY vsomeip3)
find_library(VSOMEIP3_CFG_LIBRARY vsomeip3-cfg)
find_library(VSOMEIP3_SD_LIBRARY vsomeip3-sd)
find_library(VSOMEIP3_E2E_LIBRARY vsomeip3-e2e)
find_library(MOSQUITTO_LIBRARY mosquitto)

# Link libraries
target_link_libraries(cumns_responder
    ${VSOMEIP3_LIBRARY}
    ${VSOMEIP3_CFG_LIBRARY}
    ${VSOMEIP3_SD_LIBRARY}
    ${VSOMEIP3_E2E_LIBRARY}
    ${MOSQUITTO_LIBRARY}
    pthread
)

install(TARGETS vsomeipClient DESTINATION bin)

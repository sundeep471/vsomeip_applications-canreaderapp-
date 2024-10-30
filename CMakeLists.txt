# CMakeLists.txt for vsomeip application

# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.0)

# Project name
project(data_translation_example)

# Source file
set(SRC main.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the vsomeipClient target
add_executable(cumns_responder ${SRC})

# Find vsomeip3 libraries
find_library(MOSQUITTO_LIBRARY mosquitto)

# Link libraries
target_link_libraries(cumns_responder
    ${MOSQUITTO_LIBRARY}
)

install(TARGETS cumns_responder DESTINATION bin)

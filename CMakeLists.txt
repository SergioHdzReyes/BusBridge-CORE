cmake_minimum_required(VERSION 3.15)
project(busbridge-core VERSION 0.2.0 LANGUAGES C CXX)

# Force Clang
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Headers
include_directories(include)
include_directories(examples)

# Determine system
if(WIN32)
    set(SRC_PLATFORM src/WindowsI2CDevice.cpp)
else()
    set(SRC_PLATFORM src/LinuxI2CDevice.cpp)
endif()

# Static library
add_library(busbridge-core STATIC
        src/I2CDevice.cpp
        ${SRC_PLATFORM}
)

# Examples
add_executable(read_bme280
        src/BME280Reader.cpp
        examples/read_bme280.cpp
)
target_link_libraries(read_bme280 busbridge-core)

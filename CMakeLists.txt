cmake_minimum_required(VERSION 3.15)
project(busbridge-core VERSION 0.3.0 LANGUAGES C CXX)

# Force Clang
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# ----------------------------------
# Global configuration
# ----------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------
# Common directories
# ----------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include_directories(${PROJECT_SOURCE_DIR}/include)
include(packaging/CPackConfig.cmake)

# ----------------------------------
# Subprojects
# ----------------------------------
add_subdirectory(src)
add_subdirectory(examples)

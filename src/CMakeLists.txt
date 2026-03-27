# --------------------------------------------------
# I2C module
# --------------------------------------------------
if(WIN32)
    set(BUSBRIDGE_I2C_PLATFORM_SRC i2c/WindowsI2CDevice.cpp)
else()
    set(BUSBRIDGE_I2C_PLATFORM_SRC i2c/LinuxI2CDevice.cpp)
endif()

set(BUSBRIDGE_I2C_SOURCES
    i2c/I2CDevice.cpp
    i2c/BME280Reader.cpp
    i2c/I2CError.cpp
    ${BUSBRIDGE_I2C_PLATFORM_SRC}
)

# --------------------------------------------------
# Core library (CAN + I2C)
# --------------------------------------------------
set(BUSBRIDGE_CORE_SOURCES
    ${BUSBRIDGE_I2C_SOURCES}
    can/CANMessage.cpp
    can/CANError.cpp
    can/CANDevice.cpp
    can/CANSniffer.cpp
    can/SocketCANInterface.cpp
)

add_library(busbridge-core SHARED ${BUSBRIDGE_CORE_SOURCES})

target_compile_features(busbridge-core PUBLIC cxx_std_17)

target_include_directories(busbridge-core
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(UNIX AND NOT APPLE)
    target_link_libraries(busbridge-core PRIVATE pthread)
endif()

# --------------------------------------------------
# Core aggregator
# --------------------------------------------------
# From here forward the existing core install path is preserved
# (already in busbridge-core target)

# Library and headers installation
install(TARGETS busbridge-core
        EXPORT busbridgeTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "platform/*" EXCLUDE
        PATTERN "platform" EXCLUDE
)

# Export configuration for find_packages()
install(EXPORT busbridgeTargets
        FILE busbridge-coreTargets.cmake
        NAMESPACE busbridge-core::
        DESTINATION lib/cmake/busbridge-core
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/busbridge-coreConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

configure_file(${CMAKE_SOURCE_DIR}/cmake/busbridgeConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/busbridge-coreConfig.cmake
        @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/busbridge-coreConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/busbridge-coreConfigVersion.cmake
        DESTINATION lib/cmake/busbridge-core
)

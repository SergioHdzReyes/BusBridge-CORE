# Set platform
if(WIN32)
    set(SRC_PLATFORM WindowsI2CDevice.cpp)
else()
    set(SRC_PLATFORM LinuxI2CDevice.cpp)
endif()

# Create static library
add_library(busbridge-core STATIC
        I2CDevice.cpp
        BME280Reader.cpp
        I2CError.cpp
        ${SRC_PLATFORM}
)

target_include_directories(busbridge-core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

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

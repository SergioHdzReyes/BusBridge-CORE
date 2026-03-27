/**
 * @file I2CDevice.cpp
 * @brief Implementation of I2CDevice factory class
 */

#include "i2c/I2CDevice.h"

#if defined(_WIN32)
#include "i2c/platform/WindowsI2CDevice.h"
#else
#include "i2c/platform/LinuxI2CDevice.h"
#endif

namespace busbridge::i2c {

/**
 * @brief Factory constructor that creates platform-specific I2C implementation
 *
 * Selects the appropriate I2CInterface implementation based on the platform
 * at compile time. On Linux, uses LinuxI2CDevice. On Windows, would use
 * WindowsI2CDevice (to be implemented).
 */
I2CDevice::I2CDevice(const std::string& bus, uint8_t address) {
#if defined(_WIN32)
    impl = std::make_unique<WindowsI2CDevice>(bus, address);
#else
    impl = std::make_unique<LinuxI2CDevice>(bus, address);
#endif
}

}  // namespace busbridge::i2c

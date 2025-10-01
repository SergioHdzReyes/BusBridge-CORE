#include "I2CDevice.h"

#if defined(_WIN32)
#include "platform/WindowsI2CDevice.h"
#else
#include "platform/LinuxI2CDevice.h"
#endif

I2CDevice::I2CDevice(const std::string& bus, uint8_t address) {
#if defined(_WIN32)
    impl = std::make_unique<WindowsI2CDevice>(bus, address);
#else
    impl = std::make_unique<LinuxI2CDevice>(bus, address);
#endif
}

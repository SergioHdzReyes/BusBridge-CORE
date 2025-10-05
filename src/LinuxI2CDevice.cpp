#include "platform/LinuxI2CDevice.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

LinuxI2CDevice::LinuxI2CDevice(const std::string& bus, uint8_t address)
        : m_bus(bus), m_address(address), m_fd(-1) {}

LinuxI2CDevice::~LinuxI2CDevice() {
    closeBus();
}

bool LinuxI2CDevice::openBus() {
    m_fd = ::open(m_bus.c_str(), O_RDWR);
    if (m_fd < 0) return false;
    if (ioctl(m_fd, I2C_SLAVE, m_address) < 0) return false;
    return true;
}

void LinuxI2CDevice::closeBus() {
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

bool LinuxI2CDevice::writeByte(uint8_t value)
{
    ssize_t result = ::write(m_fd, &value, 1);
    return result == 1;
}

bool LinuxI2CDevice::writeBytes(const uint8_t* data, size_t length)
{
    if (!data || length == 0)
        return false;

    ssize_t result = ::write(m_fd, data, length);
    return result == static_cast<ssize_t>(length);
}

bool LinuxI2CDevice::readByte(uint8_t& value)
{
    ssize_t result = ::read(m_fd, &value, 1);
    return result == 1;
}

bool LinuxI2CDevice::readBytes(uint8_t* buffer, size_t length)
{
    if (!buffer || length == 0)
        return false;

    ssize_t result = ::read(m_fd, buffer, length);
    return result == static_cast<ssize_t>(length);
}

bool LinuxI2CDevice::writeRegister(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = { reg, value };
    return writeBytes(data, 2);
}

bool LinuxI2CDevice::readRegister(uint8_t reg, uint8_t& value)
{
    if (!writeBytes(&reg, 1))
        return false;
    return readBytes(&value, 1);
}

bool LinuxI2CDevice::readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length)
{
    if (!buffer || length == 0)
        return false;

    if (!writeBytes(&reg, 1))
        return false;

    return readBytes(buffer, length);
}

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

bool LinuxI2CDevice::writeByte(uint8_t reg, uint8_t data) {
    uint8_t buffer[2] = {reg, data};
    return ::write(m_fd, buffer, 2) == 2;
}

bool LinuxI2CDevice::writeBytes(uint8_t reg, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> buffer;
    buffer.push_back(reg);
    buffer.insert(buffer.end(), data.begin(), data.end());
    return ::write(m_fd, buffer.data(), buffer.size()) == (ssize_t)buffer.size();
}

bool LinuxI2CDevice::readByte(uint8_t reg, uint8_t& data) {
    if (::write(m_fd, &reg, 1) != 1) return false;
    return ::read(m_fd, &data, 1) == 1;
}

bool LinuxI2CDevice::readBytes(uint8_t reg, std::vector<uint8_t>& data, size_t length) {
    if (::write(m_fd, &reg, 1) != 1) return false;
    data.resize(length);
    return ::read(m_fd, data.data(), length) == (ssize_t)length;
}

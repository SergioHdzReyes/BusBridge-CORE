#pragma once

#include "I2CInterface.h"

#include <string>
#include <vector>
#include <cstdint>

class LinuxI2CDevice: public I2CInterface {
public:
    LinuxI2CDevice(const std::string& bus, uint8_t address);
    ~LinuxI2CDevice() override;

    I2CError openBus() override;
    void closeBus() override;

    I2CError writeByte(uint8_t value) override;
    I2CError writeBytes(const uint8_t* data, size_t length) override;

    I2CError readByte(uint8_t& value) override;
    I2CError readBytes(uint8_t* buffer, size_t length) override;

    I2CError writeRegister(uint8_t reg, uint8_t value) override;
    I2CError readRegister(uint8_t reg, uint8_t& value) override;
    I2CError readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) override;

private:
    std::string m_bus;
    uint8_t m_address;
    int m_fd;
};

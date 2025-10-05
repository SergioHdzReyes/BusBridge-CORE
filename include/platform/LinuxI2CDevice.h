#pragma once

#include "I2CInterface.h"

#include <string>
#include <vector>
#include <cstdint>

class LinuxI2CDevice: public I2CInterface {
public:
    LinuxI2CDevice(const std::string& bus, uint8_t address);
    ~LinuxI2CDevice() override;

    bool openBus() override;
    void closeBus() override;

    bool writeByte(uint8_t value) override;
    bool writeBytes(const uint8_t* data, size_t length) override;

    bool readByte(uint8_t& value) override;
    bool readBytes(uint8_t* buffer, size_t length) override;

    bool writeRegister(uint8_t reg, uint8_t value) override;
    bool readRegister(uint8_t reg, uint8_t& value) override;
    bool readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) override;

private:
    std::string m_bus;
    uint8_t m_address;
    int m_fd;
};

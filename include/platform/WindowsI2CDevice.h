#pragma once

#include "I2CInterface.h"

#include <string>
#include <vector>
#include <cstdint>

class WindowsI2CDevice: public I2CInterface {
public:
    WindowsI2CDevice(const std::string& bus, uint8_t address) {}
    ~WindowsI2CDevice() override {}

    bool openBus() override { return false; }
    void closeBus() override {}

    bool writeByte(uint8_t value) override { return false; }
    bool writeBytes(const uint8_t* data, size_t length) override { return false; }

    bool readByte(uint8_t& value) override { return false; }
    bool readBytes(uint8_t* buffer, size_t length) override { return false; }

    bool writeRegister(uint8_t reg, uint8_t value) override { return false; }
    bool readRegister(uint8_t reg, uint8_t& value) override { return false; }
    bool readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) override { return false; }
};

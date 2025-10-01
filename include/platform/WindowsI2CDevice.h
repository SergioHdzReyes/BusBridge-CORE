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

    bool writeByte(uint8_t, uint8_t) override { return false; }
    bool writeBytes(uint8_t, const std::vector<uint8_t>&) override { return false; }

    bool readByte(uint8_t, uint8_t&) override { return false; }
    bool readBytes(uint8_t, std::vector<uint8_t>&, size_t) override { return false; }
};

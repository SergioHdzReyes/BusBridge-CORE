#pragma once

#include "I2CInterface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>


class I2CDevice {
public:
    I2CDevice(const std::string& bus, uint8_t address);

    bool openBus() { return impl->openBus(); }
    void closeBus() { impl->closeBus(); }

    bool writeByte(uint8_t value) { return impl->writeByte(value); }
    bool writeBytes(const uint8_t* data, size_t length) { return impl->writeBytes(data, length); }

    bool readByte(uint8_t& value) { return impl->readByte(value); }
    bool readBytes(uint8_t* buffer, size_t length) { return impl->readBytes(buffer, length); }

    bool writeRegister(uint8_t reg, uint8_t value) { return impl->writeRegister(reg, value); }
    bool readRegister(uint8_t reg, uint8_t& value) { return impl->readRegister(reg, value); }
    bool readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) { return impl->readRegisterBlock(reg, buffer, length); }

private:
    std::unique_ptr<I2CInterface> impl;
};
#pragma once

#include "I2CInterface.h"

#include <memory>

namespace busbridge::i2c {

class I2CDevice {
public:
    I2CDevice(const std::string& bus, uint8_t address);
    I2CInterface& getImpl() { return *impl; }

    I2CError openBus() { return impl->openBus(); }
    void closeBus() { impl->closeBus(); }

    I2CError writeByte(uint8_t value) { return impl->writeByte(value); }
    I2CError writeBytes(const uint8_t* data, size_t length) { return impl->writeBytes(data, length); }

    I2CError readByte(uint8_t& value) { return impl->readByte(value); }
    I2CError readBytes(uint8_t* buffer, size_t length) { return impl->readBytes(buffer, length); }

    I2CError writeRegister(uint8_t reg, uint8_t value) { return impl->writeRegister(reg, value); }
    I2CError readRegister(uint8_t reg, uint8_t& value) { return impl->readRegister(reg, value); }
    I2CError readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) { return impl->readRegisterBlock(reg, buffer, length); }

private:
    std::unique_ptr<I2CInterface> impl;
};

}

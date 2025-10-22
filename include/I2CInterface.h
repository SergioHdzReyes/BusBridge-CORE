#pragma once

#include "I2CError.h"

#include <cstdint>
#include <cstdlib>

class I2CInterface {
public:
    virtual ~I2CInterface() = default;

    virtual I2CError openBus() = 0;
    virtual void closeBus() = 0;

    virtual I2CError writeByte(uint8_t value) = 0;
    virtual I2CError writeBytes(const uint8_t* data, size_t length) = 0;

    virtual I2CError readByte(uint8_t& value) = 0;
    virtual I2CError readBytes(uint8_t* buffer, size_t length) = 0;

    virtual I2CError writeRegister(uint8_t reg, uint8_t value) = 0;
    virtual I2CError readRegister(uint8_t reg, uint8_t& value) = 0;
    virtual I2CError readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) = 0;
};

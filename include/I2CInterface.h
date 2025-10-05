#pragma once

#include <string>
#include <vector>
#include <cstdint>

class I2CInterface {
public:
    virtual ~I2CInterface() = default;

    virtual bool openBus() = 0;
    virtual void closeBus() = 0;

    virtual bool writeByte(uint8_t value) = 0;
    virtual bool writeBytes(const uint8_t* data, size_t length) = 0;

    virtual bool readByte(uint8_t& value) = 0;
    virtual bool readBytes(uint8_t* buffer, size_t length) = 0;

    virtual bool writeRegister(uint8_t reg, uint8_t value) = 0;
    virtual bool readRegister(uint8_t reg, uint8_t& value) = 0;
    virtual bool readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) = 0;
};

#pragma once
#include <string>
#include <vector>
#include <cstdint>

class I2CInterface {
public:
    virtual ~I2CInterface() = default;

    virtual bool openBus() = 0;
    virtual void closeBus() = 0;

    virtual bool writeByte(uint8_t reg, uint8_t data) = 0;
    virtual bool writeBytes(uint8_t reg, const std::vector<uint8_t>& data) = 0;

    virtual bool readByte(uint8_t reg, uint8_t& data) = 0;
    virtual bool readBytes(uint8_t reg, std::vector<uint8_t>& data, size_t length) = 0;
};

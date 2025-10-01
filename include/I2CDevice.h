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

    bool writeByte(uint8_t reg, uint8_t data) { return impl->writeByte(reg, data); }
    bool writeBytes(uint8_t reg, const std::vector<uint8_t>& data) { return impl->writeBytes(reg, data); }

    bool readByte(uint8_t reg, uint8_t& data) { return impl->readByte(reg, data); }
    bool readBytes(uint8_t reg, std::vector<uint8_t>& data, size_t length) { return impl->readBytes(reg, data, length); }

private:
    std::unique_ptr<I2CInterface> impl;
};
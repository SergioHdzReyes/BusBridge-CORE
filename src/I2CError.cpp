/**
 * @file I2CError.cpp
 * @brief Implementation of I2CError class
 */

#include "I2CError.h"

namespace busbridge::i2c {

I2CError::I2CError() noexcept
    : m_code(ErrorCode::Success), m_message(describeErrorCode(ErrorCode::Success)) {}

I2CError::I2CError(ErrorCode code, std::string msg)
    : m_code(code), m_message(msg.empty() ? describeErrorCode(code) : std::move(msg)) {}

bool I2CError::ok() const noexcept {
    return m_code == ErrorCode::Success;
}

int I2CError::getCode() const noexcept {
    return static_cast<int>(m_code);
}

std::string I2CError::getMessage() const {
    return m_message;
}

bool I2CError::operator==(ErrorCode code) const noexcept {
    return m_code == code;
}

bool I2CError::operator!=(ErrorCode code) const noexcept {
    return m_code != code;
}

std::string I2CError::describeErrorCode(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success:
            return "Operation successful";
        case ErrorCode::BusOpenFailed:
            return "Failed to open I2C bus";
        case ErrorCode::ReadFailed:
            return "Failed to read from I2C device";
        case ErrorCode::WriteFailed:
            return "Failed to write to I2C device";
        case ErrorCode::Timeout:
            return "I2C operation timed out";
        case ErrorCode::InvalidParameter:
            return "Invalid parameter provided";
        case ErrorCode::UnsupportedOperation:
            return "Operation not supported by device/implementation";
        case ErrorCode::DeviceNotFound:
            return "I2C device not found on bus";
        default:
            return "Unknown I2C error";
    }
}

}  // namespace busbridge::i2c

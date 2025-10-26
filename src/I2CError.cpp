#include "I2CError.h"

namespace busbridge::i2c {

I2CError::I2CError() noexcept
        : m_code(ErrorCode::Success), m_message(describeErrorCode(ErrorCode::Success)) {}

I2CError::I2CError(ErrorCode code, std::string msg)
        : m_code(code),
          m_message(msg.empty() ? describeErrorCode(code) : std::move(msg)) {}

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
        case ErrorCode::Success: return "No error";
        case ErrorCode::BusOpenFailed: return "I2C: Failed to open bus";
        case ErrorCode::ReadFailed: return "I2C: Failed to read from device";
        case ErrorCode::WriteFailed: return "I2C: Failed to write to device";
        case ErrorCode::Timeout: return "I2C: Operation timed out";
        case ErrorCode::InvalidParameter: return "I2C: Invalid parameter";
        case ErrorCode::DeviceNotFound: return "I2C: Device not found";
        default: return "I2C: Unknown error";
    }
}

}

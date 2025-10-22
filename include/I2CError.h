#pragma once

#include "IProtocolError.h"
#include <string>

enum class ErrorCode {
    Success = 0,
    BusOpenFailed,
    ReadFailed,
    WriteFailed,
    Timeout,
    InvalidParameter,
    UnsupportedOperation,
    DeviceNotFound
};

class I2CError : public IProtocolError {
public:
    I2CError() noexcept;
    explicit I2CError(ErrorCode code, std::string msg = "");

    bool ok() const noexcept override;
    int getCode() const noexcept override;
    std::string getMessage() const override;
    const char* getProtocolName() const noexcept override { return "I2C"; }

    bool operator==(ErrorCode code) const noexcept;
    bool operator!=(ErrorCode code) const noexcept;

private:
    ErrorCode m_code;
    std::string m_message;

    static std::string describeErrorCode(ErrorCode code);
};

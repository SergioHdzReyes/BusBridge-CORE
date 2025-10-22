#pragma once

#include <string>

class IProtocolError {
public:
    virtual ~IProtocolError() = default;

    virtual bool ok() const noexcept = 0;
    virtual int getCode() const noexcept = 0;
    virtual std::string getMessage() const = 0;
    virtual const char* getProtocolName() const noexcept = 0;
};

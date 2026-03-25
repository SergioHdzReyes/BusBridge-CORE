#pragma once

#include "IProtocolError.h"
#include <string>

namespace busbridge::i2c {

/**
 * @enum ErrorCode
 * @brief I2C bus error codes
 * @details Represents all possible error conditions in I2C operations
 */
enum class ErrorCode {
    /// @brief Operation completed successfully
    Success = 0,

    /// @brief Failed to open I2C bus
    BusOpenFailed,

    /// @brief Failed to read from I2C device
    ReadFailed,

    /// @brief Failed to write to I2C device
    WriteFailed,

    /// @brief Operation timed out
    Timeout,

    /// @brief Invalid parameter provided
    InvalidParameter,

    /// @brief Operation not supported by device/implementation
    UnsupportedOperation,

    /// @brief I2C device not found on bus
    DeviceNotFound
};

/**
 * @class I2CError
 * @brief Encapsulates I2C operation error information
 *
 * @details
 * Provides a type-safe, non-throwing error handling mechanism for I2C operations.
 * Contains both error code and descriptive message for logging and debugging.
 * Implements the IProtocolError interface for consistency across protocols.
 *
 * @note Follows SOLID principles: immutable after construction
 * @see ErrorCode for complete list of error conditions
 */
class I2CError : public IProtocolError {
public:
    /**
     * @brief Constructor with default success state
     *
     * @details
     * Creates an I2CError representing successful operation.
     */
    I2CError() noexcept;

    /**
     * @brief Constructor with error code
     *
     * @param code The error code to assign
     * @param msg Optional custom error message
     *
     * @details
     * Creates an I2CError with the specified error code and optional message.
     * If no message is provided, a default message for the error code is used.
     */
    explicit I2CError(ErrorCode code, std::string msg = "");

    /**
     * @brief Check if operation was successful
     *
     * @return true if error code is Success, false otherwise
     */
    bool ok() const noexcept override;

    /**
     * @brief Get error code as integer
     *
     * @return The error code as integer value
     */
    int getCode() const noexcept override;

    /**
     * @brief Get human-readable error message
     *
     * @return Descriptive error message string
     *
     * @details
     * Returns either a custom message (if provided during construction)
     * or a default message based on the error code.
     */
    std::string getMessage() const override;

    /**
     * @brief Get protocol name
     *
     * @return String "I2C" identifying the protocol
     */
    const char* getProtocolName() const noexcept override { return "I2C"; }

    /**
     * @brief Equality comparison with error code
     *
     * @param code Error code to compare against
     *
     * @return true if this error matches the given code
     */
    bool operator==(ErrorCode code) const noexcept;

    /**
     * @brief Inequality comparison with error code
     *
     * @param code Error code to compare against
     *
     * @return true if this error does not match the given code
     */
    bool operator!=(ErrorCode code) const noexcept;

    /**
     * @brief Implicit conversion to bool (for idiomatic error checking)
     *
     * @return true if error occurred (opposite of ok())
     *
     * @note Allows syntax like: `if (i2cError) { handle_error(); }`
     */
    explicit operator bool() const noexcept { return !ok(); }

    /**
     * @brief Explicit conversion to error code
     *
     * @return The underlying ErrorCode value
     */
    explicit operator ErrorCode() const noexcept { return m_code; }

private:
    /// @brief Error code
    ErrorCode m_code;

    /// @brief Descriptive error message
    std::string m_message;

    /**
     * @brief Get default message for error code
     *
     * @param code The error code
     *
     * @return Default human-readable message
     */
    static std::string describeErrorCode(ErrorCode code);
};

}  // namespace busbridge::i2c

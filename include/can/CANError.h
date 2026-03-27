#pragma once

#include <string>
#include <cstdint>

namespace busbridge::can {

/**
 * @enum ErrorCode
 * @brief CAN bus error codes
 * @details Represents all possible error conditions in CAN operations
 */
enum class ErrorCode : uint8_t {
    /// @brief Operation completed successfully
    Success = 0,

    /// @brief Failed to open CAN interface
    FailedToOpen = 1,

    /// @brief Failed to close CAN interface
    FailedToClose = 2,

    /// @brief Failed to send message
    FailedToSend = 3,

    /// @brief Failed to receive message
    FailedToReceive = 4,

    /// @brief Message data length exceeds maximum
    InvalidDataLength = 5,

    /// @brief CAN identifier is invalid
    InvalidCANID = 6,

    /// @brief Interface not initialized or opened
    NotInitialized = 7,

    /// @brief Bus-off error (recovery needed)
    BusOff = 8,

    /// @brief Bus error detected by controller
    BusError = 9,

    /// @brief Timeout waiting for operation
    Timeout = 10,

    /// @brief Permission denied (e.g., insufficient privileges)
    PermissionDenied = 11,

    /// @brief Device/interface not found
    DeviceNotFound = 12,

    /// @brief Invalid parameters provided
    InvalidParameters = 13,

    /// @brief Memory allocation failed
    MemoryError = 14,

    /// @brief Invalid argument provided
    InvalidArgument = 15,

    /// @brief Unknown or generic error
    UnknownError = 255
};

/**
 * @class CANError
 * @brief Encapsulates CAN operation error information
 * 
 * @details
 * Provides a type-safe, non-throwing error handling mechanism for CAN operations.
 * Contains both error code and descriptive message for logging and debugging.
 * 
 * @note Follows SOLID principles: immutable after construction
 * @see ErrorCode for complete list of error conditions
 */
class CANError {
public:
    /**
     * @brief Constructor with error code
     * @param code The error code to assign
     */
    explicit CANError(ErrorCode code = ErrorCode::Success) noexcept;

    /**
     * @brief Constructor with error code and custom message
     * @param code The error code to assign
     * @param message Custom error message
     */
    CANError(ErrorCode code, const std::string& message) noexcept;

    /**
     * @brief Check if operation was successful
     * @return true if error code is Success, false otherwise
     */
    bool ok() const noexcept { return m_code == ErrorCode::Success; }

    /**
     * @brief Get error code
     * @return The ErrorCode value
     */
    ErrorCode getCode() const noexcept { return m_code; }

    /**
     * @brief Get human-readable error message
     * @return Descriptive error message string
     * @details
     * Returns either a default message (based on error code) or a custom
     * message if provided during construction
     */
    const std::string& getMessage() const noexcept { return m_message; }

    /**
     * @brief Implicit conversion to bool (for idiomatic error checking)
     * @return true if error occurred (opposite of ok())
     * @note Allows syntax like: `if (canError) { handle_error(); }`
     */
    explicit operator bool() const noexcept { return !ok(); }

    /**
     * @brief Explicit conversion to error code
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
     * @param code The error code
     * @return Default human-readable message
     */
    static std::string describeErrorCode(ErrorCode code) noexcept;
};

}  // namespace busbridge::can

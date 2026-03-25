/**
 * @file CANError.cpp
 * @brief Implementation of CANError class
 */

#include "can/CANError.h"

namespace busbridge::can {

CANError::CANError(ErrorCode code) noexcept
    : m_code(code), m_message(describeErrorCode(code)) {}

CANError::CANError(ErrorCode code, const std::string& message) noexcept
    : m_code(code), m_message(message) {}

std::string CANError::describeErrorCode(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Success:
            return "Operation successful";
        case ErrorCode::FailedToOpen:
            return "Failed to open CAN interface";
        case ErrorCode::FailedToClose:
            return "Failed to close CAN interface";
        case ErrorCode::FailedToSend:
            return "Failed to send CAN message";
        case ErrorCode::FailedToReceive:
            return "Failed to receive CAN message";
        case ErrorCode::InvalidDataLength:
            return "Message data length exceeds maximum (8 bytes)";
        case ErrorCode::InvalidCANID:
            return "Invalid CAN identifier";
        case ErrorCode::NotInitialized:
            return "CAN interface not initialized or opened";
        case ErrorCode::BusOff:
            return "CAN bus is in off state (recovery needed)";
        case ErrorCode::BusError:
            return "CAN bus error detected";
        case ErrorCode::Timeout:
            return "Operation timed out";
        case ErrorCode::PermissionDenied:
            return "Permission denied (insufficient privileges)";
        case ErrorCode::DeviceNotFound:
            return "CAN device/interface not found";
        case ErrorCode::InvalidParameters:
            return "Invalid parameters provided";
        case ErrorCode::MemoryError:
            return "Memory allocation failed";
        case ErrorCode::UnknownError:
        default:
            return "Unknown error";
    }
}

}  // namespace busbridge::can

#pragma once

#include "can/CANMessage.h"
#include "can/CANError.h"

#include <string>
#include <functional>

namespace busbridge::can {

/**
 * @class ICANInterface
 * @brief Abstract interface for CAN bus communication
 * 
 * @details
 * Defines the contract for all CAN interface implementations (SocketCAN, PEAK, etc.).
 * Implementations should handle platform-specific details while maintaining
 * this common interface.
 * 
 * @par Design Pattern
 * Strategy pattern for pluggable CAN implementations.
 * 
 * @par Thread Safety
 * - openChannel(): Non-blocking, should be called once before other operations
 * - closeChannel(): Blocks until channel is closed
 * - receiveMessage(): Blocking operation
 * - sendMessage(): Non-blocking (writes to kernel buffer)
 * 
 * @note Based on SOLID principles - Single Responsibility, Open/Closed, Liskov Substitution
 * @see CANDevice for factory implementation
 */
class ICANInterface {
public:
    /// @brief Virtual destructor for proper cleanup of derived classes
    virtual ~ICANInterface() = default;

    /**
     * @brief Open CAN interface/channel
     * 
     * @param interface_name Name or path of the interface
     *        - Linux SocketCAN: "can0", "can1", etc.
     *        - Windows PEAK: "\\\\.\\PCAN_USBBUS1", etc.
     * 
     * @return CANError object indicating success or failure
     * 
     * @details
     * Initializes the CAN interface for communication. Must be called before
     * any other operations (send/receive). Performs all necessary setup:
     * - Resolving interface by name
     * - Creating communication socket/handle
     * - Setting up buffer structures
     * 
     * @note idempotent: calling multiple times should be safe (closes previous if needed)
     * 
     * @see closeChannel()
     */
    virtual CANError openChannel(const std::string& interface_name) = 0;

    /**
     * @brief Close CAN interface/channel
     * 
     * @details
     * Gracefully closes the CAN interface and releases all resources.
     * After calling this, the interface must be reopened before further operations.
     * 
     * @note idempotent: safe to call multiple times
     * @see openChannel()
     */
    virtual void closeChannel() = 0;

    /**
     * @brief Send a CAN message
     * 
     * @param message The CANMessage to transmit
     * 
     * @return CANError indicating success or reason for failure
     * 
     * @details
     * Transmits a CAN message on the open interface. Returns immediately after
     * writing to kernel buffer; actual transmission may be asynchronous.
     * 
     * @pre Interface must be opened via openChannel()
     * @post Message is queued for transmission
     * 
     * @throw Does not throw; always returns CANError object
     * 
     * @see receiveMessage()
     */
    virtual CANError sendMessage(const CANMessage& message) = 0;

    /**
     * @brief Receive a CAN message
     * 
     * @param[out] message Reference to CANMessage to populate with received data
     * 
     * @return CANError indicating success or reason for failure
     * 
     * @details
     * Blocking call that waits for a CAN message on the interface.
     * Populates the provided @c message reference with received frame data including
     * ID, payload, DLC, timestamp, and frame type flags.
     * 
     * @pre Interface must be opened via openChannel()
     * @post @c message parameter contains valid data if CANError::ok() is true
     * 
     * @throw Does not throw; always returns CANError object
     * 
     * @note May timeout based on implementation; check returned error code
     * @see sendMessage()
     */
    virtual CANError receiveMessage(CANMessage& message) = 0;

    /**
     * @brief Check if interface is currently open and operational
     * 
     * @return true if interface is open and ready, false otherwise
     * 
     * @details
     * Lightweight check without side effects. Useful for condition checks
     * before performing operations.
     */
    virtual bool isOpen() const = 0;

    /**
     * @brief Get descriptive name/identifier of the interface
     * 
     * @return String identifying the interface (e.g., "can0", "SocketCAN")
     * 
     * @details
     * Returns a human-readable identifier for the interface, useful for
     * logging and debugging. Format is implementation-specific.
     */
    virtual std::string getInterfaceName() const = 0;
};

}  // namespace busbridge::can

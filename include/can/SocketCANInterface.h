#pragma once

#include "can/ICANInterface.h"

#include <string>

namespace busbridge::can {

/**
 * @class SocketCANInterface
 * @brief Linux SocketCAN implementation for CAN communication
 * 
 * @details
 * Implements ICANInterface using Linux kernel SocketCAN subsystem.
 * Compatible with:
 * - candlelight firmware (USB CAN adapter)
 * - PEAK CAN interfaces with SocketCAN drivers
 * - Kvaser CAN interfaces with SocketCAN drivers
 * - Native interfaces with SocketCAN support (can0, can1, etc.)
 * 
 * Uses raw CAN sockets (AF_CAN, SOCK_RAW, CAN_RAW) for direct access to
 * the CAN bus without requiring serial communication or proprietary protocols.
 * 
 * @par Features
 * - Non-blocking socket operations (configurable timeout)
 * - Support for standard (11-bit) and extended (29-bit) CAN IDs
 * - Full CAN frame data including timestamps (kernel precision)
 * - Error frame detection and reporting
 * 
 * @par Linux Requirements
 * - Kernel with SocketCAN support (CONFIG_CAN_*=y)
 * - Headers: <linux/can.h>, <sys/socket.h>
 * - Device presentation as network interface (ip link show)
 * 
 * @par Platform Support
 * - Linux only (uses Linux-specific socket APIs)
 * - Requires C++17 for std::optional and other features
 * 
 * @note Thread-safe: receiveMessage() and sendMessage() can be called from
 *       different threads once the socket is open. openChannel()/closeChannel()
 *       should synchronize with other operations.
 * 
 * @see https://www.kernel.org/doc/html/latest/networking/can.html
 */
class SocketCANInterface : public ICANInterface {
public:
    /**
     * @brief Default receive timeout in milliseconds
     * @details Prevents blocking indefinitely on receiveMessage()
     */
    static constexpr int DEFAULT_RECEIVE_TIMEOUT_MS = 1000;

    /**
     * @brief Constructor
     * 
     * @details
     * Initializes SocketCANInterface in closed state. Call openChannel()
     * to establish connection to the CAN interface.
     */
    SocketCANInterface();

    /**
     * @brief Destructor
     * 
     * @details
     * Ensures socket is properly closed and resources released.
     * Calls closeChannel() if interface is still open.
     */
    ~SocketCANInterface() override;

    /**
     * @brief Open a CAN interface via SocketCAN
     * 
     * @param interface_name Name of the CAN interface
     *        Examples: "can0", "can1", "vcan0" (virtual)
     * 
     * @return CANError::Success if opened successfully
     *         CANError::DeviceNotFound if interface doesn't exist
     *         CANError::FailedToOpen if socket creation fails
     *         CANError::PermissionDenied if insufficient permissions
     * 
     * @details
     * Performs the following steps:
     * 1. Resolves interface name to network interface index (getifaddrs)
     * 2. Creates raw CAN socket (AF_CAN, SOCK_RAW, CAN_RAW)
     * 3. Binds socket to the interface
     * 4. Sets socket to non-blocking mode with timeout
     * 5. Enables timestamp reception from kernel
     * 
     * @pre Interface must exist and be up (ip link set can0 up)
     * @post Socket ready for send/receive operations
     * 
     * @note idempotent: closing previous socket if needed
     */
    CANError openChannel(const std::string& interface_name) override;

    /**
     * @brief Close the CAN interface
     * 
     * @details
     * Closes the socket and releases all resources. Safe to call even
     * if interface is already closed.
     * 
     * @post Socket closed, file descriptor invalidated
     */
    void closeChannel() override;

    /**
     * @brief Send a CAN message
     * 
     * @param message The message to transmit
     * 
     * @return CANError::Success if message queued for transmission
     *         CANError::NotInitialized if socket not open
     *         CANError::FailedToSend if kernel write fails
     *         CANError::InvalidDataLength if message.dlc > 8
     * 
     * @details
     * Converts CANMessage to kernel struct can_frame and writes to socket.
     * Returns immediately after queueing; actual transmission is async.
     * 
     * @pre openChannel() must have been called and returned successfully
     * @post Message queued in kernel buffer for transmission
     * 
     * @note Non-blocking: returns immediately
     * @note Thread-safe once opened
     */
    CANError sendMessage(const CANMessage& message) override;

    /**
     * @brief Receive a CAN message
     * 
     * @param[out] message Populated with received frame data
     * 
     * @return CANError::Success if message received
     *         CANError::NotInitialized if socket not open
     *         CANError::FailedToReceive if socket read fails
     *         CANError::Timeout if no data within timeout period
     * 
     * @details
     * Reads from socket with timeout (DEFAULT_RECEIVE_TIMEOUT_MS).
     * Converts kernel struct can_frame to CANMessage including:
     * - Standard/extended ID detection
     * - Error frame detection
     * - Remote frame detection
     * - Kernel timestamp capture
     * - DLC extraction and validation
     * 
     * @pre openChannel() must have been called
     * @post @c message reference populated with frame data
     * 
     * @note Blocking with timeout: may return Timeout error
     * @note Thread-safe once opened
     */
    CANError receiveMessage(CANMessage& message) override;

    /**
     * @brief Check if interface is currently open
     * 
     * @return true if socket is open and valid, false otherwise
     */
    bool isOpen() const override { return m_socketFd >= 0; }

    /**
     * @brief Get the name of this interface
     * 
     * @return String like "SocketCAN (can0)" describing the interface
     */
    std::string getInterfaceName() const override;

private:
    /// @brief Socket file descriptor (-1 if not open)
    int m_socketFd;

    /// @brief Last opened interface name (for error messages)
    std::string m_currentInterfaceName;

    /**
     * @brief Get network interface index by name
     * 
     * @param interface_name Name like "can0"
     * 
     * @return Interface index (>0) on success
     *         -1 if not found or error
     * 
     * @details
     * Uses getifaddrs() to enumerate network interfaces and find
     * the matching interface by name.
     */
    static int getInterfaceIndex_(const std::string& interface_name);

    /**
     * @brief Convert kernel can_frame timestamp to microseconds
     * 
     * @param tv_sec Seconds component
     * @param tv_usec Microseconds component
     * 
     * @return Timestamp as uint64_t microseconds since epoch
     */
    static uint64_t timestampToMicroseconds_(time_t tv_sec, long tv_usec);
};

}  // namespace busbridge::can

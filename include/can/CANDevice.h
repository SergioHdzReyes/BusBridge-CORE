#pragma once

#include "ICANInterface.h"

#include <memory>
#include <string>

namespace busbridge::can {

/**
 * @class CANDevice
 * @brief Factory class for creating platform-specific CAN interface implementations
 * 
 * @details
 * Provides abstraction over platform-specific CAN implementations. On Linux with
 * SocketCAN (e.g., candlelight), creates SocketCANInterface. Can be extended for
 * other platforms (Windows with PEAK PCAN API, etc.).
 * 
 * Follows the Factory and Strategy design patterns, delegating actual operations
 * to the appropriate implementation while maintaining a consistent interface.
 * 
 * @par Usage
 * @code
 * CANDevice device("can0");
 * auto result = device.openBus();
 * if (!result.ok()) {
 *     std::cerr << "Error: " << result.getMessage() << std::endl;
 *     return result.getCode();
 * }
 * 
 * CANMessage msg;
 * device.receiveMessage(msg);
 * @endcode
 * 
 * @par Design
 * - Constructor argument specifies interface name (e.g., "can0")
 * - Internally creates appropriate ICANInterface implementation via unique_ptr
 * - Delegates all operations to underlying implementation
 * - Provides convenient wrapper methods that match ICANInterface
 * 
 * @note Thread-safe if underlying implementation is thread-safe
 * @see ICANInterface for operation details
 */
class CANDevice {
public:
    /**
     * @brief Constructor
     * 
     * @param interface Interface name or identifier
     *        - Linux: "can0", "can1", etc. (SocketCAN interface names)
     *        - Windows: "\\\\.\\PCAN_USBBUS1" or similar (PEAK identifier)
     * 
     * @details
     * Creates a CANDevice and automatically selects the appropriate
     * implementation based on the platform and interface name.
     * The actual implementation instance is created here via factory logic.
     * 
     * @note Does NOT open the interface; call openBus() for that
     * @see openBus()
     */
    explicit CANDevice(const std::string& interface);

    /// @brief Destructor - ensures proper cleanup of implementation
    ~CANDevice() = default;

    /**
     * @brief Get reference to underlying implementation
     * 
     * @return Reference to ICANInterface implementation
     * 
     * @details
     * Provides direct access to the underlying interface implementation.
     * Useful for advanced operations or when implementation-specific
     * features are needed.
     * 
     * @pre Underlying implementation pointer must be valid
     * @throw May throw if not properly initialized (implementation-dependent)
     */
    ICANInterface& getImpl() { return *m_impl; }

    /**
     * @brief Get const reference to underlying implementation
     * 
     * @return Const reference to ICANInterface implementation
     * 
     * @see getImpl()
     */
    const ICANInterface& getImpl() const { return *m_impl; }

    /**
     * @brief Open the CAN interface
     * 
     * @return CANError indicating success or failure
     * 
     * @details
     * Wrapper that delegates to underlying implementation's openChannel().
     * 
     * @see ICANInterface::openChannel()
     */
    CANError openBus() { 
        return m_impl->openChannel(m_interfaceName); 
    }

    /**
     * @brief Close the CAN interface
     * 
     * @details
     * Wrapper that delegates to underlying implementation's closeChannel().
     * 
     * @see ICANInterface::closeChannel()
     */
    void closeBus() { m_impl->closeChannel(); }

    /**
     * @brief Send a CAN message
     * 
     * @param message The message to transmit
     * 
     * @return CANError indicating success or failure
     * 
     * @details
     * Wrapper that delegates to underlying implementation's sendMessage().
     * 
     * @see ICANInterface::sendMessage()
     */
    CANError sendMessage(const CANMessage& message) { 
        return m_impl->sendMessage(message); 
    }

    /**
     * @brief Receive a CAN message
     * 
     * @param[out] message Reference to populate with received data
     * 
     * @return CANError indicating success or failure
     * 
     * @details
     * Wrapper that delegates to underlying implementation's receiveMessage().
     * 
     * @see ICANInterface::receiveMessage()
     */
    CANError receiveMessage(CANMessage& message) { 
        return m_impl->receiveMessage(message); 
    }

    /**
     * @brief Check if interface is open
     * 
     * @return true if operational, false otherwise
     * 
     * @see ICANInterface::isOpen()
     */
    bool isOpen() const { return m_impl->isOpen(); }

    /**
     * @brief Get interface name
     * 
     * @return Interface identifier string
     * 
     * @see ICANInterface::getInterfaceName()
     */
    std::string getInterfaceName() const { 
        return m_impl->getInterfaceName(); 
    }

private:
    /// @brief Interface name (for reference and error messages)
    std::string m_interfaceName;

    /// @brief Pointer to platform-specific implementation
    std::unique_ptr<ICANInterface> m_impl;
};

}  // namespace busbridge::can

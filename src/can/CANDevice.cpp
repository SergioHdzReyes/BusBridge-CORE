/**
 * @file CANDevice.cpp
 * @brief Implementation of CANDevice factory class
 */

#include "can/CANDevice.h"

// Platform-specific includes
#if defined(_WIN32)
    // Windows PEAK PCAN API (future implementation)
    // #include "platform/WindowsCANInterface.h"
#else
    // Linux SocketCAN implementation
    #include "can/SocketCANInterface.h"
#endif

namespace busbridge::can {

/**
 * @brief Factory constructor that creates platform-specific CAN implementation
 * 
 * Selects the appropriate ICANInterface implementation based on the platform
 * at compile time. On Linux, uses SocketCAN (compatible with candlelight).
 * On Windows, would use PEAK PCAN API (to be implemented).
 */
CANDevice::CANDevice(const std::string& interface)
    : m_interfaceName(interface)
{
#if defined(_WIN32)
    // Windows: Use PEAK PCAN API implementation (future)
    // m_impl = std::make_unique<WindowsCANInterface>(interface);
    // For now, this will fail at runtime on Windows
    m_impl = nullptr;
#else
    // Linux: Use SocketCAN implementation (works with candlelight, PEAK, etc.)
    m_impl = std::make_unique<SocketCANInterface>();
#endif
}

}  // namespace busbridge::can

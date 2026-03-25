/**
 * @file SocketCANInterface.cpp
 * @brief Linux SocketCAN implementation
 */

#include "can/SocketCANInterface.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>
#include <iostream>

namespace busbridge::can {

SocketCANInterface::SocketCANInterface()
    : m_socketFd(-1), m_currentInterfaceName("")
{
}

SocketCANInterface::~SocketCANInterface()
{
    if (isOpen()) {
        closeChannel();
    }
}

int SocketCANInterface::getInterfaceIndex_(const std::string& interface_name)
{
    // Use if_nametoindex directly - simpler and more reliable
    unsigned int interface_index = if_nametoindex(interface_name.c_str());
    
    if (interface_index == 0) {
        std::cerr << "Interface '" << interface_name << "' not found. "
                  << "Make sure it exists and is brought up with: "
                  << "sudo ip link set " << interface_name << " type can bitrate 500000 up"
                  << std::endl;
        return -1;
    }

    return static_cast<int>(interface_index);
}

uint64_t SocketCANInterface::timestampToMicroseconds_(time_t tv_sec, long tv_usec)
{
    return (static_cast<uint64_t>(tv_sec) * 1000000) + static_cast<uint64_t>(tv_usec);
}

CANError SocketCANInterface::openChannel(const std::string& interface_name)
{
    // Close any previously open socket
    if (isOpen()) {
        closeChannel();
    }

    m_currentInterfaceName = interface_name;

    // Get network interface index
    int if_index = getInterfaceIndex_(interface_name);
    if (if_index < 0) {
        return CANError(ErrorCode::DeviceNotFound,
                       "CAN interface '" + interface_name + "' not found");
    }

    // Create raw CAN socket
    m_socketFd = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socketFd < 0) {
        int err = errno;
        std::cerr << "socket() failed: " << strerror(err) << std::endl;
        m_socketFd = -1;

        // Check for permission issues
        if (err == EPERM) {
            std::cerr << "\n⚠️  Permission denied! Try running with sudo:\n"
                      << "   sudo ./read_can_sniffer\n" << std::endl;
            return CANError(ErrorCode::PermissionDenied,
                           "Insufficient permissions to open CAN socket (run as root)");
        }
        if (err == EAFNOSUPPORT) {
            std::cerr << "\n⚠️  CAN support not available in kernel!\n"
                      << "   Make sure CONFIG_CAN_RAW is enabled.\n" << std::endl;
            return CANError(ErrorCode::FailedToOpen,
                           "CAN support not available (check kernel config)");
        }
        return CANError(ErrorCode::FailedToOpen,
                       std::string("Failed to create CAN socket: ") + strerror(err));
    }

    // Bind socket to interface
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_index;

    if (bind(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        int err = errno;
        std::cerr << "bind() failed: " << strerror(err) << std::endl;
        close(m_socketFd);
        m_socketFd = -1;

        if (err == ENODEV) {
            std::cerr << "\n⚠️  Interface '" << interface_name << "' not found or not UP!\n"
                      << "   Bring it up with: sudo ip link set " << interface_name 
                      << " type can bitrate 500000 up\n" << std::endl;
            return CANError(ErrorCode::DeviceNotFound,
                           "Interface not found or not UP");
        }
        if (err == EBUSY) {
            std::cerr << "\n⚠️  Interface '" << interface_name << "' is busy or already in use!\n" << std::endl;
            return CANError(ErrorCode::FailedToOpen,
                           "Interface is busy");
        }
        return CANError(ErrorCode::FailedToOpen,
                       std::string("Failed to bind to CAN interface: ") + strerror(err));
    }

    // Note: SocketCAN doesn't support SO_RCVTIMEO like regular sockets
    // We'll use poll() in receiveMessage() instead for timeout handling
    
    // Enable kernel timestamp reception (optional)
    int enable_ts = 1;
    if (setsockopt(m_socketFd, SOL_SOCKET, SO_TIMESTAMP, &enable_ts, sizeof(enable_ts)) < 0) {
        // Non-critical: continue without kernel timestamps
        // We'll use system time as fallback
    }

    return CANError(ErrorCode::Success);
}

void SocketCANInterface::closeChannel()
{
    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
        m_currentInterfaceName = "";
    }
}

CANError SocketCANInterface::sendMessage(const CANMessage& message)
{
    if (!isOpen()) {
        return CANError(ErrorCode::NotInitialized,
                       "CAN interface not open");
    }

    if (message.dlc > MAX_CAN_DATA_SIZE) {
        return CANError(ErrorCode::InvalidDataLength,
                       "Message DLC exceeds maximum (8 bytes)");
    }

    // Convert to kernel can_frame structure
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));

    // Set CAN ID (with extended flag if needed)
    frame.can_id = message.id;
    if (message.is_extended) {
        frame.can_id |= CAN_EFF_FLAG;  // Extended frame flag
    }

    // Set frame flags
    if (message.is_rtr) {
        frame.can_id |= CAN_RTR_FLAG;
    }

    // Copy data and set DLC
    frame.can_dlc = message.dlc;
    memcpy(frame.data, message.data.data(), message.dlc);

    // Write to socket
    ssize_t bytes_written = write(m_socketFd, &frame, sizeof(frame));
    if (bytes_written < 0) {
        std::cerr << "write() failed: " << strerror(errno) << std::endl;
        return CANError(ErrorCode::FailedToSend,
                       "Failed to send CAN message");
    }

    return CANError(ErrorCode::Success);
}

CANError SocketCANInterface::receiveMessage(CANMessage& message)
{
    if (!isOpen()) {
        return CANError(ErrorCode::NotInitialized,
                       "CAN interface not open");
    }

    // Use poll() to handle timeout (SocketCAN doesn't support SO_RCVTIMEO)
    struct pollfd pfd;
    pfd.fd = m_socketFd;
    pfd.events = POLLIN;

    int poll_result = poll(&pfd, 1, DEFAULT_RECEIVE_TIMEOUT_MS);

    if (poll_result < 0) {
        // Error in poll
        int err = errno;
        std::cerr << "poll() failed: " << strerror(err) << std::endl;
        return CANError(ErrorCode::FailedToReceive,
                       std::string("poll() error: ") + strerror(err));
    }

    if (poll_result == 0) {
        // Timeout - no data available
        return CANError(ErrorCode::Timeout,
                       "No CAN message received within timeout");
    }

    // poll_result > 0: data is available
    struct can_frame frame;
    struct timeval timestamp;
    memset(&frame, 0, sizeof(frame));
    memset(&timestamp, 0, sizeof(timestamp));

    // Read from socket
    ssize_t bytes_read = recvfrom(m_socketFd, &frame, sizeof(frame),
                                  0, nullptr, nullptr);

    if (bytes_read < 0) {
        int err = errno;
        if (err == EBADF || err == ENOTSOCK) {
            // Socket was closed
            m_socketFd = -1;
            return CANError(ErrorCode::NotInitialized,
                           "CAN socket was closed");
        }
        std::cerr << "recvfrom() failed: " << strerror(err) << std::endl;
        return CANError(ErrorCode::FailedToReceive,
                       std::string("recvfrom() error: ") + strerror(err));
    }

    if (bytes_read < static_cast<ssize_t>(sizeof(struct can_frame))) {
        return CANError(ErrorCode::FailedToReceive,
                       "Incomplete CAN frame received");
    }

    // Convert kernel frame to CANMessage
    message.dlc = frame.can_dlc;
    message.id = frame.can_id & (CAN_EFF_MASK | CAN_ERR_MASK);
    message.is_extended = (frame.can_id & CAN_EFF_FLAG) != 0;
    message.is_rtr = (frame.can_id & CAN_RTR_FLAG) != 0;
    message.is_error = (frame.can_id & CAN_ERR_FLAG) != 0;

    // Copy data
    for (uint8_t i = 0; i < message.dlc && i < MAX_CAN_DATA_SIZE; ++i) {
        message.data[i] = frame.data[i];
    }

    // Get system timestamp as fallback
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        message.timestamp = timestampToMicroseconds_(ts.tv_sec, ts.tv_nsec / 1000);
    } else {
        message.timestamp = 0;
    }

    return CANError(ErrorCode::Success);
}

std::string SocketCANInterface::getInterfaceName() const
{
    return "SocketCAN (" + m_currentInterfaceName + ")";
}

}  // namespace busbridge::can

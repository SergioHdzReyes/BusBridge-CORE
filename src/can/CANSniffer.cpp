/**
 * @file CANSniffer.cpp
 * @brief Implementation of CANSniffer class for CAN message capture
 */

#include "can/CANSniffer.h"

#include <iostream>
#include <chrono>

namespace busbridge::can {

CANSniffer::CANSniffer(ICANInterface& interface)
    : m_interface(interface), m_callback(nullptr)
{
}

CANSniffer::~CANSniffer()
{
    // Ensure sniffing is stopped when sniffer is destroyed
    if (m_is_sniffing.load()) {
        stopSniffing();
    }
}

CANError CANSniffer::startSniffing(MessageCallback callback)
{
    // Verify interface is open
    if (!m_interface.isOpen()) {
        return CANError(ErrorCode::NotInitialized, 
                       "CAN interface is not open");
    }

    // Check if already sniffing
    if (m_is_sniffing.load()) {
        return CANError(ErrorCode::Success, 
                       "Already sniffing");
    }

    // Set callback and start sniffing flag
    m_callback = callback;
    m_is_sniffing.store(true);
    m_messages_received.store(0);
    m_error_count.store(0);

    // Create and start sniffing thread
    m_sniffing_thread = std::make_unique<std::thread>(
        &CANSniffer::sniffingLoop_, this
    );

    return CANError(ErrorCode::Success);
}

void CANSniffer::stopSniffing()
{
    // Signal thread to stop
    m_is_sniffing.store(false);

    // Wait for thread to finish
    if (m_sniffing_thread && m_sniffing_thread->joinable()) {
        m_sniffing_thread->join();
        m_sniffing_thread.reset();
    }

    // Clear callback
    m_callback = nullptr;
}

void CANSniffer::sniffingLoop_()
{
    CANMessage message;

    while (m_is_sniffing.load()) {
        // Attempt to receive a message
        CANError result = m_interface.receiveMessage(message);

        if (result.ok()) {
            // Successfully received message
            m_messages_received++;

            // Invoke callback if set
            if (m_callback) {
                try {
                    m_callback(message);
                } catch (const std::exception& e) {
                    // Catch exceptions from callback to prevent thread crash
                    std::cerr << "Exception in CAN message callback: " 
                              << e.what() << std::endl;
                }
            }
        } else {
            // Increment error counter
            m_error_count++;

            // Log error (in verbose mode, or only critical errors)
            // Don't break on error - continue trying to receive
        }

        // Small yield to prevent busy-waiting if receive is non-blocking
        std::this_thread::yield();
    }
}

}  // namespace busbridge::can

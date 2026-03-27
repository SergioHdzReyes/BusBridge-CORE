#pragma once

#include "can/ICANInterface.h"
#include "can/CANMessage.h"
#include "can/CANError.h"

#include <functional>
#include <memory>
#include <atomic>
#include <thread>

namespace busbridge::can {

/**
 * @typedef MessageCallback
 * @brief Callback function type for received CAN messages
 * 
 * @param message Const reference to the received CANMessage
 * 
 * @details
 * Called asynchronously when a message is received during sniffing.
 * Keep callback implementation lightweight to avoid blocking message reception.
 * 
 * @note Callback is invoked from the sniffing thread context
 * @see CANSniffer::startSniffing()
 */
using MessageCallback = std::function<void(const CANMessage&)>;

/**
 * @class CANSniffer
 * @brief Captures and processes CAN bus traffic
 * 
 * @details
 * Monitors a CAN interface for incoming messages and dispatches them to a
 * callback handler. Runs in a separate thread to avoid blocking the main
 * application. Supports clean start/stop operations with resource management.
 * 
 * @par Features
 * - Non-blocking async message capture
 * - Custom callback handler support
 * - Thread-safe start/stop operations
 * - Graceful shutdown on destructor
 * 
 * @par Usage
 * @code
 * CANDevice device("can0");
 * device.openBus();
 * 
 * CANSniffer sniffer(device.getImpl());
 * 
 * auto callback = [](const CANMessage& msg) {
 *     std::cout << "ID: 0x" << std::hex << msg.id << std::endl;
 * };
 * 
 * sniffer.startSniffing(callback);
 * 
 * // ... your application runs ...
 * 
 * sniffer.stopSniffing();
 * device.closeBus();
 * @endcode
 * 
 * @par Thread Safety
 * - startSniffing(): Thread-safe; creates internal thread
 * - stopSniffing(): Thread-safe; waits for thread termination
 * - Callback: Invoked from internal sniffing thread; should be thread-safe
 * 
 * @note Requires C++17 for std::atomic<bool> and thread support
 * @see ICANInterface, CANMessage
 */
class CANSniffer {
public:
    /**
     * @brief Constructor
     * 
     * @param interface Reference to an open ICANInterface implementation
     * 
     * @details
     * Creates a sniffer bound to the provided interface. The interface must
     * remain valid for the lifetime of this sniffer. Does not start sniffing
     * automatically; call startSniffing() to begin capturing.
     * 
     * @pre @c interface must be initialized and opened
     * @note Does not take ownership; caller manages interface lifetime
     */
    explicit CANSniffer(ICANInterface& interface);

    /**
     * @brief Destructor
     * 
     * @details
     * Ensures sniffing is stopped and thread is joined before destruction.
     * Safe to call even if sniffing hasn't been started.
     */
    ~CANSniffer();

    /**
     * @brief Start capturing CAN messages
     * 
     * @param callback Function to invoke for each received message
     * 
     * @return CANError indicating success or failure
     * 
     * @details
     * Spawns an internal thread that continuously receives messages from the
     * CAN interface and invokes the callback for each one. The callback runs
     * in the sniffer thread context, so keep it non-blocking for performance.
     * 
     * Replaces any previously set callback. Idempotent if already sniffing
     * with this callback.
     * 
     * @pre Interface must be open (interface_->isOpen() == true)
     * @post Internal thread started, capturing messages asynchronously
     * 
     * @see stopSniffing(), MessageCallback
     */
    CANError startSniffing(MessageCallback callback);

    /**
     * @brief Stop capturing CAN messages
     * 
     * @details
     * Signals the sniffing thread to terminate and waits for it to finish.
     * Blocks until thread has fully stopped and all resources are cleaned up.
     * Safe to call multiple times.
     * 
     * @pre startSniffing() must have been called
     * @post Thread stopped, internal flag cleared, callback cleared
     * 
     * @see startSniffing()
     */
    void stopSniffing();

    /**
     * @brief Check if sniffer is actively capturing
     * 
     * @return true if sniffing thread is running, false otherwise
     * 
     * @details
     * Lightweight check without side effects. Reflects the current sniffing state.
     */
    bool isSniffing() const { return m_is_sniffing.load(); }

    /**
     * @brief Get count of successfully received and processed messages
     *
     * @return Number of messages processed since startSniffing() was called
     *
     * @details
     * Counter incremented for each successful receiveMessage() call.
     * Resets to zero when sniffing is restarted.
     *
     * Useful for monitoring traffic volume and detecting stalls.
     */
    uint64_t getMessagesReceived() const { 
        return m_messages_received.load(); 
    }

    /**
     * @brief Reset the message counter
     *
     * @details
     * Sets messages_received counter back to zero. Useful when starting
     * bulk operations and wanting to track only new traffic.
     */
    void resetMessageCounter() { m_messages_received.store(0); }

    /**
     * @brief Get count of receive errors encountered
     *
     * @return Number of failed receiveMessage() calls since startSniffing()
     *
     * @details
     * Counter incremented when receiveMessage() returns an error.
     * Resets to zero when sniffing is restarted.
     *
     * Useful for diagnosing CAN interface issues.
     */
    uint64_t getErrorCount() const { return m_error_count.load(); }

    /**
     * @brief Reset the error counter
     *
     * @details
     * Sets error_count counter back to zero.
     */
    void resetErrorCounter() { m_error_count.store(0); }

private:
    /// @brief Reference to the CAN interface
    ICANInterface& m_interface;

    /// @brief Current message callback function
    MessageCallback m_callback;

    /// @brief Flag indicating if sniffing is active
    std::atomic<bool> m_is_sniffing{false};

    /// @brief Counter of successfully received messages
    std::atomic<uint64_t> m_messages_received{0};

    /// @brief Counter of receive errors
    std::atomic<uint64_t> m_error_count{0};

    /// @brief Sniffing thread handle
    std::unique_ptr<std::thread> m_sniffing_thread;

    /**
     * @brief Main sniffing loop (runs in separate thread)
     * 
     * @details
     * Continuously calls receiveMessage() and dispatches to callback.
     * Runs until is_sniffing_ is set to false by stopSniffing().
     * 
     * @note Internal use only; called from sniffing_thread_
     */
    void sniffingLoop_();
};

}  // namespace busbridge::can

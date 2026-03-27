/**
 * @file can_sniffer.cpp
 * @brief Example: CAN Bus Sniffer using candlelight firmware
 * 
 * @details
 * This example demonstrates how to use BusBridge's CAN sniffer to capture
 * and display all CAN messages from a candlelight USB adapter.
 * 
 * @par Requirements
 * - candlelight firmware on USB CAN adapter
 * - Interface brought up: `sudo ip link set can0 type can bitrate 500000 up`
 * - Root or appropriate permissions (CAP_NET_RAW)
 * 
 * @par Usage
 * ```bash
 * sudo ./read_can_sniffer <can_interface>
 * ```
 *
 * Example:
 * ```bash
 * sudo ./read_can_sniffer can0
 * ```
 *
 * @par Output
 * ```
 * CAN Sniffer started on interface: SocketCAN (can0)
 * ===================================================
 * ID: 0x123 Extended: No  DLC: 8 Data: 01 02 03 04 05 06 07 08
 * ID: 0x456 Extended: No  DLC: 4 Data: AA BB CC DD
 * ...
 * ```
 * 
 * Press Ctrl+C to stop sniffing.
 */

#include "can/CANDevice.h"
#include "can/CANSniffer.h"

#include <iostream>
#include <iomanip>
#include <signal.h>
#include <atomic>

using namespace busbridge::can;

/// @brief Global flag for graceful shutdown on SIGINT
static std::atomic<bool> g_shutdown_requested(false);

/**
 * @brief Signal handler for graceful shutdown
 * @param signum Signal number (typically SIGINT)
 */
void signal_handler(int signum)
{
    std::cout << "\n\nShutdown requested..." << std::endl;
    g_shutdown_requested.store(true);
}

/**
 * @brief Callback function for each received CAN message
 * 
 * @param message The received CANMessage
 * 
 * @details
 * Invoked asynchronously from the CANSniffer thread for each received message.
 * Prints message details in human-readable format.
 */
void onMessageReceived(const CANMessage& message)
{
    // Print CAN ID
    std::cout << "ID: 0x" << std::hex << std::setfill('0') << std::setw(3) 
              << message.id << std::dec << std::setfill(' ');

    // Print extended/standard flag
    std::cout << " Extended: " << (message.is_extended ? "Yes" : "No");

    // Print frame type
    if (message.is_rtr) {
        std::cout << " [RTR]";
    }
    if (message.is_error) {
        std::cout << " [ERROR]";
    }

    // Print DLC and data
    std::cout << " DLC: " << static_cast<int>(message.dlc) << " Data: ";
    
    for (uint8_t i = 0; i < message.dlc; ++i) {
        std::cout << std::hex << std::setfill('0') << std::setw(2) 
                  << static_cast<int>(message.data[i]) << std::dec << std::setfill(' ');
        if (i < message.dlc - 1) std::cout << " ";
    }

    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    // Register signal handler for clean shutdown
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::cout << "BusBridge CAN Sniffer Example\n"
              << "==============================\n" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <can_interface>" << std::endl;
        std::cerr << "Example: " << argv[0] << " can0" << std::endl;
        return static_cast<int>(ErrorCode::InvalidArgument);
    }

    const std::string interface_name = argv[1];

    // Create CAN device for interface
    CANDevice can_device(interface_name);

    std::cout << "Opening CAN interface '" << interface_name << "'..." << std::endl;
    CANError open_result = can_device.openBus();
    
    if (!open_result.ok()) {
        std::cerr << "Error: " << open_result.getMessage() << std::endl;
        std::cerr << "\nTroubleshooting:" << std::endl;
        
        if (open_result.getCode() == ErrorCode::PermissionDenied) {
            std::cerr << "  ❌ Permission denied - Run with sudo:" << std::endl;
            std::cerr << "     sudo ./read_can_sniffer" << std::endl;
        } else if (open_result.getCode() == ErrorCode::DeviceNotFound) {
            std::cerr << "  ❌ Interface not found - Set it up:" << std::endl;
            std::cerr << "     sudo ip link set can0 type can bitrate 500000 up" << std::endl;
        } else {
            std::cerr << "  ❌ Other error - Check that:" << std::endl;
            std::cerr << "     1. Connect candlelight USB adapter" << std::endl;
            std::cerr << "     2. Bring up interface: sudo ip link set can0 type can bitrate 500000 up" << std::endl;
            std::cerr << "     3. Run with sudo: sudo ./read_can_sniffer" << std::endl;
        }
        return static_cast<int>(open_result.getCode());
    }

    std::cout << "Interface opened: " << can_device.getInterfaceName() << std::endl;

    // Create sniffer with the device's interface
    CANSniffer sniffer(can_device.getImpl());

    std::cout << "\nStarting CAN bus sniffer..." << std::endl;
    CANError sniff_result = sniffer.startSniffing(onMessageReceived);
    
    if (!sniff_result.ok()) {
        std::cerr << "Error starting sniffer: " << sniff_result.getMessage() << std::endl;
        can_device.closeBus();
        return static_cast<int>(sniff_result.getCode());
    }

    std::cout << "CAN Sniffer active. Press Ctrl+C to stop.\n"
              << "===================================================\n" << std::endl;

    // Main loop: print statistics every 5 seconds until shutdown requested
    uint64_t last_msg_count = 0;
    
    while (!g_shutdown_requested.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        uint64_t current_msg_count = sniffer.getMessagesReceived();
        uint64_t error_count = sniffer.getErrorCount();

        if (current_msg_count > last_msg_count) {
            std::cout << "\n[Statistics] Messages: " << current_msg_count 
                      << " | Errors: " << error_count << std::endl;
            last_msg_count = current_msg_count;
        }
    }
//Ya esta jalando el sniffer, le mando algun frame con "cansend can0 123#DEADBEEF" y lo obtiene, falta probarlo en el carro
    // Graceful shutdown
    std::cout << "Stopping sniffer..." << std::endl;
    sniffer.stopSniffing();

    std::cout << "Closing CAN interface..." << std::endl;
    can_device.closeBus();

    std::cout << "\n[Final Statistics]" << std::endl;
    std::cout << "Total messages received: " << sniffer.getMessagesReceived() << std::endl;
    std::cout << "Total errors: " << sniffer.getErrorCount() << std::endl;
    std::cout << "\nCAN Sniffer stopped." << std::endl;

    return 0;
}

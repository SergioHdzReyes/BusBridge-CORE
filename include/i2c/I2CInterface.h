#pragma once

#include "i2c/I2CError.h"

#include <cstdint>
#include <cstdlib>

namespace busbridge::i2c {

/**
 * @class I2CInterface
 * @brief Abstract interface for I2C bus communication
 *
 * @details
 * Defines the contract for all I2C interface implementations (Linux I2C-dev, Windows, etc.).
 * Implementations should handle platform-specific details while maintaining
 * this common interface.
 *
 * @par Design Pattern
 * Strategy pattern for pluggable I2C implementations.
 *
 * @par Thread Safety
 * - openBus(): Non-blocking, should be called once before other operations
 * - closeBus(): Blocks until bus is closed
 * - Read/write operations: Non-blocking (but may timeout based on implementation)
 *
 * @note Based on SOLID principles - Single Responsibility, Open/Closed, Liskov Substitution
 * @see I2CDevice for factory implementation
 */
class I2CInterface {
public:
    /// @brief Virtual destructor for proper cleanup of derived classes
    virtual ~I2CInterface() = default;

    /**
     * @brief Open I2C bus
     *
     * @return I2CError object indicating success or failure
     *
     * @details
     * Initializes the I2C bus for communication. Must be called before
     * any other operations (read/write). Performs all necessary setup:
     * - Opening device file or initializing hardware
     * - Setting bus parameters
     * - Configuring timeouts
     *
     * @pre Bus device exists and is accessible
     * @post Bus ready for read/write operations
     *
     * @note idempotent: calling multiple times should be safe (closes previous if needed)
     *
     * @see closeBus()
     */
    virtual I2CError openBus() = 0;

    /**
     * @brief Close I2C bus
     *
     * @details
     * Gracefully closes the I2C bus and releases all resources.
     * After calling this, the bus must be reopened before further operations.
     *
     * @note idempotent: safe to call multiple times
     * @see openBus()
     */
    virtual void closeBus() = 0;

    /**
     * @brief Write a single byte to the I2C device
     *
     * @param value The byte value to write
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Writes a single byte to the currently addressed I2C device.
     * The device address must have been set during bus opening.
     *
     * @pre Bus must be opened via openBus()
     * @post Byte written to device
     *
     * @see writeBytes(), writeRegister()
     */
    virtual I2CError writeByte(uint8_t value) = 0;

    /**
     * @brief Write multiple bytes to the I2C device
     *
     * @param data Pointer to the data buffer
     * @param length Number of bytes to write
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Writes a block of data to the currently addressed I2C device.
     * The device address must have been set during bus opening.
     *
     * @pre Bus must be opened via openBus()
     * @pre @c data must point to valid memory of at least @c length bytes
     * @post Data block written to device
     *
     * @see writeByte(), writeRegister()
     */
    virtual I2CError writeBytes(const uint8_t* data, size_t length) = 0;

    /**
     * @brief Read a single byte from the I2C device
     *
     * @param[out] value Reference to store the read byte
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Reads a single byte from the currently addressed I2C device.
     * The device address must have been set during bus opening.
     *
     * @pre Bus must be opened via openBus()
     * @post @c value parameter contains the read byte if successful
     *
     * @see readBytes(), readRegister()
     */
    virtual I2CError readByte(uint8_t& value) = 0;

    /**
     * @brief Read multiple bytes from the I2C device
     *
     * @param[out] buffer Buffer to store the read data
     * @param length Number of bytes to read
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Reads a block of data from the currently addressed I2C device.
     * The device address must have been set during bus opening.
     *
     * @pre Bus must be opened via openBus()
     * @pre @c buffer must have space for at least @c length bytes
     * @post @c buffer contains the read data if successful
     *
     * @see readByte(), readRegisterBlock()
     */
    virtual I2CError readBytes(uint8_t* buffer, size_t length) = 0;

    /**
     * @brief Write a byte to a specific register
     *
     * @param reg Register address
     * @param value Value to write
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Performs a combined write operation: first writes the register address,
     * then writes the value byte. This is a common I2C device access pattern.
     *
     * @pre Bus must be opened via openBus()
     * @post Register contains the new value
     *
     * @see readRegister(), writeBytes()
     */
    virtual I2CError writeRegister(uint8_t reg, uint8_t value) = 0;

    /**
     * @brief Read a byte from a specific register
     *
     * @param reg Register address
     * @param[out] value Reference to store the read value
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Performs a combined read operation: first writes the register address,
     * then reads the value byte. This is a common I2C device access pattern.
     *
     * @pre Bus must be opened via openBus()
     * @post @c value parameter contains the register value if successful
     *
     * @see writeRegister(), readRegisterBlock()
     */
    virtual I2CError readRegister(uint8_t reg, uint8_t& value) = 0;

    /**
     * @brief Read multiple bytes from consecutive registers
     *
     * @param reg Starting register address
     * @param[out] buffer Buffer to store the read data
     * @param length Number of bytes to read
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Performs a combined operation: writes the starting register address,
     * then reads multiple consecutive bytes. Useful for reading sensor data
     * or configuration blocks.
     *
     * @pre Bus must be opened via openBus()
     * @pre @c buffer must have space for at least @c length bytes
     * @post @c buffer contains the register block data if successful
     *
     * @see readRegister(), readBytes()
     */
    virtual I2CError readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) = 0;
};

}  // namespace busbridge::i2c

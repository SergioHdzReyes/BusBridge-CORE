#pragma once

#include "i2c/I2CInterface.h"

#include <memory>

namespace busbridge::i2c {

/**
 * @class I2CDevice
 * @brief Factory class for creating platform-specific I2C interface implementations
 *
 * @details
 * Provides abstraction over platform-specific I2C implementations. On Linux,
 * creates LinuxI2CDevice. Can be extended for other platforms (Windows, etc.).
 *
 * Follows the Factory and Strategy design patterns, delegating actual operations
 * to the appropriate implementation while maintaining a consistent interface.
 *
 * @par Usage
 * @code
 * I2CDevice device("/dev/i2c-1", 0x76);
 * auto result = device.openBus();
 * if (!result.ok()) {
 *     std::cerr << "Error: " << result.getMessage() << std::endl;
 *     return result.getCode();
 * }
 *
 * uint8_t data;
 * device.readRegister(0xD0, data);
 * @endcode
 *
 * @par Design
 * - Constructor arguments specify bus path and device address
 * - Internally creates appropriate I2CInterface implementation via unique_ptr
 * - Delegates all operations to underlying implementation
 * - Provides convenient wrapper methods that match I2CInterface
 *
 * @note Thread-safe if underlying implementation is thread-safe
 * @see I2CInterface for operation details
 */
class I2CDevice {
public:
    /**
     * @brief Constructor
     *
     * @param bus Bus device path (e.g., "/dev/i2c-1")
     * @param address 7-bit I2C device address (0x00-0x7F)
     *
     * @details
     * Creates an I2CDevice and automatically selects the appropriate
     * implementation based on the platform. The actual implementation
     * instance is created here via factory logic.
     *
     * @pre @c bus path must exist and be accessible
     * @pre @c address must be valid 7-bit I2C address
     * @note Does NOT open the bus; call openBus() for that
     * @see openBus()
     */
    I2CDevice(const std::string& bus, uint8_t address);

    /// @brief Destructor - ensures proper cleanup of implementation
    ~I2CDevice() = default;

    /**
     * @brief Get reference to underlying implementation
     *
     * @return Reference to I2CInterface implementation
     *
     * @details
     * Provides direct access to the underlying interface implementation.
     * Useful for advanced operations or when implementation-specific
     * features are needed.
     *
     * @pre Underlying implementation pointer must be valid
     * @throw May throw if not properly initialized (implementation-dependent)
     */
    I2CInterface& getImpl() { return *impl; }

    /**
     * @brief Get const reference to underlying implementation
     *
     * @return Const reference to I2CInterface implementation
     *
     * @see getImpl()
     */
    const I2CInterface& getImpl() const { return *impl; }

    /**
     * @brief Open the I2C bus
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's openBus().
     *
     * @see I2CInterface::openBus()
     */
    I2CError openBus() { return impl->openBus(); }

    /**
     * @brief Close the I2C bus
     *
     * @details
     * Wrapper that delegates to underlying implementation's closeBus().
     *
     * @see I2CInterface::closeBus()
     */
    void closeBus() { impl->closeBus(); }

    /**
     * @brief Write a single byte
     *
     * @param value The byte value to write
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's writeByte().
     *
     * @see I2CInterface::writeByte()
     */
    I2CError writeByte(uint8_t value) { return impl->writeByte(value); }

    /**
     * @brief Write multiple bytes
     *
     * @param data Pointer to the data buffer
     * @param length Number of bytes to write
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's writeBytes().
     *
     * @see I2CInterface::writeBytes()
     */
    I2CError writeBytes(const uint8_t* data, size_t length) {
        return impl->writeBytes(data, length);
    }

    /**
     * @brief Read a single byte
     *
     * @param[out] value Reference to store the read byte
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's readByte().
     *
     * @see I2CInterface::readByte()
     */
    I2CError readByte(uint8_t& value) { return impl->readByte(value); }

    /**
     * @brief Read multiple bytes
     *
     * @param[out] buffer Buffer to store the read data
     * @param length Number of bytes to read
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's readBytes().
     *
     * @see I2CInterface::readBytes()
     */
    I2CError readBytes(uint8_t* buffer, size_t length) {
        return impl->readBytes(buffer, length);
    }

    /**
     * @brief Write to a register
     *
     * @param reg Register address
     * @param value Value to write
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's writeRegister().
     *
     * @see I2CInterface::writeRegister()
     */
    I2CError writeRegister(uint8_t reg, uint8_t value) {
        return impl->writeRegister(reg, value);
    }

    /**
     * @brief Read from a register
     *
     * @param reg Register address
     * @param[out] value Reference to store the read value
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's readRegister().
     *
     * @see I2CInterface::readRegister()
     */
    I2CError readRegister(uint8_t reg, uint8_t& value) {
        return impl->readRegister(reg, value);
    }

    /**
     * @brief Read a block of registers
     *
     * @param reg Starting register address
     * @param[out] buffer Buffer to store the read data
     * @param length Number of bytes to read
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Wrapper that delegates to underlying implementation's readRegisterBlock().
     *
     * @see I2CInterface::readRegisterBlock()
     */
    I2CError readRegisterBlock(uint8_t reg, uint8_t* buffer, size_t length) {
        return impl->readRegisterBlock(reg, buffer, length);
    }

private:
    /// @brief Pointer to platform-specific implementation
    std::unique_ptr<I2CInterface> impl;
};

}  // namespace busbridge::i2c

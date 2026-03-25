#pragma once

#include "I2CInterface.h"

/// @brief BME280 sensor register addresses
constexpr uint8_t REG_CALIBSTART = 0x88;

constexpr uint8_t REG_ID        = 0xD0;
constexpr uint8_t REG_RESET     = 0xE0;
constexpr uint8_t REG_CTRL_HUM  = 0xF2;
constexpr uint8_t REG_STATUS    = 0xF3;
constexpr uint8_t REG_CTRL_MEAS = 0xF4;
constexpr uint8_t REG_CONFIG    = 0xF5;

constexpr uint8_t REG_PRESS_MSB = 0xF7;
constexpr uint8_t REG_PRESS_LSB = 0xF8;
constexpr uint8_t REG_PRESS_XLSB= 0xF9;

constexpr uint8_t REG_TEMP_MSB  = 0xFA;
constexpr uint8_t REG_TEMP_LSB  = 0xFB;
constexpr uint8_t REG_TEMP_XLSB = 0xFC;

constexpr uint8_t REG_HUM_MSB   = 0xFD;
constexpr uint8_t REG_HUM_LSB   = 0xFE;

/**
 * @brief BME280 calibration data structure
 * @details Packed structure for efficient memory layout matching sensor format
 */
#pragma pack(push,1)
struct CalibrationData {
    uint16_t dig_T1;  ///< Temperature calibration coefficient T1
    int16_t  dig_T2;  ///< Temperature calibration coefficient T2
    int16_t  dig_T3;  ///< Temperature calibration coefficient T3
    uint16_t dig_P1;  ///< Pressure calibration coefficient P1
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
};
#pragma pack(pop)

namespace busbridge::i2c {

/**
 * @class BME280Reader
 * @brief Driver for Bosch BME280 environmental sensor
 *
 * @details
 * Provides high-level interface for reading temperature, pressure, and humidity
 * from Bosch BME280 sensor. Handles calibration data reading, compensation
 * calculations, and sensor configuration.
 *
 * The BME280 is a digital pressure, temperature, and humidity sensor with
 * high accuracy and low power consumption.
 *
 * @par Usage
 * @code
 * I2CDevice device("/dev/i2c-1", 0x76);
 * device.openBus();
 *
 * BME280Reader sensor(device.getImpl());
 * sensor.initialize();
 *
 * int32_t temp_raw, press_raw, hum_raw;
 * sensor.readRawData(temp_raw, press_raw, hum_raw);
 *
 * float temperature = sensor.compensateTemperature(temp_raw);
 * float pressure = sensor.compensatePressure(press_raw);
 * float humidity = sensor.compensateHumidity(hum_raw);
 * @endcode
 *
 * @note Requires I2C interface to be opened before use
 * @see https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/
 */
class BME280Reader {
public:
    /**
     * @brief Constructor
     *
     * @param device Reference to I2C interface for sensor communication
     *
     * @details
     * Creates a BME280Reader instance bound to the provided I2C interface.
     * The interface should be configured for the correct sensor address.
     */
    explicit BME280Reader(I2CInterface& device);

    /**
     * @brief Initialize the BME280 sensor
     *
     * @return I2CError indicating success or failure
     *
     * @details
     * Performs sensor initialization including:
     * - Reading calibration data from sensor
     * - Configuring sensor operating mode
     * - Setting measurement parameters
     *
     * @pre I2C interface must be opened and sensor must be accessible
     * @post Sensor is ready for measurements
     */
    I2CError initialize();

    I2CError readRawData(int32_t& adc_T, int32_t& adc_P, int32_t& adc_H);

    float compensateTemperature(uint32_t adc_T);

    [[nodiscard]] float compensatePressure(uint32_t adc_P) const;

    [[nodiscard]] float compensateHumidity(uint16_t adc_H) const;

private:
    I2CInterface& m_device;
    int32_t m_t_fine;
    CalibrationData m_calib{};

    I2CError readCalibrationData();
};

}

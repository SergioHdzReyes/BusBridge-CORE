#pragma once

#include "I2CInterface.h"

constexpr uint8_t REG_CALIBSTART= 0x88;

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

#pragma pack(push,1)
struct CalibrationData {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
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

class BME280Reader {
public:
    explicit BME280Reader(I2CInterface& device);

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

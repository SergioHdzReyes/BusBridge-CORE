#include "BME280Reader.h"

#include <cstring>

namespace busbridge::i2c {

BME280Reader::BME280Reader(I2CInterface& device)
    : m_device(device), m_t_fine(0) {

}

I2CError BME280Reader::initialize() {
    I2CError calibDataRes(readCalibrationData());
    if (!calibDataRes.ok()) {
        return calibDataRes;
    }

    uint8_t ctrl_hum = 0b001;
    uint8_t ctrl_meas = 0b00100111;
    uint8_t config = 0b10100000;

    I2CError regHumRes(m_device.writeRegister(REG_CTRL_HUM, ctrl_hum));
    if (!regHumRes.ok()) {
        return regHumRes;
    }

    I2CError regMeasRes(m_device.writeRegister(REG_CTRL_MEAS, ctrl_meas));
    if (!regMeasRes.ok()) {
        return regMeasRes;
    }

    I2CError regConfigRes(m_device.writeRegister(REG_CONFIG, config));
    if (!regConfigRes.ok()) {
        return regConfigRes;
    }

    return {};
}

I2CError BME280Reader::readCalibrationData()
{
    uint8_t calibBlock1[26];
    I2CError calibFirstRes(m_device.readRegisterBlock(REG_CALIBSTART, calibBlock1, sizeof(calibBlock1)));
    if (!calibFirstRes.ok())
        return calibFirstRes;

    std::memcpy(&m_calib, calibBlock1, 26);

    uint8_t calibBlock2[7];
    I2CError calibSecondRes(m_device.readRegisterBlock(0xE1, calibBlock2, sizeof(calibBlock2)));
    if (!calibSecondRes.ok())
        return calibSecondRes;

    m_calib.dig_H2 = (int16_t)(calibBlock2[1] << 8 | calibBlock2[0]);
    m_calib.dig_H3 = calibBlock2[2];
    m_calib.dig_H4 = (int16_t)((calibBlock2[3] << 4) | (calibBlock2[4] & 0x0F));
    m_calib.dig_H5 = (int16_t)((calibBlock2[5] << 4) | (calibBlock2[4] >> 4));
    m_calib.dig_H6 = (int8_t)calibBlock2[6];

    return {};
}

I2CError BME280Reader::readRawData(int32_t& adc_T, int32_t& adc_P, int32_t& adc_H)
{
    uint8_t data[8];
    I2CError pressRes(m_device.readRegisterBlock(REG_PRESS_MSB, data, sizeof(data)));
    if (!pressRes.ok())
        return pressRes;

    adc_P = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((data[2] >> 4) & 0x0F));
    adc_T = (int32_t)(((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((data[5] >> 4) & 0x0F));
    adc_H = (int32_t)((data[6] << 8) | data[7]);
    return {};
}

float BME280Reader::compensateTemperature(uint32_t adc_T) {
    uint32_t var1 = ((((adc_T >> 3) - (m_calib.dig_T1 << 1))) * m_calib.dig_T2) >> 11;
    uint32_t var2 = (((((adc_T >> 4) - m_calib.dig_T1) * ((adc_T >> 4) - m_calib.dig_T1)) >> 12) * m_calib.dig_T3) >> 14;

    m_t_fine = (int32_t)(var1 + var2);
    double T = (m_t_fine * 5 + 128) >> 8;
    return T / 100.0;
}

float BME280Reader::compensatePressure(uint32_t adc_P) const {
    int64_t var1, var2, p;

    var1 = ((int64_t)m_t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)m_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)m_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)m_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)m_calib.dig_P3) >> 8) +
           ((var1 * (int64_t)m_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)m_calib.dig_P1) >> 33;

    if (var1 == 0) {
        return 0;
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)m_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)m_calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)m_calib.dig_P7) << 4);
    return (float)p / 256.0f / 100.0f;
}

float BME280Reader::compensateHumidity(uint16_t adc_H) const {
    int32_t v_x1_u32r;

    v_x1_u32r = (m_t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)m_calib.dig_H4) << 20) -
                    (((int32_t)m_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)m_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)m_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)m_calib.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r -
                 (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                   ((int32_t)m_calib.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    double h = (v_x1_u32r >> 12);
    return h / 1024.0;
}

}

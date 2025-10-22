#include "BME280Reader.h"
#include "I2CDevice.h"

#include <iostream>

int main()
{
    uint8_t sensorAddr = 0x77;
    I2CDevice i2c("/dev/i2c-1", sensorAddr);
    std::cerr << "Trying to open BME280 sensor\n\n";

    I2CError result(i2c.openBus());
    if (!result.ok()) {
        std::cerr << "It was not possible to open the device: " << result.getMessage() << std::endl;
        return result.getCode();
    }

    std::unique_ptr<BME280Reader> sensor = std::make_unique<BME280Reader>(i2c.getImpl());

    I2CError initRes(sensor->initialize());
    if (!initRes.ok()) {
        std::cerr << "The device could not be initialized: " << initRes.getMessage() << std::endl;
        return initRes.getCode();
    }

    int32_t raw_T = 0, raw_P = 0, raw_H = 0;
    I2CError readRawRes(sensor->readRawData(raw_T, raw_P, raw_H));
    if (!readRawRes.ok()) {
        std::cerr << "Error reading sensor data: " << readRawRes.getMessage() << std::endl;
        return readRawRes.getCode();
    }

    float temperature = sensor->compensateTemperature(raw_T);
    float pressure    = sensor->compensatePressure(raw_P);
    float humidity    = sensor->compensateHumidity(raw_H);

    std::cout << "Temperature: " << temperature << " °C" << std::endl;
    std::cout << "Pressure: " << pressure << " hPa" << std::endl;
    std::cout << "Humidity: " << humidity << " %" << std::endl;

    return static_cast<int>(ErrorCode::Success);
}

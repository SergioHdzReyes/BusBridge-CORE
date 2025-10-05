#include "BME280Reader.h"
#include "platform/LinuxI2CDevice.h"

#include <iostream>

int main()
{
    uint8_t sensorAddr = 0x77;
    LinuxI2CDevice i2c("/dev/i2c-1", sensorAddr);
    std::cerr << "Trying to open BME280 sensor\n\n";

    if (!i2c.openBus()) {
        std::cerr << "It was not possible to open the device.\n";
        return -1;
    }

    BME280Reader sensor(i2c);

    if (!sensor.initialize())
    {
        std::cerr << "The device could not be initialized." << std::endl;
        return -1;
    }

    int32_t raw_T = 0, raw_P = 0, raw_H = 0;
    if (!sensor.readRawData(raw_T, raw_P, raw_H))
    {
        std::cerr << "Error reading sensor data." << std::endl;
        return -1;
    }

    float temperature = sensor.compensateTemperature(raw_T);
    float pressure    = sensor.compensatePressure(raw_P);
    float humidity    = sensor.compensateHumidity(raw_H);

    std::cout << "Temperature: " << temperature << " °C" << std::endl;
    std::cout << "Pressure: " << pressure << " hPa" << std::endl;
    std::cout << "Humidity: " << humidity << " %" << std::endl;

    return 0;
}

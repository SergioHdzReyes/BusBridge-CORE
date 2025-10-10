# BusBridge CORE

**BusBridge CORE is a cross-platform C++ middleware library for embedded communication buses.**

🔹 Free Core (MIT) supporting **I2C**  
🔹 Roadmap includes **SPI, UART, CAN** in future PRO releases  
🔹 Designed to be efficient, portable, and easy to extend

---

## ✨ Features

- 📍 **Cross-platform**: Linux and Raspberry Pi initially (Windows planned)
- ⚡ **Efficient C++**: low resource usage, ideal for embedded systems
- 🔧 **Clean API** for integration with IoT and automation projects
- 🌱 **Open Source (MIT)**: free to use, modify, and contribute
- 🛠 **Extensible**: built to grow with support for additional buses in PRO version

---

## 🚀 Quick Start (I2C Sensor Reading)

### 🧠 Run the Example

```bash
1️⃣ Clone the repository
git clone https://github.com/youruser/busbridge-core.git
cd busbridge-core

2️⃣ Build library and examples
cmake -B build
cmake --build build -j$(nproc)

3️⃣ Run the BME280 example
sudo ./build/read_bme280
```

### 🧾 Expected Output
```bash
Trying to open BME280 sensor

Temperature: 24.6 °C
Pressure: 1013.1 hPa
Humidity: 47.9 %
```

### 🧠 What Does This Example Do?

The example read_bme280.cpp demonstrates how to:

1️⃣ Open the I²C bus (/dev/i2c-1)

2️⃣ Initialize a BME280Reader instance

3️⃣ Read sensor data

4️⃣ Print temperature, humidity, and pressure

Source code: **examples/read_bme280.cpp**

## 💡 Quick Look at the API

If you just want to see how simple it is to use the library, here’s a minimal snippet:

```bash
I2CDevice i2c("/dev/i2c-1", 0x76);
BME280Reader reader(i2c);

int32_t temp = 0, press = 0, hum = 0;
reader->readRawData(temp, press, hum);
float temperature = sensor->compensateTemperature(temp);

std::cout << temperature << " °C\n";
```

That’s all it takes — clean, modern C++ that talks directly to your sensor.
If you want to see it running, just build and run the example above 🚀

---

## 🗺 Roadmap

- ✅ **Initial I2C support (Core OSS)**
- 🔄 Integration examples with common sensors (BMP280, BME280)
- ⏳ SPI and UART support (future PRO release)
- ⏳ CAN bus support (future PRO release)
- ⏳ REST API / daemon for remote integration (future PRO release)

> Note: Features marked as "future PRO release" will be available only in the commercial PRO version.

---

## 🤝 Contributing

Community contributions are welcome! 🙌  
To contribute:

1. Fork the repo
2. Create a branch (`feature/new-feature`)
3. Commit your changes with clear messages
4. Submit a pull request

Issues can be opened to:
- Report bugs 🐛
- Suggest new features 💡
- Improve documentation 📖

---

## 📚 Documentation

Documentation is available in the [`docs/`](./docs) folder:
- Installation guides
- API reference
- Usage examples

---

## 📜 License

This project is licensed under the **MIT License**. See [`LICENSE`](./LICENSE) for details.

---

## ⭐ Support the Project

If you find this project useful:
- Give it a **star** ⭐ on GitHub
- Share with other developers
- Consider supporting future PRO development (SPI, UART, CAN, PRO version)

💡 *This is just the beginning. With your help, we can build a robust, cross-platform library for embedded hardware integration.*
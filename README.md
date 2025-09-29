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

```cpp
#include <i2c/I2CDevice.h>
#include <iostream>

int main() {
    I2CDevice sensor(0x76, "/dev/i2c-1"); // sensor address on the bus
    uint8_t temp_reg = 0xFA;

    auto data = sensor.readBytes(temp_reg, 2);
    int temperature = (data[0] << 8) | data[1];

    std::cout << "Temperature: " << temperature / 100.0 << " °C" << std::endl;
    return 0;
}
```

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
# GoofyBLEProvisioning

**ESP32 Arduino Library for BLE Wi-Fi Provisioning, Sensor Reporting & Motor Control**

An integrated library enabling seamless Wi-Fi credential provisioning over BLE, persistent storage in EEPROM, JSON-based sensor data reporting, and remote motor control on ESP32 platforms—without any web server or captive portal.

---

## 🌟 Key Features

1. **BLE-Only Wi-Fi Provisioning**
   - No AP or Web UI needed. Configure network using BLE writes:
     ```json
     { "ssid": "YourNetwork", "pass": "YourPassword" }
     ```
   - Credentials safely stored in 128 bytes of EEPROM.

2. **Rapid Wi-Fi Connection**
   - Non-blocking connect routine with configurable 10 s timeout (`WIFI_TIMEOUT_MS`).
   - Automatic fallback to BLE provisioning if connection fails.

3. **Flexible Sensor Data Reporting**
   - Periodic BLE notifications with structured JSON:
     ```json
     {
       "device_id": "ESP32_001",
       "timestamp": 1625247600,
       "sensors": { "temperature": 25.4, "humidity": 60.1 },
       "motor_status": "ON"
     }
     ```
   - Easily extendable to add additional sensors (e.g., LDR, pressure).

4. **BLE-Based Motor Control**
   - Control digital outputs with simple JSON
     ```json
     { "motor": "ON" }
     { "motor": "OFF" }
     ```
   - Output pin configurable via `MOTOR_PIN` (default GPIO 5).
   - Internal state tracking with `isMotorOn()` API.

5. **Two-Way JSON Messaging**
   - Parse incoming BLE writes via ArduinoJson.
   - Notify sensor updates and status changes seamlessly.

---

## 🚀 Installation

1. **Download** the latest ZIP (`GoofyBLEProvisioning_v3.1_MotorPin.zip`).
2. Open **Arduino IDE** → **Sketch** → **Include Library** → **Add .ZIP Library...**
3. Select the ZIP to install. The library appears under **Examples → GoofyBLEProvisioning**.

---

## 🛠️ Hardware Setup

- **Motor/Relay** → Connect control pin to `MOTOR_PIN` (GPIO 5 by default).
- **DHT22 Sensor** (optional) → DATA → GPIO 27, VCC → 3.3 V, GND → GND.
- **Power** → ESP32 must run at 3.3 V.

---

## ⚙️ Usage Example

Open the provided example sketch: **File → Examples → GoofyBLEProvisioning → BLEWiFiProvisioning**

```cpp
#include <GoofyBLEProvisioning.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 27
#define DHTTYPE DHT22

GoofyBLEProvisioning provisioner;
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastSend = 0;
const unsigned long interval = 5000;

void setup() {
  Serial.begin(115200);
  dht.begin();
  provisioner.begin();
}

void loop() {
  provisioner.loop();

  if (millis() - lastSend > interval) {
    lastSend = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    DynamicJsonDocument doc(256);
    doc["device_id"] = "ESP32_001";
    doc["timestamp"] = now(); // if RTC available
    auto sensors = doc.createNestedObject("sensors");
    sensors["temperature"] = isnan(t) ? -1 : t;
    sensors["humidity"]    = isnan(h) ? -1 : h;
    doc["motor_status"] = provisioner.isMotorOn() ? "ON" : "OFF";

    String payload;
    serializeJson(doc, payload);
    Serial.println("BLE Notify: " + payload);
    provisioner.sendBLE(payload);
  }
}
```

---

## 🔍 API Reference

### `GoofyBLEProvisioning`

| Method                                 | Description                                      |
|----------------------------------------|--------------------------------------------------|
| `void begin()`                         | Initialize EEPROM, configure pins, attempt Wi-Fi |
| `void loop()`                          | Must be called in `loop()`, handles BLE & updates|
| `void sendBLE(const String &msg)`      | Send a JSON string over BLE notify               |
| `bool isMotorOn()`                     | Returns current motor state                      |

---

## 🏗️ Configuration Options

- **`src/config/Config.h`**:
  - `WIFI_TIMEOUT_MS` (default 10000 ms) — Wi-Fi connect timeout
  - `MOTOR_PIN` (default 5) — GPIO for motor control
  - `SERVICE_UUID`, `CHAR_UUID_*` — BLE service & characteristic UUIDs

---

## 📂 Folder Structure and Files

```
GoofyBLEProvisioning/
├── src/
│   ├── config/
│   │   └── Config.h                // Constants and pin definitions
│   ├── core/
│   │   ├── ProvisioningManager.h  // Class declaration for BLE+WiFi logic
│   │   └── ProvisioningManager.cpp // Logic for handling BLE, EEPROM, WiFi, Motor
│   ├── GoofyBLEProvisioning.h     // Public interface for library
│   └── GoofyBLEProvisioning.cpp   // Links public interface to internal manager
│
├── examples/
│   └── BLEWiFiProvisioning/
│       └── BLEWiFiProvisioning.ino // Demo using DHT22 and BLE provisioning
│
├── library.properties              // Arduino metadata for Library Manager
├── README.md                       // Documentation (this file)
└── LICENSE                         // License file (MIT)
```

### Key File Descriptions:

- **`Config.h`**
  - Central place to change pins, BLE UUIDs, and timeouts.

- **`ProvisioningManager`**
  - Core logic. Handles BLE callbacks, stores credentials, motor control, BLE notify.

- **`GoofyBLEProvisioning.{h,cpp}`**
  - Wrapper class used in your `.ino` sketch. Simple API: `begin()`, `loop()`, `sendBLE()`, `isMotorOn()`.

- **Example Sketch**
  - Shows full BLE provisioning → sensor read → BLE JSON notify → motor control loop.

---

## 🤝 Contributing

Feel free to fork and submit pull requests. For major changes, create an issue first to discuss.

---

## 📄 License

MIT © Chamath Adithya (SOLVEO)

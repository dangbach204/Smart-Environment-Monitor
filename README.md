# Smart Environment Monitor (ESP32 + Blynk + OLED + DHT11 + RGB LED)

## Introduction

The **Smart Environment Monitor** project monitors **temperature**, **humidity**, and **ambient light** using an **ESP32** microcontroller.  
Data is displayed on an **OLED SSD1306** screen, visualized with an **RGB LED**, and sent to the **Blynk IoT App** for remote monitoring.

---

## Hardware Components

| Component                          | Function                                          |
| ---------------------------------- | ------------------------------------------------- |
| **ESP32**                          | Main controller, handles WiFi and data processing |
| **DHT11 Sensor**                   | Measures temperature and humidity                 |
| **LDR (Light Dependent Resistor)** | Detects ambient light intensity                   |
| **OLED SSD1306 (I2C)**             | Displays sensor readings and mode status          |
| **RGB LED**                        | Indicates environment status based on temperature |
| **Push Button**                    | Switches between operation modes                  |
| **Blynk IoT App**                  | Displays data and allows remote mode control      |

---

## Operating Modes

| Mode  | Name             | Description                                                                           |
| ----- | ---------------- | ------------------------------------------------------------------------------------- |
| **1** | **Full Mode**    | Displays temperature, humidity, and light on OLED. Sends data to Blynk.               |
| **2** | **Sleep Mode**   | Displays only light level; blue LED indicates darkness.                               |
| **3** | **Warning Mode** | Displays temperature in large font; RGB LED changes color based on temperature level. |

---

## Functional Overview

### 🌤 Sensor Readings & Display

- **DHT11** → Reads temperature (°C) and humidity (%).
- **LDR** → Reads light intensity (0–4095) and maps it to percentage (100% = bright, 0% = dark).
- **OLED** → Displays sensor data depending on selected mode.

### RGB LED Color Logic (Temperature Warning)

| Temperature (°C) | LED Color          | Description      |
| ---------------- | ------------------ | ---------------- |
| < 25             | Blue               | Cool             |
| 25–27            | Cyan / Light Green | Normal           |
| 28–30            | Yellow             | Warm             |
| 31–34            | Red                | Hot              |
| ≥ 35             | Blinking Red       | Overheat Warning |

### Blynk IoT Integration

| Virtual Pin | Data Sent           | Description            |
| ----------- | ------------------- | ---------------------- |
| **V0**      | Temperature         | °C                     |
| **V1**      | Humidity            | %                      |
| **V2**      | Light Intensity     | %                      |
| **V3**      | Mode Control Button | Switches between modes |

---

## Pin Configuration

| Component           | ESP32 GPIO | Notes                     |
| ------------------- | ---------- | ------------------------- |
| **DHT11 Sensor**    | 7          | Data pin                  |
| **LDR Sensor**      | 3          | Analog light sensor input |
| **RGB LED - Red**   | 4          | PWM output                |
| **RGB LED - Green** | 5          | PWM output                |
| **RGB LED - Blue**  | 6          | PWM output                |
| **Push Button**     | 1          | Interrupt-based input     |
| **OLED SDA**        | 19         | I2C data                  |
| **OLED SCL**        | 18         | I2C clock                 |

> ⚠️ You can modify pin assignments according to your circuit design.

---

## System Workflow

1. ESP32 connects to WiFi and initializes the Blynk connection.
2. A periodic **1-second timer** (`esp_timer`) triggers sensor readings.
3. Sensor data is:
   - Displayed on the OLED screen
   - Sent to Blynk Cloud (if WiFi is available)
4. The user can switch modes by:
   - **Pressing the physical button**, or
   - **Using the Blynk App (V3)**
5. In **Warning Mode**, the RGB LED color changes dynamically according to temperature levels.

---

## Recommended Blynk Dashboard Setup

| Widget     | Virtual Pin | Type        | Description                  |
| ---------- | ----------- | ----------- | ---------------------------- |
| **Gauge**  | V0          | Temperature | Displays temperature (°C)    |
| **Gauge**  | V1          | Humidity    | Displays humidity (%)        |
| **Gauge**  | V2          | Light       | Displays light intensity (%) |
| **Button** | V3          | Switch      | Changes operation mode       |

---

## WiFi & Blynk Configuration

In your code, update the following section:

```cpp
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"
```

> Obtain your **Auth Token** from the **Blynk Cloud Dashboard**.

---

## Required Libraries

Make sure to install these libraries in **Arduino IDE** or **PlatformIO**:

- `Blynk` (v1.0.1 or later)
- `Adafruit SSD1306`
- `Adafruit GFX`
- `DHT sensor library`
- `ESP32 board package`

---

## Upload Instructions

1. Connect ESP32 to your computer via USB.
2. Open the `.ino` file in Arduino IDE.
3. Go to **Tools → Board → ESP32 Dev Module**.
4. Select the correct **COM Port**.
5. Enter your WiFi and Blynk credentials.
6. Click **Upload**.
7. Open **Serial Monitor (115200 baud)** to check output logs.

---

## Technical Notes

- Uses **hardware interrupt** for mode switching via button.
- Uses **esp_timer** for periodic sensor reading (non-blocking).
- Runs in **offline mode** when WiFi/Blynk is not available.
- Optimized OLED refresh using full-frame redraw every cycle.

---

## Author

| Field            | Information                              |
| ---------------- | ---------------------------------------- |
| **Project Name** | Smart Environment Monitor                |
| **Platform**     | ESP32                                    |
| **Tools Used**   | Arduino IDE / PlatformIO                 |
| **Developer**    | Student (IoT / Embedded Systems Project) |

---

## Conclusion

The **Smart Environment Monitor** demonstrates a complete **IoT-based sensing system**, integrating sensor acquisition, OLED display, and Blynk IoT connectivity.  
It can be further extended with:

- Data logging to a web server or database,
- Automatic control of devices (fans, lights),
- Integration with **MQTT**, **Firebase**, or **Home Assistant**.

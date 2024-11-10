# Hip Joint Angle Monitoring System with ESP32

This project aims to monitor and control the hip joint angles of patients post-surgery using embedded systems and sensors. The device tracks 
specific movement thresholds (such as rotation, excessive adduction, cross-leg, etc.) and issues audio warnings when these angles exceed 
programmed safe limits. This system is particularly valuable in minimizing strain or damage to surrounding tissues after procedures like 
hip replacement surgery.

---

## 📋 Project Overview

The Hip Joint Angle Monitoring System is built to:
- **Monitor hip joint angles** to prevent injury during recovery.
- **Alert the user with an audio warning** when angles exceed safe limits.
- **Support customizable limits** for individual patient needs.

This system is based on the **ESP32** microcontroller, which offers robust processing power, Wi-Fi capabilities for potential extensions, and easy integration with various sensors. The system uses I2C-compatible sensors to detect angular movement in real-time, providing accurate monitoring and feedback.

---

## 🔧 Components

- **ESP32 Development Board**: Handles data processing and communicates with sensors.
- **MPU6050 Gyroscope and Accelerometer** (or similar I2C sensor): Tracks the angle and orientation of the hip joints.
- **Buzzer**: Issues audio warnings when the angles exceed safe limits.
- **Breadboard and Jumper Wires**: For circuit connections.
- **Power Source**: USB or battery pack compatible with ESP32.

---

## ⚙️ System Setup

1. **Wiring the Components**:
    - Connect the **SDA** and **SCL** pins of the MPU6050 to GPIO 21 (SDA) and GPIO 22 (SCL) on the ESP32, respectively.
    - Connect the **buzzer** to a digital GPIO pin on the ESP32 (e.g., GPIO 15).
    - Ensure the ESP32 and all components share a common **ground (GND)**.

2. **Software Setup**:
    - Install the latest **Arduino IDE** and add the **ESP32 board** to the IDE by following [this guide](https://github.com/espressif/arduino-esp32).
    - Install necessary libraries:
        - [Wire](https://www.arduino.cc/en/Reference/Wire): For I2C communication.
        - [ezBuzzer](https://github.com/yasiralijaved/ezBuzzer) (or similar library): For buzzer control.
        - MPU6050 Library for sensor control: Can be installed via the Arduino Library Manager.

3. **Code Setup**:
    - Download or clone this repository and open the `.ino` file in the Arduino IDE.
    - Select **ESP32** as your board in the Arduino IDE, then choose the appropriate COM port.
    - Upload the code to the ESP32.

---

## 🚀 Usage

1. **Power On**: Power the ESP32 either through USB or a compatible power source.
2. **Monitoring**: The system will immediately begin monitoring the hip joint angles.
3. **Audio Alerts**: When the angles exceed the preset limits, the buzzer will sound an alert.
4. **Adjusting Limits**: Modify angle thresholds in the code (`rollLimit`, `pitchLimit`, and `yawLimit`) based on patient needs and upload the updated code.

---

## 📐 Customization

You can adjust:
- **Angle Limits**: Change angle threshold macros in the code (e.g., `rollLimit_gyroA`, `pitchLimit_gyroA`) to customize warning sensitivity.
- **Buzzer Warnings**: Adjust buzzer warning conditions to sound only when angles approach (but do not exceed) the limits for gentler alerts.

---

## 📂 File Structure

```
project-folder
│
├── GetAccAnglesWire.ino        # Main code for ESP32 and sensor integration
├── README.md                   # Project documentation
└── LICENSE                     # License for the project
```

---

## 🔄 Future Enhancements

- **Wireless Connectivity**: Use ESP32’s Wi-Fi for remote monitoring.
- **Data Logging**: Store data for tracking patient progress over time.
- **Mobile App Integration**: Connect with a mobile app for patient or doctor notifications.

---

## 📝 Acknowledgments

Special thanks to all contributors, libraries, and resources that made this project possible.

# 🌱 Arduino Smart Greenhouse – Auto Watering, LED Lighting & Solar Charging

This Arduino project is a **smart greenhouse system** that:

- 🌿 **Automatically waters plants** when the soil is dry
- 🌡️ **Monitors temperature, humidity, and light**
- 💡 **Turns on LED lights when it gets dark**
- ☀️ **Uses a solar panel to charge the battery**

It's coded in **C++** and designed to help plants grow with minimal human input, making it perfect for small-scale gardening or home automation.

---

## 🔧 How It Works

- A **soil moisture sensor** monitors the dryness of the soil.
  - If the soil is dry, a **MOSFET** activates the water pump to hydrate the plant.
- A **photoresistor (LDR)** detects light levels.
  - When it gets dark, **LEDs automatically turn on** to provide extra light.
- A **DHT11 sensor** measures **temperature and humidity** to monitor growing conditions.
- A **solar panel** charges the battery during the day, with help from a **charger protector**, **booster**, and **capacitor** for safe power delivery.

---

## 🧰 Hardware Used

- Arduino Uno (or compatible board)
- Breadboard
- Jumper wires
- **Soil Moisture Sensor**
- **MOSFET** (for switching the water pump)
- **DHT11 Temperature & Humidity Sensor**
- **Photoresistor (LDR)** – for light detection
- **LEDs** (for nighttime lighting)
- **Solar Panel**
- **Battery**
- **Booster module** (to step up voltage)
- **Charger Protector** (to safely manage charging)
- **Capacitor** (to stabilize voltage and prevent brownouts)

---

## 🖥️ Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)

---

## ⚙️ Features

- 🌿 **Auto-waters plants** when soil moisture is low
- 🌡️ **Reads temperature and humidity**
- ☀️ **Detects light level** with photoresistor
- 💡 **Turns on LEDs** automatically in darkness
- 🔋 **Charges battery** with a solar panel
- ⚡ **Power-safe circuit** using a capacitor, a booster, and a charger protector

---

## 💡 Notes

- The **soil moisture threshold** and **light level threshold** can be adjusted in the Arduino code.
- Make sure the **solar panel** is exposed to enough sunlight for effective charging.
- LEDs can be upgraded with grow lights for better indoor support.

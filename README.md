# HC-SR04 Spotify Controller

Turn an **ultrasonic distance sensor** into a wireless volume controller (and macro input) for Spotify.

## How it works

1. An **HC-SR04 ultrasonic sensor** measures the distance in front of it (up to ~4 m with ~1 cm resolution).
2. An **ESP-32** reads the sensor and streams the measurement over **UDP**.
3. A **Python server** receives the UDP data and controls Spotify through the **Spotify Web API**.
4. The ESP-32 also presents itself as a **Bluetooth keyboard**, so the sensor doubles as a macro input device.

## Repository layout

- `src/` — ESP-32 firmware (`main.cpp`, `net.cpp`, `net.h`)
- `PythonServer/` — Python UDP receiver + Spotify API control (`main.py`)
- `platformio.ini` — PlatformIO project configuration
- `test/`, `include/`, `lib/` — firmware tests and support code

## Getting started

```sh
# Firmware (PlatformIO)
pio run -t upload

# Server
python PythonServer/main.py
```
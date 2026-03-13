# ESP-32 Robot with Obstacle Avoidance

An autonomous robot platform based on ESP32 with dual motor control and ultrasonic obstacle detection.

## Hardware Components

- ESP32 (esp32dev board)
- Dual motor driver (L298N, TB6612, or compatible)
- 2x DC motors with wheels
- HC-SR04 ultrasonic sensor
- Robot chassis kit
- Power supply (7-12V for motors, 5V for ESP32)

## GPIO Pin Configuration

See `include/pin_config.h` for complete pin definitions.

| Component    | Signal  | ESP32 Pin | Notes                        |
|--------------|---------|-----------|------------------------------|
| Motor A      | ENA     | GPIO13    | PWM speed control (ch0)      |
| Motor A      | IN1     | GPIO12    | Direction                    |
| Motor A      | IN2     | GPIO14    | Direction                    |
| Motor B      | ENB     | GPIO27    | PWM speed control (ch1)      |
| Motor B      | IN3     | GPIO26    | Direction                    |
| Motor B      | IN4     | GPIO25    | Direction                    |
| Ultrasonic   | TRIG    | GPIO23    | 5V tolerant                |
| Ultrasonic   | ECHO    | GPIO22    | Requires voltage divider     |

**Important:** ECHO pin needs a voltage divider (2.2kΩ + 3.3kΩ) to convert 5V→3.3V.

## Project Structure

```
esp-robot/
├── include/           # Header files
│   └── pin_config.h   # Pin definitions and hardware constants
├── src/              # Source code (to be implemented)
│   ├── main.cpp
│   ├── Motor.cpp/h
│   ├── MotorDriver.cpp/h
│   ├── UltrasonicSensor.cpp/h
│   ├── ObstacleDetector.cpp/h
│   ├── Robot.cpp/h
│   └── calibration.cpp
├── lib/              # Custom libraries (to be implemented)
├── test/             # Unit tests (to be implemented)
├── platformio.ini    # PlatformIO configuration
├── TASKS.md          # Development progress
└── README.md         # This file
```

## Current Progress

✅ **Phase 1 Complete:**
- [x] GPIO pin assignments defined in `include/pin_config.h`
- [ ] PlatformIO project initialization
- [ ] Directory structure and skeleton files
- [ ] Serial monitor configuration and blink test

## Setup Instructions

1. Install PlatformIO (VS Code extension or CLI)
2. Create `platformio.ini` configured for `esp32dev` board
3. Install required libraries: `NewPing`, `PID`, `AsyncTCP` (optional)
4. Connect hardware according to pin configuration
5. Build and flash to ESP32

## Motor Control

- PWM frequency: 5kHz
- Speed range: 0-255 (8-bit)
- Direction control via two GPIO pins per motor
- Functionality: forward, backward, stop, brake, variable speed

## Ultrasonic Sensor

- Model: HC-SR04
- Max range: 500cm (5m)
- Trigger pulse: 10µs
- Timeout: 30ms
- Pin TRIG: output (5V compatible)
- Pin ECHO: input with 5V→3.3V voltage divider

## Obstacle Avoidance

The robot implements a simple state machine with avoidance behavior:
1. Drive forward continuously
2. Monitor front distance
3. If obstacle within 50cm: stop, reverse 0.5s, pivot turn 90°
4. Resume driving

## Power Requirements

- ESP32: 5V via USB or regulator (draws ~200-500mA)
- Motors: 7-12V depending on motor specs (up to 1A each under load)
- Motor driver: Must match motor voltage, handle peak current

Recommended: Separate power for motors and logic to avoid noise.

## License

TBD

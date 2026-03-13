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
├── src/              # Source code
│   ├── main.cpp
│   ├── Motor.cpp/h          # Individual motor control via PWM
│   ├── MotorDriver.cpp/h    # Differential drive control
│   ├── UltrasonicSensor.cpp/h  # HC-SR04 interface
│   ├── ObstacleDetector.cpp/h   # Filtered distance reading
│   ├── SensorArray.cpp/h        # Multi-sensor management
│   ├── calibration.cpp/h       # Motor speed calibration
│   └── Robot.cpp/h             # Main state machine (planned)
├── test/             # Unit tests
│   ├── motor_test.cpp
│   └── sensor_test.cpp
├── platformio.ini    # PlatformIO configuration
├── TASKS.md          # Development progress
└── README.md         # This file
```

## Calibration Procedure

The motors may have different characteristics, so calibration is essential for accurate driving and turning.

### What Calibration Does

The calibration routine:
1. Tests the robot at multiple PWM values (50-255)
2. Measures actual speed (cm/s) for each motor
3. Stores a PWM-to-speed lookup table in NVS (non-volatile storage)
4. Enables speed compensation: you can request a speed in cm/s and the system converts to the correct PWM

### How to Run Calibration

1. Place the robot on a clear, level surface with a measured distance (default: 50cm)
2. Upload the code with calibration support
3. Open serial monitor at 115200 baud
4. Send the `cal` command (or trigger calibration mode via code)
5. Follow the prompts:
   - For each PWM test value, place robot at start marker
   - Press any key to start the test run
   - Robot will drive forward at the test PWM
   - Press any key when it reaches the end marker
   - The system records time and calculates speed
6. Repeat for both motors (left then right)
7. Data is automatically saved to NVS and persists across reboots

### Calibration Data Storage

Data is stored in ESP32 NVS under namespace `robot_cal` with key `cal_data`. To clear calibration:
- Send `cal_clear` command, OR
- Call `calibration.clear()` in code

### Using Calibrated Speeds

After calibration, use:
- `MotorDriver::driveForwardCalibrated(float speed_cm_s)` - drive at exact cm/s
- `MotorDriver::turnCalibrated(uint8_t baseSpeed, int16_t degrees)` - precise turns using speed-based differential
- `calibration.getPWMForSpeed('L' or 'R', desired_speed)` - manual PWM lookup

## Current Progress

✅ **Phase 1 Complete:**
- [x] GPIO pin assignments defined in `include/pin_config.h`
- [x] PlatformIO project initialization
- [x] Directory structure and skeleton files
- [x] Serial monitor configuration and blink test

✅ **Phase 2 Complete:**
- [x] Motor class with PWM speed control
- [x] MotorDriver class with movement methods
- [x] Unit tests for motor control
- [x] Calibration routine with NVS storage

✅ **Phase 3 Complete:**
- [x] UltrasonicSensor class (NewPing library)
- [x] ObstacleDetector with filtering and hysteresis
- [x] Multi-sensor support via SensorArray
- [x] Sensor tests (unit tests with mock ISonar interface)

⏳ **Phase 4 In Progress:**
- [ ] Robot state machine implementation
- [ ] Obstacle avoidance algorithm integration
- [ ] Serial remote control
- [ ] Debug dashboard
- [ ] Integration tests
- [ ] Final documentation

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

## Calibration Procedure

The motors may have different characteristics, so calibration is essential for accurate driving and turning.

### What Calibration Does

The calibration routine:
1. Tests the robot at multiple PWM values (50-255)
2. Measures actual speed (cm/s) for each motor
3. Stores a PWM-to-speed lookup table in NVS (non-volatile storage)
4. Enables speed compensation: you can request a speed in cm/s and the system converts to the correct PWM

### How to Run Calibration

1. Place the robot on a clear, level surface with a measured distance (default: 50cm)
2. Upload the code with calibration support
3. Open serial monitor at 115200 baud
4. Send the `cal` command (or trigger calibration mode via code)
5. Follow the prompts:
   - For each PWM test value, place robot at start marker
   - Press any key to start the test run
   - Robot will drive forward at the test PWM
   - Press any key when it reaches the end marker
   - The system records time and calculates speed
6. Repeat for both motors (left then right)
7. Data is automatically saved to NVS and persists across reboots

### Calibration Data Storage

Data is stored in ESP32 NVS under namespace `robot_cal` with key `cal_data`. To clear calibration:
- Send `cal_clear` command, OR
- Call `calibration.clear()` in code

### Using Calibrated Speeds

After calibration, use:
- `MotorDriver::driveForwardCalibrated(float speed_cm_s)` - drive at exact cm/s
- `MotorDriver::turnCalibrated(uint8_t baseSpeed, int16_t degrees)` - precise turns using speed-based differential
- `calibration.getPWMForSpeed('L' or 'R', desired_speed)` - manual PWM lookup

## Power Requirements

- ESP32: 5V via USB or regulator (draws ~200-500mA)
- Motors: 7-12V depending on motor specs (up to 1A each under load)
- Motor driver: Must match motor voltage, handle peak current

Recommended: Separate power for motors and logic to avoid noise.

## License

TBD

# esp-robot
**Mission:** create some code for an esp-32 controlled robot with 2 motors for wheels and an ultrasonic sensor. Just make best guesses for pins and whatnot, and just the c++ code should be enough for now

## Phase 1: Planning & Setup
- [x] Define ESP32 GPIO pin assignments for motor driver (ENA, IN1, IN2, ENB, IN3, IN4) and ultrasonic sensor (TRIG, ECHO). Document in `include/pin_config.h` with descriptive comments and voltage levels.
- [x] Initialize PlatformIO project with Arduino-ESP32 framework. Create `platformio.ini` configured for `esp32dev` board with required libraries: `NewPing`, `PID`, and `AsyncTCP` (optional for later WiFi).
- [x] Set up project directory structure: `src/` for main code, `include/` for headers, `lib/` for custom libraries, and `test/` for unit tests. Create basic skeleton files: `src/main.cpp`, `src/Motor.h`, `src/UltrasonicSensor.h`.
- [x] Configure serial monitor output at 115200 baud for debugging. Implement simple "blink" test to verify build environment and basic ESP32 functionality before adding peripherals.

## Phase 2: Motor Control Implementation
- [x] Implement `Motor` class in `src/Motor.cpp/h` using PWM (ledc) for speed control and GPIO for direction. Add methods: `begin()`, `setSpeed(uint8_t)`, `forward()`, `backward()`, `stop()`, `brake()`.
- [x] Implement `MotorDriver` class in `src/MotorDriver.cpp/h` that encapsulates two `Motor` instances. Provide high-level movement methods: `driveForward(speed)`, `driveBackward(speed)`, `turn(speed, angle)`, `pivotTurn(speed)`.
- [x] Write unit tests in `test/motor_test.cpp` that verify PWM frequency/duty cycle configuration and motor state changes using mock pins or simulated outputs.
- [ ] Create calibration routine in `src/calibration.cpp` that measures actual motor speeds and generates PWM-to-speed mapping. Store results in non-volatile storage (NVS) or config file.

## Phase 3: Ultrasonic Sensor Integration
- [x] Implement `UltrasonicSensor` class in `src/UltrasonicSensor.cpp/h` using NewPing library. Add `begin()`, `ping()`, `getDistanceCM()`, `getDistanceInches()`. Set timeout to 30ms for 5m max range.
- [ ] Create `ObstacleDetector` class in `src/ObstacleDetector.cpp/h` that reads ultrasonic sensor and implements filtering (moving average) and threshold comparison with hysteresis to avoid false positives.
- [ ] Add support for multiple sensors: implement daisy-chained or multiplexed reading if using additional front/side sensors. Define sensor positions (front-left, front-right, rear).
- [ ] Write test in `test/sensor_test.cpp` that simulates echo pulses and validates distance calculation accuracy, edge cases (no echo, too far), and filtering behavior.

## Phase 4: Main Logic & Integration
- [ ] Create main robot state machine in `src/Robot.cpp/h` with states: `IDLE`, `DRIVING`, `AVOIDING`, `STOPPED`. Implement state transitions based on sensor input and timeout.
- [ ] Implement obstacle avoidance algorithm: when front obstacle detected within safety threshold, stop, reverse for 0.5s, then pivot turn 90° away from obstacle, resume driving. Add random turn direction if symmetric.
- [ ] Add simple remote control via serial commands: 'w/a/s/d' for movement, 'q/e' for rotate, 'x' for stop. Implement command parser in `src/CommandParser.cpp/h` with safety interlock (only accept commands when in IDLE or override mode).
- [ ] Integrate all components in `main.cpp`: initialize motor driver, ultrasonic sensor, and robot controller. Add watchdog timer to reset system if any component hangs. Implement graceful shutdown on panic condition.
- [ ] Create debug dashboard: output state, distance readings, motor speeds, and battery voltage (if ADC pin assigned) to serial monitor at 1Hz. Add diagnostic commands to test individual components.
- [ ] Write final integration test `test/robot_integration_test.cpp` that simulates sensor readings and verifies correct state transitions and motor outputs using test doubles.
- [ ] Document entire system in `TASKS.md` and `README.md` with complete pinout diagram, power requirements (voltage/current estimates), assembly steps, and calibration procedure.

---
**Deliverable:** Complete C++ Arduino project ready for PlatformIO build and flash to ESP32dev board with functional obstacle-avoiding robot behavior.
```

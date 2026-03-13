/**
 * @file pin_config.h
 * @brief ESP32 GPIO pin assignments for robot controller
 *
 * This header defines all hardware pin configurations for the ESP32 robot platform.
 * Pins are chosen to be compatible with common motor drivers (L298N, TB6612, etc.)
 * and HC-SR04 ultrasonic sensors. All pins are 3.3V logic compatible.
 *
 * IMPORTANT: ESP32 pins 34-39 are input-only and cannot be used for PWM output.
 * Pins 0, 2, 4, 12, 15 are strapping pins - avoid unless you understand boot implications.
 *
 * @author esp-robot
 * @version 1.0
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>
#include <cstdint>

/* ============================================================================
 *  MOTOR DRIVER CONFIGURATION
 * ============================================================================ */

/**
 * @brief Left motor driver pins (Motor A)
 *
 * Using L298N/TB6612-style driver with PWM speed control and 2 direction pins.
 * PWM frequency: 5kHz (adjustable in Motor class)
 * Voltage levels: 3.3V logic from ESP32 to driver inputs.
 * Driver should be powered separately (7-12V typical) from motor supply.
 */
#define LEFT_MOTOR_ENA_PIN    13   // PWM capable (LEDC channel 0)
#define LEFT_MOTOR_IN1_PIN    12   // Direction control 1
#define LEFT_MOTOR_IN2_PIN    14   // Direction control 2

/**
 * @brief Right motor driver pins (Motor B)
 *
 * Independent motor control for differential steering.
 * PWM frequency: 5kHz (adjustable in Motor class)
 */
#define RIGHT_MOTOR_ENB_PIN   27   // PWM capable (LEDC channel 1)
#define RIGHT_MOTOR_IN3_PIN   26   // Direction control 1
#define RIGHT_MOTOR_IN4_PIN   25   // Direction control 2

/* ============================================================================
 *  ULTRASONIC SENSOR CONFIGURATION
 * ============================================================================ */

/**
 * @brief HC-SR04 Ultrasonic sensor pins
 *
 * HC-SR04 operates at 5V, but trigger pin is 5V-tolerant on ESP32.
 * ECHO pin outputs 5V signal - requires voltage divider (2.2k + 3.3k resistors)
 * to drop to 3.3V for ESP32 GPIO safety.
 *
 * Max range: 400cm (5m theoretical, 4m practical)
 * Timing: 10us trigger pulse, echo pulse width proportional to distance
 */
#define ULTRASONIC_TRIG_PIN   23   // Output - 5V compatible
#define ULTRASONIC_ECHO_PIN   22   // Input - requires voltage divider to 3.3V

/* ============================================================================
 *  OPTIONAL HARDWARE (for future expansion)
 * ============================================================================ */

/**
 * @brief Battery monitoring (optional)
 *
 * If using voltage divider to monitor battery level, connect to ADC1 channel.
 * ESP32 ADC reference: ~3.3V (1100mV typical when using internal attenuator)
 * Note: ADC pins are single-use only. Choose any ADC1 channel (0-7).
 */
#define BATTERY_ADC_PIN       A0   // Not currently used, 3.3V analog input

/**
 * @brief Status LED (optional)
 *
 * On-board LED for visual status indication (boot confirmation, errors, etc.)
 */
#define STATUS_LED_PIN        2    // Built-in LED on many ESP32 boards

/* ============================================================================
 *  HARDWARE CONSTANTS
 * ============================================================================ */

/**
 * @brief Motor driver and motor specifications
 *
 * These values may need calibration based on actual hardware.
 * PWM frequency: 5kHz (20ms period assumption for ESC/driver compatibility)
 * Max speed: 255 (8-bit PWM)
 */
constexpr uint32_t MOTOR_PWM_FREQ_HZ = 5000;      // 5kHz PWM
constexpr uint8_t  MOTOR_PWM_RESOLUTION = 8;      // 8-bit (0-255)
constexpr uint8_t  MOTOR_MAX_SPEED = 255;         // 100% duty cycle
constexpr uint8_t  MOTOR_MIN_SPEED = 50;          // Minimum effective speed

/**
 * @brief Ultrasonic sensor timing parameters
 *
 * Sound speed: ~343 m/s at 20°C
 * Timeout for 5m range: 5m * 2 / 343 m/s = ~29ms, use 30ms for safety
 */
constexpr uint16_t ULTRASONIC_MAX_DISTANCE_CM = 500;    // 5 meters max
constexpr uint16_t ULTRASONIC_TIMEOUT_US = 30000;       // 30ms timeout

/**
 * @brief Safety thresholds (millimeters)
 *
 * Obstacle detection thresholds for different behaviors.
 * These should be tuned based on robot size and turning radius.
 */
constexpr uint16_t SAFETY_STOP_DISTANCE_MM = 300;      // Stop immediately
constexpr uint16_t SAFETY_AVOID_DISTANCE_MM = 500;     // Start avoidance
constexpr uint16_t SAFETY_NAVIGATION_DISTANCE_MM = 800; // Normal driving

#endif // PIN_CONFIG_H

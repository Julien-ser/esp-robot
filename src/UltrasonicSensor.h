#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>
#include <NewPing.h>

/**
 * @brief Ultrasonic sensor class using HC-SR04 with NewPing library
 *
 * Provides distance measurements in centimeters and inches.
 * Uses non-blocking ping method for better timing control.
 */
class UltrasonicSensor {
private:
    NewPing _sonar;             // NewPing instance
    uint16_t _maxDistance;      // Maximum distance in cm
    uint16_t _timeout;           // Timeout in microseconds

public:
    /**
     * @brief Construct a new Ultrasonic Sensor object
     *
     * @param triggerPin GPIO pin for TRIG (output)
     * @param echoPin GPIO pin for ECHO (input)
     * @param maxDistance Maximum distance in cm (default 500)
     * @param timeout Timeout in microseconds (default 30000 for 5m)
     */
    UltrasonicSensor(uint8_t triggerPin, uint8_t echoPin,
                     uint16_t maxDistance = 500, uint16_t timeout = 30000);

    /**
     * @brief Initialize the ultrasonic sensor
     */
    void begin();

    /**
     * @brief Send a ping and get distance in centimeters
     *
     * @return uint16_t Distance in cm, 0 if no echo or out of range
     */
    uint16_t ping();

    /**
     * @brief Get the last distance measurement in centimeters
     *
     * @return uint16_t Distance in cm
     */
    uint16_t getDistanceCM();

    /**
     * @brief Get the last distance measurement in inches
     *
     * @return uint16_t Distance in inches
     */
    uint16_t getDistanceInches();
};

#endif // ULTRASONIC_SENSOR_H

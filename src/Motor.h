#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

/**
 * @brief Motor class for controlling a single DC motor via PWM and direction pins
 *
 * Uses ESP32 LEDC (LED PWM Controller) for speed control and GPIO for direction.
 * Supports forward, backward, stop, and brake operations.
 */
class Motor {
private:
    int _enaPin;      // PWM pin (ENA)
    int _in1Pin;      // Direction pin 1
    int _in2Pin;      // Direction pin 2
    int _channel;     // LEDC channel (0-15)
    int _freqHz;      // PWM frequency
    int _resolution;  // PWM resolution (bits)
    uint8_t _currentSpeed;   // Last commanded speed (0-255)

    void _setDirection(bool forward);

public:
    /**
     * @brief Construct a new Motor object
     *
     * @param enaPin PWM enable pin
     * @param in1Pin Direction control pin 1
     * @param in2Pin Direction control pin 2
     * @param channel LEDC channel (0-15)
     * @param freqHz PWM frequency in Hz (default 2000)
     * @param resolution PWM resolution in bits (default 8, range 0-255)
     */
    Motor(int enaPin, int in1Pin, int in2Pin, int channel, int freqHz = 2000, int resolution = 8);

    /**
     * @brief Initialize motor pins and LEDC timer
     */
    void begin();

    /**
     * @brief Set motor speed (PWM duty cycle)
     *
     * @param speed Speed value (0-255). 0 = stop, 255 = full speed.
     */
    void setSpeed(uint8_t speed);

    /**
     * @brief Get the last set motor speed
     *
     * @return uint8_t Current speed (0-255)
     */
    uint8_t getSpeed() const;

    /**
     * @brief Drive motor forward
     */
    void forward();

    /**
     * @brief Drive motor backward
     */
    void backward();

    /**
     * @brief Stop motor (coast to stop)
     */
    void stop();

    /**
     * @brief Brake motor (active low to stop quickly)
     */
    void brake();
};

#endif // MOTOR_H
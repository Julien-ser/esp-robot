#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "Motor.h"

/**
 * @brief Motor driver class for differential drive robot
 *
 * Encapsulates two Motor instances and provides high-level
 * movement methods for robot control.
 */
class MotorDriver {
private:
    Motor _leftMotor;    // Left motor
    Motor _rightMotor;   // Right motor

public:
    /**
     * @brief Construct a new Motor Driver object
     *
     * @param leftMotor Left motor instance
     * @param rightMotor Right motor instance
     */
    MotorDriver(Motor leftMotor, Motor rightMotor);

    /**
     * @brief Initialize both motors
     */
    void begin();

    /**
     * @brief Drive forward at specified speed
     *
     * @param speed Speed (0-255)
     */
    void driveForward(uint8_t speed);

    /**
     * @brief Drive backward at specified speed
     *
     * @param speed Speed (0-255)
     */
    void driveBackward(uint8_t speed);

    /**
     * @brief Turn by specifying speed and angle
     *
     * Implements a turn by differentially controlling motors.
     * Positive angle = turn right, negative = turn left.
     *
     * @param speed Base speed (0-255)
     * @param degrees Turn angle in degrees (positive/negative)
     */
    void turn(uint8_t speed, int16_t degrees);

    /**
     * @brief Pivot turn in place (rotate left or right)
     *
     * One motor forward, other backward = rotate in place.
     *
     * @param speed Speed for outer motor (0-255)
     * @param clockwise true = clockwise, false = counter-clockwise
     */
    void pivotTurn(uint8_t speed, bool clockwise);

    /**
     * @brief Stop both motors (coast to stop)
     */
    void stop();

    /**
     * @brief Apply brake to both motors
     */
    void brake();
};

#endif // MOTOR_DRIVER_H

#ifndef IMOTOR_DRIVER_H
#define IMOTOR_DRIVER_H

#include <stdint.h>

/**
 * @brief Interface for motor driver control
 *
 * Defines the contract for motor driver implementations,
 * enabling test doubles and dependency injection.
 */
class IMotorDriver {
public:
    virtual ~IMotorDriver() = default;

    virtual void begin() = 0;
    virtual void driveForward(uint8_t speed) = 0;
    virtual void driveBackward(uint8_t speed) = 0;
    virtual void turn(uint8_t speed, int16_t degrees) = 0;
    virtual void pivotTurn(uint8_t speed, bool clockwise) = 0;
    virtual void stop() = 0;
    virtual void brake() = 0;
    virtual uint8_t getLeftSpeed() const = 0;
    virtual uint8_t getRightSpeed() const = 0;
};

#endif // IMOTOR_DRIVER_H

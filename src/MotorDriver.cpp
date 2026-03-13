#include "MotorDriver.h"
#include "pin_config.h"

MotorDriver::MotorDriver(Motor leftMotor, Motor rightMotor)
    : _leftMotor(leftMotor), _rightMotor(rightMotor) {
}

void MotorDriver::begin() {
    _leftMotor.begin();
    _rightMotor.begin();
}

void MotorDriver::driveForward(uint8_t speed) {
    _leftMotor.setSpeed(speed);
    _rightMotor.setSpeed(speed);
    _leftMotor.forward();
    _rightMotor.forward();
}

void MotorDriver::driveBackward(uint8_t speed) {
    _leftMotor.setSpeed(speed);
    _rightMotor.setSpeed(speed);
    _leftMotor.backward();
    _rightMotor.backward();
}

void MotorDriver::turn(uint8_t speed, int16_t degrees) {
    // Simple differential turn: slower inner motor, normal outer motor
    // This is a basic implementation - for precise angle control,
    // you'd integrate wheel encoders or use PID (task 14)

    if (degrees > 0) {
        // Turn right: left motor faster, right motor slower or reverse
        _leftMotor.setSpeed(speed);
        _rightMotor.setSpeed(speed / 2);  // Inner wheel slower
        _leftMotor.forward();
        _rightMotor.forward();
    } else if (degrees < 0) {
        // Turn left: right motor faster, left motor slower
        _rightMotor.setSpeed(speed);
        _leftMotor.setSpeed(speed / 2);   // Inner wheel slower
        _rightMotor.forward();
        _leftMotor.forward();
    } else {
        // No turn (shouldn't happen)
        driveForward(speed);
    }
}

void MotorDriver::pivotTurn(uint8_t speed, bool clockwise) {
    if (clockwise) {
        // Pivot clockwise: left forward, right backward
        _leftMotor.setSpeed(speed);
        _rightMotor.setSpeed(speed);
        _leftMotor.forward();
        _rightMotor.backward();
    } else {
        // Pivot counter-clockwise: left backward, right forward
        _leftMotor.setSpeed(speed);
        _rightMotor.setSpeed(speed);
        _leftMotor.backward();
        _rightMotor.forward();
    }
}

void MotorDriver::stop() {
    _leftMotor.stop();
    _rightMotor.stop();
}

void MotorDriver::brake() {
    _leftMotor.brake();
    _rightMotor.brake();
}

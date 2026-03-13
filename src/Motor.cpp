#include "Motor.h"
#include "pin_config.h"

/* ============================================================================
 *  PRIVATE METHODS
 * ============================================================================ */

void Motor::_setDirection(bool forward) {
    if (forward) {
        digitalWrite(_in1Pin, HIGH);
        digitalWrite(_in2Pin, LOW);
    } else {
        digitalWrite(_in1Pin, LOW);
        digitalWrite(_in2Pin, HIGH);
    }
}

/* ============================================================================
 *  PUBLIC METHODS
 * ============================================================================ */

Motor::Motor(int enaPin, int in1Pin, int in2Pin, int channel, int freqHz, int resolution)
    : _enaPin(enaPin), _in1Pin(in1Pin), _in2Pin(in2Pin), _channel(channel),
      _freqHz(freqHz), _resolution(resolution) {
}

void Motor::begin() {
    // Initialize PWM pin
    ledcSetup(_channel, _freqHz, _resolution);
    ledcAttachPin(_enaPin, _channel);

    // Initialize direction pins
    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);

    // Initialize motor stopped
    stop();
}

void Motor::setSpeed(uint8_t speed) {
    // Clamp speed to valid range
    speed = constrain(speed, 0, MOTOR_MAX_SPEED);
    ledcWrite(_channel, speed);
}

void Motor::forward() {
    _setDirection(true);
}

void Motor::backward() {
    _setDirection(false);
}

void Motor::stop() {
    // Coast to stop (PIM=0)
    ledcWrite(_channel, 0);
    // Set both direction pins low
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, LOW);
}

void Motor::brake() {
    // Active brake (both direction pins high)
    ledcWrite(_channel, 0);
    digitalWrite(_in1Pin, HIGH);
    digitalWrite(_in2Pin, HIGH);
}

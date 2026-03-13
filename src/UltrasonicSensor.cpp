#include "UltrasonicSensor.h"
#include "NewPingAdapter.h"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(uint8_t triggerPin, uint8_t echoPin,
                                   uint16_t maxDistance, uint16_t timeout)
    : _maxDistance(maxDistance), _timeout(timeout), _ownsSonar(true) {
    _sonar = new NewPingAdapter(triggerPin, echoPin, maxDistance);
}

UltrasonicSensor::UltrasonicSensor(ISonar* sonar, uint16_t timeout)
    : _sonar(sonar), _maxDistance(0), _timeout(timeout), _ownsSonar(false) {
}

UltrasonicSensor::~UltrasonicSensor() {
    if (_ownsSonar && _sonar != nullptr) {
        delete _sonar;
    }
}

void UltrasonicSensor::begin() {
    // NewPing doesn't require explicit initialization, but we can add delay if needed
    // For HC-SR04, a brief delay after power-up is recommended
    delay(50);  // Wait for sensor to stabilize
}

uint16_t UltrasonicSensor::ping() {
    return _sonar->ping();
}

uint16_t UltrasonicSensor::getDistanceMM() {
    return _sonar->convert_cm(ping()) * 10;
}

uint16_t UltrasonicSensor::getDistanceCM() {
    return _sonar->convert_cm(ping());
}

uint16_t UltrasonicSensor::getDistanceInches() {
    return _sonar->convert_in(ping());
}

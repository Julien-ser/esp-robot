#include "UltrasonicSensor.h"
#include <NewPing.h>

UltrasonicSensor::UltrasonicSensor(uint8_t triggerPin, uint8_t echoPin,
                                   uint16_t maxDistance, uint16_t timeout)
    : _sonar(triggerPin, echoPin, maxDistance), _maxDistance(maxDistance), _timeout(timeout) {
}

void UltrasonicSensor::begin() {
    // NewPing doesn't require explicit initialization, but we can add delay if needed
    // For HC-SR04, a brief delay after power-up is recommended
    delay(50);  // Wait for sensor to stabilize
}

uint16_t UltrasonicSensor::ping() {
    return _sonar.ping();
}

uint16_t UltrasonicSensor::getDistanceCM() {
    return _sonar.convert_cm(ping());
}

uint16_t UltrasonicSensor::getDistanceInches() {
    return _sonar.convert_in(ping());
}

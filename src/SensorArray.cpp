#include "SensorArray.h"
#include <Arduino.h>
#include <algorithm>

SensorArray::SensorArray() {
    // Initialize all pointers to nullptr
    for (int i = 0; i < static_cast<int>(SensorPosition::MAX); i++) {
        _detectors[i] = nullptr;
        _sensors[i] = nullptr;
    }
}

SensorArray::~SensorArray() {
    // Clean up allocated sensors and detectors
    for (int i = 0; i < static_cast<int>(SensorPosition::MAX); i++) {
        if (_detectors[i] != nullptr) {
            delete _detectors[i];
        }
        if (_sensors[i] != nullptr) {
            delete _sensors[i];
        }
    }
}

void SensorArray::begin() {
    for (int i = 0; i < static_cast<int>(SensorPosition::MAX); i++) {
        if (_detectors[i] != nullptr) {
            _detectors[i]->begin();
        }
    }
}

void SensorArray::update() {
    for (int i = 0; i < static_cast<int>(SensorPosition::MAX); i++) {
        if (_detectors[i] != nullptr) {
            _detectors[i]->update();
        }
    }
}

uint16_t SensorArray::getDistanceMM(SensorPosition position) {
    int idx = static_cast<int>(position);
    if (idx >= 0 && idx < static_cast<int>(SensorPosition::MAX) && _detectors[idx] != nullptr) {
        return _detectors[idx]->getDistanceMM();
    }
    return 0;  // No sensor or invalid position
}

bool SensorArray::isObstacleDetected(SensorPosition position) {
    int idx = static_cast<int>(position);
    if (idx >= 0 && idx < static_cast<int>(SensorPosition::MAX) && _detectors[idx] != nullptr) {
        return _detectors[idx]->isObstacleDetected();
    }
    return false;
}

uint16_t SensorArray::getNearestDistance() {
    uint16_t minDist = 9999;  // Large default
    for (int i = 0; i < static_cast<int>(SensorPosition::MAX); i++) {
        if (_detectors[i] != nullptr) {
            uint16_t dist = _detectors[i]->getDistanceMM();
            if (dist > 0 && dist < minDist) {
                minDist = dist;
            }
        }
    }
    return (minDist == 9999) ? 0 : minDist;
}

bool SensorArray::anyObstacleDetected() {
    for (int i = 0; i < static_cast<int>(SensorPosition::MAX); i++) {
        if (_detectors[i] != nullptr && _detectors[i]->isObstacleDetected()) {
            return true;
        }
    }
    return false;
}

void SensorArray::addSensor(SensorPosition position, ObstacleDetector* detector, UltrasonicSensor* sensor) {
    int idx = static_cast<int>(position);
    if (idx >= 0 && idx < static_cast<int>(SensorPosition::MAX)) {
        _detectors[idx] = detector;
        _sensors[idx] = sensor;
    }
}

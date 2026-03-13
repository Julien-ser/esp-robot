#include "ObstacleDetector.h"
#include <Arduino.h>

ObstacleDetector::ObstacleDetector(UltrasonicSensor& sensor,
                                   uint16_t filterSize,
                                   uint16_t warningThreshold,
                                   uint16_t dangerThreshold,
                                   uint16_t clearThreshold)
    : _sensor(sensor), _filterSize(filterSize),
      _warningThreshold(warningThreshold),
      _dangerThreshold(dangerThreshold),
      _clearThreshold(clearThreshold) {
    _samples = nullptr;
    _sampleIndex = 0;
    _filteredDistance = 0;
    _obstacleDetected = false;
}

ObstacleDetector::~ObstacleDetector() {
    if (_samples != nullptr) {
        delete[] _samples;
    }
}

void ObstacleDetector::begin() {
    // Allocate circular buffer for filter
    if (_samples == nullptr && _filterSize > 0) {
        _samples = new uint16_t[_filterSize];
        for (uint16_t i = 0; i < _filterSize; i++) {
            _samples[i] = 0;
        }
    }

    _sampleIndex = 0;
    _filteredDistance = 0;

    // Initialize underlying sensor
    _sensor.begin();

    // Take initial readings to fill filter
    for (uint16_t i = 0; i < _filterSize; i++) {
        uint16_t reading = _sensor.getDistanceMM();
        if (reading == 0) {
            reading = _sensor.getDistanceCM() * 10; // Convert cm to mm if needed
        }
        if (reading > 0) {
            _samples[i] = reading;
            _filteredDistance += reading;
        }
    }
    if (_filterSize > 0) {
        _filteredDistance /= _filterSize;
    }

    _obstacleDetected = (_filteredDistance > 0 && _filteredDistance <= _dangerThreshold);
}

void ObstacleDetector::update() {
    // Get new raw reading (in mm)
    uint16_t rawDistance = _sensor.getDistanceMM();
    if (rawDistance == 0) {
        uint16_t cm = _sensor.getDistanceCM();
        if (cm > 0) {
            rawDistance = cm * 10; // Convert cm to mm
        } else {
            rawDistance = 9999; // Error/no echo - treat as far
        }
    }

    // Update circular buffer for moving average
    if (_filterSize > 0 && _samples != nullptr) {
        // Subtract oldest sample from sum
        _filteredDistance -= _samples[_sampleIndex];
        // Store new sample
        _samples[_sampleIndex] = rawDistance;
        // Add new sample to sum
        _filteredDistance += rawDistance;
        // Advance index
        _sampleIndex = (_sampleIndex + 1) % _filterSize;
        // Compute average
        _filteredDistance = _filteredDistance / _filterSize;
    } else {
        _filteredDistance = rawDistance;
    }

    // Apply hysteresis for obstacle state
    if (_obstacleDetected) {
        // Currently detected: stay detected until distance exceeds clear threshold
        if (_filteredDistance > _clearThreshold) {
            _obstacleDetected = false;
        }
    } else {
        // Currently not detected: become detected if distance below danger threshold
        if (_filteredDistance > 0 && _filteredDistance <= _dangerThreshold) {
            _obstacleDetected = true;
        }
    }
}

uint16_t ObstacleDetector::getDistanceMM() const {
    return _filteredDistance;
}

bool ObstacleDetector::isObstacleDetected() const {
    return _obstacleDetected;
}

bool ObstacleDetector::isWarning() const {
    // Warning: distance between warning and danger thresholds, but not yet obstacle
    return (!_obstacleDetected && _filteredDistance > 0 &&
            _filteredDistance > _dangerThreshold && _filteredDistance <= _warningThreshold);
}

void ObstacleDetector::reset() {
    if (_samples != nullptr) {
        for (uint16_t i = 0; i < _filterSize; i++) {
            _samples[i] = 0;
        }
    }
    _sampleIndex = 0;
    _filteredDistance = 0;
    _obstacleDetected = false;
}

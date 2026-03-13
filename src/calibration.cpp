#include "calibration.h"
#include "MotorDriver.h"
#include "pin_config.h"
#include <Preferences.h>
#include <cmath>

// NVS storage constants
const char* Calibration::NVS_NAMESPACE = "robot_cal";
const char* Calibration::NVS_KEY = "cal_data";

Calibration::Calibration(MotorDriver& motorDriver, float distanceCm, uint8_t numPoints)
    : _motorDriver(motorDriver), _distanceCm(distanceCm), _numPoints(numPoints) {
    // Initialize calibration data
    _data.leftCount = 0;
    _data.rightCount = 0;
    _data.calibrated = false;
}

void Calibration::beginCalibration() {
    Serial.println("\n=== Motor Calibration Procedure ===");
    Serial.println("This will measure actual motor speeds at different PWM values.");
    Serial.printf("Place robot on a clear %0.1fcm path with distance markers.\n", _distanceCm);
    Serial.println("Press any key when ready to start...");
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read(); // Clear input

    runCalibrationTests();
    save();
    Serial.println("\nCalibration complete! Data saved to NVS.");
    printData();
}

bool Calibration::load() {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, true)) {
        Serial.println("Failed to open NVS namespace");
        return false;
    }

    size_t size = prefs.getBytesLength(NVS_KEY);
    if (size != sizeof(CalibrationData)) {
        Serial.println("Calibration data size mismatch or missing");
        prefs.end();
        return false;
    }

    if (!prefs.getBytes(NVS_KEY, &_data, sizeof(CalibrationData))) {
        Serial.println("Failed to read calibration data");
        prefs.end();
        return false;
    }

    prefs.end();
    return _data.calibrated;
}

bool Calibration::save() {
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, false)) {
        Serial.println("Failed to open NVS namespace for writing");
        return false;
    }

    if (!prefs.putBytes(NVS_KEY, &_data, sizeof(CalibrationData))) {
        Serial.println("Failed to write calibration data");
        prefs.end();
        return false;
    }

    prefs.end();
    return true;
}

uint8_t Calibration::getPWMForSpeed(char motor, float desiredSpeed) const {
    if (!_data.calibrated) {
        Serial.println("Warning: Not calibrated, returning default PWM");
        return (uint8_t)(desiredSpeed / 10.0); // Rough estimate
    }

    const MotorCalibration* points;
    uint8_t count;
    
    if (motor == 'L' || motor == 'l') {
        points = _data.leftMotor;
        count = _data.leftCount;
    } else if (motor == 'R' || motor == 'r') {
        points = _data.rightMotor;
        count = _data.rightCount;
    } else {
        return 0;
    }

    // Clamp desired speed to calibrated range
    float maxSpeed = getMaxSpeed(motor);
    if (desiredSpeed > maxSpeed) {
        desiredSpeed = maxSpeed;
    }
    if (desiredSpeed < 0) {
        desiredSpeed = 0;
    }

    return interpolate(points, count, desiredSpeed);
}

float Calibration::getMaxSpeed(char motor) const {
    const MotorCalibration* points;
    uint8_t count;
    
    if (motor == 'L' || motor == 'l') {
        points = _data.leftMotor;
        count = _data.leftCount;
    } else if (motor == 'R' || motor == 'r') {
        points = _data.rightMotor;
        count = _data.rightCount;
    } else {
        return 0;
    }

    if (count == 0) return 0;

    // Find max speed (typically at max PWM)
    float maxSpeed = 0;
    for (uint8_t i = 0; i < count; i++) {
        if (points[i].speed > maxSpeed) {
            maxSpeed = points[i].speed;
        }
    }
    return maxSpeed;
}

void Calibration::clear() {
    _data.leftCount = 0;
    _data.rightCount = 0;
    _data.calibrated = false;
    
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, false);
    prefs.remove(NVS_KEY);
    prefs.end();
}

void Calibration::printData() const {
    Serial.println("\n--- Calibration Data ---");
    
    Serial.println("Left Motor:");
    for (uint8_t i = 0; i < _data.leftCount; i++) {
        Serial.printf("  PWM=%3d -> Speed=%5.1f cm/s\n", 
                      _data.leftMotor[i].pwm, _data.leftMotor[i].speed);
    }
    
    Serial.println("Right Motor:");
    for (uint8_t i = 0; i < _data.rightCount; i++) {
        Serial.printf("  PWM=%3d -> Speed=%5.1f cm/s\n", 
                      _data.rightMotor[i].pwm, _data.rightMotor[i].speed);
    }
    
    Serial.printf("Max speed - Left: %.1f cm/s, Right: %.1f cm/s\n",
                  getMaxSpeed('L'), getMaxSpeed('R'));
    Serial.println("-----------------------\n");
}

float Calibration::measureSpeed(uint8_t pwm) {
    Serial.printf("\nTesting PWM=%d\n", pwm);
    Serial.println("Place robot at start line. Press any key to begin test...");
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read();

    // Reset position tracking (user manually marks start/end)
    Serial.println("Starting... timing begins when robot moves.");
    delay(2000); // Countdown

    unsigned long startTime = millis();
    _motorDriver.driveForward(pwm);

    Serial.println("Robot moving! Press any key when it reaches the end marker.");
    while (!Serial.available()) {
        delay(50);
    }
    unsigned long endTime = millis();
    _motorDriver.stop();
    Serial.read();

    // Calculate speed
    float timeSec = (endTime - startTime) / 1000.0;
    if (timeSec < 0.5) {
        Serial.println("Warning: Too fast! Increase distance or use lower PWM.");
        return 0;
    }

    float speed = _distanceCm / timeSec;
    Serial.printf("Time: %.2f s, Speed: %.1f cm/s\n", timeSec, speed);

    return speed;
}

std::vector<uint8_t> Calibration::generateTestPoints() const {
    std::vector<uint8_t> points;
    
    // Generate evenly spaced PWM values from 50 to 255
    // Skip very low PWM (0-50) as motors may not move reliably
    uint8_t minPWM = 50;
    uint8_t maxPWM = 255;
    uint8_t step = (maxPWM - minPWM) / (_numPoints - 1);
    
    for (uint8_t i = 0; i < _numPoints; i++) {
        points.push_back(minPWM + i * step);
    }
    
    // Ensure max PWM is included
    if (points.back() != maxPWM) {
        points.back() = maxPWM;
    }
    
    return points;
}

void Calibration::runCalibrationTests() {
    auto testPoints = generateTestPoints();
    
    Serial.println("\n--- Left Motor Calibration ---");
    _data.leftCount = _numPoints;
    
    for (uint8_t i = 0; i < _numPoints; i++) {
        uint8_t pwm = testPoints[i];
        _data.leftMotor[i].pwm = pwm;
        _data.leftMotor[i].speed = measureSpeed(pwm);
    }

    Serial.println("\n--- Right Motor Calibration ---");
    _data.rightCount = _numPoints;
    
    for (uint8_t i = 0; i < _numPoints; i++) {
        uint8_t pwm = testPoints[i];
        _data.rightMotor[i].pwm = pwm;
        _data.rightMotor[i].speed = measureSpeed(pwm);
    }

    _data.calibrated = true;
}

uint8_t Calibration::interpolate(const MotorCalibration* points, uint8_t count, float targetSpeed) const {
    if (count == 0) return 0;
    
    // Find surrounding calibration points
    if (targetSpeed <= points[0].speed) {
        return points[0].pwm;
    }
    
    for (uint8_t i = 0; i < count - 1; i++) {
        float speed1 = points[i].speed;
        float speed2 = points[i+1].speed;
        
        if (targetSpeed >= speed1 && targetSpeed <= speed2) {
            // Linear interpolation
            float t = (targetSpeed - speed1) / (speed2 - speed1);
            uint8_t pwm = points[i].pwm + t * (points[i+1].pwm - points[i].pwm);
            return (uint8_t)round(pwm);
        }
    }
    
    // If beyond last point, return max PWM
    return points[count-1].pwm;
}

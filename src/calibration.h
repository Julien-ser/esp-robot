#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <vector>

// Forward declaration to avoid including full MotorDriver.h
class MotorDriver;

// Calibration data structure for a single motor
struct MotorCalibration {
    uint8_t pwm;      // PWM value tested
    float speed;      // Measured speed in cm/s
};

// Full calibration data for both motors
struct CalibrationData {
    MotorCalibration leftMotor[10];   // Up to 10 calibration points per motor
    MotorCalibration rightMotor[10];
    uint8_t leftCount;               // Number of calibration points stored
    uint8_t rightCount;
    bool calibrated;                 // Flag indicating calibration is valid
};

class Calibration {
public:
    /**
     * @brief Construct a new Calibration object
     *
     * @param motorDriver Reference to motor driver for testing
     * @param distanceCm Distance to travel during calibration tests (in cm)
     * @param numPoints Number of calibration points to measure (max 10)
     */
    Calibration(class MotorDriver& motorDriver, float distanceCm = 50.0, uint8_t numPoints = 5);

    /**
     * @brief Begin calibration procedure
     * 
     * This will guide the user through calibration steps via serial.
     * Robot should be placed on a clear path with measured distance markers.
     */
    void beginCalibration();

    /**
     * @brief Load calibration data from NVS
     * @return true if calibration loaded successfully
     */
    bool load();

    /**
     * @brief Save calibration data to NVS
     * @return true if saved successfully
     */
    bool save();

    /**
     * @brief Check if calibration data exists and is valid
     */
    bool isCalibrated() const { return _data.calibrated; }

    /**
     * @brief Get calibrated PWM value for desired speed using interpolation
     * 
     * @param motor 'L' for left, 'R' for right
     * @param desiredSpeed Desired speed in cm/s
     * @return PWM value (0-255) that should produce the desired speed
     */
    uint8_t getPWMForSpeed(char motor, float desiredSpeed) const;

    /**
     * @brief Get maximum measured speed for a motor
     */
    float getMaxSpeed(char motor) const;

    /**
     * @brief Clear calibration data from NVS
     */
    void clear();

    /**
     * @brief Print calibration data to serial
     */
    void printData() const;

private:
    MotorDriver& _motorDriver;
    float _distanceCm;
    uint8_t _numPoints;
    CalibrationData _data;

    // NVS namespace and key
    static const char* NVS_NAMESPACE;
    static const char* NVS_KEY;

    // Helper to measure time to travel fixed distance
    float measureSpeed(uint8_t pwm);

    // Generate evenly spaced PWM test points (from min to max)
    std::vector<uint8_t> generateTestPoints() const;

    // Build calibration curve by testing each PWM value
    void runCalibrationTests();

    // Interpolate PWM value from calibration data
    uint8_t interpolate(const MotorCalibration* points, uint8_t count, float targetSpeed) const;
};

#endif // CALIBRATION_H

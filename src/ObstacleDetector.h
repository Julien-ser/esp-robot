#ifndef OBSTACLE_DETECTOR_H
#define OBSTACLE_DETECTOR_H

#include "IObstacleDetector.h"
#include "UltrasonicSensor.h"

/**
 * @brief Obstacle detector with filtering and thresholding
 *
 * Wraps an ultrasonic sensor with moving average filtering and
 * hysteresis-based threshold comparison to avoid false positives.
 * Implements IObstacleDetector interface for testability.
 */
class ObstacleDetector : public IObstacleDetector {
private:
    UltrasonicSensor& _sensor;   // Reference to ultrasonic sensor
    uint16_t _filterSize;       // Number of samples for moving average
    uint16_t* _samples;         // Circular buffer of distance samples
    uint16_t _sampleIndex;      // Current index in circular buffer
    uint16_t _filteredDistance; // Current filtered distance in mm
    bool _obstacleDetected;     // Current obstacle state

    // Thresholds (in mm)
    uint16_t _warningThreshold;  // Start warning
    uint16_t _dangerThreshold;   // Obstacle detected
    uint16_t _clearThreshold;    // Obstacle cleared

public:
    /**
     * @brief Construct a new Obstacle Detector object
     *
     * @param sensor Reference to UltrasonicSensor
     * @param filterSize Size of moving average filter (default 5)
     * @param warningThreshold Warning distance in mm (default 500)
     * @param dangerThreshold Obstacle detection threshold in mm (default 300)
     * @param clearThreshold Hysteresis clear threshold in mm (default 400)
     */
    ObstacleDetector(UltrasonicSensor& sensor,
                     uint16_t filterSize = 5,
                     uint16_t warningThreshold = 500,
                     uint16_t dangerThreshold = 300,
                     uint16_t clearThreshold = 400);

    ~ObstacleDetector();

    /**
     * @brief Initialize the detector (also initializes underlying sensor)
     */
    void begin() override;

    /**
     * @brief Update the detector - call periodically (e.g., every 50-100ms)
     *
     * Takes a new distance reading, updates filter, and evaluates thresholds.
     */
    void update() override;

    /**
     * @brief Get the current filtered distance in millimeters
     *
     * @return uint16_t Distance in mm (0 = no reading)
     */
    uint16_t getDistanceMM() const override;

    /**
     * @brief Check if obstacle is currently detected
     *
     * Uses hysteresis to avoid rapid state changes.
     *
     * @return true Obstacle within danger threshold
     * @return false No obstacle or only warning level
     */
    bool isObstacleDetected() const override;

    /**
     * @brief Check if we are in warning zone (not yet dangerous)
     *
     * @return true Distance between warning and danger thresholds
     * @return false Otherwise
     */
    bool isWarning() const override;

    /**
     * @brief Reset the filter and state
     */
    void reset() override;
};

#endif // OBSTACLE_DETECTOR_H

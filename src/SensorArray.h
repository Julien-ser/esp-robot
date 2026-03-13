#ifndef SENSOR_ARRAY_H
#define SENSOR_ARRAY_H

#include "ObstacleDetector.h"

/**
 * @brief Enumeration of sensor positions on the robot
 */
enum class SensorPosition {
    FRONT_LEFT,
    FRONT_RIGHT,
    REAR,
    MAX  // Sentinel value
};

/**
 * @brief Manages multiple ultrasonic sensors for obstacle detection
 *
 * Provides an array-based interface to manage multiple ObstacleDetector
 * instances at different positions around the robot.
 */
class SensorArray {
private:
    ObstacleDetector* _detectors[static_cast<int>(SensorPosition::MAX)];
    UltrasonicSensor* _sensors[static_cast<int>(SensorPosition::MAX)];

public:
    SensorArray();
    ~SensorArray();

    /**
     * @brief Initialize all sensors
     */
    void begin();

    /**
     * @brief Update all sensors (call periodically)
     */
    void update();

    /**
     * @brief Get distance from a specific sensor position in mm
     *
     * @param position Sensor position
     * @return uint16_t Distance in mm (0 if no reading)
     */
    uint16_t getDistanceMM(SensorPosition position);

    /**
     * @brief Check if obstacle detected at a specific position
     *
     * @param position Sensor position
     * @return true Obstacle detected
     * @return false No obstacle
     */
    bool isObstacleDetected(SensorPosition position);

    /**
     * @brief Get the nearest obstacle distance among all sensors
     *
     * @return uint16_t Minimum distance in mm
     */
    uint16_t getNearestDistance();

    /**
     * @brief Check if any sensor has detected an obstacle
     *
     * @return true Any obstacle detected
     * @return false No obstacles
     */
    bool anyObstacleDetected();

    /**
     * @brief Add a sensor to a specific position (optional)
     *
     * Allows dynamic configuration of sensors.
     *
     * @param position Sensor position
     * @param detector Pointer to ObstacleDetector
     * @param sensor Pointer to UltrasonicSensor (owned by detector)
     */
    void addSensor(SensorPosition position, ObstacleDetector* detector, UltrasonicSensor* sensor);
};

#endif // SENSOR_ARRAY_H

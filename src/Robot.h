#ifndef ROBOT_H
#define ROBOT_H

#include <stdint.h>
#include "MotorDriver.h"
#include "ObstacleDetector.h"

/**
 * @brief Robot state machine for autonomous obstacle-avoiding robot
 *
 * Manages robot states: IDLE, DRIVING, AVOIDING, STOPPED
 * Implements state transitions based on sensor input and timeouts.
 */
class Robot {
public:
    // Robot states
    enum class State {
        IDLE,       // Initial state, waiting for start command
        DRIVING,    // Normal forward driving
        AVOIDING,   // Avoiding an obstacle (maneuver in progress)
        STOPPED     // Emergency stop or fault condition
    };

private:
    MotorDriver& _motorDriver;      // Reference to motor driver
    ObstacleDetector& _detector;    // Reference to obstacle detector

    State _currentState;            // Current robot state
    State _previousState;           // Previous state (for debugging)

    unsigned long _stateStartTime;  // Time when current state started (millis)

    // Safety threshold for obstacle avoidance (mm)
    uint16_t _safetyThreshold;

    // Default driving speed (0-255)
    uint8_t _defaultSpeed;

    // Avoidance parameters
    uint16_t _reverseTimeMs;        // How long to reverse when obstacle found
    uint16_t _turnTimeMs;           // How long to pivot turn
    bool _lastTurnDirection;        // Remember turn direction to alternate

public:
    /**
     * @brief Construct a new Robot object
     *
     * @param motorDriver Reference to MotorDriver
     * @param detector Reference to ObstacleDetector
     * @param safetyThreshold Distance threshold for obstacle (mm, default 300)
     * @param defaultSpeed Default driving speed (0-255, default 150)
     */
    Robot(MotorDriver& motorDriver,
          ObstacleDetector& detector,
          uint16_t safetyThreshold = 300,
          uint8_t defaultSpeed = 150);

    /**
     * @brief Initialize robot components
     */
    void begin();

    /**
     * @brief Main update loop - call periodically (e.g., every 50-100ms)
     *
     * Implements state machine logic and transitions.
     */
    void update();

    /**
     * @brief Get current robot state
     *
     * @return Current state enum
     */
    State getState() const;

    /**
     * @brief Get time when current state started (milliseconds)
     *
     * @return unsigned long Timestamp of state entry
     */
    unsigned long getStateStartTime() const;

    /**
     * @brief Get human-readable state string
     *
     * @return const char* State name
     */
    const char* getStateString() const;

    /**
     * @brief Get left motor speed
     *
     * @return uint8_t Current speed (0-255)
     */
    uint8_t getLeftMotorSpeed() const;

    /**
     * @brief Get right motor speed
     *
     * @return uint8_t Current speed (0-255)
     */
    uint8_t getRightMotorSpeed() const;
};

#endif // ROBOT_H

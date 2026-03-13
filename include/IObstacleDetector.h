#ifndef IOBSTACLE_DETECTOR_H
#define IOBSTACLE_DETECTOR_H

#include <stdint.h>

/**
 * @brief Interface for obstacle detection
 *
 * Defines the contract for obstacle detector implementations,
 * enabling test doubles and dependency injection.
 */
class IObstacleDetector {
public:
    virtual ~IObstacleDetector() = default;

    virtual void begin() = 0;
    virtual void update() = 0;
    virtual uint16_t getDistanceMM() const = 0;
    virtual bool isObstacleDetected() const = 0;
    virtual bool isWarning() const = 0;
    virtual void reset() = 0;
};

#endif // IOBSTACLE_DETECTOR_H

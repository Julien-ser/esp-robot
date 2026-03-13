#include "Robot.h"
#include "Arduino.h"

// State name strings for debugging
static const char* STATE_NAMES[] = {
    "IDLE",
    "DRIVING",
    "AVOIDING",
    "STOPPED"
};

Robot::Robot(MotorDriver& motorDriver,
             ObstacleDetector& detector,
             uint16_t safetyThreshold,
             uint8_t defaultSpeed)
    : _motorDriver(motorDriver)
    , _detector(detector)
    , _currentState(State::IDLE)
    , _previousState(State::IDLE)
    , _stateStartTime(0)
    , _safetyThreshold(safetyThreshold)
    , _defaultSpeed(defaultSpeed)
    , _reverseTimeMs(500)       // Reverse for 500ms
    , _turnTimeMs(1000)         // Turn for 1000ms (90° pivot)
    , _lastTurnDirection(false) // Start turning right
{
}

void Robot::begin() {
    Serial.println("Robot: Initializing...");
    _motorDriver.begin();
    _detector.begin();
    _currentState = State::IDLE;
    _previousState = State::IDLE;
    _stateStartTime = millis();
    Serial.println("Robot: Ready (IDLE state)");
}

void Robot::update() {
    // Save previous state for debugging
    _previousState = _currentState;

    // Always update obstacle detector (must be called frequently)
    _detector.update();

    // State machine
    switch (_currentState) {
        case State::IDLE:
            // In IDLE, motors stopped, waiting for start command
            // Nothing to do - stay stopped
            break;

        case State::DRIVING:
            // Check for obstacles
            if (_detector.isObstacleDetected()) {
                Serial.println("Robot: Obstacle detected! Initiating avoidance...");
                _motorDriver.stop();
                _currentState = State::AVOIDING;
                _stateStartTime = millis();
                _lastTurnDirection = !_lastTurnDirection; // Alternate direction
            } else {
                // Continue driving forward
                _motorDriver.driveForward(_defaultSpeed);
            }
            break;

        case State::AVOIDING:
            {
                unsigned long elapsed = millis() - _stateStartTime;
                
                if (elapsed < _reverseTimeMs) {
                    // Phase 1: Reverse
                    _motorDriver.driveBackward(_defaultSpeed / 2); // Reverse slower
                } else if (elapsed < _reverseTimeMs + _turnTimeMs) {
                    // Phase 2: Pivot turn
                    // Alternate left/right for variety
                    _motorDriver.pivotTurn(_defaultSpeed, _lastTurnDirection);
                } else {
                    // Phase 3: Avoidance complete, resume driving
                    Serial.println("Robot: Avoidance complete, resuming...");
                    _motorDriver.driveForward(_defaultSpeed);
                    _currentState = State::DRIVING;
                    _stateStartTime = millis();
                }
            }
            break;

        case State::STOPPED:
            // Emergency stop - motors already braked
            // Stay here until reset() is called
            break;
    }
}

Robot::State Robot::getState() const {
    return _currentState;
}

unsigned long Robot::getStateStartTime() const {
    return _stateStartTime;
}

const char* Robot::getStateString() const {
    return STATE_NAMES[static_cast<int>(_currentState)];
}

uint8_t Robot::getLeftMotorSpeed() const {
    return _motorDriver.getLeftSpeed();
}

uint8_t Robot::getRightMotorSpeed() const {
    return _motorDriver.getRightSpeed();
}

void Robot::startDriving() {
    if (_currentState == State::IDLE) {
        Serial.println("Robot: Starting autonomous driving...");
        _currentState = State::DRIVING;
        _stateStartTime = millis();
        _motorDriver.driveForward(_defaultSpeed);
    } else {
        Serial.println("Robot: Cannot start - not in IDLE state");
    }
}

void Robot::stopRobot() {
    if (_currentState != State::STOPPED) {
        Serial.println("Robot: Stopping and returning to IDLE");
        _motorDriver.stop();
        _currentState = State::IDLE;
        _stateStartTime = millis();
    }
}

bool Robot::manualForward(uint8_t speed) {
    if (_currentState != State::IDLE) {
        Serial.println("Robot: manualForward ignored - not in IDLE");
        return false;
    }
    Serial.print("Robot: Manual forward, speed = ");
    Serial.println(speed);
    _motorDriver.driveForward(speed);
    return true;
}

bool Robot::manualBackward(uint8_t speed) {
    if (_currentState != State::IDLE) {
        Serial.println("Robot: manualBackward ignored - not in IDLE");
        return false;
    }
    Serial.print("Robot: Manual backward, speed = ");
    Serial.println(speed);
    _motorDriver.driveBackward(speed);
    return true;
}

bool Robot::manualTurn(uint8_t speed, int16_t degrees) {
    if (_currentState != State::IDLE) {
        Serial.println("Robot: manualTurn ignored - not in IDLE");
        return false;
    }
    Serial.print("Robot: Manual turn, speed = ");
    Serial.print(speed);
    Serial.print(", degrees = ");
    Serial.println(degrees);
    _motorDriver.turn(speed, degrees);
    return true;
}

bool Robot::manualPivot(uint8_t speed, bool clockwise) {
    if (_currentState != State::IDLE) {
        Serial.println("Robot: manualPivot ignored - not in IDLE");
        return false;
    }
    Serial.print("Robot: Manual pivot, speed = ");
    Serial.print(speed);
    Serial.print(", clockwise = ");
    Serial.println(clockwise ? "true" : "false");
    _motorDriver.pivotTurn(speed, clockwise);
    return true;
}

void Robot::emergencyStop() {
    if (_currentState != State::STOPPED) {
        Serial.println("Robot: EMERGENCY STOP!");
        _motorDriver.brake();
        _currentState = State::STOPPED;
        _stateStartTime = millis();
    }
}

bool Robot::isMoving() const {
    return (_currentState == State::DRIVING || _currentState == State::AVOIDING);
}

void Robot::reset() {
    if (_currentState == State::STOPPED) {
        Serial.println("Robot: Resetting from STOPPED to IDLE");
        _currentState = State::IDLE;
        _stateStartTime = millis();
    } else {
        Serial.println("Robot: Reset only valid from STOPPED state");
    }
}

#ifdef UNIT_TEST
void Robot::setStateStartTimeForTest(unsigned long time) {
    _stateStartTime = time;
}
#endif

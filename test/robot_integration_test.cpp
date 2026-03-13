#include "unity.h"
#include "Robot.h"
#include "IMotorDriver.h"
#include "IObstacleDetector.h"
#include "Arduino.h"

// ============================================================================
// Mock MotorDriver
// ============================================================================
class MockMotorDriver : public IMotorDriver {
public:
    enum Method {
        METHOD_NONE,
        METHOD_BEGIN,
        METHOD_DRIVE_FORWARD,
        METHOD_DRIVE_BACKWARD,
        METHOD_TURN,
        METHOD_PIVOT_TURN,
        METHOD_STOP,
        METHOD_BRAKE
    };

    struct Call {
        Method method;
        uint8_t speed;
        bool boolParam;
        int16_t intParam;
    };

    Call callHistory[32];
    uint8_t callCount;

    // Last call info
    Method lastMethod;
    uint8_t lastSpeed;
    bool lastBool;
    int16_t lastInt;

    // Motor speed state (for getters)
    uint8_t leftSpeed;
    uint8_t rightSpeed;

    MockMotorDriver()
        : callCount(0), lastMethod(METHOD_NONE), lastSpeed(0), lastBool(false),
          lastInt(0), leftSpeed(0), rightSpeed(0) {}

    void begin() override {
        recordCall(METHOD_BEGIN, 0, false, 0);
    }

    void driveForward(uint8_t speed) override {
        recordCall(METHOD_DRIVE_FORWARD, speed, false, 0);
        leftSpeed = rightSpeed = speed;
    }

    void driveBackward(uint8_t speed) override {
        recordCall(METHOD_DRIVE_BACKWARD, speed, false, 0);
        leftSpeed = rightSpeed = speed;
    }

    void turn(uint8_t speed, int16_t degrees) override {
        recordCall(METHOD_TURN, speed, false, degrees);
        // Approximate motor speeds for turn (used in manual control)
        if (degrees > 0) {
            leftSpeed = speed;
            rightSpeed = speed / 2;
        } else if (degrees < 0) {
            rightSpeed = speed;
            leftSpeed = speed / 2;
        } else {
            leftSpeed = rightSpeed = speed;
        }
    }

    void pivotTurn(uint8_t speed, bool clockwise) override {
        recordCall(METHOD_PIVOT_TURN, speed, clockwise, 0);
        leftSpeed = rightSpeed = speed;
    }

    void stop() override {
        recordCall(METHOD_STOP, 0, false, 0);
        leftSpeed = rightSpeed = 0;
    }

    void brake() override {
        recordCall(METHOD_BRAKE, 0, false, 0);
        leftSpeed = rightSpeed = 0;
    }

    uint8_t getLeftSpeed() const override { return leftSpeed; }
    uint8_t getRightSpeed() const override { return rightSpeed; }

    void recordCall(Method method, uint8_t speed, bool boolParam, int16_t intParam) {
        if (callCount < 32) {
            callHistory[callCount] = {method, speed, boolParam, intParam};
        }
        callCount++;
        lastMethod = method;
        lastSpeed = speed;
        lastBool = boolParam;
        lastInt = intParam;
    }

    void reset() {
        callCount = 0;
        lastMethod = METHOD_NONE;
        leftSpeed = rightSpeed = 0;
    }

    bool wasCalled(Method method) const {
        for (uint8_t i = 0; i < callCount; i++) {
            if (callHistory[i].method == method) return true;
        }
        return false;
    }

    uint8_t getCallCount(Method method) const {
        uint8_t count = 0;
        for (uint8_t i = 0; i < callCount; i++) {
            if (callHistory[i].method == method) count++;
        }
        return count;
    }

    bool getLastCallParams(Method method, uint8_t* speed, bool* boolParam, int16_t* intParam) const {
        for (int i = callCount - 1; i >= 0; i--) {
            if (callHistory[i].method == method) {
                if (speed) *speed = callHistory[i].speed;
                if (boolParam) *boolParam = callHistory[i].boolParam;
                if (intParam) *intParam = callHistory[i].intParam;
                return true;
            }
        }
        return false;
    }
};

// ============================================================================
// Mock ObstacleDetector
// ============================================================================
class MockObstacleDetector : public IObstacleDetector {
public:
    bool obstacleDetected;
    bool warning;

    MockObstacleDetector() : obstacleDetected(false), warning(false) {}

    void begin() override {}
    void update() override {}
    uint16_t getDistanceMM() const override { return 0; }
    bool isObstacleDetected() const override { return obstacleDetected; }
    bool isWarning() const override { return warning; }
    void reset() override {}

    void setObstacleDetected(bool detected) { obstacleDetected = detected; }
    void setWarning(bool warn) { warning = warn; }
};

// ============================================================================
// Test Cases
// ============================================================================

void test_initialState(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();

    TEST_ASSERT_EQUAL(Robot::State::IDLE, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_BEGIN));
}

void test_startDriving(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();
    robot.startDriving();

    TEST_ASSERT_EQUAL(Robot::State::DRIVING, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_FORWARD));

    uint8_t speed;
    bool ok = mockMotor.getLastCallParams(MockMotorDriver::METHOD_DRIVE_FORWARD, &speed, nullptr, nullptr);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(150, speed); // default speed

    TEST_ASSERT_EQUAL_UINT8(150, robot.getLeftMotorSpeed());
    TEST_ASSERT_EQUAL_UINT8(150, robot.getRightMotorSpeed());
}

void test_obstacleTriggersAvoidance(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();
    robot.startDriving();
    mockMotor.reset();

    mockDetector.setObstacleDetected(true);
    robot.update();

    TEST_ASSERT_EQUAL(Robot::State::AVOIDING, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_STOP));
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_BACKWARD));

    uint8_t speed;
    ok = mockMotor.getLastCallParams(MockMotorDriver::METHOD_DRIVE_BACKWARD, &speed, nullptr, nullptr);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(75, speed); // defaultSpeed/2

    TEST_ASSERT_EQUAL_UINT8(75, robot.getLeftMotorSpeed());
    TEST_ASSERT_EQUAL_UINT8(75, robot.getRightMotorSpeed());
}

void test_avoidanceSequence(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();
    robot.startDriving();
    mockMotor.reset();
    mockDetector.setObstacleDetected(true);

    // First update: enter AVOIDING, call stop and driveBackward immediately
    robot.update();
    TEST_ASSERT_EQUAL(Robot::State::AVOIDING, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_STOP));
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_BACKWARD));

    // Simulate time passing: after 600ms (elapsed > 500 reverse, < 500+1000 turn)
    unsigned long now = millis();
    robot.setStateStartTimeForTest(now - 600);
    mockMotor.reset();
    robot.update();

    // Should be in turn phase, call pivotTurn
    TEST_ASSERT_EQUAL(Robot::State::AVOIDING, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_PIVOT_TURN));

    uint8_t speed;
    bool clockwise;
    ok = mockMotor.getLastCallParams(MockMotorDriver::METHOD_PIVOT_TURN, &speed, &clockwise, nullptr);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(150, speed); // defaultSpeed
    TEST_ASSERT_TRUE(clockwise); // first avoidance toggles to true

    TEST_ASSERT_EQUAL_UINT8(150, robot.getLeftMotorSpeed());
    TEST_ASSERT_EQUAL_UINT8(150, robot.getRightMotorSpeed());

    // Simulate further: after 1600ms total (elapsed > 1500)
    robot.setStateStartTimeForTest(now - 1600);
    mockMotor.reset();
    robot.update();

    // Should transition to DRIVING and call driveForward
    TEST_ASSERT_EQUAL(Robot::State::DRIVING, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_FORWARD));
    ok = mockMotor.getLastCallParams(MockMotorDriver::METHOD_DRIVE_FORWARD, &speed, nullptr, nullptr);
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_UINT8(150, speed);

    TEST_ASSERT_EQUAL_UINT8(150, robot.getLeftMotorSpeed());
    TEST_ASSERT_EQUAL_UINT8(150, robot.getRightMotorSpeed());
}

void test_emergencyStop(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();
    robot.startDriving();
    mockMotor.reset();

    robot.emergencyStop();
    TEST_ASSERT_EQUAL(Robot::State::STOPPED, robot.getState());
    TEST_ASSERT_TRUE(mockMotor.wasCalled(MockMotorDriver::METHOD_BRAKE));
    TEST_ASSERT_EQUAL_UINT8(0, robot.getLeftMotorSpeed());
    TEST_ASSERT_EQUAL_UINT8(0, robot.getRightMotorSpeed());
}

void test_resetFromStopped(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();
    robot.emergencyStop();
    mockMotor.reset();

    robot.reset();
    TEST_ASSERT_EQUAL(Robot::State::IDLE, robot.getState());
    TEST_ASSERT_FALSE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_FORWARD));

    // Reset from non-STOPPED should do nothing
    robot.startDriving(); // go to DRIVING
    mockMotor.reset();
    robot.reset();
    TEST_ASSERT_EQUAL(Robot::State::DRIVING, robot.getState());
    TEST_ASSERT_FALSE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_FORWARD));
}

void test_manualCommandsRejectedWhenNotIdle(void) {
    MockMotorDriver mockMotor;
    MockObstacleDetector mockDetector;
    Robot robot(mockMotor, mockDetector);
    robot.begin();
    robot.startDriving(); // now in DRIVING

    bool result = robot.manualForward(200);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_FORWARD));

    result = robot.manualBackward(200);
    TEST_ASSERT_FALSE(result);
    result = robot.manualTurn(150, 45);
    TEST_ASSERT_FALSE(result);
    result = robot.manualPivot(150, true);
    TEST_ASSERT_FALSE(result);

    // Also test from STOPPED state
    robot.emergencyStop();
    mockMotor.reset();
    result = robot.manualForward(100);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(mockMotor.wasCalled(MockMotorDriver::METHOD_DRIVE_FORWARD));
}

// ============================================================================
// Test Runner
// ============================================================================

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initialState);
    RUN_TEST(test_startDriving);
    RUN_TEST(test_obstacleTriggersAvoidance);
    RUN_TEST(test_avoidanceSequence);
    RUN_TEST(test_emergencyStop);
    RUN_TEST(test_resetFromStopped);
    RUN_TEST(test_manualCommandsRejectedWhenNotIdle);
    return UNITY_END();
}

#ifdef ARDUINO
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; } // wait for serial monitor
    runUnityTests();
}

void loop() {
    // Nothing
}
#else
int main() {
    return runUnityTests();
}
#endif

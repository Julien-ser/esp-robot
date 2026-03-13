#include "ObstacleDetector.h"
#include "unity.h"

// Mock UltrasonicSensor for testing
class MockUltrasonicSensor {
private:
    uint16_t _distanceMM;

public:
    MockUltrasonicSensor() : _distanceMM(0) {}

    void setDistance(uint16_t distanceMM) {
        _distanceMM = distanceMM;
    }

    void begin() { }

    uint16_t ping() {
        return _distanceMM > 0 ? _distanceMM * 58 / 10 : 0; // Rough conversion to µs (optional)
    }

    uint16_t getDistanceMM() {
        return _distanceMM;
    }

    uint16_t getDistanceCM() {
        return _distanceMM / 10;
    }

    uint16_t getDistanceInches() {
        return _distanceMM / 25; // Approx
    }
};

// Test fixtures
static MockUltrasonicSensor mockSonar;
static ObstacleDetector* detector;

void setUp(void) {
    mockSonar = MockUltrasonicSensor();
    // Use default thresholds: danger=300mm, warning=500mm, clear=400mm, filter=5
    detector = new ObstacleDetector(mockSonar, 5, 500, 300, 400);
    detector->begin();
}

void tearDown(void) {
    if (detector != nullptr) {
        delete detector;
        detector = nullptr;
    }
}

void test_ObstacleDetector_initial_state(void) {
    // After begin with no readings, filtered distance should be 0
    TEST_ASSERT_EQUAL_UINT16(0, detector->getDistanceMM());
    TEST_ASSERT_FALSE(detector->isObstacleDetected());
}

void test_ObstacleDetector_no_obstacle(void) {
    // Set distance to 800mm (far)
    mockSonar.setDistance(800);
    detector->update();
    detector->update();
    detector->update();
    detector->update();
    detector->update(); // Fill filter

    uint16_t dist = detector->getDistanceMM();
    TEST_ASSERT_TRUE(dist > 700 && dist < 900);
    TEST_ASSERT_FALSE(detector->isObstacleDetected());
    TEST_ASSERT_FALSE(detector->isWarning());
}

void test_ObstacleDetector_warning_zone(void) {
    // Set distance to 450mm (between warning 500 and danger 300)
    mockSonar.setDistance(450);
    for (int i = 0; i < 5; i++) {
        detector->update();
    }

    TEST_ASSERT_TRUE(detector->isWarning());
    TEST_ASSERT_FALSE(detector->isObstacleDetected());
}

void test_ObstacleDetector_danger_obstacle(void) {
    // Set distance to 200mm (within danger threshold)
    mockSonar.setDistance(200);
    for (int i = 0; i < 5; i++) {
        detector->update();
    }

    TEST_ASSERT_TRUE(detector->isObstacleDetected());
    TEST_ASSERT_FALSE(detector->isWarning());
}

void test_ObstacleDetector_hysteresis(void) {
    // Start with obstacle at 200mm
    mockSonar.setDistance(200);
    for (int i = 0; i < 5; i++) {
        detector->update();
    }
    TEST_ASSERT_TRUE(detector->isObstacleDetected());

    // Increase distance to 350mm (between danger 300 and clear 400)
    // Should still be detected because hysteresis clear is 400
    mockSonar.setDistance(350);
    detector->update();
    TEST_ASSERT_TRUE(detector->isObstacleDetected());

    // Increase to 450mm (above clear threshold)
    mockSonar.setDistance(450);
    detector->update();
    TEST_ASSERT_FALSE(detector->isObstacleDetected());
}

void test_ObstacleDetector_filtering(void) {
    // Test that filter smooths out jitter
    mockSonar.setDistance(300);
    detector->update();
    uint16_t first = detector->getDistanceMM();

    mockSonar.setDistance(350);
    detector->update();
    uint16_t second = detector->getDistanceMM();

    // Second reading should be between 300 and 350 due to filtering
    TEST_ASSERT_TRUE(second >= first && second <= 350);
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ObstacleDetector_initial_state);
    RUN_TEST(test_ObstacleDetector_no_obstacle);
    RUN_TEST(test_ObstacleDetector_warning_zone);
    RUN_TEST(test_ObstacleDetector_danger_obstacle);
    RUN_TEST(test_ObstacleDetector_hysteresis);
    RUN_TEST(test_ObstacleDetector_filtering);
    return UNITY_END();
}

#ifdef ARDUINO
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    Serial.println("Running sensor tests...");
    runUnityTests();
}

void loop() { }
#else
int main() {
    return runUnityTests();
}
#endif

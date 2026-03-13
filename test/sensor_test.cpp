#include "UltrasonicSensor.h"
#include "ObstacleDetector.h"
#include "ISonar.h"
#include "unity.h"

// Mock implementation of ISonar for testing
class MockSonar : public ISonar {
private:
    uint16_t _pingMicroseconds;
public:
    MockSonar() : _pingMicroseconds(0) {}
    void setPingMicroseconds(uint16_t us) { _pingMicroseconds = us; }
    uint16_t ping() override { return _pingMicroseconds; }
    uint16_t convert_cm(uint16_t us) override { return us / 58; } // 58 µs per cm
    uint16_t convert_in(uint16_t us) override { return us / 148; } // 148 µs per inch
};

static MockSonar mockSonar;

// === UltrasonicSensor Unit Tests ===

void test_UltrasonicSensor_begin(void) {
    UltrasonicSensor sensor(&mockSonar);
    sensor.begin();
    TEST_ASSERT_TRUE(true);
}

void test_UltrasonicSensor_getDistanceCM(void) {
    UltrasonicSensor sensor(&mockSonar);
    sensor.begin();
    mockSonar.setPingMicroseconds(580); // 10 cm
    TEST_ASSERT_EQUAL_UINT16(10, sensor.getDistanceCM());
}

void test_UltrasonicSensor_getDistanceMM(void) {
    UltrasonicSensor sensor(&mockSonar);
    sensor.begin();
    mockSonar.setPingMicroseconds(580);
    TEST_ASSERT_EQUAL_UINT16(100, sensor.getDistanceMM());
}

void test_UltrasonicSensor_getDistanceInches(void) {
    UltrasonicSensor sensor(&mockSonar);
    sensor.begin();
    mockSonar.setPingMicroseconds(148); // 1 inch
    TEST_ASSERT_EQUAL_UINT16(1, sensor.getDistanceInches());
    mockSonar.setPingMicroseconds(296); // 2 inches
    TEST_ASSERT_EQUAL_UINT16(2, sensor.getDistanceInches());
    mockSonar.setPingMicroseconds(444); // 3 inches
    TEST_ASSERT_EQUAL_UINT16(3, sensor.getDistanceInches());
}

void test_UltrasonicSensor_no_echo(void) {
    UltrasonicSensor sensor(&mockSonar);
    sensor.begin();
    mockSonar.setPingMicroseconds(0); // no echo
    TEST_ASSERT_EQUAL_UINT16(0, sensor.ping());
    TEST_ASSERT_EQUAL_UINT16(0, sensor.getDistanceCM());
    TEST_ASSERT_EQUAL_UINT16(0, sensor.getDistanceMM());
    TEST_ASSERT_EQUAL_UINT16(0, sensor.getDistanceInches());
}

void test_UltrasonicSensor_max_distance(void) {
    UltrasonicSensor sensor(&mockSonar);
    sensor.begin();
    mockSonar.setPingMicroseconds(29000); // 500 cm (max)
    TEST_ASSERT_EQUAL_UINT16(500, sensor.getDistanceCM());
}

// === ObstacleDetector Unit Tests ===

void test_ObstacleDetector_initial_state(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();
    TEST_ASSERT_EQUAL_UINT16(0, detector.getDistanceMM());
    TEST_ASSERT_FALSE(detector.isObstacleDetected());
}

void test_ObstacleDetector_no_obstacle(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();

    // 800mm => 80cm => 4640 µs
    mockSonar.setPingMicroseconds(4640);
    for (int i = 0; i < 5; i++) {
        detector.update();
    }
    uint16_t dist = detector.getDistanceMM();
    TEST_ASSERT_TRUE(dist > 700 && dist < 900);
    TEST_ASSERT_FALSE(detector.isObstacleDetected());
    TEST_ASSERT_FALSE(detector.isWarning());
}

void test_ObstacleDetector_warning_zone(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();

    // 450mm => 45cm => 2610 µs
    mockSonar.setPingMicroseconds(2610);
    for (int i = 0; i < 5; i++) {
        detector.update();
    }
    TEST_ASSERT_TRUE(detector.isWarning());
    TEST_ASSERT_FALSE(detector.isObstacleDetected());
}

void test_ObstacleDetector_danger_obstacle(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();

    // 200mm => 20cm => 1160 µs
    mockSonar.setPingMicroseconds(1160);
    for (int i = 0; i < 5; i++) {
        detector.update();
    }
    TEST_ASSERT_TRUE(detector.isObstacleDetected());
    TEST_ASSERT_FALSE(detector.isWarning());
}

void test_ObstacleDetector_hysteresis(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();

    // Start with obstacle at 200mm
    mockSonar.setPingMicroseconds(1160);
    for (int i = 0; i < 5; i++) {
        detector.update();
    }
    TEST_ASSERT_TRUE(detector.isObstacleDetected());

    // Increase to 350mm (between danger 300 and clear 400)
    mockSonar.setPingMicroseconds(2030);
    detector.update();
    TEST_ASSERT_TRUE(detector.isObstacleDetected()); // still detected

    // Increase to 450mm (above clear threshold)
    mockSonar.setPingMicroseconds(2610);
    detector.update();
    TEST_ASSERT_FALSE(detector.isObstacleDetected());
}

void test_ObstacleDetector_filtering(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();

    mockSonar.setPingMicroseconds(1740); // 300mm
    detector.update();
    uint16_t first = detector.getDistanceMM();

    mockSonar.setPingMicroseconds(2030); // 350mm
    detector.update();
    uint16_t second = detector.getDistanceMM();

    // Second reading should be between 300 and 350 due to filter
    TEST_ASSERT_TRUE(second >= first && second <= 350);
}

void test_ObstacleDetector_no_echo_handling(void) {
    UltrasonicSensor sensor(&mockSonar);
    ObstacleDetector detector(sensor, 5, 500, 300, 400);
    detector.begin();

    // Simulate no echo
    mockSonar.setPingMicroseconds(0);
    for (int i = 0; i < 10; i++) {
        detector.update();
    }
    // Treated as far distance (9999)
    TEST_ASSERT_EQUAL_UINT16(9999, detector.getDistanceMM());
    TEST_ASSERT_FALSE(detector.isObstacleDetected());
    TEST_ASSERT_FALSE(detector.isWarning());
}

int runUnityTests(void) {
    UNITY_BEGIN();
    // UltrasonicSensor tests
    RUN_TEST(test_UltrasonicSensor_begin);
    RUN_TEST(test_UltrasonicSensor_getDistanceCM);
    RUN_TEST(test_UltrasonicSensor_getDistanceMM);
    RUN_TEST(test_UltrasonicSensor_getDistanceInches);
    RUN_TEST(test_UltrasonicSensor_no_echo);
    RUN_TEST(test_UltrasonicSensor_max_distance);
    // ObstacleDetector tests
    RUN_TEST(test_ObstacleDetector_initial_state);
    RUN_TEST(test_ObstacleDetector_no_obstacle);
    RUN_TEST(test_ObstacleDetector_warning_zone);
    RUN_TEST(test_ObstacleDetector_danger_obstacle);
    RUN_TEST(test_ObstacleDetector_hysteresis);
    RUN_TEST(test_ObstacleDetector_filtering);
    RUN_TEST(test_ObstacleDetector_no_echo_handling);
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

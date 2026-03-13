#include <Arduino.h>
#include "Motor.h"
#include "MotorDriver.h"
#include "UltrasonicSensor.h"
#include "ObstacleDetector.h"
#include "Robot.h"
#include "CommandParser.h"
#include "pin_config.h"

// Component instances
Motor leftMotor(
    LEFT_MOTOR_ENA_PIN,
    LEFT_MOTOR_IN1_PIN,
    LEFT_MOTOR_IN2_PIN,
    0,  // LEDC channel 0
    MOTOR_PWM_FREQ_HZ,
    MOTOR_PWM_RESOLUTION
);

Motor rightMotor(
    RIGHT_MOTOR_ENB_PIN,
    RIGHT_MOTOR_IN3_PIN,
    RIGHT_MOTOR_IN4_PIN,
    1,  // LEDC channel 1
    MOTOR_PWM_FREQ_HZ,
    MOTOR_PWM_RESOLUTION
);

MotorDriver driver(leftMotor, rightMotor);
UltrasonicSensor ultrasonic(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
ObstacleDetector detector(ultrasonic);
Robot robot(driver, detector, SAFETY_AVOID_DISTANCE_MM, 150);
CommandParser parser(robot, 150);  // default speed 150

// Timing
unsigned long lastDebugTime = 0;
const unsigned long DEBUG_INTERVAL_MS = 1000;  // Debug output every 1 second
const unsigned long AUTO_START_DELAY_MS = 5000; // Auto-start after 5 seconds

void setup() {
    Serial.begin(115200);
    delay(100);  // Wait for serial to stabilize
    Serial.println("\n=== ESP32 Robot Autonomous System ===");
    
    // Initialize all components
    driver.begin();
    ultrasonic.begin();
    detector.begin();
    robot.begin();
    
    Serial.println("System initialized. Robot is IDLE.");
    Serial.println("Auto-start in 5 seconds, or use serial commands:");
    Serial.println("  w/a/s/d - manual movement (requires IDLE)");
    Serial.println("  q/e     - pivot turn");
    Serial.println("  x       - emergency stop");
    Serial.println("  r       - reset from STOPPED");
    Serial.println("  ?       - print this help");
    Serial.println("Autonomous mode starts automatically after 5s if IDLE.");
    Serial.println("=======================================\n");
    
    lastDebugTime = millis();
}

bool autoStartTriggered = false;

void loop() {
    unsigned long now = millis();
    
    // Update robot state machine (call at ~50-100Hz)
    robot.update();
    
    // Process serial commands
    parser.update();
    
    // Auto-start after delay (for unattended operation)
    if (!autoStartTriggered && robot.getState() == Robot::State::IDLE && 
        (now - robot.getStateStartTime() >= AUTO_START_DELAY_MS)) {
        Serial.println("Auto-start: entering autonomous driving mode.");
        robot.startDriving();
        autoStartTriggered = true;
    }
    
    // Debug dashboard - output at 1Hz (plus on state change)
    if (now - lastDebugTime >= DEBUG_INTERVAL_MS) {
        Serial.println("\n--- Robot Status ---");
        Serial.print("State: ");
        Serial.print(robot.getStateString());
        Serial.print(" (");
        Serial.print(robot.isMoving() ? "MOVING" : "STOPPED");
        Serial.println(")");
        
        Serial.print("Distance: ");
        Serial.print(detector.getDistanceMM());
        Serial.println(" mm");
        
        Serial.print("Obstacle: ");
        Serial.println(detector.isObstacleDetected() ? "YES" : "NO");
        
        lastDebugTime = now;
    }
    
    // Small delay to prevent overwhelming the CPU (adjust as needed)
    delay(10);
}
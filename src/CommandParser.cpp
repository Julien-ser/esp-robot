#include "CommandParser.h"
#include "Arduino.h"

CommandParser::CommandParser(Robot& robot, uint8_t defaultSpeed)
    : _robot(robot)
    , _defaultSpeed(defaultSpeed)
{
}

bool CommandParser::available() {
    return Serial.available() > 0;
}

void CommandParser::processNextCommand() {
    if (!available()) {
        return;
    }

    char cmd = Serial.read();
    bool isMovement = false;
    bool accepted = false;

    switch (cmd) {
        case 'w':
        case 'W':
            isMovement = true;
            if (_robot.getState() == Robot::State::IDLE) {
                accepted = _robot.manualForward(_defaultSpeed);
            }
            break;

        case 's':
        case 'S':
            isMovement = true;
            if (_robot.getState() == Robot::State::IDLE) {
                accepted = _robot.manualBackward(_defaultSpeed);
            }
            break;

        case 'a':
        case 'A':
            isMovement = true;
            if (_robot.getState() == Robot::State::IDLE) {
                accepted = _robot.manualTurn(_defaultSpeed, -90); // left
            }
            break;

        case 'd':
        case 'D':
            isMovement = true;
            if (_robot.getState() == Robot::State::IDLE) {
                accepted = _robot.manualTurn(_defaultSpeed, 90); // right
            }
            break;

        case 'q':
        case 'Q':
            isMovement = true;
            if (_robot.getState() == Robot::State::IDLE) {
                accepted = _robot.manualPivot(_defaultSpeed, false); // CCW
            }
            break;

        case 'e':
        case 'E':
            isMovement = true;
            if (_robot.getState() == Robot::State::IDLE) {
                accepted = _robot.manualPivot(_defaultSpeed, true); // CW
            }
            break;

        case 'x':
        case 'X':
            // Emergency stop always accepted
            Serial.println("CMD: Emergency stop");
            _robot.emergencyStop();
            accepted = true;
            break;

        case 'r':
        case 'R':
            // Reset always accepted
            Serial.println("CMD: Reset");
            _robot.reset();
            accepted = true;
            break;

        case '?':
        case 'h':
        case 'H':
            Serial.println("\n=== Robot Remote Control ===");
            Serial.println("w - drive forward");
            Serial.println("s - drive backward");
            Serial.println("a - turn left");
            Serial.println("d - turn right");
            Serial.println("q - pivot left (CCW)");
            Serial.println("e - pivot right (CW)");
            Serial.println("x - emergency stop");
            Serial.println("r - reset from STOPPED");
            Serial.println("? - this help");
            Serial.println("Note: Movement commands require robot in IDLE state.");
            Serial.println("Auto-start enabled after 5s if idle.\n");
            accepted = true;
            break;

        default:
            Serial.print("Unknown command: ");
            Serial.println(cmd);
            break;
    }

    // Feedback for movement commands
    if (isMovement && !accepted) {
        Serial.println("CMD rejected: Robot not in IDLE state. Stop first or wait for auto-start.");
    }
}

void CommandParser::update() {
    while (available()) {
        processNextCommand();
    }
}

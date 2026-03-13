#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "Robot.h"

/**
 * @brief Serial command parser for robot remote control
 *
 * Parses single-character commands from Serial and dispatches to Robot.
 * Implements safety interlock: movement commands only accepted when robot in IDLE state.
 */
class CommandParser {
private:
    Robot& _robot;
    uint8_t _defaultSpeed;     // Default speed for manual commands

public:
    /**
     * @brief Construct a new Command Parser
     *
     * @param robot Reference to Robot instance
     * @param defaultSpeed Default speed for movement commands (0-255)
     */
    CommandParser(Robot& robot, uint8_t defaultSpeed = 150);

    /**
     * @brief Check if any serial data is available
     *
     * @return true if commands are pending
     */
    bool available();

    /**
     * @brief Read and process the next command
     *
     * Blocks until a command is available if available() is true.
     * Supported commands:
     *   w - drive forward
     *   s - drive backward
     *   a - turn left (differential turn)
     *   d - turn right (differential turn)
     *   q - pivot turn counter-clockwise
     *   e - pivot turn clockwise
     *   x - emergency stop
     *   r - reset from STOPPED to IDLE
     *   ? - print help
     */
    void processNextCommand();

    /**
     * @brief Process all pending commands (non-blocking convenience)
     *
     * Call this in loop() to handle all queued commands.
     */
    void update();
};

#endif // COMMAND_PARSER_H

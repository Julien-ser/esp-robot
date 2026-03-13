#include <Arduino.h>

// Blink LED pin (built-in LED on most ESP32 boards)
const int LED_PIN = 2;  // GPIO2 is typically the built-in LED

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("ESP32 Robot Project - Blink Test");
    Serial.println("Build environment configured successfully!");
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}
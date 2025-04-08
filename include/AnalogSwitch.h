#pragma once

#include <Arduino.h>

class AnalogSwitch {
private:
    uint8_t pinS0; // GPIO pin for S0
    uint8_t pinS1; // GPIO pin for S1
    uint8_t pinS2; // GPIO pin for S2

public:
    // Constructor
    AnalogSwitch(uint8_t s0, uint8_t s1, uint8_t s2);

    // Initialize the pins
    void begin();

    // Switch to a specific channel (0-7)
    void switchTo(uint8_t channel);
};
#include "AnalogSwitch.h"

AnalogSwitch::AnalogSwitch(uint8_t s0, uint8_t s1)
    : pinS0(s0), pinS1(s1) {}

void AnalogSwitch::begin() {
    // Set the pins as outputs
    pinMode(pinS0, OUTPUT);
    pinMode(pinS1, OUTPUT);

    // Set all pins to LOW initially
    digitalWrite(pinS0, LOW);
    digitalWrite(pinS1, LOW);
}

void AnalogSwitch::switchTo(uint8_t channel) {
    // Ensure the channel is within the valid range (0-7)
    if (channel > 3) return;

    // Set the S0, S1, and S2 pins based on the channel
    digitalWrite(pinS0, channel & 0x01); // Least significant bit
    digitalWrite(pinS1, (channel >> 1) & 0x01); // Second bit
}
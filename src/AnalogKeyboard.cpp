#include "AnalogKeyboard.h"

AnalogKeyboard::AnalogKeyboard(uint8_t analogPin)
    : pin(analogPin) {
    // The voltage divider is set up as follows:
    // Vcc - OK - 1KOhm - UP - 1KOhm - DOWN -1KOhm - LEFT - 1KOhm - RIGHT - 1KOhm- gnd
    // A0 has a 10KOhm pull-down resistor to ground

    // Set thresholds based on the calculated ADC values with a 20% margin
    thresholds[0] = 1023 * 0.8; // OK
    thresholds[1] = 818 * 0.8;  // UP
    thresholds[2] = 614 * 0.8;  // DOWN
    thresholds[3] = 410 * 0.8;  // LEFT
    thresholds[4] = 205 * 0.8;  // RIGHT
}

uint16_t AnalogKeyboard::getPressedKeys() {
    uint16_t analogValue = analogRead(pin);
    uint16_t pressedKeys = 0;

    // Check each threshold and set the corresponding key bit
    if (analogValue > thresholds[0]) pressedKeys |= KEY_OK;
    else if (analogValue > thresholds[1]) pressedKeys |= KEY_UP;
    else if (analogValue > thresholds[2]) pressedKeys |= KEY_DOWN;
    else if (analogValue > thresholds[3]) pressedKeys |= KEY_LEFT;
    else if (analogValue > thresholds[4]) pressedKeys |= KEY_RIGHT;

    return pressedKeys;
}
#pragma once

#include <IKeyboard.h>

class AnalogKeyboard : public IKeyboard {
private:
    uint8_t pin; // Analog pin to read from
    uint16_t thresholds[5]; // Thresholds for each key (OK, UP, DOWN, LEFT, RIGHT)

public:
    AnalogKeyboard(uint8_t analogPin);

    uint16_t getPressedKeys() override;
};
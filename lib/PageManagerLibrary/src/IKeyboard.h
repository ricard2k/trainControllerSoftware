#pragma once

#include <Arduino.h>

enum Key {
    KEY_OK = 1,       // Weight: 1
    KEY_UP = 2,       // Weight: 2
    KEY_DOWN = 4,     // Weight: 4
    KEY_LEFT = 8,     // Weight: 8
    KEY_RIGHT = 16    // Weight: 16
};

class IKeyboard {
public:
    virtual ~IKeyboard() = default;

    // Calculate the weighted sum of all pressed keys
    virtual uint16_t getPressedKeys() = 0;
};
#pragma once

#include "IKeyboard.h"

class DefaultKeyboard : public IKeyboard {
private:
    uint8_t pinOK, pinUp, pinDown, pinLeft, pinRight;
public:
    DefaultKeyboard(uint8_t ok, uint8_t up, uint8_t down, uint8_t left, uint8_t right);

    void begin();

    uint16_t getPressedKeys() override ;
};
#include "IKeyboard.h"

class DefaultKeyboard : public IKeyboard {
private:
    uint8_t pinOK, pinUp, pinDown, pinLeft, pinRight;

public:
    DefaultKeyboard(uint8_t ok, uint8_t up, uint8_t down, uint8_t left, uint8_t right)
        : pinOK(ok), pinUp(up), pinDown(down), pinLeft(left), pinRight(right) {}

    void begin() {
        pinMode(pinOK, INPUT_PULLUP);
        pinMode(pinUp, INPUT_PULLUP);
        pinMode(pinDown, INPUT_PULLUP);
        pinMode(pinLeft, INPUT_PULLUP);
        pinMode(pinRight, INPUT_PULLUP);
    }

    uint16_t getPressedKeys() override {
        uint16_t sum = 0;
        if (digitalRead(pinOK) == LOW) sum |= KEY_OK;
        if (digitalRead(pinUp) == LOW) sum |= KEY_UP;
        if (digitalRead(pinDown) == LOW) sum |= KEY_DOWN;
        if (digitalRead(pinLeft) == LOW) sum |= KEY_LEFT;
        if (digitalRead(pinRight) == LOW) sum |= KEY_RIGHT;
        return sum;
    }
};
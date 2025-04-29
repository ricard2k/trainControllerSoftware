#include "MatrixKeyboard.h"

MatrixKeyboard::MatrixKeyboard(const std::vector<uint8_t>& rows, const std::vector<uint8_t>& cols)
    : rowPins(rows), colPins(cols) {
    // Initialize the key map (example for a 5x5 matrix)
    keyMap[0][0] = KEY_OK;
    keyMap[0][1] = KEY_UP;
    keyMap[0][2] = KEY_DOWN;
    keyMap[0][3] = KEY_LEFT;
    keyMap[0][4] = KEY_RIGHT;

    // Initialize the pins
    initializePins();
}

void MatrixKeyboard::initializePins() {
    // Set row pins as outputs and initialize them to HIGH
    for (uint8_t row : rowPins) {
        pinMode(row, OUTPUT);
        digitalWrite(row, HIGH);
    }

    // Set column pins as inputs with pull-ups
    for (uint8_t col : colPins) {
        pinMode(col, INPUT_PULLUP);
    }
}

uint16_t MatrixKeyboard::getPressedKeys() {
    return scanMatrix();
}

uint16_t MatrixKeyboard::scanMatrix() {
    uint16_t pressedKeys = 0;

    // Iterate through each row
    for (size_t row = 0; row < rowPins.size(); ++row) {
        // Set the current row to LOW
        digitalWrite(rowPins[row], LOW);

        // Check each column for a LOW signal (indicating a pressed key)
        for (size_t col = 0; col < colPins.size(); ++col) {
            if (digitalRead(colPins[col]) == LOW) {
                pressedKeys |= keyMap[row][col]; // Add the key to the pressed keys
            }
        }

        // Set the current row back to HIGH
        digitalWrite(rowPins[row], HIGH);
    }

    return pressedKeys;
}
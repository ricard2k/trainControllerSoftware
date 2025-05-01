#include "MatrixKeyboard.h"
#include "ExtendedKeys.h" 

MatrixKeyboard::MatrixKeyboard(const uint8_t rows[2], const uint8_t cols[3], uint8_t powerPin) 
    : softPowerPin(powerPin) {
    
    // Store row and column pins
    rowPins[0] = rows[0];
    rowPins[1] = rows[1];
    
    colPins[0] = cols[0];
    colPins[1] = cols[1];
    colPins[2] = cols[2];
    
    // Define key mapping for each position in the matrix
    // Row 0
    keyMap[0][0] = KEY_UP;     // Top-left
    keyMap[0][1] = KEY_DOWN;     // Top-middle
    keyMap[0][2] = KEY_LEFT;  // Top-right
    
    // Row 1
    keyMap[1][0] = KEY_RIGHT;   // Bottom-left
    keyMap[1][1] = ExtendedKeys::KEY_TIGHT_BRAKE;   // Bottom-middle 
    keyMap[1][2] = ExtendedKeys::KEY_RELEASE_BRAKE;  // Bottom-right 
    
    // Initialize all pins
    initializePins();
}

void MatrixKeyboard::initializePins() {
    // Set row pins as outputs with HIGH state (inactive)
    for (int i = 0; i < 2; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    
    // Set column pins as inputs with pull-up resistors
    for (int i = 0; i < 3; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }
    
    // Initialize soft power button as input with pull-up
    pinMode(softPowerPin, INPUT_PULLUP);
}

uint16_t MatrixKeyboard::getPressedKeys() {
    uint16_t pressedKeys = 0;
    
    // Scan each row of the matrix
    for (int row = 0; row < 2; row++) {
        scanRow(row, pressedKeys);
    }
    
    // Check soft power button (when configured as input)
    if (digitalRead(softPowerPin) == LOW) {
        pressedKeys |= KEY_OK;  // Soft power button represents OK when pressed
    }
    
    return pressedKeys;
}

void MatrixKeyboard::scanRow(uint8_t row, uint16_t& pressedKeys) {
    // Set the current row LOW (active)
    digitalWrite(rowPins[row], LOW);
    
    // Small delay to stabilize
    delayMicroseconds(10);
    
    // Read all columns
    for (int col = 0; col < 3; col++) {
        // If column reads LOW, the button at this row/column is pressed
        if (digitalRead(colPins[col]) == LOW) {
            pressedKeys |= keyMap[row][col];
        }
    }
    
    // Set the row back to HIGH (inactive)
    digitalWrite(rowPins[row], HIGH);
}

void MatrixKeyboard::powerOff() {
    // Configure as output
    pinMode(softPowerPin, OUTPUT);
    
    // Send LOW signal to trigger power off
    digitalWrite(softPowerPin, LOW);
    
    // Wait a moment to ensure signal is processed
    delay(100);
}